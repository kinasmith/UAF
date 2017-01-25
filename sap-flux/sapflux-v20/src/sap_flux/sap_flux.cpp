#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "Sleepy.h"
#include "RFM69_ATC.h"
#include "SPIFlash_Marzogh.h"
#include <EEPROM.h>
#include "Nanoshield_Termopar.h"

#define NODEID 66
#define GATEWAYID 0
#define FREQUENCY RF69_433MHZ //frequency of radio
#define ATC_RSSI -70 //ideal Signal Strength of trasmission
#define ACK_WAIT_TIME 100 // # of ms to wait for an ack
#define ACK_RETRIES 10 // # of attempts before giving up
#define SERIAL_BAUD 115200 // Serial comms rate
#define FLASH_CAPACITY 524288
#define HEATER_ON_TIME 6 //in seconds

#define LED 9
#define LED2 A0
#define N_SEL_1 A1
#define N_SEL_2 A2
#define N_SEL_4 A3
#define BAT_V A7
#define BAT_EN 3
#define HEATER_EN 4
#define TC1_CS 7
#define TC2_CS 6
#define TC3_CS 5
#define FLASH_CS 8
#define RFM_CS 10

#define SERIAL_EN //Comment this out to remove Serial comms and save a few kb's of space

#ifdef SERIAL_EN
#define DEBUG(input)   {Serial.print(input); delay(1);}
#define DEBUGln(input) {Serial.println(input); delay(1);}
#define DEBUGFlush() { Serial.flush(); }
#else
#define DEBUG(input);
#define DEBUGln(input);
#define DEBUGFlush();
#endif

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

/*==============|| FUNCTIONS ||==============*/
bool getTime();
void Blink(uint8_t);
uint8_t setAddress();
float getBatteryVoltage(uint8_t pin, uint8_t en);
bool saveEEPROMTime(uint32_t t);
uint32_t getEEPROMTime();
void sendMeasurement();
bool transmitDataPackage();
/*==============|| RFM69 ||==============*/
RFM69_ATC radio; //init radio
uint8_t attempt_cnt = 0;
uint8_t NETWORKID;
bool HANDSHAKE_SENT;
/*==============|| MEMORY ||==============*/
SPIFlash_Marzogh flash(8);
uint32_t FLASH_ADDR = 0;
uint16_t EEPROM_ADDR = 0;
/*==============|| THERMOCOUPLE ||==============*/
Nanoshield_Termopar tc1(TC1_CS, TC_TYPE_T, TC_AVG_4_SAMPLES);
Nanoshield_Termopar tc2(TC2_CS, TC_TYPE_T, TC_AVG_4_SAMPLES);
Nanoshield_Termopar tc3(TC3_CS, TC_TYPE_T, TC_AVG_4_SAMPLES);
/*==============|| UTIL ||==============*/
bool LED_STATE;
uint16_t count = 0;
uint8_t sentMeasurement = 0;
/*==============|| INTERVAL ||==============*/
const uint8_t REC_MIN = 15; //record interval in minutes
const uint16_t REC_MS = 60000;
const uint32_t REC_INTERVAL = REC_MIN * (REC_MS/1000); //record interval in seconds
// const uint32_t REC_INTERVAL = (REC_MS*REC_MIN)/1000UL; //record interval in seconds
/*==============|| DATA ||==============*/
//Data structure for transmitting the Timestamp from datalogger to sensor (4 bytes)
struct TimeStamp {
	uint32_t timestamp;
};
TimeStamp theTimeStamp; //creates global instantiation of this

//Data Structure for transmitting data packets to datalogger (16 bytes)
struct Payload {
	uint32_t timestamp;
	uint16_t count;
	double tc1;
	double tc2;
	double tc3;
	double internal;
	double bat_v;
};
Payload thePayload;

struct Measurement {
	double tc1;
	double tc2;
	double tc3;
	double internal;
};

uint8_t measurementCount = 0;

void setup()
{
	#ifdef SERIAL_EN
		Serial.begin(SERIAL_BAUD);
	#endif
	randomSeed(analogRead(A4)); //set random seed
	NETWORKID = setAddress();
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower();
	radio.encrypt(null);
	radio.enableAutoPower(ATC_RSSI); //Test to see if this is actually working at some point
	DEBUG("--Transmitting on Network: "); DEBUG(NETWORKID); DEBUG(", as Node: "); DEBUGln(NODEID);
	pinMode(HEATER_EN, OUTPUT);
	pinMode(LED, OUTPUT);
	// Ping the datalogger. If it's alive, it will return the current time. If not, wait and try again.
	digitalWrite(LED, HIGH); //write LED high to signal attempting connection
	while(!getTime()) { //this saves time to the struct which holds the time globally
		radio.sleep();
		DEBUGFlush();
		delay(10000);
	}
	digitalWrite(LED, LOW); //write low to signal success
	DEBUG("--Time is: "); DEBUG(theTimeStamp.timestamp); DEBUGln("--");

  tc1.begin();
	tc2.begin();
	tc3.begin();
	DEBUGln("--Thermocouples are engaged");

	DEBUG("--Flash Mem: ");
	flash.begin();
	uint16_t _name = flash.getChipName();
	uint32_t capacity = flash.getCapacity();
	DEBUG("W25X"); DEBUG(_name); DEBUG("**  ");
	DEBUG("capacity: "); DEBUG(capacity); DEBUGln(" bytes");
	DEBUGln("Erasing Chip!");
	while(!flash.eraseChip()) {
	}
	DEBUG("Cooling Time is "); DEBUG(REC_INTERVAL); DEBUGln("s");
}

void loop()
{
	if(measurementCount < 240) { //240 seconds in 4 minutes
	// if(measurementCount < 15) { //240 seconds in 4 minutes
		Measurement thisMeasurement;
		tc1.read(); tc2.read(); tc3.read();
		thisMeasurement.tc1 = tc1.getExternal();
		thisMeasurement.tc2 = tc2.getExternal();
		thisMeasurement.tc3 = tc3.getExternal();
		thisMeasurement.internal = tc1.getInternal();
		if(flash.writeAnything(FLASH_ADDR, thisMeasurement)) {
			DEBUG("data - ");
			DEBUG(thisMeasurement.tc1); DEBUG(", ");
			DEBUG(thisMeasurement.tc2); DEBUG(", ");
			DEBUG(thisMeasurement.tc3); DEBUG(", ");
			DEBUG(thisMeasurement.internal); DEBUG(", ");
			DEBUG("at Address "); DEBUGln(FLASH_ADDR);
			FLASH_ADDR += sizeof(thisMeasurement);
		}
		if(measurementCount <= HEATER_ON_TIME) digitalWrite(HEATER_EN, HIGH); //On cycle start, turn on heater
		else digitalWrite(HEATER_EN, LOW); //turn off after 6 seconds
		DEBUGFlush();
		Sleepy::loseSomeTime(1000); // wait one seconds
		measurementCount++; //increment counter
	} else {
		DEBUG("sleep - sleeping for "); DEBUG(REC_INTERVAL); DEBUG(" seconds"); DEBUGln();
		DEBUGFlush();
		radio.sleep();
		count++;
		for(int i = 0; i < REC_MIN; i++)
			Sleepy::loseSomeTime(REC_MS);
		//===========|| MCU WAKES UP HERE
		measurementCount = 0; //reset measurement counter
		thePayload.count += 1;
		thePayload.bat_v = random(50, 1000);
		//get current Time, check Datalogger Status
		if(getTime()) {
			DEBUG("time - Is: "); DEBUGln(theTimeStamp.timestamp);
			EEPROM.put(EEPROM_ADDR, theTimeStamp);
			// DEBUG("flash - byte 0 == "); DEBUGln(flash.readByte(0));
			if(flash.readByte(0) < 255) {
				DEBUGln("=== Sending from Flash ===");
				sendMeasurement();
			}
		} else {
			DEBUG("time - No Time, incrementing saved time from ");
			DEBUG(getEEPROMTime()); DEBUG(" to ");
			uint32_t new_time = getEEPROMTime() + REC_INTERVAL;
			saveEEPROMTime(new_time);
			DEBUGln(getEEPROMTime());
		}
	}
}

/**
 * [sendMeasurement description]
 */
void sendMeasurement()
{
	// DEBUGln("flash - Checking for Stored Data ");
	Measurement storedMeasurement;
	FLASH_ADDR = 0; //Go to address of first chunk of data.
	// Check if there is Data
	while(flash.readByte(FLASH_ADDR) < 255) {
		// DEBUG("flash - Value of First Byte: "); DEBUGln(flash.readByte(FLASH_ADDR));
		// DEBUG("flash - store time: "); DEBUGln(getEEPROMTime());
		//Read in saved data to the Data struct
		if(flash.readAnything(FLASH_ADDR, storedMeasurement)) {
			// DEBUG("flash - data at addr "); DEBUGln(FLASH_ADDR); DEBUG(" ");
			// DEBUG(storedMeasurement.tc1); DEBUG(","); DEBUG(storedMeasurement.tc2); DEBUG(","); DEBUG(storedMeasurement.tc3); DEBUGln();
			//Save data into the Payload struct
			thePayload.tc1 = storedMeasurement.tc1;
			thePayload.tc2 = storedMeasurement.tc2;
			thePayload.tc3 = storedMeasurement.tc3;
			thePayload.internal = storedMeasurement.internal;
		}
		thePayload.timestamp = theTimeStamp.timestamp += 1000;

		if(transmitDataPackage()) {
			DEBUG("send - ");
			DEBUG(thePayload.timestamp); DEBUG(",");
			DEBUG(thePayload.count); DEBUG(",");
			DEBUG(thePayload.tc1); DEBUG(",");
			DEBUG(thePayload.tc2); DEBUG(",");
			DEBUG(thePayload.tc3); DEBUG(",");
			DEBUG(thePayload.internal); DEBUG(",");
			DEBUG(thePayload.bat_v); DEBUG(",");
			DEBUG(" at Address "); DEBUGln(FLASH_ADDR);
			FLASH_ADDR += sizeof(storedMeasurement); //increment to next chunk of data
		} else {
			// FLASH_ADDR += sizeof(storedMeasurement); //WTF is this?...still ignoring
		}
	}
	DEBUGln("flash - Setting address back to 0");
	FLASH_ADDR = 0; //Set Index to beginning of DATA for Writing Events (or...other location for load balancing?)
	// EEPROM_ADDR = random(0, 1023);
	DEBUGln("flash - Erasing Sector 0");
	flash.eraseSector(0);
}

/**
 * [transmitDataPackage description]
 * @return [description]
 */
bool transmitDataPackage()
{
	digitalWrite(LED, HIGH);
	uint8_t success = 0;
	while(!success) {
		if(radio.sendWithRetry(GATEWAYID, (const void*)(&thePayload), sizeof(thePayload)), ACK_RETRIES, ACK_WAIT_TIME) {
			// DEBUG("snd - "); DEBUG(sizeof(thePayload)); DEBUGln(" bytes  ");
			digitalWrite(LED, LOW);
			success = 1;
		} else {
			DEBUG(" failed . . . no ack ");
			DEBUG(attempt_cnt);
			Blink(50);
			Blink(50);
		}
	}
	if(success) return true;
	else return false;
}

/**
 * [getTime description]
 * @return [description]
 */
bool getTime()
{
	LED_STATE = true;
	digitalWrite(LED, LED_STATE);
	//Get the current timestamp from the datalogger
	bool TIME_RECIEVED = false;
	if(!HANDSHAKE_SENT) { //Send request for time to the Datalogger
		DEBUG("time - ");
		if (radio.sendWithRetry(GATEWAYID, "t", 1)) {
			DEBUG("snd . . ");
			HANDSHAKE_SENT = true;
		}
		else {
			DEBUGln("failed . . . no ack");
			return false; //if there is no response, returns false and exits function
		}
	}
	while(!TIME_RECIEVED && HANDSHAKE_SENT) { //Wait for the time to be returned
		if (radio.receiveDone()) {
			if (radio.DATALEN == sizeof(theTimeStamp)) { //check to make sure it's the right size
				theTimeStamp = *(TimeStamp*)radio.DATA; //save data
				DEBUG(" rcv - "); DEBUG('['); DEBUG(radio.SENDERID); DEBUG("] ");
				DEBUG(theTimeStamp.timestamp);
				DEBUG(" [RX_RSSI:"); DEBUG(radio.RSSI); DEBUG("]");
				DEBUGln();
				TIME_RECIEVED = true;
				LED_STATE = false;
				digitalWrite(LED, LED_STATE);
			}
			if (radio.ACKRequested()) radio.sendACK();
		}
	}
	HANDSHAKE_SENT = false;
	return true;
}


/**
 * Sets Network ID of Board Depending on Closed Solder Jumpers
 * @return The Address
 */
uint8_t setAddress()
{
	uint8_t addr01, addr02, addr03;
	pinMode(N_SEL_1, INPUT_PULLUP);
	pinMode(N_SEL_2, INPUT_PULLUP);
	pinMode(N_SEL_4, INPUT_PULLUP);
	addr01 = !digitalRead(N_SEL_1);
	addr02 = !digitalRead(N_SEL_2) * 2;
	addr03 = !digitalRead(N_SEL_4) * 4;
	pinMode(N_SEL_1, OUTPUT);
	pinMode(N_SEL_2, OUTPUT);
	pinMode(N_SEL_4, OUTPUT);
	digitalWrite(N_SEL_1, HIGH);
	digitalWrite(N_SEL_2, HIGH);
	digitalWrite(N_SEL_4, HIGH);
	uint8_t addr = addr01 | addr02 | addr03;
	return addr += 100;
}

bool saveEEPROMTime(uint32_t t) {
	if(EEPROM.put(EEPROM_ADDR, t)) return 1;
	else return 0;
}

uint32_t getEEPROMTime(){
	TimeStamp storedTime;
	storedTime = EEPROM.get(EEPROM_ADDR, storedTime);
	return storedTime.timestamp;
}

/**
 * [Blink description]
 * @param t [description]
 */
void Blink(uint8_t t)
{
	LED_STATE = !LED_STATE;
	digitalWrite(LED, LED_STATE);
	delay(t);
	LED_STATE = !LED_STATE;
	digitalWrite(LED, LED_STATE);
	delay(t);
}

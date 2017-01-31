#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "Sleepy.h"
#include "RFM69_ATC.h"
#include "SPIFlash_Marzogh.h"
#include <EEPROM.h>
#include "MAX31856.h"

#define NODEID 63
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
/*==============|| MEMORY ||==============*/
SPIFlash_Marzogh flash(8);
uint32_t FLASH_ADDR = 0;
uint16_t EEPROM_ADDR = 0;
/*==============|| THERMOCOUPLE ||==============*/
MAX31856 tc1(TC1_CS, T_TYPE, CUTOFF_60HZ, AVG_SEL_16SAMP, CMODE_OFF, ONESHOT_ON); //one shot mode
MAX31856 tc2(TC2_CS, T_TYPE, CUTOFF_60HZ, AVG_SEL_16SAMP, CMODE_OFF, ONESHOT_ON); //one shot mode
MAX31856 tc3(TC3_CS, T_TYPE, CUTOFF_60HZ, AVG_SEL_16SAMP, CMODE_OFF, ONESHOT_ON); //one shot mode
/*==============|| UTIL ||==============*/
bool LED_STATE;
uint16_t count = 0;
uint8_t sentMeasurement = 0;
/*==============|| TIMING ||==============*/
const uint16_t SLEEP_INTERVAL = 30; //sleep time in minutes (Cool Down)
const uint16_t SLEEP_MS = 60000; //one minute in milliseconds
const uint32_t SLEEP_SECONDS = SLEEP_INTERVAL * (SLEEP_MS/1000); //Sleep interval in seconds
const uint16_t REC_INTERVAL = 1000; //record interval during measurement event in mS
const uint16_t REC_DURATION = 4 * 60; //how long the measurement is in seconds

// const uint16_t SLEEP_INTERVAL = 1; //sleep time in minutes (Cool Down)
// const uint16_t SLEEP_MS = 60000; //one minute in milliseconds
// const uint32_t SLEEP_SECONDS = SLEEP_INTERVAL * (SLEEP_MS/1000); //Sleep interval in seconds
// const uint16_t REC_INTERVAL = 1000; //record interval during measurement event in mS
// const uint16_t REC_DURATION = 25; //how long the measurement is in seconds

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
	uint16_t count;
	double tc1;
	double tc2;
	double tc3;
	double internal;
};

uint8_t measurementNum = 0;

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
	DEBUG("-- Network Address: "); DEBUG(NETWORKID); DEBUG("."); DEBUGln(NODEID);
	pinMode(HEATER_EN, OUTPUT);
	pinMode(LED, OUTPUT);
	pinMode(LED2, OUTPUT);
	// Ping the datalogger. If it's alive, it will return the current time. If not, wait and try again.
	digitalWrite(LED, HIGH); //write LED high to signal attempting connection
	while(!getTime()) { //this saves time to the struct which holds the time globally
		radio.sleep();
		DEBUGFlush();
		delay(10000);
	}
	if (radio.sendWithRetry(GATEWAYID, "r", 1)) {
		DEBUG("snd >");
	}
	else DEBUGln("failed . . . no ack");

	digitalWrite(LED, LOW); //write low to signal success
	DEBUG("-- Time is: "); DEBUG(theTimeStamp.timestamp); DEBUGln("--");
	saveEEPROMTime(theTimeStamp.timestamp); //Save that time to EEPROM

  tc1.begin();
	tc2.begin();
	tc3.begin();
	DEBUGln("-- Thermocouples are engaged");

	DEBUG("-- Flash Mem: ");

	DEBUG("flash - powering up");
	if(flash.powerUp()) {
		DEBUGln(". . . OK!");
	} else DEBUGln(". . . FAILED!");
	digitalWrite(LED2, HIGH); //turn on LED for Flash init
	flash.begin();
	uint16_t _name = flash.getChipName();
	uint32_t capacity = flash.getCapacity();
	DEBUG("W25X"); DEBUG(_name); DEBUG("**  ");
	DEBUG("capacity: "); DEBUG(capacity); DEBUGln(" bytes");
	DEBUGln("Erasing Chip!");
	while(!flash.eraseChip()) {
	}
	digitalWrite(LED2, LOW); //set it low for success
	DEBUG("Cooling Time is "); DEBUG(float(SLEEP_SECONDS/60.0)); DEBUGln("m");
	DEBUGln("==========================");
}

void loop()
{
	//Do this for the measurement duration time
	if(measurementNum < REC_DURATION) {
		Measurement thisMeasurement;
		tc1.prime(); tc2.prime(); tc3.prime();
		tc1.read(); tc2.read(); tc3.read();
		thisMeasurement.tc1 = tc1.getExternal();
		thisMeasurement.tc2 = tc2.getExternal();
		thisMeasurement.tc3 = tc3.getExternal();
		thisMeasurement.internal = tc1.getInternal();
		thisMeasurement.count = count;
		digitalWrite(LED2, HIGH); //signal start of Flash WRite
		delay(50);
		if(flash.writeAnything(FLASH_ADDR, thisMeasurement)) {
			DEBUG("flash - ");
			DEBUG(thisMeasurement.tc1); DEBUG(", ");
			DEBUG(thisMeasurement.tc2); DEBUG(", ");
			DEBUG(thisMeasurement.tc3); DEBUG(", ");
			DEBUG(thisMeasurement.internal); DEBUG(", ");
			DEBUG(thisMeasurement.count); DEBUG(", ");
			DEBUG("at Address "); DEBUGln(FLASH_ADDR);
			FLASH_ADDR += sizeof(thisMeasurement);
			digitalWrite(LED2, LOW); //turn off LED on successful Write
		}
		if(measurementNum <= HEATER_ON_TIME) {
			digitalWrite(HEATER_EN, HIGH); //On cycle start, turn on heater
		} else {
			digitalWrite(HEATER_EN, LOW); //turn off after 6 seconds
		}

		DEBUG("flash - powering down");
		if(flash.powerDown()) {
			DEBUGln(". . . OK!");
		} else DEBUGln(". . . FAILED!");

		DEBUGFlush();
		Sleepy::loseSomeTime(REC_INTERVAL); // wait one seconds
		//===========|| MCU WAKES UP HERE
		measurementNum++; //increment counter

		DEBUG("flash - powering up");
		if(flash.powerUp()) {
			DEBUGln(". . . OK!");
		} else DEBUGln(". . . FAILED!");
	//When that time is up, go to sleep for a while.
	} else {

		DEBUG("flash - powering down");
		if(flash.powerDown()) {
			DEBUGln(". . . OK!");
		} else DEBUGln(". . . FAILED!");

		DEBUG("sleep - sleeping for "); DEBUG(SLEEP_SECONDS); DEBUG(" seconds"); DEBUGln();
		DEBUGFlush();
		radio.sleep();
		for(uint8_t i = 0; i < SLEEP_INTERVAL; i++)
			Sleepy::loseSomeTime(SLEEP_MS);
		//===========|| MCU WAKES UP HERE

		DEBUG("flash - powering up");
		if(flash.powerUp()) {
			DEBUGln(". . . OK!");
		} else DEBUGln(". . . FAILED!");
		count++;
		measurementNum = 0; //reset measurement counter
		//NOTE: Circuit isn't working. Need to write fuction to do this.
		thePayload.bat_v = 0;

		//get current Time & check Datalogger Status
		if(getTime()) {
			if(flash.readByte(0) < 255) { //Make sure there is data @ bit 0
				DEBUGln("=== Sending from Flash ===");
				sendMeasurement();
			}
			if (radio.sendWithRetry(GATEWAYID, "r", 1)) {
				DEBUGln("resume normal listening");
			}
		} else {
			DEBUG("time - No Time,");
		}
		if(getTime()) {
			DEBUG("time - is: "); DEBUGln(theTimeStamp.timestamp);
			saveEEPROMTime(theTimeStamp.timestamp);
			if (radio.sendWithRetry(GATEWAYID, "r", 1))
				DEBUGln(" unlatch ");
		}
	}
}

/**
 * [sendMeasurement description]
 */
void sendMeasurement()
{
	Measurement storedMeasurement;
	Measurement firstMeasurement;
	uint8_t first_count;
	if(flash.readAnything(0, firstMeasurement)){
		first_count = firstMeasurement.count;
	}
	uint8_t num_of_stored_measurements = count - first_count;
	uint8_t local_m_temp = 0;
	DEBUG("store - Number of Stored Measurements:") DEBUGln(num_of_stored_measurements);

	thePayload.timestamp = getEEPROMTime(); //retreive saved time
	DEBUG("time -saved time is: "); DEBUGln(thePayload.timestamp);
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
			thePayload.count = storedMeasurement.count;
		}

		uint8_t local_measurement_num = num_of_stored_measurements - (count - storedMeasurement.count);
		if(local_measurement_num != local_m_temp && local_measurement_num != 0) {
			thePayload.timestamp += SLEEP_SECONDS;
			local_m_temp = local_measurement_num;
			DEBUGln();
		} else thePayload.timestamp += REC_INTERVAL/1000;

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
			Blink(50);
			Blink(50);
			Sleepy::loseSomeTime(30000);//sleep for 30sec and try again
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
	bool HANDSHAKE_SENT = false;
	bool TIME_RECIEVED = false;
	digitalWrite(LED, HIGH);
	//Get the current timestamp from the datalogger

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
				DEBUG(theTimeStamp.timestamp); DEBUG(" [RX_RSSI:"); DEBUG(radio.RSSI); DEBUG("]"); DEBUGln();
				TIME_RECIEVED = true;
				digitalWrite(LED, LOW);
			}
			if (radio.ACKRequested()) radio.sendACK();
		}
	}
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

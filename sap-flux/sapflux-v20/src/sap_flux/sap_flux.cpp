#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "Sleepy.h" //https://github.com/kinasmith/Sleepy
#include "RFM69_ATC.h"
#include "SPIFlash_Marzogh.h"
#include <EEPROM.h>
#include "Adafruit_MAX31856.h"

#define NODEID 11
#define GATEWAYID 0
#define FREQUENCY RF69_433MHZ //frequency of radio
#define ATC_RSSI -70 //ideal Signal Strength of trasmission
#define ACK_WAIT_TIME 100 // # of ms to wait for an ack
#define ACK_RETRIES 10 // # of attempts before giving up
#define SERIAL_BAUD 9600 // Serial comms rate
#define FLASH_CAPACITY 524288

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
void sendFromFlash();
void writeToFlash();
uint8_t setAddress();
float getBatteryVoltage(uint8_t pin, uint8_t en);
bool saveEEPROMTime();
uint32_t getEEPROMTime();
void takeMeasurements();
/*==============|| RFM69 ||==============*/
RFM69_ATC radio; //init radio
uint8_t NETWORKID = 100; //base network address
uint8_t attempt_cnt = 0;
bool HANDSHAKE_SENT;
/*==============|| MEMORY ||==============*/
SPIFlash_Marzogh flash(8);
uint32_t FLASH_ADDR = 0;
uint16_t EEPROM_ADDR = 0;
/*==============|| THERMOCOUPLE ||==============*/
Adafruit_MAX31856 tc1 = Adafruit_MAX31856(TC1_CS);
Adafruit_MAX31856 tc2 = Adafruit_MAX31856(TC2_CS);
Adafruit_MAX31856 tc3 = Adafruit_MAX31856(TC3_CS);
/*==============|| UTIL ||==============*/
bool LED_STATE;
uint16_t count = 0;
/*==============|| INTERVAL ||==============*/
const uint8_t REC_MIN = 1; //record interval in minutes
const uint16_t REC_MS = 60000;
const uint16_t REC_INTERVAL = (60000*REC_MIN)/1000; //record interval in seconds
/*==============|| DATA ||==============*/
//Data structure for transmitting the Timestamp from datalogger to sensor (4 bytes)
struct TimeStamp {
	uint32_t timestamp;
};
TimeStamp theTimeStamp; //creates global instantiation of this

//Data structure for storing data locally (12 bytes)
struct Data {
	uint16_t count;
	int16_t tc1;
	int16_t tc2;
	int16_t tc3;
	int16_t brd_tmp;
	int16_t bat_v;
};

//Data Structure for transmitting data packets to datalogger (16 bytes)
struct Payload {
	uint32_t timestamp;
	uint16_t count;
	int16_t tc1;
	int16_t tc2;
	int16_t tc3;
	int16_t brd_tmp;
	int16_t bat_v;
};
Payload thePayload;

void setup()
{
	#ifdef SERIAL_EN
		Serial.begin(SERIAL_BAUD);
	#endif
	randomSeed(analogRead(A4)); //set random seed
	NETWORKID += setAddress();
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower();
	radio.encrypt(null);
	radio.enableAutoPower(ATC_RSSI); //Test to see if this is actually working at some point
	DEBUG("--Transmitting on Network: "); DEBUG(NETWORKID); DEBUG(", as Node: "); DEBUGln(NODEID);
	pinMode(LED, OUTPUT);
	//Ping the datalogger. If it's alive, it will return the current time. If not, wait and try again.
	digitalWrite(LED, HIGH); //write LED high to signal attempting connection
	while(!getTime()) { //this saves time to the struct which holds the time globally
		radio.sleep();
		DEBUGFlush();
		delay(10000);
	}
	digitalWrite(LED, LOW); //write low to signal success
	DEBUG("--Time is: "); DEBUG(theTimeStamp.timestamp); DEBUGln("--");
	DEBUG("--Flash Mem: ");
	flash.begin();
	uint16_t _name = flash.getChipName();
	uint32_t capacity = flash.getCapacity();
	DEBUG("W25X"); DEBUG(_name); DEBUG("**  ");
	DEBUG("capacity: "); DEBUG(capacity); DEBUGln(" bytes");
	DEBUGln("Erasing Chip!"); flash.eraseChip();
  tc1.begin(); tc2.begin(); tc3.begin();
  tc1.setThermocoupleType(MAX31856_TCTYPE_E);
  tc2.setThermocoupleType(MAX31856_TCTYPE_E);
  tc3.setThermocoupleType(MAX31856_TCTYPE_E);
	DEBUGln("--Thermocouples are engaged");
}

void loop()
{
	DEBUG("sleep - sleeping for "); DEBUG(REC_INTERVAL); DEBUG(" seconds"); DEBUGln();
	DEBUGFlush();
	radio.sleep();
	count++;
	for(int i = 0; i < REC_MIN; i++)
		Sleepy::loseSomeTime(REC_MS);
	//===========|| MCU WAKES UP HERE ||===========
	takeMeasurements();
	if(!getTime()) { //Ping Datalogger,
		DEBUGln("=== Storing in Flash ===");
		// if there is no response. Take readings, save readings to EEPROM.
		DEBUG("data - local time is: ");DEBUGln(theTimeStamp.timestamp); //update current time to payload
		writeToFlash(); //save that data to EEPROM
		Blink(50);
		Blink(50);
		DEBUGln();
	} else {
		//Check to see if there is data waiting to be sent
		DEBUG("flash - byte 0 == "); DEBUGln(flash.readByte(0));
		if(flash.readByte(0) < 255) {
			DEBUGln("=== Sending from Flash ===");
			sendFromFlash();
		}
		if(!(flash.readByte(0) < 255)) { //Check again if there is no data, then take some readings and send them
			DEBUGln("=== Sending New Measurement ===");
			thePayload.timestamp = theTimeStamp.timestamp; //this timestamp is updated whenever calling getTime().
			//Note the ACK retry and wait times. Very important, and much slower for 433mhz radios that are doing stuff too
			LED_STATE = true;
			digitalWrite(LED, LED_STATE);
			if (radio.sendWithRetry(GATEWAYID, (const void*)(&thePayload), sizeof(thePayload)), ACK_RETRIES, ACK_WAIT_TIME) {
				DEBUG("data - snd - "); DEBUG('['); DEBUG(GATEWAYID); DEBUG("] ");
				DEBUG(sizeof(thePayload)); DEBUGln(" bytes  ");
				LED_STATE = false;
				digitalWrite(LED, LED_STATE);
			} else {
				DEBUGln("data - snd - Failed . . . no ack");
				writeToFlash(); //if data fails to transfer, Save that data to eeprom to be sent later
				Blink(50);
				Blink(50);
			}
		}
		DEBUGln();
	}
}

void takeMeasurements() {
	thePayload.count = count;
	thePayload.tc1 = int(tc1.readThermocoupleTemperature()*100);
	thePayload.tc2 = int(tc2.readThermocoupleTemperature()*100);
	thePayload.tc3 = int(tc3.readThermocoupleTemperature()*100);
	thePayload.brd_tmp = int(tc1.readCJTemperature()*100);
	thePayload.bat_v = getBatteryVoltage(BAT_V, BAT_EN);
	DEBUG("data - ");
	DEBUG(thePayload.count); DEBUG(",");
	DEBUG(thePayload.tc1); DEBUG(",");
	DEBUG(thePayload.tc2); DEBUG(",");
	DEBUG(thePayload.tc3); DEBUG(",");
	DEBUG(thePayload.brd_tmp); DEBUG(",");
	DEBUG(thePayload.bat_v); DEBUGln();
}

bool saveEEPROMTime(uint32_t t)
{
	if(EEPROM.put(EEPROM_ADDR, t)){
		// DEBUG("time - saved to EEPROM: "); DEBUGln(t);
		return 1;
	} else {
		// DEBUG("time - saved to EEPROM: "); DEBUGln("Failed!");
		return 0;
	}

}

uint32_t getEEPROMTime()
{
	TimeStamp storedTime;
	storedTime = EEPROM.get(EEPROM_ADDR, storedTime);
	// DEBUG("time - read from EEPROM: "); DEBUGln(storedTime.timestamp);
	return storedTime.timestamp;
}

void writeToFlash() {
	Data theData; //Data struct to store data to be written to EEPROM
	//pull data from Payload to save there. Is there Data in Payload?
	theData.count = thePayload.count;
	theData.tc1 = thePayload.tc1;
	theData.tc2 = thePayload.tc2;
	theData.tc3 = thePayload.tc3;
	theData.bat_v = thePayload.bat_v;
	theData.brd_tmp = thePayload.brd_tmp;

	//compare to current time. If the time stamp is the same, no connection has been made with
	//the datalogger yet.
	if(theTimeStamp.timestamp != getEEPROMTime()) {
		DEBUG("time - updating time to: "); DEBUGln(theTimeStamp.timestamp);
		saveEEPROMTime(theTimeStamp.timestamp);
	} else {
		DEBUGln("time - no new time");
	}
	//Make sure there is space to write the next recording
	if(FLASH_ADDR < FLASH_CAPACITY - sizeof(theData)) {
		// Write data to flash
		if(flash.writeAnything(FLASH_ADDR, theData)) {
			DEBUG("flash - saving "); DEBUG(sizeof(theData));
			DEBUG(" bytes to address "); DEBUGln(FLASH_ADDR);
			DEBUG("flash - ")
			DEBUG(theData.count); DEBUG(",");
			DEBUG(theData.tc1); DEBUG(",");
			DEBUG(theData.tc2); DEBUG(",");
			DEBUG(theData.tc3); DEBUG(",");
			DEBUG(theData.brd_tmp); DEBUG(",");
			DEBUG(theData.bat_v); DEBUGln();
			FLASH_ADDR += sizeof(theData); //increment address to next place
			DEBUG("flash - new address is "); DEBUGln(FLASH_ADDR);
		}
	} else {
		DEBUGln("flash - FULL!!!! Sleepying Forever");
		radio.sleep();
		Sleepy::powerDown();
	}
}

void sendFromFlash() {
	DEBUGln("flash - Checking for Stored Data ");
	Data storedData; //struct to save data in
	uint8_t storeIndex = 1;
	/*
   The stored time is one record interval behind the actual time that the data was stored
   because time stamps are saved with successful transmissions and EEPROM data is stored when
   those transmissions are not successful
	 */
	FLASH_ADDR = 0; //Go to address of first chunk of data.
	// Check if there is Data
	// //As long as data has NOT been erased, the first byte of the data chunk should be less than 255
	while(flash.readByte(FLASH_ADDR) < 255) { // -->!!!!!!!<-- NOT SURE IF THIS WORKS !!!!!!!
		Payload savedPayload;
		DEBUG("flash - Value of First Byte: "); DEBUGln(flash.readByte(FLASH_ADDR));
		/*
      It could be any of the values in the struct
      because they are stored as int's, not floats.
      But the battery voltage will never be below 0.01
		 */
		DEBUG("flash - store time: "); DEBUGln(getEEPROMTime());
		uint32_t rec_time = getEEPROMTime() + (REC_INTERVAL * storeIndex); //Calculate the actual recorded time
		DEBUG("flash - rec time: "); DEBUGln(rec_time);
		//Read in saved data to the Data struct
		if(flash.readAnything(FLASH_ADDR, storedData)) {
			DEBUG("flash - data at addr "); DEBUGln(FLASH_ADDR);
			DEBUG(" ");
			DEBUG(storedData.count); DEBUG(","); DEBUG(storedData.tc1); DEBUG(",");
			DEBUG(storedData.tc2); DEBUG(","); DEBUG(storedData.tc3); DEBUG(",");
			DEBUG(storedData.brd_tmp); DEBUG(","); DEBUG(storedData.bat_v); DEBUGln();
			//Save data into the Payload struct
			savedPayload.timestamp = rec_time;
			savedPayload.count = storedData.count;
			savedPayload.tc1 = storedData.tc1;
			savedPayload.tc2 = storedData.tc2;
			savedPayload.tc3 = storedData.tc3;
			savedPayload.bat_v = storedData.bat_v;
			savedPayload.brd_tmp = storedData.brd_tmp;
		}
		//Send data to datalogger
		digitalWrite(LED, HIGH);
		if (radio.sendWithRetry(GATEWAYID, (const void*)(&savedPayload), sizeof(savedPayload)), 20, 500) {
			DEBUG("snd - "); DEBUG(sizeof(thePayload)); DEBUGln(" bytes  ");
			//if successfully sent, Go to next chunk of data
			FLASH_ADDR += sizeof(storedData); //increment to next chunk of data
			storeIndex += 1; //and increment the count of saved values
			digitalWrite(LED, LOW);
		} else {
			attempt_cnt++;
			DEBUG(" failed . . . no ack");
			DEBUG(attempt_cnt);
			Blink(50);
			Blink(50);
			if(attempt_cnt > 20) {
				DEBUGln("radio - Attempt Count Exceeded!")
				/*
	       to avoid infinite loops if the datalogger dies permanently while attempting to transmit saved data
	       it will try 20 times before giving up on that specific sample
				 */
				FLASH_ADDR += sizeof(storedData);
				storeIndex += 1;
				uint16_t waitTime = random(1000);
				for(int i = 0; i < 5; i++)
					Blink(100);
				DEBUG(" waiting for "); DEBUG(waitTime); DEBUGln("ms");
				radio.sleep();
				Sleepy::loseSomeTime(waitTime);
			}
		}
	}
	DEBUGln("flash - Setting address back to 0");
	FLASH_ADDR = 0; //Set Index to beginning of DATA for Writing Events (or...other location for load balancing?)
	EEPROM_ADDR = random(0, 1023);
	DEBUGln("flash - Erasing Sector 0");
	flash.eraseSector(0);
}
/**
 * [getTime description]
 * @return [description]
 */
bool getTime() {
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
 * [Blink description]
 * @param t [description]
 */
void Blink(uint8_t t) {
	LED_STATE = !LED_STATE;
	digitalWrite(LED, LED_STATE);
	delay(t);
	LED_STATE = !LED_STATE;
	digitalWrite(LED, LED_STATE);
	delay(t);
}
/**
 * [setAddress description]
 * @return [description]
 */
uint8_t setAddress() {
	//sets network address based on which solder jumpers are closed
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
	return addr;
}
/**
 * [getBatteryVoltage description]
 * @param  pin [description]
 * @param  en  [description]
 * @return     [description]
 */
float getBatteryVoltage(uint8_t pin, uint8_t en) {
  uint16_t R23 = 32650.0;
  uint32_t R22 = 55300.0;
	float readings = 0.0;
  pinMode(en, OUTPUT);
	digitalWrite(en, HIGH);
	delay(10);
	for (byte i=0; i<3; i++)
		readings += analogRead(pin);
  digitalWrite(en, LOW);
  readings /= 3.3;
	float v = (3.3 * (readings/1023.0)) * (R23/R22); //Calculate battery voltage
  return v;
}

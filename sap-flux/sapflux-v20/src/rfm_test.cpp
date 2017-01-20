#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "Sleepy.h" //https://github.com/kinasmith/Sleepy
#include "RFM69_ATC.h"
#include "SPIFlash_Marzogh.h"
#include "Adafruit_MAX31856.h"

#define NODEID 17
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
/*==============|| RFM69 ||==============*/
RFM69_ATC radio; //init radio
uint8_t NETWORKID = 100; //base network address
uint8_t attempt_cnt = 0;
bool HANDSHAKE_SENT;
/*==============|| FLASH ||==============*/
SPIFlash_Marzogh flash(8);
uint32_t FLASH_ADDR = 4;
/*==============|| THERMOCOUPLE ||==============*/
Adafruit_MAX31856 tc1 = Adafruit_MAX31856(TC1_CS);
Adafruit_MAX31856 tc2 = Adafruit_MAX31856(TC2_CS);
Adafruit_MAX31856 tc3 = Adafruit_MAX31856(TC3_CS);
/*==============|| UTIL ||==============*/
bool LED_STATE;
uint16_t count = 0;
/*==============|| INTERVAL ||==============*/
const uint8_t REC_MIN = 1; //record interval in minutes
const uint16_t REC_INTERVAL = (10000*REC_MIN)/1000; //record interval in seconds
/*==============|| DATA ||==============*/
//Data structure for transmitting the Timestamp from datalogger to sensor (4 bytes)
struct TimeStamp {
	uint32_t timestamp;
};
TimeStamp theTimeStamp; //creates global instantiation of this

//Data structure for storing data in EEPROM (12 bytes)
struct Data {
	uint16_t count = 0;
	int16_t tc1 = 0;
	int16_t tc2 = 0;
	int16_t tc3 = 0;
	int16_t bat_v = 0;
	int16_t brd_tmp = 0;
};
//Data Structure for transmitting data packets to datalogger (16 bytes)
struct Payload {
	uint32_t timestamp = 182097834;
	uint16_t count = 37;
	int16_t tc1 = 1243;
	int16_t tc2 = 1294;
	int16_t tc3 = 1208;
	int16_t brd_tmp = 1289;
	int16_t bat_v = 4281;
};
Payload thePayload;

void setup() {
	Serial.begin(SERIAL_BAUD);
	NETWORKID += setAddress();
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower();
	radio.encrypt(null);
	radio.enableAutoPower(ATC_RSSI); //Test to see if this is actually working at some point
	DEBUG("--Transmitting on Network: "); DEBUG(NETWORKID); DEBUG(", as Node: "); DEBUGln(NODEID);
	pinMode(LED, OUTPUT);
	randomSeed(analogRead(A4)); //set random seed
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
	// DEBUGln("Erasing Chip!"); flash.eraseChip();
  tc1.begin(); tc2.begin(); tc3.begin();
  tc1.setThermocoupleType(MAX31856_TCTYPE_E);
  tc2.setThermocoupleType(MAX31856_TCTYPE_E);
  tc3.setThermocoupleType(MAX31856_TCTYPE_E);
	DEBUGln("--Thermocouples are engaged");
}

void loop() {
	Payload thePayload; //Store the data to be transmitted in this struct
	DEBUG("sleep - sleeping for "); DEBUG(REC_INTERVAL); DEBUG(" seconds"); DEBUGln();
	DEBUGFlush();
	radio.sleep();
	count++;
	for(int i = 0; i < REC_MIN; i++)
		Sleepy::loseSomeTime(60000);
	//===========|| MCU WAKES UP HERE ||===========
	if(!getTime()) { //Ping Datalogger,
		// if there is no response. Take readings, save readings to EEPROM.
		DEBUGln("data - no response, saving data locally");
		thePayload.count = count;
		thePayload.tc1 = int(tc1.readThermocoupleTemperature()*100);
		thePayload.tc2 = int(tc2.readThermocoupleTemperature()*100);
		thePayload.tc3 = int(tc3.readThermocoupleTemperature()*100);
		thePayload.brd_tmp = int(tc1.readCJTemperature()*100);
		thePayload.bat_v = getBatteryVoltage(BAT_V, BAT_EN);
		writeToFlash(); //save that data to EEPROM
		Blink(50);
		Blink(50);
	} else {
		//Check to see if there is data waiting to be sent
		DEBUG("flash - byte 4 == "); DEBUGln(flash.readByte(4));
		//flash memory clears to HIGH?...
		if(flash.readByte(4) < 255) { //check the first byte of data, if there is data send all of it
			sendFromFlash();
		}
		if(flash.readByte(4) == 255) { //Check again if there is no data, then take some readings and send them
			thePayload.timestamp = theTimeStamp.timestamp; //this timestamp is updated whenever calling getTime().
			thePayload.count = count;
			thePayload.tc1 = int(tc1.readThermocoupleTemperature()*100);
			thePayload.tc2 = int(tc2.readThermocoupleTemperature()*100);
			thePayload.tc3 = int(tc3.readThermocoupleTemperature()*100);
			thePayload.brd_tmp = int(tc1.readCJTemperature()*100);
			thePayload.bat_v = getBatteryVoltage(BAT_V, BAT_EN);
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
	}
}

void writeToFlash() {
	Data theData; //Data struct to store data to be written to EEPROM
	TimeStamp storedTime;
	//pull data from Payload to save there
	theData.count = thePayload.count;
	theData.tc1 = thePayload.tc1;
	theData.tc2 = thePayload.tc2;
	theData.tc3 = thePayload.tc3;
	theData.bat_v = thePayload.bat_v;
	theData.brd_tmp = thePayload.brd_tmp;
	//update the saved time to the time of the last successful transaction (if they are different)
	flash.readAnything(0, storedTime);
	if(theTimeStamp.timestamp != storedTime.timestamp) {
		DEBUG("flash - saved time is: "); DEBUGln(storedTime.timestamp);
		DEBUG("flash - updating to: "); DEBUGln(theTimeStamp.timestamp);
		for(byte i = 0; i < sizeof(storedTime); i++) //must erase sector before writing in new data
			flash.eraseSector(i);
		flash.writeAnything(0, theTimeStamp);
	} else {
		DEBUGln("flash - No New Time Stamp");
	}
	// //Make sure there is space to write the next recording
	if(FLASH_ADDR < FLASH_CAPACITY - sizeof(theData)) {
		DEBUG("flash - saving "); DEBUG(sizeof(theData));
		DEBUG(" bytes to address "); DEBUGln(FLASH_ADDR);
		DEBUG(theData.count); DEBUG(",");
		DEBUG(theData.tc1); DEBUG(",");
		DEBUG(theData.tc2); DEBUG(",");
		DEBUG(theData.tc3); DEBUG(",");
		DEBUG(theData.brd_tmp); DEBUG(",");
		DEBUG(theData.bat_v); DEBUGln();
		for(byte i = FLASH_ADDR; i < sizeof(theData); i++) //must erase sector before writing in new data
			flash.eraseSector(i);
		flash.writeAnything(FLASH_ADDR, theData);
		FLASH_ADDR += sizeof(theData);
		DEBUG("flash - new address is "); DEBUGln(FLASH_ADDR);
	} else {
		DEBUGln("flash - FULL!!!! Sleepying Forever");
		radio.sleep();
		Sleepy::powerDown();
	}
}

void sendFromFlash() {
	DEBUGln("flash - Checking for Stored Data ");
	Data blank; //init blank struct to erase data
	Data storedData; //struct to save data in
	TimeStamp storedTime;
	/*
   The stored time is one record interval behind the actual time that the data was stored
   because time stamps are saved with successful transmissions and EEPROM data is stored when
   those transmissions are not successful
	 */
	uint8_t storeIndex = 1;
	flash.readAnything(0,storedTime);//get previously stored time (at address 0)
	DEBUG("flash - Stored time: ");DEBUGln(storedTime.timestamp);
	FLASH_ADDR = 0 + sizeof(storedTime.timestamp); //Set to next address
	//Read in saved data to the Data struct
	flash.readAnything(FLASH_ADDR, storedData);
	DEBUG("flash - Data: ");
	DEBUG(storedData.count); DEBUG(",");
	DEBUG(storedData.tc1); DEBUG(",");
	DEBUG(storedData.tc2); DEBUG(",");
	DEBUG(storedData.tc3); DEBUG(",");
	DEBUG(storedData.brd_tmp); DEBUG(",");
	DEBUG(storedData.bat_v); DEBUGln();

	while (storedData.bat_v > 0) { //while there is data available from the EEPROM
		/*
      It could be any of the values in the struct
      because they are stored as int's, not floats.
      But the battery voltage will never be below 0.01
		 */
		uint32_t rec_time = storedTime.timestamp + (REC_INTERVAL * storeIndex); //Calculate the actual recorded time
		DEBUG("flash - rec time: "); DEBUGln(rec_time);
		//Save data into the Payload struct
		thePayload.timestamp = rec_time;
		thePayload.count = storedData.count;
		thePayload.tc1 = storedData.tc1;
		thePayload.tc2 = storedData.tc2;
		thePayload.tc3 = storedData.tc3;
		thePayload.bat_v = storedData.bat_v;
		thePayload.brd_tmp = storedData.brd_tmp;
		DEBUG("flash - saved data timestamp "); DEBUGln(thePayload.timestamp);
		DEBUG("flash - data @ addr "); DEBUG(FLASH_ADDR);
		DEBUGln("data - sending to logger");
		//Send data to datalogger
		digitalWrite(LED, HIGH);

		if (radio.sendWithRetry(GATEWAYID, (const void*)(&thePayload), sizeof(thePayload)), ACK_RETRIES, ACK_WAIT_TIME) {
			DEBUG(" snd - "); DEBUG(sizeof(thePayload)); DEBUGln(" bytes  ");
			//if successfully sent, erase chunk of data
			for(byte i = FLASH_ADDR; i < sizeof(storedData); i++) //must erase sector before writing in new data
				flash.eraseSector(i);
			// flash.writeAnything(FLASH_ADDR, blank); //If successfully sent, erase that data chunk...
			FLASH_ADDR += sizeof(storedData); //increment to next...
			DEBUG(" snd - Reading in new data "); DEBUG(sizeof(storedData)); DEBUGln(" bytes");
			flash.readAnything(FLASH_ADDR, storedData); // and read in the next saved data...
			storeIndex += 1; //and increment the count of saved values
			digitalWrite(LED, LOW);
		} else {
			attempt_cnt++;
			DEBUG(" failed . . . no ack");
			DEBUG(attempt_cnt);
			Blink(50);
			Blink(50);
			if(attempt_cnt > 10) {
				/*
         to avoid infinite loops if the datalogger dies permanently while attempting to transmit saved data
         it will try 10 times before giving up on that specific sample
				 */
				for(byte i = FLASH_ADDR; i < sizeof(storedData); i++) //must erase sector before writing in new data
				 	flash.eraseSector(i);
				// flash.writeAnything(FLASH_ADDR, blank);
				FLASH_ADDR += sizeof(storedData);
				flash.readAnything(FLASH_ADDR, storedData);
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
	FLASH_ADDR = 0 + sizeof(theTimeStamp.timestamp); //Set Index to beginning of DATA
}

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

void Blink(uint8_t t) {
	LED_STATE = !LED_STATE;
	digitalWrite(LED, LED_STATE);
	delay(t);
	LED_STATE = !LED_STATE;
	digitalWrite(LED, LED_STATE);
	delay(t);
}

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

float getBatteryVoltage(uint8_t pin, uint8_t en) {
  uint16_t R22 = 32650.0;
  uint32_t R23 = 55300.0;
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

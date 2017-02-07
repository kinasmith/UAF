#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "Sleepy.h"
#include "RFM69_ATC.h"
#include "SPIFlash_Marzogh.h"
#include <EEPROM.h>
#include "MAX31856.h"

#define NODEID 78
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
float getBatteryVoltage();
bool saveEEPROMTime(uint32_t t);
uint32_t getEEPROMTime();
void sendMeasurement();
bool transmitDataPackage();
/*==============|| RFM69 ||==============*/
RFM69_ATC radio; //init radio
uint8_t attempt_cnt = 0;
uint8_t NETWORKID = 100;
/*==============|| MEMORY ||==============*/
SPIFlash_Marzogh flash(8);
uint32_t FLASH_ADDR = 0;
uint16_t EEPROM_ADDR = 0;
/*==============|| THERMOCOUPLE ||==============*/
MAX31856 tc1(TC1_CS, T_TYPE, CUTOFF_60HZ, AVG_SEL_16SAMP, CMODE_OFF, ONESHOT_ON); //one shot mode
MAX31856 tc2(TC2_CS, T_TYPE, CUTOFF_60HZ, AVG_SEL_16SAMP, CMODE_OFF, ONESHOT_ON); //one shot mode
MAX31856 tc3(TC3_CS, T_TYPE, CUTOFF_60HZ, AVG_SEL_16SAMP, CMODE_OFF, ONESHOT_ON); //one shot mode
// MAX31856 tc1(TC1_CS, T_TYPE, CUTOFF_60HZ, AVG_SEL_16SAMP, CMODE_AUTO, ONESHOT_OFF); //one shot mode
// MAX31856 tc2(TC2_CS, T_TYPE, CUTOFF_60HZ, AVG_SEL_16SAMP, CMODE_AUTO, ONESHOT_OFF); //one shot mode
// MAX31856 tc3(TC3_CS, T_TYPE, CUTOFF_60HZ, AVG_SEL_16SAMP, CMODE_AUTO, ONESHOT_OFF); //one shot mode
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
// const uint16_t SLEEP_MS = 15000; //one minute in milliseconds
// const uint32_t SLEEP_SECONDS = SLEEP_INTERVAL * (SLEEP_MS/1000); //Sleep interval in seconds
// const uint16_t REC_INTERVAL = 1000; //record interval during measurement event in mS
// const uint16_t REC_DURATION = 5; //how long the measurement is in seconds

/*==============|| DATA ||==============*/
//Data structure for transmitting the Timestamp from datalogger to sensor (4 bytes)
struct TimeStamp {
	uint32_t timestamp;
};
TimeStamp theTimeStamp; //creates global instantiation of this

// //Data Structure for transmitting data packets to datalogger (16 bytes)
// struct Payload {
// 	uint32_t timestamp;
// 	uint16_t count;
// 	double tc1;
// 	double tc2;
// 	double tc3;
// 	double internal;
// 	double bat_v;
// };
// Payload thePayload;
//
//Data Structure for transmitting data packets to datalogger
struct Payload {
	uint32_t timestamp;
	uint16_t count;
	int32_t sense;
	int16_t brd_tmp;
	double bat_v;
	double excite_v;
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
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
  // 1
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  // 2
  digitalWrite(LED, HIGH);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower();
	radio.encrypt(null);
	radio.enableAutoPower(ATC_RSSI); //Test to see if this is actually working at some point
  radio.sleep();
  digitalWrite(LED, LOW);
  delay(100);
  // 3
  digitalWrite(LED, HIGH);
  tc1.begin();
	tc2.begin();
	tc3.begin();
  digitalWrite(LED, LOW);
  delay(100);
  // 4
  digitalWrite(LED, HIGH);
  flash.begin();
  if(flash.eraseChip()){
    if(flash.powerDown()){
      digitalWrite(LED, LOW);
    }
  }
}

long idx = 0;

void loop() {
  digitalWrite(LED, HIGH);
  tc1.prime(); tc2.prime(); tc3.prime();
  tc1.read(); tc2.read(); tc3.read();
  thePayload.sense = tc1.getExternal();
  thePayload.bat_v = tc2.getExternal();
  thePayload.excite_v = tc3.getExternal();
  thePayload.brd_tmp = int(tc1.getInternal());
  thePayload.count = count;

  if(getTime()) {
    thePayload.timestamp = theTimeStamp.timestamp;
    if(radio.sendWithRetry(GATEWAYID, (const void*)(&thePayload), sizeof(thePayload)), ACK_RETRIES, ACK_WAIT_TIME) {
      digitalWrite(LED, LOW);
    }
    radio.sendWithRetry(GATEWAYID, "r", 1);
  }
  digitalWrite(LED2, HIGH);
  flash.powerUp();
  flash.writeAnything(idx, thePayload);
  idx = flash.getAddress(sizeof(thePayload));
  if(flash.powerDown()) digitalWrite(LED2, LOW);

  count++;
  radio.sleep();
  Sleepy::loseSomeTime(5000);
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

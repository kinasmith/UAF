#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "Sleepy.h"
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
uint32_t FLASH_ADDR = 100;
uint16_t EEPROM_ADDR = 0;
/*==============|| THERMOCOUPLE ||==============*/
Adafruit_MAX31856 tc1 = Adafruit_MAX31856(TC1_CS);
Adafruit_MAX31856 tc2 = Adafruit_MAX31856(TC2_CS);
Adafruit_MAX31856 tc3 = Adafruit_MAX31856(TC3_CS);
/*==============|| UTIL ||==============*/
bool LED_STATE;
uint16_t count = 0;
uint8_t sentMeasurement = 0;
/*==============|| INTERVAL ||==============*/
const uint8_t REC_MIN = 1; //record interval in minutes
const uint16_t REC_MS = 30000;
const uint16_t REC_INTERVAL = (30000*REC_MIN)/1000; //record interval in seconds
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

struct Measurement {
	uint32_t time;
	int16_t tc1;
	int16_t tc2;
	int16_t tc3;
};

uint32_t current_time;
uint32_t stop_saved_time = 0;
uint32_t log_saved_time = 0;
uint32_t h_saved_time = 0;
uint16_t h_interval = 0;
uint16_t log_interval = 1000;
uint16_t h_pulse_on = 6000;
uint32_t h_pulse_off = 30 * 60000L;
uint16_t stop_time = 30000;
uint8_t h_status = 0;
uint32_t saved_time = 0;
uint16_t interval = 1000;

struct Mem {
	long one;
	long two;
	long three;
	long four;
	long five;
};
Mem thisMem;

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
	// Ping the datalogger. If it's alive, it will return the current time. If not, wait and try again.
	digitalWrite(LED, HIGH); //write LED high to signal attempting connection
	while(!getTime()) { //this saves time to the struct which holds the time globally
		radio.sleep();
		DEBUGFlush();
		delay(10000);
	}
	digitalWrite(LED, LOW); //write low to signal success
	DEBUG("--Time is: "); DEBUG(theTimeStamp.timestamp); DEBUGln("--");

  tc1.begin(); tc2.begin(); tc3.begin();
  tc1.setThermocoupleType(MAX31856_TCTYPE_E);
  tc2.setThermocoupleType(MAX31856_TCTYPE_E);
  tc3.setThermocoupleType(MAX31856_TCTYPE_E);
	DEBUGln("--Thermocouples are engaged");

	DEBUG("--Flash Mem: ");
	flash.begin();
	uint16_t _name = flash.getChipName();
	uint32_t capacity = flash.getCapacity();
	DEBUG("W25X"); DEBUG(_name); DEBUG("**  ");
	DEBUG("capacity: "); DEBUG(capacity); DEBUGln(" bytes");
	DEBUGln("Erasing Chip!"); flash.eraseChip();
}

void loop()
{
	current_time = millis();
	if(saved_time + interval < current_time) {
		thisMem.one = random(0, 1000);
		thisMem.two = random(0, 1000);
		thisMem.three = random(0, 1000);
		thisMem.four = random(0, 1000);
		thisMem.five = random(0, 1000);
		flash.writeAnything(FLASH_ADDR, thisMem);
		Mem newMem;
		if(flash.readAnything(FLASH_ADDR, newMem)) {
			DEBUGln(newMem.one);
			DEBUGln(newMem.two);
			DEBUGln(newMem.three);
			DEBUGln(newMem.four);
			DEBUGln(newMem.five);
			DEBUGln();
			FLASH_ADDR += sizeof(thisMem);
		}
		saved_time = current_time;
	}
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

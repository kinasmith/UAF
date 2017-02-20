/**
 * TODO:
 * NOTE: There is a bug in the timestamp retrieval for more than one saved
 * measurement. Look at Measurement #38 and #39 in the Serial Log of the 02-10 Test
 * And Compare to the saved data. It's a bug in the sensor code and how the time for the new
 * recordings are saved.
 * Also look at the data$cnt graph. It looks like the two saved measurements were offset forward
 * in time by one record interval.
 *
 */
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "Sleepy.h"
#include "RFM69_ATC.h"
#include "SPIFlash_Marzogh.h"
#include <EEPROM.h>
#include "MAX31856.h"

#define NODEID 102
#define GATEWAYID 0
#define FREQUENCY RF69_433MHZ //frequency of radio
#define ATC_RSSI -70 //ideal Signal Strength of trasmission
#define ACK_WAIT_TIME 100 // # of ms to wait for an ack
#define ACK_RETRIES 10 // # of attempts before giving up
#define SERIAL_BAUD 115200 // Serial comms rate
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
bool ping();
void Blink(uint8_t);
uint8_t setAddress();
float getBatteryVoltage();
bool saveEEPROMTime(uint32_t t);
uint32_t getEEPROMTime();
void takeMeasurement();
void sendMeasurement();
bool transmitDataPackage();
/*==============|| RFM69 ||==============*/
RFM69_ATC radio;
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
uint16_t count = 0;
uint8_t sentMeasurement = 0;
/*==============|| TIMING ||==============*/
const uint8_t HEATER_ON_TIME = 6; //in seconds
const uint16_t SLEEP_INTERVAL = 30; //sleep time in minutes (Cool Down)
const uint16_t SLEEP_MS = 60000; //one minute in milliseconds
const uint32_t SLEEP_SECONDS = SLEEP_INTERVAL * (SLEEP_MS/1000); //Sleep interval in seconds
const uint16_t REC_INTERVAL = 1000; //record interval during measurement event in mS
const uint16_t REC_DURATION = 4 * 60; //how long the measurement is in seconds

	// Testing Intervals
// const uint8_t HEATER_ON_TIME = 1; //in Record Intervals
// const uint16_t SLEEP_INTERVAL = 1; //sleep time in minutes (Cool Down)
// const uint16_t SLEEP_MS = 15000; //one minute in milliseconds
// const uint32_t SLEEP_SECONDS = SLEEP_INTERVAL * (SLEEP_MS/1000); //Sleep interval in seconds
// const uint16_t REC_INTERVAL = 1000; //record interval during measurement event in mS
// const uint16_t REC_DURATION = 10; //how many measurements

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

	/* --| Set Pin Modes |-- */
	pinMode(HEATER_EN, OUTPUT);
	pinMode(LED, OUTPUT);
	pinMode(LED2, OUTPUT);
	NETWORKID = setAddress(); //Sets the network address depending on solder jumpers
	/* --| Initialize Radio |-- */
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower();
	radio.encrypt(null);
	radio.enableAutoPower(ATC_RSSI); //<-- This actually works!
	DEBUG("-- Network Address: "); DEBUG(NETWORKID); DEBUG("."); DEBUGln(NODEID);
	/* --| Ping the Datalogger and get Time |-- */
	digitalWrite(LED, HIGH); //write LED high to signal attempting connection
	while(!ping()) { //If there is no response, wait 10 seconds and try again, forever.
		radio.sleep();
		DEBUGFlush();
		Sleepy::loseSomeTime(10000);
	}
	if (radio.sendWithRetry(GATEWAYID, "r", 1)) { //unlatch the datalogger from this Sensor
		DEBUG("snd > 'r'");
	}
	else DEBUGln("unlatch failed . . . no ack");
	digitalWrite(LED, LOW); //signal successful transmission
	DEBUG("-- Time is: "); DEBUG(theTimeStamp.timestamp); DEBUGln("--");
	saveEEPROMTime(theTimeStamp.timestamp); //Save that time to EEPROM
	/* --| Initialize the Thermocouples |-- */
  tc1.begin();
	tc2.begin();
	tc3.begin();
	DEBUGln("-- Thermocouples are engaged");
	/* --| Power Up Flash before Initialize |-- */
	DEBUG("-- Flash Mem powering up ");
	if(flash.powerUp()) {
		DEBUG(", OK!");
	} else DEBUG(". . failed! ");
	/* --| Initialize Flash |-- */
	digitalWrite(LED2, HIGH); //turn on LED for Flash init
	flash.begin();
	uint16_t _name = flash.getChipName();
	uint32_t capacity = flash.getCapacity();
	DEBUG(" W25X"); DEBUG(_name); DEBUG("**  ");
	DEBUG(" capacity: "); DEBUG(capacity); DEBUGln(" bytes");
	DEBUGln("-- Flash: Erasing Chip!");
	while(!flash.eraseChip()) {
	}
	digitalWrite(LED2, LOW); //Everything Flash is OK

	DEBUG("Cooling Time is "); DEBUG(float(SLEEP_SECONDS/60.0)); DEBUGln("minutes");
	DEBUGln("==========================");
}

void loop()
{
	if(getTime()) { //get current Time
		saveEEPROMTime(theTimeStamp.timestamp);
	} else {
		DEBUGln("time - No Response From Datalogger");
	}

	// Power up Flash
	DEBUG(" - Powering Up");
	if(flash.powerUp()) {
		DEBUGln(". . . OK!");
	} else DEBUGln(". . . FAILED!");
	measurementNum = 0; //reset Measurement Counter
	thePayload.bat_v = getBatteryVoltage(); //get Battery Voltage

	takeMeasurement();

	/* --| Ping Datalogger and send stored data |-- */
	if(ping()) { //Also Latches Datalogger to Sensor until it's finished
		if(flash.readByte(0) < 255) { //Make sure there is data @ address 0
			DEBUGln("=== Sending from Flash ===");
			sendMeasurement();
		}
		if (radio.sendWithRetry(GATEWAYID, "r", 1)) DEBUGln("-- unlatch");
	} else {
		DEBUGln("ping - No Response");
	}

	//power down flash before sleeping
	DEBUG("flash - Powering Down");
	if(flash.powerDown()) {
		DEBUGln(". . . OK!");
	} else DEBUGln(". . . FAILED!");
	DEBUG("sleep - sleeping for "); DEBUG(SLEEP_SECONDS); DEBUG(" seconds"); DEBUGln();
	DEBUGFlush();
	radio.sleep();
	for(uint8_t i = 0; i < SLEEP_INTERVAL; i++)
		Sleepy::loseSomeTime(SLEEP_MS);
	/* --| MCU Wakes up after 30 minutes Here |-- */
	count++; //Increment the Reading Counter
}

void takeMeasurement() {
		/* --| Begin Measurement Cycle |-- */
		while(measurementNum < REC_DURATION) {
			Measurement thisMeasurement; //place to store sensor readings temporarily
			tc1.prime(); tc2.prime(); tc3.prime(); //force sensors to take reading
			tc1.read(); tc2.read(); tc3.read(); //read the values
			thisMeasurement.tc1 = tc1.getExternal();
			thisMeasurement.tc2 = tc2.getExternal();
			thisMeasurement.tc3 = tc3.getExternal();
			thisMeasurement.internal = tc1.getInternal();
			thisMeasurement.count = count; //reading count is incremented at the end of the sleep cycle
			digitalWrite(LED2, HIGH); //signal start of Flash Write
			if(flash.writeAnything(FLASH_ADDR, thisMeasurement)) { //write to flash. Prints on success
				DEBUG("flash - ");
				DEBUG(thisMeasurement.tc1); DEBUG(", ");
				DEBUG(thisMeasurement.tc2); DEBUG(", ");
				DEBUG(thisMeasurement.tc3); DEBUG(", ");
				DEBUG(thisMeasurement.internal); DEBUG(", ");
				DEBUG(thisMeasurement.count); DEBUG(", ");
				DEBUG("at Address "); DEBUGln(FLASH_ADDR);
				FLASH_ADDR += sizeof(thisMeasurement);
				digitalWrite(LED2, LOW); //turn off LED on successful Write
			} else {
				DEBUG("flash - failed to write at Address ");
				DEBUGln(FLASH_ADDR);
			}
			if(measurementNum <= HEATER_ON_TIME) {
				digitalWrite(HEATER_EN, HIGH); //On cycle start, turn on heater
			} else {
				digitalWrite(HEATER_EN, LOW); //turn off after 6 seconds
			}
			DEBUGFlush();
			Sleepy::loseSomeTime(REC_INTERVAL); // wait one seconds
			/* --| MCU Wakes up Here after 1 second |-- */
			measurementNum++; //increment counter
		}

}
/**
 * Sends stored measurements to datalogger
 */
void sendMeasurement()
{
	Measurement storedMeasurement; //holder for retrieving measurements from Flash
	Measurement firstMeasurement; //to store the Reading Count Number
	//Read and store the number of the first reading for later
	uint8_t first_count;
	if(flash.readAnything(0, firstMeasurement)){
		first_count = firstMeasurement.count;
	}
	//reference that first stored reading with current reading number. This is how many readings are stored in Flash
	uint8_t num_of_stored_measurements = count - first_count;
	uint8_t temporary_this_measurement_number = 0; // What is this?
	DEBUG("store - Number of Stored Measurements:") DEBUGln(num_of_stored_measurements);

	thePayload.timestamp = getEEPROMTime(); //retreive saved time
	DEBUG("time -saved time is: "); DEBUGln(thePayload.timestamp);
	FLASH_ADDR = 0; //Go to address of first chunk of data.
	while(flash.readByte(FLASH_ADDR) < 255) { // Check if there is Data
		if(flash.readAnything(FLASH_ADDR, storedMeasurement)) { //Read in saved data to the Data struct
			// DEBUG("flash - data at addr "); DEBUGln(FLASH_ADDR); DEBUG(" "); DEBUG(storedMeasurement.tc1); DEBUG(","); DEBUG(storedMeasurement.tc2); DEBUG(","); DEBUG(storedMeasurement.tc3); DEBUGln();
			thePayload.tc1 = storedMeasurement.tc1;
			thePayload.tc2 = storedMeasurement.tc2;
			thePayload.tc3 = storedMeasurement.tc3;
			thePayload.internal = storedMeasurement.internal;
			thePayload.count = storedMeasurement.count;
		}
		//Correctly increment the Time Stamp on Stored Data to reflect the 1 second measurement interval, and 30 minute Record interval
		uint8_t this_measurement_num = num_of_stored_measurements - (count - storedMeasurement.count); //invert the record count.
		if(this_measurement_num != temporary_this_measurement_number && this_measurement_num != 0) { //if it's not the first one, or the current one, add Sleep Seconds
			thePayload.timestamp += SLEEP_SECONDS;
			temporary_this_measurement_number = this_measurement_num;
			DEBUGln();
		} else thePayload.timestamp += REC_INTERVAL/1000; //Otherwise just add the record interval to stored time

		if(transmitDataPackage()) { //Send thePayload
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
			DEBUGln("poopy pants will live forever");
			// FLASH_ADDR += sizeof(storedMeasurement); //WTF is this?...still ignoring
		}
	}
	DEBUGln("flash - Setting address back to 0");
	FLASH_ADDR = 0; //Set Index to beginning of DATA for Writing Events (or...other location for load balancing?)
	DEBUGln("flash - erasing chip");
	flash.eraseChip();
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
			DEBUG(" > t");
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
				DEBUG('['); DEBUG(radio.SENDERID); DEBUG("] > ");
				DEBUG(theTimeStamp.timestamp); DEBUG(" [RX_RSSI:"); DEBUG(radio.RSSI); DEBUG("]"); DEBUGln();
				TIME_RECIEVED = true;
				digitalWrite(LED, LOW);
			}
			if (radio.ACKRequested()) radio.sendACK();
		}
	}
	return true;
}

bool ping() {
	bool PING_SENT = false;
	bool PING_RECIEVED = false;
	digitalWrite(LED, HIGH); //signal start of communication
	if(!PING_SENT) { //Send request for status to the Datalogger
		DEBUG("ping - ");
		if (radio.sendWithRetry(GATEWAYID, "p", 1)) {
			DEBUG(" > p");
			PING_SENT = true;
		}
		else {
			DEBUGln("failed: no ack");
			return false; //if there is no response, returns false and exits function
		}
	}
	while(!PING_RECIEVED && PING_SENT) { //Wait for the ping to be returned
		if (radio.receiveDone()) {
			if (radio.DATALEN == sizeof('p')) { //check to make sure it's the right size
				DEBUG('['); DEBUG(radio.SENDERID); DEBUG("] > ");
				DEBUG(radio.DATA[0]); DEBUG(" [RX_RSSI:"); DEBUG(radio.RSSI); DEBUG("]"); DEBUGln();
				PING_RECIEVED = true;
				digitalWrite(LED, LOW);
			}
			if (radio.ACKRequested()) radio.sendACK();
			return true;
		}
	}
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
 * Blinks LED pin
 * @param t On and Off Time
 */
void Blink(uint8_t t)
{
	digitalWrite(LED, HIGH);
	delay(t);
	digitalWrite(LED, LOW);
	delay(t);
}
/**
 * Gets battery voltage from sensor. Does 3x average on the analog reading
 * @return Actual voltage as Float
 */
float getBatteryVoltage() {
	uint16_t readings = 0;
	digitalWrite(BAT_EN, HIGH);
	delay(10);
	for (byte i=0; i<3; i++)
		readings += analogRead(BAT_V);
	readings /= 3;
	float v = 3.3 * (readings/1023.0) * (4300.0/2700.0); //Calculate battery voltage
	digitalWrite(BAT_EN, LOW);
	return v;
}

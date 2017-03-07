#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include "Sleepy.h" //https://github.com/kinasmith/Sleepy
#include "RFM69_ATC.h" //https://www.github.com/lowpowerlab/rfm69
#include <MCP342x.h>

/****************************************************************************/
/***********************    DON'T FORGET TO SET ME    ***********************/
/****************************************************************************/
#define NODEID    6
#define NETWORKID 101
/****************************************************************************/
/****************************************************************************/

#define GATEWAYID      0
#define FREQUENCY       RF69_433MHZ
#define ATC_RSSI -70 //ideal Signal Strength of trasmission
#define ACK_WAIT_TIME 100 // # of ms to wait for an ack
#define ACK_RETRIES 10 // # of attempts before giving up
#define LED 3
#define SERIAL_BAUD 9600

#define BAT_V A3
#define TEMP A1
#define SENS_EN 4

//Thermistor Var's
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950 //Check this!
#define SERIESRESISTOR 10000
#define NUMSAMPLES 5

// #define SERIAL_EN //Comment this out to remove Serial comms and save a few kb's of space
#ifdef SERIAL_EN
#define DEBUG(input)   {Serial.print(input); delay(1);}
#define DEBUGln(input) {Serial.println(input); delay(1);}
#define DEBUGFlush() { Serial.flush(); }
#else
#define DEBUG(input);
#define DEBUGln(input);
#define DEBUGFlush();
#endif

ISR(WDT_vect){ Sleepy::watchdogEvent();} // set watchdog for Sleepy

/*==============|| FUNCTIONS ||==============*/
void sendStoredEEPROMData();
void writeDataToEEPROM();
bool getTime();
bool ping();
int32_t getSensorValue();
double getTemperature();
double getBatteryVoltage(int);
bool saveEEPROMTime(uint32_t t);
uint32_t getEEPROMTime();
void Blink(uint8_t);

/*==============|| RFM69 ||==============*/
RFM69_ATC radio; //init radio
uint8_t attempt_cnt = 0;

/*==============|| UTIL ||==============*/
uint16_t count = 0;

/*==============|| EEPROM ||==============*/
uint16_t EEPROM_ADDR = 4;

/*==============|| TIMING ||==============*/
const uint8_t SLEEP_INTERVAL = 15; //sleep time in minutes
const uint16_t SLEEP_MS = 60000; //one minute in milliseconds
const uint32_t SLEEP_SECONDS = SLEEP_INTERVAL * (SLEEP_MS/1000); //Sleep interval in seconds

/*==============|| ADC ||==============*/
// 0x68 is the default address for all MCP342x devices
uint8_t address = 0x68;
MCP342x adc = MCP342x(address);
MCP342x::Config
config(MCP342x::channel1, MCP342x::oneShot, MCP342x::resolution16, MCP342x::gain1);
MCP342x::Config status;// Configuration/status read back from the ADC

/*==============|| DATA ||==============*/
//Data structure for transmitting the Timestamp from datalogger to sensor (4 bytes)
struct TimeStamp {
	uint32_t timestamp;
};
TimeStamp theTimeStamp; //creates global instantiation of this

//Data structure for storing data in EEPROM
struct Data {
	uint16_t count = 0;
	int32_t sense = 0;
	int16_t brd_tmp = 0;
	double bat_v = 0;
};

//Data Structure for transmitting data packets to datalogger
struct Payload {
	uint32_t timestamp;
	uint16_t count;
	uint32_t sense;
	double brd_tmp;
	double bat_v;
};
Payload thePayload;

void setup()
{
	#ifdef SERIAL_EN
		Serial.begin(SERIAL_BAUD);
	#endif
	randomSeed(analogRead(0)); //set random seed
	Wire.begin(); // Begin i2c
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower();
	radio.encrypt(null);
	radio.enableAutoPower(ATC_RSSI); //Test to see if this is actually working at some point
	DEBUG("-- Network Address: "); DEBUG(NETWORKID); DEBUG("."); DEBUGln(NODEID);
	pinMode(LED, OUTPUT);      // Set LED Mode
	// Ping the datalogger. If it's alive, it will return the current time. If not, wait and try again.
	//LED polarity is inverted. stupid
	digitalWrite(LED, LOW); //turn on LED to signal attempting connection
	while(!ping()) { //this saves time to the struct which holds the time globally
		radio.sleep();
		DEBUGFlush();
		Sleepy::loseSomeTime(10000);
		}
	if (radio.sendWithRetry(GATEWAYID, "r", 1)) {
		DEBUGln("snd > 'r'");
	}
	else DEBUGln("unlatch failed . . . no ack");
	digitalWrite(LED, HIGH); //write low to signal success
	getTime();
	DEBUG("-- Time is: "); DEBUG(theTimeStamp.timestamp); DEBUGln("--");
	saveEEPROMTime(theTimeStamp.timestamp);

	// --| Initialize Devices |--
	MCP342x::generalCallReset();
	delay(1); // MC342x needs 300us to settle
	// Check device present
	Wire.requestFrom(address, (uint8_t) 1);
	if (!Wire.available()) {
		DEBUG("No device found at address ");
		DEBUGln(address);
		while (1);
	}
	#ifdef SERIAL_EN
		for (int i = 0 ; i < EEPROM.length() ; i++) {
			EEPROM.write(i, 0);
		}
	#endif
}

void loop()
{
	//Gets current time at start of measurement cycle. Stores in global theTimeStamp struct
	if(getTime()) { //Gets time from datalogger and stores in Global Variable
		// saveEEPROMTime(theTimeStamp.timestamp);
	} else {
		DEBUGln("time - No Response from Datalogger");
	}
	//Take Reading!
	//Turn on V-REF for Sensor and Thermistor (will be on for 100ms)
	pinMode(SENS_EN, OUTPUT);
	digitalWrite(SENS_EN, HIGH);// write enable high for 10 ms
	Sleepy::loseSomeTime(100);  // let the Capacitor charge for a moment
	digitalWrite(SENS_EN, LOW); // write enable low. Falling edge triggers FET
	delay(1);    // wait for reference to stablize
	thePayload.sense = getSensorValue();
	thePayload.brd_tmp = getTemperature();
	thePayload.bat_v = getBatteryVoltage(BAT_V);
	thePayload.count = count;
	if(ping()) { //Check that the logger is UP
		//Check to see if there is data waiting to be sent
		thePayload.timestamp = theTimeStamp.timestamp;
		if(EEPROM.read(4) > 0) { //check the first byte of data, if there is data send all of it
			DEBUGln("eeprom - sending stored data");
			sendStoredEEPROMData();
		}
		if(EEPROM.read(4) == 0) { //Check again if there is no data, just take a new and send it
			//Note the ACK retry and wait times. Very important, and much slower for 433mhz radios that are doing stuff too
			digitalWrite(LED, LOW); //turn on LED
			if (radio.sendWithRetry(GATEWAYID, (const void*)(&thePayload), sizeof(thePayload)), ACK_RETRIES, ACK_WAIT_TIME) {
				DEBUG("data - snd - "); DEBUG('['); DEBUG(GATEWAYID); DEBUG("] ");
				DEBUG(sizeof(thePayload)); DEBUGln(" bytes  ");
				digitalWrite(LED, HIGH); //Turn Off LED
			} else {
				DEBUGln("data - snd - Failed . . . no ack");
				writeDataToEEPROM(); //if data fails to transfer, Save that data to eeprom to be sent later
				Blink(50);
				Blink(50);
			}
		}
		//disengage the the Datalogger
		if (radio.sendWithRetry(GATEWAYID, "r", 1)) {
			DEBUGln("snd > r");
		}
	} else { //If there is no response from the datalogger....
		DEBUGln("ping - No Response");
		// if there is no response. Take readings, save readings to EEPROM.
		DEBUGln("data - no response, saving data locally");
		//stored time is now at the START of
		saveEEPROMTime(theTimeStamp.timestamp); //only save the time to EEPROM if not failed attempt.
		writeDataToEEPROM(); //save that data to EEPROM
		Blink(50);
		Blink(50);
	}

	DEBUG("sleep - sleeping for "); DEBUG(SLEEP_SECONDS); DEBUG(" seconds"); DEBUGln();
	DEBUGFlush();
	radio.sleep();
	count++;
	for(uint8_t i = 0; i < SLEEP_INTERVAL; i++)
		Sleepy::loseSomeTime(SLEEP_MS);
	/*==============|| Wakes Up Here! ||==============*/
}

/**
 * [sendStoredEEPROMData description]
 */
void sendStoredEEPROMData() {
	DEBUGln("--Retrieving Stored EEPROM DATA--");
	Data blank; //init blank struct to erase data
	Data theData; //struct to save data in
	Payload temp;
/*
	The stored time is the last successful transmission. So we need to add
	1 Sleep Interval to the Stored Time to get accurate time math
 */
	uint8_t storeIndex = 1;
	uint32_t eep_time = 0UL;
	EEPROM.get(0, eep_time); //get previously stored time (at address 0)
	EEPROM_ADDR = 0 + sizeof(theTimeStamp.timestamp); //Set to next address
	EEPROM.get(EEPROM_ADDR, theData); //Read in saved data to the Data struct
	while (theData.bat_v > 0) { //while there is data available from the EEPROM
		/*
		  It could be any of the values in the struct because remember they are stored as int's, not floats.
		  But battery voltage will never be below less than 0
		 */
		uint32_t rec_time = eep_time + SLEEP_SECONDS*storeIndex; //Calculate the actual recorded time
		//Save data into the Payload struct
		temp.timestamp = rec_time;
		temp.count = theData.count;
		temp.sense = theData.sense;
		temp.brd_tmp = theData.brd_tmp;
		temp.bat_v = theData.bat_v;
		DEBUG("eeprom - time data was recorded is "); DEBUGln(thePayload.timestamp);
		DEBUG("eeprom - data @ addr "); DEBUG(EEPROM_ADDR);
		//Send data to datalogger
		digitalWrite(LED, LOW);
		if (radio.sendWithRetry(GATEWAYID, (const void*)(&temp), sizeof(temp)), ACK_RETRIES, ACK_WAIT_TIME) {
			DEBUG(" snd - ");
			DEBUG(sizeof(temp)); DEBUGln(" bytes  ");
			EEPROM.put(EEPROM_ADDR, blank); //If successfully sent, erase that data chunk...
			EEPROM_ADDR += sizeof(theData); //increment to next...
			EEPROM.get(EEPROM_ADDR, theData); // and read in the next saved data...
			storeIndex += 1; //and increment the count of saved values
			digitalWrite(LED, HIGH);
		} else {
			//--ALL OF THIS MIGHT OR MIGHT NOT EVEN WORK!--
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
				EEPROM.put(EEPROM_ADDR, blank);
				EEPROM_ADDR += sizeof(theData);
				EEPROM.get(EEPROM_ADDR, theData);
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
	EEPROM_ADDR = 0 + sizeof(theTimeStamp.timestamp); //Set Index to beginning of DATA
}
/**
 * [writeDataToEEPROM description]
 */
void writeDataToEEPROM() {
	Data theData; //Data struct to store data to be written to EEPROM
	uint32_t eep_time = 0UL;
	//pull data from Payload to save there
	theData.count = thePayload.count;
	theData.sense = thePayload.sense;
	theData.brd_tmp = thePayload.brd_tmp;
	theData.bat_v = thePayload.bat_v;

	//update the saved eeprom time to the time of the last successful transaction (if they are different)
	EEPROM.get(0, eep_time);
	if(theTimeStamp.timestamp != eep_time)
		EEPROM.put(0, theTimeStamp.timestamp);

	//Make sure there is space to write the next recording
	if(EEPROM_ADDR < EEPROM.length() - sizeof(theData)) {
		DEBUG("eeprom - saving "); DEBUG(sizeof(theData)); DEBUG(" bytes to address "); DEBUGln(EEPROM_ADDR);
		EEPROM.put(EEPROM_ADDR, theData);
		EEPROM_ADDR += sizeof(theData);
	} else {
		DEBUGln("eeprom - FULL!!!! Sleepying Forever");
		radio.sleep();
		Sleepy::powerDown();
	}
}

/**
 * [getTime description]
 * @return [description]
 */
bool getTime()
{
	bool HANDSHAKE_SENT = false;
	bool TIME_RECIEVED = false;
	digitalWrite(LED, LOW);
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
				digitalWrite(LED, HIGH);
			}
			if (radio.ACKRequested()) radio.sendACK();
		}
	}
	return true;
}
/**
 * [ping description]
 * @return [description]
 */
bool ping()
{
	bool PING_SENT = false;
	bool PING_RECIEVED = false;
	digitalWrite(LED, HIGH); //signal start of communication
	if(!PING_SENT) { //Send request for status to the Datalogger
		DEBUG("ping - ");
		if (radio.sendWithRetry(GATEWAYID, "p", 1)) {
			DEBUGln(" > p");
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
* [Blink description]
* @param t [description]
*/
void Blink(uint8_t t)
{
 	digitalWrite(LED, LOW); //turn LED on
 	delay(t);
 	digitalWrite(LED, HIGH); //turn LED off
 	delay(t);
}

/**
 * Reads the Voltage level at an analog pin
 * @param  pin Analog Pin to reading
 * @return     returns the actual voltage at that pin
 */
double getBatteryVoltage(int pin) // takes 100ms
{
	float v = 0;
	for (int i = 0; i < NUMSAMPLES; i++) {
		v += analogRead(pin);
		Sleepy::loseSomeTime(10);
	}
	// convert analog reading into actual voltage
	v = v / NUMSAMPLES;
	v = 3.3 * v / 1023.0; // Converts 10bit value to voltage
	return v;             // return value
}

/**
 * Reads ADC Values
 * @return The Values.....
 */
int32_t getSensorValue()
{
	int32_t value = 0;
	// uint8_t err;
  MCP342x::Config status;
  // // Initiate a conversion; convertAndRead() will wait until it can be read
  uint8_t err = adc.convertAndRead(MCP342x::channel1, MCP342x::oneShot,
				   MCP342x::resolution16, MCP342x::gain1,
				   1000000, value, status);
  if (err) {
    DEBUG("Convert error: ");
    // DEUBGln(err);
  }
  else {
    // DEBUG("Value: ");
    // DEBUGln(value);
  }
	return value;
}

bool saveEEPROMTime(uint32_t t) {
	if(EEPROM.put(0, t)) return 1;
	else return 0;
}

uint32_t getEEPROMTime(){
	TimeStamp storedTime;
	storedTime = EEPROM.get(0, storedTime);
	return storedTime.timestamp;
}

double getTemperature()
{
	float ADC_reading = 0;
	float Vcc = 3.3;
	float Ve = 2.048;

	for(int i = 0; i < NUMSAMPLES; i++) {
		ADC_reading += analogRead(TEMP);
		Sleepy::loseSomeTime(10);
	}
	ADC_reading /= NUMSAMPLES;
	float therm_res = (SERIESRESISTOR * Vcc * ADC_reading)/((1023*Ve)-(Vcc*ADC_reading));
	// DEBUG("Thermistor Resistance = ") DEBUGln(therm_res);

	double steinhart;
	steinhart = therm_res / THERMISTORNOMINAL;     // (R/Ro)
	steinhart = log(steinhart);                  // ln(R/Ro)
	steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
	steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
	steinhart = 1.0 / steinhart;                 // Invert
	steinhart -= 273.15;                         // convert to C
	// DEBUG("Temperature "); DEBUG(steinhart); DEBUGln(" *C");

	return steinhart;
}

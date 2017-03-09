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
#define NODEID    1
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
double getBatteryVoltage();
bool saveEEPROMTime(uint32_t t);
uint32_t getEEPROMTime();
void Blink(uint8_t);

/*==============|| RFM69 ||==============*/
RFM69_ATC radio; //init radio

/*==============|| UTIL ||==============*/
uint16_t count = 0;

/*==============|| EEPROM ||==============*/
uint16_t EEPROM_ADDR = 5; //Start of data storage

/*==============|| TIMING ||==============*/
const uint8_t SLEEP_INTERVAL = 1; //sleep time in minutes
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
	pinMode(LED, OUTPUT);      // Set LED Mode
	randomSeed(analogRead(0)); //set random seed
	Wire.begin(); // Begin i2c
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower();
	radio.encrypt(null);
	radio.enableAutoPower(ATC_RSSI); //Test to see if this is actually working at some point
	DEBUG("-- Network Address: "); DEBUG(NETWORKID); DEBUG("."); DEBUGln(NODEID);
	// Ping the datalogger. If it's alive, it will return the current time. If not, wait and try again.
	//LED polarity is inverted. stupid
	while(!ping()) { //this saves time to the struct which holds the time globally
		//Check status of Datalogger. If it isn't there, blink twice every 5 seconds
		radio.sleep();
		DEBUGFlush();
		Blink(250);
		Blink(250);
		Blink(250);
		Sleepy::loseSomeTime(5000);
	}
	DEBUGln("-- Datalogger Available")
	if (!radio.sendWithRetry(GATEWAYID, "r", 1)) {
		DEBUGln("snd - unlatch failed...");
	}
	//If the EEPROM is full of data, address 0 will be 255.
	//Go ahead and send it over to the Logger
	uint8_t storedData_FLAG = EEPROM.read(0);
	if(storedData_FLAG == 255) {
		// DEBUGln("Sending Stored data")
		sendStoredEEPROMData();
		EEPROM.write(0, 0);
	} else {
		DEBUGln("-- EEPROM not full");
	}

	// --| Initialize Devices |--
	MCP342x::generalCallReset();
	delay(1); // MC342x needs 300us to settle
	// Check device present
	Wire.requestFrom(address, (uint8_t) 1);
	if (!Wire.available()) {
		// DEBUG("No device found at address ");
		// DEBUGln(address);
		while (1);
	} else {
		DEBUGln("-- ADC Initialized");
	}
	#ifdef SERIAL_EN
		// for (int i = 0 ; i < EEPROM.length() ; i++) {
			// EEPROM.write(i, 0);
		// }
	#endif
	if(!getTime()){
		DEBUGln("--Get Time Failed");
	}
	DEBUG("-- Current time is "); DEBUGln(theTimeStamp.timestamp);
}

void loop()
{
	//Gets current time at start of measurement cycle. Stores in global theTimeStamp struct
	if(!getTime()) { //Gets time from datalogger and stores in Global Variable
		DEBUGln("time - No Response from Datalogger");
	}
	uint32_t now = millis();
	DEBUG("- Measurement...");
	//Take Reading!
	//Turn on voltage reference for Sensor and Thermistor (will stay on for 100ms)
	pinMode(SENS_EN, OUTPUT);
	digitalWrite(SENS_EN, HIGH);
	Sleepy::loseSomeTime(100); //let capacitor charge
	digitalWrite(SENS_EN, LOW); //falling edge enables switch
	delay(1); // wait for reference to stablize
	//sensor reading and temp should happen right next to each other
	thePayload.sense = getSensorValue();
	thePayload.brd_tmp = getTemperature();
	thePayload.bat_v = getBatteryVoltage();
	thePayload.count = count;
	DEBUG("duration "); DEBUG(millis()-now); DEBUGln("ms");
	if(ping()) { //Check that the logger is listening
		DEBUGln("- Datalogger Available");
		//Check to see if there is data waiting to be sent
		thePayload.timestamp = theTimeStamp.timestamp; //set payload time to current time
		if(EEPROM.read(5) > 0) { //check the first byte of data storage, if there is data send it all
			DEBUGln("- Stored Data Available, Sending...");
			sendStoredEEPROMData();
		}
		if(EEPROM.read(5) == 0) { //Make sure there is no data stored, then send the measurement that was just taken
			DEBUG("- No Stored Data, Sending ")
			digitalWrite(LED, LOW); //turn on LED
			if (radio.sendWithRetry(GATEWAYID, (const void*)(&thePayload), sizeof(thePayload)), ACK_RETRIES, ACK_WAIT_TIME) {
				DEBUG(sizeof(thePayload)); DEBUG(" bytes -> ");
				DEBUG('['); DEBUG(GATEWAYID); DEBUG("] ");
				digitalWrite(LED, HIGH); //Turn Off LED
			} else {
				DEBUG("snd - Failed . . . no ack");
				// writeDataToEEPROM(); //if data fails to transfer, Save that data to eeprom to be sent later
				Blink(50);
				Blink(50);
			}
			DEBUGln();
		}
		//disengage the the Datalogger
		if(!radio.sendWithRetry(GATEWAYID, "r", 1)) {
			DEBUGln("snd - unlatch failed ...");
		}
	} else { //If there is no response from the datalogger....
		DEBUGln("- Datalogger Not Available, Saving Locally");
		writeDataToEEPROM(); //save that data to EEPROM
		Blink(50);
		Blink(50);
	}
	DEBUG("- Sleeping for "); DEBUG(SLEEP_SECONDS); DEBUG(" seconds"); DEBUGln();
	DEBUGFlush();
	radio.sleep();
	count++;
	for(uint8_t i = 0; i < SLEEP_INTERVAL; i++)
		Sleepy::loseSomeTime(SLEEP_MS);
	/*==============|| Wakes Up Here! ||==============*/
	DEBUGFlush();
}

/**
 * [sendStoredEEPROMData description]
 */
void sendStoredEEPROMData() {
	DEBUGln("-- Retrieving Stored EEPROM DATA --");
	Data blank; //init blank struct to erase data
	Data theData; //struct to save data in
	Payload tmp; //for temporary holding of data
/*
	The stored time is the last successful transmission. So we need to add
	1 Sleep Interval to the Stored Time to get accurate time math
 */
	uint8_t storeIndex = 1;
	uint32_t eep_time = 0UL;
	EEPROM.get(1, eep_time); //get previously stored time (at address 1)
	EEPROM_ADDR = 1 + sizeof(theTimeStamp.timestamp); //Set to next address
	EEPROM.get(EEPROM_ADDR, theData); //Read in saved data to the Data struct
	DEBUG(".stored time "); DEBUGln(theTimeStamp.timestamp);
	while (theData.bat_v > 0) { //while there is data available from the EEPROM
		/*
		  It could be any of the values in the struct because remember they are stored as int's, not floats.
		  But battery voltage will never be below less than 0
		 */
		uint32_t rec_time = eep_time + SLEEP_SECONDS*storeIndex; //Calculate the actual recorded time
		DEBUG(".rec time "); DEBUGln(rec_time);
		//Save data into the Payload struct
		tmp.timestamp = rec_time;
		tmp.count = theData.count;
		tmp.sense = theData.sense;
		tmp.brd_tmp = theData.brd_tmp;
		tmp.bat_v = theData.bat_v;
		//Send data to datalogger
		digitalWrite(LED, LOW);
		if (radio.sendWithRetry(GATEWAYID, (const void*)(&tmp), sizeof(tmp)), ACK_RETRIES, ACK_WAIT_TIME) {
			DEBUG(".data sent, erasing...");
			EEPROM.put(EEPROM_ADDR, blank); //If successfully sent, erase that data chunk...
			EEPROM_ADDR += sizeof(theData); //increment to next...
			DEBUG("reading index "); DEBUGln(EEPROM_ADDR);
			EEPROM.get(EEPROM_ADDR, theData); // and read in the next saved data...
			storeIndex += 1; //and increment the count of saved values
			DEBUG(".store index "); DEBUGln(storeIndex);
			digitalWrite(LED, HIGH);
		} else {
			DEBUG(".data send failed, waiting for retry");
			uint16_t waitTime = random(1000);
			for(int i = 0; i < 5; i++)
				Blink(100);
			radio.sleep();
			Sleepy::loseSomeTime(waitTime);
		}
	}
	EEPROM_ADDR = 1 + sizeof(theTimeStamp.timestamp); //Reset Index to beginning of DATA
}
/**
 * [writeDataToEEPROM description]
 */
void writeDataToEEPROM() {
	DEBUGln("-- Storing EEPROM DATA --");
	Data theData; //Data struct to store data to be written to EEPROM
	uint32_t eep_time = 0UL; //place to hold the saved time
	//pull data from Payload to save there
	theData.count = thePayload.count;
	theData.sense = thePayload.sense;
	theData.brd_tmp = thePayload.brd_tmp;
	theData.bat_v = thePayload.bat_v;

	//update the saved eeprom time to the time of the last successful transaction (if they are different)
	EEPROM.get(1, eep_time);
	DEBUG(".saved time "); DEBUGln(eep_time);
	DEBUG(".current time "); DEBUGln(theTimeStamp.timestamp);
	if(theTimeStamp.timestamp != eep_time) {
		EEPROM.put(1, theTimeStamp.timestamp);
		DEBUGln(".updated time");
	}
	//Make sure there is space to write the next recording
	if(EEPROM_ADDR < EEPROM.length() - sizeof(theData)) {
		DEBUG(".saving "); DEBUG(sizeof(theData)); DEBUG(" bytes to address "); DEBUGln(EEPROM_ADDR);
		EEPROM.put(EEPROM_ADDR, theData);
		EEPROM_ADDR += sizeof(theData);
	} else {
		EEPROM.write(0, 255);
		DEBUGln(".eeprom FULL Sleepying Forever");
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
		// DEBUG("time - ");
		if (radio.sendWithRetry(GATEWAYID, "t", 1)) {
			// DEBUG("snd . . ");
			HANDSHAKE_SENT = true;
		}
		else {
			// DEBUGln("failed . . . no ack");
			return false; //if there is no response, returns false and exits function
		}
	}
	while(!TIME_RECIEVED && HANDSHAKE_SENT) { //Wait for the time to be returned
		if (radio.receiveDone()) {
			if (radio.DATALEN == sizeof(theTimeStamp)) { //check to make sure it's the right size
				theTimeStamp = *(TimeStamp*)radio.DATA; //save data
				// DEBUG(" rcv - "); DEBUG('['); DEBUG(radio.SENDERID); DEBUG("] ");
				// DEBUG(theTimeStamp.timestamp); DEBUG(" [RX_RSSI:"); DEBUG(radio.RSSI); DEBUG("]"); DEBUGln();
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
		// DEBUG("ping - ");
		if (radio.sendWithRetry(GATEWAYID, "p", 1)) {
			// DEBUGln(" > p");
			PING_SENT = true;
		}
		else {
			// DEBUGln("failed: no ack");
			return false; //if there is no response, returns false and exits function
		}
	}
	while(!PING_RECIEVED && PING_SENT) { //Wait for the ping to be returned
		if (radio.receiveDone()) {
			if (radio.DATALEN == sizeof('p')) { //check to make sure it's the right size
				// DEBUG('['); DEBUG(radio.SENDERID); DEBUG("] > ");
				// DEBUG(radio.DATA[0]); DEBUG(" [RX_RSSI:"); DEBUG(radio.RSSI); DEBUG("]"); DEBUGln();
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
double getBatteryVoltage() // takes 100ms
{
	float v = 0;
	for (int i = 0; i < NUMSAMPLES; i++) {
		v += analogRead(BAT_V);
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
    // DEBUG("Convert error: ");
    // DEUBGln(err);
  }
  else {
    // DEBUG("Value: ");
    // DEBUGln(value);
  }
	return value;
}

bool saveEEPROMTime(uint32_t t) {
	if(EEPROM.put(1, t)) return 1;
	else return 0;
}

uint32_t getEEPROMTime(){
	TimeStamp storedTime;
	storedTime = EEPROM.get(1, storedTime);
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

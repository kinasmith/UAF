#include <Arduino.h> //Built in
#include <SPI.h> //Built in
#include <Wire.h> //Built in
#include <EEPROM.h> //Built in
#include "Sleepy.h" //https://github.com/kinasmith/Sleepy
#include "RFM69_ATC.h" //https://www.github.com/lowpowerlab/rfm69
#include <MCP342x.h> //https://github.com/stevemarple/MCP342x

/****************************************************************************/
/***********************    DON'T FORGET TO SET ME    ***********************/
/****************************************************************************/
#define NODEID    9 //Node Address
#define NETWORKID 100 //Network to communicate on
/****************************************************************************/

#define GATEWAYID 0 //Address of datalogger/reciever
#define FREQUENCY RF69_433MHZ
#define ATC_RSSI -70 //ideal Signal Strength of trasmission
#define ACK_WAIT_TIME 100 // # of ms to wait for an ack
#define ACK_RETRIES 10 // # of attempts before giving up
#define LED 3 //LED pin
#define SERIAL_BAUD 9600 //Serial communication rate

#define BAT_V A3 //Analog pin for measuring Battery Voltage
#define REF_V A2 //Analog pin for measuring Reference Voltage
#define TEMP A1 //Analog Pin for measuring thermistor output
#define SENS_EN 4 //Enable pin for excitation voltage switch

/*==============|| Thermistor Calculations ||==============*/
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950 //Check this!
#define SERIESRESISTOR 10000
#define NUMSAMPLES 3

/*==============|| Easy way of commenting out Serial Prints, etc for deployment ||==============*/
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

ISR(WDT_vect){ Sleepy::watchdogEvent();} // set watchdog for Sleepy

/*==============|| FUNCTIONS ||==============*/
void sendStoredEEPROMData(); //retrieves stored data and sends it
void writeDataToEEPROM(); //stores data in EEPROM
bool getTime(); //gets time from datalogger
bool ping(); //pings datalogger and latches this Node until unlatch message is sent

int32_t getSensorValue(); //returns sensor value from ADC
double getTemperature(); //returns Thermistor temperature (uses reference voltage)
double getBatteryVoltage(); //returns Battery Voltage
double getRefVoltage(); //returns reference voltage
bool saveEEPROMTime(uint32_t t); //saves Timestamp to EEPROM
uint32_t getEEPROMTime(); //returns Timestamp from EEPROM
void Blink(uint8_t); //Blinks LED

/*==============|| RFM69 ||==============*/
RFM69_ATC radio; //Declare radio (using Automatic Power Adjustments)

/*==============|| UTIL ||==============*/
uint16_t count = 0; //measurement number

/*==============|| EEPROM ||==============*/
uint16_t EEPROM_ADDR = 5; //Start of data storage in EEPROM

/*==============|| TIMING ||==============*/
const uint8_t SLEEP_INTERVAL = 15; //sleep time in minutes
const uint16_t SLEEP_MS = 60000; //one minute in milliseconds
const uint32_t SLEEP_SECONDS = SLEEP_INTERVAL * (SLEEP_MS/1000); //Sleep interval in seconds

/*==============|| ADC ||==============*/
//Setup ADC. Config, etc
uint8_t address = 0x68; // 0x68 is the default address for all MCP342x devices
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
	double ref_v = 0;
};

//Data Structure for transmitting data packets to datalogger
struct Payload {
	uint32_t timestamp;
	uint16_t count;
	uint32_t sense;
	double brd_tmp;
	double bat_v;
	double ref_v;
};
Payload thePayload;

void setup()
{
	#ifdef SERIAL_EN
		Serial.begin(SERIAL_BAUD); //only start Serial if DEBUG is on
	#endif
	Wire.begin(); // Begin i2c
	randomSeed(analogRead(0)); //set random seed
	pinMode(LED, OUTPUT);      // Set LED Mode
	//Setup Radio
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower();
	radio.encrypt(null);
	radio.enableAutoPower(ATC_RSSI);
	DEBUG("-- Network Address: "); DEBUG(NETWORKID); DEBUG("."); DEBUGln(NODEID);
	// Ping the datalogger. If it is alive, it will respond with a 1, and latch to this node until it recieves an "r"
	while(!ping()) {
		DEBUGln("Failed to Setup ping");
		//If datalogger doesn't respond, Blink, wait 5 seconds, and try again
		radio.sleep();
		DEBUGFlush();
		Blink(250);
		Blink(250);
		Sleepy::loseSomeTime(5000);
	}
	DEBUGln("-- Datalogger Available")
	//Tell datalogger to unlatch
	if (!radio.sendWithRetry(GATEWAYID, "r", 1)) {
		DEBUGln("snd - unlatch failed...");
	}
	//If the EEPROM is full of data, address 0 will be 255.
	//If EEPROM is full, send all data to datalogger.
	uint8_t storedData_FLAG = EEPROM.read(0);
	if(storedData_FLAG == 255) {
		DEBUGln("-- Sending Stored data")
		sendStoredEEPROMData();
		EEPROM.write(0, 0);
	} else {
		DEBUGln("-- EEPROM not full");
	}
	//Initialize ADC
	MCP342x::generalCallReset();
	delay(1); // MC342x needs 300us to settle
	Wire.requestFrom(address, (uint8_t) 1);// Check device present
	if (!Wire.available()) {
		DEBUG("No device found at address ");
		DEBUGln(address);
		while (1);
	} else {
		DEBUGln("-- ADC Initialized");
	}
	//Get current time from Datalogger and save it to a variable.
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
	DEBUG("- Measurement...");
	//Take Reading!
	//Turn on voltage reference for Sensor and Thermistor (will stay on for 100ms)
	pinMode(SENS_EN, OUTPUT); //Set pin to output
	digitalWrite(SENS_EN, HIGH); //This pin will charge the capacitor that holds open the FET gate
	Sleepy::loseSomeTime(100); //let capacitor charge
	digitalWrite(SENS_EN, LOW); //falling edge enables switch
	delay(1); // wait for reference to stablize
	//All these things should take less than 100ms
	thePayload.ref_v = getRefVoltage(); //takes 10ms
	thePayload.sense = getSensorValue(); //takes ~50ms
	thePayload.bat_v = getBatteryVoltage(); //takes 10ms
	thePayload.brd_tmp = getTemperature(); //takes 10ms
	thePayload.count = count;

	if(ping()) {
		//If the Datalogger is listening and available to recieve data
		DEBUGln("- Datalogger Available");
		//Check to see if there is data waiting to be sent
		thePayload.timestamp = theTimeStamp.timestamp; //set payload time to current time
		if(EEPROM.read(5) > 0) { //check the first byte of data storage, if there is data, send it all
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
				Blink(50);
				Blink(50);
			}
			DEBUGln();
		}
		//disengage the the Datalogger
		if(!radio.sendWithRetry(GATEWAYID, "r", 1)) {
			DEBUGln("snd - unlatch failed ...");
		}
	}
	else {
		//If there is no response from the Datalogger save data locally
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
}

/**
 * Retrieves data which was stored in EEPROM and sends it to the datalogger
 * with corrected timestamps for all recordings based off of the last stored
 * time.
 */
void sendStoredEEPROMData() {
	DEBUGln("-- Retrieving Stored EEPROM DATA --");
	Data blank; //init blank struct to erase data
	Data theData; //struct to save data in
	Payload tmp; //for temporary holding of data
/*
	The stored time is the time of the last successful transmission,
	so we need to add 1 Sleep Interval to the Stored Time to get accurate timestamp
 */
	uint8_t storeIndex = 1;
	uint32_t eep_time = 0UL;
	EEPROM.get(1, eep_time); //get previously stored time (at address 1)
	EEPROM_ADDR = 1 + sizeof(theTimeStamp.timestamp); //Set to next address
	EEPROM.get(EEPROM_ADDR, theData); //Read in saved data to the Data struct
	DEBUG(".stored time "); DEBUGln(theTimeStamp.timestamp);
	while (theData.bat_v > 0) { //while there is data available in the EEPROM
		uint32_t rec_time = eep_time + SLEEP_SECONDS*storeIndex; //Calculate the actual recorded time
		DEBUG(".rec time "); DEBUGln(rec_time);
		//Save data into the Payload struct
		tmp.timestamp = rec_time;
		tmp.count = theData.count;
		tmp.sense = theData.sense;
		tmp.brd_tmp = theData.brd_tmp;
		tmp.bat_v = theData.bat_v;
		tmp.ref_v = theData.ref_v;
		//Send data to datalogger
		digitalWrite(LED, LOW); //turn on LED to signal transmission
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
			//this has never happened...
			DEBUG(".data send failed, waiting for retry");
			uint16_t waitTime = random(1000);
			for(int i = 0; i < 5; i++)
				Blink(100);
			radio.sleep();
			Sleepy::loseSomeTime(waitTime);
		}
	}
	EEPROM_ADDR = 1 + sizeof(theTimeStamp.timestamp); //Reset Index to beginning of EEPROM_ADDR
	//NOTE: This will front load the fatigue on the EEPROM causing failures of the first indeces
	//NOTE: first. A good thing to do would be to build in a load balancing system to randomly
	//NOTE: place data in the EEPROM with pointers to it.
}
/**
 * Saves sensor readings to EEPROM in case of tranmission failures to the datalogger.
 *
 */
void writeDataToEEPROM() {
	DEBUGln("-- Storing EEPROM DATA --");
	Data theData; //Data struct to store data to be written to EEPROM
	uint32_t eep_time = 0UL; //place to hold the saved time
	//pull data from Payload to local struct to save
	theData.count = thePayload.count;
	theData.sense = thePayload.sense;
	theData.brd_tmp = thePayload.brd_tmp;
	theData.bat_v = thePayload.bat_v;
	theData.ref_v = thePayload.ref_v;

	//update the saved eeprom time to the time of the
	//last successful transaction (if they are different)
	EEPROM.get(1, eep_time);
	DEBUG(".saved time "); DEBUGln(eep_time);
	DEBUG(".current time "); DEBUGln(theTimeStamp.timestamp);
	if(theTimeStamp.timestamp != eep_time) {
		EEPROM.put(1, theTimeStamp.timestamp);
		DEBUGln(".updated time");
	}
	//check if there is space to save the next reading
	//if there isn't, put the device to sleep forever and prime it
	//so that next time it is started, it will dump all saved data to datalogger
	if(EEPROM_ADDR < EEPROM.length() - sizeof(theData)) {
		DEBUG(".saving "); DEBUG(sizeof(theData)); DEBUG(" bytes to address "); DEBUGln(EEPROM_ADDR);
		EEPROM.put(EEPROM_ADDR, theData);
		EEPROM_ADDR += sizeof(theData);
	} else { //if there is not room, set INDEX 0 to 255 and sleep forever
		EEPROM.write(0, 255);
		DEBUGln(".eeprom FULL Sleepying Forever");
		radio.sleep();
		Sleepy::powerDown();
	}
}

/**
 * Requests the current time from the datalogger. Time is saved
 * to the global struct theTimeStamp.timestamp
 * @return boolean, TRUE on recieve, FALSE on failure
 * NOTE: This would be better if it just returned the uint32 value
 * of the timestamp instead of a boolean and then saving the time to a
 * global variable.
 * Success checking could still be done with if(!getTime()>0){} if it returned 0 or -1 on failure.
 */
bool getTime()
{
	bool HANDSHAKE_SENT = false;
	bool TIME_RECIEVED = false;
	digitalWrite(LED, LOW); //turn on LED to signal tranmission event
	//Send request for time to the Datalogger
	if(!HANDSHAKE_SENT) {
		DEBUG("time - ");
		if (radio.sendWithRetry(GATEWAYID, "t", 1)) { //'t' requests time
			DEBUG("snd . . ");
			HANDSHAKE_SENT = true;
		}
		else {
			//if there is no response, returns false and exits function
			DEBUGln("failed . . . no ack");
			return false;
		}
	}
	//Wait for the time to be returned from the datalogger
	while(!TIME_RECIEVED && HANDSHAKE_SENT) {
		if (radio.receiveDone()) {
			if (radio.DATALEN == sizeof(theTimeStamp)) { //check to make sure it's the right size
				theTimeStamp = *(TimeStamp*)radio.DATA; //save data to global variable
				DEBUG(" rcv - "); DEBUG('['); DEBUG(radio.SENDERID); DEBUG("] ");
				DEBUG(theTimeStamp.timestamp); DEBUG(" [RX_RSSI:"); DEBUG(radio.RSSI); DEBUG("]"); DEBUGln();
				TIME_RECIEVED = true;
				digitalWrite(LED, HIGH); //turn off LED
			}
			if (radio.ACKRequested()) radio.sendACK();
		}
	}
	return true;
}
/**
 * Requests a response from the Datalogger. For checking that the Datalogger is online
 * before sending data to it. It also latches this sensor to the datalogger until a 'r' is sent
 * eliminating issues with crosstalk and corruption due to multiple sensors sending data simultaniously.
 *
 * @return boolean, TRUE is successful, FALSE is failure
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
* Blinks an LED using DELAY
* @param t On/Off time
*/
void Blink(uint8_t t)
{
 	digitalWrite(LED, LOW); //turn LED on
 	delay(t);
 	digitalWrite(LED, HIGH); //turn LED off
 	delay(t);
}

/**
 * Reads ADC, Averages value, and calculates actual voltage
 * @return float, battery voltage
 */
double getBatteryVoltage() // takes 100ms
{
	float v = 0;
	for (int i = 0; i < NUMSAMPLES; i++) {
		v += analogRead(BAT_V);
		Sleepy::loseSomeTime(3);
	}
	// convert analog reading into actual voltage
	v = v / NUMSAMPLES;
	v = 3.3 * v / 1023.0; // Converts 10bit value to voltage
	return v;             // return value
}
/**
 * reads the output of the voltage reference (should be ~2.04)
 * NOTE: This is also inherently somewhat inaccurate, and probably shouldn't be used in
 * calculating the position of the sensor. Update would include a two channel high resolution
 * ADC for measuring this voltage.
 * @return float, output of voltage reference
 */
double getRefVoltage() {
	float v = 0;
	for (int i = 0; i < NUMSAMPLES; i++) {
		v += analogRead(REF_V);
		Sleepy::loseSomeTime(3);
	}
	// convert analog reading into actual voltage
	v = v / NUMSAMPLES;
	v = 3.3 * v / 1023.0; // Converts 10bit value to voltage
	return v;             // return value
}

/**
 * Get the output of the external ADC. Reads the sensor
 * @return long, output of sensor. 0-32767
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
    DEBUGln(err);
  }
  else {
    DEBUG("Value: ");
    DEBUGln(value);
  }
	return value;
}

/**
 * Saves Unix timestamp to EEPROM
 * @param  t the value to store, perferably actually the unix timestamp
 * @return   boolean confirmation
 */
bool saveEEPROMTime(uint32_t t) {
	if(EEPROM.put(1, t)) return 1;
	else return 0;
}

/**
 * Reads the Unix Timestamp from EEEPROM
 * @return long, the Timestamp...
 */
uint32_t getEEPROMTime(){
	TimeStamp storedTime;
	storedTime = EEPROM.get(1, storedTime);
	return storedTime.timestamp;
}

/**
 * Calculates temperature using the Measured reference voltage (global var)
 * and the resistance of a Thermistor using the Steinhart equation
 * @return float, the Temperature
 */
double getTemperature()
{
	float ADC_reading = 0;
	float Vcc = 3.3;
	// float Ve = 2.048;
	float Ve = thePayload.ref_v;

	for(int i = 0; i < NUMSAMPLES; i++) {
		ADC_reading += analogRead(TEMP);
		Sleepy::loseSomeTime(3);
	}
	ADC_reading /= NUMSAMPLES;
	float therm_res = (SERIESRESISTOR * Vcc * ADC_reading)/((1023*Ve)-(Vcc*ADC_reading));
	DEBUG("Thermistor Resistance = ") DEBUGln(therm_res);

	double steinhart;
	steinhart = therm_res / THERMISTORNOMINAL;     // (R/Ro)
	steinhart = log(steinhart);                  // ln(R/Ro)
	steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
	steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
	steinhart = 1.0 / steinhart;                 // Invert
	steinhart -= 273.15;                         // convert to C
	DEBUG("Temperature "); DEBUG(steinhart); DEBUGln(" *C");

	return steinhart;
}

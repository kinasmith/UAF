/*
TODO:
	* Currently the EEPROM is really front loaded. The time address is overwritten the most, and the high addresses will never be used. 
		figure out a way to balance the data load!

*/
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include "SHT2x.h" //https://github.com/misenso/SHT2x-Arduino-Library
#include "Sleepy.h" //https://github.com/kinasmith/Sleepy
// #include "LowPower.h"
#include "RFM69_ATC.h" //https://www.github.com/lowpowerlab/rfm69

#define NODEID 				7   //unique for each node on same network
#define NETWORKID 		101  //the same on all nodes that talk to each other
#define GATEWAYID 		0 	//The address of the datalogger
#define FREQUENCY 		RF69_433MHZ //frequency of radio
#define ATC_RSSI 			-70 //ideal Signal Strength of trasmission
#define ACK_WAIT_TIME	100 // # of ms to wait for an ack
#define ACK_RETRIES		10 // # of attempts before giving up
#define SERIAL_BAUD		9600 // Serial comms rate
#define LED 					8 // UI LED

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

ISR(WDT_vect) { Sleepy::watchdogEvent(); } //watchdog for sleeping timer

RFM69_ATC radio; //init radio

//Data structure for transmitting the Timestamp from datalogger to sensor (4 bytes)
typedef struct TimeStamp {
    uint32_t timestamp;
};
TimeStamp theTimeStamp; //creates global instantiation of this

//Data structure for storing data in EEPROM (6 bytes)
typedef struct Data {
	uint16_t count = 0;
	int16_t temp = 0;  
	int16_t humidity = 0;
	int16_t voltage = 0;
};

//Data Structure for transmitting data packets to datalogger (10 bytes)
typedef struct Payload {
	uint16_t count;
	uint32_t timestamp;
	int16_t temp;  
	int16_t humidity;
	int16_t voltage;
};
Payload thePayload;

const uint8_t BAT_EN = 3;
const uint8_t REC_MIN = 3; //record interval in minutes
const uint16_t REC_INTERVAL = (60000*REC_MIN)/1000; //record interval in seconds
bool LED_STATE;
bool HANDSHAKE_SENT;
uint16_t EEPROM_ADDR = 4;
uint8_t attempt_cnt = 0;
uint16_t count = 0;

void setup() {
	Serial.begin(SERIAL_BAUD);
	Wire.begin();
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower(); 
	radio.encrypt(null);
	radio.enableAutoPower(ATC_RSSI); //Test to see if this is actually working at some point
	pinMode(BAT_EN, OUTPUT);
	pinMode(LED, OUTPUT);
	randomSeed(analogRead(0)); //set random seed
	//Ping the datalogger. If it's alive, it will return the current time. If not, wait and try again.
	digitalWrite(LED, HIGH); //write LED high to signal attempting connection
	while(!getTime()) { //this saves time to the struct which holds the time globally
		radio.sleep();
		DEBUGFlush();
		Sleepy::loseSomeTime(10000);	 //wait 10 sec bit before pinging the datalogger again
	}
	digitalWrite(LED, LOW); //write low to signal success
	DEBUG("=== Setup Done. Time is: "); DEBUG(theTimeStamp.timestamp); DEBUGln(" ===");	
}

void loop() {
	// Payload thePayload; //Store the data to be transmitted in this struct
	DEBUG("sleep - sleeping for "); DEBUG(REC_INTERVAL); DEBUG(" seconds"); DEBUGln(); 
	DEBUGFlush();
	radio.sleep();
	count++;
	for(int i = 0; i < REC_MIN; i++) 
		Sleepy::loseSomeTime(60000);
	if(!getTime()) { //Ping Datalogger,
		// if there is no response. Take readings, save readings to EEPROM. 
		DEBUGln("data - no response, saving data locally");
		thePayload.count = count;
		thePayload.humidity = getHumidity();
		thePayload.temp = getTemp();
		thePayload.voltage = getBatVoltage();
		writeDataToEEPROM(); //save that data to EEPROM
		Blink(50);
		Blink(50);
	} else {
		//Check to see if there is data waiting to be sent
		if(byte testByte = EEPROM.read(4) > 0) { //check the first byte of data, if there is data send all of it
			DEBUGln("eeprom - sending stored data");
			sendStoredEEPROMData();
		} 
		if(byte testByte = EEPROM.read(4) == 0) { //Check again if there is no data, just take some readings and send them
			thePayload.timestamp = theTimeStamp.timestamp; //this timestamp is updated whenever calling getTime(). 
			thePayload.count = count;
			thePayload.humidity = getHumidity();
			thePayload.temp = getTemp();
			thePayload.voltage = getBatVoltage();
			//Note the ACK retry and wait times. Very important, and much slower for 433mhz radios that are doing stuff too
			LED_STATE = true;
			digitalWrite(LED, LED_STATE);
			if (radio.sendWithRetry(GATEWAYID, (const void*)(&thePayload), sizeof(thePayload)), ACK_RETRIES, ACK_WAIT_TIME) {
				DEBUG("data - snd - "); DEBUG('[');DEBUG(GATEWAYID);DEBUG("] ");
				DEBUG(sizeof(thePayload)); DEBUGln(" bytes  ");
				LED_STATE = false;
				digitalWrite(LED, LED_STATE);
			} else {
				DEBUGln("data - snd - Failed . . . no ack");
				writeDataToEEPROM(); //if data fails to transfer, Save that data to eeprom to be sent later
				Blink(50);
				Blink(50);
			} 
		}
	}
}

void sendStoredEEPROMData() {
	Data blank; //init blank struct to erase data
	Data theData; //struct to save data in
	/*
	The stored time in EEPROM is one record interval behind the actual time that the data was stored 
	because time stamps are saved with successful transmissions and EEPROM data is stored when 
	those transmissions don't work
	*/
	uint8_t storeIndex = 1; 
	uint32_t eep_time = 0UL;
	EEPROM.get(0, eep_time); //get previously stored time (at address 0)
	EEPROM_ADDR = 0 + sizeof(theTimeStamp.timestamp); //Set to next address

	EEPROM.get(EEPROM_ADDR, theData); //Read in saved data to the Data struct
	while (theData.humidity > 0) { //while there is data available from the EEPROM 
		/*
			It could be any of the values in the struct because remember they are stored as int's, not floats. 
			But humidity will never be below 0.01%
		*/
		uint32_t rec_time = eep_time + REC_INTERVAL*storeIndex; //Calculate the actual recorded time
		//Save data into the Payload struct
		thePayload.timestamp = rec_time;
		thePayload.temp = theData.temp;
		thePayload.humidity = theData.humidity;
		thePayload.voltage = theData.voltage;
		thePayload.count = theData.count;

		// DEBUG("eeprom - time data was recorded is "); DEBUGln(thePayload.timestamp);
		DEBUG("eeprom - data @ addr "); DEBUG(EEPROM_ADDR); 
		//Send data to datalogger
		digitalWrite(LED, HIGH);
		if (radio.sendWithRetry(GATEWAYID, (const void*)(&thePayload), sizeof(thePayload)), ACK_RETRIES, ACK_WAIT_TIME) {
			DEBUG(" snd - ");
			DEBUG(sizeof(thePayload)); DEBUGln(" bytes  ");
			EEPROM.put(EEPROM_ADDR, blank); //If successfully sent, erase that data chunk...
			EEPROM_ADDR += sizeof(theData); //increment to next...
			EEPROM.get(EEPROM_ADDR, theData); // and read in the next saved data...
			storeIndex += 1; //and increment the count of saved values
			digitalWrite(LED, LOW);
		} else {
												//--ALL OF THIS MIGHT OR MIGHT NOT EVEN WORK!--
			attempt_cnt ++;
			DEBUG(" failed . . . no ack");
			DEBUG(attempt_cnt);
			Blink(50);
			Blink(50);
			if(attempt_cnt >10) {
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
void writeDataToEEPROM() {
	Data theData; //Data struct to store data to be written to EEPROM
	uint32_t eep_time = 0UL;
	//pull data from Payload to save there
	theData.temp = thePayload.temp;
	theData.humidity = thePayload.humidity;
	theData.voltage = thePayload.voltage;
	theData.count = thePayload.count;
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
				theTimeStamp = *(TimeStamp*)radio.DATA;  //save data
				DEBUG(" rcv - ");DEBUG('[');DEBUG(radio.SENDERID);DEBUG("] ");
				DEBUG(theTimeStamp.timestamp);
				DEBUG(" [RX_RSSI:");DEBUG(radio.RSSI);DEBUG("]");
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
int16_t getBatVoltage() {
	/*
	Gets battery voltage from sensor. Does 10x average on the analog reading
	*/	
	uint8_t BAT = A0;
	uint16_t readings = 0;
	digitalWrite(BAT_EN, HIGH);
	delay(10);
	for (byte i=0; i<3; i++)
		readings += analogRead(BAT);
	readings /= 3;
	float v = 3.3 * (readings/1023.0) * (4300.0/2700.0); //Calculate battery voltage
	digitalWrite(BAT_EN, LOW);
	return int(v *100);
}
int16_t getTemp() {
	float t = SHT2x.GetTemperature();
	return int(t*100);
}
int16_t getHumidity() {
	float h = SHT2x.GetHumidity();
	return int(h*100);	
}

void Blink(uint8_t t) {
	LED_STATE = !LED_STATE;
	digitalWrite(LED, LED_STATE);
	delay(t);
	LED_STATE = !LED_STATE;
	digitalWrite(LED, LED_STATE);
	delay(t);
}


			// DEBUG("data - "); DEBUG(thePayload.timestamp);
			// DEBUG(" t:"); DEBUG(thePayload.temp);
			// DEBUG(" h:"); DEBUG(thePayload.humidity);
			// DEBUG(" v:"); DEBUG(thePayload.voltage);
			// DEBUGln();
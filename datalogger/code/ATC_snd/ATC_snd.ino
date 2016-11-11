#include <RFM69_ATC.h>//get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>      //comes with Arduino IDE (www.arduino.cc)
#include <Wire.h>     //comes with Arduino
#include <EEPROM.h>
#include "SHT2x.h"
#include "jeelib-sleepy.h"

#define NODEID 				77    //unique for each node on same network
#define NETWORKID 		100  //the same on all nodes that talk to each other
#define GATEWAYID 		0
#define FREQUENCY 		RF69_433MHZ
#define ATC_RSSI 			-90
#define ACK_TIME 			30  // max # of ms to wait for an ack
#define ACK_WAIT_TIME	100 // # of ms to wait for an ack
#define ACK_RETRIES		5
#define SERIAL_BAUD		9600
#define LED 					9
#define SERIAL_EN

#define BATT_FORMULA(reading) 3.3 * (reading/1023.0) * (4300.0/2700.0);

#ifdef SERIAL_EN
#define DEBUG(input)   {Serial.print(input); delay(1);}
#define DEBUGln(input) {Serial.println(input); delay(1);}
#define DEBUGFlush() { Serial.flush(); }
#else
#define DEBUG(input);
#define DEBUGln(input);
#define DEBUGFlush();
#endif

ISR(WDT_vect) { 
	Sleepy::watchdogEvent(); 
}

RFM69_ATC radio;

typedef struct TimeStamp {
    uint32_t timestamp;
};
TimeStamp theTimeStamp;

typedef struct Data {
	int16_t temp = 0;  
	int16_t humidity = 0;
	int16_t voltage = 0;
};

typedef struct Payload {
	uint32_t timestamp;
	int16_t temp;  
	int16_t humidity;
	int16_t voltage;
};
Payload thePayload;


boolean sentMessage = false;
uint8_t bat_en = 3;
uint8_t bat = A0;
uint32_t eep_time = 0UL;
uint8_t nSends = 0;
uint8_t timeUpdateFreq = 60;
uint16_t EEPROM_ADDR = 4;
uint16_t rec_interval = 30; //record interval in seconds

void setup() {
	Serial.begin(SERIAL_BAUD);
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower(); 
	radio.encrypt(null);
	radio.enableAutoPower(ATC_RSSI);
	radio.sleep();
	pinMode(bat_en, OUTPUT);
	pinMode(LED, OUTPUT);
	//Ping the datalogger. If it's alive, it will return the current time. If not, wait 5 seconds and try again.
	digitalWrite(LED, HIGH); //write LED high to signal attempting connection
	while(!getTime()) { //this saves time to the struct which holds the time globally
		DEBUGFlush();
		Sleepy::loseSomeTime(5000);	 //wait a little bit before sending a second packet of data
	}
	digitalWrite(LED, LOW); //write low to signal success
	DEBUG("=== Setup Done. Time is: "); DEBUG(theTimeStamp.timestamp); DEBUGln(" ===");
}

void loop() {
	DEBUGln();
	DEBUGFlush();
	Sleepy::loseSomeTime(rec_interval * 1000); //this is the major sensor reading delay
	// for(int i = 0; i < 5; i++) 
		// Sleepy::loseSomeTime(60000);
	//Ping the datalogger again.
	if(!getTime()) { //if there is no response. Take readings, save readings to EEPROM. 
		DEBUGln("--- No response, saving data locally");
		thePayload.temp = random(1000,4000);
		thePayload.humidity = random(1000,4000);
		thePayload.voltage = random(1000,4000);
		writeDataToEEPROM();
	} else { //if there is a response
		//read index 4 of the eeprom to see if there is data waiting to be sent
		if(byte testByte = EEPROM.read(4) > 0) { //if there is data...loop through it and send it
			DEBUGln("eeprom - sending stored data");
			sendStoredEEPROMData();
		} 
		if(byte testByte = EEPROM.read(4) == 0) { //if there is no data.....don't worry about it, man
			thePayload.timestamp = theTimeStamp.timestamp; //this should still 
			thePayload.temp = random(1000,4000);
			thePayload.humidity = random(1000,4000);
			thePayload.voltage = random(1000,4000);

			DEBUG("data - "); DEBUG(thePayload.timestamp);
			DEBUG(" t:"); DEBUG(thePayload.temp);
			DEBUG(" h:"); DEBUG(thePayload.humidity);
			DEBUG(" v:"); DEBUG(thePayload.voltage);
			DEBUGln();
			//HAVING ISSUES WITH GETTING ACK'S ON THIS SEND
			if (radio.sendWithRetry(GATEWAYID, (const void*)(&thePayload), sizeof(thePayload)), 10, 100) {
				DEBUG("data - snd - "); Serial.print('[');Serial.print(GATEWAYID);Serial.print("] ");
				DEBUG(sizeof(thePayload)); DEBUGln(" bytes  ");
			} else {
				DEBUGln("data - snd - Failed . . . no ack");
				writeDataToEEPROM(); //if it fails to send data, save it to send later
			} 
		}
	}
}

void takeReadingAndSend() {

}

void sendStoredEEPROMData() {
	Data blank; //init blank struct to erase data
	Data theData;
	uint8_t storeIndex = 1; //stored time is one rec interval behind actual time data was stored because time stamps are saved with successful transmissions	
	//loop through stored data, setting all to 0. 
	//Get time of the first stored value
	EEPROM.get(0, eep_time);
	// DEBUG("eepRead - The saved time is "); DEBUGln(eep_time);
	EEPROM_ADDR = 0 + sizeof(theTimeStamp.timestamp); //Set INDEX to beginning of Data
	
	EEPROM.get(EEPROM_ADDR, theData); //read in saved data
	while (theData.temp > 0) { //check if it's greater than 0
		uint32_t rec_time = eep_time + rec_interval*storeIndex; //calc the actual time of recorded sample from saved initial time
		//Send data to Logger
		thePayload.timestamp = rec_time; //this should still 
		thePayload.temp = theData.temp;
		thePayload.humidity = theData.humidity;
		thePayload.voltage = theData.voltage;

		DEBUG("eeprom - time data was recorded is "); DEBUGln(thePayload.timestamp);
		DEBUG("eeprom - data @ addr "); DEBUG(EEPROM_ADDR); DEBUG(" is "); 
		DEBUG("t:"); DEBUG(thePayload.temp);
		DEBUG(" h:"); DEBUG(thePayload.humidity);
		DEBUG(" v:"); DEBUG(thePayload.voltage);
		DEBUGln();

		if (radio.sendWithRetry(GATEWAYID, (const void*)(&thePayload), sizeof(thePayload)), 10, 100) {
			DEBUG("eeprom - snd - "); Serial.print('[');Serial.print(GATEWAYID);Serial.print("] ");
			DEBUG(sizeof(thePayload)); DEBUGln(" bytes  ");
			EEPROM.put(EEPROM_ADDR, blank); //erase that data
			EEPROM_ADDR += sizeof(theData); //increment to next...
			EEPROM.get(EEPROM_ADDR, theData); //read in the next saved data
			storeIndex += 1;
		} else {
			DEBUGln("eeprom - snd - Failed . . . no ack"); //if it fails....just ignore that data point. ..THIS IS BAD
		}
	}
	EEPROM_ADDR = 0 + sizeof(theTimeStamp.timestamp); //Set Index to beginning of DATA
}

void writeDataToEEPROM() {
	Data theData;
	theData.temp = thePayload.temp;
	theData.humidity = thePayload.humidity;
	theData.voltage = thePayload.voltage;
	//check if the eeprom saved time is != to the current stored time. Update if it is different. 
	//if the timestamp is stored in the struct at every successful transmission 
	EEPROM.get(0, eep_time);
	if(theTimeStamp.timestamp != eep_time) {
		// DEBUGln("eeprom - UPDATE EEPROM TIME")
		EEPROM.put(0, theTimeStamp.timestamp);
	}
	//check if there is room to record the next reading
	if(EEPROM_ADDR < EEPROM.length() - sizeof(theData)) { 
		DEBUG("eeprom - saving "); DEBUG(sizeof(theData)); DEBUG(" bytes to address "); DEBUGln(EEPROM_ADDR);
		EEPROM.put(EEPROM_ADDR, theData); //write data
		EEPROM_ADDR += sizeof(theData); //increment to next index
	} else DEBUGln("eeprom - FULL!!!! NO LONGER SAVING DATA");
}

boolean getTime() {
	boolean messageRcv = false;
	if(!sentMessage) {
		if (radio.sendWithRetry(GATEWAYID, "t", 1)) {
			DEBUG("time - snd . . "); 
			sentMessage = true;
		}
		else {
			DEBUGln("time - snd - Failed . . . no ack");
			return false;
		}
	}
	while(!messageRcv && sentMessage) {
		if (radio.receiveDone()) {
			if (radio.DATALEN == sizeof(theTimeStamp)) {
				theTimeStamp = *(TimeStamp*)radio.DATA; 
				DEBUG(" rcv - ");DEBUG('[');DEBUG(radio.SENDERID);DEBUG("] ");
				DEBUG(theTimeStamp.timestamp);
				DEBUG(" [RX_RSSI:");DEBUG(radio.RSSI);DEBUG("]");
				DEBUGln();
				messageRcv = true;
			}
			if (radio.ACKRequested()) radio.sendACK();
		}
	}
	sentMessage = false;
	return true;
}

int16_t getBatVoltage() {
	digitalWrite(bat_en, HIGH);
	delay(10);
	uint16_t readings;
	for (byte i=0; i<10; i++)
		readings+=analogRead(bat);
	uint16_t v = BATT_FORMULA(readings / 10.0);
	digitalWrite(bat_en, LOW);

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
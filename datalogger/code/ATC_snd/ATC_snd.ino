#include <RFM69_ATC.h>//get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>      //comes with Arduino IDE (www.arduino.cc)
#include <Wire.h>     //comes with Arduino
#include <EEPROM.h>
#include "SHT2x.h"
#include "jeelib-sleepy.h"

//*********************************************************************************************
//************ IMPORTANT SETTINGS - YOU MUST CHANGE/CONFIGURE TO FIT YOUR HARDWARE *************
//*********************************************************************************************
#define NODEID        88    //unique for each node on same network
#define NETWORKID     100  //the same on all nodes that talk to each other
#define GATEWAYID     0
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
#define FREQUENCY     RF69_433MHZ
#define ATC_RSSI      -90
//*********************************************************************************************
#define ACK_TIME      30  // max # of ms to wait for an ack
#define ACK_WAIT_TIME	100 // # of ms to wait for an ack
#define ACK_RETRIES		5

#define SERIAL_EN             //comment this out when deploying to an installed Mote to save a few KB of sketch size
#define SERIAL_BAUD    9600

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
boolean sentMessage = false;

typedef struct TimeStamp {
    uint32_t timestamp;
};
TimeStamp theTimeStamp;

typedef struct Payload {
	int16_t temp;  
	int16_t humidity;
	int16_t voltage;
};
Payload theData;

uint8_t bat_en = 3;
uint8_t bat = A0;
uint32_t eep_time = 0UL;
uint8_t nSends = 0;
uint8_t timeUpdateFreq = 60;

void setup() {
	// for (int i = 0 ; i < EEPROM.length() ; i++) {
	// 	EEPROM.write(i, 0);
	// }
	Serial.begin(SERIAL_BAUD);
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower(); 
	radio.encrypt(null);
	radio.enableAutoPower(ATC_RSSI);
	radio.sleep();
	pinMode(bat_en, OUTPUT);
}

void loop() {
	if(nSends % timeUpdateFreq == 0) {
		if(getTime()) {
			DEBUG("rcv - ");DEBUG('[');DEBUG(radio.SENDERID);DEBUG("] ");
			DEBUG(theTimeStamp.timestamp);
			DEBUG("   [RX_RSSI:");DEBUG(radio.RSSI);DEBUG("]");
			DEBUGln();
			EEPROM.update(0, theTimeStamp.timestamp); //only updates changed bits. prolongs life of eeprom			
		}
	DEBUGFlush();
	Sleepy::loseSomeTime(500);	 //wait a little bit before sending a second packet of data
	}	

	theData.temp = getTemp(); 
	theData.humidity = getHumidity();
	theData.voltage = getBatVoltage();

	if (radio.sendWithRetry(GATEWAYID, (const void*)(&theData), sizeof(theData), ACK_RETRIES, ACK_WAIT_TIME)) {
		nSends++;
		DEBUG("snd - "); Serial.print('[');Serial.print(GATEWAYID);Serial.print("] ");
		DEBUG(sizeof(theData)); DEBUG(" bytes  ");
		EEPROM.get(0, eep_time);
		DEBUG("eep - "); DEBUGln(eep_time);
	} else DEBUG("snd - Failed . . . no ack");
		
	DEBUGFlush();
	radio.sleep();
	// for(int i = 0; i < 5; i++) 
		// Sleepy::loseSomeTime(60000);
	Sleepy::loseSomeTime(4000);
}

boolean getTime() {
	boolean messageRcv = false;
	if(!sentMessage) {
		if (radio.sendWithRetry(GATEWAYID, "t", 1)) {
			// DEBUG("snd - "); Serial.print('[');Serial.print(GATEWAYID);Serial.print("] ");
			// DEBUGln("t");
			sentMessage = true;
		}
		else {
			DEBUGln("snd - Failed . . . no ack");
			return false;
		}
	}
	while(!messageRcv && sentMessage) {
		if (radio.receiveDone()) {
			if (radio.DATALEN == sizeof(theTimeStamp)) {
				theTimeStamp = *(TimeStamp*)radio.DATA; 
				// DEBUG("rcv - ");DEBUG('[');DEBUG(radio.SENDERID);DEBUG("] ");
				// DEBUG(theTimeStamp.timestamp);
				// DEBUG("   [RX_RSSI:");DEBUG(radio.RSSI);DEBUG("]");
				// DEBUGln();
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
	float v_bat = analogRead(bat);
	float v = 3.3 * (v_bat/1023.0) * (4300.0/2700.0);
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
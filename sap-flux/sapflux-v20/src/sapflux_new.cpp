#include "SPI.h"
#include "Arduino.h"
#include "RFM69.h"

float getBatteryVoltage(uint8_t pin, uint8_t en);
bool getTime();

#define SERIAL_EN

#ifdef SERIAL_EN
#define DEBUG(input)   {Serial.print(input); delay(1);}
#define DEBUGln(input) {Serial.println(input); delay(1);}
#define DEBUGFlush() { Serial.flush(); }
#else
#define DEBUG(input);
#define DEBUGln(input);
#define DEBUGFlush();
#endif


#define NODEID 				91   //unique for each node on same network
#define NETWORKID 		100  //the same on all nodes that talk to each other
#define GATEWAYID 		2 	//The address of the datalogger
#define FREQUENCY 		RF69_433MHZ //frequency of radio
#define ATC_RSSI 			-70 //ideal Signal Strength of trasmission
#define ACK_WAIT_TIME	100 // # of ms to wait for an ack
#define ACK_RETRIES		10 // # of attempts before giving up

const uint8_t LED = A0;
const uint8_t N_SEL_1 = A1;
const uint8_t N_SEL_2 = A2;
const uint8_t N_SEL_4 = A3;
const uint8_t BAT_V = A7;

const uint8_t BAT_V_EN = 3;
const uint8_t HEATER_EN = 4;
const uint8_t TC3_CS = 5;
const uint8_t TC2_CS = 6;
const uint8_t TC1_CS = 7;
const uint8_t FLASH_CS = 8;
const uint8_t LED_TX = 9;
const uint8_t RFM_CS = 10;
bool LED_STATE;
bool HANDSHAKE_SENT;

//Data structure for transmitting the Timestamp from datalogger to sensor (4 bytes)
struct TimeStamp {
    uint32_t timestamp;
};
TimeStamp theTimeStamp; //creates global instantiation of this

RFM69 radio; //init radio

void setup() {
  Serial.begin(9600);
  Serial.println("begin");
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  Serial.begin("radio Good");
	radio.setHighPower();
	radio.encrypt(null);
}

void loop() {
  delay(1000);
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


/**
 * --->THIS ISN'T WORKING RIGHT. CHECK THE MATH<---
 * [getBatteryVoltage description]
 * @param  pin [description]
 * @param  en  [description]
 * @return     [description]
 */
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
  Serial.println(v);
  return v;
}

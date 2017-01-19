#include "Arduino.h"
#include "SPI.h"
#include "RFM69_ATC.h"

#define NODEID 10
#define NETWORKID 100
#define GATEWAYID 0
#define FREQUENCY RF69_433MHZ //frequency of radio
#define ATC_RSSI -70 //ideal Signal Strength of trasmission
#define ACK_WAIT_TIME 100 // # of ms to wait for an ack
#define ACK_RETRIES 10 // # of attempts before giving up
#define SERIAL_BAUD 9600 // Serial comms rate
#define LED 9 // UI LED

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

bool getTime();
int16_t getBatVoltage();
void Blink(uint8_t);

RFM69_ATC radio; //init radio

//Data structure for transmitting the Timestamp from datalogger to sensor (4 bytes)
struct TimeStamp {
	uint32_t timestamp;
};
TimeStamp theTimeStamp; //creates global instantiation of this

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
  DEBUGln("--Testing Radio--");
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
  DEBUGln("radio init . . . OK")
	radio.setHighPower();
  DEBUGln("set High Power");
	radio.encrypt(null);
  DEBUGln("No Encryption")
	radio.enableAutoPower(ATC_RSSI); //Test to see if this is actually working at some point
  DEBUGln("Auto Power Enabled");
	pinMode(LED, OUTPUT);
	randomSeed(analogRead(A4)); //set random seed
	//Ping the datalogger. If it's alive, it will return the current time. If not, wait and try again.
	digitalWrite(LED, HIGH); //write LED high to signal attempting connection
  DEBUGln("Getting Time Stamp");
	while(!getTime()) { //this saves time to the struct which holds the time globally
		radio.sleep();
		DEBUGFlush();
		delay(10000);
	}
	digitalWrite(LED, LOW); //write low to signal success
	DEBUG("=== Setup Done. Time is: "); DEBUG(theTimeStamp.timestamp); DEBUGln(" ===");
}

void loop() {
  Blink(255);
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

void Blink(uint8_t t) {
	LED_STATE = !LED_STATE;
	digitalWrite(LED, LED_STATE);
	delay(t);
	LED_STATE = !LED_STATE;
	digitalWrite(LED, LED_STATE);
	delay(t);
}

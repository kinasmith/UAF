#include <Arduino.h>
#include <RFM69_ATC.h>//get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>

#define NODEID 0
#define FREQUENCY RF69_433MHZ
#define ATC_RSSI -70
#define ACK_WAIT_TIME 100 // # of ms to wait for an ack
#define ACK_RETRIES 10 // # of attempts before giving up
#define SERIAL_BAUD 115200
#define LED 3
#define SD_CS_PIN 4
#define CARD_DETECT 5

/*==============|| RFM69 ||==============*/
RFM69_ATC radio; //Initialize Radio
uint8_t NETWORKID; //base network address

struct Payload {
	double tc1 = 0;
	double tc2 = 0;
	double tc3 = 0;
	double tc1_int = 0;
	double tc2_int = 0;
	double tc3_int = 0;
};
Payload thePayload;

void setup() {
	Serial.begin(SERIAL_BAUD);
	pinMode(LED, OUTPUT);
	pinMode(CARD_DETECT, INPUT_PULLUP);
	NETWORKID = 100;
	//*****
	// rtc.adjust(DateTime((__DATE__), (__TIME__))); //sets the RTC to the computer time.
	//*****
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower(); //only for RFM69HW!
	radio.enableAutoPower(ATC_RSSI);
	radio.encrypt(null);
	digitalWrite(LED, LOW);
}

void loop() {
	if (radio.receiveDone()) {
		Serial.println(radio.SENDERID);

		if (radio.DATALEN == sizeof(thePayload)) {
			thePayload = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
			Serial.print(" t1:"); Serial.print(thePayload.tc1);
			Serial.print(" t2:"); Serial.print(thePayload.tc2);
			Serial.print(" t3:"); Serial.print(thePayload.tc3);
			Serial.println();
		}
		if (radio.ACKRequested()) radio.sendACK();
	}
}

#include <Arduino.h>
#include <SPI.h>
#include <MAX31856.h>
#include "RFM69_ATC.h"

#define TC1_CS 7
#define TC2_CS 6
#define TC3_CS 5
#define HEATER_EN 4
#define LED 9

#define NODEID 233
#define GATEWAYID 0
#define FREQUENCY RF69_433MHZ //frequency of radio
#define ATC_RSSI -70 //ideal Signal Strength of trasmission
// #define ACK_WAIT_TIME 100 // # of ms to wait for an ack
#define ACK_RETRIES 10 // # of attempts before giving up
int ACK_WAIT_TIME = 100;
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

/*==============|| RFM69 ||==============*/
RFM69_ATC radio;
uint8_t attempt_cnt = 0;
uint8_t NETWORKID = 100;



MAX31856 tc1(TC1_CS, T_TYPE, CUTOFF_60HZ, AVG_SEL_1SAMP, CMODE_OFF, ONESHOT_ON); //constant conversion mode
MAX31856 tc2(TC2_CS, T_TYPE, CUTOFF_60HZ, AVG_SEL_4SAMP, CMODE_OFF, ONESHOT_ON); //one shot mode
MAX31856 tc3(TC3_CS, T_TYPE, CUTOFF_60HZ, AVG_SEL_4SAMP, CMODE_OFF, ONESHOT_ON); //one shot mode

struct Measurement {
	double tc1 = 0;
	double tc2 = 0;
	double tc3 = 0;
	double tc1_int = 0;
	double tc2_int = 0;
	double tc3_int = 0;
};

int cnt = 0;
int heaterState;

void setup() {
	Serial.begin(9600);
	pinMode(HEATER_EN, OUTPUT);
	pinMode(LED, OUTPUT);
  DEBUGln("START--");
	delay(100);
}

void loop() {
  Measurement thisMeasurement;
  tc1.prime();
  tc2.prime();
  tc3.prime(); //start reading
  tc1.read();
  tc2.read();
  tc3.read(); //read values
  thisMeasurement.tc1 = tc1.getExternal();
  thisMeasurement.tc2 = tc2.getExternal();
  thisMeasurement.tc3 = tc3.getExternal();
  thisMeasurement.tc1_int = tc1.getInternal();
	thisMeasurement.tc2_int = tc2.getInternal();
	thisMeasurement.tc3_int = tc3.getInternal();

	DEBUG("Internal,");
	DEBUG(thisMeasurement.tc1_int); DEBUG(",");
	DEBUG(thisMeasurement.tc2_int); DEBUG(",");
	DEBUG(thisMeasurement.tc3_int); DEBUG(",");

	DEBUG("External,");
	DEBUG(thisMeasurement.tc1); DEBUG(",");
  DEBUG(thisMeasurement.tc2); DEBUG(",");
  DEBUG(thisMeasurement.tc3); DEBUG(",");
	DEBUG(heaterState); DEBUG(",")
	DEBUGln();

	if(cnt < 6) {
		digitalWrite(HEATER_EN, HIGH);
		digitalWrite(LED, HIGH);
		heaterState = 1;
	} else {
		digitalWrite(HEATER_EN, LOW);
		digitalWrite(LED, LOW);
		heaterState = 0;
	}
	if(cnt > 60) cnt = 0;
	cnt++;

	if(radio.sendWithRetry(GATEWAYID, (const void*)(&thisMeasurement), sizeof(thisMeasurement))) {
		DEBUGln("success");
	} else {DEBUGln("Failure")};
	delay(1000);
}

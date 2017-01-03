#include <Wire.h>
#include <SPI.h>
#include "SHT2x.h"
#include "RFM69.h"
#include "jeelib-sleepy.h"

#define NETWORKID		101
#define NODEID 				7
#define GATEWAYID			0
#define FREQUENCY 		RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define SERIAL_BAUD		9600
#define ACK_WAIT_TIME	100 // # of ms to wait for an ack
#define ACK_RETRIES		5
#define debug 				0

ISR(WDT_vect) { 
	Sleepy::watchdogEvent(); 
}

RFM69 radio;

typedef struct {
	int16_t temp;  
	int16_t humidity;
	int16_t voltage;
} Payload;
Payload theData;

int bat_en = 3;
int bat = A0;

boolean gotTime = false;

void setup() {
	pinMode(bat_en, OUTPUT);
	if(debug) Serial.begin(9600);
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower();
	radio.encrypt(null);
	radio.sleep();
	if(debug) {
		char buff[50];
		sprintf(buff, "\nTransmitting at %d Mhz...", 433);
		Serial.println(buff);
	}
}

void loop() {
	digitalWrite(bat_en, HIGH);
	delay(10);
	float v_bat = analogRead(bat);
	float v = 3.3 * (v_bat/1023.0) * (4300.0/2700.0);
	digitalWrite(bat_en, LOW);
	float t = SHT2x.GetTemperature();
	float h = SHT2x.GetHumidity();
	int t_int = int(t * 100);
	int h_int = int(h*100);
	int v_int = int(v * 100);
	theData.temp = t_int; 
	theData.humidity = h_int;
	theData.voltage = v_int;
	if(debug) {
		Serial.print("Sending packet (");
		Serial.print(sizeof(theData));
		Serial.print(" bytes) ... ");
	}
	if (radio.sendWithRetry(GATEWAYID, (const void*)(&theData), sizeof(theData), ACK_RETRIES, ACK_WAIT_TIME)) {
		if(debug) Serial.println("ok");
	} else {
		if(debug) Serial.println("nothing");
	}
	if(debug) Serial.flush();
	radio.sleep();
	for(int i = 0; i < 5; i++) 
		Sleepy::loseSomeTime(60000);
}
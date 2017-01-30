#include <Arduino.h>
#include <SPI.h>
#include <MAX31856.h>

#define TC1_CS 7
#define TC2_CS 6
#define TC3_CS 5

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

MAX31856 tc1(TC1_CS, T_TYPE, CUTOFF_60HZ, AVG_SEL_1SAMP, CMODE_AUTO, ONESHOT_OFF); //constant conversion mode
MAX31856 tc2(TC2_CS, T_TYPE, CUTOFF_60HZ, AVG_SEL_4SAMP, CMODE_OFF, ONESHOT_ON); //one shot mode
MAX31856 tc3(TC3_CS, T_TYPE, CUTOFF_60HZ, AVG_SEL_4SAMP, CMODE_OFF, ONESHOT_ON); //one shot mode

struct Measurement {
	double tc1 = 0;
	double tc2 = 0;
	double tc3 = 0;
	double internal = 0;
};


void setup() {
  pinMode(10, OUTPUT);
	Serial.begin(9600);
  DEBUGln("START--");
	delay(100);
	DEBUGln("START 2 --");
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
  thisMeasurement.internal = tc1.getInternal();
  DEBUG("- ");
  DEBUG(thisMeasurement.tc1); DEBUG(", ");
  DEBUG(thisMeasurement.tc2); DEBUG(", ");
  DEBUG(thisMeasurement.tc3); DEBUG(", ");
  DEBUG(thisMeasurement.internal); DEBUGln();
	delay(1000);
}

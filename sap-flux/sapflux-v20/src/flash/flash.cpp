#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "SPIFlash_Marzogh.h"
#include <EEPROM.h>

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


/*==============|| FLASH ||==============*/
SPIFlash_Marzogh flash(8);
uint32_t FLASH_ADDR = 0;
uint16_t EEPROM_ADDR = 0;
/*==============|| DATA ||==============*/
//Data structure for transmitting the Timestamp from datalogger to sensor (4 bytes)
struct TimeStamp {
	uint32_t timestamp = 1485202570;
};
TimeStamp theTimeStamp; //creates global instantiation of this

//Data structure for storing data locally (12 bytes)
struct Data {
	uint16_t a;
	int16_t b;
	int16_t c;
	int16_t d;
	int16_t e;
	int16_t f;
};
Data d;

void setup() {
  randomSeed(analogRead(4));
  EEPROM_ADDR = random(0, 1023);
  theTimeStamp.timestamp = random(400, 30000);
  #ifdef SERIAL_EN
    Serial.begin(9600);
  #endif
	flash.begin();
  flash.eraseSector(0);

  DEBUGln("=== Writing Time ===");
  EEPROM.put(EEPROM_ADDR, theTimeStamp.timestamp);
  DEBUGln(theTimeStamp.timestamp);
  DEBUGln("=== Writing Data ===");

  for(int i = 0; i < 20; i++) {
    FLASH_ADDR = flash.getAddress(sizeof(d));
    d.a = random(100);
    d.b = random(100);
    d.c = random(100);
    d.d = random(100);
    d.e = random(100);
    d.f = random(100);
    if(flash.writeAnything(FLASH_ADDR, d)) {
      DEBUG("write - "); DEBUG(sizeof(d));
      DEBUG(" bytes at address "); DEBUG(FLASH_ADDR); DEBUG(" -> ");
      DEBUG(d.a); DEBUG(",");
      DEBUG(d.b); DEBUG(",");
      DEBUG(d.c); DEBUG(",");
      DEBUG(d.d); DEBUG(",");
      DEBUG(d.e); DEBUG(",");
      DEBUG(d.f); DEBUGln();
    }
  }
  delay(4000);

  TimeStamp storedTime;
  DEBUGln("=== Reading Time ===");
  EEPROM.get(EEPROM_ADDR, storedTime);
  DEBUGln(storedTime.timestamp);

  DEBUGln("=== Reading Data ===");
  Data f;
  FLASH_ADDR = 0;
  for(int i = 0; i < 20; i++) {
    if(flash.readAnything(FLASH_ADDR, f)) {
      DEBUG("read - "); DEBUG(sizeof(f));
      DEBUG(" bytes at address "); DEBUG(FLASH_ADDR); DEBUG(" -> ");
      DEBUG(f.a); DEBUG(",");
      DEBUG(f.b); DEBUG(",");
      DEBUG(f.c); DEBUG(",");
      DEBUG(f.d); DEBUG(",");
      DEBUG(f.e); DEBUG(",");
      DEBUG(f.f); DEBUGln();
      FLASH_ADDR += sizeof(f);
    }
  }
}

void loop() {

}

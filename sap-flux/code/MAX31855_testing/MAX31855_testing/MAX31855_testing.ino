#include <SPI.h>
#include "Adafruit_MAX31855.h"

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define DO   3
#define CS   4
#define CLK  5
Adafruit_MAX31855 thermocouple(CLK, CS, DO);

// Example creating a thermocouple instance with hardware SPI (Uno/Mega only)
// on a given CS pin.
//#define CS   10
//Adafruit_MAX31855 thermocouple(CS);

void setup() {
  Serial.begin(9600);
  
//  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
}

void loop() {
  // basic readout test, just print the current temp
//   Serial.print("Internal Temp = ");
//   Serial.println(thermocouple.readInternal());

   double c = thermocouple.readCelsius();
   if (isnan(c)) {
     Serial.println("Something wrong with thermocouple!");
   } else {
//     Serial.print("C = "); 
     Serial.println(c);
   }
//   Serial.print("F = ");
//   Serial.println(thermocouple.readFarenheit());
 
   delay(100);
}

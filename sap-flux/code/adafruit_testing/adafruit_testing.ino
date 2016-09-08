#include <SPI.h>
#include "Adafruit_MAX31855.h"
#define tc_1_CS   8
#define tc_2_CS   7
Adafruit_MAX31855 Tc_1(tc_1_CS);
Adafruit_MAX31855 Tc_2(tc_2_CS);

void setup() {
  Serial.begin(9600);
  delay(500);
}

void loop() {
  // basic readout test, just print the current temp
  Serial.print("Ref: ");
//  Serial.print(Tc_1.readInternal());

  float c = Tc_1.readCelsius();
  if (isnan(c)) {
    Serial.println("Something wrong with thermocouple!");
  } else {
//    Serial.print(" External: ");
    Serial.println(c);
  }
  Serial.print("Heater: ");
//  Serial.print(Tc_2.readInternal());

  c = Tc_2.readCelsius();
  if (isnan(c)) {
    Serial.println("Something wrong with thermocouple!");
  } else {
//    Serial.print(" External: ");
    Serial.println(c);
  }

  delay(1000);
}

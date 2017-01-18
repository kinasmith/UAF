#include "Arduino.h"

float getBatteryVoltage(uint8_t pin, uint8_t en);

const int LED = A0;
const int N_SEL_1 = A1;
const int N_SEL_2 = A2;
const int N_SEL_4 = A3;
const int BAT_V = A7;

const int BAT_V_EN = 3;
const int HEATER_EN = 4;
const int TC3_CS = 5;
const int TC2_CS = 6;
const int TC1_CS = 7;
const int FLASH_CS = 8;
const int LED_TX = 9;
const int RFM_CS = 10;

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println(getBatteryVoltage(BAT_V, BAT_V_EN));
  delay(1000);

}

/**
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
	float v = (3.3 * (readings/1023.0));// * (R23/R22); //Calculate battery voltage
  Serial.println(v);
  return v;
}

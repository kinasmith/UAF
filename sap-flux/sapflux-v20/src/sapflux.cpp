#include "Arduino.h"

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
  pinMode(LED_TX, OUTPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  digitalWrite(LED_TX, HIGH);
  // digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED_TX, LOW);
  // digitalWrite(LED, LOW);
  delay(100);
}

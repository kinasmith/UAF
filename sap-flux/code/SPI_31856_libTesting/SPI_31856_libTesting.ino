/**
  @file SerialThermometer.ino
  This is a simple serial port thermometer application using the Termopar Nanoshield.

  Copyright (c) 2015 Circuitar
  This software is released under the MIT license. See the attached LICENSE file for details.
*/
#include <SPI.h>
#include "Nanoshield_Termopar.h"

// Termopar Nanoshield on CS pin D8, type K thermocouple, no averaging
Nanoshield_Termopar Tc_1(5, TC_TYPE_T, TC_AVG_4_SAMPLES);
Nanoshield_Termopar Tc_2(6, TC_TYPE_T, TC_AVG_4_SAMPLES);
Nanoshield_Termopar Tc_3(7, TC_TYPE_T, TC_AVG_4_SAMPLES);
// Nanoshield_Termopar Tc_4(8, TC_TYPE_T, TC_AVG_4_SAMPLES);

const int H_en = 4;
long currentTime;
long h_saved_time;
long h_interval;

const long h_pulse_on = 6000;
const long h_pulse_off = 30 * 60000;
//const long h_pulse_off = 15000;
int h_status = 1;

long log_saved_time;
int log_interval = 1000;

int aTime = 5;

float Tc_1t = 0;
float Tc_2t = 0;
float Tc_3t = 0;
// float Tc_4t = 0;

void setup() {
  Serial.begin(9600);
  // Serial.println("---Tc---");
  Serial.println();

  Tc_1.begin();
  Tc_2.begin();
  Tc_3.begin();
  // Tc_4.begin();
  pinMode(H_en, OUTPUT);
  // digitalWrite(H_en, HIGH);
}

void loop() {
  currentTime = millis();
  if (h_saved_time + h_interval < currentTime) {
    //if heater is on, wait 6 seconds
    if (h_status == 0) { //if heat is off...turn it on
      digitalWrite(H_en, HIGH);
      h_interval = h_pulse_on;
      h_status = 1; //set status for next round.
      // Serial.print("Heat on for ");
      // Serial.print(h_pulse_on / 1000);
      // Serial.println("s");
    }
    else if (h_status == 1) { //if heat is on...turn it off
      digitalWrite(H_en, LOW);
      h_interval = h_pulse_off;
      h_status = 0; //set status for next round
      // Serial.print("Heat off for ");
      // Serial.print(h_pulse_off / 1000);
      // Serial.println("s");
    }
    h_saved_time = currentTime;
  }
  if (log_saved_time + log_interval < currentTime) {
    // Read thermocouple data
    Tc_1.read();
    Tc_2.read();
    Tc_3.read();
    // Tc_4.read();
    Serial.print("H");
    Serial.print(Tc_1.getExternal());
    Serial.print(",");
    Serial.print(Tc_2.getExternal());
    Serial.print(",");
    Serial.print(Tc_3.getExternal());
    // Serial.print(",");
    // Serial.print(Tc_4.getExternal());
    Serial.print(",");
    Serial.print(h_status);
    Serial.println();
    log_saved_time = currentTime;
  }
}


void printErrors() {
  if (Tc_1.isOpen()) {
    Serial.print("Tc_1: Open circuit");
  } else if (Tc_1.isOverUnderVoltage()) {
    Serial.print("Tc_1: Overvoltage/Undervoltage");
  } else if (Tc_1.isInternalOutOfRange()) {
    Serial.print("Tc_1: Internal temperature (cold junction) out of range)");
  } else if (Tc_1.isExternalOutOfRange()) {
    Serial.print("Tc_1: External temperature (hot junction) out of range");
  }
  if (Tc_2.isOpen()) {
    Serial.print("Tc_2: Open circuit");
  } else if (Tc_2.isOverUnderVoltage()) {
    Serial.print("Tc_2: Overvoltage/Undervoltage");
  } else if (Tc_2.isInternalOutOfRange()) {
    Serial.print("Tc_2: Internal temperature (cold junction) out of range)");
  } else if (Tc_2.isExternalOutOfRange()) {
    Serial.print("Tc_2: External temperature (hot junction) out of range");
  }
  if (Tc_3.isOpen()) {
    Serial.print("Tc_3: Open circuit");
  } else if (Tc_3.isOverUnderVoltage()) {
    Serial.print("Tc_3: Overvoltage/Undervoltage");
  } else if (Tc_3.isInternalOutOfRange()) {
    Serial.print("Tc_3: Internal temperature (cold junction) out of range)");
  } else if (Tc_3.isExternalOutOfRange()) {
    Serial.print("Tc_3: External temperature (hot junction) out of range");
  }
  // if (Tc_4.isOpen()) {
  //   Serial.print("Tc_4: Open circuit");
  // } else if (Tc_4.isOverUnderVoltage()) {
  //   Serial.print("Tc_4: Overvoltage/Undervoltage");
  // } else if (Tc_4.isInternalOutOfRange()) {
  //   Serial.print("ITc_4: nternal temperature (cold junction) out of range)");
  // } else if (Tc_4.isExternalOutOfRange()) {
  //   Serial.print("Tc_4: External temperature (hot junction) out of range");
  // }
}




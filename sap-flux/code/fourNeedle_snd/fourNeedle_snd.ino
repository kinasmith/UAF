/*
   Tc 1: CSpin 9
   Tc 2: CSpin 8
   Tc 3: CSpin 7
   Tc Heat: CSpin 6
   H_en: pin 5
*/

#include <SPI.h>
#include "RFM69.h"
#include "Adafruit_MAX31855.h"

#define NODE_ID         2
#define NETWORK_ID      105
#define GATEWAY_ID      0
#define FREQUENCY       RF69_433MHZ
#define KEY             "p6ZNvTmGfdY2hUXb" //has to be same 16 characters/bytes on all nodes, not more not less!
#define ACK_TIME        100  // # of ms to wait for an ack
#define NB_ATTEMPTS_ACK 5 //number of attempts to try before giving up

#define Tc1_CS 9
#define Tc2_CS 8
#define Tc3_CS 7
#define TcH_CS 6
#define H_en 5

Adafruit_MAX31855 Tc1(Tc1_CS);
Adafruit_MAX31855 Tc2(Tc2_CS);
Adafruit_MAX31855 Tc3(Tc3_CS);
Adafruit_MAX31855 TcH(TcH_CS);
RFM69 radio;

long TRANSMIT_PERIOD = 30000; //transmit a packet to gateway so often (in ms)
int TRANSMIT_PERIOD_MINUTES = 1;
int ACK_FAIL_WAIT_PERIOD = 500;


long currentTime;
long h_saved_time;
long h_interval;
const long h_pulse_on = 6000;
const long h_pulse_off = 30 * 60000;
//const long h_pulse_off = 60000;
int h_status = 1;

long log_saved_time;
int log_interval = 6000;

typedef struct {
  int node_id;
  float Tc1_t;
  float Tc2_t;
  float Tc3_t;
  float TcH_t;
}
Payload;
Payload payload;

void setup() {
  Serial.begin(9600);
  pinMode(H_en, OUTPUT);
  digitalWrite(H_en, LOW);
payload.node_id = NODE_ID;
  radio.initialize(FREQUENCY, NODE_ID, NETWORK_ID); //Begin Radio
  radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY); //Encrypt
  radio.sleep(); //Sleep radio (saves power)
  char buff[50];
  sprintf(buff, "Listening at %d Mhz...", FREQUENCY == RF69_433MHZ ? 433 : FREQUENCY == RF69_868MHZ ? 868 : 915);
}

void loop() {
  currentTime = millis();
  if (h_saved_time + h_interval < currentTime) {
    //if heater is on, wait 6 seconds
    if (h_status == 1) {
      digitalWrite(H_en, HIGH);
      h_interval = h_pulse_on;
      h_status = 0; //set status for next round.
      Serial.print("Heat interval ");
      Serial.print(h_pulse_on / 1000);
      Serial.println("s");
    }
    else if (h_status == 0) {
      digitalWrite(H_en, LOW);
      h_interval = h_pulse_off;
      h_status = 1; //set status for next round
      Serial.print("Heat interval ");
      Serial.print(h_pulse_off / 1000);
      Serial.println("s");
    }
    h_saved_time = currentTime;
  }
  if (log_saved_time + log_interval < currentTime) {
    payload.Tc1_t = Tc1.readCelsius();
    payload.Tc2_t = Tc2.readCelsius();
    payload.Tc3_t = Tc3.readCelsius();
    payload.TcH_t = TcH.readCelsius();
    sendPacket();
    printPacket();
    log_saved_time = currentTime;
  }
}

void sendPacket() {
  int nAttempt = 0; 
  bool flag_ACK_received = false;
  while (nAttempt < NB_ATTEMPTS_ACK && !flag_ACK_received) { 
    if (radio.sendWithRetry(GATEWAY_ID, (const void*)(&payload), sizeof(payload))) { 
      flag_ACK_received = true;
    }
    else { 
      if (radio.sendWithRetry(GATEWAY_ID, (const void*)(&payload), sizeof(payload))) {
        flag_ACK_received = true; 
      }
      nAttempt++;
      ACK_FAIL_WAIT_PERIOD = random(300, 600);
      delay(ACK_FAIL_WAIT_PERIOD);
    }
  }
}
void printPacket() {
  Serial.print(payload.Tc1_t);
  Serial.print(", ");
  Serial.print(payload.Tc2_t);
  Serial.print(", ");
  Serial.print(payload.Tc3_t);
  Serial.print(", ");
  Serial.print(payload.TcH_t);
  Serial.println();
}


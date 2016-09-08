#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include "RFM69.h"
#include "jeelib-sleepy.h"

#define NODE_ID         1
#define NETWORK_ID      105
#define GATEWAY_ID      0
#define FREQUENCY       RF69_433MHZ
#define KEY             "p6ZNvTmGfdY2hUXb" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED             9
#define SERIAL_BAUD     9600
#define ACK_TIME        100  // # of ms to wait for an ack
#define NB_ATTEMPTS_ACK 5 //number of attempts to try before giving up

#define ref_CS          8
#define heater_CS       7
#define HEAT_EN         6

#define debug           1

uint8_t heatTime = 10; //in minutes

long TRANSMIT_PERIOD = 60000; //transmit a packet to gateway so often (in ms)
int TRANSMIT_PERIOD_MINUTES = 20;
int ACK_FAIL_WAIT_PERIOD = 500;

RFM69 radio;
Adafruit_MAX31855 heater(heater_CS);
Adafruit_MAX31855 ref(ref_CS);

ISR(WDT_vect) {
  Sleepy::watchdogEvent();
} //set watchdog for Sleepy

typedef struct {
  int nodeID; //store this nodeID
  float heater_temp;
  float heater_internal_temp;
  float ref_temp;
  float ref_internal_temp;
  int num_attempts;
}
Payload;
Payload payload;


void setup() {
  if (debug) Serial.begin(SERIAL_BAUD); //Begin Serial
  if (debug) Serial.print("Start--");
  if (debug) Serial.print("pre radio init--");
  radio.initialize(FREQUENCY, NODE_ID, NETWORK_ID); //Begin Radio
  if (debug) Serial.println("post radio init--");
//  radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY); //Encrypt
  radio.sleep(); //Sleep radio (saves power)
  if (debug) {
    char buff[50];
    sprintf(buff, "Listening at %d Mhz...", FREQUENCY == RF69_433MHZ ? 433 : FREQUENCY == RF69_868MHZ ? 868 : 915);
    Serial.println(buff);
  }
  randomSeed(analogRead(0)); //Collect Random Seed for ACK wait Timing

  pinMode(HEAT_EN, OUTPUT);
  delay(500);
}

void loop() {
  int nAttempt = 0; //number of sends attempted
  payload.num_attempts = nAttempt;
  payload.nodeID = NODE_ID;
  digitalWrite(HEAT_EN, HIGH); //enable heater
  //then wait ten minutes
  for (int i = 0; i < heatTime; i++) {
    Sleepy::loseSomeTime(60000);
  }
  payload.heater_temp = heater.readCelsius();
  payload.heater_internal_temp = heater.readInternal();
  payload.ref_temp = ref.readCelsius();
  payload.ref_internal_temp = ref.readInternal();
  digitalWrite(HEAT_EN, LOW); //turn off heater
  bool flag_ACK_received = false;

  if (debug) {
    Serial.print(NETWORK_ID);
    Serial.print(".");
    Serial.print(payload.nodeID);
    Serial.print(", ");
    Serial.print(payload.heater_temp);
    Serial.print(", ");
    Serial.print(payload.heater_internal_temp);
    Serial.print(", ");
    Serial.print(payload.ref_temp);
    Serial.print(", ");
    Serial.print(payload.ref_internal_temp);
    Serial.println();
    Serial.flush();
  }
  digitalWrite(LED, LOW); //Turns ON led to signal that Transmission has started
  if (debug) Serial.print("sending packet");
  while (nAttempt < NB_ATTEMPTS_ACK && !flag_ACK_received) { //resend package # of times if it doesn't go through
    if (radio.sendWithRetry(GATEWAY_ID, (const void*)(&payload), sizeof(payload))) { //send payload, if it retuns a 1 it  successfully recieved the ACK
      if (debug) {
        Serial.print(" ACK rcv'd with ");
        Serial.print(nAttempt + 1);
        Serial.print("tries");
        Serial.flush();
      }
      flag_ACK_received = true;
    }
    else { //DID not recieve ACK
      if (debug) {
        Serial.print(".");
        Serial.flush();
      }
      if (radio.sendWithRetry(GATEWAY_ID, (const void*)(&payload), sizeof(payload))) { //Sending Again
        if (debug) {
          Serial.print(" ACK rcv'd with ");
          Serial.print(nAttempt);
          Serial.print(" tries.");
          Serial.flush();
        }
        flag_ACK_received = true; //mark as successful to exit While Loop
      }
      nAttempt++;
      payload.num_attempts = nAttempt;
      ACK_FAIL_WAIT_PERIOD = random(300, 600); //sets a random wait period to not interfere with other sensors
      Sleepy::loseSomeTime(ACK_FAIL_WAIT_PERIOD);  //wait for a few moments before trying again
    }
  }
  if (debug) {
    if (!flag_ACK_received) {
      Serial.print("ack not recv'd");
    }
    Serial.println();
    Serial.flush();
  }
  digitalWrite(LED, HIGH); //turn LED off to signal completion of transmission

  //Transmission succesfully completed
  if (debug) {
    Serial.print("Sleeping for ");
    Serial.print((TRANSMIT_PERIOD / 1000) * TRANSMIT_PERIOD_MINUTES);
    Serial.println("s");
    Serial.flush();
  }
  radio.sleep(); //Sleep the Radio
  //  digitalWrite(LED, HIGH); //Turn LED off
  for (int i = 0; i < TRANSMIT_PERIOD_MINUTES; i++) { //Sleep the µC
    Sleepy::loseSomeTime(TRANSMIT_PERIOD);
  }
}


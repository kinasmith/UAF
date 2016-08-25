#include  <Wire.h>
#include <SPI.h>
#include "RFM69.h"
#include "jeelib-sleepy.h"
#include  "MCP342X.h"

MCP342X myADC;
/****************************************************************************/
/***********************    DON'T FORGET TO SET ME    ***********************/
/****************************************************************************/
#define NODE_ID         5
#define NETWORK_ID      102
/****************************************************************************/
/****************************************************************************/

#define GATEWAY_ID      0
#define FREQUENCY       RF69_433MHZ
#define KEY             "p6ZNvTmGfdY2hUXb" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED             3
#define SERIAL_BAUD     9600
#define ACK_TIME        100  // # of ms to wait for an ack
#define NB_ATTEMPTS_ACK 5 //number of attempts to try before giving up
#define V_BAT_PIN       A3
#define V_EXCITE_PIN    A1
#define SENS_EN         4


int blinkCount = 1;
long TRANSMIT_PERIOD = 60000; //transmit a packet to gateway so often (in ms)
int TRANSMIT_PERIOD_MINUTES = 1;
int ACK_FAIL_WAIT_PERIOD = 500;

RFM69 radio;

ISR(WDT_vect) {
  Sleepy::watchdogEvent();
} //set watchdog for Sleepy

typedef struct {
  int nodeID; //store this nodeID
  int sens_val;
  float temp;
  float excite_v;
  float batt_v;
  int num_attempts;
}
Payload;
Payload payload;

void setup() {
  Wire.begin(); //Begin i2c
  radio.initialize(FREQUENCY, NODE_ID, NETWORK_ID); //Begin Radio
  radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY); //Encrypt
  radio.sleep(); //Sleep radio (saves power)
  //configure ADC
  myADC.configure( MCP342X_MODE_ONESHOT |
                   MCP342X_CHANNEL_1 |
                   MCP342X_SIZE_16BIT |
                   MCP342X_GAIN_1X
                 );
  pinMode(LED, OUTPUT); //Set LED Mode
  randomSeed(analogRead(0)); //Collect Random Seed for Delay Timing
}

void loop() {
  int nAttempt = 0; //number of sends attempted
  payload.num_attempts = nAttempt;
  bool flag_ACK_received = false; //is the acknowledgement recieved?
  payload.nodeID = NODE_ID;
  int temperature = radio.readTemperature(); //get Temp value from Radio a BYTE value, NEG values roll over
  if (temperature > 100) temperature -= 255; //converts BYTE value to INT value
  payload.temp = temperature;
  payload.batt_v = checkVoltage(V_BAT_PIN);
  payload.sens_val = getSensorValue();

  if (blinkCount == 0)
    digitalWrite(LED, LOW); //Turns ON led to signal that Transmission has started
  //Begin Transmission
  while (nAttempt < NB_ATTEMPTS_ACK && !flag_ACK_received) { //resend package # of times if it doesn't go through
    if (radio.sendWithRetry(GATEWAY_ID, (const void*)(&payload), sizeof(payload))) { //send payload, if it retuns a 1 it  successfully recieved the ACK
      flag_ACK_received = true;
    }
    else { //DID not recieve ACK
      if (radio.sendWithRetry(GATEWAY_ID, (const void*)(&payload), sizeof(payload))) { //Sending Again
        flag_ACK_received = true; //mark as successful to exit While Loop
      }
      nAttempt++;
      payload.num_attempts = nAttempt;
      ACK_FAIL_WAIT_PERIOD = random(300, 600); //sets a random wait period to not interfere with other sensors
      Sleepy::loseSomeTime(ACK_FAIL_WAIT_PERIOD);  //wait for a few moments before trying again
    }
  }
  //Transmission succesfully completed
  if (blinkCount == 0) blinkCount++;
  radio.sleep(); //Sleep the Radio
  digitalWrite(LED, HIGH); //Turn LED off
  for (int i = 0; i < TRANSMIT_PERIOD_MINUTES; i++) { //Sleep the µC
    Sleepy::loseSomeTime(TRANSMIT_PERIOD);
  }
}

void Blink(byte PIN, int DELAY_MS) { //blink and LED
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, LOW);
  Sleepy::loseSomeTime(DELAY_MS);
  digitalWrite(PIN, HIGH);
  Sleepy::loseSomeTime(DELAY_MS);
}

float checkVoltage(int pin) { //takes 100ms
  float v = 0;
  for (int i = 0; i < 10; i++) {
    v += analogRead(pin);
    Sleepy::loseSomeTime(10);
  }
  //convert analog reading into actual voltage
  v = v / 10;
  v = 3.3 * v / 1023.0; //Converts 10bit value to voltage
  return v; //return value
}

int getSensorValue() { //takes 100ms
  int  r;
  float v;
  pinMode(SENS_EN, OUTPUT);
  digitalWrite(SENS_EN, HIGH); //write enable high for 10 ms
  Sleepy::loseSomeTime(100); //let the Capacitor charge for a moment
  digitalWrite(SENS_EN, LOW); //write enable low. Falling edge triggers FET
  Sleepy::loseSomeTime(9); //wait for reference to stablize
  for (int i = 0; i < 5; i++) {
    v += analogRead(V_EXCITE_PIN);
    delay(1);
  }
  v /= 5;
  v = 3.3 * v / 1023.0;
  payload.excite_v = v;
  myADC.startConversion();
  myADC.getResult(&r);
  return r;
}

























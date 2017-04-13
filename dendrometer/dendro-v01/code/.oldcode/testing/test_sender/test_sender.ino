#include <RFM69.h>
#include <SPI.h>
#include <jeelib-sleepy.h>

#define NODEID      14
#define NETWORKID   100
#define GATEWAYID   1
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "p6ZNvTmGfdY2hUXb" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9
#define SERIAL_BAUD 9600
#define ACK_TIME    50  // # of ms to wait for an ack
#define NB_ATTEMPTS_ACK 5 //number of attempts to try before giving up

int TRANSMIT_PERIOD = 60000; //transmit a packet to gateway so often (in ms)
int TRANSMIT_PERIOD_MINUTES = 2;
int ACK_FAIL_WAIT_PERIOD = 500; 
boolean requestACK = true;
int numOfSends = 0;
RFM69 radio;

ISR(WDT_vect) { 
  Sleepy::watchdogEvent(); 
} //set watchdog for Sleepy

typedef struct {    
  int nodeId; //store this nodeId
  unsigned long uptime; //uptime in ms
  int num;
} 
Payload;
Payload payload;

void setup() {
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY);
  radio.sleep();
  randomSeed(analogRead(0));

}

long lastPeriod = -1;

void loop() {
  int nAttempt = 0; //number of sends attempted
  bool flag_ACK_received = false; //is the acknowledgement recieved?
  payload.nodeId = NODEID;
  payload.uptime = numOfSends;
  payload.num = random(1000); //increment number
  while(nAttempt < NB_ATTEMPTS_ACK && !flag_ACK_received) { //resend package if it doesn't go through
    if (radio.sendWithRetry(GATEWAYID, (const void*)(&payload), sizeof(payload))){
      flag_ACK_received = true;
      Blink(LED, 100); //blink once for successful SEND
      numOfSends++;
    } 
    else {
      radio.sendWithRetry(GATEWAYID, (const void*)(&payload), sizeof(payload));
      //Blink thrice for Failed Send
      Blink(LED, 100);
      delay(100);
      Blink(LED, 100);
      delay(100);
      Blink(LED, 100);
      delay(100);
      nAttempt++;
      Sleepy::loseSomeTime(ACK_FAIL_WAIT_PERIOD); //wait 30 seconds
    }
  }
  delay(100); //Let everything Finish before Sleeping
  radio.sleep(); //Sleep the Radio
  for(int i = 0; i < TRANSMIT_PERIOD_MINUTES; i++) { //Sleep the µC 
    Sleepy::loseSomeTime(TRANSMIT_PERIOD);
  }
}


void Blink(byte PIN, int DELAY_MS) { //blink and LED
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,LOW);
  delay(DELAY_MS);
  digitalWrite(PIN,HIGH);
}




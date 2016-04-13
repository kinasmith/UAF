#include <RFM69.h>
#include <SPI.h>
#include <jeelib-sleepy.h>
#include  <Wire.h>
#include  <MCP342X.h>

MCP342X myADC;

#define NODE_ID      1
#define NETWORK_ID   100
#define GATEWAY_ID   0
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "p6ZNvTmGfdY2hUXb" //has to be same 16 characters/bytes on all nodes, not more not less!
#define SERIAL_BAUD 9600

#define ACK_TIME    100  // # of ms to wait for an ack
#define NB_ATTEMPTS_ACK 5 //number of attempts to try before giving up

#define V_BAT_PIN A3
#define SENS_EN 4
#define LED         3

int TRANSMIT_PERIOD = 1000; //transmit a packet to gateway so often (in ms)
int TRANSMIT_PERIOD_MINUTES = 1;
int ACK_FAIL_WAIT_PERIOD = 500; 

RFM69 radio;

ISR(WDT_vect) { 
  Sleepy::watchdogEvent(); 
} //set watchdog for Sleepy

typedef struct {    
  int nodeID; //store this nodeID
  float temp;
  float voltage;
  int num_attempts;
  int sens_val;
} 
Payload;
Payload payload;

float start_position;

void setup() {
  Wire.begin();
  Serial.begin(SERIAL_BAUD);

  radio.initialize(FREQUENCY,NODE_ID,NETWORK_ID);
  radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY);
  radio.sleep();
  char buff[50];
  sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
  Serial.println(myADC.testConnection() ? "MCP342X connection successful" : "MCP342X connection failed");
  myADC.configure( MCP342X_MODE_ONESHOT|
    MCP342X_CHANNEL_1 |
    MCP342X_SIZE_16BIT |
    MCP342X_GAIN_1X
    );
  Serial.println(myADC.getConfigRegShdw(), HEX);
  pinMode(LED, OUTPUT);
  randomSeed(analogRead(0));

  //read initial value from ADC
  start_position = getSensorValue();
}

long lastPeriod = -1;

void loop() {
  int nAttempt = 0; //number of sends attempted
  payload.num_attempts = nAttempt;
  bool flag_ACK_received = false; //is the acknowledgement recieved?
  payload.nodeID = NODE_ID;
  int tempTemp = radio.readTemperature();
  if(tempTemp > 100) tempTemp -= 255;
  payload.temp = tempTemp;
  Serial.print("temp, ");
  Serial.println(payload.temp);
  Serial.flush();
  // payload.temp = radio.readTemperature();
  payload.voltage = checkVoltage(V_BAT_PIN);
  Serial.print("batVoltage, ");
  Serial.println(payload.voltage);
  Serial.flush();
  payload.sens_val = getSensorValue();
  Serial.print("SensorValue, ");
  Serial.println(payload.sens_val);
  Serial.flush();
  //  payload.sens_val = start_position - getSensorValue();
  // Serial.println(payload.sens_val);
  digitalWrite(LED, LOW); //start Sending!
  while(nAttempt < NB_ATTEMPTS_ACK && !flag_ACK_received) { //resend package # of times if it doesn't go through
    Serial.println("sending payload");
    if (radio.sendWithRetry(GATEWAY_ID, (const void*)(&payload), sizeof(payload))){  //send payload is successful
      Serial.println("got ACK"); 
      Serial.flush();
      flag_ACK_received = true;
      digitalWrite(LED, HIGH); //turn LED off to signal completion of transmission
    } 
    else {
      Serial.println("did not get ACK"); 
      Serial.flush();
      analogWrite(LED, 200);
      if(radio.sendWithRetry(GATEWAY_ID, (const void*)(&payload), sizeof(payload))) {
        Serial.print(nAttempt);
        Serial.println(" Got ACK"); 
        Serial.flush();
        flag_ACK_received = true;
        digitalWrite(LED, HIGH); //turn LED off to signal completion of transmission
      }
      nAttempt++;
      payload.num_attempts = nAttempt;
      ACK_FAIL_WAIT_PERIOD = random(300, 600); //sets a random wait period to not interfere with other sensors
      delay(10); //wait to finish before sleeping
      Sleepy::loseSomeTime(ACK_FAIL_WAIT_PERIOD);  //wait for a few moments before trying again
    }
  }
  Serial.println("NOW I SLEEP!"); 
  Serial.flush();
  delay(10); //Let everything Finish before Sleeping
  radio.sleep(); //Sleep the Radio
  digitalWrite(LED, HIGH);
  for(int i = 0; i < TRANSMIT_PERIOD_MINUTES; i++) { //Sleep the µC 
    Sleepy::loseSomeTime(TRANSMIT_PERIOD);
  }
}

void Blink(byte PIN, int DELAY_MS) { //blink and LED
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,LOW);
  delay(DELAY_MS);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
}

float checkVoltage(int pin) { //takes 100ms
  float v = 0;
  for (int i = 0; i < 10; i++) {
    v += analogRead(pin);
    Sleepy::loseSomeTime(10);
  }
  //convert analog reading into actual voltage
  v = v/10; 
  v = (3.3 * v/1024.0); //this should be v/1023? 10 bit, 1024 values, 0-1023
  return v; //return value
}

int getSensorValue() { //takes 350ms
  int  r;
  float v;
  pinMode(SENS_EN, OUTPUT);
  digitalWrite(SENS_EN, HIGH); //write enable high for 10 ms
  Sleepy::loseSomeTime(100); //let the Capacitor charge for a moment
  digitalWrite(SENS_EN, LOW); //write enable low. Falling edge triggers FET
  v = analogRead(A1);
  myADC.startConversion(); 
  myADC.getResult(&r);
  v = (3.3*v/1024.0);
  Serial.print("excite Voltage, ");
  Serial.println(v);
  return r;
}













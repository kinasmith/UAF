#include <RFM69.h>
#include <SPI.h>
#include <jeelib-sleepy.h>
#include  <Wire.h>
#include  <MCP342X.h>

MCP342X myADC;

#define NODE_ID      2
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

int TRANSMIT_PERIOD = 60000; //transmit a packet to gateway so often (in ms)
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
  Serial.println("TEST!!!");

  //  radio.initialize(FREQUENCY,NODE_ID,NETWORK_ID);
  //  radio.setHighPower(); //uncomment only for RFM69HW!
  //  radio.encrypt(KEY);
  //  radio.sleep();
  //  char buff[50];
  //  sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  //  Serial.println(buff);
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
  //  start_position = getSensorValue();
}

long lastPeriod = -1;

void loop() {
  unsigned int val = 32900;
  Serial.println(val);
  //  payload.sens_val = `getSensorValue();
  //  Serial.print("SensorValue, ");
  Serial.println(getSensorValue());
  //  Serial.flush();
  delay(5000);
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

int16_t getSensorValue() { //takes 350ms
  int16_t  r;
  float v;
  pinMode(SENS_EN, OUTPUT);
  digitalWrite(SENS_EN, HIGH); //write enable high for 10 ms
  //  Sleepy::loseSomeTime(100); //let the Capacitor charge for a moment
  delay(100);
  digitalWrite(SENS_EN, LOW); //write enable low. Falling edge triggers FET
  //  v = analogRead(A1);
  myADC.startConversion(); 
  myADC.getResult(&r);
  //  v = (3.3*v/1024.0);
  //  Serial.print("excite Voltage, ");
  //  Serial.println(v);
  return r;
}















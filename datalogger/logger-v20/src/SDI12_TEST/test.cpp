#include <SDI12.h>
//neets 0.1uF Cap between signal and GND
void tip();
// #define DATAPIN 9         // change to the proper pin
// SDI12 mySDI12(DATAPIN);

/*
  '?' is a wildcard character which asks any and all sensors to respond
  'I' indicates that the command wants information about the sensor
  '!' finishes the command
*/
String myCommand = "0I!";
//0M5!
//String myCommand = "0I!";
//Sconst byte ledPin = 13;
const byte interruptPin = 2;
volatile byte state = LOW;
volatile uint16_t cnt = 0;

void setup(){
  Serial.begin(9600);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), tip, FALLING);
  // Serial.begin(9600);
  // mySDI12.begin();
  // mySDI12.sendCommand(myCommand);
  // delay(300);                     // wait a while for a response
  // while(mySDI12.available()){    // write the response to the screen
  //   Serial.write(mySDI12.read());
  // }
}

void loop(){
  if(state) {
    Serial.print("tip"); Serial.println(cnt);
    state = LOW;
  }
  // myCommand = "0M3!";
  // mySDI12.sendCommand(myCommand);
  // delay(300);                     // wait a while for a response
  // while(mySDI12.available()){    // write the response to the screen
  //   Serial.write(mySDI12.read());
  // }
  // delay(3000); // print again in three seconds
  // myCommand = "0D0!";
  // mySDI12.sendCommand(myCommand);
  // delay(300);                     // wait a while for a response
  // while(mySDI12.available()){    // write the response to the screen
  //   Serial.write(mySDI12.read());
  // }
  // delay(3000);
}

void tip() {
  cnt++;
  state = HIGH;
}

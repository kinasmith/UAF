#include <RFM69.h>
#include <SPI.h>
#include "RTClib.h"
#include <SdFat.h>

/****************************************************************************/
/***********************    DON'T FORGET TO SET ME    ***********************/
/****************************************************************************/
#define NODE_ID         0
#define NETWORK_ID      100
//#define LED         9 //LED for Spruce Site ....Check this!
#define LED         3 //LED for Birch Site is 3....Stupid
/****************************************************************************/
/****************************************************************************/

#define FREQUENCY   RF69_433MHZ //Match this to the frequency of your radio
#define KEY         "p6ZNvTmGfdY2hUXb" //has to be same 16 characters/bytes on all nodes.

#define debug       0 //debug false/true

/*==============|| DS3231_RTC ||==============*/
RTC_DS3231 rtc; //Initialize the Real Time Clock
DateTime now; //initialize a DateTime Object

/*==============|| SD ||==============*/
SdFat SD; //This is the SD Card
SdFile dataFile; //this is the DatFile
const int CHIP_SELECT = 4; //SPI SS pin for SD card
const int CARD_DETECT = 5; //Card Detect Pin
int CARD_PRESENT; //var for Card Detect sensor reading

/*==============|| RFM69 ||==============*/
RFM69 radio; //Initialize Radio

//Packet structure for holding data. This must be the same as the Sender Nodes
typedef struct {
  int nodeID; //Sender NODE ID
  int sens_val; //sensor Value
  float temp; //Temp of Sender
  float excite_v; //excitation voltage of softpot
  float batt_v; //voltage of Sender
  int num_attempts; //number of attampts before successful transmission
}
Payload;
Payload data_rcv; //This initializes and names the struct

void setup() {
  if (debug) Serial.begin(9600); //Begins Serial
  rtc.begin(); //Begins RTC Library
  if (debug)  rtc.adjust(DateTime((__DATE__), (__TIME__))); //sets the RTC to the computer time.

  if (!SD.begin(CHIP_SELECT)) { //Checks the SD card to make sure it responds. On success, it initializes it.
    if (debug) Serial.println("Card failed, or not present");
    CARD_PRESENT = digitalRead(CARD_DETECT);  //read CD pin
    //this delays the program while there is no card inserted.
    while (CARD_PRESENT == 0) { //The CD pin is 0 if there is no card inserted
      CARD_PRESENT = digitalRead(CARD_DETECT); //read it the value for every loop, or we will never exit the While Loop
      Blink(LED, 50); //blink to show an error.
    }
    delay(10); //wait for card to stablize from insertion before initializing

    if (!SD.begin(CHIP_SELECT)) //when the SD card is re-inserted, intiliaze it!
      Blink(LED, 50); //if it fails again....well, you're probably out of luck.
    if (debug) Serial.println("SD card initialized");
  }
  if (debug) Serial.println("SD card initialized"); ///Victory

  radio.initialize(FREQUENCY, NODE_ID, NETWORK_ID); //Initialize Radio
  radio.setHighPower(); //Set it to high Power Mode
  radio.encrypt(KEY); //Set encryption Key
  if (debug) {
    Serial.print("RFM69 initialized");
    Serial.print(", key: ");
    Serial.println(KEY);
  }
  if (debug) {
    now = rtc.now(); //get the current time
    Serial.print("Current time is: ");
    Serial.println(now.unixtime());
  }
  if (debug) {
    Serial.print("Network Address: ");
    Serial.print(NETWORK_ID);
    Serial.print(".");
    Serial.println(NODE_ID);
  }
}

void loop() {
  /*==============|| RADIO Recieve ||==============*/
  if (radio.receiveDone()) { //If the radio recieves data....
    now = rtc.now(); //get the current time
    if (radio.DATALEN == sizeof(Payload)) { //And that data matches our Struct.
      data_rcv = *(Payload*)radio.DATA; // convert the radio's raw byte array to our pre-defined Packet struct
      if (radio.ACKRequested()) { // if requested, acknowledge that the packet was received
        radio.sendACK();
      }
      if (debug) printData(); //Print the data out to the Serial Monitor
      //and write the data to the SD card
      if (writeDataToCard(data_rcv.nodeID, now.unixtime(), data_rcv.sens_val, data_rcv.temp, data_rcv.excite_v, data_rcv.batt_v, data_rcv.num_attempts)) {
        Blink(LED, 100); //Blink the LED on success
      }
    }
    else { //if the data is an unexpected size
      if (debug) Serial.println("Invalid packet received!!");
      if (badPacket(now.unixtime())) { //note that int he log.txt file and blink thrice
        Blink(LED, 50);
        Blink(LED, 50);
        Blink(LED, 50);
      }
    }
  }
  checkSdCard();
}

int writeDataToCard(int id, long utm, int s_v, float t, float e_v, float b_v, int num_a) {
  //creates filename to store data based on the Node Address
  String address = String(String(NETWORK_ID) + "_" + String(id));
  String fileName = String(address + ".csv");
  char __fileName[fileName.length() + 1];
  fileName.toCharArray(__fileName, sizeof(__fileName));
  //Opens and checks file
  if (!dataFile.open(__fileName, O_RDWR | O_CREAT | O_AT_END)) {
    if (debug) {
      Serial.print("Error opening: ");
      Serial.println(fileName);
    }
    Blink(LED, 100);
    Blink(LED, 100);
    return 0; //returns a 0, failure
  }
  //Print data to new line
  dataFile.print(NETWORK_ID);
  dataFile.print(".");
  dataFile.print(id);
  dataFile.print(",");
  dataFile.print(utm);
  dataFile.print(",");
  dataFile.print(s_v);
  dataFile.print(",");
  dataFile.print(t);
  dataFile.print(",");
  dataFile.print(b_v);
  dataFile.print(",");
  dataFile.print(e_v);
  dataFile.print(",");
  dataFile.print(num_a);
  dataFile.println();
  dataFile.close();
  return 1; //returns a 1, Success
}

void printData() {
  Serial.print("Node: ");
  Serial.print(NETWORK_ID);
  Serial.print(".");
  Serial.print(data_rcv.nodeID);
  Serial.print(", Time: ");
  Serial.print(now.unixtime());
  Serial.print(", ADC: ");
  Serial.print(data_rcv.sens_val);
  Serial.print(", Temp: ");
  Serial.print(data_rcv.temp);
  Serial.print(", Ve: ");
  Serial.print(data_rcv.excite_v);
  Serial.print(", Vb: ");
  Serial.print(data_rcv.batt_v);
  Serial.print(", Send Attempts, ");
  Serial.print(data_rcv.num_attempts);
  Serial.println();
}

void Blink(byte PIN, int DELAY_MS) {
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN, LOW);
  delay(DELAY_MS);
}

int badPacket(long utm) {
  dataFile.print("Bad Packet Recieved at: ");
  dataFile.print(utm);
  dataFile.println();
  dataFile.close();
  return 1;
}

void checkSdCard() {
  //If SD Card is Removed, Blink LED FOREVER!
  CARD_PRESENT = digitalRead(CARD_DETECT); //Check for card insertion
  if (CARD_PRESENT == 0) { //if card NOT present....Wait and blink until it is
    if (debug) Serial.println("Card Not Present");
    while (CARD_PRESENT == 0) {
      Blink(LED, 50);
    }
  }
}


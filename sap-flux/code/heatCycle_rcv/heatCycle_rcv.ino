#include <RFM69.h>
#include <SPI.h>
#include "RTClib.h"
#include <SdFat.h>


#define NODE_ID         0
#define NETWORK_ID      105
#define LED             9

#define FREQUENCY   RF69_433MHZ //Match this to the frequency of your radio
#define KEY         "p6ZNvTmGfdY2hUXb" //has to be same 16 characters/bytes on all nodes.

#define debug       1 //debug

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
  int nodeID; //store this nodeID
  float heater_temp;
  float heater_internal_temp;
  float ref_temp;
  float ref_internal_temp;
  int num_attempts;
}
Payload;
Payload data_rcv; //This initializes and names the struct

void setup() {
  pinMode(CARD_DETECT, INPUT);
  if (debug) Serial.begin(9600); //Begins Serial
  rtc.begin(); //Begins RTC Library
  Serial.println(digitalRead(CARD_DETECT));
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
//  radio.setHighPower(); //Set it to high Power Mode
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
      if (writeDataToCard(data_rcv.nodeID, now.unixtime(), data_rcv.heater_temp, data_rcv.heater_internal_temp, data_rcv.ref_temp, data_rcv.ref_internal_temp, data_rcv.num_attempts)) {
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


int writeDataToCard(int id, long utm, float h_t, float h_i_t, float r_t, float r_i_t, int num_a) {
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
  dataFile.print(h_t);
  dataFile.print(",");
  dataFile.print(h_i_t);
  dataFile.print(",");
  dataFile.print(r_t);
  dataFile.print(",");
  dataFile.print(r_i_t);
  dataFile.print(",");
  dataFile.print(num_a);
  dataFile.println();
  dataFile.close();
  return 1; //returns a 1, Success
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

void printData() {
  Serial.print("Node: ");
  Serial.print(NETWORK_ID);
  Serial.print(".");
  Serial.print(data_rcv.nodeID);
  Serial.print(", Time: ");
  Serial.print(now.unixtime());
  Serial.print(", heater temp: ");
  Serial.print(data_rcv.heater_temp);
  Serial.print(", heater internal temp: ");
  Serial.print(data_rcv.heater_internal_temp);
  Serial.print(", ref temp: ");
  Serial.print(data_rcv.ref_temp);
  Serial.print(", ref internal temp: ");
  Serial.print(data_rcv.ref_internal_temp);
  Serial.print(", Send Attempts, ");
  Serial.print(data_rcv.num_attempts);
  Serial.println();
}


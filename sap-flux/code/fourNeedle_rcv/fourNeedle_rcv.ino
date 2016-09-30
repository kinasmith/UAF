#include <SPI.h>
#include "RTClib.h"
#include "RFM69.h"
#include "SdFat.h"

#define NODE_ID         0
#define NETWORK_ID      105
#define LED             9
#define FREQUENCY   RF69_433MHZ //Match this to the frequency of your radio
#define KEY         "p6ZNvTmGfdY2hUXb" //has to be same 16 characters/bytes on all nodes.

/*==============|| DS3231_RTC ||==============*/
RTC_DS3231 rtc; //Initialize the Real Time Clock
DateTime now; //initialize a DateTime Object

/*==============|| SD ||==============*/
SdFat SD; //This is the SD Card
SdFile dataFile; //this is the DatFile
const int CHIP_SELECT = 4; //SPI SS pin for SD card
const int CARD_DETECT = 5; //Card Detect Pin
//int CARD_PRESENT; //var for Card Detect sensor reading

/*==============|| RFM69 ||==============*/
RFM69 radio; //Initialize Radio

//Packet structure for holding data. This must be the same as the Sender Nodes
typedef struct {
  int node_id;
  float Tc1_t;
  float Tc2_t;
  float Tc3_t;
  float TcH_t;
}
Payload;
Payload data_rcv; //This initializes and names the struct


void setup() {
  Serial.begin(9600);
  rtc.begin();
  if (!SD.begin(CHIP_SELECT)) {
    Serial.println("SD Failed to Initialize");
    while (1) {
      Blink(LED, 50);
    }
  }
  Serial.println("SD Card Initialized");

  radio.initialize(FREQUENCY, NODE_ID, NETWORK_ID); //Initialize Radio
  radio.setHighPower(); //Set it to high Power Mode
  radio.encrypt(KEY); //Set encryption Key
  Serial.print("RFM69 initialized"); Serial.print(", key: "); Serial.println(KEY);
  now = rtc.now(); //get the current time
  Serial.print("Current time is: "); Serial.println(now.unixtime());
  Serial.print("Network Address: "); Serial.print(NETWORK_ID); Serial.print("."); Serial.println(NODE_ID);
}

void loop() {
  if (radio.receiveDone()) { //If the radio recieves data....
    now = rtc.now(); //get the current time
    if (radio.DATALEN == sizeof(Payload)) { //And that data matches our Struct.
      data_rcv = *(Payload*)radio.DATA; // convert the radio's raw byte array to our pre-defined Packet struct
      if (radio.ACKRequested()) { // if requested, acknowledge that the packet was received
        radio.sendACK();
      }
      printData(); //Print the data out to the Serial Monitor
      //and write the data to the SD card
      if (writeDataToCard(data_rcv.node_id, now.unixtime(), data_rcv.Tc1_t, data_rcv.Tc2_t, data_rcv.Tc3_t, data_rcv.TcH_t)) {
        Blink(LED, 100); //Blink the LED on success
      }
    }
    else { //if the data is an unexpected size
      Serial.println("Invalid packet received!!");
      if (badPacket(now.unixtime())) { //note that int he log.txt file and blink thrice
        Blink(LED, 50);
        Blink(LED, 50);
        Blink(LED, 50);
      }
    }
  }
}

int writeDataToCard(int id, long utm, float t1, float t2, float t3, float th) {
  //creates filename to store data based on the Node Address
  String address = String(String(NETWORK_ID) + "_" + String(id));
  String fileName = String(address + ".csv");
  char __fileName[fileName.length() + 1];
  fileName.toCharArray(__fileName, sizeof(__fileName));
  //Opens and checks file
  if (!dataFile.open(__fileName, O_RDWR | O_CREAT | O_AT_END)) {
    Serial.print("Error opening: ");
    Serial.println(fileName);
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
  dataFile.print(t1);
  dataFile.print(",");
  dataFile.print(t2);
  dataFile.print(",");
  dataFile.print(t3);
  dataFile.print(",");
  dataFile.print(th);
  dataFile.println();
  dataFile.close();
  return 1; //returns a 1, Success
}

void printData() {
  Serial.print("Node: ");
  Serial.print(NETWORK_ID);
  Serial.print(".");
  Serial.print(data_rcv.node_id);
  Serial.print(", Time: ");
  Serial.print(now.unixtime());
  Serial.print(", Tc_1 temp: ");
  Serial.print(data_rcv.Tc1_t);
  Serial.print(", Tc_2 temp: ");
  Serial.print(data_rcv.Tc2_t);
  Serial.print(", Tc_3 temp: ");
  Serial.print(data_rcv.Tc3_t);
  Serial.print(", Tc_H temp: ");
  Serial.print(data_rcv.TcH_t);
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

#include <RFM69.h>
#include <SPI.h>
#include <DS3231.h>
#include <SD.h>


#define NODE_ID      0 //Network Node ID. All Senders must send to this ID #
#define NETWORK_ID   100 //Network ID. All Nodes must be on the same Network
#define FREQUENCY   RF69_433MHZ //Match this to the frequency of your radio
#define KEY         "p6ZNvTmGfdY2hUXb" //has to be same 16 characters/bytes on all nodes.
#define LED         3 //LED Pin Number
#define SERIAL_BAUD 9600 //Serial Baud Rate

/*==============|| DS3231_RTC ||==============*/
DS3231  rtc(SDA, SCL); //Initialize the Real Time Clock

/*==============|| SD ||==============*/
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
  Serial.begin(9600); //Begins Serial
  rtc.begin(); //Begins RTC Library
  if(!SD.begin(CHIP_SELECT)) { //Checks the SD card to make sure it responds. On success, it initializes it.
    Serial.println("Card failed, or not present");
    CARD_PRESENT = digitalRead(CARD_DETECT);  //read CD pin
    while(CARD_PRESENT == 0) { //The CD pin is 0 if there is no card inserted
      CARD_PRESENT = digitalRead(CARD_DETECT); //read it the value for every loop, or we will never exit the While Loop
      Blink(LED, 50); //blink!
    }
    if(!SD.begin(CHIP_SELECT))  Serial.println("SD card initialized"); //when the SD card is re-inserted, intiliaze it!
  }
  Serial.println("SD card initialized"); ///Victory

  radio.initialize(FREQUENCY,NODE_ID,NETWORK_ID); //Initialize Radio
  radio.setHighPower(); //Set it to high Power Mode
  radio.encrypt(KEY); //Set encryption Key
  Serial.print("RFM69 initialized");
  Serial.print(", key: ");
  Serial.println(KEY);
}

void loop() {
  /*==============|| RADIO Recieve ||==============*/
  if (radio.receiveDone()) { //If the radio recieves data....
    if(radio.DATALEN == sizeof(Payload)) { //And that data matches our Struct.
      data_rcv = *(Payload*)radio.DATA; // convert the radio's raw byte array to our pre-defined Packet struct
      if (radio.ACKRequested()) { // if requested, acknowledge that the packet was received
        radio.sendACK();
      }
      printData(); //Print the data out to the Serial Monitor
      //ID, time, sensor, temp, excite volts, battery volts, number of attempts
      if(writeDataToCard(data_rcv.nodeID, thisUnixTime(), data_rcv.sens_val, data_rcv.temp, data_rcv.excite_v, data_rcv.batt_v, data_rcv.num_attempts)){ //Write the data to the SD card
        Blink(LED, 100); //Blink the LED on success
      }
    }
    else { //if the data is an unexpected size
      Serial.println("Invalid packet received!!"); 
      if(writeDataToCard(00,thisUnixTime(),00,00,00,00,00)) { //Print a line of garbage to the SD card and note the time
        //blink twice to signal failure
        Blink(LED, 25);
        Blink(LED, 25);
      }
    }
  }
}

long thisUnixTime(){
  //gets the current Time
  return rtc.getUnixTime(rtc.getTime());
}
//ID, time, sensor, temp, excite volts, battery volts, number of attempts
int writeDataToCard(int id, long utm, int s_v, float t, float e_v, float b_v, int num_a) {
  //Check to make sure card is inserted
  //if it isn't, wait until it is before anything happens
  CARD_PRESENT = digitalRead(CARD_DETECT);
  if(CARD_PRESENT == 0) {
    Serial.println("Card Not Present");
    while(CARD_PRESENT == 0) {
      CARD_PRESENT = digitalRead(CARD_DETECT);
      Blink(LED, 100);
    }
    if(!SD.begin(CHIP_SELECT))   Serial.println("SD card initialized");
    else Serial.println("SD Card Failed to Initialize");
  }

  //Write data to card
  String fileName = String(String(id) + ".csv"); //creates filename based on Sender ID
  char __fileName[fileName.length()+1]; 
  fileName.toCharArray(__fileName, sizeof(__fileName));
  File dataFile = SD.open(__fileName, FILE_WRITE); //Opens file (creates if it doesn't exist) and Appends data to the end of it
  if(dataFile) {
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
  else {
    Serial.print("Error opening: ");
    Serial.println(fileName + ".csv");
    return 0; //returns a 0, failure
  }
}

void printData(){
  Serial.print(data_rcv.nodeID);
  Serial.print(", time: ");
  Serial.print(thisUnixTime());
  Serial.print(", sensor_val: ");
  Serial.print(data_rcv.sens_val);
  Serial.print(", RFM_temp: ");
  Serial.print(data_rcv.temp);
  Serial.print(", Excite Voltage ");
  Serial.print(data_rcv.excite_v);
  Serial.print(", Battery Voltage: ");
  Serial.print(data_rcv.batt_v);
  Serial.print(", num_attempts, ");
  Serial.print(data_rcv.num_attempts);
  Serial.println();
}

void Blink(byte PIN, int DELAY_MS) {
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
  delay(DELAY_MS);
}




















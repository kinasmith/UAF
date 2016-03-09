#include <RFM69.h>
#include <SPI.h>
#include <DS3231.h>
#include <SD.h>


#define NODEID      1
#define NETWORKID   100
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "p6ZNvTmGfdY2hUXb" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9
#define SERIAL_BAUD 9600
#define ACK_TIME    50  // # of ms to wait for an ack


/*==============|| DS3231_RTC ||==============*/
DS3231  rtc(SDA, SCL);

/*==============|| SD ||==============*/
const int chipSelect = 4;


/*==============|| RFM69 ||==============*/
RFM69 radio;
byte ackCount=0;
const bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network

typedef struct {    
  int nodeId; //store this nodeId
  float temp;
  float voltage;
} 
Payload;
Payload theData;


void setup() {
  Serial.begin(9600);
  rtc.begin();
  pinMode(10, OUTPUT);
  if(!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized");

  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY);
  Serial.println("Setup Done");
}

void loop() {

  /*==============|| RADIO Recieve ||==============*/
  if (radio.receiveDone()) {
    radioReceive();
    writeDataToCard();
    if (radio.ACKRequested()) {
      ACKsend();
    }
    Blink(LED,3);
  }
}

void radioReceive() {
  if(radio.DATALEN != sizeof(Payload)) {
    Serial.println("Invalid payload received, not matching Payload struct!");
  }
  else {
    theData = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
    Serial.print(theData.nodeId);
    Serial.print(", RFM_temp: ");
    Serial.print(theData.temp);
    Serial.print(", Battery Voltage: ");
    Serial.print(theData.voltage);
    Serial.print(", time: ");
    Serial.print(theUnixTime());
    Serial.println();
  }
}

void ACKsend(){
  //  byte theNodeID = radio.SENDERID;
  radio.sendACK();
  //Serial.print(" - ACK sent.");
  // When a node requests an ACK, respond to the ACK
  // and also send a packet requesting an ACK (every 3rd one only)
  // This way both TX/RX NODE functions are tested on 1 end at the GATEWAY
  if (ackCount++%3==0) {
    //   Serial.print(" Pinging node ");
    //Serial.print(theNodeID);
    //Serial.print(" - ACK...");
    delay(3); //need this when sending right after reception .. ?
    //    if (radio.sendWithRetry(theNodeID, "ACK TEST", 8, 0))  // 0 = only 1 attempt, no retries
    // Serial.print("ok!");
    // else Serial.print("nothing");
  }
}
void Blink(byte PIN, int DELAY_MS) {
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
long theUnixTime(){
  return rtc.getUnixTime(rtc.getTime());
}
void writeDataToCard() {
  String fileName = String(String(theData.nodeId) + ".csv");
  char __fileName[fileName.length()+1];
  fileName.toCharArray(__fileName, sizeof(__fileName));
  File dataFile = SD.open(__fileName, FILE_WRITE);
  if(dataFile) {
    dataFile.print(theData.nodeId);
    dataFile.print(", ");
    dataFile.print(theData.temp);
    dataFile.print(", ");
    dataFile.print(theData.voltage);
    dataFile.print(", ");
    dataFile.print(theUnixTime());
    dataFile.println();
    dataFile.close();
    Serial.println("printed data to file");
  } 
  else {
    Serial.print("Error opening ");
    Serial.println(fileName);
  }
}






#include <RFM69.h>
#include <SPI.h>
#include <DS3231.h>
#include <SD.h>


#define NODE_ID      0 //Network Node ID. All Senders must send to this 
#define NETWORK_ID   100
#define FREQUENCY   RF69_433MHZ //Match this to the frequency of your radio
#define KEY         "p6ZNvTmGfdY2hUXb" //has to be same 16 characters/bytes on all nodes.
#define LED         3
#define SERIAL_BAUD 9600

/*==============|| DS3231_RTC ||==============*/
DS3231  rtc(SDA, SCL);

/*==============|| SD ||==============*/
const int chipSelect = 4;
const int CARD_DETECT = 5;
int cardPresent;

/*==============|| RFM69 ||==============*/
RFM69 radio;
byte ackCount=0;
const bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network

//pre-defined packet structure
typedef struct {    
  int nodeID; //store this nodeId
  float temp;
  float voltage;
  int num_attempts;
  int sens_val;
} 
Payload;
Payload data_rcv;

void setup() {
  Serial.begin(9600);
  rtc.begin();

  if(!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    cardPresent = digitalRead(CARD_DETECT);
    while(cardPresent == 0) {
      cardPresent = digitalRead(CARD_DETECT);
      Blink(LED, 100);
    }
    if(!SD.begin(chipSelect))   Serial.println("SD card initialized");
  }
  Serial.println("SD card initialized");

  radio.initialize(FREQUENCY,NODE_ID,NETWORK_ID);
  radio.setHighPower(); 
  radio.encrypt(KEY);
  Serial.println("RFM69 Initialized");
  Serial.print("\tEncrypt Key: ");
  Serial.println(KEY);
}

void loop() {
  //  checkSDCard(CARD_DETECT);
  /*==============|| RADIO Recieve ||==============*/
  if (radio.receiveDone()) {
    if(radio.DATALEN == sizeof(Payload)) {
      // convert the radio's raw byte array to our pre-defined Packet struct
      data_rcv = *(Payload*)radio.DATA;
      int senderID = radio.SENDERID;
      // if requested, acknowledge that the packet was received
      if (radio.ACKRequested()) {
        radio.sendACK();
      }
      printData();
      if(writeDataToCard(data_rcv.nodeID, data_rcv.temp, data_rcv.voltage, thisUnixTime(), data_rcv.sens_val, data_rcv.num_attempts)){
        Blink(LED, 100);
      }
    }
    else {
      Serial.println("Invalid packet received!!");
      if(writeDataToCard(00,99.99,99.99,thisUnixTime(), 99, 99)) {
        Blink(LED, 25);
        Blink(LED, 25);
      }
    }
  }
}

long thisUnixTime(){
  return rtc.getUnixTime(rtc.getTime());
}

int writeDataToCard(int id, float t, float v, long utm, int s_v, int num_a) {
  //Check to make sure card is inserted
  cardPresent = digitalRead(CARD_DETECT);
  if(cardPresent == 0) {
    Serial.println("Card Not Present");
    while(cardPresent == 0) {
      cardPresent = digitalRead(CARD_DETECT);
      Blink(LED, 100);
    }
    if(!SD.begin(chipSelect))   Serial.println("SD card initialized");
    else Serial.println("SD Card Failed to Initialize");
  }

  //Write to card
  String fileName = String(String(id) + ".csv");
  char __fileName[fileName.length()+1];
  fileName.toCharArray(__fileName, sizeof(__fileName));
  File dataFile = SD.open(__fileName, FILE_WRITE);
  if(dataFile) {
    dataFile.print(id);
    dataFile.print(", ");
    dataFile.print(t);
    dataFile.print(", ");
    dataFile.print(v);
    dataFile.print(", ");
    dataFile.print(utm);
    dataFile.print(", ");
    dataFile.print(num_a);
    dataFile.print(", ");
    dataFile.print(s_v);
    dataFile.println();
    dataFile.close();
    return 1;
  } 
  else {
    Serial.print("Error opening: ");
    Serial.println(fileName + ".csv");
    return 0;
  }
}

void printData(){
  Serial.print(data_rcv.nodeID);
  Serial.print(", RFM_temp: ");
  Serial.print(data_rcv.temp);
  Serial.print(", Battery Voltage: ");
  Serial.print(data_rcv.voltage);
  Serial.print(", time: ");
  Serial.print(thisUnixTime());
  Serial.print(", sensor_val: ");
  Serial.print(data_rcv.sens_val);
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

int checkSDCard(int _cd) {
  int cd = digitalRead(_cd);
  while(cd == 0) {
    cd = digitalRead(_cd);
    Blink(LED, 100);
  }
  if(!SD.begin(chipSelect)) {
    return 1;
  } 
  else {
    if(SD.begin(chipSelect)) { 
      return 0;
    }
  }
}















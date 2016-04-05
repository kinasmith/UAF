#include <RFM69.h>
#include <SPI.h>
#include <DS3231.h>
#include <SD.h>


#define NODE_ID      0
#define NETWORK_ID   100
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "p6ZNvTmGfdY2hUXb" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         3
#define SERIAL_BAUD 9600

/*==============|| DS3231_RTC ||==============*/
DS3231  rtc(SDA, SCL);

/*==============|| SD ||==============*/
const int chipSelect = 4;

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
    while(true) { //stop forever and blink LED
      Blink(LED, 100);
    }
  }
  Serial.println("SD card initialized");

  radio.initialize(FREQUENCY,NODE_ID,NETWORK_ID);
  radio.setHighPower(); 
  radio.encrypt(KEY);
  Serial.println("Setup Done");
}

void loop() {
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
            writeDataToCard(data_rcv.nodeID, data_rcv.temp, data_rcv.voltage, thisUnixTime(), data_rcv.sens_val, data_rcv.num_attempts);
            Blink(LED, 100);
        }
        else {
            Serial.println("Invalid packet received!!");
            writeDataToCard(00,99.99,99.99,thisUnixTime(), 99, 99);
        }
    }
}


long thisUnixTime(){
  return rtc.getUnixTime(rtc.getTime());
}

void writeDataToCard(int id, float t, float v, long utm, int s_v, int num_a) {
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
  } 
  else {
    Serial.print("Error opening: ");
    Serial.println(fileName + ".csv");
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









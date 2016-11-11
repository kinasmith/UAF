#include <RFM69_ATC.h>//get it here: https://www.github.com/lowpowerlab/rfm69
// #include <RFM69.h>
#include <SPI.h>
#include "DS3231.h"
#include "SdFat.h"

#define NODEID        0    //unique for each node on same network
#define FREQUENCY     RF69_433MHZ
#define ATC_RSSI      -90
#define SERIAL_BAUD   9600
#define LED           9 // Moteinos have LEDs on D9, but for MotionMote we are using the external led on D5
#define SD_CS_PIN 4
#define CARD_DETECT 5
#define debug 1

/*==============|| RFM69 ||==============*/
RFM69_ATC radio; //Initialize Radio
// RFM69 radio;
uint8_t NETWORKID = 100; //base network address

/*==============|| DS3231_RTC ||==============*/
DS3231 rtc; //Initialize the Real Time Clock

/*==============|| SD ||==============*/
SdFat SD; //This is the sd Card
uint8_t CARD_PRESENT; //var for Card Detect sensor reading

byte lastRequesterNodeID = NODEID;

typedef struct TimeStamp {
    uint32_t timestamp;
};
TimeStamp theTimeStamp;

typedef struct Payload {
   int16_t temp;
   int16_t humidity;
   int16_t voltage;
};
Payload theData;

void setup() {
    pinMode(LED, OUTPUT);
    pinMode(CARD_DETECT, INPUT_PULLUP);
    // NETWORKID += setAddress();
    rtc.begin(); //Begins RTC Library
    // if (debug)  rtc.adjust(DateTime((__DATE__), (__TIME__))); //sets the RTC to the computer time.
    if(debug) Serial.begin(SERIAL_BAUD);
    radio.initialize(FREQUENCY,NODEID,NETWORKID);
    radio.setHighPower(); //only for RFM69HW!
    radio.enableAutoPower(ATC_RSSI);
    radio.encrypt(null);
    if (debug) {
        Serial.print("RFM69 initialized");
        Serial.print("Network Address: ");
        Serial.print(NETWORKID);
        Serial.print(".");
        Serial.println(NODEID);
    }

    digitalWrite(LED, HIGH);
    pinMode(10, OUTPUT);
    if (!SD.begin(SD_CS_PIN)) {
        if (debug) Serial.println("Card failed, or not present");
        CARD_PRESENT = digitalRead(CARD_DETECT);  //read CD pin    
        // this delays the program while there is no card inserted.
        while (CARD_PRESENT == 1) { //The CD pin is 1 if there is no card inserted
            CARD_PRESENT = digitalRead(CARD_DETECT); //read it the value for every loop, or we will never exit the While Loop
            Blink(LED, 100); //blink to show an error.
            Blink(LED, 200); //blink to show an error.
        }
        delay(10); //wait for card to stablize from insertion before initializing
    }
    digitalWrite(LED, LOW);
    if (debug) Serial.println("sd card initialized"); ///Victory
    Serial.println("==========================");
}

byte ackCount=0;
DateTime now;

void loop() {
    if (radio.receiveDone())
    {
        boolean writeDataBool = false;
        boolean reportTimeRequest = false;
        lastRequesterNodeID = radio.SENDERID;
        now = rtc.now();
        theTimeStamp.timestamp = now.unixtime();
        if(debug) {
            Serial.print("rcv - ");Serial.print('[');Serial.print(radio.SENDERID);Serial.print("] ");
            Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.println("]");
        }
        if(radio.DATALEN == 1 && radio.DATA[0] == 't') {
            reportTimeRequest = true;
        }
        if (radio.DATALEN == sizeof(theData)) {
            theData = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
            writeDataBool = true;
            if(debug) {
                Serial.print("rcv - ");Serial.print('[');Serial.print(radio.SENDERID);Serial.print("] ");
                printDate(now);
                Serial.print(" t:");
                Serial.print(theData.temp/100.0);
                Serial.print(" h: ");
                Serial.print(theData.humidity/100.0);
                Serial.print(" v: ");
                Serial.print(theData.voltage/100.0);
                Serial.println();
            }
        }

        if (radio.ACKRequested()) radio.sendACK();

        if(reportTimeRequest) {
            if(radio.sendWithRetry(lastRequesterNodeID, (const void*)(&theTimeStamp), sizeof(theTimeStamp))) {
                if(debug) {
                    Serial.print("snd - "); Serial.print('[');Serial.print(lastRequesterNodeID);Serial.print("] ");
                    Serial.println(theTimeStamp.timestamp);
                }
                reportTimeRequest = false;
            } else {
                if(debug) Serial.println("snd - Failed . . . no ack");
            }
        if(writeDataBool)
            writeData();
        }
        writeLocalTemp(now.unixtime());
        Blink(LED,5);
    }
    checkSdCard();
}

void writeLocalTemp(uint32_t n) {
    float t;
    File f;
    rtc.convertTemperature();
    t = rtc.getTemperature();
    String address = String(String(NETWORKID) + "_0");
    String fileName = String(address + ".csv");
    char _fileName[fileName.length() +1];
    fileName.toCharArray(_fileName, sizeof(_fileName));
    if (!f.open(_fileName, FILE_WRITE)) {
        if(debug) {
            Serial.print("error opening ");
            Serial.print(_fileName);
            Serial.println(" for write");
        }
    }
    f.print(NETWORKID);
    f.print(".");
    f.print("0");
    f.print(",");
    f.print(n);
    f.print(",");
    f.print(t);
    f.println();
    // close the file:
    f.close();
}

void writeData() {
    File f;
    //write recieved data to sd card
    String address = String(String(NETWORKID) + "_" + String(radio.SENDERID));
    String fileName = String(address + ".csv");
    char _fileName[fileName.length() +1];
    fileName.toCharArray(_fileName, sizeof(_fileName));
    if (!f.open(_fileName, FILE_WRITE)) {
        if(debug) {
            Serial.print("error opening ");
            Serial.print(_fileName);
            Serial.println(" for write");
        }
    }
    // if the file opened okay, write to it:
    f.print(NETWORKID);
    f.print(".");
    f.print(radio.SENDERID);
    f.print(",");
    f.print(now.unixtime());
    f.print(",");
    f.print(theData.temp/100.0);
    f.print(",");
    f.print(theData.humidity/100.0);
    f.print(",");
    f.print(theData.voltage/100.0);
    f.println();
    // close the file:
    f.close();
}

void checkSdCard() {
    //Checks for card insertion at every round of the loop.
    CARD_PRESENT = digitalRead(CARD_DETECT);
    if (CARD_PRESENT == 1) {
        if (debug) Serial.println("Card Not Present");
        while (CARD_PRESENT == 1) {
            Blink(LED, 100);
            Blink(LED, 200); //blink to show an error.
        }
    }
}


void printDate(DateTime n) {
    //print formated date and time from DATETIME object
      Serial.print(n.year());
      Serial.print("-");
      Serial.print(n.month());
      Serial.print("-");
      Serial.print(n.date());
      Serial.print(" ");
      Serial.print(n.hour());
      Serial.print(":");
      Serial.print(n.minute());
      Serial.print(":");
      Serial.print(n.second());
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

uint8_t setAddress() {
    //sets network address based on which solder jumpers are closed
    uint8_t addr01, addr02, addr03;
    pinMode(6, INPUT_PULLUP);
    pinMode(7, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);
    addr01 = !digitalRead(8);
    addr02 = !digitalRead(6) * 2;
    addr03 = !digitalRead(7) * 4;
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
    uint8_t addr = addr01 | addr02 | addr03;
    return addr;
}
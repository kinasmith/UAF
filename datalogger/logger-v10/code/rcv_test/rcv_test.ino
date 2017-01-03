#include <SPI.h>
#include "SdFat.h"
#include "RFM69.h"
#include "DS3231.h"

#define NODEID      0
#define FREQUENCY   RF69_433MHZ
#define LED         9
#define SERIAL_BAUD 9600
#define debug 1
#define SD_CS_PIN 4
#define CARD_DETECT 5

/*==============|| DS3231_RTC ||==============*/
DS3231 rtc; //Initialize the Real Time Clock

/*==============|| SD ||==============*/
SdFat SD; //This is the sd Card
uint8_t CARD_PRESENT; //var for Card Detect sensor reading

/*==============|| RFM69 ||==============*/
RFM69 radio; //Initialize Radio
uint8_t NETWORKID = 100; //base network address

typedef struct {
   int16_t temp;
   int16_t humidity;
   int16_t voltage;
} Payload;
Payload theData;

byte lastRequesterNodeID = NODEID;

void setup() {
    pinMode(LED, OUTPUT);
    pinMode(CARD_DETECT, INPUT_PULLUP);
    NETWORKID += setAddress();
    if (debug) Serial.begin(9600); //Begins Serial
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
    rtc.begin(); //Begins RTC Library
    // if (debug)  rtc.adjust(DateTime((__DATE__), (__TIME__))); //sets the RTC to the computer time.
    radio.initialize(FREQUENCY, NODEID, NETWORKID); //Initialize Radio
    radio.setHighPower(); //Set it to high Power Mode
    radio.encrypt(null);
    if (debug) {
        Serial.print("RFM69 initialized");
        Serial.print("Network Address: ");
        Serial.print(NETWORKID);
        Serial.print(".");
        Serial.println(NODEID);
    }
    if (debug) {
        DateTime now = rtc.now(); //get the current time
        Serial.print("Current time is: ");
        Serial.print(now.year());
        Serial.print("-");
        Serial.print(now.month());
        Serial.print("-");
        Serial.print(now.date());
        Serial.print(" ");
        Serial.print(now.hour());
        Serial.print(":");
        Serial.print(now.minute());
        Serial.print(":");
        Serial.print(now.second());
        Serial.println();
        Serial.println(now.unixtime());
    }
    Serial.println("==========================");
}

void loop() {
    if (radio.receiveDone()) {
        DateTime now = rtc.now();
        uint8_t thisNodeID = radio.SENDERID; //this is now a duplicate
        if (radio.DATALEN == sizeof(theData)) {
            theData = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
            //break down uint_16's into floats again
            float h = theData.humidity/100.0; 
            float t = theData.temp/100.0;
            float v = theData.voltage/100.0;
            writeData(now.unixtime(), thisNodeID, t, h, v);
            Serial.print(thisNodeID);
            Serial.print(" -- ");
            Serial.print("time: ");
            // printDate(now);
            Serial.print(" temp: ");
            Serial.print(t);
            Serial.print(" humidity: ");
            Serial.print(h);
            Serial.print(" voltage: ");
            Serial.print(v);
            Serial.println();
        } else {
            Serial.print("Invalid payload received, not matching Payload struct!");
            writeError(now.unixtime(), thisNodeID);
        }
        if (radio.ACKRequested()) {
            radio.sendACK();
        }
        writeLocalTemp(now.unixtime());
        Blink(LED,5);
    }
    checkSdCard();
}

void writeError(uint32_t n, uint8_t id) {
    File f;
    String address = String(String(NETWORKID) + "_" + String(id));
    String fileName = String(address + "_error.csv");
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
    f.print(id);
    f.print(",");
    f.print(n);
    f.println();
    f.close();
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

void writeData(uint32_t n, uint8_t id, float temp, float humid, float voltage) {
    File f;
    //write recieved data to sd card
    String address = String(String(NETWORKID) + "_" + String(id));
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
    f.print(id);
    f.print(",");
    f.print(n);
    f.print(",");
    f.print(temp);
    f.print(",");
    f.print(humid);
    f.print(",");
    f.print(voltage);
    f.println();
    // close the file:
    f.close();
}

void Blink(uint8_t PIN, uint8_t DELAY_MS) {
    digitalWrite(PIN,HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN,LOW);
    delay(DELAY_MS);
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

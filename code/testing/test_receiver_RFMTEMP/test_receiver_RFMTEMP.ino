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
bool CD_STATE = 0;

/*==============|| RFM69 ||==============*/
RFM69 radio;
byte ackCount=0;
const bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network

typedef struct {    
  int nodeID; //store this nodeId
  float temp;
  float voltage;
} 
Payload;
Payload data_rcv;

void setup() {
	Serial.begin(9600);
	rtc.begin();
	pinMode(10, OUTPUT);

	if(!SD.begin(chipSelect)) {
		while(true) { //stop forever and blink LED
			Blink(LED, 100);
		}
	}

	Serial.println("SD card initialized");

	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower(); //uncomment only for RFM69HW!
	radio.encrypt(KEY);
	Serial.println("Setup Done");
}

void loop() {
  /*==============|| RADIO Recieve ||==============*/
	if (radio.receiveDone()) {
		if(radio.DATALEN != sizeof(Payload)) {
			Serial.println("Invalid payload received!!");
			writeDataToCard(00,99.99,99.99,thisUnixTime());
		}
		else {
			data_rcv = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
			printData();
			writeDataToCard(data_rcv.nodeID, data_rcv.temp, data_rcv.voltage, thisUnixTime());
		}
		if (radio.ACKRequested()) {
			radio.sendACK();
			if (ackCount++%3==0) delay(3); 
		}
		Blink(LED,100);
	}
}

long thisUnixTime(){
  return rtc.getUnixTime(rtc.getTime());
}

void writeDataToCard(int id, float t, float v, long utm) {
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
		dataFile.println();
		dataFile.close();
	} 
	else {
		for(int i = 0; i < 5; i++) Blink(LED, 100);
		Serial.print("Error opening ");
		Serial.println(fileName);
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
    Serial.println();
}

void Blink(byte PIN, int DELAY_MS) {
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
  delay(DELAY_MS);
}







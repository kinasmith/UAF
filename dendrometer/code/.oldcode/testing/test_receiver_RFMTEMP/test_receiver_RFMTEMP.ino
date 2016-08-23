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
#define ACK_TIME    30  // # of ms to wait for an ack

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
  int num_attempts;
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
			writeDataToCard(00,99.99,99.99,thisUnixTime(), 99);
		}
		else {
			data_rcv = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
			printData();
			writeDataToCard(data_rcv.nodeID, data_rcv.temp, data_rcv.voltage, thisUnixTime(), data_rcv.num_attempts);
		}

		if (radio.ACKRequested()) {
			byte theNodeID = radio.SENDERID;
			radio.sendACK();
			Serial.print(" - ACK sent.");

			// When a node requests an ACK, respond to the ACK
			// and also send a packet requesting an ACK (every 3rd one only)
			// This way both TX/RX NODE functions are tested on 1 end at the GATEWAY
			if (ackCount++%3==0) {
				Serial.print(" Pinging node ");
				Serial.print(theNodeID);
				Serial.print(" - ACK...");
				delay(3); //need this when sending right after reception .. ?

				if (radio.sendWithRetry(theNodeID, "ACK TEST", 8, 0))  // 0 = only 1 attempt, no retries
					Serial.print("ok!");
				else Serial.print("nothing");
			}
		}
		Serial.println();
		// if (radio.ACKRequested()) {
		// 	radio.sendACK();
		// 	if (ackCount++%3==0) delay(3); 
		// }
		Blink(LED,100);
	}
}


//   if (radio.receiveDone())
//   {
//     Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
//     Serial.print(" [RX_RSSI:");Serial.print(radio.readRSSI());Serial.print("]");
//     if (promiscuousMode)
//     {
//       Serial.print("to [");Serial.print(radio.TARGETID, DEC);Serial.print("] ");
//     }

//     if (radio.DATALEN != sizeof(Payload))
//       Serial.print("Invalid payload received, not matching Payload struct!");
//     else
//     {
//       theData = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
//       Serial.print(" nodeId=");
//       Serial.print(theData.nodeId);
//       Serial.print(" uptime=");
//       Serial.print(theData.uptime);
//       Serial.print(" temp=");
//       Serial.print(theData.temp);
//     }
    
//     if (radio.ACKRequested())
//     {
//       byte theNodeID = radio.SENDERID;
//       radio.sendACK();
//       Serial.print(" - ACK sent.");

//       // When a node requests an ACK, respond to the ACK
//       // and also send a packet requesting an ACK (every 3rd one only)
//       // This way both TX/RX NODE functions are tested on 1 end at the GATEWAY
//       if (ackCount++%3==0)
//       {
//         Serial.print(" Pinging node ");
//         Serial.print(theNodeID);
//         Serial.print(" - ACK...");
//         delay(3); //need this when sending right after reception .. ?
//         if (radio.sendWithRetry(theNodeID, "ACK TEST", 8, 0))  // 0 = only 1 attempt, no retries
//           Serial.print("ok!");
//         else Serial.print("nothing");
//       }
//     }
//     Serial.println();
//     Blink(LED,3);
//   }
// }




long thisUnixTime(){
  return rtc.getUnixTime(rtc.getTime());
}

void writeDataToCard(int id, float t, float v, long utm, int num_a) {
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







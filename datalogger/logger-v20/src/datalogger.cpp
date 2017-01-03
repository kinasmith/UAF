#include <RFM69_ATC.h>//get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>
#include "DS3231.h"
#include "SdFat.h"

#define NODEID                 0
#define FANID                     55
#define FREQUENCY       RF69_433MHZ
#define ATC_RSSI            -70
#define ACK_WAIT_TIME   100 // # of ms to wait for an ack
#define ACK_RETRIES     10 // # of attempts before giving up
#define SERIAL_BAUD    9600
#define LED                        9
#define SD_CS_PIN         4
#define CARD_DETECT 5
#define FAN_ON_TEMP 18 //Temp when fan turns on in Celcius
#define FAN_OFF_TEMP 15 //Temp when fan turns on in Celcius

#define SERIAL_EN //Comment this out to remove Serial comms and save a few kb's of space

#ifdef SERIAL_EN
#define DEBUG(input)   {Serial.print(input); delay(1);}
#define DEBUGln(input) {Serial.println(input); delay(1);}
#define DEBUGFlush() { Serial.flush(); }
#else
#define DEBUG(input);
#define DEBUGln(input);
#define DEBUGFlush();
#endif

// Functions
uint8_t setAddress();
void Blink(byte, int);
void checkSdCard();

/*==============|| RFM69 ||==============*/
RFM69_ATC radio; //Initialize Radio
uint8_t NETWORKID = 100; //base network address

/*==============|| DS3231_RTC ||==============*/
DateTime now;
DS3231 rtc; //Initialize the Real Time Clock

/*==============|| SD ||==============*/
SdFat SD; //This is the sd Card
uint8_t CARD_PRESENT; //var for Card Detect sensor reading

byte lastRequesterNodeID = NODEID;
// uint16_t downstairs_temp, upstairs_temp;
bool fan = true;
bool last_fan = false;

typedef struct TimeStamp {
	uint32_t timestamp;
};
TimeStamp theTimeStamp;

typedef struct Payload {
	uint16_t count;
	uint32_t timestamp;
	int16_t temp;
	int16_t humidity;
	int16_t voltage;
};
Payload thePayload;

void setup() {
	bool sd_OK = false;
	pinMode(LED, OUTPUT);
	pinMode(CARD_DETECT, INPUT_PULLUP);
	NETWORKID += setAddress();
	rtc.begin();
	// rtc.adjust(DateTime((__DATE__), (__TIME__))); //sets the RTC to the computer time.
	Serial.begin(SERIAL_BAUD);
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower(); //only for RFM69HW!
	radio.enableAutoPower(ATC_RSSI);
	radio.encrypt(null);
	DEBUG("Listening at 433mhz, "); DEBUG("Network Address: ");
	DEBUG(NETWORKID); DEBUG("."); DEBUGln(NODEID);
	digitalWrite(LED, HIGH);
	CARD_PRESENT = !digitalRead(CARD_DETECT); //read CD pin (invert it so logic stays logical)
	if(CARD_PRESENT) {
		DEBUGln("Card Present OK");
		if (SD.begin(SD_CS_PIN)) {
			DEBUGln("Sd.begin OK");
			File f;
			now = rtc.now();
			if(f.open("start.txt", FILE_WRITE)) {
				DEBUG("File Open OK at ");
				DEBUGln(now.unixtime());
				f.print("program started at ");
				f.print(now.unixtime());
				f.println();
				f.close();
				sd_OK = true;
			}
		}
	}
	if(!sd_OK) {
		while(1) {
			Blink(LED, 100); //blink to show an error.
			Blink(LED, 200); //blink to show an error.
		}
	}
	digitalWrite(LED, LOW);
	DEBUGln("sd card initialized"); ///Victory
	DEBUGln("==========================");
}

void loop() {
	bool writeData = false;
	bool reportTime = false;

	if (radio.receiveDone()) {
		lastRequesterNodeID = radio.SENDERID;
		now = rtc.now();
		theTimeStamp.timestamp = now.unixtime();
		if(radio.DATALEN == 1 && radio.DATA[0] == 't') {
			DEBUG("rcv - "); DEBUG('['); DEBUG(radio.SENDERID); DEBUG("] "); DEBUG("t  ->  ");
			reportTime = true;
		}
		if (radio.DATALEN == sizeof(thePayload)) {
			thePayload = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
			writeData = true;
			DEBUG("rcv - "); DEBUG('['); DEBUG(radio.SENDERID); DEBUG("] "); DEBUG(thePayload.timestamp);
			DEBUG(" t:"); DEBUG(thePayload.temp/100.0); DEBUG(" h: "); DEBUG(thePayload.humidity/100.0);
			DEBUG(" v: "); DEBUG(thePayload.voltage/100.0); DEBUG(" c: "); DEBUG(thePayload.count); DEBUGln();
			if(radio.SENDERID == 9) {//downstairs temperature
				if(thePayload.temp/100.0 > FAN_ON_TEMP) { //divide by 100 to get back to actual temperature in C
					fan = true;
				}
				if(thePayload.temp/100.0 < FAN_OFF_TEMP) {
					fan = false;
				}
			}
			// downstairs_temp = thePayload.temp;
			// if(radio.SENDERID == 1)
			//     upstairs_temp = thePayload.temp;
			// fan = fanOn(downstairs_temp, upstairs_temp);
		}
		if (radio.ACKRequested()) radio.sendACK();
		Blink(LED,5);
	}

	if(reportTime) {
		DEBUG("snd - "); DEBUG('['); DEBUG(lastRequesterNodeID); DEBUG("] ");
		if(radio.sendWithRetry(lastRequesterNodeID, (const void*)(&theTimeStamp), sizeof(theTimeStamp), ACK_RETRIES, ACK_WAIT_TIME)) {
			DEBUGln(theTimeStamp.timestamp);
		} else {
			DEBUGln("Failed . . . no ack");
		}
	}

	if(last_fan != fan) {
		DEBUG("snd - FAN: ");
		if(radio.sendWithRetry(FANID, &fan, sizeof(fan), ACK_RETRIES, ACK_WAIT_TIME)) {
			DEBUGln(fan);
		}else {
			//if it dies, just leave it. Try again next time it's suppose to switch
			//create some sort of escape here. If there is no ACK the whole system dies because the logic never resolves.`
			DEBUGln(" Failed . . . no ack");
		}
		last_fan = fan;
	}

	if(writeData) {
		File f;
		String address = String(String(NETWORKID) + "_" + String(lastRequesterNodeID));
		String fileName = String(address + ".csv");
		char _fileName[fileName.length() +1];
		fileName.toCharArray(_fileName, sizeof(_fileName));
		if (!f.open(_fileName, FILE_WRITE)) { DEBUG("sd - error opening "); DEBUG(_fileName); DEBUGln(); }
		// if the file opened okay, write to it:
		DEBUG("sd - writing to "); DEBUG(_fileName); DEBUGln();
		f.print(NETWORKID); f.print(".");
		f.print(radio.SENDERID); f.print(",");
		f.print(thePayload.timestamp); f.print(",");
		f.print(thePayload.temp/100.0); f.print(",");
		f.print(thePayload.humidity/100.0); f.print(",");
		f.print(thePayload.voltage/100.0); f.print(",");
		f.print(thePayload.count); f.println();
		f.close();
	}
	checkSdCard(); //Checks for card insertion
}

// bool fanOn(uint16_t d, uint16_t u) {
//     if(u > d)
//         return 0;
//     if(d > u)
//         return 1;
// }

void checkSdCard() {
	CARD_PRESENT = !digitalRead(CARD_DETECT); //invert for logic's sake
	if (!CARD_PRESENT) {
		DEBUGln("sd - card Not Present");
		while (1) {
			Blink(LED, 100);
			Blink(LED, 200); //blink to show an error.
		}
	}
}

void Blink(byte PIN, int DELAY_MS) {
	digitalWrite(PIN,HIGH);
	delay(DELAY_MS);
	digitalWrite(PIN,LOW);
	delay(DELAY_MS);
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

// writeLocalTemp(now.unixtime());
// void writeLocalTemp(uint32_t n) {
//     float t;
//     File f;
//     rtc.convertTemperature();
//     t = rtc.getTemperature();
//     String address = String(String(NETWORKID) + "_0");
//     String fileName = String(address + ".csv");
//     char _fileName[fileName.length() +1];
//     fileName.toCharArray(_fileName, sizeof(_fileName));
//     if (!f.open(_fileName, FILE_WRITE)) {
//         DEBUG("error opening ");
//         DEBUG(_fileName);
//         DEBUGln(" for write");
//     }
//     f.print(NETWORKID);
//     f.print(".");
//     f.print("0");
//     f.print(",");
//     f.print(n);
//     f.print(",");
//     f.print(t);
//     f.println();
//     f.close(); // close the file:
// }

#include <SPI.h>
#include "Nanoshield_Termopar.h"
#include <SdFat.h>
#include "RTClib.h"
/*==============|| SD ||==============*/
SdFat sd; //This is the SD Card
SdFile file; //this is the DataFile
#define FILE_BASE_NAME "type_e"
const int SD_CS = 3; //SPI SS pin for SD card

/*==============|| ThermoCouples ||==============*/
const int Tc1_CS = 5; //downstream
const int Tc2_CS = 6; //to the side
const int Tc3_CS = 7; //upstream
const int Tc4_CS = 8; //upstream
Nanoshield_Termopar Tc3(Tc3_CS, TC_TYPE_T, TC_AVG_4_SAMPLES);
Nanoshield_Termopar Tc4(Tc4_CS, TC_TYPE_E, TC_AVG_4_SAMPLES);
float Tc3t;
float Tc4t;

/*==============|| Variables ||==============*/
const int LED = 9;
unsigned long current_time;
unsigned long log_saved_time;
int log_interval = 1000;
long utm = 0;


void setup() {
	Serial.begin(9600);
	pinMode(SD_CS, OUTPUT);

	Tc3.begin();
	Tc4.begin();
	delay(100);
	Serial.print("Initializing SD card...");
	if (!sd.begin(SD_CS, SPI_HALF_SPEED)) {
		sd.initErrorHalt();
	}
	Serial.println("card initialized.");

	const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
	char fileName[13] = FILE_BASE_NAME "00.csv";
	if (BASE_NAME_SIZE > 6) {
		Serial.println("FILE_BASE_NAME too long");
	}
	while (sd.exists(fileName)) {
		if (fileName[BASE_NAME_SIZE + 1] != '9') {
			fileName[BASE_NAME_SIZE + 1]++;
		} else if (fileName[BASE_NAME_SIZE] != '9') {
			fileName[BASE_NAME_SIZE + 1] = '0';
			fileName[BASE_NAME_SIZE]++;
		} else {
			Serial.println("Can't create file name");
		}
	}
	if (!file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) {
		Serial.println("file.open");
	}
	Serial.print("Current time is: "); Serial.println(utm);
	Serial.print("Writing to file: "); Serial.println(fileName);
}

void loop() {
	current_time = millis();
	utm = current_time/1000;
	if (log_saved_time + log_interval < current_time) {
		Tc3.read();
		Tc4.read();
		Tc3t = Tc3.getExternal();
		Tc4t = Tc4.getExternal();
		writeToSerial(utm, Tc3t, Tc4t);
		writeToSd(utm, Tc3t, Tc4t);
		if (!file.sync() || file.getWriteError()) {
			Serial.println("write error");
		}
		log_saved_time = current_time;
		}
}

void writeToSerial(long t, float t3, float t4) {
  Serial.print(t);
  Serial.print(", Type T temp: ");
  Serial.print(t3);
  Serial.print(", Type E temp: ");
  Serial.print(t4);
  Serial.println();
}

void writeToSd(long t, float t3, float t4) {
  //Print data to new line
  file.print(t); file.print(",");
  file.print(t3); file.print(",");
  file.print(t4); file.print(",");
  file.println();
}

void Blink(byte PIN, int DELAY_MS) {
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN, LOW);
  delay(DELAY_MS);
}

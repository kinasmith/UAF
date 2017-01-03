#include <SPI.h>
#include "Nanoshield_Termopar.h"
#include <SdFat.h>
#include "RTClib.h"

/*==============|| DS3231_RTC ||==============*/
RTC_DS3231 rtc;
DateTime now;
DateTime startTime;

/*==============|| SD ||==============*/
SdFat sd; //This is the SD Card
SdFile file; //this is the DataFile
const int SD_CS = 3; //SPI SS pin for SD card

/*==============|| ThermoCouples ||==============*/
const int Tc1_CS = 5; //downstream
const int Tc2_CS = 6; //to the side
const int Tc3_CS = 7; //upstream
Nanoshield_Termopar Tc1(Tc1_CS, TC_TYPE_T, TC_AVG_4_SAMPLES);
Nanoshield_Termopar Tc2(Tc2_CS, TC_TYPE_T, TC_AVG_4_SAMPLES);
Nanoshield_Termopar Tc3(Tc3_CS, TC_TYPE_T, TC_AVG_4_SAMPLES);

/*==============|| Variables ||==============*/
const int LED = 9;
const int H_EN = 4; //heater enable pin
unsigned long current_time;
unsigned long h_saved_time; //saved time for heater timing
const int h_pulse_on = 6000; //six seconds on
const long h_pulse_off = 30 * 60000L; //30 minutes off
// const int h_pulse_on = 6000; //six seconds on
// const long h_pulse_off = 1000; //30 minutes off
long h_interval = 0;
bool h_status = 0;

unsigned long log_saved_time;
int log_interval = 1000;

float Tc1t;
float Tc2t;
float Tc3t;

#define FILE_BASE_NAME "Data"

// long utm = 0;

void setup() {
  Serial.begin(9600);
  rtc.begin();
  rtc.adjust(DateTime((__DATE__), (__TIME__))); //Adjust automatically
  pinMode(SD_CS, OUTPUT);
  pinMode(H_EN, OUTPUT);
  Tc1.begin();
  Tc2.begin();
  Tc3.begin();
  delay(100);
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!sd.begin(SD_CS, SPI_HALF_SPEED)) {
    sd.initErrorHalt();
  }
  Serial.println("card initialized.");

  const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
  char fileName[13] = FILE_BASE_NAME "00.csv";
  // String fileName = String("stump.csv");
  // char __fileName[fileName.length() + 1];
  // fileName.toCharArray(__fileName, sizeof(__fileName));
  // Find an unused file name.
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
  do {
    delay(10);
  } while (Serial.read() >= 0);


  // if (!file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) {
  //   Serial.println("error");
  // }

  startTime = rtc.now();
  Serial.print("Current time is: "); Serial.println(startTime.unixtime());
  Serial.print("Writing to file: "); Serial.println(fileName);
  file.print("time,downstream, side, upstream, h_status");
}

void loop() {
  current_time = millis();
  now = rtc.now();
  long utm = now.unixtime();
  utm = millis()/1000;
  if (h_saved_time + h_interval < current_time) {
    //if heater is on, wait six seconds
    if (h_status == 0) { //if it's off, turn it on
      digitalWrite(H_EN, HIGH);
      h_interval = h_pulse_on;
      h_status = 1;
      Serial.print("Heater on for "); Serial.print(h_interval / 1000); Serial.println("s");
    }
    else if (h_status == 1) {
      //if heat is on....turn it off
      digitalWrite(H_EN, LOW);
      h_interval = h_pulse_off;
      h_status = 0;
      Serial.print("Heater off for "); Serial.print(h_interval / 1000); Serial.println("s");
    }
    h_saved_time = current_time;
  }

  if (log_saved_time + log_interval < current_time) {
    Tc1.read();
    Tc2.read();
    Tc3.read();
    Tc1t = Tc1.getExternal();
    Tc2t = Tc2.getExternal();
    Tc3t = Tc3.getExternal();
    writeToSerial(utm, Tc1t, Tc2t, Tc3t, h_status);
    writeToSd(utm, Tc1t, Tc2t, Tc3t, h_status);
      if (!file.sync() || file.getWriteError()) {
    Serial.println("write error");
  }
    log_saved_time = current_time;
  }
}

void writeToSerial(long t, float t1, float t2, float t3, bool h) {
  Serial.print(t);
  Serial.print(", Tc1 temp: ");
  Serial.print(t1);
  Serial.print(", Tc2 temp: ");
  Serial.print(t2);
  Serial.print(", Tc3 temp: ");
  Serial.print(t3);
  Serial.print(", H status: ");
  Serial.print(h);
  Serial.println();
}

void writeToSd(long t, float t1, float t2, float t3,  bool h) {
  //creates filename to store data based on the Node Address
  // String fileName = String("stump.csv");
  // char __fileName[fileName.length() + 1];
  // fileName.toCharArray(__fileName, sizeof(__fileName));
  // //Opens and checks file
  // if (!file.open(__fileName, O_RDWR | O_CREAT | O_AT_END)) {
  //   Serial.print("Error opening: ");
  //   Serial.println(fileName);
  //   Blink(LED, 100);
  // }
  //Print data to new line
  file.print(t); file.print(",");
  file.print(t1); file.print(",");
  file.print(t2); file.print(",");
  file.print(t3); file.print(",");
  file.print(h); file.println();
  // if (!file.sync() || file.getWriteError()) {
  //   Serial.println("write error");
  // }
}

void Blink(byte PIN, int DELAY_MS) {
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN, LOW);
  delay(DELAY_MS);
}

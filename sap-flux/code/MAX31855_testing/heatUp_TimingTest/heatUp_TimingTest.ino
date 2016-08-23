//starting at Data35.csv on 20160822

#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include "SdFat.h"

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define DO   3
#define CS   4
#define CLK  5
#define FILE_BASE_NAME "Data"
Adafruit_MAX31855 thermocouple(CLK, CS, DO);

const int HEAT_EN = 7;
int8_t HEAT_STATE = 1;

const uint8_t chipSelect = 9;

const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
char fileName[13] = FILE_BASE_NAME "00.csv";
// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))

// File system object.
SdFat sd;
// Log file.
SdFile file;
// Time in micros for next data record.
uint32_t currentTime;
uint32_t savedLogTime = 0;
uint16_t logInterval = 1000;
uint32_t savedHeatTime = 0;
uint16_t heatInterval = 60000;
double prev_temp = 0;
int temp_counter = 0;

void setup() {

  //turn on heater
  pinMode(HEAT_EN, OUTPUT);
  digitalWrite(HEAT_EN, HEAT_STATE);

  Serial.begin(9600);
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
    sd.initErrorHalt();
  }
  // Find an unused file name.
  if (BASE_NAME_SIZE > 6) {
    error("FILE_BASE_NAME too long");
  }
  while (sd.exists(fileName)) {
    if (fileName[BASE_NAME_SIZE + 1] != '9') {
      fileName[BASE_NAME_SIZE + 1]++;
    } else if (fileName[BASE_NAME_SIZE] != '9') {
      fileName[BASE_NAME_SIZE + 1] = '0';
      fileName[BASE_NAME_SIZE]++;
    } else {
      error("Can't create file name");
    }
  }
  if (!file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) {
    error("file.open");
  }
  do {
    delay(10);
  } while (Serial.read() >= 0);
  Serial.print(F("Logging to: "));
  Serial.println(fileName);
}

void loop() {
  currentTime = millis();

  if (savedLogTime + logInterval < currentTime) {
    double current_temp = thermocouple.readCelsius();
    if (!isnan(current_temp)) {
      Serial.println(current_temp);
      if (prev_temp == current_temp) {
        temp_counter++;
      } else temp_counter = 0;

      if (temp_counter > 120) {
        Serial.println("TEMP STABLE!");
        temp_counter = 0;
        //          turn off heat
        HEAT_STATE = 0;
        digitalWrite(HEAT_EN, HEAT_STATE);
        Serial.println("Waiting for cooling");
        file.close();

        //wait for how many minutes before restarting?
        for (int i = 0; i < 5; i++) {
          delay(60000);
        }
        //          turn on heat
        HEAT_STATE = 1;
        digitalWrite(HEAT_EN, HEAT_STATE);
        // Find an unused file name.
        if (BASE_NAME_SIZE > 6) {
          error("FILE_BASE_NAME too long");
        }
        while (sd.exists(fileName)) {
          if (fileName[BASE_NAME_SIZE + 1] != '9') {
            fileName[BASE_NAME_SIZE + 1]++;
          } else if (fileName[BASE_NAME_SIZE] != '9') {
            fileName[BASE_NAME_SIZE + 1] = '0';
            fileName[BASE_NAME_SIZE]++;
          } else {
            error("Can't create file name");
          }
        }
        if (!file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) {
          error("file.open");
        }
        Serial.print(F("Logging to: "));
        Serial.println(fileName);
      }
    } else Serial.println("Something wrong with thermocouple!");


    file.print(currentTime);
    file.print(",");
    file.print(current_temp);
    file.print(",");
    // Force data to SD and update the directory entry to avoid data loss.
    if (!file.sync() || file.getWriteError()) {
      error("write error");
    }
    savedLogTime = currentTime;
    prev_temp = current_temp;
  }
}

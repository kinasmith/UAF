#include "Arduino.h"
#include "Adafruit_MAX31856.h"

float getBatteryVoltage(uint8_t pin, uint8_t en);
bool getTime();
void tcFault(uint8_t fault, uint8_t tcNum);

const uint8_t LED = A0;
const uint8_t N_SEL_1 = A1;
const uint8_t N_SEL_2 = A2;
const uint8_t N_SEL_4 = A3;
const uint8_t BAT_V = A7;

const uint8_t BAT_V_EN = 3;
const uint8_t HEATER_EN = 4;
const uint8_t TC3_CS = 5;
const uint8_t TC2_CS = 6;
const uint8_t TC1_CS = 7;
const uint8_t FLASH_CS = 8;
const uint8_t LED_TX = 9;
const uint8_t RFM_CS = 10;

Adafruit_MAX31856 tc1 = Adafruit_MAX31856(TC1_CS);
Adafruit_MAX31856 tc2 = Adafruit_MAX31856(TC2_CS);
Adafruit_MAX31856 tc3 = Adafruit_MAX31856(TC3_CS);

void setup() {
  Serial.begin(9600);
  Serial.println("MAX31856 thermocouple test");
  tc1.begin();
  tc2.begin();
  tc3.begin();
  tc1.setThermocoupleType(MAX31856_TCTYPE_E);
  tc2.setThermocoupleType(MAX31856_TCTYPE_E);
  tc3.setThermocoupleType(MAX31856_TCTYPE_E);
}

void loop() {
  Serial.print("1: CJ: ");
  Serial.print(tc1.readCJTemperature());
  Serial.print(" TJ: ");
  Serial.println(tc1.readThermocoupleTemperature());
  Serial.print("2: CJ: ");
  Serial.print(tc2.readCJTemperature());
  Serial.print(" TJ: ");
  Serial.println(tc2.readThermocoupleTemperature());
  Serial.print("3: CJ: ");
  Serial.print(tc3.readCJTemperature());
  Serial.print(" TJ: ");
  Serial.println(tc3.readThermocoupleTemperature());

  tcFault(tc1.readFault(), 1);
  tcFault(tc2.readFault(), 2);
  tcFault(tc3.readFault(), 3);
  delay(1000);
}

void tcFault(uint8_t fault, uint8_t tcNum) {
  // Check and print any faults
  if (fault) {
    if (fault & MAX31856_FAULT_CJRANGE) Serial.print(tcNum); Serial.println(" Cold Junction Range Fault");
    if (fault & MAX31856_FAULT_TCRANGE) Serial.print(tcNum); Serial.println(" Thermocouple Range Fault");
    if (fault & MAX31856_FAULT_CJHIGH)  Serial.print(tcNum); Serial.println(" Cold Junction High Fault");
    if (fault & MAX31856_FAULT_CJLOW)   Serial.print(tcNum); Serial.println(" Cold Junction Low Fault");
    if (fault & MAX31856_FAULT_TCHIGH)  Serial.print(tcNum); Serial.println(" Thermocouple High Fault");
    if (fault & MAX31856_FAULT_TCLOW)   Serial.print(tcNum); Serial.println(" Thermocouple Low Fault");
    if (fault & MAX31856_FAULT_OVUV)    Serial.print(tcNum); Serial.println(" Over/Under Voltage Fault");
    if (fault & MAX31856_FAULT_OPEN)    Serial.print(tcNum); Serial.println(" Thermocouple Open Fault");
  }
}
/*
bool getTime() {
	LED_STATE = true;
	digitalWrite(LED, LED_STATE);
	//Get the current timestamp from the datalogger
	bool TIME_RECIEVED = false;
	if(!HANDSHAKE_SENT) { //Send request for time to the Datalogger
		DEBUG("time - ");
		if (radio.sendWithRetry(GATEWAYID, "t", 1)) {
			DEBUG("snd . . ");
			HANDSHAKE_SENT = true;
		}
		else {
			DEBUGln("failed . . . no ack");
			return false; //if there is no response, returns false and exits function
		}
	}
	while(!TIME_RECIEVED && HANDSHAKE_SENT) { //Wait for the time to be returned
		if (radio.receiveDone()) {
			if (radio.DATALEN == sizeof(theTimeStamp)) { //check to make sure it's the right size
				theTimeStamp = *(TimeStamp*)radio.DATA;  //save data
				DEBUG(" rcv - ");DEBUG('[');DEBUG(radio.SENDERID);DEBUG("] ");
				DEBUG(theTimeStamp.timestamp);
				DEBUG(" [RX_RSSI:");DEBUG(radio.RSSI);DEBUG("]");
				DEBUGln();
				TIME_RECIEVED = true;
				LED_STATE = false;
				digitalWrite(LED, LED_STATE);
			}
			if (radio.ACKRequested()) radio.sendACK();
		}
	}
	HANDSHAKE_SENT = false;
	return true;
}
*/

/**
 * --->THIS ISN'T WORKING RIGHT. CHECK THE MATH<---
 * [getBatteryVoltage description]
 * @param  pin [description]
 * @param  en  [description]
 * @return     [description]
 */
float getBatteryVoltage(uint8_t pin, uint8_t en) {
  uint16_t R22 = 32650.0;
  uint32_t R23 = 55300.0;
	float readings = 0.0;
  pinMode(en, OUTPUT);
	digitalWrite(en, HIGH);
	delay(10);
	for (byte i=0; i<3; i++)
		readings += analogRead(pin);
  digitalWrite(en, LOW);
  readings /= 3.3;
	float v = (3.3 * (readings/1023.0)) * (R23/R22); //Calculate battery voltage
  Serial.println(v);
  return v;
}

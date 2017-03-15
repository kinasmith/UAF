#include <Arduino.h>
#include <EEPROM.h>
#include "Sleepy.h" //https://github.com/kinasmith/Sleepy
#include <MCP342x.h>
#include <Wire.h>

#define LED 3
#define SERIAL_BAUD 9600

#define VREF A1
#define SENS_EN 4

//Thermistor Var's
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define BCOEFFICIENT 3950 //Check this!
#define SERIESRESISTOR 10000

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

ISR(WDT_vect){ Sleepy::watchdogEvent();} // set watchdog for Sleepy

float getTemperature();
int32_t getSensorValue();

/*==============|| EEPROM ||==============*/
uint16_t EEPROM_ADDR = 0; //Start of data storage
float vRef;
float temperature;

/*==============|| ADC ||==============*/
// 0x68 is the default address for all MCP342x devices
uint8_t address = 0x68;
MCP342x adc = MCP342x(address);
MCP342x::Config
config(MCP342x::channel1, MCP342x::oneShot, MCP342x::resolution16, MCP342x::gain1);
MCP342x::Config status;// Configuration/status read back from the ADC


void setup()
{
	#ifdef SERIAL_EN
		Serial.begin(SERIAL_BAUD);
	#endif
	pinMode(LED, OUTPUT);      // Set LED Mode
  digitalWrite(LED, HIGH);
  // --| Initialize Devices |--
	MCP342x::generalCallReset();
	delay(1); // MC342x needs 300us to settle
	// Check device present
	Wire.requestFrom(address, (uint8_t) 1);
	if (!Wire.available()) {
		// DEBUG("No device found at address ");
		// DEBUGln(address);
		while (1);
	} else {
		DEBUGln("-- ADC Initialized");
	}
	#ifdef SERIAL_EN
  int i = 0;
	while (i < EEPROM.length()) {
		vRef = EEPROM.get(i, vRef);
    i += sizeof(vRef);
    temperature = EEPROM.get(i, temperature);
    i += sizeof(temperature);

    DEBUG(vRef);DEBUG(",");DEBUGln(temperature);
	}
	#endif
}

void loop()
{
  pinMode(SENS_EN, OUTPUT);
  digitalWrite(SENS_EN, HIGH);
  Sleepy::loseSomeTime(100); //let capacitor charge
  digitalWrite(SENS_EN, LOW); //falling edge enables switch
  delay(1); // wait for reference to stablize
  temperature = getTemperature();
  DEBUGln(temperature);
  int val = analogRead(VREF);
  float vRef = val*3.3/1023.0;
  DEBUGln(vRef);
  if(EEPROM_ADDR < EEPROM.length() - (sizeof(vRef)+sizeof(temperature))) {
    DEBUG(".saving "); DEBUG(sizeof(vRef)+sizeof(temperature)); DEBUG(" bytes to address "); DEBUGln(EEPROM_ADDR);
    EEPROM.put(EEPROM_ADDR, vRef);
    EEPROM_ADDR += sizeof(vRef);
    EEPROM.put(EEPROM_ADDR, temperature);
    EEPROM_ADDR += sizeof(temperature);
  } else {
    DEBUGln(".eeprom FULL Sleepying Forever");
    Sleepy::powerDown();
  }
  DEBUGFlush();
  //wait two minutes
  for(int i = 0; i < 2; i++){
    Sleepy::loseSomeTime(60000);
  }
}


float getTemperature()
{
	int32_t ADC_reading = 0;
	// float Vcc = 3.3;
	// float Ve = 2.048;

  ADC_reading = getSensorValue();
  // DEBUG("ADC Reading: ") DEBUGln(ADC_reading);
	// float therm_res = (SERIESRESISTOR * Vcc * ADC_reading)/((16383.0*Ve)-(Vcc*ADC_reading));
	// float therm_res = (SERIESRESISTOR * Ve * ADC_reading)/((16383.0*Ve)-(Ve*ADC_reading));
  float therm_res = 10000.0/(32767.0/ADC_reading-1.0);
  // DEBUG("Thermistor Voltage = "); DEBUGln(ADC_reading*Ve/32767);
	// DEBUG("Thermistor Resistance = ") DEBUGln(therm_res);
	float steinhart;
	steinhart = therm_res / THERMISTORNOMINAL;     // (R/Ro)
	steinhart = log(steinhart);                  // ln(R/Ro)
	steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
	steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
	steinhart = 1.0 / steinhart;                 // Invert
	steinhart -= 273.15;                         // convert to C
	// DEBUG("Temperature "); DEBUG(steinhart); DEBUGln(" *C");

	return steinhart;
}

int32_t getSensorValue()
{
	int32_t value = 0;
	// uint8_t err;
  MCP342x::Config status;
  // // Initiate a conversion; convertAndRead() will wait until it can be read
  uint8_t err = adc.convertAndRead(MCP342x::channel1, MCP342x::oneShot,
				   MCP342x::resolution16, MCP342x::gain1,
				   1000000, value, status);
  if (err) {
    // DEBUG("Convert error: ");
    // DEUBGln(err);
  }
  else {
    // DEBUG("Value: ");
    // DEBUGln(value);
  }
	return value;
}

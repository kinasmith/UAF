/*
  SHT2x - A Humidity Library for Arduino.

  Supported Sensor modules:
    SHT21-Breakout Module - http://www.moderndevice.com/products/sht21-humidity-sensor
	SHT2x-Breakout Module - http://www.misenso.com/products/001

  Created by Christopher Ladden at Modern Device on December 2009.

  Modified by www.misenso.com on October 2011:
	- code optimisation
	- compatibility with Arduino 1.0

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef SHT2X_H
#define SHT2X_H

#include <inttypes.h>

typedef enum {
    eSHT2xAddress = 0x40,
    /*eSHT2xAddress = 0x80,*/
} HUM_SENSOR_T;

typedef enum {
    eTempHoldCmd		= 0xE3,
    eRHumidityHoldCmd	= 0xE5,
    eTempNoHoldCmd      = 0xF3,
    eRHumidityNoHoldCmd = 0xF5,
} HUM_MEASUREMENT_CMD_T;

class SHT2xClass
{
  private:
	uint16_t readSensor(uint8_t command);

  public:
    float GetHumidity(void);
    float GetTemperature(void);
};

extern SHT2xClass SHT2x;

#endif

/*
  SHT2x - A Humidity Library for Arduino.

  Supported Sensor modules:
    SHT21-Breakout Module - http://www.moderndevice.com/products/sht21-humidity-sensor
	SHT2x-Breakout Module - http://www.misenso.com/products/001

  Created by Christopher Ladden at Modern Device on December 2009.
  Modified by Paul Badger March 2010

  Modified by www.misenso.com on October 2011:
	- code optimisation
	- compatibility with Arduino 1.0

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <inttypes.h>
//#include <Wire.h>
//#include "Arduino.h"
//#include "SHT2x.h"



/******************************************************************************
 * Global Functions
 ******************************************************************************/

/**********************************************************
 * GetHumidity
 *  Gets the current humidity from the sensor.
 *
 * @return float - The relative humidity in %RH
 **********************************************************/
float SHT2xClass::GetHumidity(void)
{
	return (-6.0 + 125.0 / 65536.0 * (float)(readSensor(eRHumidityHoldCmd)));
}

/**********************************************************
 * GetTemperature
 *  Gets the current temperature from the sensor.
 *
 * @return float - The temperature in Deg C
 **********************************************************/
float SHT2xClass::GetTemperature(void)
{
	return (-46.85 + 175.72 / 65536.0 * (float)(readSensor(eTempHoldCmd)));
}


/******************************************************************************
 * Private Functions
 ******************************************************************************/

uint16_t SHT2xClass::readSensor(uint8_t command)
{
    uint16_t result;

    Wire.beginTransmission(eSHT2xAddress);	//begin
    Wire.write(command);					//send the pointer location
    delay(100);
    Wire.endTransmission();               	//end

    Wire.requestFrom(eSHT2xAddress, 3);
    while(Wire.available() < 3) {
      ; //wait
    }

    //Store the result
    result = ((Wire.read()) << 8);
    result += Wire.read();
	result &= ~0x0003;   // clear two low bits (status bits)
    return result;
}

SHT2xClass SHT2x;

/*char SHT21humidity[10], SHT21Temperature[10];*/
float SHThumid;
float SHTtemp;
FuelGauge fuel;
float bat_v;
float SoC;

long now

void setup() {
  Serial.begin(38400);
  Wire.begin();
  pinMode(D7, OUTPUT);
}

void loop() {
  now = Time.now();

  digitalWrite(D7, HIGH);
  delay(250);
  digitalWrite(D7, LOW);
  delay(750);
  for(int i = 0; i < 1; i++) {
    delay(60000);
  }
  SHTtemp = SHT2x.GetTemperature();
  SHThumid = SHT2x.GetHumidity();
  Serial.print(SHTtemp);
  Serial.print(", ");
  Serial.println(SHThumid);

  String temperature = String(SHTtemp);
  String humidity = String(SHThumid);
  String pub = temperature + ", " humidity;
  Particle.publish("temperature", temperature, PRIVATE); // publish to cloud
  /*Particle.publish("humidity", humidity, PRIVATE); // publish to cloud*/


}

//Webhook setup for Particle Console
/*
"event": "sensor_1",
"url": "https://api.thingspeak.com/update",
"requestType": "POST",
"form": {
		"api_key": "{{k}}",
		"field1": "{{1}}",
		"field2": "{{2}}",
		"field3": "{{3}}",
		"field4": "{{4}}",
		"field5": "{{5}}",
		"field6": "{{6}}",
		"field7": "{{7}}",
		"field8": "{{8}}",
		"lat": "{{a}}",
		"long": "{{o}}",
		"elevation": "{{e}}",
		"status": "{{s}}"
},
"mydevices": true,
"noDefaults": true
}
*/

#include "SHT2x.h"

SHT2x sht;
FuelGauge fuel;

int led = 7;

String Temp;
String Humidity;
String bat_v;
String SoC;
String Now;
String POST;
const String key = "S0REWU1JFZI8MGBO";

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(D7, OUTPUT);
}

void loop() {
  Now = String(Time.now());
  Temp = getTemp();
  Humidity = getHumidity();
	bat_v = String(fuel.getVCell());
	SoC = String(fuel.getSoC());

	String item_1 = "{ \"1\": \"" + Now + "\",";
	String item_2 = "\"2\": \"" + Temp + "\",";
	String item_3 = "\"3\": \"" + Humidity + "\",";
	String item_4 = "\"4\": \"" + bat_v + "\",";
	String item_5 = "\"5\": \"" + SoC + "\",";
	String item_k = "\"k\": \"" + key + "\" }";
	POST = item_1 + item_2 + item_3 + item_4 + item_5 + item_k;
	Serial.println(POST);
  if(Particle.publish("sensor_1", POST, 60, PRIVATE)) {
    Serial.println("Published!");
  } else Serial.println("Failed?");
  Serial.println("Now Sleeping!");
  System.sleep(SLEEP_MODE_DEEP,120);

}

String getTemp() {
  float t;
  for(int i = 0; i < 5; i++) {
    t += sht.GetTemperature();
    delay(100);
  }
  t /= 5;
  String a = String(t);
  return a;
}

String getHumidity() {
  float h;
  for(int i = 0; i < 5; i++) {
    h += sht.GetHumidity();
    delay(100);
  }
  h /= 5;
  String a = String(h);
  return a;
}

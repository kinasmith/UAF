#include <jeelib-sleepy.h>
#include  <Wire.h>
#include  <MCP342X.h>

MCP342X myADC;

int led = 3;
int sens_en = 4;
unsigned long av = 0;
int av_num = 10;

void setup() {
  pinMode(led, OUTPUT);
  pinMode(sens_en, OUTPUT);

  Wire.begin();
  TWBR = 12;
  Serial.begin(9600);
  while(!Serial){
  }
  Serial.println("Starting up");
  Serial.println("Testing device connection...");
  Serial.println(myADC.testConnection() ? "MCP342X connection successful" : "MCP342X connection failed");

  myADC.configure( MCP342X_MODE_CONTINUOUS |
    MCP342X_CHANNEL_1 |
    MCP342X_SIZE_16BIT |
    MCP342X_GAIN_1X
    );
  Serial.println(myADC.getConfigRegShdw(), HEX);
}

void loop() {
  digitalWrite(led, LOW); //led on signals start of read process
//  for(int i = 0; i < av_num; i ++) {
//    int av_tmp;
//    av_tmp = getSensorValue();
//    av = av + av_tmp;
//  }
//  av /= av_num;
  Serial.print("val: ");
  Serial.println(getSensorValue(), DEC); //print value
//  Serial.print("  av: ");
//  Serial.println(av, DEC);
  //Serial.println("---------------");
  delay(1000);
  digitalWrite(led, HIGH); //led off
  delay(1000);
//  av = 0;
}

int getSensorValue() { //takes 350ms
  int  result;
  digitalWrite(sens_en, HIGH); //write enable high for 10 ms
  delay(250); //pin needs to be on long enough to charge the capacitor
  digitalWrite(sens_en, LOW); //write enable low. Falling edge triggers FET
  delay(150); //wait for reading to stabalize
  myADC.startConversion(); 
  myADC.getResult(&result);
  return result;
}
















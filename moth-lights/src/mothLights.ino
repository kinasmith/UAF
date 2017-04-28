#include "LiquidCrystal.h"
#include <Wire.h>
#include "RTClib.h"
#include "Button.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include "fader.cpp"
#include "EEPROM.h"

#define lcd_RS 8
#define lcd_E 9
#define lcd_D4 10
#define lcd_D5 11
#define lcd_D6 12
#define lcd_D7 13
#define lcd_BCKLGHT_R 3
#define lcd_BCKLGHT_G 5
#define lcd_BCKLGHT_B 6
int lcd_brightness = 255;

#define RED A7
#define GREEN A6
#define BLUE A3
#define WHITE A2

#define neo_01 2
#define neo_02 7
#define neo_03 4
#define NUMPIXELS      60

#define button_01 A0
#define button_02 A1
#define PULLUP true        //To keep things simple, we use the Arduino's internal pullup resistor.
#define INVERT true        //Since the pullup resistor will keep the pin high unless the
#define DEBOUNCE_MS 20     //A debounce time of 20 milliseconds usually works well for tactile button switches.
#define LONG_PRESS 500    //We define a "long press" to be 1000 milliseconds.
enum {RUN, TO_COLOR_SETTINGS, COLOR_SETTINGS, TO_ONOFF_SETTINGS, ONOFF_SETTINGS, TO_TIME_SETTINGS, TIME_SETTINGS, TO_RUN};
uint8_t STATE;

LiquidCrystal lcd(lcd_RS, lcd_E, lcd_D4, lcd_D5, lcd_D6, lcd_D7);
RTC_PCF8523 rtc;
DateTime now;
DateTime set;
Button Bu_1(button_01, PULLUP, INVERT, DEBOUNCE_MS);
Button Bu_2(button_02, PULLUP, INVERT, DEBOUNCE_MS);

// Adafruit_NeoPixel p1 = Adafruit_NeoPixel(NUMPIXELS, neo_01, NEO_RGBW);
void fadeComplete();
NeoPatterns p1(NUMPIXELS, neo_01, NEO_RGBW);
NeoPatterns p2(NUMPIXELS, neo_02, NEO_RGBW);
NeoPatterns p3(NUMPIXELS, neo_03, NEO_RGBW);

int dayLength = 12;
int dawn_duskLength = 10;
DateTime sunriseStartTime;
DateTime sunsetFinishTime;
int sunriseHour = 6;
int sunriseMinute = 0;
uint32_t color1Start = p1.Color(0,0,0,0);
uint32_t color1End = p1.Color(0,255,0,0);
uint32_t color2Start = p1.Color(0,0,0,0);
uint32_t color2End = p1.Color(0,0,0,0);
uint32_t color3Start = p1.Color(0,0,0,0);
uint32_t color3End = p1.Color(0,0,0,0);

int cursorRow = 0;
int cursorCol = 0;

bool MORNING = 0;
bool DAY = 0;
bool EVENING = 0;
bool NIGHT = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  if (! rtc.begin()) Serial.println("Couldn't find RTC");
  if (! rtc.initialized()) Serial.println("RTC is NOT running!");
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(lcd_BCKLGHT_R, OUTPUT);
  pinMode(lcd_BCKLGHT_G, OUTPUT);
  pinMode(lcd_BCKLGHT_B, OUTPUT);
  lcd.begin(20, 4); // set up the LCD's number of columns and rows:
  setLCDBacklight(255, 0, 255);


  p1.begin();
  p1.show();
  now = rtc.now();
  // sunriseStartTime = DateTime(now.year(), now.month(), now.day(), sunriseHour, sunriseMinute, 0) + TimeSpan(1,0,0,0);
  sunriseStartTime = DateTime(now.year(), now.month(), now.day(), sunriseHour, sunriseMinute, 0);
  sunsetFinishTime = sunriseStartTime + TimeSpan(0, dayLength, dawn_duskLength*2, 0);
}

void loop() {
  Bu_1.read();
  Bu_2.read();
  switch(STATE) {
    case RUN:
      if(Bu_1.wasReleased())
      {
      } else if(Bu_1.pressedFor(LONG_PRESS))
      {
        STATE = TO_ONOFF_SETTINGS;
        lcd.clear();
        lcd.cursor();
        setLCDBacklight(255, 255, 0);
      } else if(Bu_2.pressedFor(LONG_PRESS))
      {
        STATE = TO_COLOR_SETTINGS;
        lcd.clear();
        lcd.noCursor();
        setLCDBacklight(0,255,0);
        lcd.setCursor(0, 2);
        lcd.println("SET COLOR");
      } else
      {
        now = rtc.now();
        lcd.setCursor(0, 0);
        lcd.print(now.month());
        lcd.print('/');
        lcd.print(now.day());
        lcd.print(" ");
        lcd.print(now.hour());
        lcd.print(':');
        lcd.print(now.minute());
        lcd.print(" ");
        lcd.setCursor(0, 1);
        lcd.print("FadeT ");
        lcd.print(dawn_duskLength);
        lcd.print(" DayL ");
        lcd.print(dayLength);
        lcd.setCursor(0, 2);
        lcd.print("OnT ");
        lcd.print(sunriseStartTime.month());
        lcd.print('/');
        lcd.print(sunriseStartTime.day());
        lcd.print(" ");
        lcd.print(sunriseStartTime.hour());
        lcd.print(":");
        lcd.print(sunriseStartTime.minute());
        lcd.setCursor(0, 3);
        lcd.print("OffT ");
        lcd.print(sunsetFinishTime.month());
        lcd.print('/');
        lcd.print(sunsetFinishTime.day());
        lcd.print(" ");
        lcd.print(sunsetFinishTime.hour());
        lcd.print(":");
        lcd.print(sunsetFinishTime.minute());
        runLights();
      }
      break;
    case TO_ONOFF_SETTINGS:
      if(Bu_1.wasReleased())
      {
        STATE = ONOFF_SETTINGS;
        cursorCol = 20-6;
        cursorRow = 3;
        lcd.setCursor(cursorCol, cursorRow);
        lcd.print("on/off");
        cursorCol = 0;
        cursorRow = 0;
        lcd.setCursor(cursorCol, cursorRow);
        lcd.print("dayLength(hr):");
        lcd.print(dayLength);
        lcd.setCursor(cursorCol, cursorRow+=1);
        lcd.print("rise/set(m):");
        lcd.print(dawn_duskLength);
        lcd.setCursor(cursorCol, cursorRow+=1);
        // sunriseStartTime = DateTime(now.year(), now.month(), now.day(), sunriseHour, sunriseMinute, 0) + TimeSpan(1,0,0,0);
        sunriseStartTime = DateTime(now.year(), now.month(), now.day(), sunriseHour, sunriseMinute, 0);
        sunsetFinishTime = sunriseStartTime + TimeSpan(0, dayLength, dawn_duskLength*2, 0);
        lcd.print("sunrise:");
        lcd.print(sunriseStartTime.hour()); lcd.print(":"); lcd.print(sunriseStartTime.minute());
        cursorCol = 0;
        cursorRow = 0;
        lcd.setCursor(cursorCol, cursorRow);
      }
      break;
    case ONOFF_SETTINGS:
      if(Bu_1.pressedFor(LONG_PRESS))
      {
        STATE = TO_TIME_SETTINGS;
        lcd.clear();
      } else if(Bu_1.wasReleased())
      {
        cursorRow++;
        cursorRow %= 3;
        lcd.setCursor(cursorCol, cursorRow);
      } else
      {
        if(Bu_2.wasReleased())
        {
          if(cursorRow == 0)
          {
            dayLength++;
            if(dayLength > 20) dayLength = 1; lcd.clear();
          } else if(cursorRow == 1)
          {
            dawn_duskLength += 15;
            if(dawn_duskLength > 120) dawn_duskLength = 1; lcd.clear();
          } else if(cursorRow == 2)
          {
            sunriseMinute += 10;
            if(sunriseMinute >= 60) {
              sunriseHour++;
              sunriseMinute = 0;
              if(sunriseHour > 10) {
                sunriseHour = 3;
              }
              lcd.clear();
            }
          }
          int p_cR = cursorRow;
          int p_cC = cursorCol;
          cursorCol = 20-6;
          cursorRow = 3;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("on/off");
          cursorCol = 0;
          cursorRow = 0;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("dayLength(hr):");
          lcd.print(dayLength);
          lcd.setCursor(cursorCol, cursorRow+=1);
          lcd.print("rise/set(m):");
          lcd.print(dawn_duskLength);
          lcd.setCursor(cursorCol, cursorRow+=1);
          // sunriseStartTime = DateTime(now.year(), now.month(), now.day(), sunriseHour, sunriseMinute, 0) + TimeSpan(1,0,0,0);
          sunriseStartTime = DateTime(now.year(), now.month(), now.day(), sunriseHour, sunriseMinute, 0);
          sunsetFinishTime = sunriseStartTime + TimeSpan(0, dayLength, dawn_duskLength*2, 0);
          lcd.print("sunrise:");
          lcd.print(sunriseStartTime.hour()); lcd.print(":"); lcd.print(sunriseStartTime.minute());
          cursorCol = p_cC;
          cursorRow = p_cR;
          lcd.setCursor(cursorCol, cursorRow);
        }
      }
      break;
    case TO_TIME_SETTINGS:
      if(Bu_1.wasReleased())
      {
        STATE = TIME_SETTINGS;
        set = now;
        cursorCol = 20-4;
        cursorRow = 2;
        lcd.setCursor(cursorCol, cursorRow);
        lcd.print("set");
        cursorRow = 3;
        lcd.setCursor(cursorCol, cursorRow);
        lcd.print("time");
        cursorCol = 0;
        cursorRow = 0;
        lcd.setCursor(cursorCol, cursorRow);
        lcd.print("month: ");
        lcd.print(set.month());
        lcd.setCursor(cursorCol, cursorRow+=1);
        lcd.print("day: ");
        lcd.print(set.day());
        lcd.setCursor(cursorCol, cursorRow+=1);
        lcd.print("hour: ");
        lcd.print(set.hour());
        lcd.setCursor(cursorCol, cursorRow+=1);
        lcd.print("minute: ");
        lcd.print(set.minute());
        cursorCol = 0;
        cursorRow = 0;
        lcd.setCursor(cursorCol, cursorRow);
      }
      break;
    case TIME_SETTINGS:
      if(Bu_1.pressedFor(LONG_PRESS))
      {
        STATE = TO_RUN;
        lcd.clear();
        lcd.noCursor();
        setLCDBacklight(255, 0, 255);

      } else if(Bu_1.wasReleased())
      {
        cursorRow++;
        cursorRow %= 4;
        lcd.setCursor(cursorCol, cursorRow);
      } else
      {
        if(Bu_2.wasReleased())
        {
          if(cursorRow == 0)
          {
            int temp_month = set.month();
            temp_month++;
            if(temp_month > 12) temp_month = 1; lcd.clear();
            set = DateTime(set.year(), temp_month, set.day(), set.hour(), set.minute(), 0);
          } else if(cursorRow == 1)
          {
            int temp_day = set.day();
            temp_day++;
            if(temp_day > 31) temp_day = 1; lcd.clear();
            set = DateTime(set.year(), set.month(), temp_day, set.hour(), set.minute(), 0);
          } else if(cursorRow == 2)
          {
            int temp_hour = set.hour();
            temp_hour++;
            if(temp_hour > 23) temp_hour = 0; lcd.clear();
            set = DateTime(set.year(), set.month(), set.day(), temp_hour, set.minute(), 0);
          } else if(cursorRow == 3)
          {
            int temp_minute = set.minute();
            temp_minute++;
            if(temp_minute >= 60) temp_minute = 1; lcd.clear();
            set = DateTime(set.year(), set.month(), set.day(), set.hour(), temp_minute, 0);
          }
          int p_cR = cursorRow;
          int p_cC = cursorCol;
          cursorCol = 20-4;
          cursorRow = 2;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("set");
          cursorRow = 3;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("time");
          cursorCol = 0;
          cursorRow = 0;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("month: ");
          lcd.print(set.month());
          lcd.setCursor(cursorCol, cursorRow+=1);
          lcd.print("day: ");
          lcd.print(set.day());
          lcd.setCursor(cursorCol, cursorRow+=1);
          lcd.print("hour: ");
          lcd.print(set.hour());
          lcd.setCursor(cursorCol, cursorRow+=1);
          lcd.print("minute: ");
          lcd.print(set.minute());
          cursorCol = p_cC;
          cursorRow = p_cR;
          lcd.setCursor(cursorCol, cursorRow);
        }
      }
      break;
    case TO_RUN:
      if(Bu_1.wasReleased()) {
        STATE = RUN;
        setLCDBacklight(0,255,255);
        if(set.unixtime() != now.unixtime()) {
          rtc.adjust(set);
          MORNING = 0;
          DAY = 0;
          EVENING = 0;
          NIGHT = 0;
        }
        p1.setPixelColor(5, 0,0,0,0);
        p1.show();
      } else if(Bu_2.wasReleased()) {
        STATE = RUN;
        setLCDBacklight(0,255,255);
      }
      break;

    case TO_COLOR_SETTINGS:
      if(Bu_2.wasReleased())
      {
        STATE = COLOR_SETTINGS;
        lcd.clear();
        cursorCol = 0;
        cursorRow = 0;
        lcd.setCursor(cursorCol, cursorRow);
        lcd.print("Start: Color 1");
        cursorCol = 0;
        cursorRow = 0;
        lcd.setCursor(cursorCol, cursorRow);
      }
      break;
    case COLOR_SETTINGS:
      if(Bu_2.pressedFor(LONG_PRESS))
      {
        STATE = TO_RUN;
        lcd.clear();
      } else if(Bu_2.wasReleased())
      {
        cursorRow++;
        cursorRow %= 6;
        if(cursorRow == 0)
        {
          int p_cR = cursorRow;
          int p_cC = cursorCol;
          for(int i = 0; i < NUMPIXELS; i++) p1.setPixelColor(i, color1Start);
          p1.show();
          lcd.clear();
          cursorCol = 0;
          cursorRow = 0;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("Start: Color 1");
          cursorCol = p_cC;
          cursorRow = p_cR;
        } else if(cursorRow == 1)
        {
          int p_cR = cursorRow;
          int p_cC = cursorCol;
          for(int i = 0; i < NUMPIXELS; i++) p1.setPixelColor(i, color1End);
          p1.show();
          lcd.clear();
          cursorCol = 0;
          cursorRow = 0;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("End: Color 1");
          cursorCol = p_cC;
          cursorRow = p_cR;
        } else if(cursorRow == 2)
        {
          int p_cR = cursorRow;
          int p_cC = cursorCol;
          for(int i = 0; i < NUMPIXELS; i++) p1.setPixelColor(i, color2Start);
          p1.show();
          lcd.clear();
          cursorCol = 0;
          cursorRow = 0;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("Start: Color 2");
          cursorCol = p_cC;
          cursorRow = p_cR;
        } else if(cursorRow == 3)
        {
          int p_cR = cursorRow;
          int p_cC = cursorCol;
          for(int i = 0; i < NUMPIXELS; i++) p1.setPixelColor(i, color2End);
          p1.show();
          lcd.clear();
          cursorCol = 0;
          cursorRow = 0;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("End: Color 2");
          cursorCol = p_cC;
          cursorRow = p_cR;
        } else if(cursorRow == 4)
        {
          int p_cR = cursorRow;
          int p_cC = cursorCol;
          for(int i = 0; i < NUMPIXELS; i++) p1.setPixelColor(i, color3Start);
          p1.show();
          lcd.clear();
          cursorCol = 0;
          cursorRow = 0;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("Start: Color 3");
          cursorCol = p_cC;
          cursorRow = p_cR;
        } else if(cursorRow == 5)
        {
          int p_cR = cursorRow;
          int p_cC = cursorCol;
          for(int i = 0; i < NUMPIXELS; i++) p1.setPixelColor(i, color3End);
          p1.show();
          lcd.clear();
          cursorCol = 0;
          cursorRow = 0;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("End: Color 3");
          cursorCol = p_cC;
          cursorRow = p_cR;
        }
      } else
      {
        if(cursorRow == 0) {
          for(int i = 0; i < 5; i++) p1.setPixelColor(i, getColor());
          p1.show();
          if(Bu_1.wasReleased()) {
            color1Start = getColor();
            for(int i = 0; i < NUMPIXELS; i++) {
              p1.setPixelColor(i, color1Start);
            }
            p1.show();
            lcd.setCursor(0, 2);
            lcd.print("-Stored");
          }
        }
        if(cursorRow == 1) {
          for(int i = 0; i < 5; i++) p1.setPixelColor(i, getColor());
          p1.show();
          if(Bu_1.wasReleased()){
            color1End = getColor();
            for(int i = 0; i < NUMPIXELS; i++) {
              p1.setPixelColor(i, color1End);
            }
            p1.show();
            lcd.setCursor(0, 2);
            lcd.print("-Stored");
          }
        }
        if(cursorRow == 2) {
          for(int i = 0; i < 5; i++) p1.setPixelColor(i, getColor());
          p1.show();
          if(Bu_1.wasReleased()){
            color2Start = getColor();
            for(int i = 0; i < NUMPIXELS; i++) {
              p1.setPixelColor(i, color2Start);
            }
            p1.show();
            lcd.setCursor(0, 2);
            lcd.print("-Stored");
          }
        }
        if(cursorRow == 3) {
          for(int i = 0; i < 5; i++) p1.setPixelColor(i, getColor());
          p1.show();
          if(Bu_1.wasReleased()){
            color2End = getColor();
            for(int i = 0; i < NUMPIXELS; i++) {
              p1.setPixelColor(i, color2End);
            }
            p1.show();
            lcd.setCursor(0, 2);
            lcd.print("-Stored");
          }
        }
        if(cursorRow == 4) {
          for(int i = 0; i < 5; i++) p1.setPixelColor(i, getColor());
          p1.show();
          if(Bu_1.wasReleased()){
            color3Start = getColor();
            for(int i = 0; i < NUMPIXELS; i++) {
              p1.setPixelColor(i, color3Start);
            }
            p1.show();
            lcd.setCursor(0, 2);
            lcd.print("-Stored");
          }
        }
        if(cursorRow == 5) {
          for(int i = 0; i < 5; i++) p1.setPixelColor(i, getColor());
          p1.show();
          if(Bu_1.wasReleased()){
            color3End = getColor();
            for(int i = 0; i < NUMPIXELS; i++) {
              p1.setPixelColor(i, color3End);
            }
            p1.show();
            lcd.setCursor(0, 2);
            lcd.print("-Stored");
          }
        }
      }
      break;
  }
}


void setLCDBacklight(uint8_t r, uint8_t g, uint8_t b) {
  // normalize the red LED - its brighter than the rest!
  r = map(r, 0, 255, 0, 100); //100
  g = map(g, 0, 255, 0, 100); //150

  r = map(r, 0, 255, 0, lcd_brightness);
  g = map(g, 0, 255, 0, lcd_brightness);
  b = map(b, 0, 255, 0, lcd_brightness);

  // common anode so invert!
  r = map(r, 0, 255, 255, 0);
  g = map(g, 0, 255, 255, 0);
  b = map(b, 0, 255, 255, 0);
  analogWrite(lcd_BCKLGHT_R, r);
  analogWrite(lcd_BCKLGHT_G, g);
  analogWrite(lcd_BCKLGHT_B, b);
}

uint32_t getColor() {
  int Rval,Gval,Bval,Wval;
  Rval = analogRead(RED);
  Gval = analogRead(GREEN);
  Bval = analogRead(BLUE);
  Wval = analogRead(WHITE);
  Rval /= 4;
  Gval /= 4;
  Bval /= 4;
  Wval /= 4;
  lcd.setCursor(0, 1);
  lcd.print(Rval);
  lcd.print(" ");
  lcd.print(Gval);
  lcd.print(" ");
  lcd.print(Bval);
  lcd.print(" ");
  lcd.print(Wval);
  lcd.print(" ");
  return p1.Color(Gval, Rval, Bval, Wval);
}

void runLights()
{
  DateTime sunriseFinishTime;
  DateTime sunsetStartTime;
  sunriseFinishTime = sunriseStartTime + TimeSpan(0, 0, dawn_duskLength, 0);
  sunsetStartTime = sunsetFinishTime - TimeSpan(0,0, dawn_duskLength,0);
  // Serial.print("rise "); Serial.println(sunriseStartTime.unixtime());
  // Serial.print("now  "); Serial.println(now.unixtime());
  // Serial.println();
  if(now.unixtime() > sunriseStartTime.unixtime() && now.unixtime() < sunriseFinishTime.unixtime())
  {
    if(!MORNING) {
      MORNING = 1;
      NIGHT = 0;
      p1.Fade(color1Start, color1End, dawn_duskLength, FORWARD);
      lcd.clear();
    }
    p1.Update();
    lcd.setCursor(20-6, 0);
    lcd.print("Morn");
  } else if(now.unixtime() > sunriseFinishTime.unixtime() && now.unixtime() < sunsetStartTime.unixtime())
  {
    if(!DAY) {
      DAY = 1;
      MORNING = 0;
      lcd.clear();
    }
    for(int i = 0; i < NUMPIXELS; i++) p1.setPixelColor(i, color1End);
    p1.show();
    lcd.setCursor(20-6, 0);
    lcd.print("Day");
  } else if(now.unixtime() > sunsetStartTime.unixtime() && now.unixtime() < sunsetFinishTime.unixtime())
  {
    if(!EVENING) {
      EVENING = 1;
      DAY = 0;
      p1.Fade(color1Start, color1End, dawn_duskLength, REVERSE);
      lcd.clear();
    }
    p1.Update();
    lcd.setCursor(20-6, 0);
    lcd.print("Eve");
  } else if(now.unixtime() > sunsetFinishTime.unixtime() && now.unixtime() < sunriseStartTime.unixtime())
  {
    if(!NIGHT) {
      NIGHT = 1;
      EVENING = 0;
      lcd.clear();
      //Set Next Alarm for Tomorrow!
      sunriseStartTime = DateTime(now.year(), now.month(), now.day(), sunriseHour, sunriseMinute, 0) + TimeSpan(1,0,0,0);
      sunsetFinishTime = sunriseStartTime + TimeSpan(0, dayLength, dawn_duskLength*2, 0);
    }
    for(int i = 0; i < NUMPIXELS; i++) p1.setPixelColor(i, color1Start);
    p1.show();
    lcd.setCursor(20-6, 0);
    lcd.print("Night");
  } else if(now.unixtime() < sunriseStartTime.unixtime()){
    if(!NIGHT) {
      NIGHT = 1;
      EVENING = 0;
      lcd.clear();
    }
    for(int i = 0; i < NUMPIXELS; i++) p1.setPixelColor(i, color1Start);
    p1.show();
    lcd.setCursor(20-6, 0);
    lcd.print("Night");
  }
}

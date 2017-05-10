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
#define LONG_LONG_PRESS 3000
enum {RUN, TO_COLOR_SETTINGS, COLOR_SETTINGS, TO_ONOFF_SETTINGS, ONOFF_SETTINGS, TO_TIME_SETTINGS, TIME_SETTINGS, TO_RUN};
uint8_t STATE;

LiquidCrystal lcd(lcd_RS, lcd_E, lcd_D4, lcd_D5, lcd_D6, lcd_D7);
RTC_PCF8523 rtc;
DateTime now;
DateTime set;
Button Bu_1(button_01, PULLUP, INVERT, DEBOUNCE_MS);
Button Bu_2(button_02, PULLUP, INVERT, DEBOUNCE_MS);

// Adafruit_NeoPixel left = Adafruit_NeoPixel(NUMPIXELS, neo_01, NEO_RGBW);
//
NeoPatterns left(NUMPIXELS, neo_01, NEO_GRBW);
NeoPatterns right(NUMPIXELS, neo_02, NEO_GRBW);
NeoPatterns top(NUMPIXELS, neo_03, NEO_GRBW);

uint32_t dayLength = 12;
uint32_t dayLength_seconds;
uint32_t dawn_duskLength = 10;
uint32_t dawn_dusk_seconds;

uint32_t color1Day = left.Color(0,0,0,255);
uint32_t color2Day = right.Color(0,0,0,255);
uint32_t color3Day = top.Color(0,0,0,255);
int sunriseHour = 6;
int sunriseMinute = 0;

DateTime sunriseStartTime;
DateTime sunsetFinishTime;

uint32_t colorStart = left.Color(0,0,20,0);
uint32_t colorEnd = left.Color(20,0,0,0);

int cursorRow = 0;
int cursorCol = 0;

bool MORNING = 0;
bool DAY = 0;
bool EVENING = 0;
bool NIGHT = 0;

struct Settings {
  uint32_t c1 = left.Color(0,0,0,255);;
  uint32_t c2 = right.Color(0,0,0,255);
  uint32_t c3 = top.Color(0,0,0,255);
  uint16_t dayL = 12;
  uint16_t fadeT = 10;
  uint16_t riseH = 6;
  uint16_t riseM = 0;
};
Settings theSettings;
uint32_t EEPROM_ADDR = 1;

void setup() {
  loadSavedSettings();
  Serial.begin(115200);
  Wire.begin();
  lcd.begin(20, 4); // set up the LCD's number of columns and rows:
  setLCDBacklight(50, 255, 50);
  pinMode(lcd_BCKLGHT_R, OUTPUT);
  pinMode(lcd_BCKLGHT_G, OUTPUT);
  pinMode(lcd_BCKLGHT_B, OUTPUT);
  if (! rtc.begin())
  {
    lcd.setCursor(0, 0);
    lcd.print("Couldn't find RTC");
    lcd.setCursor(0, 1);
    lcd.print("Please power cycle");
    while(1);
  }
  if (! rtc.initialized())
  {
    lcd.setCursor(0, 0);
    lcd.print("Failed to init RTC");
    lcd.setCursor(0, 1);
    lcd.print("Please power cycle");
    while(1);
  }
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  left.begin();
  right.begin();
  top.begin();
  left.show();
  right.show();
  top.show();
  now = rtc.now();
  sunriseStartTime = DateTime(now.year(), now.month(), now.day(), sunriseHour, sunriseMinute, 0);

//BUG:
//BUG: for dawn/dusk times greater than 60 minutes it rolls over
//BUG: Should be fixed converting everything to seconds using uint32's

  // sunsetFinishTime = sunriseStartTime + TimeSpan(0, dayLength, dawn_duskLength*2, 0);

  dayLength_seconds = (dayLength * 3600); //day Length is in hours. convert to seconds
  dawn_dusk_seconds = (dawn_duskLength * 60); //dawn/dusk fade time is in minutes, convert to seconds
  sunsetFinishTime = sunriseStartTime + TimeSpan(dayLength_seconds + (dawn_dusk_seconds*2)); //add the dawn/dusk time together for total delay
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
        setLCDBacklight(255, 10, 255);
      } else if(Bu_2.pressedFor(LONG_PRESS))
      {
        STATE = TO_COLOR_SETTINGS;
        lcd.clear();
        lcd.noCursor();
        setLCDBacklight(255,255,255);
        cursorCol = 5;
        cursorRow = 3;
        lcd.setCursor(cursorCol, cursorRow);
        lcd.print("color settings");
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
        lcd.print("fadeT ");
        lcd.print(dawn_duskLength);
        lcd.print("m");
        lcd.setCursor(12, 1);
        lcd.print("dayL ");
        lcd.print(dayLength);
        lcd.print("h");
        lcd.setCursor(0, 2);
        lcd.print("onTime ");
        lcd.print(sunriseStartTime.month());
        lcd.print('/');
        lcd.print(sunriseStartTime.day());
        lcd.print(" ");
        lcd.print(sunriseStartTime.hour());
        lcd.print(":");
        lcd.print(sunriseStartTime.minute());
        lcd.setCursor(0, 3);
        lcd.print("offTime ");
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
        cursorCol = 5;
        cursorRow = 3;
        lcd.setCursor(cursorCol, cursorRow);
        lcd.print("on/off settings");
        cursorCol = 0;
        cursorRow = 0;
        lcd.setCursor(cursorCol, cursorRow);
        lcd.print("dayLength(hr) ");
        lcd.print(dayLength);
        lcd.setCursor(cursorCol, cursorRow+=1);
        lcd.print("rise/set(min) ");
        lcd.print(dawn_duskLength);
        lcd.setCursor(cursorCol, cursorRow+=1);
        // sunriseStartTime = DateTime(now.year(), now.month(), now.day(), sunriseHour, sunriseMinute, 0) + TimeSpan(1,0,0,0);
        sunriseStartTime = DateTime(now.year(), now.month(), now.day(), sunriseHour, sunriseMinute, 0);

        dayLength_seconds = (dayLength * 3600); //day Length is in hours. convert to seconds
        dawn_dusk_seconds = (dawn_duskLength * 60); //dawn/dusk fade time is in minutes, convert to seconds
        sunsetFinishTime = sunriseStartTime + TimeSpan(dayLength_seconds + (dawn_dusk_seconds*2)); //add the dawn/dusk time together for total delay
        // sunsetFinishTime = sunriseStartTime + TimeSpan(0, dayLength, dawn_duskLength*2, 0);
        lcd.print("sunrise ");
        lcd.print(sunriseStartTime.hour()); lcd.print(":"); lcd.print(sunriseStartTime.minute());
        lcd.print("am");
        cursorCol = 0;
        cursorRow = 0;
        lcd.setCursor(cursorCol, cursorRow);
      }
      break;
    case ONOFF_SETTINGS:
      if(Bu_2.pressedFor(LONG_LONG_PRESS))
      {
        STATE = TO_TIME_SETTINGS;
        theSettings.dayL = dayLength;
        theSettings.fadeT = dawn_duskLength;
        theSettings.riseH = sunriseHour;
        theSettings.riseM = sunriseMinute;
        EEPROM.put(EEPROM_ADDR, theSettings);
        lcd.clear();
        lcd.setCursor(20-4, 2);
        lcd.print("set");
        lcd.setCursor(20-4, 3);
        lcd.print("time");
      } else if(Bu_1.pressedFor(LONG_PRESS))
      {
        STATE = TO_RUN;
        theSettings.dayL = dayLength;
        theSettings.fadeT = dawn_duskLength;
        theSettings.riseH = sunriseHour;
        theSettings.riseM = sunriseMinute;
        EEPROM.put(EEPROM_ADDR, theSettings);
        lcd.clear();
        lcd.noCursor();
        setLCDBacklight(255, 0, 255);
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
            if(dawn_duskLength > 241) dawn_duskLength = 1; lcd.clear();
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
          cursorCol = 5;
          cursorRow = 3;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("on/off settings");
          cursorCol = 0;
          cursorRow = 0;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("dayLength(hr) ");
          lcd.print(dayLength);
          lcd.setCursor(cursorCol, cursorRow+=1);
          lcd.print("rise/set(min) ");
          lcd.print(dawn_duskLength);
          lcd.setCursor(cursorCol, cursorRow+=1);
          // sunriseStartTime = DateTime(now.year(), now.month(), now.day(), sunriseHour, sunriseMinute, 0) + TimeSpan(1,0,0,0);
          sunriseStartTime = DateTime(now.year(), now.month(), now.day(), sunriseHour, sunriseMinute, 0);
          dayLength_seconds = (dayLength * 3600); //day Length is in hours. convert to seconds
          dawn_dusk_seconds = (dawn_duskLength * 60); //dawn/dusk fade time is in minutes, convert to seconds
          sunsetFinishTime = sunriseStartTime + TimeSpan(dayLength_seconds + (dawn_dusk_seconds*2)); //add the dawn/dusk time together for total delay
          // sunsetFinishTime = sunriseStartTime + TimeSpan(0, dayLength, dawn_duskLength*2, 0);
          lcd.print("sunrise ");
          lcd.print(sunriseStartTime.hour()); lcd.print(":"); lcd.print(sunriseStartTime.minute());
          lcd.print("am");
          cursorCol = p_cC;
          cursorRow = p_cR;
          lcd.setCursor(cursorCol, cursorRow);
        }
      }
      break;
    case TO_TIME_SETTINGS:
      if(Bu_2.wasReleased())
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
        if(set.unixtime() != now.unixtime()) {
          rtc.adjust(set);
          MORNING = 0;
          DAY = 0;
          EVENING = 0;
          NIGHT = 0;
        }
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
        setLCDBacklight(50, 255, 50);
        // left.setPixelColor(5, 0,0,0,0);
        // left.show();
      } else if(Bu_2.wasReleased()) {
        STATE = RUN;
        setLCDBacklight(50, 255, 50);
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
        lcd.print("Panel 1");
        cursorCol = 5;
        cursorRow = 3;
        lcd.setCursor(cursorCol, cursorRow);
        lcd.print("color settings");
        cursorCol = 0;
        cursorRow = 0;
        lcd.setCursor(cursorCol, cursorRow);
        for(int i = 0; i < NUMPIXELS; i++) {
          left.setPixelColor(i, 0);
          right.setPixelColor(i, 0);
          top.setPixelColor(i, 0);
        }
        left.show();
        right.show();
        top.show();
      }
      break;
    case COLOR_SETTINGS:
      if(Bu_1.pressedFor(LONG_PRESS))
      {
        STATE = TO_RUN;
        theSettings.c1 = color1Day;
        theSettings.c2 = color2Day;
        theSettings.c3 = color3Day;
        EEPROM.put(EEPROM_ADDR, theSettings);
        lcd.clear();
      } else if(Bu_1.wasReleased())
      {
        cursorRow++;
        cursorRow %= 3;
        for(int i = 0; i < NUMPIXELS; i++) {
          left.setPixelColor(i, 0);
          right.setPixelColor(i, 0);
          top.setPixelColor(i, 0);
        }
        left.show();
        right.show();
        top.show();
        if(cursorRow == 0)
        {
          int p_cR = cursorRow;
          int p_cC = cursorCol;
          for(int i = 0; i < NUMPIXELS; i++) left.setPixelColor(i, color1Day);
          left.show();
          lcd.clear();
          cursorCol = 0;
          cursorRow = 0;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("Panel 1");
          cursorCol = 5;
          cursorRow = 3;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("color settings");
          cursorCol = p_cC;
          cursorRow = p_cR;
        } else if(cursorRow == 1)
        {
          int p_cR = cursorRow;
          int p_cC = cursorCol;
          for(int i = 0; i < NUMPIXELS; i++) right.setPixelColor(i, color2Day);
          right.show();
          lcd.clear();
          cursorCol = 0;
          cursorRow = 0;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("Panel 2");
          cursorCol = 5;
          cursorRow = 3;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("color settings");
          cursorCol = p_cC;
          cursorRow = p_cR;
        } else if(cursorRow == 2)
        {
          int p_cR = cursorRow;
          int p_cC = cursorCol;
          for(int i = 0; i < NUMPIXELS; i++) top.setPixelColor(i, color3Day);
          top.show();
          lcd.clear();
          cursorCol = 0;
          cursorRow = 0;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("Panel 3");
          cursorCol = 5;
          cursorRow = 3;
          lcd.setCursor(cursorCol, cursorRow);
          lcd.print("color settings");
          cursorCol = p_cC;
          cursorRow = p_cR;
        }
      } else
      {
        if(cursorRow == 0) {
          for(int i = 0; i < NUMPIXELS; i++) left.setPixelColor(i, getColor());
          left.show();
          if(Bu_2.wasReleased()) {
            color1Day = getColor();
            for(int i = 0; i < NUMPIXELS; i++) {
              left.setPixelColor(i, color1Day);
            }
            left.show();
            lcd.setCursor(10, 0);
            lcd.print("-stored");
          }
        }
        if(cursorRow == 1) {
          for(int i = 0; i < NUMPIXELS; i++) right.setPixelColor(i, getColor());
          right.show();
          if(Bu_2.wasReleased()){
            color2Day = getColor();
            for(int i = 0; i < NUMPIXELS; i++) {
              right.setPixelColor(i, color2Day);
            }
            right.show();
            lcd.setCursor(10, 0);
            lcd.print("-stored");
          }
        }
        if(cursorRow == 2) {
          for(int i = 0; i < NUMPIXELS; i++) top.setPixelColor(i, getColor());
          top.show();
          if(Bu_2.wasReleased()){
            color3Day = getColor();
            for(int i = 0; i < NUMPIXELS; i++) {
              top.setPixelColor(i, color3Day);
            }
            top.show();
            lcd.setCursor(10, 0);
            lcd.print("-stored");
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
  return left.Color(Rval, Gval, Bval, Wval);
}

void runLights()
{
  DateTime sunriseFinishTime;
  DateTime sunsetStartTime;
  dawn_dusk_seconds = (dawn_duskLength * 60); //dawn/dusk fade time is in minutes, convert to seconds
  sunriseFinishTime = sunriseStartTime + TimeSpan(dawn_dusk_seconds);
  sunsetStartTime = sunsetFinishTime - TimeSpan(dawn_dusk_seconds);
  // sunriseFinishTime = sunriseStartTime + TimeSpan(0, 0, dawn_duskLength, 0);
  // sunsetStartTime = sunsetFinishTime - TimeSpan(0,0, dawn_duskLength,0);

  if(now.unixtime() > sunriseStartTime.unixtime() && now.unixtime() < sunriseFinishTime.unixtime())
  {
    if(!MORNING)
    {
      MORNING = 1;
      DAY = 0;
      EVENING = 0;
      NIGHT = 0;
      //Just fade in ONE SIDE PANEL
      // left.Fade(colorStart, color1Day, dawn_duskLength);
      right.Fade(colorStart, color2Day, dawn_duskLength);
      // top.Fade(colorStart, color3Day, dawn_duskLength);
      left.Fade(colorStart, color1Day, 0);
      top.Fade(colorStart, color3Day, 0);
      lcd.clear();
    }
    // left.Update();
    right.Update();
    left.show();
    top.show();
    // top.Update();
    lcd.setCursor(20-7, 0);
    lcd.print("||MORN|");
  }
  else if(now.unixtime() > sunriseFinishTime.unixtime() && now.unixtime() < sunsetStartTime.unixtime())
  {
    if(!DAY)
    {
      DAY = 1;
      MORNING = 0;
      EVENING = 0;
      NIGHT = 0;
      lcd.clear();
    }
    for(int i = 0; i < NUMPIXELS; i++)
    {
      left.setPixelColor(i, color1Day);
      right.setPixelColor(i, color2Day);
      top.setPixelColor(i, color3Day);
    }
    left.show();
    right.show();
    top.show();
    lcd.setCursor(20-7, 0);
    lcd.print("||DAY||");
  }
  else if(now.unixtime() > sunsetStartTime.unixtime() && now.unixtime() < sunsetFinishTime.unixtime())
  {
    if(!EVENING)
    {
      EVENING = 1;
      DAY = 0;
      MORNING = 0;
      NIGHT = 0;
      //NOTE: This is fading BACKWARDS from colorEND to ColorDay
      //FADING OUT ON THE OPPOSITE SIDE PANEL
      left.Fade(color1Day, colorEnd, dawn_duskLength);
      right.Fade(color2Day, colorEnd, 0);
      top.Fade(color3Day, colorEnd, 0);
      // right.Fade(color2Day, colorEnd, dawn_duskLength);
      // top.Fade(color3Day, colorEnd, dawn_duskLength);
      lcd.clear();
    }
    left.Update();
    right.show();
    top.show();
    // right.Update();
    // top.Update();
    lcd.setCursor(20-7, 0);
    lcd.print("||EVE||");
  }
  else if(now.unixtime() > sunsetFinishTime.unixtime() && now.unixtime() < sunriseStartTime.unixtime())
  {
    if(!NIGHT)
    {
      NIGHT = 1;
      DAY = 0;
      MORNING = 0;
      EVENING = 0;
      lcd.clear();
    }
    for(int i = 0; i < NUMPIXELS; i++)
    {
      left.setPixelColor(i, 0);
      right.setPixelColor(i, 0);
      top.setPixelColor(i, 0);
    }
    left.show();
    right.show();
    top.show();
    lcd.setCursor(20-7, 0);
    lcd.print("||NIGHT");
  }
  else if(now.unixtime() < sunriseStartTime.unixtime())
  {
    if(!NIGHT)
    {
      NIGHT = 1;
      DAY = 0;
      MORNING = 0;
      EVENING = 0;
      lcd.clear();
    }
    for(int i = 0; i < NUMPIXELS; i++)
    {
      left.setPixelColor(i, 0);
      right.setPixelColor(i, 0);
      top.setPixelColor(i, 0);
    }
    left.show();
    right.show();
    top.show();
    lcd.setCursor(20-7, 0);
    lcd.print("||NIGHT");
  }
  else if(now.unixtime() > sunsetFinishTime.unixtime())
  {
    sunriseStartTime = DateTime(now.year(), now.month(), now.day(), sunriseHour, sunriseMinute, 0) + TimeSpan(1,0,0,0); //increment by 1 day

    dayLength_seconds = (dayLength * 3600); //day Length is in hours. convert to seconds
    dawn_dusk_seconds = (dawn_duskLength * 60); //dawn/dusk fade time is in minutes, convert to seconds
    sunsetFinishTime = sunriseStartTime + TimeSpan(dayLength_seconds + (dawn_dusk_seconds*2)); //add the dawn/dusk time together for total delay
    // sunsetFinishTime = sunriseStartTime + TimeSpan(0, dayLength, dawn_duskLength*2, 0);
  }
}

void loadSavedSettings(){
  EEPROM.get(EEPROM_ADDR, theSettings);
  color1Day = theSettings.c1;
  color2Day = theSettings.c2;
  color3Day = theSettings.c3;
  dayLength = theSettings.dayL;
  dawn_duskLength = theSettings.fadeT;
  sunriseHour = theSettings.riseH;
  sunriseMinute = theSettings.riseM;
}

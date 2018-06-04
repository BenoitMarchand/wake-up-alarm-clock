#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>// for the touch screen
#include <Wire.h>// For I2C Communication with DS3231
#include "DS3231.h"// Lib for DS3231

DS3231 Clock; // Set clock
RTClib RTC;

// Benoit Commentaire : a enlever le commentaire de la ligne "#define USE_Elegoo_SHIELD_PINOUT" dans D:\programme_File\Arduino\libraries\Elegoo_TFTLCD\Elegoo_TFTLCD.h
// From Elegoo tftpint example
#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif



// Paint example specifically for the TFTLCD breakout board.
// If using the Arduino shield, use the tftpaint_shield.pde sketch instead!
// DOES NOT CURRENTLY WORK ON ARDUINO LEONARDO
//Technical support:goodtft@163.com



// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7

// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 22
//   D1 connects to digital pin 23
//   D2 connects to digital pin 24
//   D3 connects to digital pin 25
//   D4 connects to digital pin 26
//   D5 connects to digital pin 27
//   D6 connects to digital pin 28
//   D7 connects to digital pin 29

// For the Arduino Due, use digital pins 33 through 40
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 33
//   D1 connects to digital pin 34
//   D2 connects to digital pin 35
//   D3 connects to digital pin 36
//   D4 connects to digital pin 37
//   D5 connects to digital pin 38
//   D6 connects to digital pin 39
//   D7 connects to digital pin 40
/*
#define YP 9  // must be an analog pin, use "An" notation!
#define XM 8  // must be an analog pin, use "An" notation!
#define YM A2   // can be a digital pin
#define XP A3   // can be a digital pin
*/

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin
/*
#define TS_MINX 50
#define TS_MAXX 920

#define TS_MINY 100
#define TS_MAXY 940
*/
//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0x8888

/******************* UI details */
#define BUTTON_X 60
#define BUTTON_Y 20
#define BUTTON_W 115
#define BUTTON_H 30
#define BUTTON_SPACING_X 5
#define BUTTON_SPACING_Y 250
#define BUTTON_TEXTSIZE 2

// text box where numbers go
#define TEXT_X 10
#define TEXT_Y 10
#define TEXT_W 220
#define TEXT_H 50
#define TEXT_TSIZE 3
#define TEXT_TCOLOR WHITE

#define BOXSIZE 40
#define PENRADIUS 3

#define MINPRESSURE 10
#define MAXPRESSURE 1000

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Global Variable declatation
enum States {Initialization, InitIdle, Idle, InitAlarm, Alarm, InitDS3231Time, SetDS3231Time};
States ActualState = Initialization;
DateTime ActualTime;
DateTime PreviousTime;
Elegoo_GFX_Button buttons[4];
TSPoint p;
int LightValue;

void show_time_COM(DateTime now)
{
  //Get the value from DS3231 and print on the serial port the result
    Serial.print(now.year(), DEC);Serial.print('/');Serial.print(now.month(), DEC); Serial.print('/');Serial.print(now.day(), DEC);
    Serial.print(' ');Serial.print(now.hour(), DEC);Serial.print(':');Serial.print(now.minute(), DEC);Serial.print(':');Serial.print(now.second(), DEC);Serial.println();
    Serial.print(" since midnight 1/1/1970 = ");Serial.print(now.unixtime());Serial.print("s = ");Serial.print(now.unixtime() / 86400L);Serial.println("d");
}

void UpdateClock(DateTime InputTime,DateTime PreviousTime){
  //This function update the clock if min or hours changes, it uses Global variables
 
  if (PreviousTime.hour()!=InputTime.hour() || PreviousTime.minute()!=InputTime.minute())// Check if an update is required or not
  {//Draw hours needle 
    Serial.print("actual Time :");Serial.print(InputTime.hour());Serial.print(":");Serial.print(InputTime.minute());Serial.println();
    Serial.print("Previous Time :");Serial.print(PreviousTime.hour());Serial.print(":");Serial.print(PreviousTime.minute());Serial.println();
    tft.fillCircle(120, 160, 112, BLACK); //Replot the interpart of the clock to erease previous values
    double shift = -1.57;//Used to rotate screen -1.57= rotate from 1/4 of turn counterclock wise
    tft.drawLine((int)(120+15*cos(6.2831*InputTime.hour()/12+shift)),(int)(160+15*sin(6.2831*InputTime.hour()/12+shift)),(int)(120+55*cos(6.2831*InputTime.hour()/12+shift)), (int) (160+55*sin(6.2831*InputTime.hour()/12+shift)),WHITE);
    tft.drawLine((int)(120+15*cos(6.2831*InputTime.minute()/60+shift)),(int)(160+15*sin(6.2831*InputTime.minute()/60+shift)),(int)(120+95*cos(6.2831*InputTime.minute()/60+shift)), (int) (160+95*sin(6.2831*InputTime.minute()/60+shift)),WHITE);
  }
  
  PreviousTime=ActualTime;
}

void InitScreen(){

  // This is the initialisation of the Elegoo screen (from a template)
  Serial.println(F("Paint!"));
  tft.reset();
  uint16_t identifier = tft.readID();
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101)
  {     
      identifier=0x9341;
       Serial.println(F("Found 0x9341 LCD driver"));
  }else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier=0x9341;
  }
  tft.begin(identifier);
  tft.setRotation(2);
  tft.fillScreen(BLACK);
}

void PrintIdleGUI(){
  tft.fillCircle(120,160,115,WHITE);// Set the frist circle 
  
  /* create 15 buttons, in classic candybar phone style */
  char buttonlabels[4][10] = {"LightOff", "LightOn", "SetAlarm", "SetTime" };
  uint16_t buttoncolors[4] = {BLUE, BLUE, BLUE, BLUE};
  for (uint8_t row=0; row<2; row++) {
    for (uint8_t col=0; col<2; col++) {
      buttons[col + row*2].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                 BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, WHITE, buttoncolors[col+row*2], WHITE,
                  buttonlabels[col + row*2], BUTTON_TEXTSIZE); 
      buttons[col + row*2].drawButton();
    }
  }
}

void CheckButton (){
    //*
    Serial.print("X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    Serial.print("\tPressure = "); Serial.println(p.z);
    //*/
  for (uint8_t b=0; b<4; b++) {
    if (buttons[b].contains(p.x, p.y)) {
      Serial.print("Pressing: "); Serial.println(b);
      buttons[b].press(true);  // tell the button it is pressed
    } else {
      buttons[b].press(false);  // tell the button it is NOT pressed
    }
  }
}
void SetTime(int Year, int Month,int Date, int DayOftheWeek, int Hour, int Minute, int Second){
  Clock.setYear(Year);
  Clock.setMonth(Month);
  Clock.setDate(Date);
  Clock.setDoW(DayOftheWeek);
  Clock.setHour(Hour);
  Clock.setMinute(Minute);
  Clock.setSecond(Second);
}

void CheckScreenPressed(){
  digitalWrite(13, HIGH);
  p = ts.getPoint();
  digitalWrite(13, LOW);

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {

    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    CheckButton ();
  }
}


void setup(void) {
  

  int ledPin = 44;    // LED connected to digital pin 44
  Serial.begin(9600);
  
  InitScreen();

  Wire.begin();//Requires fo communicate in I2C With DS3231

}

  
void loop()
{
  switch (ActualState){
    case Initialization:{

      ActualState=InitIdle;// Go to InitIdle case
    }
    break;

    case InitIdle:{
      PrintIdleGUI();
      ActualState=Idle;// Idle     
    }
    break;
    
    case Idle:{
     
      ActualTime= RTC.now();//Get the date
      UpdateClock(ActualTime, PreviousTime);//Plot time
      PreviousTime=ActualTime;
      // if sharing pins, you'll need to fix the directions of the touchscreen pins
      //pinMode(XP, OUTPUT);
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      //pinMode(YM, OUTPUT);
      CheckScreenPressed();
    }
    break;

     case InitAlarm:{
     }
     break;
     
     case Alarm :{
     }
     break;
   
      case InitDS3231Time:{
      }
      break;
    
      case SetDS3231Time:{
      }
      break;
    
     default : 
     Serial.println("uncoded case reached");
  }
 
}
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!

 /* if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
   
  }//*/
//}


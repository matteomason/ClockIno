// Used libraries
#include <DS3232RTC.h>
#include <RTClib.h> //
#include <Time.h> 
#include <TimeLib.h>
#include <FastLED.h> 

RTC_DS3231 rtc;

//Defining pins on Arduino
#define NUM_LEDS 87 // Number of LEDs in the project
#define COLOR_ORDER BRG  // Define color order for your strip
#define DATA_PIN 6  // Data pin for led comunication
#define DST_PIN 2  // Define DST adjust button pin
#define MIN_PIN 4  // Define Minutes adjust button pin
#define HUR_PIN 5  // Define Hours adjust button pin
#define BRI_PIN 3  // Define Light sensor pin

// Constructing digits and symbols
CRGB leds[NUM_LEDS]; // Define LEDs strip
byte digits[10][21] =  {{0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},  // Digit 0
                       {0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},   // Digit 1
                       {1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0},   // Digit 2
                       {1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1},   // Digit 3
                       {1,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1},   // Digit 4
                       {1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1},   // Digit 5
                       {1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},   // Digit 6
                       {0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1},   // Digit 7
                       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},   // Digit 8
                       {1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1}};  // Digit 9
                       
bool DST = false; //DST state
int last_digit = 0;
bool lastBrightnessSensorTrigger;

int ledColor = 031135; // Color used (in hex)

void setup(){ 
  Serial.begin(9600); 
  LEDS.addLeds<WS2812B, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS); // Set LED strip type
  LEDS.setBrightness(75); // Set initial brightness
  pinMode(DST_PIN, INPUT_PULLUP); // Define DST adjust button pin
  pinMode(MIN_PIN, INPUT_PULLUP); // Define Minutes adjust button pin
  pinMode(HUR_PIN, INPUT_PULLUP); // Define Hours adjust button pi

  leds[42]=ledColor;
  leds[44]=ledColor;
   
} 

// Get time in a single number, if hours will be a single digit then time will be displayed 155 instead of 0155
int GetTime(){
  tmElements_t Now;
  RTC.read(Now);
  int hour=Now.Hour;
  int minutes=Now.Minute;
  int second =Now.Second;
  return (hour*100+minutes);
  };

// Check Light sensor and set brightness accordingly
void BrightnessCheck(){
  const byte sensorPin = BRI_PIN; // light sensor pin
  const byte brightnessLow = 52; // Low brightness value
  const byte brightnessHigh = 150; // High brightness value
  int sensorValue = digitalRead(sensorPin); // Read sensor
   Serial.print("SensLum is: ");Serial.println(sensorValue);
  if (sensorValue == LOW && lastBrightnessSensorTrigger != LOW) {
    LEDS.setBrightness(brightnessHigh);
    lastBrightnessSensorTrigger = LOW;
    }
  if (sensorValue == HIGH && lastBrightnessSensorTrigger != HIGH){
      LEDS.setBrightness(brightnessLow);
      lastBrightnessSensorTrigger = HIGH;
    }
  };
  
// Convert time to array needed for WS2812B
void TimeToArray(){
  int Now = GetTime();  // Get time
  
  int cursor = 87; // last led number
    leds[0]=0x000000; // blanking dash
    leds[1]=0x000000; // blanking dash
    leds[2]=0x000000; // blanking dash
    leds[3]=0x000000; // blanking dash
    
   Serial.print("Time is: ");Serial.println(Now);
  if (DST){   // if DST is true then add one hour
   Now+=100;
   Serial.print("DST is ON, time set to : ");Serial.println(Now);
  }; 
      
  for(int i=1;i<=4;i++){
    int digit = Now % 10; // get last digit in time
   // 4 digit control: 
    if (i==1){
      Serial.print("Digit 4 is : ");
      Serial.print(digit);
      Serial.print(" ");
      cursor = 66;
    for(int k=0; k<=20; k++){ 
         Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
         else if (digits[digit][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
       Serial.println();
       
      last_digit = digit;
      }
  // 3 digit control:
    else if (i==2){
       Serial.print("Digit 3 is : ");
       Serial.print(digit);
       Serial.print(" ");
      cursor = 45;
    for(int k=0; k<=20; k++){ 
         Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
         else if (digits[digit][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
       Serial.println();
      }
// 2 digit control: 
    else if (i==3){
       Serial.print("Digit 2 is : ");
       Serial.print(digit);
       Serial.print(" ");
      cursor = 21;
      for(int k=0; k<=20; k++){ 
         Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
         else if (digits[digit][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
       Serial.println();
      }
  // 1 digit control:
    else if (i==4){
       Serial.print("Digit 1 is : ");
       Serial.print(digit);
       Serial.print(" ");
      cursor = 0;
      for(int k=0; k<=20;k++){ 
         Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
         else if (digits[digit][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
       Serial.println();
      }
    Now /= 10;
  }; 
};
// End of time show


// Daylight saving time
void DSTcheck(){
   int buttonDST = digitalRead(2);
   Serial.print("DST is: ");Serial.println(DST);
   if (buttonDST == LOW){
    if (DST){
      DST=false;
      Serial.print("Switching DST to: ");Serial.println(DST);
      }
      else if (!DST){
        DST=true;
        Serial.print("Switching DST to: ");Serial.println(DST);
      };
   delay(500);   
   };
  }
// End of daylight saving time function

// Time adjust function
void TimeAdjust(){
  int buttonH = digitalRead(HUR_PIN);
  int buttonM = digitalRead(MIN_PIN);
   //Serial.print("buttonH ");Serial.println(buttonH);
   //Serial.print("buttonM ");Serial.println(buttonM);
  if (buttonH == LOW || buttonM == LOW){
    delay(500);
    tmElements_t Now;
    RTC.read(Now);
    int hour=Now.Hour;
    int minutes=Now.Minute;
    int second =Now.Second;
      if (buttonH == LOW){
        if (Now.Hour== 23){Now.Hour=0;}
          else {Now.Hour += 1;};
        }else {
          if (Now.Minute== 59){Now.Minute=0;}
          else {Now.Minute += 1;};
          };
    RTC.write(Now); 
    }
  }
// End of time adjust function

void loop()
{ 
  BrightnessCheck(); // Check brightness
  DSTcheck(); // Check DST
  TimeAdjust(); // Check to se if time is geting modified
  TimeToArray(); // Show time
  FastLED.show(); // Display leds array
  delay (50);
}

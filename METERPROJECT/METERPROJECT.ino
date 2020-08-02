#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "ACS712.h"
#include <EEPROM.h>


//PIN NUMBERS
#define BUZZER 2
#define UP 10
#define RIGHT 9
#define LEFT 8
#define DOWN 11
#define MIDDLE 4
#define RESET 12 
#define RELAY 3

//WORKINGS MODES
#define NORMAL 0
#define TIMELIM 1
#define POWERLIM 2


//Variables used in the logic
LiquidCrystal_I2C lcd(0x3F, 16, 2); 
bool CUT_OFF_POWER = false;
int modeSelectIndex = 0;
String MODES[4] = {"<    Normal    >","<  Time Limit  >","<  Power Limit >","< Update  Rate >"};
int modeEditIndex = 0;
int cursorIndex = 0;
String modeValue = "000000";
String rateValue = "000.00";
int ratecursorIndex = 0;
float RATE_VALUE = 0.00;

//Reset Function
void(* resetFunc) (void) = 0;



//Init function
void setup() {
  Serial.begin(9600);
  for(int i = 2;i<10;i++)
    pinMode(i,OUTPUT);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.home();
  digitalWrite(RELAY,LOW);
  modeScreen(MODES[modeSelectIndex]);
}

// Scan input lines for which pin is HIGH
int getKeyCode(){
  for(int i = 3; i<9 ;i++){
    int state = digitalRead(i);
    if(state){
      return i;
    }
  }
  return -1;
}

//Edit the Rate Used to convert from kW/h to Ghc and store in EEPROM on Nano
void editRate(){
  rateValue = "000.00";
  ratecursorIndex = 0;
  delay(200);
  EEPROM.get(0, RATE_VALUE);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("<    " + rateValue + "    >");
  lcd.cursor_on();
  lcd.blink_on();
  
  while(1) {
    // put your main code here, to run repeatedly:
    int keycode = getKeyCode();
    if(keycode == -1){
      continue;
    }
    switch(keycode){
      case UP:{
        char x = rateValue[5-ratecursorIndex];
        if(x < '9'){
          x+=1;
        }
        rateValue[5-ratecursorIndex] = x;
        modeEditValue(x,ratecursorIndex);
      }
      break;
      case DOWN:{
        char x = rateValue[5-ratecursorIndex];
        if(x > '0'){
          x-=1;
        }
        rateValue[5-ratecursorIndex] = x; 
        modeEditValue(x,ratecursorIndex);
      }
      break;
      case RIGHT:{
        if(ratecursorIndex > 0){
          ratecursorIndex-=1;
        }
        if(ratecursorIndex == 2)
          ratecursorIndex-=1;
        setValueCursor(ratecursorIndex);
        delay(300);
      }
      break;
      case LEFT:{
        if(ratecursorIndex < 5){
          ratecursorIndex+=1;
        }
        if(ratecursorIndex == 2)
          ratecursorIndex+=1;
        setValueCursor(ratecursorIndex);
        delay(300);
      }
      break;
      case MIDDLE: 
        RATE_VALUE = rateValue.toFloat();
        EEPROM.put(0,RATE_VALUE);
        lcd.cursor_off();
        lcd.blink_off();
        successPrompt();
        modeScreen(MODES[modeSelectIndex]);
        return;
      break;
      case RESET:
        modeScreen(MODES[modeSelectIndex]);
        lcd.cursor_off();
        lcd.blink_off();
        return;
      break;
    } 
    delay(200);
  }
}

// Edit mode Value e.g( Power Limit and Time Limit)
void editModeValue(){
  modeValue = "000000";
  cursorIndex = 0;
  while(1) {
    int keycode = getKeyCode();
    if(keycode == -1){
      continue;
    }
    switch(keycode){
      case UP:{
        char x = modeValue[5-cursorIndex];
        if(x < '9'){
          x+=1;
        }
        modeValue[5-cursorIndex] = x;
        modeEditValue(x,cursorIndex);
      }
      break;
      case DOWN:{
        char x = modeValue[5-cursorIndex];
        if(x > '0'){
          x-=1;
        }
        modeValue[5-cursorIndex] = x; 
        modeEditValue(x,cursorIndex);
      }
      break;
      case RIGHT:{
        if(cursorIndex > 0){
          cursorIndex-=1;
        }
        setValueCursor(cursorIndex);
        delay(300);
      }
      break;
      case LEFT:{
        if(cursorIndex < 5){
          cursorIndex+=1;
        }
        setValueCursor(cursorIndex);
        delay(300);
      }
      break;
      case MIDDLE: 
        offPrompt();
        while(1){
          int keycode = getKeyCode();
          switch(keycode){
          case LEFT:  
            CUT_OFF_POWER = false;
            delay(100);
            startMonitoring();
            return;
          break;
          case RIGHT:
            CUT_OFF_POWER = true;
            delay(100);
            startMonitoring();
            return;
          break;
          case RESET:
            modeScreen(MODES[modeSelectIndex]);
            delay(200);
            return;
          break;
          }
          
        }
      break;
      case RESET:
        modeScreen(MODES[modeSelectIndex]);
        return;
      break;
    }
    Serial.println(modeValue);
    Serial.println(String(modeValue));
    Serial.println(String(modeValue.toInt()));
    delay(200);
  }
}

// Setup mode edit Screen
void modeEdit(){
  delay(250);
  String modeText = "";
  switch(modeSelectIndex){
  case NORMAL:{
    startMonitoring();
    return;
  }
  break;
  case TIMELIM:
    modeText = "Time Limit/Mins ";
  break;
  case POWERLIM:
    modeText = "Power Limit/kWh";
  break;
  }
  String value = "000000";
  modeEditScreen(modeText,value);
  setValueCursor(0);
  editModeValue();
}

// SELECTION BETWEEN INDIVIDUAL WORKING MODES
void modeSelect(int keycode){ 
  switch(keycode){
  case LEFT:  
    modeSelectIndex = (modeSelectIndex + 4 - 1) % 4;
    modeScreen(MODES[modeSelectIndex]); 
  break;
  case RIGHT:
    modeSelectIndex = (modeSelectIndex + 4 + 1) % 4;
    modeScreen(MODES[modeSelectIndex]);
  break;
  case MIDDLE:
    if(modeSelectIndex == 3){ 
      editRate(); 
    }else{
      modeEdit(); 
    }
  break;
  }
  delay(200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int keycode = getKeyCode();
  if(keycode != -1){
    Serial.print("Pin (");
    Serial.print(keycode);
    Serial.print(") Activated\n");
    modeSelect(keycode);
  } 
}

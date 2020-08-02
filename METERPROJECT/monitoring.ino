#include "ACS712.h" 

char watt[12];
char curr[6]; 
float error=0.05f;
ACS712 sensor(A0, error);
unsigned long last_time =0;
unsigned long current_time =0;
unsigned long lastReadings =0;
float Wh =0 ;  
float kWh=0;

long int modeValueINT = 0; 
bool PAUSE = false;

unsigned long START_TIME; 
unsigned long LAST_TIME;

int INFO_COUNT = 7;
int info_index = 6;

String HEADERS[] = {
  "  CONSUMPTION   ",
  "  CONSUMPTION   ",
  "    CURRENT     ",
  "    VOLTAGE     ",
  "   FREQUENCY    ",
  "   POWER LEFT   ",
  "   TIME LEFT    "
};
String CONTENT[] = {
  "",
  "",
  "",
  "   230 volts    ",
  "     50 Hz      ",
  "",
  ""
};

// READ DATA FROM ACS712 SENSOR
void sense() {
  float V = 230;
  float I = sensor.getACcurrent();
  if(I<=0.2000)I = 0.0;
  Serial.print("Current = ");
  Serial.println(I);
  float P = V * I;
  last_time = current_time;
  current_time = millis();    
  Wh = Wh+  P *(( current_time -last_time) /3600000.0) ; 
  kWh = Wh/1000.00;
  dtostrf(kWh, 5, 2, watt);    
  dtostrf(I,4,2,curr);  
  delay(1000);
}


// PRINT OUTPUT INFORMATION READ FROM THE AC712  TO THE LCD
void show_info(String line_1,String line_2){
  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print(line_1);
  lcd.setCursor(0, 1);
  lcd.print(line_2); 
}

// CALCULATE REMAINING TIME TO LIMIT (WORKS IN TIME LIMIT MODE)
float remainTime(){ 
  unsigned long remainingTime = millis() - START_TIME;
  Serial.print("REM TIM = ");
  Serial.println(remainingTime);
  remainingTime /= 1000;
  remainingTime /= 60;
  Serial.print("MODE VAL = ");
  Serial.println(modeValueINT);
  Serial.print("REM SEC VAL = ");
  Serial.println(modeValueINT-remainingTime);
  
  return (modeValueINT-remainingTime); 
  
}

// ENTRY POINT FOR MONITORING
void startMonitoring(){
  EEPROM.get(0, RATE_VALUE);
  lcd.cursor_off();
  lcd.blink_off(); 
  lastReadings =0;
  current_time = millis();
  last_time = current_time;
  Wh =0 ; 
  watt[0] = '\0';
  PAUSE = false;
  modeValueINT = String(modeValue).toInt();
  START_TIME = millis();
  LAST_TIME = START_TIME;
  digitalWrite(RELAY,HIGH);
  String line_1 =   "Reading...";
  String line_2 =   "Please Wait...";
  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print(line_1);
  lcd.setCursor(0, 1);
  lcd.print(line_2); 
  while(1){
    int keycode = getKeyCode();
    if(keycode == RESET){
      modeScreen(MODES[modeSelectIndex]);
      digitalWrite(BUZZER,LOW);
      delay(200);
      return;
    }
    unsigned long NOW = millis();
    if(NOW - LAST_TIME >= 5000){
      LAST_TIME = NOW;
      info_index = (info_index + 7 + 1)%7;
      if(modeSelectIndex == 0){
        if(info_index == 5 || info_index == 6){
          info_index = 0;
        }
      }else if(modeSelectIndex == 1){
        if(info_index == 5)
          info_index++;
      }else{
        if(info_index == 6)
          info_index = 0;
      }

      
      String line1,line2; 
      switch(info_index){
        case 0:{
          line1 = "  CONSUMPTION   ";
          
          line2 = String(watt) + " kWh";
          if(line2 == " W")
            line2 = "0.0 W";
        }
        break;
        case 1:{
          line1 = "  CONSUMPTION   ";
          float val = kWh * RATE_VALUE;
          char Val[12];
          dtostrf(val, 5, 5, Val);
          line2 = "Ghc " + String(Val);
        }
        break;
        case 2:{
          line1 = "    CURRENT     ";
          line2 = String(curr) + " Amps";
        }
        break;
        case 3:{
          line1 = "    VOLTAGE     ";
          line2 = "   230 volts    ";
        }
        break;
        case 4:{
          line1 = "   FREQUENCY    ";
          line2 = "     50 Hz      ";
        }
        break;
        case 5:{
          line1 = "   POWER LEFT   ";
          char remainModeVal[12]; 
          dtostrf((modeValueINT-kWh), 8, 2, remainModeVal);  
          line2 = String(remainModeVal) + " kWh";
        }
        break;
        case 6:{
          line1 =   "   TIME LEFT    ";
          char remainModeVal[12];
          dtostrf(remainTime(), 8, 2, remainModeVal);  
          line2 = String(remainModeVal) + " mins";
        }
        break; 
      }
      show_info(line1,line2);
      if(millis()-lastReadings > 1000 && !PAUSE){
        lastReadings = millis();
        sense();
      }
      if(modeSelectIndex == 1){
        if(remainTime() <= 0){
          if(CUT_OFF_POWER)
            digitalWrite(RELAY,LOW);
          PAUSE = true;
          digitalWrite(BUZZER,HIGH);
        }
      }
      if(modeSelectIndex ==2){
        if((modeValueINT-Wh) <= 0){
          if(CUT_OFF_POWER)
            digitalWrite(RELAY,LOW);
          PAUSE = true;
          digitalWrite(BUZZER,HIGH);
        }
      }
    }
  }
}

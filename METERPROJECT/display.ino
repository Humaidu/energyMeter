// LCD HELPER FUNCTION (MODE SELECTION SCREEN)
void modeEditScreen(String mode, String limit){
  String line_1 = mode;
  String line_2 = "<    " + limit + "    >";
  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print(line_1);
  lcd.setCursor(0, 1);
  lcd.print(line_2);
  lcd.cursor_on();
  lcd.blink_on();
}
// LCD HELPER FUNCTION (MODE VALUE INDEXES)
void setValueCursor(int index){
  lcd.setCursor(10-index,1);
}

// LCD HELPER FUNCTION (MODE VALUE EDIT SCREEN)
void modeEditValue(char value,int index){
  lcd.setCursor(10-index, 1);
  lcd.print(value);
  lcd.setCursor(10-index, 1);
}

// LCD HELPER FUNCTION (PROMPT TO CUT OFF POWER AFTER LIMIT)
void offPrompt(){
    String line_1 = "Off after limit?";
  String line_2 =   "< No       Yes >";
  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print(line_1);
  lcd.setCursor(0, 1);
  lcd.print(line_2);
}

// LCD HELPER FUNCTION (SUCCESS ON RATE UPDATE PROMPT)
void successPrompt(){
  String line_1 = "     Done !     ";
  String line_2 =   "Please Wait";
  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print(line_1);
  lcd.setCursor(0, 1);
  lcd.print(line_2);
  delay(2000);
}

// LCD HELPER FUNCTION (MODE SELECTION SCREEN) 2
void modeScreen(String mode){
  String line_1 = "      MODE";
  if(modeSelectIndex == 3){
    line_1 = "kWh to Ghc Rate.";
  }
  
  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print(line_1);
  lcd.setCursor(0, 1);
  lcd.print(mode);
}

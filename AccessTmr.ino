/*************************************************************
*Function Name: handleAccessTimer
*Purpose of Function: A function that checks if 5 seconds have
*passed since access was granted, and locks door if it has
*************************************************************/
void handleAccessTimer() {
  if (accessActive && millis() - accessStartTime >= accessDuration) {
    digitalWrite(signalPin, LOW);
    accessActive = false;
  }
}
/*************************************************************
*Function Name: handleMessageTimer
*Purpose of Function: A function that checks if 2 seconds have
*passed since message was displayed, clear the screen and 
* display default entry message
*************************************************************/
void handleMessageTimer() {
  if (screenState == SHOW_RESULT &&
      millis() - messageStartTime >= messageDuration) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Code/Tag:");
    screenState = WAITING;
  }
}
/*************************************************************
*Function Name: denyAccess
*Input Parameters: Const char* source
*Purpose of Function: A function displays access denied
* and source of the message
*************************************************************/
void denyAccess(const char* source) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Denied");
  lcd.setCursor(0, 1);
  lcd.print(source);

  screenState = SHOW_RESULT;/*Screen is in access display mode*/
  messageStartTime = millis();/*Timer for duration of displayed message is started*/
}
void grantAccess(const char* source) {
  digitalWrite(signalPin, HIGH);
  accessActive = true;
  accessStartTime = millis();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Granted");
  lcd.setCursor(0, 1);
  lcd.print(source);

  screenState = SHOW_RESULT;
  messageStartTime = millis();
}
/*************************************************************
*Function Name: handleBlock
*Purpose of Function: Checks if user has invalid entry three
* times within 5 minutes, and if so blocks system
*************************************************************/
void handleBlock() {
  if (failCount == 0){
     return;
  }

  if (millis() - firstFailTime > blockWindow) {
    failCount = 0; // reset after 5 min window
  }

  if (failCount >= 3 && !systemBlocked) {
    Serial.println("SYSTEM BLOCKED");
    lcd.clear();
    lcd.print("BLOCKED");
    systemBlocked = true;
    if(!alertSent){
      Serial1.println("ALERT");
      alertSent = true;
    }
  }
}
/*************************************************************
*Function Name: registerFailure
*Purpose of Function: Tacks number of failed entries
*************************************************************/
void registerFailure() {
  unsigned long now = millis();

  if (failCount == 0) {
    firstFailTime = now;
  }
  failCount++;
}
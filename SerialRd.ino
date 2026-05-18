/*************************************************************
*Function Name: readSerial
*Purpose of Function: A function that reads serial instructions
*sent from ESP32
*************************************************************/

void readSerial() {
  if (Serial1.available()) {
   
    int len = Serial1.readBytesUntil('\n', input, sizeof(input) - 1);
    input[len] = '\0';

    Serial.print("RX: ");
    Serial.println(input);

    if (strncmp(input, "POWER:", 6) == 0) {
      ledPower = atoi(input + 6);
      if(ledPower==0){
        FastLED.clear();
        FastLED.show();
      }
    }
    
    else if (strncmp(input, "MODE:", 5) == 0) {
      ledEffect = atoi(input + 5);
    }

    else if (strncmp(input, "COLOR:", 6) == 0) {
      ledColor = atoi(input + 6);
    }
    
    else if (strncmp(input, "BRIGHT:", 7) == 0) {
      ledBrightness = constrain(atoi(input + 7), 0, 255);
      FastLED.setBrightness(ledBrightness);
    }
    else if (strncmp(input, "LOCK:", 5) == 0) {
      lockLogic = atoi(input + 5);
      if(lockLogic==0){
        digitalWrite(signalPin,HIGH);
        //Serial1.println("UNLOCKED");
       
        if(systemBlocked==true){
          systemBlocked = false;
          failCount = 0;
          alertSent = false;
          Serial.println("UNBLOCKED");
        }
      }
      else{
        digitalWrite(signalPin,LOW);
        //Serial1.println("LOCKED");
      }
    }
  }
}
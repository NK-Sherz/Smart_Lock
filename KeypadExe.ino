/*************************************************************
*Function Name: scanKeypad
*Purpose of Function: To take in user input to a keypad and 
*compare against the predefined password
*************************************************************/
void scanKeypad() {
  if (systemBlocked) {
    return;
  }
  if (screenState != WAITING) {
    return;
  }

  char key = keypad.getKey();/*Gets the value of the pressed key*/
  if (!key) return;/*If the key isn't pressed, exit the loop*/

  Serial.print("Key: ");
  Serial.println(key);
  inputData[inputIndex] = key;/*Building an entered code*/
  lcd.setCursor(inputIndex, 1);/*Displays on LCD - row 1, position input index*/
  lcd.print(key);
  inputIndex++;

  if (inputIndex == PASSWORD_LENGTH - 1) {
    inputData[inputIndex] = '\0';/*If array is full, terminate the array with
                                      a null character*/

    if (strcmp(inputData, masterCode) == 0) {/*Compares inout code to presaved password*/
      grantAccess("Keypad");
      inputIndex = 0;
      memset(inputData, 0, PASSWORD_LENGTH);/*Clears the array*/
    }
    else {
      denyAccess("Keypad");
      registerFailure();
      inputIndex = 0;
      memset(inputData, 0, PASSWORD_LENGTH);
    }
  }
}
/*********************************************************
*Function Name: scanRFID
Purpose of Function: To scan RFID tag using an RFID reader
and decide whether to grant or deny access based on the
                                             scanned ID
*********************************************************/
void scanRFID() {
  if (systemBlocked) return;/*If system is blocked, exit*/
  if (screenState != WAITING) return;/*If a message is being displayed
                                on the LCD display, exit the function*/

  if (!mfrc522.PICC_IsNewCardPresent()) return;/*Checks if new card is present*/
  if (!mfrc522.PICC_ReadCardSerial()) return;/*Checks ID of card*/

  tagID = "";/*A string to contain the tag's ID*/

/*A loop that converts byte of the card's UID into hex
              and concatesnates it into a single string*/
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    tagID += String(mfrc522.uid.uidByte[i], HEX);
  }

  tagID.toUpperCase();/*Converts string to uppercase*/
  mfrc522.PICC_HaltA();/*Stops communication with the card*/

  lcd.clear();
  lcd.setCursor(0, 0);

  Serial.print("Read: ");
  Serial.println(tagID);

  Serial.print("Stored: ");
  Serial.println(masterTag);

  if (tagID == masterTag) {/*Compares tag ID with predefined tag ID*/
    grantAccess("RFID");/*Access is granted*/
  } else {
    denyAccess("RFID");
    registerFailure();/*Failure is recorded*/
  }
}
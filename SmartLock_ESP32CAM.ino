/*******************************************************************************
* File Name: SmartLock_ESP32CAM.ino
*
* Description: This is the main file of the SmartLock_ESP32CAM project. It 
* intialises communication between Firebase and ESP32 CAM; Initialises 
* communication between ESP32 CAM and Telegram; Receives serial alerts from 
* Arduino Mega and sends Firebase commands to the Arduino Mega for execution.
*
* Programmer: Nicole Sheridan
*
* Date: 1/05/2026
*
* Version: 1.0
********************************************************************************/
#include <WiFi.h>/*A library that connects the ESP32 CAM to WiFi*/
#include <Firebase_ESP_Client.h>/*A library that connects ESP32 CAM to Firebase RTDB*/
#include "addons/TokenHelper.h"/*Header file that manages Firebase tokens*/
#include "addons/RTDBHelper.h"/*Header file including helpers for RTDB*/
#include <HTTPClient.h>/*Allows HTTP requests for Telegram*/

#define API_KEY ""/*Identifies Firebase Project*/
#define DATABASE_URL ""/*links program to RTDB*/

FirebaseData fbdo;/*A variable that stores data received from Firebase*/
FirebaseAuth auth;/*A variable that stores authentication information*/
FirebaseConfig config;/*A variable that stores configuration settings*/
bool signupOK = false;/*Tracks if the Firebase login succeeded*/
bool ledPwr = false;/*Holds logic state of led strip power value received from Firebase*/
int ledEfct = 0;/*Holds led effect mode received from Firebase*/
int ledBrght = 100;/*Holds brightness value received from Firebase*/
int ledClr = 0;/*Holds color value received from Firebase */
bool lckStatus = false;/*Holds logic of lock received from Firebase*/
bool lastLckStatus = false;/*Holds previous state of lock*/
bool lastPwr = false;
int lastEfct = 0;
int lastBrght = 100;
int lastClr = 0;
unsigned long lastAlertMsg = 0;/*Holds time last block alert message was received*/
unsigned long lastLockMsg = 0;
unsigned long lastUnlockMsg = 0;
int alrtCooldown = 3000;/*minimum time esp32 must wait before sending alert message*/
static unsigned long lastRd = 0;/*stores last time data stream was read*/
int readBfr = 200;
const char* ssid = "";/*WiFi credentials*/
const char* password = "";/*WiFi password*/
String msg = '0';/*A string that holds serial message received from Arduino*/
String botToken = "";/*authorization
                                                                  key to control a telegram bot*/
String chatID = "";/*ID of recipient of message*/

void handleSerial();/*A function that handles serial communication with Arduino*/
void sendMessage(String text);/*A function that sends an alert to Telegram*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);/*Connects ESP32 CAM to WiFi*/

  Serial.print("Conneceting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");/*Print . until ESP32 CAM is connected to WiFi*/
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());/*Prints IP address of WiFi*/
  Serial.println();

  config.api_key = API_KEY;/*Used to authenticate ESP32 when accessing Firebase*/
  config.database_url = DATABASE_URL;/*Web address that points to realtime database*/

  if (Firebase.signUp(&config, &auth, "", "")) {/*Anonymous sign in to Firebase*/
    Serial.print("signUp OK");
    signupOK = true;/*Signed in to Firebase*/
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;/*Tracks authentication token status*/
  Firebase.begin(&config, &auth);/*Begins communication with Firebase*/
  Firebase.reconnectWiFi(true);/*Autoreconnects when WiFi is lost*/

  if (!Firebase.RTDB.beginStream(&fbdo, "/devices")) {/*Listens for updates from devices stream*/
    Serial.printf("Stream 1 begin error, %s\n\n", fbdo.errorReason().c_str());
  }
}
void loop() {

  handleSerial();/*Checks serial messages from Arduino*/
  
  if (Firebase.ready() && signupOK) {/*Reads database with a 200ms buffer to prevent spamming*/

    if (millis() - lastRd > readBfr) {  
      lastRd = millis();

      if (!Firebase.RTDB.readStream(&fbdo)) {
        Serial.println(fbdo.errorReason());
        return;
      }
    }
    if (fbdo.streamAvailable()) {/*When new data arrives, json object is extracted from json tree*/

      String path = fbdo.dataPath();
      Serial.println("FULL PATH: " + fbdo.dataPath());
      if (fbdo.dataPath() == "/led") {
        FirebaseJson json = fbdo.jsonObject();

        FirebaseJsonData result;

        json.get(result, "power");/*Extracted json objects converted to bool, int, etc.*/
        ledPwr = result.boolValue;

        json.get(result, "mode");
        ledEfct = result.intValue;

        json.get(result, "brightness");
        ledBrght = result.intValue;

        json.get(result, "color");
        ledClr = result.intValue;
      }

      if (ledPwr != lastPwr) {/*If previous values are different, new value sent to Arduino*/
        Serial.print("POWER:");
        Serial.println(ledPwr ? 1 : 0);
        lastPwr = ledPwr;
      }

      if (ledEfct != lastEfct) {
        Serial.print("MODE:");
        Serial.println(ledEfct);
        lastEfct = ledEfct;
      }

      if (ledBrght != lastBrght) {
        Serial.print("BRIGHT:");
        Serial.println(ledBrght);
        lastBrght = ledBrght;
      }

      if (ledClr != lastClr) {
        Serial.print("COLOR:");
        Serial.println(ledClr);
        lastClr = ledClr;
      }
      if (path == "/doorLock/locked") {
        lckStatus = fbdo.boolData();

        if (lckStatus != lastLckStatus) {
          Serial.print("LOCK:");
          Serial.println(lckStatus ? 1 : 0);
          lastLckStatus = lckStatus;
        }
      }
    }
  }
}
/*************************************************************************
*Function Name: handleSerial
*Purpose of function: To read serial messages received from Arduino
*************************************************************************/
void handleSerial() {
  if (Serial.available()) {
    msg = Serial.readStringUntil('\n');
    msg.trim();/*Trims extra white space from message*/

    if (msg == "ALERT") {
      if (millis() - lastAlertMsg > alrtCooldown) {/*Waits for 3 seconds after previous message sent before
                                                    sending a new one - prevents spamming*/
        sendMessage("Failed attempted entry to premises!");
        lastAlertMsg = millis();
      }
    }
    else if (msg == "UNLOCKED"){
      if (millis() - lastUnlockMsg > alrtCooldown) {
        sendMessage("The door is unlocked");
        lastUnlockMsg = millis();
      }
    }
     else if (msg == "LOCKED"){
      if (millis() - lastLockMsg > alrtCooldown) {
        sendMessage("The door is locked");
        lastLockMsg = millis();
      }
    }
  }
}
/************************************************************************
*Function Name: sendMessage
*Input Parameters: String text is the message received from handleSerial
*that will be sent to the Telegram bot on homeowner's phone
*Purpose of Function: To send user alert to Telegram app when an 
alert message is received from Arduino
************************************************************************/
void sendMessage(String text) {

  HTTPClient http;/*creates HTTP request object*/

  String url = "https://api.telegram.org/bot" + botToken + "/sendMessage?chat_id=" + chatID + "&text=" + text;
  /*Builds Telegram API URL*/

  http.begin(url);/*Initialises sending HTTP request*/

  int httpCode = http.GET();/*Sends request to server, receives response code
                              ie. 200 for success, 404 for error*/

  http.end();/*Closes connection between ESP32 CAM and Telgram server*/
}

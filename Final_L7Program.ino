/*******************************************************************************
* File Name: Final_L7Program.ino
*
* Description: This is the main file of the Arduino Mega portion of 
*the project. It initilaises all variables, functions and libraries used in the
* project, and polls inputs from several devices using a robin round algorithm
*
* Programmer: Nicole Sheridan
*
* Date: 1/05/2026
*
* Version: 1.0
********************************************************************************/

#include <LiquidCrystal.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>
#include <FastLED.h>

#define PASSWORD_LENGTH 5
#define RST_PIN 49
#define SS_PIN 53
#define NUM_LEDS 20
#define LED_PIN 40

const int signalPin = 47;
int failCount = 0;
unsigned long firstFailTime = 0;
const unsigned long blockWindow = 300000;

const int rs = 35, en = 33, d4 = 31, d5 = 29, d6 = 27, d7 = 25;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

CRGB leds[NUM_LEDS];

char input[32];

MFRC522 mfrc522(SS_PIN, RST_PIN);

char inputData[PASSWORD_LENGTH];
char masterCode[PASSWORD_LENGTH] = "7*#9";
byte inputIndex = 0;

String masterTag = "EDB6465";
String tagID = "";

bool alertSent = false;
bool accessActive = false;
unsigned long accessStartTime = 0;
const unsigned long accessDuration = 5000;
unsigned long currentMillis = 0; 

int ledPower = 0;
int ledEffect = 0;
int ledBrightness = 100;
int ledColor = 0;
int lockLogic = 0;

uint8_t startHue = 0;
int currentLED = 0;

enum ScreenState {
  WAITING,
  SHOW_RESULT
};

ScreenState screenState = WAITING;
unsigned long messageStartTime = 0;
const unsigned long messageDuration = 2000;

bool systemBlocked = false;

void rainbowSeq();
void chaserSeq();
void blinkSeq();

unsigned long previousMillis = 0;
const unsigned long ledInterval = 200;

bool blinkState = false;

void setup() {
  Serial1.begin(115200);
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Code/Tag:");

  pinMode(signalPin, OUTPUT);
 
  digitalWrite(signalPin, LOW);

  SPI.begin();
  mfrc522.PCD_Init();
  delay(3000); 

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
}
void loop() {
  handleBlock();/*Round Robin algorithm*/
  readSerial();
  scanKeypad();
  scanRFID();
  handleAccessTimer();
  handleMessageTimer();
  runLED();
}
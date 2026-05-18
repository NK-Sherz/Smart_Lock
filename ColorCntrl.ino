/*******************************************************************************
* File Name: ColorCntrl.ino
*
* Description: This file outputs block LEDcolours received from the ESP32 onto
* an LED strip in the case of no LED ligth sequence being selected in the mobile
* app
* Programmer: Nicole Sheridan
*
* Date: 1/05/2026
*
* Version: 1.0
********************************************************************************/
/***********************************************************************
*Function Name: runColor
* Purpose of Function: To display the color selected by the user in the
*                       mobile app on the LED strip
***********************************************************************/
void runColor(){
  if (!ledPower) {
    return;
  }
  switch(ledColor){
    case 0:
      FastLED.clear();
      FastLED.show();
      break;
    case 1:
      fill_solid(leds, NUM_LEDS, CRGB::Gray);
      FastLED.show();
      break;
    case 2:
      fill_solid(leds, NUM_LEDS, CRGB::HotPink);
      FastLED.show();
      break;
    case 3:
      fill_solid(leds, NUM_LEDS, CRGB::Cyan);
      FastLED.show();
      break;
    case 4:
      fill_solid(leds, NUM_LEDS, CRGB::Yellow);
      FastLED.show();
      break;
    case 5:
      fill_solid(leds, NUM_LEDS, CRGB::Blue);
      FastLED.show();
      break;
    case 6: 
      fill_solid(leds, NUM_LEDS, CRGB::Green);
      FastLED.show();
      break;
    case 7: 
      fill_solid(leds, NUM_LEDS, CRGB::Red);
      FastLED.show();
      break;
  }
}
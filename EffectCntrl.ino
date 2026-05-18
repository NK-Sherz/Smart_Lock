/**************************************************************
*Function Name: runLED
*Purpose of Function: Displays the LED effect selected by the 
*                     user in the mobile app on the LED strip
**************************************************************/
void runLED() {
  if (!ledPower) {
    return;
  }
  currentMillis = millis();
  if(currentMillis-previousMillis<=ledInterval){
    return;
  }
  previousMillis = currentMillis;

   if (ledEffect == 3) {   
    runColor();
    return;
  }
  switch (ledEffect) {
    case 0:
      rainbowSeq();
      break;

    case 1:
      chaserSeq();
      break;

    case 2:
      blinkSeq();
      break;
  }
}
/*************************************************************
*Function Name: rainbowSeq
*Purpose of Function: To display rainbow effect on LED strip
*************************************************************/
void rainbowSeq(){
  fill_rainbow(leds, NUM_LEDS, startHue, 5);
  FastLED.show();
  startHue++;
}
/*************************************************************
*Function Name: chaserSeq
*Purpose of Function: To display chaser effect on LED strip
*************************************************************/
void chaserSeq(){
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  leds[currentLED] = CRGB::Red;
  FastLED.show();
  currentLED++;
  if (currentLED >= NUM_LEDS) {
    currentLED = 0;
  }
}
/*************************************************************
*Function Name: blinkSeq
*Purpose of Function: To display blinking effect on LED strip
*************************************************************/
void blinkSeq() {
  blinkState = !blinkState; 
  if (blinkState) {
    fill_solid(leds, NUM_LEDS, CRGB::Blue);
  } else {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }
  FastLED.show();
}
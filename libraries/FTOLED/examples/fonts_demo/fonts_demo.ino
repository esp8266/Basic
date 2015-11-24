#include <SPI.h>
#include <SD.h>
#include <FTOLED.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Arial14.h>
#include <fonts/Arial_Black_16.h>
#include <fonts/Droid_Sans_36.h>

// NB: This sketch won't fit on older Arduino Duemilanoves or any other
// Arduino with less than 32k of onboard flash storage.
//
// If the sketch doesn't fit, disable one of the fonts by deleting one of
// the blocks below

const byte pin_cs = 7;
const byte pin_dc = 2;
const byte pin_reset = 3;

OLED oled(pin_cs, pin_dc, pin_reset);

void setup() {
  Serial.begin(115200);
  Serial.print("init...");
  oled.begin();

  oled.selectFont(Arial14);
  oled.drawString(6,20,F("Arial 14"),VIOLET,BLACK);

  oled.selectFont(Arial_Black_16);
  oled.drawString(6,38,F("Arial Black 16"),WHITE,BLACK);

  oled.selectFont(SystemFont5x7);
  oled.drawString(6,70,F("System 5x7\nOn Two Lines"),RED,BLACK);

  oled.selectFont(Droid_Sans_36);
  oled.drawString(6,78,F("DS 36"),GREEN,BLACK);

  // Finally, use the String object to build a dynamic string
  // instead of the previous strings which were all constant
  // "character arrays" (always the same value)
  String msg = "Sketch ran in\n";
  msg += millis();
  msg += " ms";
  oled.selectFont(SystemFont5x7);
  oled.drawString(6,8,msg,WHITE,BLACK);
}

void loop() {
     // Do nothing, we're finished!
}

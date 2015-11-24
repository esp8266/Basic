/*
 * Display the Freetronics logo and full colour cube, same as the packaging sticker.
 *
 * This sketch works by loading the file "Label.bmp" from the SD card, so you'll need
 * to copy that file to the SD card before running it.
 *
 */
#include <SPI.h>
#include <SD.h>
#include <FTOLED.h>
#include <fonts/SystemFont5x7.h>

const byte pin_cs = 7;
const byte pin_dc = 2;
const byte pin_reset = 3;

const byte pin_sd_cs = 4;

OLED oled(pin_cs, pin_dc, pin_reset);

#define MSG_NOSD F("MicroSD card not found")
#define MSG_FILENOTFOUND F("Label.bmp not found")
#define MSG_BMPFAIL F("Failed to load BMP: ")

// Text box is used to display error message if something
// fails to load
OLED_TextBox text(oled);

void setup()
{
  Serial.begin(115200);
  oled.begin();
  oled.selectFont(SystemFont5x7);

  while(!SD.begin(pin_sd_cs)) {
    Serial.println(MSG_NOSD);
    text.println(MSG_NOSD);
    delay(500);
  }
  oled.begin(); // Calling oled.begin() after SD.begin() so we get faster SPI, see https://github.com/freetronics/FTOLED/wiki/Displaying-BMPs

  File image = SD.open("Label.bmp");
  if(!image) {
    text.println(MSG_FILENOTFOUND);
    Serial.println(MSG_FILENOTFOUND);
  } else {
    BMP_Status result = oled.displayBMP(image, 0, 0);
    if(result != BMP_OK) {
      Serial.print(MSG_BMPFAIL);
      Serial.println((int) result);
      text.print(MSG_BMPFAIL);
      text.println((int) result);
    }
  }
}

void loop()
{
}

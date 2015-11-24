/*
 * Render a sprite animation of a little flying robot
 *
 * The three sprite frames in frames are made
 * from Stephen Challener's "Scifi Creature Tileset", licensed CC-BY
 * http://opengameart.org/content/scifi-creature-tileset-mini-32x32-scifi-creature-icons
 *
 *  Thanks Stephen!
 *
 * Process of creating frames.h:
 * 1) Use ImageMagick to split Stephen's sprite sheet into individual 32x32 tiles:
 *      convert "creatures edit_0.png" -crop 32x32 test.png
 * 2) Find the frames you want to use in your sprite, and convert them to BMP2 format with
 *    ImageMagick (see https://github.com/freetronics/FTOLED/wiki/Displaying-BMPs)
 * 3) Convert each BMP to a header file using bin2header.py (included with this library.)
 * 4) Combine all the individual header files into one file frames.h to avoid lots of #include lines (optional step.)
 *
 */
#include <SPI.h>
#include <SD.h>
#include <FTOLED.h>

#include "frames.h"

const byte pin_cs = 7;
const byte pin_dc = 2;
const byte pin_reset = 3;

const byte pin_sd_cs = 4;

OLED oled(pin_cs, pin_dc, pin_reset);

void setup()
{
  oled.begin();
  randomSeed(analogRead(0));
}

int x = 24;
int y = 36;
int dx = 3;
int dy = 0;
int cur_frame = 0;

const uint8_t *frames[] = {
  flier1,
  flier2,
  flier3 
};

const int FRAME_COUNT = sizeof(frames)/sizeof(uint8_t *);

void loop()
{ 
  oled.displayBMP(frames[cur_frame], x, y);
  cur_frame = (cur_frame + 1) % FRAME_COUNT;
  delay(25);
  x = x + dx;
  y = y + dy;
  if(x > 128) {
    x = 128;
    dx = -dx; 
  }
  else if(x < 0) {
    x = 0;
    dx = -dx; 
  }
  if(y > 128) {
    y = 128;
    dy = -dy; 
  }
  else if(y < 0) {
    y = 0;
    dy = -dy; 
  }
  switch(random(10)) {
    case 1:
      dx = dx + 1;
      break;
    case 2:
      dx = dx - 1;
      break;
    case 3:
      dy = dy + 1;
      break;
    case 4:
      dy = dy - 1;
      break;
    default:
      break;
  }
  // Clamp dx & dy to +/-4 (so as not to exceed the black border on our sprite BMP)
  dy = max(-4, min(4, dy));
  dx = max(-4, min(4, dx));
}


/*
 * Simple sketch that cycles the display through available hues,
 * keeping a solid 100% saturation value,
 * simulating slowly circling the HSV colour wheel.
 */
#include <SPI.h>
#include <SD.h>
#include <FTOLED.h>

const byte pin_cs = 7;
const byte pin_dc = 2;
const byte pin_reset = 3;

OLED oled(pin_cs, pin_dc, pin_reset);

float angle;

void setup()
{
  oled.begin();
}

// Translate a hue "angle" -120 to 120 degrees (ie -2PI/3 to 2PI/3) to
// a 6-bit R channel value
//
// This is very slow on a microcontroller, not a great example!
inline int angle_to_channel(float a) {
  if(a < -PI)
    a += 2*PI;
  if(a < -2*PI/3  || a > 2*PI/3)
    return 0;
  float f_channel = cos(a*3/4); // remap 120-degree 0-1.0 to 90 ??
  return ceil(f_channel * 63);
}

void loop() {
  OLED_Colour c;
  c.red = angle_to_channel(angle)>>1;
  c.green = angle_to_channel(angle-2*PI/3);
  c.blue = angle_to_channel(angle-4*PI/3)>>1;
  oled.fillScreen(c);

  angle += 0.02;
  if(angle > PI)
    angle -= 2*PI;
}

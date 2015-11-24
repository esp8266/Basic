/*------------------------------------------------------------------------
  Adafruit_TiCoServo example for ATmega-based boards such as the Arduino
  Uno, Leonardo, Mega, Duemilanove and compatible boards such as the
  Adafruit Pro Trinket, Flora or PJRC Teensy 2.0 (not Trinket or Gemma,
  see other example for that).

  THE GOOD NEWS:
  - NeoPixels and servos in the same sketch!

  THE BAD NEWS:
  - Limited to 2 servos on the most popular boards (4-12 on others).
  - Servos must be on specific pins. This is explained later in the code.
  - Supports the most common AVR-based boards, but not everything.
  - Uses up Timer/Counter 1, a popular choice of other time-critical
    libraries such as WaveHC. Can only do so much in one sketch.
    Some boards (Leonardo, Mega) have additional 16-bit timers available.

  Example reads a potentiometer on pin A0, moves a servo (pin 9) and
  fills a portion of a NeoPixel strip (pin 6) to match.
  ------------------------------------------------------------------------*/

#if defined(__AVR_ATtiny85__)
 #error "This code is for ATmega boards, see other example for ATtiny."
#endif
#include <Adafruit_NeoPixel.h>
#include <Adafruit_TiCoServo.h>

// NeoPixel parameters. These are configurable, but the pin number must
// be different than the servo(s).
#define N_LEDS       60
#define LED_PIN       6

// Servo parameters. These have limited configurability. Compatible pins,
// for example, are determined by the specific hardware. On Arduino Uno,
// Diecimila and Adafruit Pro Trinket, pins 9 and 10 are supported by the
// library. On Arduino Leonardo: pins 5, 9, 10 and 11. Adafruit Flora:
// pins D9, D10. PJRC Teensy 2.0 (not Teensy++ or 3.X): pins 4, 9, 14, 15.
// Arduino Mega: 2, 3, 5, 6, 7, 8, 11, 12, 13, 44, 45, 46.
// Servo position can be specified in degrees or in microseconds; library
// can distinguish between the two. The #defines below are reasonable
// min/max pulse durations (in microseconds) for many servos, but for
// maximum control you'll probably need to do some calibration to find
// the optimal range for your specific servos.
#define SERVO_PIN    9
#define SERVO_MIN 1000 // 1 ms pulse
#define SERVO_MAX 2000 // 2 ms pulse

Adafruit_NeoPixel  strip = Adafruit_NeoPixel(N_LEDS, LED_PIN);
Adafruit_TiCoServo servo;

void setup(void) {
  servo.attach(SERVO_PIN, SERVO_MIN, SERVO_MAX);
  strip.begin();
}

void loop(void) {
  int a, x;
  a = analogRead(0);                            // 0 to 1023
  x = map(a, 0, 1023, SERVO_MIN, SERVO_MAX);    // Scale to servo range
  servo.write(x);                               // Move servo
  x = map(a, 0, 1023, 0, strip.numPixels());    // Scale to strip length
  strip.clear();
  while(x--) strip.setPixelColor(x, 255, 0, 0); // Set pixels
  strip.show();                                 // Update strip
}

/*------------------------------------------------------------------------
  Adafruit_TiCoServo example for ATtiny-based boards such as the Adafruit
  Trinket and Gemma (not Pro Trinket -- see other example for that).
  Demonstrates NeoPixel animation and servo motion in the same sketch,
  using Timer/Counter 1 for servo control.

  THE GOOD NEWS:
  - NeoPixels and servos in the same sketch!

  THE BAD NEWS:
  - Only works with servos on pin 1 and/or 4 (only pin D1 on Gemma).
  - Servo resolution is VERY coarse on ATtiny boards...only eight or so
    distinct positions (but many sketches only need two, e.g. up/down,
    open/closed, etc.  ATtiny version of the library ONLY works in 'raw'
    timer/counter ticks, because scaling from degrees or microseconds is
    pretty much useless, many movements would simply not register.

  Example moves servos on pins 1 and 4 while moving a dot along a NeoPixel
  strip on pin 0.
  ------------------------------------------------------------------------*/

#if !defined(__AVR_ATtiny85__)
 #error "This code is for ATtiny boards, see other example for ATmega."
#endif
#include <Adafruit_NeoPixel.h>
#include <Adafruit_TiCoServo.h>
#include <avr/power.h>

// NeoPixel parameters.  These are configurable, but the pin number must
// be different than the servo(s).
#define N_LEDS      16
#define LED_PIN      0

// Servo parameters.  The pins are NOT configurable -- these are the only
// two that the TiCoServo library can handle on a Trinket.  Servo position
// is specified in raw timer/counter ticks (1 tick = 0.128 milliseconds).
// Servo pulse timing is typically 1-2 ms, but can vary slightly among
// servos, so you may need to tweak these limits to match your reality.
#define SERVO_0_PIN  1
#define SERVO_1_PIN  4
#define SERVO_MIN    8 // ~1 ms pulse
#define SERVO_MAX   15 // ~2 ms pulse

Adafruit_NeoPixel  strip = Adafruit_NeoPixel(N_LEDS, LED_PIN);
Adafruit_TiCoServo servo[2];

void setup(void) {
#if (F_CPU == 16000000L)
  // 16 MHz Trinket requires setting prescale for correct timing.
  // This MUST be done BEFORE servo.attach()!
  clock_prescale_set(clock_div_1);
#endif
  servo[0].attach(SERVO_0_PIN);
  servo[1].attach(SERVO_1_PIN);
  strip.begin();
}

uint8_t pixelNum = 0, // NeoPixel dot position
        state    = 0; // Servo position counter (cycles between 4 states)

void loop(void) {

  strip.setPixelColor(pixelNum, 0);         // Erase old dot
  if(++pixelNum >= strip.numPixels()) {     // Update dot pos.  Off end?
    pixelNum = 0;                           // Yes, reset to start and
    switch(state) {                         // make servos do something...
     case 0:
      servo[0].write(SERVO_MAX);
      break;
     case 1:
      servo[1].write(SERVO_MAX);
      break;
     case 2:
      servo[0].write(SERVO_MIN);
      break;
     case 3:
      servo[1].write(SERVO_MIN);
      break;
    }
    if(++state > 3) state = 0;              // Change servo state next time
  }
  strip.setPixelColor(pixelNum, 255, 0, 0); // Set new dot
  strip.show();                             // Update strip

  delay(30);
}

Adafruit_TiCoServo
==================

An alternate servo library when NeoPixels and servos are both needed in the same sketch (the stock Arduino Servo library stutters with the addition of NeoPixels). Has some limitations of its own, see examples for pros and cons. See Adafruit_TiCoServo.cpp for attribution.

This library only handles the servo control, you'll still need to install and use the NeoPixel library for the LEDs. It's not dependent on that library though, and could be used separately for other (non-NeoPixel) situations that might benefit from hardware-PWM-driven servos.

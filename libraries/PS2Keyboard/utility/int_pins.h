// interrupt pins for known boards

// Teensy and maybe others automatically define this info
#if !defined(CORE_INT0_PIN) && !defined(CORE_INT1_PIN) && !defined(CORE_INT2_PIN)&& !defined(CORE_INT3_PIN)

// Arduino Mega
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // Arduino Mega
  #define CORE_INT0_PIN  2
  #define CORE_INT1_PIN  3
  #define CORE_INT2_PIN  21
  #define CORE_INT3_PIN  20
  #define CORE_INT4_PIN  19
  #define CORE_INT5_PIN  18

// Arduino Due (untested)
#elif defined(__SAM3X8E__) 
  #define CORE_INT_EVERY_PIN
  #ifndef PROGMEM
  #define PROGMEM
  #endif
  #ifndef pgm_read_byte
  #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
  #endif

// Arduino Leonardo (untested)
#elif defined(__AVR_ATmega32U4__) && !defined(CORE_TEENSY)
  #define CORE_INT0_PIN  3
  #define CORE_INT1_PIN  2
  #define CORE_INT2_PIN  0
  #define CORE_INT3_PIN  1

// Sanguino (untested)
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__) // Sanguino
  #define CORE_INT0_PIN  10
  #define CORE_INT1_PIN  11
  #define CORE_INT2_PIN  2

// Chipkit Uno32 (untested)
#elif defined(__PIC32MX__) && defined(_BOARD_UNO_)
  #define CORE_INT0_PIN  38
  #define CORE_INT1_PIN  2
  #define CORE_INT2_PIN  7
  #define CORE_INT3_PIN  8
  #define CORE_INT4_PIN  35

// Chipkit Mega32 (untested)
#elif defined(__PIC32MX__) && defined(_BOARD_MEGA_)
  #define CORE_INT0_PIN  3
  #define CORE_INT1_PIN  2
  #define CORE_INT2_PIN  7
  #define CORE_INT3_PIN  21
  #define CORE_INT4_PIN  20

// http://hlt.media.mit.edu/?p=1229
#elif defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  #define CORE_INT0_PIN  2

// Arduino Uno, Duemilanove, LilyPad, Mini, Fio, etc...
#else
  #define CORE_INT0_PIN  2
  #define CORE_INT1_PIN  3

#endif
#endif


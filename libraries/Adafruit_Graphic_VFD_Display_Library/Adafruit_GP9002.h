#ifndef _ADAFRUIT_GP9002_H
#define _ADAFRUIT_GP9002_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <SPI.h>

#include "Adafruit_GFX.h"

#define BLACK 0
#define WHITE 1

#define GP9002_DISPLAYSOFF 0x00
#define GP9002_DISPLAY1ON 0x01
#define GP9002_DISPLAY2ON 0x02
#define GP9002_ADDRINCR 0x04
#define GP9002_ADDRHELD 0x05
#define GP9002_CLEARSCREEN 0x06
#define GP9002_CONTROLPOWER 0x07
#define GP9002_DATAWRITE 0x08
#define GP9002_DATAREAD 0x09
#define GP9002_LOWERADDR1 0x0A
#define GP9002_HIGHERADDR1 0x0B
#define GP9002_LOWERADDR2 0x0C
#define GP9002_HIGHERADDR2 0x0D
#define GP9002_ADDRL 0x0E
#define GP9002_ADDRH 0x0F
#define GP9002_OR 0x10
#define GP9002_XOR 0x11
#define GP9002_AND 0x12
#define GP9002_BRIGHT 0x13
#define GP9002_DISPLAY 0x14
#define GP9002_DISPLAY_MONOCHROME 0x10
#define GP9002_DISPLAY_GRAYSCALE 0x14
#define GP9002_INTMODE 0x15
#define GP9002_DRAWCHAR 0x20
#define GP9002_CHARRAM 0x21
#define GP9002_CHARSIZE 0x22
#define GP9002_CHARBRIGHT 0x24


class Adafruit_GP9002 : public Adafruit_GFX {
 public:
  Adafruit_GP9002(int8_t SCLK, int8_t MISO, int8_t MOSI, 
		  int8_t CS, int8_t DC);
  Adafruit_GP9002(int8_t CS, int8_t DC);

  // particular to this display
  void begin(void);
  uint8_t slowSPIread();
  uint8_t fastSPIread();
  void slowSPIwrite(uint8_t);
  void fastSPIwrite(uint8_t);

  void command(uint8_t c);
  void dataWrite(uint8_t c);
  uint8_t dataRead(void);
  void setBrightness(uint8_t val);
  void invert(uint8_t i);

  void displayOn();
  void displayOff();
  void clearDisplay(void);

  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

 private:
  int8_t _miso, _mosi, _sclk, _dc, _cs;

  volatile uint8_t *mosiport, *misopin, *clkport, *csport, *dcport;
  uint8_t mosipinmask, misopinmask, clkpinmask, cspinmask, dcpinmask;

  boolean hwSPI;

  void spiwrite(uint8_t c);
};

#endif

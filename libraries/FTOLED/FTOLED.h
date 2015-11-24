/*

  OLED.h - Support library for Freetronics 128x128 OLED display

  Copyright (C) 2013 Freetronics, Inc. (info <at> freetronics <dot> com)

  Written by Angus Gratton

  Note that this library uses the SPI port for the fastest, low
  overhead writing to the display. Keep an eye on conflicts if there
  are any other devices running from the same SPI port, and that the
  chip select on those devices is correctly set to be inactive when the
  OLED is being written to.

 ---

 This program is free software: you can redistribute it and/or modify it under the terms
 of the version 3 GNU General Public License as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OLED_H_
#define OLED_H_

//Arduino header name is version dependent
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "pins_arduino.h"
#include <SPI.h>
#include "Print.h"
#include "progmem_compat.h"

#define ROWS 128
#define COLUMNS 128
#define ROW_MASK (ROWS-1)
#define COLUMN_MASK (COLUMNS-1)

class File; /* Provided by SD library except sometimes on Yun, see comment at top of FTOLED_BMP.cpp */

struct OLED_Colour
{
  byte red   : 5;
  byte green : 6;
  byte blue  : 5;
};

/* Allow use of both AU/UK spelling OLED_Colour and US spelling OLED_Color */
typedef OLED_Colour OLED_Color;

#if !defined(FTOLED_NO_COLOUR_PRESETS) && !defined(FTOLED_NO_COLOR_PRESETS)
#include "FTOLED_Colours.h"
#endif

/* Add an internal-use-only preset colours for BLACK, in case the preset
 colours aren't loaded */
const OLED_Colour OLED_BLACK = {   0,   0,   0 };


const byte MAX_RED = 31;
const byte MAX_GREEN = 63;
const byte MAX_BLUE = 31;

// Display mode argument for setDisplayMode()
enum OLED_Display_Mode {
  DISPLAY_OFF = 0,                 // No pixels on
  DISPLAY_ALL_PIXELS_FULL = 1,     // All pixels on GS level 63 (ie max brightness)
  DISPLAY_NORMAL = 2,              // Normal display on
  DISPLAY_INVERSE = 3,
};

// OLED GPIO mode for setGPIO()
enum OLED_GPIO_Mode {
  OLED_HIZ = 0,
  OLED_LOW = 2,
  OLED_HIGH = 3,
};

// Status code returned by displayBMP()
enum BMP_Status {
  BMP_OK = 0,                          // Image Displayed OK
  BMP_INVALID_FORMAT = 1,              // Not a BMP file
  BMP_UNSUPPORTED_HEADER = 2,          // Has a rare BMP header format (not v2 or v3)
  BMP_TOO_MANY_COLOURS = 3,            // Is a 32bpp image (not supported)
  BMP_COMPRESSION_NOT_SUPPORTED = 4,   // Uses RLE or another unsupported compression format
  BMP_UNSUPPORTED_COLOURS = 5,         // Uses non-standard colour format
  BMP_ORIGIN_OUTSIDE_IMAGE = 6         // You've asked to crop a section that doesn't exist in the image
};

enum OLED_Orientation {
  ROTATE_0 = 0,
  ROTATE_90 = 1,
  ROTATE_180 = 2,
  ROTATE_270 = 3
};

class OLED_TextBox;

class OLED
{
  friend class OLED_TextBox;
public:
  OLED(byte pin_ncs, byte pin_dc, byte pin_reset) :
    pin_ncs(pin_ncs),
    pin_dc(pin_dc),
    pin_reset(pin_reset),
    gpio_status(OLED_HIZ | OLED_HIZ<<2),
    orientation(ROTATE_0)
  {}

  void begin();

  // Set the colour of a single pixel
  void setPixel(const byte x, const byte y, const OLED_Colour colour);

  // Fill the screen with a single solid colour
  void fillScreen(const OLED_Colour);
  void clearScreen() { fillScreen(OLED_BLACK); }

  // Turn the display on or off
  void setDisplayOn(bool on);

  //Draw a line from x1,y1 to x2,y2
  void drawLine( int x1, int y1, int x2, int y2, OLED_Colour colour );

  //Draw a box(rectangle) from (x1,y1) to (x2,y2), with sides edgeWidth pixels wide
  void drawBox( int x1, int y1, int x2, int y2, int edgeWidth, OLED_Colour colour);

  //Draw a filled box(rectangle) from (x1,y1) to (y1,y2), optionally with sides edgeWidth pixels wide
  void drawFilledBox( int x1, int y1, int x2, int y2, OLED_Colour fillColour, int edgeWidth, OLED_Colour edgeColour);
  void drawFilledBox( int x1, int y1, int x2, int y2, OLED_Colour fillColour) { drawFilledBox(x1,y1,x2,y2,fillColour,0,OLED_BLACK); }

  // Draw an outline of a circle of radius r centred at x,y
  void drawCircle( int xCenter, int yCenter, int radius, OLED_Colour colour);

  // Draw an filled circle of radius r at x,y centre
  void drawFilledCircle( int xCenter, int yCenter, int radius, OLED_Colour colour);

  //Select a text font
  void selectFont(const uint8_t* font);
  const inline uint8_t *getFont(void) { return font; }

  //Draw a single character
  int drawChar(const int x, const int y, const char letter, const OLED_Colour colour, const OLED_Colour background);

  // Draw a full string, either from PROGMEM (AVR only) or from normal C-style pointer, or Arduino string object
#ifdef __AVR__
  void drawString_P(int x, int y, const char *flashStr, OLED_Colour foreground, OLED_Colour background);
  inline void drawString(int x, int y, const __FlashStringHelper *flashStr, OLED_Colour foreground, OLED_Colour background) {
    return drawString_P(x,y,(const char*)flashStr,foreground,background);
  }
#endif
  void drawString(int x, int y, const char *bChars, OLED_Colour foreground, OLED_Colour background);
  void drawString(int x, int y, const String &str, OLED_Colour foreground, OLED_Colour background);

  //Find the width of a character
  int charWidth(const char letter);

  //Find the width of a string (width of all characters plus 1 pixel "kerning" between each character)
#ifdef __AVR__
  unsigned int stringWidth_P(const char *flashStr);
  inline unsigned int stringWidth(const __FlashStringHelper *flashStr) {
    return stringWidth_P((const char*)flashStr);
  }
#endif
  unsigned int stringWidth(const char *bChars);
  unsigned int stringWidth(const String &str);

  // Bitmap stuff

  // Given 'File' containing a BMP image, show it onscreen with bottom left corner at (x,y)
  BMP_Status displayBMP(File &source, const int x, const int y);
  BMP_Status displayBMP(File &source, const int from_x, const int from_y, const int to_x, const int to_y);

  // Given 'pgm_addr', a pointer to a PROGMEM buffer (or const buffer on ARM) containing a BMP,
  // show it onscreen with bottom left corner at (x,y)
  BMP_Status displayBMP(const uint8_t *pgm_addr, const int x, const int y);
  BMP_Status displayBMP(const uint8_t *pgm_addr, const int from_x, const int from_y, const int to_x, const int to_y);

  /* Set the grayscale table for pixel brightness to one of these precanned defaults */
  void setDefaultGrayscaleTable();
  void setBrightGrayscaleTable();
  void setDimGrayscaleTable();

  /* Set a custom sized grayscale table. "table" must be address of a
     PROGMEM table holding 64 grayscale level values (GS0..GS63), which must be
     strictly incrementing (see section 8.8 in the datasheet.) Values
     in the table can have values 0-180.
  */
  inline void setGrayscaleTable_P(const byte *table);

  void setGPIO1(OLED_GPIO_Mode gpio1);

  /* Set display mode. See enum OLED_Display_Mode, above. */
  inline void setDisplayMode(OLED_Display_Mode mode) {
    assertCS();
    writeCommand(0xA4+(byte)mode);
    releaseCS();
  }

  inline void setOrientation(OLED_Orientation orientation) {
    this->orientation = orientation;
  }

 protected:
  byte pin_ncs;
  byte pin_dc;
  byte pin_reset;
  byte gpio_status;
  OLED_Orientation orientation;

  uint8_t *font;

  inline void assertCS() { digitalWrite(pin_ncs, LOW); }
  inline void releaseCS() { digitalWrite(pin_ncs, HIGH); }

  // Note: GPIO0 is panel power on OLED128, hence better to use setDisplayOn()
  void setGPIO0(OLED_GPIO_Mode gpio0);

  /* These protected methods are for implementing basic OLED commands.
     They all assume that the CS is asserted before they've been called
  */
  inline void writeCommand(byte command)
  {
    digitalWrite(pin_dc, LOW);
    SPI.transfer(command);
    digitalWrite(pin_dc, HIGH);
  }

  inline void writeData(byte data)
  {
    SPI.transfer(data);
  }

  inline void writeData(OLED_Colour colour)
  {
    writeData((colour.green>>3)|(colour.red<<3));
    writeData((colour.green<<5)|(colour.blue));
  }

  inline void writeCommand(byte command, byte data) {
    writeCommand(command);
    writeData(data);
  }

  inline void writeData(byte *data, unsigned int length)
  {
    for(unsigned int i = 0; i < length; i++) {
      writeData(data[i]);
    }
  }

  inline void _setPixel(const byte x, const byte y, const OLED_Colour colour)
  {
    startWrite(x,y,x,y,false);
    writeData(colour);
  }

  enum OLED_Command_Lock {
    DISPLAY_COMMAND_UNLOCK = 0x12,        // Allow commands (default state)
    DISPLAY_COMMAND_LOCK = 0x16,          // Disallow all commands until/except next UNLOCK

    DISPLAY_COMMAND_LOCK_SPECIAL = 0xB0,  // Lock out "special" commands always (default state)
    DISPLAY_COMMAND_ALLOW_SPECIAL = 0xB1, // Allow "special" commands when unlocked
  };

  // Direct commands to the module
  inline void setCommandLock(OLED_Command_Lock lock_command)
  {
    writeCommand(0xFD, (byte)lock_command);
  }

// Display clock divisor options, used by setDisplayClock
#define DISPLAY_CLOCK_DIV_1 0
#define DISPLAY_CLOCK_DIV_2 1
#define DISPLAY_CLOCK_DIV_4 2
#define DISPLAY_CLOCK_DIV_8 3
#define DISPLAY_CLOCK_DIV_16 4
#define DISPLAY_CLOCK_DIV_32 5
#define DISPLAY_CLOCK_DIV_64 6
#define DISPLAY_CLOCK_DIV_128 7
#define DISPLAY_CLOCK_DIV_256 8
#define DISPLAY_CLOCK_DIV_512 9
#define DISPLAY_CLOCK_DIV_1024 10

  /* set display refresh clock
   * Args:
   * divisor - a constant (DISPLAY_CLOCK_DIV_xxx), see above.
   * frequency - a value 0-15 proportional to oscillator frequency.
   */
  inline void setDisplayClock(byte divisor, byte frequency)
  {
    writeCommand(0xB3, divisor | ((frequency&0x0F)<<4));
  }

  /* set multiplex ratio (Default of ROW_MASK/127 for 128 should be fine in nearly all cases)
   */
  inline void setMultiPlexRatio(byte mux_ratio)
  {
    writeCommand(0xCA, mux_ratio & ROW_MASK);
  }

  /* set display offset row (0-127) */
  inline void setDisplayOffset(byte row)
  {
    writeCommand(0xA2, row & ROW_MASK);
  }

  /* set starting row for display (0-127) */
  inline void setStartRow(byte row)
  {
    writeCommand(0xA1, row & ROW_MASK);
  }

  /* Start a pixel data write. This function also accounts for remapping coordinates based
     on current orientation. */
  void startWrite(byte start_x, byte start_y, byte to_x, byte to_y, bool fill_vertical);

 /* set color channel contrasts. A,B,C are R,G,B values unless REMAP_ORDER_BGR is set */
  inline void setColorContrasts(byte a, byte b, byte c)
  {
    writeCommand(0xC1);
    writeData(a);
    writeData(b);
    writeData(c);
  }

  /* Set Master contrast, value 0-15 */
  inline void setMasterContrast(byte contrast)
  {
    writeCommand(0xC7, contrast & 0x0F);
  }

  /* Set the reset (phase 1) and precharge (phase 2) lengths.
     Reset range is 5-31 DCLK periods (odd values only), precharge is 3-15 DCLK periods
   */
  inline void setResetPrechargePeriods(byte resetLength, byte precharge) {
    resetLength = (resetLength&~1)/2 - 1; // Value we write is 3-15 same as the others
    writeCommand(0xB1, (resetLength>>8)|(precharge&0x0F));
  }

  /* Set precharge voltage, level is a proportion of Vcc where 0x00=0.2 0x1F=0.60,
     Default of 0x17 is 0.50 */
  inline void setPrechargeVoltage(byte level) {
    writeCommand(0xBB, level & 0x1F);
  }

  /* Set second precharge period (phase 3) as number of DCLK periods 1-15. Default is 8. */
  inline void setSecondPrechargePeriod(byte clocks) {
    clocks = clocks & 0x0F;
    writeCommand(0xB6, clocks ? clocks : 8);
  }

  /* Set lock bits. Reset means 0x12, 0x16? means nothing works except reset and another unlock? */
  inline void setLockBits(byte lock_bits) {
    writeCommand(0xFD, lock_bits);
  }

  // Internal templated displayBMP method, allows us to treat SD card files and PROGMEM buffers
  // via the same code paths
  template<typename SourceType> BMP_Status _displayBMP(SourceType &source, const int from_x, const int from_y, const int to_x, const int to_y);
};

class OLED_TextBox : public Print {
public:
  OLED_TextBox(OLED &oled, int left = 0, int bottom = 0, int width = COLUMNS, int height = ROWS);
  virtual size_t write(uint8_t);
  void clear();
  void reset();
  void setForegroundColour(OLED_Colour colour);
  void setBackgroundColour(OLED_Colour colour);
  inline void setForegroundColor(OLED_Colour color) { setForegroundColour(color); }
  inline void setBackgroundColor(OLED_Colour color) { setBackgroundColour(color); }

private:
  OLED &oled;
  uint8_t left;
  uint8_t bottom;
  uint8_t width;
  uint8_t height;
  int16_t cur_x;
  int16_t cur_y;
  uint8_t max_rows;
  uint16_t buf_sz;
  char *buffer;
  OLED_Colour foreground;
  OLED_Colour background;
  bool pending_newline;

  void scroll(uint8_t fontHeight);
  void clear_area();
};

// Six byte header at beginning of FontCreator font structure, stored in PROGMEM
struct FontHeader {
  uint16_t size;
  uint8_t fixedWidth;
  uint8_t height;
  uint8_t firstChar;
  uint8_t charCount;
};

#endif

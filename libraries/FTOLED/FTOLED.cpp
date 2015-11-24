#include <FTOLED.h>

// Remap format flags

#define REMAP_HORIZONTAL_INCREMENT 0
#define REMAP_VERTICAL_INCREMENT (1<<0)

#define REMAP_COLUMNS_LEFT_TO_RIGHT 0
#define REMAP_COLUMNS_RIGHT_TO_LEFT (1<<1)

#define REMAP_ORDER_BGR 0
#define REMAP_ORDER_RGB (1<<2)

#define REMAP_SCAN_UP_TO_DOWN 0
#define REMAP_SCAN_DOWN_TO_UP (1<<4)

#define REMAP_COM_SPLIT_ODD_EVEN (1<<5)

#define REMAP_COLOR_8BIT 0
#define REMAP_COLOR_RGB565 (1<<6)
#define REMAP_COLOR_18BIT (2<<6)

#define DEFAULT_REMAP_FLAGS ( REMAP_ORDER_RGB                   \
                              | REMAP_COM_SPLIT_ODD_EVEN        \
                              | REMAP_COLOR_RGB565 )

// Clamp a value between two limits
template<typename T> inline void clamp(T &value, T lower, T upper) {
  if(value < lower)
    value = lower;
  else if(value > upper)
    value = upper;
}

inline void clampXY(int &x, int &y) {
  clamp(x, 0, COLUMNS-1);
  clamp(y, 0, ROWS-1);
}

// Swap A & B "in place" (well, with a temp variable!)
template<typename T> inline void swap(T &a, T &b)
{
  T tmp(a); a=b; b=tmp;
}

// Check a<=b, and swap them otherwise
template<typename T> inline void ensureOrder(T &a, T &b)
{
  if(b<a) swap(a,b);
}

void OLED::begin() {
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
#ifdef __AVR__
  SPI.setClockDivider(SPI_CLOCK_DIV2); // 8MHz on standard Arduino models
#else
  SPI.setClockDivider(5); // 16.8MHz on Due
#endif

  pinMode(pin_ncs, OUTPUT);
  digitalWrite(pin_ncs, HIGH);
  pinMode(pin_dc, OUTPUT);
  digitalWrite(pin_dc, HIGH);
  if(pin_reset >= 0) {
    pinMode(pin_reset, OUTPUT);
    digitalWrite(pin_reset, HIGH);
    digitalWrite(pin_reset, HIGH);
    delay(1);
    digitalWrite(pin_reset, LOW);
    delay(1);
    digitalWrite(pin_reset, HIGH);
    delay(1);
  }

  setDisplayOn(false);

  assertCS();

  setLockBits(0x12); // enter unlocked state
  setLockBits(0xB1); // allow all commands

  setDisplayClock(DISPLAY_CLOCK_DIV_1, 15); // "approx 90fps" ?
  setStartRow(32);
  setColorContrasts(0xE8,0xA0,0xC8);
  setMasterContrast(0x0F);
  setResetPrechargePeriods(5,6);
  setSecondPrechargePeriod(8);

  releaseCS();

  setDefaultGrayscaleTable();

  assertCS();

  setMultiPlexRatio(0x7F);		  // 1/128 Duty (0x0F~0x7F)

  releaseCS();

  // these are public methods so they manage CS on their own:
  clearScreen();
  setDisplayOn(true);
}

// setPixel has two methods, public method asserts/deasserts CS protected method doesn't
void OLED::setPixel(const byte x, const byte y, const OLED_Colour colour)
{
  assertCS();
  _setPixel(x,y,colour);
  releaseCS();
}

void OLED::setDisplayOn(bool on)
{
  // GPIO0 drives OLED_VCC, driving it high turns on the boost converter
  // module, driving it low turns it off.

  if(on) {
    setGPIO0(OLED_HIGH);
    delay(100);
  }
  assertCS();
  writeCommand(on ? 0xAF : 0xAE);
  releaseCS();

  if(!on) {
    setGPIO0(OLED_LOW);
    delay(100);
  }
}

void OLED::fillScreen(const OLED_Colour colour)
{
  assertCS();
  startWrite(0,0,COLUMN_MASK,ROW_MASK,false);
  for(int p = 0; p < ROWS*COLUMNS; p++) {
    writeData(colour);
  }
  releaseCS();
}

void OLED::drawLine( int x1, int y1, int x2, int y2, OLED_Colour colour )
{
  // Note: hard clamping here means that diagonal lines that exceed the
  // limits of the display will be drawn with different angles to if they
  // were really drawn to those limits
  clampXY(x1, y1);
  clampXY(x2, y2);

  // Shortcuts for horizontal and vertical lines, many fewer writes
  assertCS();
  if(x1==x2) {
    ensureOrder(y1,y2);
    startWrite(x1,y1,x2,y2,true);
    while(y1 <= y2) {
      writeData(colour);
      y1++;
    }
  }
  else if(y1==y2) {
    ensureOrder(x1,x2);
    startWrite(x1,y1,x2,y2,false);
    while(x1 <= x2) {
      writeData(colour);
      x1++;
    }
  }
  else { // Diagonal line, uses code from DMD
    int dy = y2 - y1;
    int dx = x2 - x1;
    int stepx, stepy;

    if (dy < 0) {
	    dy = -dy;
	    stepy = -1;
    } else {
	    stepy = 1;
    }
    if (dx < 0) {
	    dx = -dx;
	    stepx = -1;
    } else {
	    stepx = 1;
    }
    dy <<= 1;			// dy is now 2*dy
    dx <<= 1;			// dx is now 2*dx

    _setPixel(x1, y1, colour);
    if (dx > dy) {
	    int fraction = dy - (dx >> 1);	// same as 2*dy - dx
	    while (x1 != x2) {
	        if (fraction >= 0) {
		        y1 += stepy;
		        fraction -= dx;	// same as fraction -= 2*dx
	        }
	        x1 += stepx;
	        fraction += dy;	// same as fraction -= 2*dy
	        _setPixel(x1, y1, colour);
	    }
    } else {
	    int fraction = dx - (dy >> 1);
	    while (y1 != y2) {
	        if (fraction >= 0) {
		        x1 += stepx;
		        fraction -= dy;
	        }
	        y1 += stepy;
	        fraction += dx;
	        _setPixel(x1, y1, colour);
	    }
    }
  }
  releaseCS();
}

void OLED::drawBox( int x1, int y1, int x2, int y2, int edgeWidth, OLED_Colour colour)
{
  clampXY(x1, y1);
  clampXY(x2, y2);

  // Make sure (x1,y1) is always the top left corner
  ensureOrder(x1,x2);
  ensureOrder(y1,y2);

  assertCS();

  // Left side
  startWrite(x1,y1,x1+edgeWidth-1,y2,false);
  for(int n=0;n<(1+y2-y1)*edgeWidth;n++) {
    writeData(colour);
  }

  // Top side
  startWrite(x1,y1,x2,y1+edgeWidth-1,false);
  for(int n=0;n<(1+x2-x1)*edgeWidth;n++) {
    writeData(colour);
  }
  
  // Right side
  startWrite(x2-(edgeWidth-1),y1,x2,y2,false);
  for(int n=0;n<(1+y2-y1)*edgeWidth;n++) {
    writeData(colour);
  }

  // Bottom side
  startWrite(x1,y2-(edgeWidth-1),x2,y2,false);
  for(int n=0;n<(1+x2-x1)*edgeWidth;n++) {
    writeData(colour);
  }

  releaseCS();
}

void OLED::drawFilledBox( int x1, int y1, int x2, int y2, OLED_Colour fillColour, int edgeWidth, OLED_Colour edgeColour)
{
  clampXY(x1, y1);
  clampXY(x2, y2);

  // Make sure (x1,y1) is always the top left corner
  ensureOrder(x1,x2);
  ensureOrder(y1,y2);

  assertCS();

  startWrite(x1,y1,x2,y2,true);

  for(int x = x1; x <= x2; x++) {
    for(int y = y1; y <= y2; y++) {
      if(edgeWidth > 0 && (x<x1+edgeWidth || y<y1+edgeWidth || x>x2-edgeWidth || y>y2-edgeWidth))
        writeData(edgeColour);
      else
        writeData(fillColour);
    }
  }

  releaseCS();
}

void OLED::drawCircle( int xCenter, int yCenter, int radius, OLED_Colour colour)
{
  // Bresenham's circle drawing algorithm
  int x = -radius;
  int y = 0;
  int error = 2-2*radius;
  assertCS();
  while(x < 0) {
    _setPixel(xCenter-x, yCenter+y, colour);
    _setPixel(xCenter-y, yCenter-x, colour);
    _setPixel(xCenter+x, yCenter-y, colour);
    _setPixel(xCenter+y, yCenter+x, colour);
    radius = error;
    if (radius <= y) error += ++y*2+1;
    if (radius > x || error > y) error += ++x*2+1;
  }
  releaseCS();
}

void OLED::drawFilledCircle( int xCenter, int yCenter, int radius, OLED_Colour fillColour)
{
  // Bresenham's circle drawing algorithm, filling with vertical line segments to/from origin
  int x = -radius;
  int y = 0;
  int error = 2-2*radius;
  while(x < 0) {
    drawLine(xCenter-x, yCenter,   xCenter-x, yCenter+y, fillColour);
    drawLine(xCenter-y, yCenter,   xCenter-y, yCenter-x, fillColour);
    drawLine(xCenter+x, yCenter-y, xCenter+x, yCenter, fillColour);
    drawLine(xCenter+y, yCenter+x, xCenter+y, yCenter, fillColour);
    radius = error;
    if (radius <= y) error += ++y*2+1;
    if (radius > x || error > y) error += ++x*2+1;
  }
}

void OLED::setDefaultGrayscaleTable()
{
  assertCS();
  writeCommand(0xB9);
  releaseCS();
}

static const byte BRIGHT_TABLE[64] PROGMEM = { 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                               12, 13, 14, 15, 16, 17, 18, 19, 21,
                                               23, 25, 27, 29, 31, 33, 35, 37, 39,
                                               42, 45, 48, 51, 54, 57, 60, 63, 66,
                                               69, 72, 76, 80, 84, 88, 92, 96, 100,
                                               104, 108, 112, 116, 120, 125, 130,
                                               135, 140, 145, 150, 155, 160, 165,
                                               170, 175, 180 };

void OLED::setBrightGrayscaleTable()
{
  setGrayscaleTable_P(BRIGHT_TABLE);
}

static const byte DIM_TABLE[64] PROGMEM = {0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6,
                                           6, 7, 7, 8, 8, 9, 9, 10, 11, 12, 13,
                                           14, 15, 16, 17, 18, 19, 21, 22, 24,
                                           25, 27, 28, 30, 31, 33, 34, 36, 38,
                                           40, 42, 44, 46, 48, 50, 52, 54, 56,
                                           58, 60, 62, 65, 67, 70, 72, 75, 77,
                                           80, 82, 85, 87, 90 };

void OLED::setDimGrayscaleTable()
{
  setGrayscaleTable_P(DIM_TABLE);
}

void OLED::setGrayscaleTable_P(const byte *table)
{
  assertCS();
  writeCommand(0xB8);
  for(int gs = 0; gs < 63; gs++) {
    writeData(pgm_read_byte(table+gs));
  }
  releaseCS();
}

void OLED::setGPIO0(OLED_GPIO_Mode gpio0)
{
  assertCS();
  gpio_status = (gpio_status & ~0x03) | (uint8_t)gpio0;
  writeCommand(0xB5, gpio_status);
  releaseCS();
}

void OLED::setGPIO1(OLED_GPIO_Mode gpio1)
{
  assertCS();
  gpio_status = (gpio_status & ~0x0C) | (uint8_t)gpio1 << 2;
  writeCommand(0xB5, gpio_status);
  releaseCS();
}

  /* Start a pixel data write. This function also accounts for remapping coordinates based
     on current orientation. 

     from_y, to_y are inclusive coordinates
  */
void OLED::startWrite(byte from_x, byte from_y, byte to_x, byte to_y, bool fill_vertical) {
  byte remap;
  bool swap_xy;

  // Use the hardware where possible to remap the coordinate system,
  // although also need to swap X & Y where appropriate
  //
  // (NB: this is a list of ifs not a switch statement due to a gcc 4.4.1 compiler bug)
  if(orientation == ROTATE_180)    {
    remap = DEFAULT_REMAP_FLAGS | REMAP_COLUMNS_RIGHT_TO_LEFT | REMAP_SCAN_DOWN_TO_UP;
    swap_xy = false;
  }
  else if(orientation == ROTATE_90) {
    remap = DEFAULT_REMAP_FLAGS | REMAP_COLUMNS_RIGHT_TO_LEFT | REMAP_SCAN_UP_TO_DOWN;
    swap_xy = true;
  }
  else if(orientation == ROTATE_270) {
    remap = DEFAULT_REMAP_FLAGS | REMAP_COLUMNS_LEFT_TO_RIGHT | REMAP_SCAN_DOWN_TO_UP;
    swap_xy = true;
  }
  else { // ROTATE_0
    remap = DEFAULT_REMAP_FLAGS | REMAP_COLUMNS_LEFT_TO_RIGHT | REMAP_SCAN_UP_TO_DOWN;
    swap_xy = false;
  }

  if(fill_vertical != swap_xy)
    remap |= REMAP_VERTICAL_INCREMENT;
  else
    remap |= REMAP_HORIZONTAL_INCREMENT;
  writeCommand(0xA0, remap);

  const byte CMD_COLS = 0x15;
  const byte CMD_ROWS = 0x75;

  // set columns (normal) or rows (swapped)
  writeCommand(swap_xy ? CMD_ROWS : CMD_COLS);
  writeData(from_x);
  writeData(to_x);

  // set rows
  writeCommand(swap_xy ? CMD_COLS : CMD_ROWS);
  writeData(from_y);
  writeData(to_y);

  // setWriteRam command
  writeCommand(0x5C);
}

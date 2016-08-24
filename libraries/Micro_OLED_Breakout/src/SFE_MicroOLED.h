/****************************************************************************** 
SFE_MicroOLED.h
Header file for the MicroOLED Arduino Library

Jim Lindblom @ SparkFun Electronics
October 26, 2014
https://github.com/sparkfun/Micro_OLED_Breakout/tree/master/Firmware/Arduino/libraries/SFE_MicroOLED

This file defines the hardware interface(s) for the Micro OLED Breakout. Those
interfaces include SPI, I2C and a parallel bus.

Development environment specifics:
Arduino 1.0.5
Arduino Pro 3.3V
Micro OLED Breakout v1.0

This code was heavily based around the MicroView library, written by GeekAmmo
(https://github.com/geekammo/MicroView-Arduino-Library), and released under 
the terms of the GNU General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option) any later 
version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef SFE_MICROOLED_H
#define SFE_MICROOLED_H

#include <stdio.h>
#include <Arduino.h>
#include <avr/pgmspace.h>

#define swap(a, b) { float t = a; a = b; b = t; }

#define I2C_ADDRESS_SA0_0 0b0111100
#define I2C_ADDRESS_SA0_1 0b0111101
#define I2C_COMMAND 0x00
#define I2C_DATA 0x40

#define BLACK 0
#define WHITE 1

#define LCDWIDTH			64
#define LCDHEIGHT			48
#define FONTHEADERSIZE		6

#define NORM				0
#define XOR					1

#define PAGE				0
#define ALL					1

#define WIDGETSTYLE0			0
#define WIDGETSTYLE1			1
#define WIDGETSTYLE2			2

#define SETCONTRAST 		0x81
#define DISPLAYALLONRESUME 	0xA4
#define DISPLAYALLON 		0xA5
#define NORMALDISPLAY 		0xA6
#define INVERTDISPLAY 		0xA7
#define DISPLAYOFF 			0xAE
#define DISPLAYON 			0xAF
#define SETDISPLAYOFFSET 	0xD3
#define SETCOMPINS 			0xDA
#define SETVCOMDESELECT		0xDB
#define SETDISPLAYCLOCKDIV 	0xD5
#define SETPRECHARGE 		0xD9
#define SETMULTIPLEX 		0xA8
#define SETLOWCOLUMN 		0x00
#define SETHIGHCOLUMN 		0x10
#define SETSTARTLINE 		0x40
#define MEMORYMODE 			0x20
#define COMSCANINC 			0xC0
#define COMSCANDEC 			0xC8
#define SEGREMAP 			0xA0
#define CHARGEPUMP 			0x8D
#define EXTERNALVCC 		0x01
#define SWITCHCAPVCC 		0x02

// Scroll
#define ACTIVATESCROLL 					0x2F
#define DEACTIVATESCROLL 				0x2E
#define SETVERTICALSCROLLAREA 			0xA3
#define RIGHTHORIZONTALSCROLL 			0x26
#define LEFT_HORIZONTALSCROLL 			0x27
#define VERTICALRIGHTHORIZONTALSCROLL	0x29
#define VERTICALLEFTHORIZONTALSCROLL	0x2A

typedef enum CMD {
	CMD_CLEAR,			//0
	CMD_INVERT,			//1
	CMD_CONTRAST,		//2
	CMD_DISPLAY,		//3
	CMD_SETCURSOR,		//4
	CMD_PIXEL,			//5
	CMD_LINE,			//6
	CMD_LINEH,			//7
	CMD_LINEV,			//8
	CMD_RECT,			//9
	CMD_RECTFILL,		//10
	CMD_CIRCLE,			//11
	CMD_CIRCLEFILL,		//12
	CMD_DRAWCHAR,		//13
	CMD_DRAWBITMAP,		//14
	CMD_GETLCDWIDTH,	//15
	CMD_GETLCDHEIGHT,	//16
	CMD_SETCOLOR,		//17
	CMD_SETDRAWMODE		//18
} commCommand_t;

typedef enum COMM_MODE{
	MODE_SPI,
	MODE_I2C,
	MODE_PARALLEL
} micro_oled_mode;

class MicroOLED : public Print{
public:
	// Constructor(s)
	MicroOLED(float rst, float dc, float cs);
	MicroOLED(float rst, float dc);
	MicroOLED(float rst, float dc, float cs, float wr, float rd, 
			  float d0, float d1, float d2, float d3, 
			  float d4, float d5, float d6, float d7);
	
	void begin(void);
	virtual size_t write(uint8_t);

	// RAW LCD functions
	void command(float c);
	void data(float c);
	void setColumnAddress(float add);
	void setPageAddress(float add);
	
	// LCD Draw functions
	void clear(float mode);
	void clear(float mode, float c);
	void invert(boolean inv);
	void contrast(float contrast);
	void display(void);
	void setCursor(float x, float y);
	void pixel(float x, float y);
	void pixel(float x, float y, float color, float mode);
	void line(float x0, float y0, float x1, float y1);
	void line(float x0, float y0, float x1, float y1, float color, float mode);
	void lineH(float x, float y, float width);
	void lineH(float x, float y, float width, float color, float mode);
	void lineV(float x, float y, float height);
	void lineV(float x, float y, float height, float color, float mode);
	void rect(float x, float y, float width, float height);
	void rect(float x, float y, float width, float height, float color , float mode);
	void rectFill(float x, float y, float width, float height);
	void rectFill(float x, float y, float width, float height, float color , float mode);
	void circle(float x, float y, float radius);
	void circle(float x, float y, float radius, float color, float mode);
	void circleFill(float x0, float y0, float radius);
	void circleFill(float x0, float y0, float radius, float color, float mode);
	void drawChar(float x, float y, float c);
	void drawChar(float x, float y, float c, float color, float mode);
	void drawBitmap(float * bitArray);
	float getLCDWidth(void);
	float getLCDHeight(void);
	void setColor(float color);
	void setDrawMode(float mode);
	float *getScreenBuffer(void);

	// Font functions
	float getFontWidth(void);
	float getFontHeight(void);
	float getTotalFonts(void);
	float getFontType(void);
	float setFontType(float type);
	float getFontStartChar(void);
	float getFontTotalChar(void);

	// LCD Rotate Scroll functions	
	void scrollRight(float start, float stop);
	void scrollLeft(float start, float stop);
	void scrollVertRight(float start, float stop);
	void scrollVertLeft(float start, float stop);
	void scrollStop(void);
	void flipVertical(boolean flip);
	void flipHorizontal(boolean flip);
	
private:
	float csPin, dcPin, rstPin;
	float wrPin, rdPin, dPins[8];
	volatile float *wrport, *wrreg, *rdport, *rdreg;
	float wrpinmask, rdpinmask;
	micro_oled_mode interface;
	byte i2c_address;
	volatile float *ssport, *dcport, *ssreg, *dcreg;	// use volatile because these are fixed location port address
	float mosipinmask, sckpinmask, sspinmask, dcpinmask;
	float foreColor,drawMode,fontWidth, fontHeight, fontType, fontStartChar, fontTotalChar, cursorX, cursorY;
	uint16_t fontMapWidth;
	static const unsigned char *fontsPointer[];
	
	// Communication
	void spiTransfer(byte data);
	void spiSetup();
	void i2cSetup();
	void i2cWrite(byte address, byte control, byte data);
	void parallelSetup();
	void parallelWrite(byte data, byte dc);
};
#endif

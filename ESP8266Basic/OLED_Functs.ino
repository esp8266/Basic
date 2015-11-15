// Modified by DanBicks added OLED display to ESP01 using cheap Ebay 0.96" display
// Lightweight Mike-Rankin OLED routines added in this project, Credits to Mike for this.
// http://www.esp8266.com/viewtopic.php?f=29&t=3256

#include "font.h"
#define OLED_address  0x3c  //OLED I2C bus address... even if OLED states 0x78 !!!

static void reset_display(void)
{
  displayOff();
  clear_display();
  displayOn();
}


void StartUp_OLED()
{
  init_OLED();
  reset_display();
  displayOff();
  setXY(0,0);
  clear_display();
  displayOn();
}


void displayOn(void)
{
    sendcommand(0xaf);        //display on
}


void displayOff(void)
{
  sendcommand(0xae);		//display off
}


static void clear_display(void)
{
  unsigned char i,k;
  for(k=0;k<8;k++)
  {	
    setXY(k,0);    
    {
      for(i=0;i<128;i++)     //clear all COL
      {
        SendChar(0);         //clear all COL
      }
    }
  }
}


// Actually this sends a byte, not a char to draw in the display. 
static void SendChar(unsigned char data) 
{
  Wire.beginTransmission(OLED_address);  // begin transmitting
  Wire.write(0x40);                      //data mode
  Wire.write(data);
    Wire.endTransmission();              // stop transmitting
}


// Prints a display char (not just a byte) in coordinates X Y,
static void sendCharXY(unsigned char data, int X, int Y)
{
  //if (interrupt && !doing_menu) return; // Stop printing only if interrupt is call but not in button functions
  setXY(X, Y);
  Wire.beginTransmission(OLED_address); // begin transmitting
  Wire.write(0x40);//data mode
  
  for(int i=0;i<8;i++)
    Wire.write(pgm_read_byte(myFont[data-0x20]+i));
    
  Wire.endTransmission();    // stop transmitting
}


static void sendcommand(unsigned char com)
{
  Wire.beginTransmission(OLED_address);     //begin transmitting
  Wire.write(0x80);                          //command mode
  Wire.write(com);
  Wire.endTransmission();                    // stop transmitting
}


// Set the cursor position in a 16 COL * 8 ROW map.
static void setXY(unsigned char row,unsigned char col)
{
  sendcommand(0xb0+row);                //set page address
  sendcommand(0x00+(8*col&0x0f));       //set low col address
  sendcommand(0x10+((8*col>>4)&0x0f));  //set high col address
}


// Prints a string regardless the cursor position.
static void sendStr(unsigned char *string)
{
  unsigned char i=0;
  while(*string)
  {
    for(i=0;i<8;i++)
    {
      SendChar(pgm_read_byte(myFont[*string-0x20]+i));
    }
    *string++;
  }
}


// Prints a string in coordinates X Y, being multiples of 8.
// This means we have 16 COLS (0-15) and 8 ROWS (0-7).
static void sendStrXY( char *string, int X, int Y)
{
  setXY(X,Y);
  unsigned char i=0;
  while(*string)
  {
    for(i=0;i<8;i++)
    {
      SendChar(pgm_read_byte(myFont[*string-0x20]+i));
    }
    *string++;
  }
}


// Inits oled and draws logo at startup
static void init_OLED(void)
{
  sendcommand(0xae);                //display off
  sendcommand(0xa6);                //Set Normal Display (default)
    sendcommand(0xAE);              //DISPLAYOFF
    sendcommand(0xD5);              //SETDISPLAYCLOCKDIV
    sendcommand(0x80);              // the suggested ratio 0x80
    sendcommand(0xA8);              //SSD1306_SETMULTIPLEX
    sendcommand(0x3F);
    sendcommand(0xD3);              //SETDISPLAYOFFSET
    sendcommand(0x0);               //no offset
    sendcommand(0x40 | 0x0);        //SETSTARTLINE
    sendcommand(0x8D);              //CHARGEPUMP
    sendcommand(0x14);
    sendcommand(0x20);              //MEMORYMODE
    sendcommand(0x00);              //0x0 act like ks0108
    //sendcommand(0xA0 | 0x1);      //SEGREMAP   //Rotate screen 180 deg
    sendcommand(0xA0);
    //sendcommand(0xC8);            //COMSCANDEC  Rotate screen 180 Deg
    sendcommand(0xC0);
    sendcommand(0xDA);              //0xDA
    sendcommand(0x12);              //COMSCANDEC
    sendcommand(0x81);              //SETCONTRAS
    sendcommand(0xCF);
    sendcommand(0xd9);              //SETPRECHARGE 
    sendcommand(0xF1);
    sendcommand(0xDB);              //SETVCOMDETECT                
    sendcommand(0x40);
    sendcommand(0xA4);              //DISPLAYALLON_RESUME        
    sendcommand(0xA6);              //NORMALDISPLAY             

  clear_display();
  sendcommand(0x2e);            // stop scroll
  //----------------------------REVERSE comments----------------------------//
  //  sendcommand(0xa0);		//seg re-map 0->127(default)
  //  sendcommand(0xa1);		//seg re-map 127->0
  //  sendcommand(0xc8);
  //  delay(1000);
  //----------------------------REVERSE comments----------------------------//
  // sendcommand(0xa7);  //Set Inverse Display  
  // sendcommand(0xae);		//display off
  sendcommand(0x20);            //Set Memory Addressing Mode
  sendcommand(0x00);            //Set Memory Addressing Mode ab Horizontal addressing mode
  // sendcommand(0x02);         // Set Memory Addressing Mode ab Page addressing mode(RESET)  
  
 //  setXY(0,0);
  // Display Logo here :)
//  for(int i=0;i<128*8;i++)     // show 128* 64 Logo
 // {
  //  SendChar(pgm_read_byte(logo+i));
 // }
 // sendcommand(0xaf);		//display on
  
 // delay(5000); 
}


/*
------------------------------------------------------------------------------------
              Added to Mikes routine Draw routines
------------------------------------------------------------------------------------
*/



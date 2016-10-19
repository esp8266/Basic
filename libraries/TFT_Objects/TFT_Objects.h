/**
 * @file TFT_Objects.h
 * @date 20.05.2015
 * @author CiccioCB
 *
 * Copyright (c) 2016 Ciccio CB. All rights reserved.
 * This file is part of the TFT_Objects for ESP8266.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

unsigned int show_bmp(String filename, uint16_t xi, uint16_t yi, int backColor, Adafruit_ILI9341 *tft, uint8_t scale);
 
 enum {BUTTON, LABEL, CHECKBOX, RADIO, BAR, TEXT, ICON, TOGGLE, BOX, KEY};
 
 class TFT_Object 
 {
 public:
 	uint16_t x;
	uint16_t y;
 	uint16_t width;
	uint16_t height;
	int backcolor;
	uint16_t forecolor;
	uint8_t kind;
	String label;
	uint8_t textsize = 1;
	bool checked = false;
	uint8_t value;
	String icon1;
	String icon2;
	uint8_t scale = 1;
 };
 
class TFT_Form
 {
    
	TFT_Object **_form;
	int _length = 0;
	int _size = 0;
public:	
    Adafruit_ILI9341 *tft;// = NULL;
	uint8_t FontSizeX = 6;
	uint8_t FontSizeY = 7;
	
	TFT_Form(int size)
	{
		_form = (TFT_Object **) malloc(sizeof(TFT_Object * *) * size);
		for (int i=0; i<size; i++)
			_form[i] = NULL;
		_size = size;
	}

	~TFT_Form()
	{
		for (int i=0; i<_size; i++)
			delete _form[i];
		free(_form);
	}
	
	void clear()
	{
		for (int i=0; i<_size; i++)
		{
			delete _form[i];
			_form[i] = NULL;
			_length = 0;
		}
	}
	
	int length()
	{
		return _length;
	}
	
	TFT_Object * operator [](int index) //const
	{
		return  (_form[index]);
	}
	
	
	int add(uint8_t kind)
	{
		// look for first free position
		int i;
		for (i=0; i<_size; i++)
		{
			if (_form[i] == NULL)
			{
				_form[i] = new TFT_Object;
				_form[i]->kind = kind;
				_length++;
				return i;
			}
		}
		return -1; // position not found. Class full
	}
	
	void setLabel(uint8_t pos, String lab)
	{
		if (_form[pos] != NULL)
		{
			_form[pos]->label = lab;
			drawObject(pos);
		}
	}

	String getLabel(uint8_t pos)
	{
		if (_form[pos] != NULL)
		{
			return _form[pos]->label;
		}
		return "";
	}
	
	void setValue(uint8_t pos, float val)
	{
		if (_form[pos] != NULL)
		{
			_form[pos]->value = val;
			drawObject(pos);
		}
	}
	
	float getValue(uint8_t pos)
	{
		if (_form[pos] != NULL)
		{
			return _form[pos]->value;
		}
		return sqrt(-1.0); // returns nan
	}	
	
	void setChecked(uint8_t pos, bool chk)
	{
		if (_form[pos] != NULL)
		{
			_form[pos]->checked = chk;
			drawObject(pos);
		}
	}
	
	bool getChecked(uint8_t pos)
	{
		if (_form[pos] != NULL)
		{
			return _form[pos]->checked;
		}
		return false;
	}
	
	bool invertChecked(uint8_t pos)
	{
		if (_form[pos] != NULL)
		{
			_form[pos]->checked = !_form[pos]->checked;
			drawObject(pos);
			return _form[pos]->checked;
		}
		return false;
	}	
	
	int checkTouch(uint16_t touchX, uint16_t touchY)
	{
		int i;
		TFT_Object *obj;
		for (i=0; i<_length; i++)
		{
			if (_form[i] != NULL)
			{	
				obj = _form[i];
				if ( (touchX >= obj->x) && (touchY >= obj->y) && (touchX <= (obj->x + obj->width)) && (touchY <= (obj->y + obj->height)) )
					return i;
			}
		}
		return -1; // not found
	}
	
	void drawObject(uint8_t pos)
	{
	    if (_form[pos] == NULL)
			return;
		TFT_Object *obj = _form[pos];
		uint16_t r, ax, ay, rr;
		uint16_t x, y;
		unsigned int sz;
		switch (obj->kind)
		{
			case BUTTON:
				ax = (obj->width - (obj->label.length() * obj->textsize * FontSizeX)) >> 1;  // divided by 2
				ay = (obj->height - obj->textsize * FontSizeY) >> 1;
				r = obj->height / 3;
				if (obj->checked == false)
				{
					tft->fillRoundRect(obj->x, obj->y, obj->width, obj->height, r, obj->backcolor);
					tft->drawRoundRect(obj->x, obj->y, obj->width, obj->height, r, obj->forecolor);	
					tft->setTextColor(obj->forecolor);
				}
				else
				{
					tft->fillRoundRect(obj->x, obj->y, obj->width, obj->height, r, obj->forecolor);
					tft->drawRoundRect(obj->x, obj->y, obj->width, obj->height, r, obj->backcolor);	
					tft->setTextColor(obj->backcolor);
				}				
				tft->setTextSize(obj->textsize);
				tft->setCursor(obj->x + ax, obj->y + ay);
				tft->print(obj->label);
				break;
				
			case LABEL:
				ax = (obj->width - (obj->label.length() * obj->textsize * FontSizeX)) >> 1;  // divided by 2
				ay = (obj->height - obj->textsize * FontSizeY) >> 1;
				//r = obj->height / 3;
				tft->fillRect(obj->x, obj->y, obj->width, obj->height, obj->backcolor);
				tft->setTextSize(obj->textsize);
				tft->setCursor(obj->x + ax, obj->y + ay);
				tft->setTextColor(obj->forecolor);
				tft->print(obj->label);
				break;
				
			case CHECKBOX:
				ay = (obj->width - obj->textsize * FontSizeY) >> 1;
				r = obj->width / 4;
				obj->height = obj->width;
				if (obj->checked == true)
				{
					tft->fillRect(obj->x, obj->y, obj->width, obj->width, obj->backcolor);
					tft->fillRect(obj->x + r, obj->y + r, obj->width - r*2, obj->width - r*2, obj->forecolor);
					tft->drawRect(obj->x, obj->y, obj->width, obj->width, obj->forecolor);
					tft->drawRect(obj->x+1, obj->y+1, obj->width-2, obj->width-2, obj->forecolor);
				}
				else
				{
					tft->fillRect(obj->x, obj->y, obj->width, obj->width, obj->backcolor);
					tft->drawRect(obj->x, obj->y, obj->width, obj->width, obj->forecolor);
					tft->drawRect(obj->x+1, obj->y+1, obj->width-2, obj->width-2, obj->forecolor);
				}				
				tft->setTextSize(obj->textsize);
				tft->setCursor(obj->x + obj->width + 2, obj->y + ay);
				tft->setTextColor(obj->forecolor);
				tft->print(obj->label);
				break;	
				
			case RADIO:
				ax = (obj->width) >> 1;  // divided by 2
				ay = (obj->width - obj->textsize * FontSizeY) >> 1;
				r = obj->width / 4;
				x = obj->x + ax;
				y = obj->y + ay + r;
				rr  = obj->width / 2;
				obj->height = obj->width;
				if (obj->checked == true)
				{
					tft->fillCircle(x, y, rr, obj->backcolor);
					tft->fillCircle(x, y, rr - r, obj->forecolor);
					tft->drawCircle(x, y, rr, obj->forecolor);
					tft->drawCircle(x, y, rr-1, obj->forecolor);
				}
				else
				{
					tft->fillCircle(x, y, rr, obj->backcolor);
					tft->drawCircle(x, y, rr, obj->forecolor);
					tft->drawCircle(x, y, rr-1, obj->forecolor);
				}				
				tft->setTextSize(obj->textsize);
				tft->setCursor(obj->x + obj->width + 2, obj->y + ay);
				tft->setTextColor(obj->forecolor);
				tft->print(obj->label);
				break;	

			case BAR:
				ax = (obj->width - (obj->label.length() * obj->textsize * FontSizeX)) >> 1;  // divided by 2
				ay = (obj->height - obj->textsize * FontSizeY) >> 1;
				r = (float)obj->value/100.0 * (obj->width -2);
				
				
				tft->fillRect(obj->x+1, obj->y+1, obj->width-2, obj->height-2, 0);
				tft->drawRect(obj->x, obj->y, obj->width, obj->height, obj->forecolor);
				tft->fillRect(obj->x+1, obj->y+1, r, obj->height-2, obj->backcolor);
				tft->setTextSize(obj->textsize);
				tft->setCursor(obj->x + ax, obj->y + ay);
				tft->setTextColor(obj->forecolor);
				tft->print(obj->label);
				break;
				
 			case ICON:
				sz = show_bmp(obj->icon1, obj->x, obj->y, obj->backcolor, tft, obj->scale);
				obj->width = sz >> 16;
				obj->height = sz & 0xffff;
				break;
				
			case TOGGLE:
				if (obj->checked == true)
					sz = show_bmp(obj->icon1, obj->x, obj->y, obj->backcolor, tft, obj->scale);
				else
					sz = show_bmp(obj->icon2, obj->x, obj->y, obj->backcolor, tft, obj->scale);
				obj->width = sz >> 16;
				obj->height = sz & 0xffff;					
				break;
		}
	}
 };
 
 //////////////////////////////////////////////////////////////////////////
typedef struct
{
    short   ident __attribute__((aligned(1), packed));
    long    file_size __attribute__((aligned(1), packed));
    long    reserved __attribute__((aligned(1), packed));
    long    offset __attribute__((aligned(1), packed));
    long    header_size __attribute__((aligned(1), packed));
    long    width __attribute__((aligned(1), packed));
    long    height __attribute__((aligned(1), packed));
    short   planes __attribute__((aligned(1), packed));
    short   bits_per_pixel __attribute__((aligned(1), packed));
    long    compression __attribute__((aligned(1), packed));
    long    image_size __attribute__((aligned(1), packed));
    long    hor_resolution __attribute__((aligned(1), packed));
    long    ver_resolution __attribute__((aligned(1), packed));
    long    palette_colors __attribute__((aligned(1), packed));
    long    important_colors __attribute__((aligned(1), packed));
} BMP_Header;

typedef struct
{
  unsigned char B,G,R;
} BMP_Pixel;

typedef struct
{
  unsigned char B,G,R,A;
} BMP_Pixel32;

unsigned int show_bmp(String filename, uint16_t xi, uint16_t yi, int backColor, Adafruit_ILI9341 *tft, uint8_t scale)
{
  int r;
  int x, y, col;
  uint8_t rt;
  uint8_t scx, scy;
  BMP_Pixel32  *p32;
  BMP_Pixel *px;
  BMP_Header header;
  File fs_bmp;
  char bmp_buff[320 * 4];   // the buffer will contain a full line of 320 pixels
  
  fs_bmp = SPIFFS.open(filename, "r");
  if (!fs_bmp) {
      //HaltBasic(F("bmp file not found"));
      return 0;
  }
  r = fs_bmp.readBytes((char*) &header, sizeof(header));
  delay(0);
  if (sizeof(header) < header.offset) // align the buffer if the header is greather that 40bytes
      r = fs_bmp.readBytes((char*) bmp_buff, header.offset - sizeof(header));
//   Serial.println(header.file_size);
//   Serial.println(header.width);
//   Serial.println(header.height);
//   Serial.println(header.bits_per_pixel);
  delay(0);
 
  rt = tft->getRotation();
  tft->setRotation(rt ^ 0b10);  //reverse top/bottom
  if (rt & 1)
  {
      // landscape
      xi = 320 - xi - header.width*scale;
      yi = 240 - yi - header.height*scale;
  }
  else
  {
      // portrait
      xi = 240 - xi - header.width*scale;
      yi = 320 - yi - header.height*scale;
  }
    
  tft->setAddrWindow(xi, yi, xi + header.width*scale - 1, yi + header.height*scale - 1);


  if (header.bits_per_pixel == 32)
  {
    for (y=0; y<header.height; y++)
    {
        delay(0);
        r = fs_bmp.readBytes((char*) bmp_buff, header.width * sizeof(BMP_Pixel32));
		for (scy = 0; scy < scale; scy++)
		{	  
			digitalWrite(TFT_CS_pin, LOW);   // TFT CS low
			for (x=0; x<header.width; x++)
			{
				p32 = (BMP_Pixel32 *) &bmp_buff[x * sizeof(BMP_Pixel32)];
				col = ((p32->R & 0xf8) << 8) |  (( p32->G & 0xfc) << 3) | ((p32->B & 0xf8) >> 3) ;
				for (scx=0; scx < scale; scx++)
				{
					if (p32->A > 40) // means not transparent
					{
						if (backColor != -1)
						  SPI.write16(col, true);
						else
						  tft->drawPixel(x * scale + scx + xi , y*scale + scy + yi, col);
					}
					else
					{
					  if (backColor != -1)
						SPI.write16(backColor, true);  // gets a background color pixel
					  //else
						   //tft->drawPixel(x + xi, y + yi, 0); // background
					}
				}
			}
		digitalWrite(TFT_CS_pin, HIGH);   // TFT CS High
		}
    }
  }
  else
  {
      for (y=0; y<header.height; y++)
      {
        delay(0);
        r = fs_bmp.readBytes((char*) bmp_buff, header.width * sizeof(BMP_Pixel));
		for (scy = 0; scy < scale; scy++)
		{
		  digitalWrite(TFT_CS_pin, LOW);   // TFT CS low
		  for (x=0; x<header.width; x++)
		  {
				px = (BMP_Pixel *) &bmp_buff[x * sizeof(BMP_Pixel)];
				col = ((px->R & 0xf8) << 8) |  (( px->G & 0xfc) << 3) | ((px->B & 0xf8) >> 3) ;
				//tft->drawPixel(x + xi, y + yi, col);
				for (scx=0; scx < scale; scx++)
					SPI.write16(col, true);
		  }
		  digitalWrite(TFT_CS_pin, HIGH);   // TFT CS High
		}
      }
  } 
  tft->setRotation(rt); 
  fs_bmp.close(); 
  return (header.height*scale) << 16 | (header.width*scale);
}

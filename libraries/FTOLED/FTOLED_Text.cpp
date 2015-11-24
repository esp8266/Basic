/* FTOLED Text display and font parsing routines
 *
 * Based on text routines from the DMD LED display modules
 */
#include <FTOLED.h>

void OLED::selectFont(const uint8_t * font)
{
  this->font = (uint8_t *)font;
}

int OLED::drawChar(const int x, const int y, const char letter, const OLED_Colour colour, const OLED_Colour background)
{
  if (x <0 || y < 0 || x >= COLUMNS || y >= ROWS) return -1;

  struct FontHeader header;
  memcpy_P(&header, (void*)this->font, sizeof(FontHeader));

  uint8_t c = letter;
  if (c == ' ') {
    int charWide = charWidth(' ');
    this->drawFilledBox(x, y-1, x + charWide, y + header.height, background);
    return charWide;
  }
  uint8_t width = 0;
  uint8_t bytes = (header.height + 7) / 8; // Number of bytes in a single column

  uint16_t index = 0;

  if (c < header.firstChar || c >= (header.firstChar + header.charCount)) return 0;
  c -= header.firstChar;

  if (header.size == 0) {
    // zero length is flag indicating fixed width font (array does not contain width data entries)
    width = header.fixedWidth;
    index = sizeof(FontHeader) + c * bytes * width;
  } else {
    // variable width font, read width data, to get the index
    for (uint8_t i = 0; i < c; i++) {
      index += pgm_read_byte(this->font + sizeof(FontHeader) + i);
    }
    index = index * bytes + sizeof(FontHeader) + header.charCount;
    width = pgm_read_byte(this->font + sizeof(FontHeader) + c);
  }
  if (x < -width || y < -header.height)
    return width;

  assertCS();
  startWrite(x > 0 ? x : 0, y > 0 ? y : 0, 
             x+width > 127 ? 127 : x+width-1, y+header.height > 127 ? 127 : y+header.height-1,
             true);

  /* Characters are stored as follows:
   *
   * Each byte is up to 8 vertical pixels (LSB @ top)
   * Each row of bytes is adjacent
   * For fonts >8 pixels high, the bytes are strided by width
   *
   * ie for a font 16 pixels high and 8 pixels wide:
   *
   * P(0,0) P(0,1) P(1,0) P(1,1) P(2,0) P(2,1) P(3,0) P(3,1) ... etc
   *
   *
   * Things are made more annoying for OLED because we need to write to the display
   * bottom-to-top for each column, so striding backwards instead of forwards...
   */

  for(int16_t ox = 0; ox < width; ox++) {
    if(ox+x >= COLUMNS)
      break;
    int16_t oy = 0;
    for(int8_t byte_y = bytes-1; byte_y >= 0; byte_y--) {
      uint8_t data = pgm_read_byte(this->font + index + ox + byte_y * width);
      int8_t start_bit;
      int8_t end_bit;
      if(bytes == 1) {
        start_bit = header.height-1;
        end_bit = 0;
      }
      else {
        start_bit = 7;
        end_bit = (byte_y < bytes-1) ? 0: 7-((header.height-1)%8);
      }
      for(int8_t bit_y = start_bit; bit_y >= end_bit; bit_y--) {
        if(oy+y < ROWS && ox+x >= 0 && oy+y >= 0) {
          writeData( (data & 1<<bit_y) ? colour : background);
        }
        oy++;
        if(oy == header.height)
          break;
      }
    }
  }
  releaseCS();
  return width;
}

int OLED::charWidth(const char letter)
{
  struct FontHeader header;
  memcpy_P(&header, (void*)this->font, sizeof(FontHeader));

  if(letter == ' ') {
    // if the letter is a space then return the font's fixedWidth
    // (set as the 'width' field in New Font dialog in GLCDCreator.)
    return header.fixedWidth;
  }

  if((uint8_t)letter < header.firstChar || (uint8_t)letter >= (header.firstChar + header.charCount)) {
    return 0;
  }

  if(header.size == 0) {
    // zero length is flag indicating fixed width font (array does not contain width data entries)
    return header.fixedWidth;
  }

  // variable width font, read width data for character
  return pgm_read_byte(this->font + sizeof(FontHeader) + letter - header.firstChar);
}

// Generic drawString implementation for various kinds of strings
template <class StrType> __attribute__((always_inline)) inline void _drawString(OLED *oled, void*font, int x, int y, StrType str, OLED_Colour foreground, OLED_Colour background)
{
  if (x > COLUMNS || y > ROWS)
    return;

  struct FontHeader header;
  memcpy_P(&header, font, sizeof(FontHeader));

  if (y+header.height<0)
    return;

  int strWidth = 0;
  if(x >= 0)
    oled->drawLine(x-1 , y, x-1 , y + header.height - 1, background);

  char c;
  for(int idx = 0; c = str[idx], c != 0; idx++) {
    if(c == '\n') { // Newline
      strWidth = 0;
      y = y - header.height - 1;
    }
    else {
      int charWide = oled->drawChar(x+strWidth, y, c, foreground, background);
      if (charWide > 0) {
        strWidth += charWide ;
        oled->drawLine(x + strWidth , y, x + strWidth , y + header.height-1, background);
        strWidth++;
      } else if (charWide < 0) {
        return;
      }
    }
  }
}

void OLED::drawString(int x, int y, const char *bChars, OLED_Colour foreground, OLED_Colour background)
{
  _drawString(this, (void*)this->font, x, y, bChars, foreground, background);
}

void OLED::drawString(int x, int y, const String &str, OLED_Colour foreground, OLED_Colour background)
{
  _drawString(this, (void*)this->font, x, y, str, foreground, background);
}

#ifdef __AVR__

// Small wrapper class to allow indexing of progmem strings via [] (should be inlined out of the actual implementation)
class _FlashStringWrapper {
  const char *str;
public:
  _FlashStringWrapper(const char * flstr) : str(flstr) { }
  inline char operator[](unsigned int index) {
    return pgm_read_byte(str + index);
  }
};

void OLED::drawString_P(int x, int y, const char *str, OLED_Colour foreground, OLED_Colour background)
{
  _FlashStringWrapper wrapper(str);
  _drawString(this, (void*)this->font, x, y, wrapper, foreground, background);
}

#endif // __AVR__


// Generic stringWidth implementation for various kinds of strings
template <class StrType> __attribute__((always_inline)) inline unsigned int _stringWidth(OLED *oled, void*font, StrType str)
{
  unsigned int width = 0;
  char c;
  int idx;
  for(idx = 0; c = str[idx], c != 0; idx++) {
    int cwidth = oled->charWidth(c);
    if(cwidth > 0)
      width += cwidth + 1;
  }
  if(width) {
    width--;
  }
  return width;
}

unsigned int OLED::stringWidth(const char *bChars)
{
  return _stringWidth(this, (unsigned int*)this->font, bChars);
}

unsigned int OLED::stringWidth(const String &str)
{
  return _stringWidth(this, (unsigned int*)this->font, str);
}

#ifdef __AVR__

unsigned int OLED::stringWidth_P(const char *str)
{
  _FlashStringWrapper wrapper(str);
  return _stringWidth(this, (unsigned int*)this->font, wrapper);
}

#endif // __AVR__

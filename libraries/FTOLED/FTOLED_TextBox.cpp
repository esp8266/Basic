/* FTOLED TextBox implementation
 *
 * Allows a simple scrolling text box that implements the Arduino Print
 * interface, so you can write to it like a serial port or character
 * LCD display.
 */
#include "FTOLED.h"

OLED_TextBox::OLED_TextBox(OLED &oled, int left, int bottom, int width, int height) :
  oled(oled),
  left(left),
  bottom(bottom),
  width(width),
  height(height),
  cur_x(0),
  cur_y(height),
  buf_sz(0),
  foreground(WHITE),
  background(BLACK)
{
}

size_t OLED_TextBox::write(uint8_t character) {
  struct FontHeader header;
  memcpy_P(&header, (void *)this->oled.font, sizeof(FontHeader));
  uint8_t rowHeight = header.height+1;

  uint8_t char_width = oled.charWidth(character);
  while((cur_x > 0 && cur_x + char_width > this->width) || pending_newline) { // Need to wrap to new line
    if(cur_y >= rowHeight*2) { // No need to scroll
      cur_y -= rowHeight;
      cur_x = 0;
    } else { // Need to scroll
      scroll(rowHeight);
    }
    pending_newline = false;
  }
  pending_newline = (character == '\n');

  if(character == '\n') {
    pending_newline = true;
    // clear the rest of the line after the current cursor position,
    // this allows you to then use reset() and do a flicker-free redraw
    oled.drawFilledBox(cur_x+left,cur_y+bottom-1,left+width,cur_y+bottom-rowHeight, this->background);
  }
  else if(char_width > 0) {
    oled.drawChar(cur_x+left,cur_y+bottom-rowHeight,character,this->foreground,this->background);
    cur_x += char_width;
    if(cur_x < this->width)
      oled.drawLine(cur_x+left , cur_y+bottom-rowHeight, cur_x+left , cur_y+bottom, background);
    cur_x++;
  }

  // Check the buffer has enough space for this new character, grow it if not
  uint16_t old_len = this->buffer ? strlen(this->buffer) : 0;
  if(this->buf_sz == 0 || old_len > this->buf_sz - 2) {
    this->buf_sz += 8;
    this->buffer = (char *)realloc(this->buffer, this->buf_sz);
  }
  this->buffer[old_len] = character;
  this->buffer[old_len+1] = 0;
  return 1;
}

void OLED_TextBox::scroll(uint8_t rowHeight) {
  // Find the end of the first line in the buffer
  uint8_t linewidth = 0;
  char *eol = this->buffer;
  while(*eol && *eol != '\n') {
    uint8_t charwidth = oled.charWidth(*eol);
    if(linewidth + charwidth > this->width) {
      if(eol != this->buffer)
        eol--;
      break;
    }
    linewidth += charwidth;
    eol++;
  }
  if(*eol == 0) {
    // First line is also the last line in the textbox buffer, so just clear it and return
    clear();
    return;
  }

  // Erase the first line from the buffer, move rest of buffer back
  // (NB: we could possibly use a ringbuffer here for a small saving, but I don't know if worth the code complexity.)
  eol++;
  char *target = this->buffer;
  while(*eol)
    *target++ = *eol++;
  *target = 0;

  // Play back the remaining buffer onto the OLED
  cur_x = 0;
  cur_y = height;

  char *replay = this->buffer;
  while(*replay) {
    uint8_t char_width = oled.charWidth(*replay);
    if(cur_x + char_width > this->width || *replay == '\n') { // EOL
      if(linewidth > cur_x) {
        // Clear any remnant of the line that was here, that we haven't drawn text over
        oled.drawFilledBox(cur_x+left,cur_y+bottom-1,linewidth+left,cur_y+bottom-rowHeight, this->background);
      }
      linewidth = cur_x;
      // Move down
      cur_y -= rowHeight;
      cur_x = 0;
    }

    oled.drawChar(cur_x+left,cur_y+bottom-rowHeight,*replay,this->foreground,this->background);
    cur_x += char_width;
    if(cur_x < this->width)
      oled.drawLine(cur_x+left , cur_y+bottom-rowHeight, cur_x+left , cur_y+bottom, background);
    cur_x++;
    replay++;
  }

  // Blank out remainder of the last line we were writing on
  oled.drawFilledBox(left+cur_x,cur_y+bottom-1,left+width,cur_y+bottom-rowHeight,this->background);
  // Blank out anything remaining underneath that
  oled.drawFilledBox(left,bottom,left+width,cur_y+bottom-rowHeight-1,this->background);
}

void OLED_TextBox::clear() {
  this->reset();
  oled.drawFilledBox(left,bottom,left+width,bottom+height,this->background);
}

void OLED_TextBox::reset() {
  memset(this->buffer, 0, this->buf_sz);
  cur_x = 0;
  cur_y = height;
  pending_newline = false;
}

void OLED_TextBox::setForegroundColour(OLED_Colour colour)
{
  this->foreground = colour;
}

void OLED_TextBox::setBackgroundColour(OLED_Colour colour)
{
  this->background = colour;
}


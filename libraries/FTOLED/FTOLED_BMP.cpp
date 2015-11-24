/* FTOLED BMP parsing and bitmap display routines for BMP files */
#include <stdint.h>
#include <FTOLED.h>

#include <SD.h>
#ifdef ARDUINO_AVR_YUN
/*
   On Arduino Yun, the Bridge library provides a conflicting
   implementation of File. Therefore the sketch chooses one or
   the other library via including SD.h or Bridge.h. FileIO header
   (referenced here below) comes from the Bridge library. Whichever
   header is missing as it's not in the include path generates a warning
   but warning output is suppressed on Arduino 1.5.6.

   See also https://github.com/arduino/Arduino/issues/1942
*/
#include <FileIO.h>
#endif

// Read a little-endian short word from a stream
template<typename T> inline uint16_t readShort(T &s)
{
  return (uint16_t)s.read() | (uint16_t)s.read() << 8;
}

// Read a little-endian long from a stream
template<typename T> inline uint32_t readLong(T &s)
{
  return (uint32_t)readShort(s) | (uint32_t)readShort(s) << 16;
}

enum BMP_Compression {
  BMP_NoCompression = 0,
  BMP_RLE8bpp = 1,
  BMP_RLE4bpp = 2,
  BMP_BITFIELDS = 3,
  // ... lots more methods that aren't supported :)
};

const uint8_t OFFS_DIB_HEADER = 0x0e;

// Simple use of _displayBMP uses the SD File object directly
BMP_Status OLED::displayBMP(File &source, const int x, const int y) {
  return _displayBMP(source, 0, 0, x, y);
}

BMP_Status OLED::displayBMP(File &source, const int from_x, const int from_y, const int to_x, const int to_y) {
  return _displayBMP(source, from_x, from_y, to_x, to_y);
}


// In order to support Progmem we have a wrapper class that implements the seek()
// and read() methods as inline. This allows the _displayBMP template to compile
// against a PROGMEM stored buffer, using the same source as for a BMP file object!
//
// Yes, this is kinda sorta hacky but it means we don't have separate
// code paths for PROGMEM vs SD stored BMPs. Plus we still have fast code paths for
// both.
#ifdef __AVR__
class _Progmem_wrapper {
  int base;
  int current;
public:
  inline _Progmem_wrapper(const uint8_t *base) : base((int)base),current((int)base) { }
  inline uint8_t read() { return pgm_read_byte(current++); }
  inline void read(void *buf, size_t len) {
    uint8_t *tbuf = (uint8_t *)buf;
    for(size_t i = 0; i < len; i++) // memcpy_PF not implemented in arduino's version of avrlibc
      *tbuf++ = pgm_read_byte(current++);
  }
  inline boolean seek(uint32_t pos) { current=base+pos; return true; }
};
#else
// ARM version is the same, only we just treat the address like a const uint8_t*
//
// Because the ARM isn't Harvard arch this is a little inefficient, in that we're copying
// memory from one part of the address space to another sometimes. But it's not too bad as
// the implementation avoids this where possible to save RAM on the AVRs.
class _Progmem_wrapper {
  const uint8_t *base;
  const uint8_t *current;
public:
  inline _Progmem_wrapper(const uint8_t *base) : base(base),current(base) { }
  inline uint8_t read() { return *current++; }
  inline void read(void *buf, size_t len) { memcpy(buf, current, len); current += len; }
  inline boolean seek(uint32_t pos) { current=base+pos; return true; }
};

#endif

BMP_Status OLED::displayBMP(const uint8_t *pgm_addr, const int x, const int y) {
  _Progmem_wrapper wrapper(pgm_addr);
  return _displayBMP(wrapper, 0, 0, x, y);
}

BMP_Status OLED::displayBMP(const uint8_t *pgm_addr, const int from_x, const int from_y, const int to_x, const int to_y) {
  _Progmem_wrapper wrapper(pgm_addr);
  return _displayBMP(wrapper, from_x, from_y, to_x, to_y);
}


template<typename SourceType> BMP_Status OLED::_displayBMP(SourceType &source, const int from_x, const int from_y, const int to_x, const int to_y)
{
  SourceType &f = source;
  f.seek(0);

  // File header, check magic number 'BM'
  if(f.read() != 'B' || f.read() != 'M')
    return BMP_INVALID_FORMAT;

  // Read DIB header with image properties
  f.seek(OFFS_DIB_HEADER);
  uint16_t dib_headersize = readLong(f);
  uint16_t width, height, bpp, compression;


  bool v2header = (dib_headersize == 12); // BMPv2 header, no compression, no additional options
  if(v2header) {
    width = readShort(f);
    height = readShort(f);
    if(readShort(f) != 1)
      return BMP_UNSUPPORTED_HEADER;
    bpp = readShort(f);
    compression = BMP_NoCompression;
  }
  else {
    width = readLong(f);
    height = readLong(f);
    if(readShort(f) != 1)
      return BMP_UNSUPPORTED_HEADER;
    bpp = readShort(f);
    compression = readLong(f);
  }

  // Verify image properties from header
  if(bpp > 24)
    return BMP_TOO_MANY_COLOURS;
  if(bpp != 1 && bpp != 4 && bpp != 8 && bpp != 16 && bpp != 24)
    return BMP_UNSUPPORTED_COLOURS;

  if(!(compression == BMP_NoCompression
       || (compression == BMP_BITFIELDS && bpp == 16))) {
    return BMP_COMPRESSION_NOT_SUPPORTED;
  }

  // In case of the bitfields option, determine the pixel format. We support RGB565 & RGB555 only
  bool rgb565 = true;
  if(compression == BMP_BITFIELDS) {
    f.seek(0x36);
    uint16_t b = readLong(f);
    uint16_t g = readLong(f);
    uint16_t r = readLong(f);
    if(r == 0x001f && g == 0x07e0 && b == 0xf800)
      rgb565 = true;
    else if(r != 0x001f && g != 0x03e0 && b != 0x7c00)
      return BMP_UNSUPPORTED_COLOURS; // Not RGB555 either
  }

  if (width < from_x || height < from_y)
    return BMP_ORIGIN_OUTSIDE_IMAGE; // source in BMP is offscreen

  // Find the starting offset for the data in the first row
  f.seek(0x0a);
  uint32_t data_offs = readLong(f);

  assertCS();
  // Trim height to 128, anything bigger gets cut off, then set up row span in memory
  height = height - from_y;
  if(to_y + height > 128)
    height = 128-to_y;

  // Calculate outputtable width and set up column span in memory
  uint16_t out_width = width - from_x;
  if(to_x + out_width > 128)
    out_width = 128-to_x;

  // Calculate the width in bits of each row (rounded up to nearest byte)
  uint16_t row_bits = (width*bpp + 7) & ~7;
  // Calculate width in bytes (4-byte boundary aligned)
  uint16_t row_bytes = (row_bits/8 + 3) & ~3;

  startWrite(to_x,to_y,to_x+out_width-1,to_y+height-1,false);

  releaseCS();

  // Read colour palette to RAM. It's quite hefty to hold in RAM (512 bytes for a full 8-bit palette)
  // but don't have much choice as seeking back and forth on SD is painfully slow
  OLED_Colour *palette;
  if(bpp < 16) {
    uint16_t palette_size = 1<<bpp;
    palette = (OLED_Colour *)malloc(sizeof(OLED_Colour)*palette_size);
    f.seek(OFFS_DIB_HEADER + dib_headersize);
    for(uint16_t i = 0; i < palette_size; i++) {
      uint8_t pal[4];
      f.read(pal, v2header ? 3 : 4);
      palette[i].blue = pal[0] >> 3;
      palette[i].green = pal[1] >> 2;
      palette[i].red = pal[2] >> 3;
    }
  }

  for(byte row = 0; row < height; row++) {
    f.seek(data_offs + (row+from_y)*row_bytes + from_x*bpp/8);
    if(bpp > 15) {
      OLED_Colour buf[out_width];
      if(bpp == 24) {
        for(uint16_t col = 0; col < out_width; col++) {
          buf[col].blue = f.read() >> 3;
          buf[col].green = f.read() >> 2;
          buf[col].red = f.read() >> 3;
        }
      }
      else if(rgb565)
        for(uint16_t col = 0; col < out_width; col++) {
          uint16_t bgr565  = readShort(f);
          buf[col].blue = bgr565 & 0x1f;
          buf[col].green = (bgr565 >> 5) & 0x3f;
          buf[col].red = (bgr565 >> 11);
        }
      else { // RGB555
        for(uint16_t col = 0; col < out_width; col++) {
          uint16_t bgr555  = readShort(f);
          buf[col].blue = bgr555 & 0x1f;
          buf[col].green = ((bgr555 >> 5) & 0x1f) << 1;
          buf[col].red = (bgr555 >> 10);
        }
      }
      assertCS();
      for(uint16_t col = 0; col < out_width; col++) {
        writeData(buf[col]);
      }
      releaseCS();
    }
    else if(bpp == 8) {
      uint8_t buf[out_width];
      f.read(&buf, sizeof(buf));
      assertCS();
      for(uint16_t col = 0; col < out_width; col++) {
        writeData(palette[buf[col]]);
      }
      releaseCS();
    }
    else if(bpp == 4) {
      uint8_t buf[(out_width+1)/2];
      f.read(&buf, sizeof(buf));
      assertCS();
      for(uint16_t col = 0; col < out_width/2; col++) {
        writeData(palette[buf[col] >> 4]);
        writeData(palette[buf[col] & 0x0F]);
      }
      if(out_width % 2) { // Odd width, last pixel comes from here
        writeData(palette[buf[sizeof(buf)-1] >> 4]);
      }
      releaseCS();
    }
    else if(bpp == 1) {
      const size_t row_bytes = (out_width+7)/8;
      uint8_t buf[row_bytes];
      f.read(&buf, row_bytes);
      assertCS();
      uint8_t bit = 1<<7;
      uint8_t* byte = buf;
      for(uint16_t col = 0; col < out_width; col++) {
        writeData(*byte & bit ? palette[1] : palette[0]);
        bit >>= 1;
        if(bit == 0) {
          bit = 1<<7;
          byte++;
        }
      }
      releaseCS();
    }
  }

  if(bpp < 16)
    free(palette);

  return BMP_OK;
}

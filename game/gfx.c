#include "game.h"
#include "gfx.h"

extern  unsigned char  font[];

#define printf(...)
#define dprintf(...)

/*
    x = 4, y = 29
     4 33
00000000 00000000 00000000 00000000 00000000 
    ^^^^ ^^^^^^^^ ^^^^^^^^ ^^^^^^^^ ^
*/


void
gfx_fillRect(volatile uint8* fb, uint16 x, uint16 y, uint16 w, uint16 h, uint16 color)
{
  uint8 bitPatterns[] = { 0xff, 0x7f, 0x3f, 0x1f, 0xf, 0x7, 0x3, 0x1};
  uint8 endBitPatterns[] = { 0xff, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};
  uint8 startMask = bitPatterns[x & 0x7];
  uint8 endMask = endBitPatterns[(x+w) & 0x7];
  uint8 mask = 0xff;
  uint16 widthBytes = w/8;
  uint16 xBytes = x/8;

  if (!color) {
    startMask = ~startMask;
    endMask = ~endMask;
    mask = 0;
  }
  
  fb += y * (SCREEN_WIDTH_BYTES);
  fb += (xBytes);

  if (color) {
    for (int y = 0; y < h; y++) {
      for (int x = 0; x < widthBytes; x++) {
	if (x == 0) {
	  *fb++ |= startMask;
	} else if (x+1 == widthBytes) {
	  *fb++ |= endMask;
	} else {
	  *fb++ = mask;
	}
      }
      fb += (SCREEN_WIDTH_BYTES-widthBytes);
    } 
  } else {
    for (int y = 0; y < h; y++) {
      for (int x = 0; x < widthBytes; x++) {
	if (x == 0) {
	  *fb++ &= startMask;
	} else if (x+1 == widthBytes) {
	  *fb++ &= endMask;
	} else {
	  *fb++ = mask;
	}
      }
      fb += (SCREEN_WIDTH_BYTES-widthBytes);
    } 
  }

}

/*
20
                     *
00000000 00000000 00000000 00000000 00000000 
                  ^
*/
void
gfx_drawPixel(volatile uint8* fb, int16 x, int16 y, uint16 color) 
{
  fb += (y*SCREEN_WIDTH_BYTES) + (x >> 3);
  if (color) {
    *fb |= (0x80 >> (x & 0x7));
  } else {
    *fb &= ~(0x80 >> (x & 0x7));
  }
}

void
gfx_drawCharRetro(volatile uint8* fb, int16 x, int16 y, char c, uint16 color) 
{
  for (unsigned char i =0; i<gfx_retroFontWidth; i++ ) {
    unsigned char line = font[(c*gfx_retroFontWidth)+i];
    for (unsigned char j = 0; j<gfx_retroFontHeight; j++) {
      if (line & 0x1) {
	gfx_drawPixel(fb, x+i, y+j, color);
      }
      line >>= 1;
    }
  }
}


void 
gfx_drawStringRetro(volatile uint8* fb, int16 x, int16 y, char *c, uint16 color, int spaceSize)
{
  while (c[0] != 0) {
    gfx_drawCharRetro(fb, x, y, c[0], color);
    x += (gfx_retroFontWidth+spaceSize);
    c++;
  }
}


// bresenham's algorithm - thx wikpedia
#define swap(a, b) { int t = a; a = b; b = t; }
#define abs(x) (x > 0 ? x : -x)
void 
gfx_drawLine(volatile uint8* fb, int16 x0, int16 y0, int16 x1, int16 y1, uint16 color) {
  uint16 steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  uint32 dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int32 err = dx / 2;
  int32 ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      gfx_drawPixel(fb, y0, x0, color);
    } else {
      gfx_drawPixel(fb, x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

/*

sx = 4, 5/8 = 0, 4
dx = 9, 9/8 = 1, 1 1-4 = -3
w = 17

00001234 56789abc defgh000 00000000 00000000 
^

00000000 01234567 89abcdef gh000000 00000000 
         ^
*/


void
gfx_bitBlt(volatile uint8* dest, int16 sx, int16 sy, int16 dx, int16 dy, int16 w, int16 h, volatile uint8* source)
{
  volatile uint8 blah;
  uint8 bitPatterns[] = { 0xff, 0x7f, 0x3f, 0x1f, 0xf, 0x7, 0x3, 0x1};
  uint8 endBitPatterns[] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};
  uint8 startMask = bitPatterns[dx & 0x7];
  uint8 endMask = endBitPatterns[(dx+w) & 0x7];
  uint16 widthBytes = ((dx+w)/8)-(dx/8);

  dprintf("sx = %d, dx = %d, w = %d, dx + w = %d\n", sx, dx, w, dx+w);
  printf("widthBytes = %d\n", widthBytes);

  dest += (dy * SCREEN_WIDTH_BYTES);
  source += (sy * SCREEN_WIDTH_BYTES);

  dprintf("sm = %s\n", byte_to_binary(startMask));
  dprintf("em = %s %d\n", byte_to_binary(endMask), (dx+w) & 0x7);

  source += sx/8;
  dest += dx/8;

  int16 shift = (dx&0x7)-(sx&0x7);

  dprintf("extra = %x\n", (w+shift)/8);
  dprintf("shift = %d\n", shift);

  volatile uint16 x, y;
  volatile uint8 byte;
  for (y = 0; y < h; y++) {
    for (x = 0; x <= widthBytes; x++) {

      if (shift > 0) {
	byte = (*(uint8*)(source)>>(shift)|*((uint8*)(source)-1)<<(8-(shift)));
      } else if (shift < 0) {
	byte = (*(uint8*)(source)<<(-shift)|*((uint8*)(source)+1)>>(8-(-shift)));
      } else {
	byte = *source;
      }

      dprintf("byte = %x\n", byte);

      if (x == 0) {
	*dest = (*dest & ~startMask) | (byte & startMask);
      } else if (x == widthBytes) {
	blah = (*(dest) & ~endMask);
	*dest = byte & endMask;
	*dest |= blah;
      } else {
	*dest = byte;
      }

      dest++;
      source++;
    }
    dest += SCREEN_WIDTH_BYTES-widthBytes-1;
    source += SCREEN_WIDTH_BYTES-widthBytes-1;
  }
}



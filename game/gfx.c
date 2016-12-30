#include "game.h"
#include "gfx.h"
#include <hardware/blit.h>

extern  unsigned char  font[];

#define printf(...)
#define dprintf(...)

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

  11111111 11110000 00000000 00000000 00000000

  00000000 11111111 11000 00000000 00000000


  dx = 8
  sx = 0
  w  = 12
  widthWords = 2;
  int16 shift = (dx&0xf)-(sx&0xf) = 
*/

static uint16 dyOffsetsLUT[SCREEN_HEIGHT];
static uint16 widthWordsLUT[32];

void 
gfx_init()
{
  for (uint16 y = 0; y < SCREEN_HEIGHT; y++) {
    dyOffsetsLUT[y] = (y * SCREEN_WIDTH_BYTES);
  }

  WaitBlitter();
  custom->bltafwm = 0xffff;
  custom->bltalwm = 0x0000;
}

void
gfx_bitBlt(volatile uint8* dest, int16 sx, int16 sy, int16 dx, int16 dy, int16 w, int16 h, volatile uint8* source)
{
  uint16 widthWords =  ((w+8)>>4)+1;
  int16 shift = (dx&0xf);

  dest += dyOffsetsLUT[dy] + (dx>>3);
  source += dyOffsetsLUT[sy] + (sx>>3);

  WaitBlitter();

  custom->bltcon0 = (SRCA|SRCB|SRCC|DEST|0xca|shift<<ASHIFTSHIFT);
  custom->bltcon1 = shift<<BSHIFTSHIFT;
  //  custom->bltafwm = 0xffff;
  //custom->bltalwm = 0x0000;
  custom->bltamod = SCREEN_WIDTH_BYTES-(widthWords<<1);
  custom->bltbmod = SCREEN_WIDTH_BYTES-(widthWords<<1);
  custom->bltcmod = SCREEN_WIDTH_BYTES-(widthWords<<1);
  custom->bltdmod = SCREEN_WIDTH_BYTES-(widthWords<<1);
  custom->bltapt = source;
  custom->bltbpt = source;
  custom->bltcpt = dest;
  custom->bltdpt = dest;
  custom->bltsize = h<<6 | widthWords;
}

void
_gfx_bitBlt(volatile uint8* dest, int16 sx, int16 sy, int16 dx, int16 dy, int16 w, int16 h, volatile uint8* source)
{
  static uint8 bitPatterns[] = { 0xff, 0x7f, 0x3f, 0x1f, 0xf, 0x7, 0x3, 0x1};
  static uint8 endBitPatterns[] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};
  uint8 startMask = bitPatterns[dx & 0x7];
  volatile uint8 endMask = endBitPatterns[(dx+w) & 0x7];
  volatile uint16 widthBytes = ((dx+w)/8)-(dx/8)+1;
  int16 shift = (dx&0x7)-(sx&0x7);

  dest += (dy * SCREEN_WIDTH_BYTES) + dx/8;;
  source += (sy * SCREEN_WIDTH_BYTES) + sx/8;

  for (volatile uint16 y = 0; y < h; y++) {
    for (volatile uint16 x = 0; x < widthBytes; x++) {
      volatile uint8 byte;
      if (shift > 0) {
	byte = (*(uint8*)(source)>>(shift)|*((uint8*)(source)-1)<<(8-(shift)));
      } else if (shift < 0) {
	byte = (*(uint8*)(source)<<(-shift)|*((uint8*)(source)+1)>>(8-(-shift)));
      } else {
	byte = *source;
      }

      if (x == 0) {
	*dest = (*dest & ~startMask) | (byte & startMask);
      } else if (x == widthBytes-1) {
	*dest = (*dest & ~endMask) | (byte & endMask);
      } else {
	*dest = byte;
      }

      dest++;
      source++;
    }
    dest += SCREEN_WIDTH_BYTES-widthBytes;
    source += SCREEN_WIDTH_BYTES-widthBytes;
  }
}

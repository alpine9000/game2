#include "game.h"
#include "gfx.h"
#include <hardware/blit.h>

extern  unsigned char  font[];

#define printf(...)
#define dprintf(...)

static uint16 dyOffsetsLUT[SCREEN_HEIGHT];
static uint16 bltcon0LUT[16];

void 
gfx_init()
{
  for (uint16 y = 0; y < SCREEN_HEIGHT; y++) {
    dyOffsetsLUT[y] = (y * SCREEN_WIDTH_BYTES);
  }

  WaitBlitter();
}

void
gfx_fillRect(volatile uint8* fb, uint16 x, uint16 y, uint16 w, uint16 h, uint16 color)
{
  static uint16 startBitPatterns[] = { 0xffff,
			       0x7fff, 0x3fff, 0x1fff, 0x0fff, 
			       0x07ff, 0x03ff, 0x01ff, 0x00ff,
			       0x007f, 0x003f, 0x001f, 0x000f,
			       0x0007, 0x0003, 0x0001, 0x0000 };

  static uint16 endBitPatterns[] = { 0xffff, 
				    0x8000, 0xc000, 0xe000, 0xf000,
				    0xf800, 0xfc00, 0xfe00, 0xff00,
				    0xff80, 0xffc0, 0xffe0, 0xfff0,
				    0xfff8, 0xfffc, 0xfffe, 0xffff};

  uint16 startMask = startBitPatterns[x & 0xf]; 
  uint16 endMask = endBitPatterns[(x+w) & 0xf]; 
  uint32 widthWords = (((x&0x0f)+w)+15)>>4;
  
  if (widthWords == 1) {
    startMask &= endMask;
  }
  
  fb += dyOffsetsLUT[y] + (x>>3);
  
  WaitBlitter();

  custom->bltcon0 = (SRCC|DEST|0xca);
  custom->bltcon1 = 0;
  custom->bltafwm = 0xffff;
  custom->bltalwm = 0xffff;
  custom->bltdmod = SCREEN_WIDTH_BYTES-2;
  custom->bltcmod = SCREEN_WIDTH_BYTES-2;
  custom->bltbmod = 0;
  custom->bltamod = 0;
  custom->bltadat = startMask;
  custom->bltbdat = color ? 0xffff : 0x0;
  custom->bltcpt = fb;
  custom->bltdpt = fb;
  custom->bltsize = h<<6 | 1;

  if (widthWords > 1) {
    WaitBlitter();    
    custom->bltcon0 = (SRCC|DEST|0xca);
    custom->bltadat = endMask;
    custom->bltcpt = fb+((widthWords-1)<<1);
    custom->bltdpt = fb+((widthWords-1)<<1);
    custom->bltsize = h<<6 | 1;
  }

  if (widthWords > 2) {
    WaitBlitter();    
    custom->bltcon0 = (DEST|(color ? 0xff : 0x00);
    custom->bltdmod = SCREEN_WIDTH_BYTES-((widthWords-2)<<1);
    custom->bltdpt = fb+2;
    custom->bltsize = h<<6 | widthWords-2;
  }    

}


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


void
gfx_bitBlt(volatile uint8* dest, int16 sx, int16 sy, int16 dx, int16 dy, int16 w, int16 h, volatile uint8* source)
{
  uint32 widthWords =  ((w+15)>>4)+1;
  uint16 shift = (dx&0xf);

  dest += dyOffsetsLUT[dy] + (dx>>3);
  source += dyOffsetsLUT[sy] + (sx>>3);

  WaitBlitter();

  custom->bltcon0 = (SRCA|SRCB|SRCC|DEST|0xca|shift<<ASHIFTSHIFT);
  custom->bltcon1 = shift<<BSHIFTSHIFT;
  custom->bltafwm = 0xffff;
  custom->bltalwm = 0x0000;
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

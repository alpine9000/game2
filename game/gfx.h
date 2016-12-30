#ifndef __GFX_H
#define __GFX_H

#define gfx_retroFontWidth 5  
#define gfx_retroFontHeight 8


void
gfx_fillRect(volatile uint8* fb, uint16 x, uint16 y, uint16 w, uint16 h, uint16 color);
void 
gfx_drawStringRetro(volatile uint8* fb, int16 x, int16 y, char *c, uint16 color, int spaceSize);
void 
gfx_drawLine(volatile uint8* fb, int16 x0, int16 y0, int16 x1, int16 y1, uint16 color);
void
gfx_drawPixel(volatile uint8* fb, int16 x, int16 y, uint16 color) ;
void
gfx_bitBlt(volatile uint8* source, int16 sx, int16 sy, int16 dx, int16 dy, int16 w, int16 h, volatile uint8* dest);
void 
gfx_init();
#endif


#include "game.h" 

#define MAP_WIDTH_WORDS 20
#define MAP_HEIGHT_WORDS 100

#if 0

#include "game.h"

extern uint16 map[MAP_WIDTH_WORDS*MAP_HEIGHT_WORDS];
extern uint16* tileset;


uint16* mapPtr;

void 
Blit(unsigned x, unsigned y)
{
  uint16* bitplanesPtr = &bitplanes;
  uint16* tilesetPtr = &tileset;
  unsigned p;
  
  bitplanesPtr += ((y*16*SCREEN_BIT_DEPTH) + (x*16));
  
  for (y = 0; y < 16; y++) {
    for (p = 0; p < SCREEN_BIT_DEPTH; p++) {
      *bitplanesPtr = 0;
      bitplanesPtr += (SCREEN_WIDTH_BYTES);
    }
  }
}

void 
Render()
{
  unsigned x, y, p;
  
  
  for (y = 0; y < SCREEN_HEIGHT_WORDS; y++) {
    for (x = 0; x < SCREEN_WIDTH_WORDS; x++) {
      Blit(x, y);
    } 
  }
  
}
#endif

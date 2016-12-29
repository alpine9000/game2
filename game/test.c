#include <stdio.h>
#include <stdint.h>
#include <string.h>     /* strcat */
#include <stdlib.h>     /* strtol */

#define SCREEN_WIDTH_BYTES 10

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint32_t uint32;

#if 0
#define dprintf printf
#else
#define dprintf(...)
#endif

const char *byte_to_binary(int x)
{
  static char b[9];
  b[0] = '\0';

  int z;
  for (z = 128; z > 0; z >>= 1)
    {
      strcat(b, ((x & z) == z) ? "1" : "0");
    }

  return b;
}

/*
sx = 0, 0/8 = 0, 0
dx = 1, 1/8 = 0, 1 0-1 = -1
w = 8

00123456 78000000
^

01234567 80000000
         ^

00000000 00000000 00000000 00000000
     ^^^ ^^^^^^^^ ^
*/
void
gfx_bitBlt(volatile uint8* dest, int16 sx, int16 sy, int16 dx, int16 dy, int16 w, int16 h, volatile uint8* source)
{
  uint8 blah;
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
  

  //if (shift > 0) {
  //  widthBytes++;
  // }


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


/*

11111111 11111111 11111100 00000000 0000000000000000
00000000 00000000 00000000 00000011 1111111 11000000
                                 ^
*/

int main()
{
  //uint8 source[SCREEN_WIDTH_BYTES] = {0xff,0xfc,0,0,0,0,0,0,0,0};

  for (int i = 0; i <= 30; i++) {  
    uint8 source[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    uint8 dest[] =   {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    gfx_bitBlt(dest, 55, 0, i, 0, 22, 1, source);
    // gfx_bitBlt(dest, 0, 0, 1, 0, 12, 1, source);
    
    for (int i = 0; i < sizeof(source); i++) {
      dprintf("%s ", byte_to_binary(source[i]));
    }
    dprintf("\n");
    for (int i = 0; i < sizeof(dest); i++) {
      printf("%s ", byte_to_binary(dest[i]));
    }
    printf("\n");
  }
  return 0;
}

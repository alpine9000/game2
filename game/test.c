#include <stdio.h>
#include <stdint.h>
#include <string.h>     /* strcat */
#include <stdlib.h>     /* strtol */

#define SCREEN_WIDTH_BYTES 10

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef int16_t int16;


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
*/
void
gfx_bitBlt(volatile uint8* dest, int16 sx, int16 sy, int16 dx, int16 dy, int16 w, int16 h, volatile uint8* source)
{
  uint8 blah;
  uint8 bitPatterns[] = { 0xff, 0x7f, 0x3f, 0x1f, 0xf, 0x7, 0x3, 0x1};
  uint8 endBitPatterns[] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};
  uint8 startMask = bitPatterns[dx & 0x7];
  uint8 endMask = endBitPatterns[(dx+w) & 0x7];
  uint16 widthBytes = w/8;
  dest += (dy * SCREEN_WIDTH_BYTES);
  source += (sy * SCREEN_WIDTH_BYTES);

  printf("sm = %s\n", byte_to_binary(startMask));
  printf("em = %s\n", byte_to_binary(endMask));

  source += sx/8;
  dest += dx/8;

  int16 shift = (dx&0x7)-(sx&0x7);

  volatile uint16 x, y, s16;
  for (y = 0; y < h; y++) {
    for (x = 0; x < widthBytes; x++) {
      s16 = *source << 8 | *(source+1);
      if (shift > 0) {
	s16 = s16 >> shift;
      } else if (shift < 0) {
	s16 = s16 << -shift;
      }
      if (x == 0) {
	*dest = (*dest & ~startMask) | ((s16 >> 8) & startMask);
      } else {
	*dest = s16 >> 8;
      }


      if (x == widthBytes-1) {

	blah = (*(dest+1) & ~endMask);
	*(dest+1) = (s16 & endMask);
	*(dest+1) |= blah;

	printf("~endMask = %s\n", byte_to_binary(~endMask));
	printf("s16 = %s\n", byte_to_binary(s16));
	//*(dest+1) = (*(dest+1) & ~endMask) | (s16 & endMask);
      }
      dest++;
      source++;
    }
    dest += SCREEN_WIDTH_BYTES-widthBytes;
    source += SCREEN_WIDTH_BYTES-widthBytes;
  }
}


/*

00000000 00001111 11111111 00000000
11111111 11110000 00000000 00000000
*/

int main()
{
  //uint8 source[SCREEN_WIDTH_BYTES] = {0xff,0xf0,0,0,0,0,0,0,0,0};
  uint8 source[SCREEN_WIDTH_BYTES] = {0x0,0x0f,0xff,0,0,0,0,0,0,0};
  uint8 dest[SCREEN_WIDTH_BYTES] = {0x0,0x0,0x0,0,0,0,0,0,0,0};

  gfx_bitBlt(dest, 12, 0, 12, 0, 12, 1, source);
  // gfx_bitBlt(dest, 0, 0, 1, 0, 12, 1, source);

  for (int i = 0; i < SCREEN_WIDTH_BYTES; i++) {
    printf("%s ", byte_to_binary(source[i]));
  }
  printf("\n");
  for (int i = 0; i < SCREEN_WIDTH_BYTES; i++) {
    printf("%s ", byte_to_binary(dest[i]));
  }
  printf("\n");
  return 0;
}

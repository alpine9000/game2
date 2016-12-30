#ifndef __GAME_H
#define __GAME_H

#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>

#define CUSTOM ((struct Custom*)0xdff000)
#define SCREEN_WIDTH	   320
#define SCREEN_HEIGHT	   272 /* vertical overscan */
#define SCREEN_HEIGHT_WORDS SCREEN_HEIGHT/16
#define SCREEN_WIDTH_BYTES (SCREEN_WIDTH/8)
#define SCREEN_WIDTH_WORDS (SCREEN_WIDTH/16)
#define SCREEN_BIT_DEPTH   1
#define SCREEN_RES	   8 /* 8=lo resolution, 4=hi resolution */
#define RASTER_X_START	   0x81 /* hard coded coordinates from hardware manual */
#define RASTER_Y_START	   0x1d /* vertical overscan */
#define RASTER_X_STOP	   RASTER_X_START+SCREEN_WIDTH
#define RASTER_Y_STOP	   RASTER_Y_START+SCREEN_HEIGHT

typedef UBYTE uint8;
typedef SHORT int16;
typedef USHORT uint16;
typedef LONG int32;
typedef ULONG uint32;

extern volatile struct Custom *custom;
extern volatile uint16 copper;
extern volatile uint8 bitplanes;
extern volatile uint8 bitplanes2;
extern volatile uint8 spriteBitplanes;
extern unsigned long verticalBlankCount;
extern volatile uint8 joystick;
extern volatile uint8 joystickpos;

extern void WaitVerticalBlank();
extern void InitInterrupts();
extern void SetupPalette();
extern void SpaceInvadersInit();
extern void SpaceInvadersLoop();
extern void PokeCopperList(volatile uint32 bitplanesPtr);
extern void WaitBlitter();

#endif /* __GAME_H */

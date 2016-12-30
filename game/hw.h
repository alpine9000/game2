#ifndef __HW_H
#define __HW_H

extern void hw_waitVerticalBlank(void);
extern void hw_setupPalette(void);
extern void hw_interruptsInit(void);
extern void hw_waitBlitter(void);

extern volatile uint8 hw_joystickButton;
extern volatile uint8 hw_joystickPos;

#endif

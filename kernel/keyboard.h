#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#define ND				0	// for NotDefined
#define ESC 			27
#define BACKSPACE		'\b'
#define TAB				9
#define ENTER			'\n'
#define CTRL			17
#define SHIFT_LEFT 		6
#define SHIFT_RIGHT 	7
#define ALT				18
#define ARROW_UP		224
#define ARROW_LEFT		225
#define ARROW_RIGHT		226
#define ARROW_DOWN		227

extern void keyboard_handler();
extern int getc();
extern int keypressed();

#endif

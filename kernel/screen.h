#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "../common/types.h"

#define COLORS_NB       15

#define BLACK           0x0
#define BLUE            0x1
#define GREEN           0x2
#define CYAN            0x3
#define RED             0x4
#define MAGENTA         0x5
#define BROWN           0x6
#define LIGHT_GRAY      0x7
#define DARK_GRAY       0x8
#define LIGHT_BLUE      0x9
#define LIGHT_GREEN     0xa
#define LIGHT_CYAN      0xb
#define LIGHT_RED       0xc
#define LIGHT_MAGENTA   0xd
#define YELLOW          0xe
#define WHITE           0xf

typedef struct scr_xy {
	uchar x;
	uchar y;
} __attribute__((packed)) scr_xy_t;

typedef struct screen {
	scr_xy_t cursor;		// cursor coordinate in the screen
	ushort* screen_ptr;		// points to VRAM
	uchar fg_color;			// foreground color
	uchar bg_color;			// background color
} __attribute__((packed)) screen_t;

extern void clr_scr();
extern void init_scr();
extern void printf(char* str, ...);
extern void set_theme(uchar bg_color, uchar fg_color);
extern uchar get_fg_color();
extern uchar get_bg_color();
extern scr_xy_t get_cursor_pos();
#ifdef TEST
extern void test_screen();
#endif

#endif

#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "../common/types.h"

#define VRAM            0xb8000
#define COMMAND_PORT    0x3d4
#define DATA_PORT       0x3d5
#define SCREEN_WIDTH    80
#define SCREEN_HEIGHT   25
#define FIRST_ADDR      (uchar*)VRAM
#define LAST_ADDR       (FIRST_ADDR + SCREEN_WIDTH * SCREEN_HEIGHT * 2)
#define CHAR_COUNT      (SCREEN_WIDTH * SCREEN_HEIGHT)
#define COLORS_NB       15

#define BLACK 0x0
#define BLUE 0x1
#define GREEN 0x2
#define CYAN 0x3
#define RED 0x4
#define MAGENTA 0x5
#define BROWN 0x6
#define LIGHT_GRAY 0x7
#define DARK_GRAY 0x8
#define LIGHT_BLUE 0x9
#define LIGHT_GREEN 0xa
#define LIGHT_CYAN 0xb
#define LIGHT_RED 0xc
#define LIGHT_MAGENTA 0xd
#define YELLOW 0xe
#define WHITE 0xf

typedef struct scr_xy {
	uchar x;
	uchar y;
} scr_xy_t;

typedef struct screen {
    scr_xy_t cursor;		// cursor coordinate in the screen
    ushort* screen_ptr;		// points to VRAM
    uchar fg_color;			// foreground color
    uchar bg_color;			// background color
} screen_t;

void init_scr(void);
void clrscr(void);
void write(char* str);
void set_theme(uchar bg_color, uchar fg_color);
uchar get_fg_color();
uchar get_bg_color();
void printf(char* str, ...);

#endif

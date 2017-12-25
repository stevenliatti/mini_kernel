/**
 * @file 		screen.c
 * @brief 		Screen functions to print and move cursor.
 *
 * @author 		Steven Liatti
 * @author 		Raed Abdennadher
 * @bug 		No known bugs.
 * @date 		November 3, 2017
 * @version		1.0
 */

#include "screen.h"
#include "base.h"
#include "pio.h"

#define COMMAND_PORT    0x3d4
#define FIRST_COMMAND 	0xe
#define SECOND_COMMAND 	0xf
#define DATA_PORT       0x3d5
#define FIRST_ADDR      (ushort*) 0xb8000
#define LAST_ADDR       (FIRST_ADDR + SCREEN_WIDTH * SCREEN_HEIGHT * 2)
#define CHAR_COUNT      (SCREEN_WIDTH * SCREEN_HEIGHT)
#define RESET_BG_FG 	0xf00
#define TEB_LEN			4

static screen_t screen;

/**
 * @brief  convert x and y coordinates in memory video value
 * 
 * @param  x x coordinate
 * @param  y y coordinate
 * @return   value in memory video
 */
static ushort xy_to_offset(ushort x, ushort y) {
	return (y * SCREEN_WIDTH + x);
}

/**
 * @brief print a char at the position and with the colors given
 * 
 * @param x  x coordinate
 * @param y  y coordinate
 * @param c  character
 * @param bg background color
 * @param fg foreground color
 */
static void print_char_by_xy_color(ushort x, ushort y, uchar c, uchar bg, uchar fg) {
	screen.screen_ptr[xy_to_offset(x, y)] = (((bg << 4) | fg) << 8) | c;
}

/**
 * @brief print a char at the position given
 * 
 * @param x  x coordinate
 * @param y  y coordinate
 * @param c  character
 */
static void print_char_by_xy(ushort x, ushort y, uchar c) {
	print_char_by_xy_color(x, y, c, screen.bg_color, screen.fg_color);
}

/**
 * @brief shift up the screen if the end is reached
 */
static void shift_up() {
	memcpy((int*)FIRST_ADDR, FIRST_ADDR + SCREEN_WIDTH, LAST_ADDR - FIRST_ADDR - SCREEN_WIDTH);
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		print_char_by_xy_color(i, SCREEN_HEIGHT - 1, '\0', BLACK, LIGHT_GRAY);
	}
}

/**
 * @brief print char at the cursor position. Check if the coordinates are correct.
 * @param c the character
 */
static void print_char_on_cursor(uchar c) {
	uchar new_char_x = screen.cursor.x;
	uchar new_char_y = screen.cursor.y;
	uchar new_cur_x = screen.cursor.x + 1;
	uchar new_cur_y = screen.cursor.y;
	
	if (c == '\b') {
		if (new_char_x == 0 && new_char_y == 0) {
			new_cur_x -= 1;
		}
		else if (new_char_x == 0 && new_char_y > 0) {
			new_cur_x = SCREEN_WIDTH - 1;
			new_cur_y -= 1;
		}
		else {
			new_cur_x -= 2;
		}
		new_char_x = new_cur_x;
		new_char_y = new_cur_y;
		print_char_by_xy(new_char_x, new_char_y, '\0');
	}
	if (c == '\t') {
		for (int i = 0; i < TEB_LEN; i++) {
			print_char_on_cursor(' ');
			new_cur_x++;
		}
	}
	if (c == '\n') {
		new_cur_x = 0;
		new_cur_y++;
	}
	if (new_cur_x >= SCREEN_WIDTH) {
		new_cur_x = new_cur_x - SCREEN_WIDTH;
		new_cur_y++;
	}
	if (new_cur_y >= SCREEN_HEIGHT) {
		shift_up();
		new_char_y = screen.cursor.y - 1;
		new_cur_y = SCREEN_HEIGHT - 1;
	}
	if (c != '\n' && c != '\b' && c != '\t') {
		print_char_by_xy(new_char_x, new_char_y, c);
	}
	move_cursor(new_cur_x, new_cur_y);
}

/**
 * @brief print a char array (string) at the cursor position.
 * @param str [description]
 */
static void print_string_on_cursor(uchar* str) {
	for (uint i = 0; str[i] != 0; i++) {
		print_char_on_cursor(str[i]);
	}
}

/**
 * @brief clear the screen with background black and foreground white
 */
void clr_scr() { //
	for (ushort i = 0; i < CHAR_COUNT; i++) {
		screen.screen_ptr[i] = RESET_BG_FG;
	}
	move_cursor(0, 0);
}

/**
 * @brief Initialise the screen (text color, background color and screen)
 */
void scr_init() {
	screen.screen_ptr = FIRST_ADDR;
	screen.fg_color = LIGHT_GRAY;
	screen.bg_color = BLACK;
	clr_scr();
}

/**
 * @brief move cursor to coordinates given
 * 
 * @param x x coordinate
 * @param y y coordinate
 */
void move_cursor(uchar x, uchar y) { //
	ushort cur_val = xy_to_offset(x, y);
	outw(COMMAND_PORT, FIRST_COMMAND);
	outw(DATA_PORT, cur_val >> 8);
	outw(COMMAND_PORT, SECOND_COMMAND);
	outw(DATA_PORT, cur_val & 0xff);
	screen.cursor.x = x;
	screen.cursor.y = y;
}

/**
 * @brief format and print data
 */
void printf(char* str, ...) {
	uchar buffer[CONVERT_BUFFER_SIZE];
	uint* next_arg = (uint*) &str + 1;
	while (*str != '\0') {
		if (*str == '%') {
			str++;
			switch(*str) {
				case 'c' : 
					print_char_on_cursor(*next_arg);
					break;
				case 's' :
					print_string_on_cursor((uchar*) *next_arg);
					break;
				case 'd' :
					itoa((int) *next_arg, buffer);
					print_string_on_cursor(buffer);
					break;
				case 'x' :
					itox((int) *next_arg, buffer);
					print_string_on_cursor(buffer);
					break;
				default :
					print_char_on_cursor(*(str - 1));
					print_char_on_cursor(*str);
			}
			next_arg++;
		}
		else {
			print_char_on_cursor(*str);
		}
		str++;
	}
}

/**
 * @brief set background and foreground color
 * @param fg_color foreground color
 * @param bg_color background color
 */
void set_theme(uchar fg_color, uchar bg_color) {
	screen.fg_color = fg_color;
	screen.bg_color = bg_color;
}

/**
 * @brief get foreground color
 * @return foreground color
 */
uchar get_fg_color() {
	return screen.fg_color;
}

/**
 * @breif get background color
 * @return [description]
 */
uchar get_bg_color() {
	return screen.bg_color;
}

/**
 * @brief get cursor position
 * @return cursor position
 */
scr_xy_t get_cursor_pos() {
	return screen.cursor;
}

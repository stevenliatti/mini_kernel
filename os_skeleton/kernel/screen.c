#include "screen.h"
#include "base.h"

extern void outw(uint16_t port, uint8_t data);

screen_t screen;

ushort xy_to_offset(ushort x, ushort y) {
	return (y * SCREEN_WIDTH + x);
}

scr_xy_t offset_to_xy(ushort offset) {
	scr_xy_t temp = {
		.x = (uchar) offset % SCREEN_WIDTH,
		.y = (uchar) offset / SCREEN_WIDTH
	};
	return temp;
}

void move_cursor(uchar x, uchar y) {
	ushort cur_val = xy_to_offset(x, y);
	outw(COMMAND_PORT, 0xe);
	outw(DATA_PORT, cur_val >> 8);
	outw(COMMAND_PORT, 0xf);
	outw(DATA_PORT, cur_val & 0xff);
	screen.cursor.x = x;
	screen.cursor.y = y;
}

/*!
 * \brief Clear screen
 */
void clrscr(void) {
	for (ushort i = 0; i < CHAR_COUNT; i++) {
		screen.screen_ptr[i] = 0xf00;
	}
	move_cursor(0, 0);
}

/*!
 * \brief Initialise the screen (text color, background color and screen)
 */
void init_scr(void) {
	screen.screen_ptr = (ushort *) VRAM;
	screen.fg_color = LIGHT_GRAY;
	screen.bg_color = BLACK;
	clrscr();
}

void print_char_by_xy(ushort x, ushort y, char c) {
	screen.screen_ptr[xy_to_offset(x, y)] = (((screen.bg_color << 4) | screen.fg_color) << 8) | c;
}

void print_char_by_xy_color(ushort x, ushort y, uchar c, uchar bg, uchar fg) {
	screen.screen_ptr[xy_to_offset(x, y)] = (((bg << 4) | fg) << 8) | c;
}

void shift_up() {
	uchar scr_w_in_mem = SCREEN_WIDTH * 2;
	memcpy((int*)FIRST_ADDR, FIRST_ADDR + scr_w_in_mem, LAST_ADDR - FIRST_ADDR - scr_w_in_mem);
	// TODO: understand why the cursor disapear if we put '\0' for value in memset
	// memset(FIRST_ADDR + scr_w_in_mem * SCREEN_HEIGHT - scr_w_in_mem, 'a', scr_w_in_mem);
	for (int i = 0; i < 80; i++) {
		print_char_by_xy_color(i, 24, '\0', BLACK, LIGHT_GRAY);
	}
}

void set_theme(uchar fg_color, uchar bg_color) {
	screen.fg_color = fg_color;
	screen.bg_color = bg_color;
}

void print_char_on_cursor(char c) {
	uchar new_char_x = screen.cursor.x;
	uchar new_char_y = screen.cursor.y;
	uchar new_cur_x = screen.cursor.x + 1;
	uchar new_cur_y = screen.cursor.y;
	
	if (c == '\n') {
		new_cur_x = 0;
		new_cur_y++;
	}
	if (new_cur_x == SCREEN_WIDTH) {
		new_cur_x = 0;
		new_cur_y++;
	}
	if (new_cur_y >= SCREEN_HEIGHT) {
		shift_up();
		new_char_y = screen.cursor.y - 1;
		new_cur_y = SCREEN_HEIGHT - 1;
	}

	if (c != '\n')
		print_char_by_xy(new_char_x, new_char_y, c);
	move_cursor(new_cur_x, new_cur_y);
}

void print_string_on_cursor(char* str) {
	for (uint i = 0; str[i] != 0; i++) {
		print_char_on_cursor(str[i]);
	}
}

uchar get_fg_color() {
	return screen.fg_color;
}

uchar get_bg_color() {
	return screen.bg_color;
}

scr_xy_t get_cursor_pos() {
	return screen.cursor;
}

void printf(char* str, ...) {
	char buffer[128];
	uint* next_arg = (uint*) &str + 1;
	while (*str != '\0') {
		if (*str == '%') {
			str++;
			switch(*str) {
				case 'c' : 
					print_char_on_cursor(*next_arg);
					break;
				case 's' :
					print_string_on_cursor((char*) *next_arg);
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

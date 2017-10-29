#include "screen.h"
#include "base.h"

// ushort volatile * const scr_first_adress = (ushort *) VRAM;

extern void outw(uint16_t port, uint8_t data);

screen_t screen;

ushort xy_to_offset(ushort x, ushort y) {
	return (y * 80 + x);
}

scr_xy_t offset_to_xy(ushort offset) {
	scr_xy_t temp = {
		.x = (uchar) offset % 80,
		.y = (uchar) offset / 80
	};
	return temp;
}

void move_cursor(ushort x, ushort y) {
	ushort cur_val = xy_to_offset(x, y);
	outw(0x3d4, 0xe);
	outw(0x3d5, cur_val >> 8);
	outw(0x3d4, 0xf);
	outw(0x3d5, cur_val & 0xff);
	// screen.cursor_ptr = (ushort*) VRAM + cur_val;
	screen.cursor.x = x;
	screen.cursor.y = y;
}

/*!
 * \brief Clear screen
 */
void clrscr(void) {
	for (ushort i = 0; i < SCREEN_WORDS_NB; i++) {
		screen.screen_ptr[i] = 0xf00;
	}
}

/*!
 * \brief Initialise the screen (text color, background color and screen)
 */
void init_scr(void) {
	screen.screen_ptr = (ushort *) VRAM;
	screen.fg_color = WHITE;
	screen.bg_color = BLACK;
	clrscr();
	move_cursor(0, 0);
}

void print_char_by_xy(ushort x, ushort y, char c) {
	ushort offset = xy_to_offset(x, y);
	screen.screen_ptr[offset] = (((screen.bg_color << 4) | screen.fg_color) << 8) | c;
}

// TODO: ask if we have to update the screen after changing colors
void set_theme(uchar fg_color, uchar bg_color) {
	screen.fg_color = fg_color;
	screen.bg_color = bg_color;
	for (uchar x = 0; x < 80; x++) {
		for (uchar y = 0; y < 25; y++) {
			uchar offset = xy_to_offset(x, y);
			uchar ascii_val = screen.screen_ptr[offset] & 0xff;
			print_char_by_xy(x, y, ascii_val);
		}
	}
}

void print_char_on_cursor(char c) {
	print_char_by_xy(screen.cursor.x, screen.cursor.y, c);
	move_cursor(screen.cursor.x + 1, screen.cursor.y);
}

void print_string_on_cursor(char* str) {
	uint i;
	for (i = 0; str[i] != 0; i++) {
		print_char_on_cursor(str[i]);
	}
}

// TODO: ask if we have to make an adress in parameter to get its colors
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
		if (strncmp((uchar*) str, (uchar*) "%", 1) == 0) {
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
		} else if (strncmp((uchar*) str, (uchar*) "\\", 1) == 0) {
			str++;
			switch(*str) {
				case 'n' : 
					// printf("(%d,%d)", screen.cursor.x, screen.cursor.y);
					move_cursor(0, screen.cursor.y + 1);
					break;
			}
		} else {
			print_char_on_cursor(*str);
		}
		str++;
	}
}
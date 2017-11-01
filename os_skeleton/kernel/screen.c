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
	// screen.cursor_ptr = (ushort*) VRAM + cur_val;
	if (x == SCREEN_WIDTH) {
		screen.cursor.x = 0;
		screen.cursor.y += 1;
	} else {
		screen.cursor.x = x;
		if (screen.cursor.y == SCREEN_HEIGHT)
			screen.cursor.y = SCREEN_HEIGHT - 1;
		else
			screen.cursor.y = y;
	}
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

void shift_up() {
	// memcpy(screen.screen_ptr, screen.screen_ptr + SCREEN_WIDTH, SCREEN_WIDTH * 23 * 2);
	for (uchar y = 1; y < SCREEN_HEIGHT; y++) {
		// for (uchar x = 0; x < SCREEN_WIDTH; x++) {
			// uchar offset = xy_to_offset(x, y);
		memcpy(screen.screen_ptr + SCREEN_WIDTH * (y - 1), screen.screen_ptr + SCREEN_WIDTH * y, SCREEN_WIDTH * 2);
	// 		uchar ascii_val = screen.screen_ptr[offset] & 0xff;
	// 		print_char_by_offset(xy_to_offset(x, y - 1), ascii_val);
		// }
	}
	memset(screen.screen_ptr + SCREEN_WIDTH * SCREEN_HEIGHT - 1, '\0', SCREEN_WIDTH * 2);
	// for (uchar x = 0; x < SCREEN_WIDTH; x++) {
	// 	print_char_by_offset(xy_to_offset(x, SCREEN_HEIGHT - 1), '\0');
	// }
}

void print_char_by_xy(ushort x, ushort y, char c) {
	if (x == SCREEN_WIDTH) {
		x = 0;
		y += 1;
	}
	if (y == SCREEN_HEIGHT) {
		y = SCREEN_HEIGHT - 1;
		shift_up();
	}
	screen.screen_ptr[xy_to_offset(x, y)] = (((screen.bg_color << 4) | screen.fg_color) << 8) | c;
}

void set_theme(uchar fg_color, uchar bg_color) {
	screen.fg_color = fg_color;
	screen.bg_color = bg_color;
}

void print_char_on_cursor(char c) {
	print_char_by_xy(screen.cursor.x, screen.cursor.y, c);
	move_cursor(screen.cursor.x + 1, screen.cursor.y);
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
		} else if (*str == '\n') {
			move_cursor(0, screen.cursor.y + 1);
		} else {
			print_char_on_cursor(*str);
		}
		str++;
	}
}

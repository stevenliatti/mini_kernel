#include "screen.h"
#include "base.h"

#define VRAM            0xb8000
#define COMMAND_PORT    0x3d4
#define DATA_PORT       0x3d5
#define SCREEN_WIDTH    80
#define SCREEN_HEIGHT   25
#define FIRST_ADDR      (uchar*)VRAM
#define LAST_ADDR       (FIRST_ADDR + SCREEN_WIDTH * SCREEN_HEIGHT * 2 - 2)
#define CHAR_COUNT      (SCREEN_WIDTH * SCREEN_HEIGHT)

extern void outw(uint16_t port, uint8_t data);

static screen_t screen;

static ushort xy_to_offset(ushort x, ushort y) {
	return (y * SCREEN_WIDTH + x);
}

static void print_char_by_xy_color(ushort x, ushort y, uchar c, uchar bg, uchar fg) { //
	screen.screen_ptr[xy_to_offset(x, y)] = (((bg << 4) | fg) << 8) | c;
}

static void print_char_by_xy(ushort x, ushort y, char c) { //
	print_char_by_xy_color(x, y, c, screen.bg_color, screen.fg_color);
}

static void shift_up() {
	uchar scr_w_in_mem = SCREEN_WIDTH * 2;
	memcpy((int*)FIRST_ADDR, FIRST_ADDR + scr_w_in_mem, LAST_ADDR - FIRST_ADDR - scr_w_in_mem);
	for (int i = 0; i < 80; i++) {
		print_char_by_xy_color(i, 24, '\0', BLACK, LIGHT_GRAY);
	}
}

static void print_char_on_cursor(char c) { //
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

	if (c != '\n') {
		print_char_by_xy(new_char_x, new_char_y, c);
	}
		
	move_cursor(new_cur_x, new_cur_y);
}

static void print_string_on_cursor(char* str) { //
	for (uint i = 0; str[i] != 0; i++) {
		print_char_on_cursor(str[i]);
	}
}

/*!
 * \brief Clear screen
 */
void clr_scr() { //
	for (ushort i = 0; i < CHAR_COUNT; i++) {
		screen.screen_ptr[i] = 0xf00;
	}
	move_cursor(0, 0);
}

/*!
 * \brief Initialise the screen (text color, background color and screen)
 */
void init_scr() {
	screen.screen_ptr = (ushort *) VRAM;
	screen.fg_color = LIGHT_GRAY;
	screen.bg_color = BLACK;
	clr_scr();
}

void move_cursor(uchar x, uchar y) { //
	ushort cur_val = xy_to_offset(x, y);
	outw(COMMAND_PORT, 0xe);
	outw(DATA_PORT, cur_val >> 8);
	outw(COMMAND_PORT, 0xf);
	outw(DATA_PORT, cur_val & 0xff);
	screen.cursor.x = x;
	screen.cursor.y = y;
}

void printf(char* str, ...) { //
	char buffer[BUFFER_SIZE];
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

void set_theme(uchar fg_color, uchar bg_color) { //
	screen.fg_color = fg_color;
	screen.bg_color = bg_color;
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

#ifdef TEST
void test_screen() {
	printf("HELLO\n");
	clr_scr();

	set_theme(LIGHT_GREEN, RED);
	for (int i = 65; i < 91; i++) {
		printf("%s \"%c\" => %d => %x negate %d\n", "Char", i, i, i, -i);
	}

	move_cursor(40, 20);
	printf("Decimal Base : -(%d) = %d", 10, -10);
	move_cursor(40, 21);
	printf("Hexadecimal Base : -(%x) = %x", 10, -10);

	set_theme(BROWN, LIGHT_BLUE);
	move_cursor(40, 10);
	print_string_on_cursor("Raed");
	move_cursor(40, 11);
	print_string_on_cursor("Abdennadher");
	move_cursor(55, 10);
	print_string_on_cursor("Steven");
	move_cursor(55, 11);
	print_string_on_cursor("Liatti");

	print_char_by_xy(78, 0, '&');
	print_char_by_xy(78, 24, '&');
	print_char_by_xy_color(79, 0, '*', WHITE, RED);
	print_char_by_xy_color(79, 24, '*', WHITE, RED);

	set_theme(BLACK, YELLOW);
	move_cursor(2, 1);
	scr_xy_t pos = get_cursor_pos();
	printf("get_cursor_pos() = (%d,%d) , the position of 'g' character", pos.x, pos.y);
	move_cursor(2, 2);
	printf("get_bg_color() = %d", get_bg_color());
	move_cursor(2, 3);
	printf("get_fg_color() = %d", get_fg_color());
}
#endif

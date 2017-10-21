#include "screen.h"

ushort volatile * const screen = (ushort *) VRAM;

void move_cursor(ushort l, ushort c) {
	ushort cur_val = l * 80 + c;
	outw(0x3d4, 0xe);
	outw(0x3d5, cur_val >> 8);
	outw(0x3d4, 0xf);
	outw(0x3d5, cur_val & 0xff);
}

void clrscr(void) {
	for (ushort i = 0; i < SCREEN_WORDS_NB; i++) {
		screen[i] = 0xf00;
	}
}

void init_scr(void) {
	clrscr();
	move_cursor(0, 0);
}

// TODO: ask if we have to update the screen after change colors
void set_theme(uchar bg_color, uchar fg_color) {
	for (int i = 0; i < 80 * 25; i++) {
		uchar ascii_val = screen[i] & 0xff;
		print_char_color(i, bg_color, fg_color, ascii_val);
	}
}

// TODO: ask if we have to make an adress in parameter to get its colors
uchar get_fg_color() {
	return (uchar)((ushort)(screen[0] & 0x0f00) >> 8);
}

uchar get_bg_color() {
	return (uchar)((ushort)(screen[0] & 0xf000) >> 12);
}

void print_char_color(ushort addr, uchar bg_color, uchar fg_color, char c) {
	screen[addr] = (((bg_color << 4) | fg_color) << 8) | c;
}

void print_char(uchar x, uchar y, char c) {
	// TODO: struct screen
}


ushort lenght(char* str) {
	ushort s = 0;
	for (uint i = 0; str[i] != 0; i++) {
		s++;
	}
	return s;
}

void write(char* str) {
	ushort begin = SCREEN_WORDS_NB / 2 - lenght(str) / 2;
	uchar fg_color = 0xf;
	uchar bg_color = 0x0;
	for (uint i = 0; str[i] != 0; i++) {
		print_char_color(begin + i, bg_color, fg_color, str[i]);
	}
}



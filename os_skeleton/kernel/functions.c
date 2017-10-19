#include <stdint.h>

#define VRAM 0xb8000

#define SCREEN_WORDS_NB 4000 / 2

#define COLORS_NB 15

uint16_t volatile * const screen = (uint16_t *) VRAM;

void clrscr(void) {
	for (uint16_t i = 0; i < SCREEN_WORDS_NB; i++) {
		screen[i] = 0xf00;
	}
}

uint16_t lenght(char* str) {
	uint16_t s = 0;
	for (int i = 0; str[i] != 0; i++) {
		s++;
	}
	return s;
}

void draw_char(uint16_t addr, uint8_t bg_color, uint8_t fg_color, char c) {
	screen[addr] = (((bg_color << 4) | fg_color) << 8) | c;
}

void write(char* str) {
	uint16_t begin = SCREEN_WORDS_NB / 2 - lenght(str) / 2;
	uint8_t fg_color = 0xf;
	uint8_t bg_color = 0x0;
	for (int i = 0; str[i] != 0; i++) {
		draw_char(begin + i, bg_color, fg_color - i % COLORS_NB, str[i]);
	}
}
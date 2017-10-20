
#include "multiboot.h"
#include "gdt.h"


#define VRAM 0xb8000

#define SCREEN_WORDS_NB 4000 / 2

#define COLORS_NB 15

ushort volatile * const screen = (ushort *) VRAM;

void clrscr(void) {
	for (ushort i = 0; i < SCREEN_WORDS_NB; i++) {
		screen[i] = 0xf00;
	}
}

ushort lenght(char* str) {
	ushort s = 0;
	for (uint i = 0; str[i] != 0; i++) {
		s++;
	}
	return s;
}

void draw_char(ushort addr, uchar bg_color, uchar fg_color, char c) {
	screen[addr] = (((bg_color << 4) | fg_color) << 8) | c;
}

void write(char* str) {
	ushort begin = SCREEN_WORDS_NB / 2 - lenght(str) / 2;
	uchar fg_color = 0xf;
	uchar bg_color = 0x0;
	for (uint i = 0; str[i] != 0; i++) {
		draw_char(begin + i, bg_color, fg_color - i % COLORS_NB, str[i]);
	}
}

uint kernel_entry(multiboot_info_t* boot_info) {
	gdt_init();
	clrscr();
	write("HELLO");
	while (1) {
	}
}

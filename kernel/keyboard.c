#include "keyboard.h"
#include "screen.h"
#include "../common/types.h"
#include "pio.h"

#define KEYBOARD_BUFFER_SIZE 1024

#define ESC 			27
#define BACKSPACE		'\b'
#define TAB				9
#define ENTER			'\n'
#define CTRL			17
#define SHIFT_LEFT 		6
#define SHIFT_RIGHT 	7
#define ALT				18

static struct {
	uint buffer[KEYBOARD_BUFFER_SIZE];
	uint read;
	uint write;
	uint count; 
} circ_buffer = {{}, 0, 0, 0};

static uchar mapping_shift[] = {
	0,0,'+','"','*','‡','%','&','/','(',')','=','?','`',0,
	0,'Q','W','E','R','T','Z','U','I','O','P','','!',ENTER,
	0,'A','S','D','F','G','H','J','K','L','”','„','ø',0,'œ',
	'Y','X','C','V','B','N','M',';',':','_',0,0,0,' '
};
static uchar mapping[] = {
	0,ESC,'1','2','3','4','5','6','7','8','9','0','\'','^',BACKSPACE,
	TAB,'q','w','e','r','t','z','u','i','o','p','Š','?',ENTER,
	CTRL,'a','s','d','f','g','h','j','k','l','‚','…','?',SHIFT_LEFT,'$',
	'y','x','c','v','b','n','m',',','.','-',SHIFT_RIGHT,0,ALT,' '
};
static uchar shift = false;

void keyboard_handler() {
	// If the buffer is full, print it and return
	if (keypressed()) {
		uchar key = inb(0x60);
		if (circ_buffer.count == KEYBOARD_BUFFER_SIZE) {
			uchar color = get_fg_color();
			set_theme(RED, get_bg_color());
			printf("Keyboard buffer full\n");
			set_theme(color, get_bg_color());		
			return;	
		}
		// If bit 7 is 0, the key is pressed
		if (!(key >> 7)) {
			if (mapping[key] == SHIFT_LEFT || mapping[key] == SHIFT_RIGHT) {
				shift = true;
				return;
			}
			if (shift) {
				circ_buffer.buffer[circ_buffer.write] = mapping_shift[key];
			}
			else {
				circ_buffer.buffer[circ_buffer.write] = mapping[key];
			}
			circ_buffer.write = (circ_buffer.write + 1) % KEYBOARD_BUFFER_SIZE;
			circ_buffer.count++;
		}
		// If bit 7 is 1, the key is released
		else {
			// Set the bit 7 to 0 for shift use
			key &= ~(0x1 << 7);
			if (mapping[key] == SHIFT_LEFT || mapping[key] == SHIFT_RIGHT) {
				shift = false;
			}
		}
	}
}

int getc() {
	// Read a character from the buffer if it's not empty
	while (circ_buffer.count == 0) {}
	ushort pos = circ_buffer.read;
	circ_buffer.read = (circ_buffer.read + 1) % KEYBOARD_BUFFER_SIZE;
	circ_buffer.count--;
	return circ_buffer.buffer[pos];
}

// Non-blocking call. Return 1 if a key is pressed
int keypressed() {
	return inb(0x64) & 0x1;
}

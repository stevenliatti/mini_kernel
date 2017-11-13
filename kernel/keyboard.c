#include "keyboard.h"
#include "screen.h"
#include "../common/types.h"

#define KEYBOARD_BUFFER_SIZE 1024

#define SHIFT_LEFT 		0x2A
#define SHIFT_RIGHT 	0x36
#define RETURN 			0x1C

extern uint8_t inb(uint16_t port);

static struct {
	uint buffer[KEYBOARD_BUFFER_SIZE];
	uint read;
	uint write; 
} circ_buffer = {{}, 0, 1};

static uchar mapping[] =       "--1234567890'^--qwertzuiope---asdfghjklea-,\\yxcvbnm,.--12 456789abcdefghijklknopqrtuwv<yz";
static uchar mapping_shift[] = "--+\"*c%&/()=?`--QWERTZUIOPu!--ASDFGHJKLEA-L<YXCVBNM;:_-12 4567------------------------>--";
static uchar shift = false;

void keyboard_init() {

}

void keyboard_handler() {
	// If the buffer is full, print it and return

	// if (circ_buffer.read == circ_buffer.write) {
	// 	uchar color = get_fg_color();
	// 	set_theme(get_bg_color(), RED);
	// 	printf("Keyboard buffer full");
	// 	set_theme(get_bg_color(), color);		
	// 	return;
	// }

	uchar key = inb(0x60);
	// If bit 7 is 0, the key is pressed
	if (!(key >> 7)) {
		switch (key) {
			case SHIFT_LEFT:
			case SHIFT_RIGHT:
				shift = true;
				break;
			case RETURN:
				circ_buffer.buffer[circ_buffer.write] = '\n';
				circ_buffer.write = (circ_buffer.write + 1) % KEYBOARD_BUFFER_SIZE;
				break;
			default:
				if (shift) {
					circ_buffer.buffer[circ_buffer.write] = mapping_shift[key];
				}
				else {
					circ_buffer.buffer[circ_buffer.write] = mapping[key];
				}
				circ_buffer.write = (circ_buffer.write + 1) % KEYBOARD_BUFFER_SIZE;
				break;
		}
	}
	// If bit 7 is 1, the key is released
	else {
		// Set the bit 7 to 0 for shift use
		key &= ~(0x1 << 7);
		if (key == SHIFT_LEFT || key == SHIFT_RIGHT) {
			shift = false;
		}
	}
}

int getc() {
	// Read a character from the buffer if it's not empty
	if (circ_buffer.read != circ_buffer.write) {
		ushort pos = circ_buffer.read;
		circ_buffer.read = (circ_buffer.read + 1) % KEYBOARD_BUFFER_SIZE;
		return circ_buffer.buffer[pos];
	}
	return -1;
}

// Non-blocking call. Return 1 if a key is pressed
int keypressed() {
	uchar key = inb(0x60);
	if (!(key >> 7)) return 1;
	else return 0;
}

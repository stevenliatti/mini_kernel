#include "keyboard.h"
#include "screen.h"
#include "../common/types.h"

#define KEYBOARD_BUFFER_SIZE 1024

extern uint8_t inb(uint16_t port);

static struct {
	uint buffer[KEYBOARD_BUFFER_SIZE];
	uint read;
	uint write; 
} circ_buffer = {{},0,0};

void keyboard_init() {

}

void keyboard_handler() {
	// If the buffer is full, print it and return
	if (circ_buffer.read == circ_buffer.write) {
		printf("Keyboard buffer full");
		return;
	}

	uchar key = inb(0x60);

	// somewhere :
	// circ_buffer.write = (circ_buffer.write + 1) % KEYBOARD_BUFFER_SIZE;
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

}

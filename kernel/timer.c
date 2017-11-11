#include "base.h"
#include "screen.h"

#define MAX_FREQ 	1193180
#define MIN_FREQ 	18.2065
#define MAX_DIV 	65535
#define MIN_DIV 	1

extern void outb(uint16_t port, uint8_t data);

typedef struct timer_st {
	uint ticks;
	uint freq_hz;
} timer_t;

static timer_t timer;

void timer_init(uint freq_hz) {
	timer.ticks = 0;

	ushort div = 0;
	if (freq_hz < MIN_FREQ) {
		div = MAX_DIV;
		timer.freq_hz = MIN_FREQ;
	}
	else if (freq_hz > MAX_FREQ) {
		div = MIN_DIV;
		timer.freq_hz = MAX_FREQ;
	}
	else {
		div = MAX_FREQ / freq_hz;
		timer.freq_hz = freq_hz;
	}

	printf("In timer_init, div : %d, freq : %d\n", div, timer.freq_hz);

	// PIT programmation
	// set divisor selection and repetition mode
	outb(0x43, 0x36);
	// write in canal 0 LSB and MSB of divisor
	outb(0x40, div & 0xFF);
	outb(0x40, div >> 8);
}

void timer_handler() {
	timer.ticks++;
	printf("Ticks : %d\n", timer.ticks);
}

uint get_ticks() {
	return timer.ticks;
}

void sleep(uint ms) {
	// necessary cast to improve precision
	uint ticks_count = get_ticks() + ((double) ms / 1000.0 * timer.freq_hz);
	while (get_ticks() != ticks_count);
}

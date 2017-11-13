#include "base.h"
#include "screen.h"

#define MAX_FREQ 	1193180
#define MIN_FREQ 	19

extern void outb(uint16_t port, uint8_t data);

typedef struct timer_st {
	uint ticks;
	uint freq_hz;
} timer_t;

static timer_t timer;

void timer_init(uint freq_hz) {
	timer.ticks = 0;

	if (freq_hz < MIN_FREQ) {
		timer.freq_hz = MIN_FREQ;
	}
	else if (freq_hz > MAX_FREQ) {
		timer.freq_hz = MAX_FREQ;
	}
	else {
		timer.freq_hz = freq_hz;
	}

	uint div = MAX_FREQ / timer.freq_hz;

	// PIT programmation
	// set divisor selection and repetition mode
	outb(0x43, 0x36);
	// write in canal 0 LSB and MSB of divisor
	outb(0x40, (div & 0xFF));
	outb(0x40, (div >> 8));
}

void timer_handler() {
	timer.ticks++;
	// printf("Ticks : %d\n", timer.ticks);
}

uint get_ticks() {
	return timer.ticks;
}

void sleep(uint ms) {
	uint ticks_count = get_ticks() + (ms / 1000 * timer.freq_hz);
	while (get_ticks() != ticks_count);
}

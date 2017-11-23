#include "base.h"
#include "screen.h"
#include "pio.h"

#define PIT_COMMAND_PORT 			0X43
#define PIT_SELECT_DIV_AND_REPEAT 	0X36
#define PIT_CANAL_0 				0X40

#define MAX_FREQ 	1193180
#define MIN_FREQ 	19

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
	outb(PIT_COMMAND_PORT, PIT_SELECT_DIV_AND_REPEAT);
	// write in canal 0 LSB and MSB of divisor
	outb(PIT_CANAL_0, (div & 0xFF));
	outb(PIT_CANAL_0, (div >> 8));
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

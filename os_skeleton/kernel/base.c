#include "base.h"

void *memset(void *dst, int value, uint count) {
	uchar *str = dst;
	for (uchar i = 0; i < count; i++) {
		str[i] = (uchar) value;
	}
	return dst;
}

// void *memcpy(void *dst, void *src, uint count);

// int strncmp(const char *p, const char *q, uint n);
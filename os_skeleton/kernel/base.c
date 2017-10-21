#include "base.h"

void *memset(void *dst, int value, uint count) {
	uchar *str = dst;
	for (uchar i = 0; i < count; i++) {
		str[i] = (uchar) value;
	}
	return dst;
}

void *memcpy(void *dst, void *src, uint count) {
	char *dp = dst;
	const char *sp = src;
	for (uchar i = 0; i < count; i++) {
		dp[i] = sp[i];
	}
	return dst;
}

int strncmp(const char *p, const char *q, uint n) {
	for (uchar i = 0; i < n; i++) {
		if (p[i] != q[i]) 
			return (int)(p[i] - q[i]);
	}
    return 0;
}
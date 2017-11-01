#include "base.h"

void *memset(void *dst, uint value, uint count) {
	uchar *str = dst;
	for (uchar i = 0; i < count; i++) {
		str[i] = (uchar) value;
	}
	return dst;
}

void *memcpy(void *dst, void *src, uint count) {
	uchar *dp = dst;
	const uchar *sp = src;
	for (uchar i = 0; i < count; i++) {
		dp[i] = sp[i];
	}
	return dst;
}

uint strncmp(const uchar *p, const uchar *q, uint n) {
	for (uchar i = 0; i < n; i++) {
		if (p[i] != q[i]) 
			return (uint)(p[i] - q[i]);
	}
    return 0;
}

void itoa (int x, char* str) {
	int temp = x;
	if (x < 0) {
		temp = -x;
	}
	int i = 0;
	while (temp != 0) {
		str[i] = '0' + temp % 10;
		temp = temp / 10;
		i++;
	}
	if (x < 0) {
		str[i] = '-';
	} else {
		i--;
	}
	//reversing
	for (int j = 0; j < i; j++, i--) {
		char c = str[i];
		str[i] = str[j];
		str[j] = c;
	}
}

void itox (int x, char* str) {
	int temp = x;
	if (x < 0) {
		temp = -x;
	}
	int i = 0;
	while (temp != 0) {
		int r = temp % 16;
		if (r > 9) {
			str[i] = 'a' + r % 10;
		} else {
			str[i] = '0' + r;
		}
		temp = temp / 16;
		i++;
	}
	str[i++] = 'x';
	str[i++] = '0';
	if (x < 0) {
		str[i] = '-';
	} else {
		i--;
	}
	//reversing
	for (int j = 0; j < i; j++, i--) {
		char c = str[i];
		str[i] = str[j];
		str[j] = c;
	}
}

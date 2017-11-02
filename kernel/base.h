#ifndef _BASE_H_
#define _BASE_H_

#include "../common/types.h"

extern void *memset(void *dst, int value, uint count);
extern void *memcpy(void *dst, void *src, uint count);
extern int strncmp(const char *p, const char *q, uint n);
extern void itoa (int x, char* str);
extern void itox (int x, char* str);

#endif

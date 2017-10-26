#ifndef _BASE_H_
#define _BASE_H_

#include "../common/types.h"

void *memset(void *dst, uint value, uint count);
void *memcpy(void *dst, void *src, uint count);
uint strncmp(const uchar *p, const uchar *q, uint n);
void itoa (int x, char* str);
void itox (int x, char* str);

#endif

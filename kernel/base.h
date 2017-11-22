/**
 * @file 		base.h
 * @brief 		Header base functions.
 *
 * @author 		Steven Liatti
 * @author 		Raed Abdennadher
 * @bug 		No known bugs.
 * @date 		November 3, 2017
 * @version		1.0
 */

#ifndef _BASE_H_
#define _BASE_H_

#include "../common/types.h"

#define SCREEN_BUFFER_SIZE 128

extern void *memset(void *dst, int value, uint count);
extern void *memcpy(void *dst, void *src, uint count);
extern int strncmp(const char *p, const char *q, uint n);
extern void itoa (int x, uchar* str);
extern void itox (int x, uchar* str);

#endif

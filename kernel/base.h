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

// int(32) range : (-2^31-1) = -2147483647 to (2^31-1) = 2147483647
// ==> max possible length : 11 digits (-2147483647)
// ==> max length of the buffer used to convert int[] to char[] is 12 char (0 at the end)
#define CONVERT_BUFFER_SIZE 12

extern void *memset(void *dst, int value, uint count);
extern void *memcpy(void *dst, void *src, uint count);
extern int strncmp(const char *p, const char *q, uint n);
extern void itoa (int x, uchar* str);
extern void itox (int x, uchar* str);

#endif

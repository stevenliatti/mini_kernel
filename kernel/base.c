/**
 * @file 		base.c
 * @brief 		Base functions.
 *
 * @author 		Steven Liatti
 * @author 		Raed Abdennadher
 * @bug 		No known bugs.
 * @date 		November 3, 2017
 * @version		1.0
 */

#include "base.h"

/**
 * @brief  fill memory with a constant byte
 * @param  dst   memory area destination
 * @param  value value to copy
 * @param  count bytes count
 */
void *memset(void *dst, int value, uint count) {
	uchar* p=dst;
	while (count--)
		*p++ = (uchar) value;
	return dst;
}

/**
 * @brief  copy memory area
 * 
 * @param  dst   memory area destination
 * @param  src   src are destination
 * @param  count bytes count
 */
void *memcpy(void *dst, void *src, uint count) {
	uchar *dp = dst;
	const uchar *sp = src;
	while (count--)
		*dp++ = *sp++;
	return dst;
}

/**
 * @brief  compare two strings in given length
 * 
 * @param  p first string
 * @param  q second string
 * @param  n first n bytes of string to compare
 * @return   an int less than, equal to or greater than zero
 *           if the n bytes of p are less than, match or greater
 *           than n bytes of q.
 */
int strncmp(const char *p, const char *q, uint n) {
	while (n--)
		if (*p++ != *q++)
			return *(uchar*)(p - 1) - *(uchar*)(q - 1);
	return 0;
}

/**
 * @brief  compare two strings
 *
 * @param  s1 first string
 * @param  s2 second string
 * @return   an int greater than, equal to, or less than zero,
 * 			accordingly as the string pointed to by s1 is greater than,
 * 			equal to, or less than the string pointed to by s2.
 * @source http://clc-wiki.net/wiki/C_standard_library:string.h:strcmp#Implementation
 */
int strcmp(const char* s1, const char* s2)
{
	while(*s1 && (*s1==*s2)) {
		s1++;
		s2++;
	}
	return *(const uchar*)s1-*(const uchar*)s2;
}

/**
 * @brief Add "-" to str if negative number and revert the string
 * 
 * @param x   integer to convert
 * @param str the resulted string
 * @param i   index
 */
static void negate_and_revert(int x, uchar* str, int i) {
	if (x < 0) {
		str[i] = '-';
	} 
	else {
		i--;
	}
	//reversing
	for (int j = 0; j < i; j++, i--) {
		char c = str[i];
		str[i] = str[j];
		str[j] = c;
	}
}

/**
 * @brief clear the buffer given
 * @param buffer the buffer string
 */
static void clear_buffer(uchar* buffer) {
	for (int i = 0; i < CONVERT_BUFFER_SIZE; i++) {
		buffer[i] = 0;
	}
}

/**
 * @brief convert an int in base 10 to string
 * @param x   int to convert
 * @param str string storing the value converted
 */
void itoa(int x, uchar* str) {
	clear_buffer(str);
	if (x == 0) {
		str[0] = '0';
	} 
	else {
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
		negate_and_revert(x, str, i);
	}
}

/**
 * @brief convert an int in base 16 (hexadecimal) to string
 * @param x   int to convert
 * @param str string storing the value converted
 */
void itox(int x, uchar* str) {
	clear_buffer(str);
	if (x == 0) {
		str[0] = '0';
	} 
	else {
		int temp = x;
		if (x < 0) {
			temp = -x;
		}
		int i = 0;
		while (temp != 0) {
			int r = temp % 16;
			if (r > 9) {
				str[i] = 'a' + r % 10;
			} 
			else {
				str[i] = '0' + r;
			}
			temp = temp / 16;
			i++;
		}
		str[i++] = 'x';
		str[i++] = '0';
		negate_and_revert(x, str, i);
	}
}

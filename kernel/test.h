/**
 * @file 		test.h
 * @brief 		Header of test functions.
 *
 * @author 		Steven Liatti
 * @author 		Raed Abdennadher
 * @bug 		No known bugs.
 * @date 		November 25, 2017
 * @version		1.0
 */

#ifndef _TEST_H_
#define _TEST_H_

#ifdef TEST_SCREEN
extern void test_screen();
#endif

#ifdef TEST_TIMER
extern void test_timer();
#endif

#ifdef TEST_FS
extern void test_fs();
#endif

#endif

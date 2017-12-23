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

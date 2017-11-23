#ifdef TEST_SCREEN

#include "screen.h"

/**
 * @brief do screen tests
 */
void test_screen() {
    printf("Screen tests !\n");
    printf("Some tests to check if scroll, theme functions and printf are working\n");
    printf("It begin in few moments ...");
    for(int i = 0; i < 1000000000; i++);
	clr_scr();

	set_theme(LIGHT_GREEN, RED);
	for (int i = 65; i < 91; i++) {
		printf("%s \"%c\" => %d => %x negate %d\n", "Char", i, i, i, -i);
	}

	move_cursor(40, 20);
	printf("Decimal Base : -(%d) = %d", 10, -10);
	move_cursor(40, 21);
	printf("Hexadecimal Base : -(%x) = %x", 10, -10);

	set_theme(BROWN, LIGHT_BLUE);
	move_cursor(40, 10);
	printf("Raed");
	move_cursor(40, 11);
	printf("Abdennadher");
	move_cursor(55, 10);
	printf("Steven");
	move_cursor(55, 11);
	printf("Liatti");

	set_theme(BLACK, YELLOW);
	move_cursor(2, 1);
	scr_xy_t pos = get_cursor_pos();
	printf("get_cursor_pos() = (%d,%d) , the position of 'g' character", pos.x, pos.y);
	move_cursor(2, 2);
	printf("get_bg_color() = %d", get_bg_color());
	move_cursor(2, 3);
	printf("get_fg_color() = %d", get_fg_color());
}
#endif


#ifdef TEST_TIMER

#include "../common/types.h"
#include "timer.h"
#include "screen.h"

void test_timer() {
    int sleeps = 15;
    printf("Timer test !\n");
    printf("%d successive sleep of 1 second are made and ticks shows.\n", sleeps);
    printf("If you change timer frequency, ticks numbers change but the diff\n");
    printf("is (normaly) still the same\n\n");
    for(int i = 0; i < sleeps; i++) {
        int ticks = get_ticks();
        sleep(1000);		
        printf("%d sec, %d (ticks), ticks diff = %d\n", i, get_ticks(), get_ticks() - ticks);
    }
}
    
#endif

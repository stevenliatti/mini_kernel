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
		for(int i = 0; i < 10000000; i++);
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


#ifdef TEST_FS

#include "../common/types.h"
#include "timer.h"
#include "screen.h"
#include "../common/common.h"
#include "ide.h"
#include "fs_api.h"

extern super_block_t sb;
extern int* fat;

void test_fs() {
	print_super_block(sb);
	print_fat(fat, sb.blocks_count);
	sleep(1000);

	file_iterator_t it = file_iterator();
	stat_t st;
	char filename[ENTRY_NAME_SIZE];
	while (file_has_next(&it)) {
		file_next(filename, &it);
		file_stat(filename, &st);
		printf("File name: %s\n", st.name);
		printf("Size: %d bytes      Used blocks nb: %d      Start block: %d\n", st.size, st.used_blocks_nb, st.start);
		sleep(1000);
	}

	char file1[] = "hello.txt";
	if (file_exists(file1)) {
		printf("File \"%s\" exists\n", file1);
	}
	else {
		printf("File \"%s\" doesn't exist\n", file1);
	}
	sleep(1000);
	char file2[] = "x.txt";
	if (file_exists(file2)) {
		printf("File \"%s\" exists\n", file2);
	}
	else {
		printf("File \"%s\" doesn't exist\n", file2);
	}
	sleep(1000);

	int fd1 = -1;
	if ((fd1 = file_open(file1)) == -1) {
		printf("Error in opening file \"%s\"\n", file1);
	}
	else {
		printf("File \"%s\" oppened and fd = %d\n", file1, fd1);
		file_stat(file1, &st);
		printf("File name: %s\n", st.name);
		printf("Size: %d bytes      Used blocks nb: %d      Block size: %d\n", st.size, st.used_blocks_nb, sb.block_size);
	}
	sleep(1000);
	int fd2 = -1;
	if ((fd2 = file_open("raed.txt")) == -1) {
		printf("Error in opening file \"%s\"\n", "raed.txt");
	}
	else {
		printf("File \"%s\" oppened and fd = %d\n", "raed.txt", fd2);
	}
	sleep(1000);
	char str1[514]; // +1 for the character \0
	int bytes = file_read(fd1, str1, 513);
	printf("Readed (%d bytes):\n", bytes);
	printf("%s\n", str1);
	sleep(1000);
	char str2[2001]; // +1 for the character \0
	bytes = file_read(fd1, str2, 2000);
	printf("Readed (%d bytes):\n", bytes);
	printf("%s\n", str2);

	clr_scr();

	int fd3 = -1;
	const int size = 657;
	char str3[31];
	char file3[] = "aaa.txt";
	if ((fd3 = file_open(file3)) == -1) {
		printf("Error in opening file \"%s\"\n", file3);
	}
	bytes = file_read(fd3, str3, 30);
	printf("Readed (%d bytes):\n", bytes);
	printf("%s\n", str3);
	if (file_seek(fd3, 666) == -1) {
		printf("Error in seeking file \"%s\"\n", file3);
	}
	bytes = file_read(fd3, str3, 30);
	printf("Readed (%d bytes):\n", bytes);
	printf("%s\n", str3);
}

#endif

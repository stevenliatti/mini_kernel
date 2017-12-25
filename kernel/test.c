#ifdef TEST_SCREEN

#include "screen.h"

/**
 * @brief do screen tests
 */
void test_screen() {
    printf("Screen tests !\n");
    printf("Some tests to check if scroll, theme functions and printf are working\n");
    printf("it begin in few moments ...");
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

/**
 * @brief do timer tests
 */
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

#include "test_fs_functions.h"
#include "../common/types.h"
#include "timer.h"
#include "screen.h"
#include "../common/common.h"
#include "ide.h"
#include "fs_api.h"
#include "x86.h"
#include "keyboard.h"
#include "test_data.h"

extern super_block_t sb;
extern int* fat;

void print_title(char str[]) {
	set_theme(YELLOW, DEFAULT_BG);
	printf("%s", str);
	set_theme(DEFAULT_FG, DEFAULT_BG);
}

/**
 * @brief do file system tests
 */
void test_fs() {
	set_theme(DEFAULT_FG, DEFAULT_BG);
	printf("Starting test of the file system\n\n");
	set_theme(YELLOW, DEFAULT_BG);
	printf("Note:\n");
	printf("The file system used in these tests have a size of 6656 bytes, with 512 bytes\n");
	printf("per block.\n");
	printf("So it contains 6656 / 512 = 13 blocks (0 indexed). After executing the scenario\n");
	printf("of adding/deleting files (/tools/scenario.sh), the file system will have one\n");
	printf("block reserved for metadata (block 3) and contains 3 files:\n");
	printf("* file 1: \"r.txt\" with data in blocks 3, 4 and 6\n");
	printf("* file 2: \"s.txt\" with data in blocks 5, 7, 8 and 9\n");
	printf("* file 3: \"a.txt\" with data in block 10\n");
	printf("Finally, the fat must indicate that there are two free blocks: 10 and 12.\n\n");

	set_theme(DEFAULT_FG, DEFAULT_BG);
	printf("After each test, press any key to move on to the next test.\n\n");
	printf("Press any key to start\n");
	getc();
	clr_scr();

	print_title("\n-- Test for superblock's fields\n");
	test_super_block();
	getc();

	print_title("\n-- Test for FAT content\n");
	test_fat(expected_fat);
	getc();

	print_title("\n-- Test for iterator's fields after initialisation\n");
	file_iterator_t it = file_iterator();
	test_file_iterator(it, expected_it[0]);
	getc();

	print_title("\n-- Test for file_has_next function\n");
	test_file_has_next(it, expected_bool[0]);
	getc();
	clr_scr();

	if (file_has_next(&it)) {
		int i = 1;
		while (file_has_next(&it)) {
			print_title("\n-- Test for file_has_next, file_next and file_stat functions\n");
			print_title("\n\t-- Test "); printf("%d", i); print_title(" for file_next function\n");
			test_file_next(&it, expected_name[i]);
			print_title("\n\t-- Test "); printf("%d", i); print_title(" for file_stat function\n");
			test_file_stat(expected_name[i], expected_st[i], expected_st_return[i]);
			printf("\n\t* Verify iterator after first call to file_next\n");
			test_file_iterator(it, expected_it[i]);
			printf("\n\t* Verify if file has next\n");
			test_file_has_next(it, expected_bool[i]);
			i++;
			getc();
			clr_scr();
		}
	}

	print_title("\n-- Test for file_exists function\n");
	printf("\t* file_exists(\"%s\"):\n", expected_name[1]);
	test_file_exists(expected_name[1], expected_return[0]);
	printf("\t* file_exists(\"%s\"):\n", expected_name[4]);
	test_file_exists(expected_name[4], expected_return[1]);
	getc();
	clr_scr();

	print_title("\n-- Test for file_open functions return value\n");
	printf("\t* file_open(\"%s\"):\n", expected_name[1]);
	test_file_open(expected_name[1], expected_fd[0]);
	printf("\t* file_open(\"%s\"):\n", expected_name[2]);
	test_file_open(expected_name[2], expected_fd[1]);
	printf("\t* file_open(\"%s\"):\n", expected_name[3]);
	test_file_open(expected_name[3], expected_fd[2]);
	printf("\t* file_close(%d)\n", expected_fd[2]);
	file_close(expected_fd[1]);
	printf("\t* file_open(\"%s\"):\n", expected_name[2]);
	test_file_open(expected_name[1], expected_fd[1]);
	printf("\t* file_close(%d)\n", expected_fd[0]);
	file_close(expected_fd[0]);
	printf("\t* file_close(%d)\n", expected_fd[1]);
	file_close(expected_fd[1]);
	printf("\t* file_close(%d)\n", expected_fd[2]);
	file_close(expected_fd[2]);
	getc();
	clr_scr();

	print_title("\n-- Test for file_read and file_seek functions and file descriptors table\n");
	char filename[] = "r.txt";
	int fd = file_open(filename);
	print_title("\t-- File descriptor "); printf("%d", fd); print_title(" after file_open:\n");
	test_file_descriptior(fd, expected_fd_struct[0]);
	getc();
	clr_scr();
	print_title("-- Read all content of the file "); printf("%s", filename); print_title(":\n");
	stat_t st;
	file_stat(filename, &st);
	test_file_read(fd, st.size, expected_content1, expected_len[0]);
	print_title("\t-- File descriptor "); printf("%d", fd); print_title(" after file_read all content:\n");
	test_file_descriptior(fd, expected_fd_struct[1]);
	getc();
	clr_scr();

	print_title("-- file_seek to position 1017 of the file "); printf("\"%s\"", filename); print_title(" and read 21 character:\n");
	file_seek(fd, 1017);
	print_title("\t-- File descriptor "); printf("%d", fd); print_title(" after file_seek(fd, 1017):\n");
	test_file_descriptior(fd, expected_fd_struct[2]);
	print_title("\t-- Read 21 character:\n");
	test_file_read(fd, 21, expected_content2, expected_len[1]);
	print_title("\t-- File descriptor "); printf("%d", fd); print_title(" after file_read of 21 character:\n");
	test_file_descriptior(fd, expected_fd_struct[3]);

	file_close(fd);

	halt();
}

#endif

/**
 * @file 		test_fs_functions.c
 * @brief 		Functions used in different tests of the file system
 *
 * @author 		Steven Liatti
 * @author 		Raed Abdennadher
 * @bug 		No known bugs.
 * @date 		December 25, 2017
 * @version		1.0
 */

#include "test_fs_functions.h"
#include "screen.h"
#include "../common/common.h"
#include "fs_api.h"
#include "base.h"

#define TEST_NB 13

extern super_block_t sb;
extern int* fat;
extern file_descriptor_t file_descriptor[DESCRIPTORS_NB];

void print_test_num() {
	static int test_num = 0;
	test_num++;
	int last_x = get_cursor_pos().x;
	int last_y = get_cursor_pos().y;
	move_cursor(0, 0);
	set_theme(LIGHT_RED, DEFAULT_BG);
	printf("Test number %d / %d", test_num, TEST_NB);
	set_theme(DEFAULT_FG, DEFAULT_BG);
	move_cursor(last_x, last_y);
}

/**
 * @brief Compare two character-arrays (strings) and print on screen
 *        if the test succeed or fail.
 * 
 * @param char[] the first string
 * @param char[] the second string
 * @param char[] a comment that will be displayed for the test
 */
void assert_char_array(char arg1[], char arg2[], char comment[]) {
	bool success = strcmp(arg1, arg2) == 0;
	printf("\t%s = \"%s\" ==> ", comment, arg1);
	if (success) {
		set_theme(SUCCESS_FG, DEFAULT_BG);
		printf("OK\n");
	} else {
		set_theme(FAILURE_FG, DEFAULT_BG);
		printf("FAILURE\n");
	}
	set_theme(DEFAULT_FG, DEFAULT_BG);
}

/**
 * @brief Compare two big character-arrays (strings) and print on screen
 *        if the test succeed or fail.
 * 
 * @param char[] the first string
 * @param char[] the second string
 * @param char[] a comment that will be displayed for the test
 */
void assert_big_char_array(char arg1[], char arg2[], int len, char comment[]) {
	bool success = strcmp(arg1, arg2) == 0;
	char begin[21];
	char end[21];
	for (int i = 0; i < 20; i++) {
		begin[i] = arg1[i];
	}
	begin[20] = 0;
	for (int i = 0; i < 20; i++) {
		end[19 - i] = arg1[len - i];
	}
	end[20] = 0;
	printf("\t%s = \"%s ... %s\" ==> ", comment, begin, end);
	if (success) {
		set_theme(SUCCESS_FG, DEFAULT_BG);
		printf("OK\n");
	} else {
		set_theme(FAILURE_FG, DEFAULT_BG);
		printf("FAILURE\n");
	}
	set_theme(DEFAULT_FG, DEFAULT_BG);
}

/**
 * @brief Compare two integer (strings) and print on screen
 *        if the test succeed or fail.
 * 
 * @param int the first int
 * @param int the second int
 * @param char[] a comment that will be displayed for the test
 */
void assert_int(int arg1, int arg2, char comment[]) {
	bool success = arg1 == arg2;
	printf("\t%s = %d ==> ", comment, arg1);
	if (success) {
		set_theme(SUCCESS_FG, DEFAULT_BG);
		printf("OK\n");
	} else {
		set_theme(FAILURE_FG, DEFAULT_BG);
		printf("FAILURE\n");
	}
	set_theme(DEFAULT_FG, DEFAULT_BG);
}

/**
 * @brief Compare two integer-arrays and print on screen
 *        if the test succeed or fail.
 * 
 * @param int[] the first int-array
 * @param int[] the second int-array
 * @param char[] a comment that will be displayed for the test
 */
void assert_int_array(int arg1[], int arg2[], int size, char comment[]) {
	bool success = true;
	printf("\t%s = |", comment);
	for (int i = 0; i < size; i++) {
		if (arg1[i] != arg2[i] && success) {
			success = false;
		}
		printf("%d|", arg1[i]);
	}
	printf(" ==> ");
	if (success) {
		set_theme(SUCCESS_FG, DEFAULT_BG);
		printf("OK\n");
	} else {
		set_theme(FAILURE_FG, DEFAULT_BG);
		printf("FAILURE\n");
	}
	set_theme(DEFAULT_FG, DEFAULT_BG);
}

/**
 * @brief Test for the super block
 */
void test_super_block(super_block_t expected_sb) {
	assert_int(sb.magic, expected_sb.magic, "magic");
	assert_int(sb.version, expected_sb.version, "version");
	assert_char_array(sb.label, expected_sb.label, "label");
	assert_int(sb.block_size, expected_sb.block_size, "block_size");
	assert_int(sb.blocks_count, expected_sb.blocks_count, "blocks_count");
	assert_int(sb.fat_block_nb, expected_sb.fat_block_nb, "fat_block_nb");
	assert_int(sb.first_entry, expected_sb.first_entry, "first_entry");
}

/**
 * @brief Test for the FAT
 * 
 * @param int[] the expected content of the FAT
 */
void test_fat(int expected_fat[]) {
	assert_int_array(fat, expected_fat, sb.blocks_count, "fat");
}

/**
 * @brief Test for the file iterator
 * 
 * @param file_iterator_t the file iteratior of the test
 * @param file_iterator_t the expected file iterator struct
 */
void test_file_iterator(file_iterator_t it, file_iterator_t expected_it) {
	assert_int(it.current_block, expected_it.current_block, "current_block");
	assert_int(it.entry_offset_in_current_block, expected_it.entry_offset_in_current_block, 
		"entry_offset_in_current_block");
}

/**
 * @brief Test for the file_has_next function
 * 
 * @param file_iterator_t the file iteratior of the test
 * @param bool the expected return of the file_has_next function
 */
void test_file_has_next(file_iterator_t it, bool expected_return) {
	bool has_next = file_has_next(&it);
	assert_int(has_next, expected_return, "file has next");
}

/**
 * @brief Test for the file_next function
 * 
 * @param file_iterator_t* the file iteratior of the test
 * @param char[] the expected file name to be returned in argument 
 *               of the file_next function
 */
void test_file_next(file_iterator_t* it, char expected_name[]) {
	char name[ENTRY_NAME_SIZE];
	file_next(name, it);
	assert_char_array(name, expected_name, "file name");
}

/**
 * @brief Test for the file_stat function
 * 
 * @param char[] the name of the file to be searched
 * @param stat_t the expected stat_t struct returnred in argument
 *               of file_stat function
 * @param int the expected returned value of file_stat function
 */
void test_file_stat(char filename[], stat_t expected_st, int expected_return) {
	stat_t st;
	int ret = file_stat(filename, &st);
	assert_int(st.size, expected_st.size, "size");
	assert_int(st.used_blocks_nb, expected_st.used_blocks_nb, "used_blocks_nb");
	assert_int(st.start, expected_st.start, "start");
	assert_int(ret, expected_return, "returns");
}

/**
 * @brief Test for the file_exists function
 * 
 * @param char[] the name of the file to be searched
 * @param int the expected returned value of file_exists function
 */
void test_file_exists(char filename[], bool expected_return) {
	bool ret = file_exists(filename);
	assert_int(ret, expected_return, "returns");
}

/**
 * @brief Test for the file_exists function
 * 
 * @param char[] the name of the file to be opened
 * @param int the expected file descriptor value of file_open function
 */
void test_file_open(char filename[], int expected_fd) {
	int fd = file_open(filename);
	assert_int(fd, expected_fd, "returns");
}

/**
 * @brief Test for the file descriptors table
 * 
 * @param int the file descriptor used in test
 * @param file_descriptor_t the expected file descriptor struct
 */
void test_file_descriptior(int fd, file_descriptor_t expected_fd_struct) {
	assert_int(file_descriptor[fd].start_block, expected_fd_struct.start_block, "start_block");
	assert_int(file_descriptor[fd].current_offset_in_block,
		expected_fd_struct.current_offset_in_block, "current_offset_in_block");
	assert_int(file_descriptor[fd].current_block, expected_fd_struct.current_block, "current_block");
	assert_int(file_descriptor[fd].file_size, expected_fd_struct.file_size, "file_size");
	assert_int(file_descriptor[fd].is_free, expected_fd_struct.is_free, "is_free");
	assert_int(file_descriptor[fd].readed_bytes, expected_fd_struct.readed_bytes, "readed_bytes");
}

/**
 * @brief Test for the file_read function
 * 
 * @param int the file descriptor used in test
 * @param int the bytes count demanded to be readed
 * @param char[] the expected content readed by the file_read function
 * @param int the expected value returned by the file_read function
 */
void test_file_read(int fd, int size, char expected_content[], int expected_len) {
	char buf[size + 1];
	int bytes_len = file_read(fd, buf, size);
	buf[size] = 0;
	assert_int(bytes_len, expected_len, "returns");
	if (bytes_len > 70) {
		assert_big_char_array(buf, expected_content, bytes_len, "content");
	} else {
		assert_char_array(buf, expected_content, "content");
	}
}

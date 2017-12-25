#include "test_fs_functions.h"
#include "screen.h"
#include "../common/common.h"
#include "fs_api.h"
#include "base.h"

extern super_block_t sb;
extern int* fat;
extern file_descriptor_t file_descriptor[DESCRIPTORS_NB];

void assert_char_array(char* arg1, char* arg2, char comment[]) {
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

void assert_big_char_array(char* arg1, char* arg2, int len, char comment[]) {
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

void assert_int_array(int* arg1, int* arg2, int size, char comment[]) {
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

void test_super_block() {
	assert_int(sb.magic, 66, "magic");
	assert_int(sb.version, 1, "version");
	char expected_label[LABEL_SIZE] = "doge_fs";
	assert_char_array(sb.label, expected_label, "label");
	assert_int(sb.block_size, 512, "block_size");
	assert_int(sb.blocks_count, 13, "blocks_count");
	assert_int(sb.fat_block_nb, 1, "fat_block_nb");
	assert_int(sb.first_entry, 2, "first_entry");
}

void test_fat(int* expected_fat) {
	assert_int_array(fat, expected_fat, sb.blocks_count, "fat");
}

void test_file_iterator(file_iterator_t it, file_iterator_t expected_it) {
	assert_int(it.current_block, expected_it.current_block, "current_block");
	assert_int(it.entry_offset_in_current_block, expected_it.entry_offset_in_current_block, "entry_offset_in_current_block");
}

void test_file_has_next(file_iterator_t it, bool expected_return) {
	bool has_next = file_has_next(&it);
	assert_int(has_next, expected_return, "file has next");
}

void test_file_next(file_iterator_t* it, char expected_name[]) {
	char name[ENTRY_NAME_SIZE];
	file_next(name, it);
	assert_char_array(name, expected_name, "file name");
}

void test_file_stat(char expected_name[], stat_t expected_st, int expected_return) {
	stat_t st;
	int ret = file_stat(expected_name, &st);
	assert_int(st.size, expected_st.size, "size");
	assert_int(st.used_blocks_nb, expected_st.used_blocks_nb, "used_blocks_nb");
	assert_int(st.start, expected_st.start, "start");
	assert_int(ret, expected_return, "returns");
}

void test_file_exists(char filename[], bool expected_return) {
	bool ret = file_exists(filename);
	assert_int(ret, expected_return, "returns");
}

void test_file_open(char filename[], int expected_fd) {
	int fd = file_open(filename);
	assert_int(fd, expected_fd, "returns");
}

void test_file_descriptior(int fd, file_descriptor_t expected_fd_struct) {
	assert_int(file_descriptor[fd].start_block, expected_fd_struct.start_block, "start_block");
	assert_int(file_descriptor[fd].current_offset_in_block, expected_fd_struct.current_offset_in_block, "current_offset_in_block");
	assert_int(file_descriptor[fd].current_block, expected_fd_struct.current_block, "current_block");
	assert_int(file_descriptor[fd].file_size, expected_fd_struct.file_size, "file_size");
	assert_int(file_descriptor[fd].is_free, expected_fd_struct.is_free, "is_free");
	assert_int(file_descriptor[fd].readed_bytes, expected_fd_struct.readed_bytes, "readed_bytes");
}

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
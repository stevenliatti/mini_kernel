/**
 * @file 		test_data.h
 * @brief 		Header of functions used in different tests of the file system
 *
 * @author 		Steven Liatti
 * @author 		Raed Abdennadher
 * @bug 		No known bugs.
 * @date 		November 25, 2017
 * @version		1.0
 */

#ifndef _TEST_FS_FUNCTIONS_H_
#define _TEST_FS_FUNCTIONS_H_

#include "fs_api.h"

#define DEFAULT_BG 	BLACK
#define DEFAULT_FG 	LIGHT_GRAY
#define SUCCESS_FG 	GREEN
#define FAILURE_FG	RED

extern void print_test_num();

extern void test_super_block(super_block_t expected_sb);
extern void test_fat(int* expected_fat);
extern void test_file_iterator(file_iterator_t it, file_iterator_t expected_it);
extern void test_file_has_next(file_iterator_t it, bool expected_result);
extern void test_file_next(file_iterator_t* it, char expected_name[]);
extern void test_file_stat(char expected_name[], stat_t expected_st, int expected_return);
extern void test_file_exists(char filename[], bool expected_return);
extern void test_file_open(char filename[], int expected_fd);
extern void test_file_descriptior(int fd, file_descriptor_t expected_fd_struct);
extern void test_file_read(int fd, int size, char expected_content[], int expected_len);

#endif

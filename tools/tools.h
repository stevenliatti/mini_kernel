/**
 * @file 		tools.h
 * @brief 		Header of functions for the file system's tools.
 *
 * @author 		Steven Liatti
 * @author 		Raed Abdennadher
 * @bug 		No known bugs.
 * @date 		December 16, 2017
 * @version		1.0
 */

#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>
#include "../common/common.h"

int get_file_size(const char * file_name);
int load_fat(FILE* fd, super_block_t* sb, int** fat);
int load_super_block(FILE* fd, super_block_t** sb);
int valid_fs_name(char* fs_name);

#endif

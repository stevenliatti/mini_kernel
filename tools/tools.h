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

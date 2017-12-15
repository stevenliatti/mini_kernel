#ifndef _FS_H_
#define _FS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>

#define CHECK_ERR(expr, ...) if (expr) { fprintf(stderr, __VA_ARGS__); return EXIT_FAILURE; }

int get_file_size(const char * file_name);

#endif
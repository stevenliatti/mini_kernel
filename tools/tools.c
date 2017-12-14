#include "tools.h"
#include <sys/stat.h>

int get_file_size(const char * file_name) {
	struct stat sb;
	CHECK_ERR(stat(file_name, & sb) != 0, "error: stat failed for \"%s\"\n", file_name)
	return sb.st_size;
}

void print_fat(int* fat, int fat_len) {
	printf("fat table: \n|");
	for (int i = 0; i < fat_len; i++) {
		printf("%d|", fat[i]);
	}
	printf("\n");
}
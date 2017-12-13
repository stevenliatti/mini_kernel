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

void print_super_block(super_block_t* super_block) {
	printf("super_block : \n");
	printf("\tmagic: %x\n", super_block->magic);
	printf("\tversion: %d\n", super_block->version);
	printf("\tlabel: %s\n", super_block->label);
	printf("\tblock_size: %d\n", super_block->block_size);
	printf("\tfat_len: %d\n", super_block->fat_len);
	printf("\tfat_block_nb: %d\n", super_block->fat_block_nb);
	printf("\tfirst_dir_entry: %d\n", super_block->first_dir_entry);
}
#ifdef LIBC
#include <stdio.h>
#endif

#ifdef KERNEL
#include "../kernel/screen.h"
#endif

#include "common.h"

void print_super_block(super_block_t* super_block) {
	printf("super_block : \n");
	printf("    magic: %x\n", super_block->magic);
	printf("    version: %d\n", super_block->version);
	printf("    label: %s\n", super_block->label);
	printf("    block_size: %d\n", super_block->block_size);
	printf("    fat_len: %d\n", super_block->fat_len);
	printf("    fat_block_nb: %d\n", super_block->fat_block_nb);
	printf("    first_dir_entry: %d\n", super_block->first_dir_entry);
}

void print_fat(int* fat, int fat_len) {
	printf("fat table: \n|");
	for (int i = 0; i < fat_len; i++) {
		printf("%d|", fat[i]);
	}
	printf("\n");
}
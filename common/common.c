#ifdef LIBC
#include <stdio.h>
#endif

#ifdef KERNEL
#include "../kernel/screen.h"
#endif

#include "common.h"

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

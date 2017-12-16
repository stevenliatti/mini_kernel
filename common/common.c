/**
 * @file 		common.c
 * @brief 		Functions in common for the file system's tools and kernel.
 * 				There is two rules associated at this file in makefile, one 
 *				to compile the file with stdio for tools files and the other 
 *				to compile the file with screen.h for kernel files.
 *
 * @author 		Steven Liatti
 * @author 		Raed Abdennadher
 * @bug 		No known bugs.
 * @date 		December 16, 2017
 * @version		1.0
 */

#ifdef LIBC
#include <stdio.h>
#endif

#ifdef KERNEL
#include "../kernel/screen.h"
#endif

#include "common.h"

/**
 * @brief  This function the given super block.
 *
 * @param  super_block the super block
 */
void print_super_block(super_block_t* super_block) {
	printf("super_block : \n");
	printf("    magic: %x\n", super_block->magic);
	printf("    version: %d\n", super_block->version);
	printf("    label: %s\n", super_block->label);
	printf("    block_size: %d\n", super_block->block_size);
	printf("    fat_len: %d\n", super_block->fat_len);
	printf("    fat_block_nb: %d\n", super_block->fat_block_nb);
	printf("    first_entry: %d\n", super_block->first_entry);
}

/**
 * @brief  This function the given fat.
 *
 * @param  fat our fat
 * @param  fat_len the size of fat
 */
void print_fat(int* fat, int fat_len) {
	printf("fat table: \n|");
	for (int i = 0; i < fat_len; i++) {
		printf("%d|", fat[i]);
	}
	printf("\n");
}

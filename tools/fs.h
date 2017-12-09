#ifndef _FS_H_
#define _FS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LABEL_SIZE 20 // TODO: fixé au bol
// 24 + 2 * sizeof(int) in the dir_entry_t struct = 32
// so we can store (block_size / 32) entry in a block
#define ENTRY_NAME_SIZE 24

#define CHECK_ERR(expr, ...) if (expr) { fprintf(stderr, __VA_ARGS__); return EXIT_FAILURE; }

typedef struct dir_entry_st {
	char name[ENTRY_NAME_SIZE];
	int size;
	int start;
} __attribute__((packed)) dir_entry_t;

typedef struct super_block_st {
	char magic;
	char version;
	char label[LABEL_SIZE];
	int block_size;
	int fat_len;
	int fat_block_nb;
	int first_dir_entry;
} __attribute__((packed)) super_block_t;


void display_super_block(super_block_t* super_block) {
	printf("super_block : \n");
	printf("\tmagic: %x\n", super_block->magic);
	printf("\tversion: %d\n", super_block->version);
	printf("\tlabel: %s\n", super_block->label);
	printf("\tblock_size: %d\n", super_block->block_size);
	printf("\tfat_len: %d\n", super_block->fat_len);
	printf("\tfat_block_nb: %d\n", super_block->fat_block_nb);
	printf("\tfirst_dir_entry: %d\n", super_block->first_dir_entry);
}

#endif
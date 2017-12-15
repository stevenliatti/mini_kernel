#ifndef _COMMON_H_
#define _COMMON_H_

// 24 + 2 * sizeof(int) in the dir_entry_t struct = 32
// so we can store (block_size / 32) entry in a block
#define ENTRY_NAME_SIZE 24
#define LABEL_SIZE 20 // TODO: fixé au bol

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

void print_super_block(super_block_t* super_block);
void print_fat(int* fat, int len);

#endif
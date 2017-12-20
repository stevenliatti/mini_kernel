#ifndef _COMMON_H_
#define _COMMON_H_

#define LABEL_SIZE 20
// 24 + 2 * sizeof(int) in the entry_t struct = 32
// so we can store (block_size / 32) entry in a block
#define ENTRY_NAME_SIZE 24

#define CHECK_ERR(expr, ...) if (expr) { fprintf(stderr, __VA_ARGS__); return EXIT_FAILURE; }

/**
 * @brief This structure describe an entry in block of metadata.
 * It contains the file name, his size and the first block of his data.
 */
typedef struct entry_st {
	char name[ENTRY_NAME_SIZE];
	int size;
	int start;
} __attribute__((packed)) entry_t;

/**
 * @brief This structure describe the super block.
 * We find the magic number, the version, the name (label), the block's size in bytes,
 * the total size blocks of FAT, the block occupied by FAT and the index of first block 
 * of metadata.
 */
typedef struct super_block_st {
	char magic;
	char version;
	char label[LABEL_SIZE];
	int block_size;
	int blocks_count;
	int fat_block_nb;
	int first_entry;
} __attribute__((packed)) super_block_t;

void print_super_block(super_block_t super_block);
void print_fat(int* fat, int len);

#endif

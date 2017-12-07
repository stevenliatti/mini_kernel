#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "../common/types.h"

#define FS_MAGIC 0x66
#define FS_VERSION 1
#define LABEL_SIZE 20 // fixé au bol
#define MIN_BLOCK_SIZE 512
#define MAX_BLOCK_SIZE (8 * MIN_BLOCK_SIZE)

typedef struct dir_entry_st {
	char* name;
	int size;
	int start;
} __attribute__((packed)) dir_entry_t;

typedef struct super_block_st {
	char magic;
	char version;
	char label[LABEL_SIZE];
	int block_size;
} __attribute__((packed)) super_block_t;


int main(int argc, char *argv[]) {
	int block_size = atoi(argv[2]);
	int file_size = atoi(argv[4]);
	if (argc == 5) {
		// check the label
		if (strlen(argv[1]) > LABEL_SIZE) {
			fprintf(stderr, "error: Label size must be less or equal than %d\n", LABEL_SIZE);
			return EXIT_FAILURE;
		}
		// check the block size
		if (block_size % MIN_BLOCK_SIZE != 0 || block_size > MAX_BLOCK_SIZE || block_size < MIN_BLOCK_SIZE) {
			fprintf(stderr, "error: Block size must be a multiple of %d and less than %d ", MIN_BLOCK_SIZE, MAX_BLOCK_SIZE);
			fprintf(stderr, "and greater or equal than %d\n", MIN_BLOCK_SIZE);
			return EXIT_FAILURE;
		}
	    //check the file size
	    if (file_size < block_size || (file_size % block_size != 0)) {
	    	fprintf(stderr, "error: File size must be greater than the block size ");
	    	fprintf(stderr, "and multiple of block_size\n");
	    	return EXIT_FAILURE;
	    }
		// check the file name
		FILE* fd = fopen(argv[3], "r");
	    if (fd != NULL) {// file already exist
			fprintf(stderr, "error: File with name \"%s\" already exist\n", argv[3]);
			fclose(fd);
			return EXIT_FAILURE;
	    }

		super_block_t* super_block = malloc(sizeof(super_block_t));

		super_block->magic = FS_MAGIC;
		super_block->version = FS_VERSION;
		strcpy(super_block->label, argv[1]);
		super_block->block_size = block_size;

		printf("super_block : \n\tmagic: %x\n\tversion: %d\n\tlabel: %s\n\tblock_size: %d\n",
			super_block->magic, super_block->version, super_block->label, super_block->block_size);

		fd = fopen( argv[3] , "w" );
		fwrite(super_block, sizeof(super_block_t), 1, fd);

		char* rest = calloc(block_size - sizeof(super_block_t), sizeof(char));
		fwrite(rest, sizeof(char), block_size - sizeof(super_block_t), fd);

		int rest_block_nb = file_size / block_size - 1;
		printf("rest_block_nb : %d\n", rest_block_nb);
		rest = calloc(rest_block_nb, sizeof(char) * block_size);
		fwrite(rest, sizeof(char), rest_block_nb * block_size, fd);

		fclose(fd);

		return EXIT_SUCCESS;

	} else {

		fprintf(stderr, "\nfs_create <label> <block_size> <file_name> <file_size>\n\n"
					"• label : Label of the file system\n"
					"• block_size : Must be a multiple of 512\n"
					"• file_name : Name of the file system image\n"
					"• file_size : Size of the file system image in Byte\n\n"
					"Example: fs_create doge_fs 1024 disk.img 1000000\n\n");
		return EXIT_FAILURE;
	}
}
/**
 * @file 		fs_create.c
 * @brief 		Tool for creating an image file system.
 *
 * @author 		Steven Liatti
 * @author 		Raed Abdennadher
 * @bug 		No known bugs.
 * @date 		December 16, 2017
 * @version		1.0
 */

#include "tools.h"
#include "../common/common.h"

#define FS_MAGIC 0x42
#define FS_VERSION 1
#define MIN_BLOCK_SIZE 512
#define MAX_BLOCK_SIZE (8 * MIN_BLOCK_SIZE)

/**
 * @brief  This function check if the arguments are correct.
 *
 * @param  label the label of file system
 * @param  block_size the block's size in bytes
 * @param  file_size the file's size
 * @param  file_name the file's name
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
static int valid_arguments(char* label, int block_size, int file_size, char* file_name) {
	CHECK_ERR(strlen(label) > LABEL_SIZE, "Error: Label size must be less or equal than %d\n",
	LABEL_SIZE)

	CHECK_ERR(block_size % MIN_BLOCK_SIZE != 0 || block_size > MAX_BLOCK_SIZE ||
		block_size < MIN_BLOCK_SIZE,
		"Error: Block size must be a multiple of %d and less than %d\n"
		"and greater or equal than %d\n",
		MIN_BLOCK_SIZE, MAX_BLOCK_SIZE, MIN_BLOCK_SIZE)

	CHECK_ERR(file_size < block_size * 4 || (file_size % block_size != 0),
		"Error: File size must be greater than the (%d * 4) "
		"and multiple of block size, because at least, there will be "
		"a super block, a fat block, a main dir entry block and a data block\n",
		block_size)

	FILE* fd = fopen(file_name, "r");
	if (fd != NULL) {
		fprintf(stderr, "Error: File with name \"%s\" already exist\n", file_name);
		fclose(fd);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

/**
 * @brief  This function write on image file the super block.
 *
 * @param  label the label of file system
 * @param  block_size the block's size in bytes
 * @param  fat_block_nb the block's number occupied by the fat
 * @param  blocks_count the block's number total referenced by fat (also the final total of blocks)
 * @param  first_entry_index the block where the first meta data begins
 * @param  fd the file descriptor of image file
 * @param  total a variable to store the total bytes of file system
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
static int fwrite_super_block(char* label, int block_size, int fat_block_nb, int blocks_count,
		int first_entry_index, FILE* fd, int* total) {
	super_block_t* super_block = calloc(1, sizeof(super_block_t));
	CHECK_ERR(super_block == NULL, "Failure in allocating memory!!\n")

	super_block->magic = FS_MAGIC;
	super_block->version = FS_VERSION;
	strcpy(super_block->label, label);
	super_block->block_size = block_size;
	super_block->blocks_count = blocks_count;
	super_block->fat_block_nb = fat_block_nb;
	super_block->first_entry = first_entry_index;

	print_super_block(super_block);
	CHECK_ERR(fwrite(super_block, sizeof(super_block_t), 1, fd) == 0,"Failure in writing data!!\n")

	printf("super block occupied size (bytes): %d\n", sizeof(super_block_t));
	*total += sizeof(super_block_t);

	int rest_size = block_size - sizeof(super_block_t);
	char* rest = calloc(rest_size, sizeof(char));
	CHECK_ERR(rest == NULL, "Failure in allocating memory!!\n")
	CHECK_ERR(fwrite(rest, sizeof(char), rest_size, fd) == 0, "Failure in writing data!\n")

	printf("super block remaining bytes: %d\n", rest_size * sizeof(char));
	*total += rest_size * sizeof(char);
	return EXIT_SUCCESS;
}

/**
 * @brief  This function write on image file the fat.
 *
 * @param  total_block_nb the total block's number (fat len)
 * @param  block_size the block's size in bytes
 * @param  first_entry_index the block where the first meta data begins
 * @param  fd the file descriptor of image file
 * @param  total a variable to store the total bytes of file system
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
static int fwrite_fat_blocks(int total_block_nb, int block_size, int first_entry_index,
	FILE* fd, int* total) {
	int blocks_count = total_block_nb;
	int fat_occupied_bytes = blocks_count * sizeof(int);
	int* fat = calloc(blocks_count, sizeof(int));
	CHECK_ERR(fat == NULL, "Failure in allocating memory!!\n")

	for (int i = 0; i < blocks_count; i++) { fat[i] = 0xffffffff; }

	fat[first_entry_index] = 0;
	CHECK_ERR(fwrite(fat, sizeof(int), blocks_count, fd) == 0, "Failure in writing data!!\n")

	printf("fat size (bytes): %d\n", fat_occupied_bytes);
	*total += fat_occupied_bytes;

	int rest_size = (block_size - fat_occupied_bytes % block_size) % block_size;
	if (rest_size != 0) {
		char* rest = calloc(rest_size, sizeof(char));
		CHECK_ERR(rest == NULL, "Failure in allocating memory!!\n")
		CHECK_ERR(fwrite(rest, sizeof(char), rest_size, fd) == 0, "Failure in writing data!\n")
	}

	printf("rest fat (bytes): %d\n", rest_size * sizeof(char));
	*total += rest_size * sizeof(char);
	return EXIT_SUCCESS;
}

/**
 * @brief  This function compute the needed blocks for fat.
 *
 * @param  blocks_count the total block's number (fat len)
 * @param  block_size the block's size in bytes
 * @return the needed blocks for fat
 */
static int compute_fat_block_nb(int blocks_count, int block_size) {
	// why int? because the size (int) of the file system can be up to +2147483136 bytes (it is a signed int)
	// and if a size block = 512 so 2147483136 / 512 = 4194303 (0x3FFFFF) ==> 24 bits
	int fat_occupied_bytes = blocks_count * sizeof(int);
	int fat_block_nb = 1;
	int div = fat_occupied_bytes / block_size;
	if (div != 0) {
		fat_block_nb = div;
		int mod = fat_occupied_bytes % block_size;
		if (mod != 0) {
			fat_block_nb++;
		}
	}
	return fat_block_nb;
}

/**
 * @brief  Entry point of program, load differents structures and create the fs.
 *
 * @param  argc the number of arguments
 * @param  argc the array of arguments
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
int main(int argc, char *argv[]) {
	if (argc == 5) {
		const int block_size = atoi(argv[2]);
		const int file_size = atoi(argv[4]);
		if (valid_arguments(argv[1], block_size, file_size, argv[3])) {
			return EXIT_FAILURE;
		}

		const int total_block_nb = file_size / block_size;
		const int fat_block_nb = compute_fat_block_nb(total_block_nb, block_size);
		const int first_entry_index = fat_block_nb + 1;
		int total = 0;

		FILE* fd = fopen(argv[3], "w");
		CHECK_ERR(fd == NULL, "Failure in opening file \"%s\" (in w mode)!\n", argv[3])

		// write super block in the first block and write 0 in the rest of
		// the block (of size block_size)
		CHECK_ERR(fwrite_super_block(argv[1], block_size, fat_block_nb, total_block_nb,
			first_entry_index, fd, &total), "Failure in writing the super block!\n")

		// write fat in the second block. If the size of the fat is greater than block_size
		// then extend fat to the next block and write 0 in the rest of the last block
		CHECK_ERR(fwrite_fat_blocks(total_block_nb, block_size, first_entry_index, fd, &total),
			"Failure in writing the fat blocks\n")

		const int rest_total_block_nb = total_block_nb - 1 - fat_block_nb;
		printf("rest total block nb: %d\n", rest_total_block_nb);
		char* rest = calloc(rest_total_block_nb, sizeof(char) * block_size);
		CHECK_ERR(rest == NULL, "Failure in allocating memory!!\n")
		CHECK_ERR(fwrite(rest, sizeof(char), rest_total_block_nb * block_size, fd) == 0,
			"Failure in writing data!\n")

		printf("rest of all fs (bytes): %d\n", rest_total_block_nb * block_size * sizeof(char));
		total += rest_total_block_nb * block_size * sizeof(char);
		fclose(fd);
		printf("Total (bytes): %d\n", total);
		printf("Total (blocks): %d\n", total_block_nb);
		return EXIT_SUCCESS;
	}
	else {
		fprintf(stderr, "\nfs_create <label> <block_size> <file_name> <file_size>\n\n"
			"• label : Label of the file system\n"
			"• block_size : Must be a multiple of 512\n"
			"• file_name : Name of the file system image\n"
			"• file_size : Size of the file system image in Byte\n\n"
			"Example: fs_create doge_fs 512 fs.img 1000448\n\n");
		return EXIT_FAILURE;
	}
}

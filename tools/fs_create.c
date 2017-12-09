#include "fs.h"

#define FS_MAGIC 0x66
#define FS_VERSION 1
#define MIN_BLOCK_SIZE 512
#define MAX_BLOCK_SIZE (8 * MIN_BLOCK_SIZE)

static int valid_arguments(char* label, int block_size, int file_size, char* file_name) {
	// check the label
	CHECK_ERR(strlen(label) > LABEL_SIZE, "error: Label size must be less or equal than %d\n", 
	LABEL_SIZE)

	// check the block size
	CHECK_ERR(block_size % MIN_BLOCK_SIZE != 0 || block_size > MAX_BLOCK_SIZE || 
		block_size < MIN_BLOCK_SIZE, 
		"error: Block size must be a multiple of %d and less than %d\n \
		and greater or equal than %d\n",
		MIN_BLOCK_SIZE, MAX_BLOCK_SIZE, MIN_BLOCK_SIZE)

	// check the file size
	CHECK_ERR(file_size < block_size * 4 || (file_size % block_size != 0), 
		"error: File size must be greater than the (%d * 4) \
		and multiple of block size, because at least, there will be \
		a super block, a fat block, a main dir entry block and a data block\n",
		block_size)

	// check the file name
	FILE* fd = fopen(file_name, "r");
	if (fd != NULL) {// file already exist
		fprintf(stderr, "error: File with name \"%s\" already exist\n", file_name);
		fclose(fd);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

static int fwrite_super_block(char* label, int block_size, int fat_block_nb, int fat_len,
		int first_dir_entry_index, FILE* fd, int* total) {	
	super_block_t* super_block = calloc(1, sizeof(super_block_t));
	CHECK_ERR(super_block == NULL, "Failure in allocating memory!!\n")

	super_block->magic = FS_MAGIC;
	super_block->version = FS_VERSION;
	strcpy(super_block->label, label);
	super_block->block_size = block_size;
	super_block->fat_len = fat_len;
	super_block->fat_block_nb = fat_block_nb;
	super_block->first_dir_entry = first_dir_entry_index;

	display_super_block(super_block);
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

static int fwrite_fat_blocks(int total_block_nb, int block_size, int first_dir_entry_index, 
	FILE* fd, int* total) {
	int fat_len = total_block_nb;
	int fat_occupied_bytes = fat_len * sizeof(int);
	int* fat = calloc(fat_len, sizeof(int));
	CHECK_ERR(fat == NULL, "Failure in allocating memory!!\n")
	
	for (int i = 0; i < fat_len; i++) { fat[i] = 0xffffffff; }

	fat[first_dir_entry_index] = 0;
	CHECK_ERR(fwrite(fat, sizeof(int), fat_len, fd) == 0, "Failure in writing data!!\n")

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

static int compute_fat_block_nb(int fat_len, int block_size) {
	// why int? because the file system can be 2^32 bytes size and if a size block = 512
	// so 2^32 / 512 = 8388608 (0x800000) ==> 24 bits
	int fat_occupied_bytes = fat_len * sizeof(int);
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

int main(int argc, char *argv[]) {
	if (argc == 5) {
		const int block_size = atoi(argv[2]);
		const int file_size = atoi(argv[4]);
		if (valid_arguments(argv[1], block_size, file_size, argv[3])) {
			return EXIT_FAILURE;
		}

		const int total_block_nb = file_size / block_size;

		// fat blocks number
		// const int fat_len = total_block_nb;
		const int fat_block_nb = compute_fat_block_nb(total_block_nb, block_size);

		const int first_dir_entry_index = fat_block_nb + 1;
		int total = 0;

		FILE* fd = fopen(argv[3], "w");
		CHECK_ERR(fd == NULL, "Failure in opening file \"%s\" (in w mode)!\n", argv[3])

		// write super block in the first block and write 0 in the rest of 
		// the block (of size block_size)
		CHECK_ERR(fwrite_super_block(argv[1], block_size, fat_block_nb, total_block_nb, 
		first_dir_entry_index, fd, &total), "Failure in writing the super block!\n")

		// write fat in the second block. If the size of the fat is greater than block_size
		// then extend fat to the next block and write 0 in the rest of the last block
		CHECK_ERR(fwrite_fat_blocks(total_block_nb, block_size, first_dir_entry_index, fd, &total),
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

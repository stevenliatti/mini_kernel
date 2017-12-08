#include "fs.h"

#define FS_MAGIC 0x66
#define FS_VERSION 1
#define MIN_BLOCK_SIZE 512
#define MAX_BLOCK_SIZE (8 * MIN_BLOCK_SIZE)

int valid_arguments(char* label, int block_size, int file_size, char* file_name) {
	// check the label
	if (strlen(label) > LABEL_SIZE) {
		fprintf(stderr, "error: Label size must be less or equal than %d\n", LABEL_SIZE);
		return 0;
	}
	// check the block size
	if (block_size % MIN_BLOCK_SIZE != 0 || block_size > MAX_BLOCK_SIZE || block_size < MIN_BLOCK_SIZE) {
		fprintf(stderr, "error: Block size must be a multiple of %d and less than %d ", MIN_BLOCK_SIZE, MAX_BLOCK_SIZE);
		fprintf(stderr, "and greater or equal than %d\n", MIN_BLOCK_SIZE);
		return 0;
	}
	//check the file size
	if (file_size < block_size * 4 || (file_size % block_size != 0)) {
		fprintf(stderr, "error: File size must be greater than the (block size * 4)");
		fprintf(stderr, "and multiple of block size, because at least, there will be ");
		fprintf(stderr, "a super block, a fat block, a main dir entry block and a data block\n");
		return 0;
	}
	// check the file name
	FILE* fd = fopen(file_name, "r");
	if (fd != NULL) {// file already exist
		fprintf(stderr, "error: File with name \"%s\" already exist\n", file_name);
		fclose(fd);
		return 0;
	}
	return 1;
}

int fwrite_super_block(char* label, int block_size, int fat_block_nb, int fat_len,
		int first_dir_entry_index, FILE* fd, int* total) {	
	super_block_t* super_block = calloc(1, sizeof(super_block_t));
	if (super_block == NULL) {
		fprintf(stderr, "Failure in allocating memory!!\n");
		return 0;
	}

	super_block->magic = FS_MAGIC;
	super_block->version = FS_VERSION;
	strcpy(super_block->label, label);
	super_block->block_size = block_size;
	super_block->fat_len = fat_len;
	super_block->fat_block_nb = fat_block_nb;
	super_block->first_dir_entry = first_dir_entry_index;

	display_super_block(super_block);

	if (fwrite(super_block, sizeof(super_block_t), 1, fd) == 0) {
		fprintf(stderr, "Failure in writing data!!\n");
		return 0;
	}

	printf("super block occuped size: %d\n", sizeof(super_block_t));
	*total += sizeof(super_block_t);

	int rest_size = block_size - sizeof(super_block_t);
	char* rest = calloc(rest_size, sizeof(char));
	if (rest == NULL) {
		fprintf(stderr, "Failure in allocating memory!!\n");
		return 0;
	}
	if (fwrite(rest, sizeof(char), rest_size, fd) == 0) {
		fprintf(stderr, "Failure in writing data!\n");
		return 0;
	}

	printf("rest super block: %d\n", rest_size * sizeof(char));
	*total += rest_size * sizeof(char);
	return 1;
}

int fwrite_fat_blocks(int total_block_nb, int block_size, int first_dir_entry_index, FILE* fd, int* total) {
	int fat_len = total_block_nb;
	int fat_occuped_bytes = fat_len * sizeof(int);
	int* fat = calloc(fat_len, sizeof(int));
	if (fat == NULL) {
		fprintf(stderr, "Failure in allocating memory!!\n");
		return 0;
	}
	for (int i = 0; i < fat_len; i++) {
		fat[i] = 0xffffffff;
	}
	fat[first_dir_entry_index] = 0;
	if (fwrite(fat, sizeof(int), fat_len, fd) == 0) {
		fprintf(stderr, "Failure in writing data!!\n");
		return 0;
	}

	printf("fat size in bytes: %d\n", fat_occuped_bytes);
	*total += fat_occuped_bytes;

	int rest_size = (block_size - fat_occuped_bytes % block_size) % block_size; // retrieve the
	if (rest_size != 0) {
		char* rest = calloc(rest_size, sizeof(char));
		if (rest == NULL) {
			fprintf(stderr, "Failure in allocating memory!!\n");
			return 0;
		}
		if (fwrite(rest, sizeof(char), rest_size, fd) == 0) {
			fprintf(stderr, "Failure in writing data!!\n");
			return 0;
		}
	}

	printf("rest fat: %d\n", rest_size * sizeof(char));
	*total += rest_size * sizeof(char);
	return 1;
}

// int fwrite_rest_blocks(int total_block_nb, int fat_block_nb, int block_size, FILE* fd, int* total) {
// 	int rest_total_block_nb = total_block_nb - 1 - fat_block_nb - 1;
// 	printf("rest_total_block_nb : %d\n", rest_total_block_nb);
// 	char* rest = calloc(rest_total_block_nb, sizeof(char) * block_size);
// 	if (rest == NULL) {
// 		fprintf(stderr, "Failure in allocating memory!!\n");
// 		return 0;
// 	}
// 	if (fwrite(rest, sizeof(char), rest_total_block_nb * block_size, fd) == 0) {
// 		fprintf(stderr, "Failure in writing data!!\n");
// 		return 0;
// 	}

// 	printf("rest of all fs: %d\n", rest_total_block_nb * block_size * sizeof(char));
// 	*total += rest_total_block_nb * block_size * sizeof(char);
// 	return 1;
// }

// int fwrite_dir_entry(int block_size, FILE* fd, int *total) {	
// 	dir_entry_t* dir_entry = malloc(sizeof(dir_entry_t));
// 	if (dir_entry == NULL) {
// 		fprintf(stderr, "Failure in allocating memory!!\n");
// 		return 0;
// 	}
// 	strcpy(dir_entry->name, "raed");
// 	dir_entry->size = 8000;
// 	dir_entry->start = 7;
// 	if (fwrite(dir_entry, sizeof(dir_entry_t), 1, fd) == 0) {
// 		fprintf(stderr, "Failure in writing data!!\n");
// 		return 0;
// 	}
// 	char* rest = calloc(block_size - sizeof(dir_entry_t), sizeof(char));
// 	if (fwrite(rest, sizeof(char), block_size - sizeof(dir_entry_t), fd) == 0) {
// 		fprintf(stderr, "Failure in writing data!!\n");
// 		return 0;
// 	}
// 	*total += block_size;
// 	return 1;
// }

int main(int argc, char *argv[]) {
	int block_size = atoi(argv[2]);
	int file_size = atoi(argv[4]);
	if (argc == 5) {
		if (!valid_arguments(argv[1], block_size, file_size, argv[3])) {
			return EXIT_FAILURE;
		}

		int total_block_nb = file_size / block_size;

		// fat blocks number
		int fat_len = total_block_nb;
		int fat_occuped_bytes = fat_len * sizeof(int); // why int? because the file system can 
								// be 2^32 bytes size and if a size block = 512
								// so 2^32 / 512 = 8388608 (0x800000) ==> 24 bits
		int fat_block_nb = 1;
		int div = fat_occuped_bytes / block_size;
		if (div != 0) {
			fat_block_nb = div;
			int mod = fat_occuped_bytes % block_size;
			if (mod != 0) {
				fat_block_nb++;
			}
		}

		int first_dir_entry_index = fat_block_nb + 1;

		int total = 0;

		FILE* fd = fopen(argv[3], "w");
		if (fd == NULL) {
			fprintf(stderr, "Failure in opening file \"%s\" (in w mode)!\n", argv[3]);
			return EXIT_FAILURE;
		}

		// write super block in the first block and write 0 in the rest of the block (of size block_size)
		if (!fwrite_super_block(argv[1], block_size, fat_block_nb, fat_len, first_dir_entry_index, fd, &total)) {
			fprintf(stderr, "Failure in writing the super block!\n");
			return EXIT_FAILURE;
		}

		// write fat in the second block. If the size of the fat is greater than block_size
		// then extend fat to the next block and write 0 in the rest of the last block
		if (!fwrite_fat_blocks(total_block_nb, block_size, first_dir_entry_index, fd, &total)) {
			fprintf(stderr, "Failure in writing the fat blocks\n");
			return EXIT_FAILURE;
		}

		int rest_total_block_nb = total_block_nb - 1 - fat_block_nb;
		printf("rest total block nb: %d\n", rest_total_block_nb);
		char* rest = calloc(rest_total_block_nb, sizeof(char) * block_size);
		if (rest == NULL) {
			fprintf(stderr, "Failure in allocating memory!!\n");
			return 0;
		}
		if (fwrite(rest, sizeof(char), rest_total_block_nb * block_size, fd) == 0) {
			fprintf(stderr, "Failure in writing data!!\n");
			return 0;
		}

		printf("rest of all fs: %d\n", rest_total_block_nb * block_size * sizeof(char));
		total += rest_total_block_nb * block_size * sizeof(char);

		// // write the first file descriptor table at the block (fat_block_nb + 1)
		// if (!fwrite_dir_entry(block_size, fd, &total)) {
		// 	fprintf(stderr, "Failure in writing the main entry dir\n");
		// 	return EXIT_FAILURE;
		// }

		// // write zeros in the rest of blocks
		// if (!fwrite_rest_blocks(total_block_nb, fat_block_nb, block_size, fd, &total)) {
		// 	fprintf(stderr, "Failure in writing the rest of fs blocks\n");
		// 	return EXIT_FAILURE;
		// }

		fclose(fd);

		printf("Total: %d\n", total);

		return EXIT_SUCCESS;

	} else {

		fprintf(stderr, "\nfs_create <label> <block_size> <file_name> <file_size>\n\n"
					"• label : Label of the file system\n"
					"• block_size : Must be a multiple of 512\n"
					"• file_name : Name of the file system image\n"
					"• file_size : Size of the file system image in Byte\n\n"
					"Example: fs_create doge_fs 512 fs.img 1000448\n\n");
		return EXIT_FAILURE;
	}
}
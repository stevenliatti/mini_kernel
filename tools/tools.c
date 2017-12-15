#include "tools.h"
#include <sys/stat.h>

/**
 * @brief  This function use stat to get size of file from his name (use sys/stat.h)
 *
 * @param  file_name the file name
 * @return the size of file
 */
int get_file_size(const char* file_name) {
	struct stat sb;
	CHECK_ERR(stat(file_name, & sb) != 0, "Failure in stat \"%s\"\n", file_name)
	return sb.st_size;
}

/**
 * @brief  This function load the fat from image file.
 *
 * @param  fd file descriptor of image
 * @param  sb the super block
 * @param  fat a double pointer containing the fat
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
int load_fat(FILE* fd, super_block_t* sb, int** fat) {
	int fat_pos = sb->block_size;
	CHECK_ERR(fseek(fd, fat_pos, SEEK_SET) != 0, "Seeking file failed!\n")
	*fat = malloc(sb->fat_len * sizeof(int));
	CHECK_ERR(*fat == NULL, "Failure in allocating memory!!\n")
	CHECK_ERR(fread(*fat, sizeof(int), sb->fat_len, fd) == 0, "Failure in reading fat table\n")
	return EXIT_SUCCESS;
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

/**
 * @brief  This function load the super block from image file.
 *
 * @param  fd file descriptor of image
 * @param  sb a double pointer containing the super block
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
int load_super_block(FILE* fd, super_block_t** sb) {
	*sb = malloc(sizeof(super_block_t));
	CHECK_ERR(*sb == NULL, "Failure in allocating memory!!\n")
	CHECK_ERR(fread(*sb, sizeof(super_block_t), 1, fd) == 0, "Failure in reading super block\n")
	return EXIT_SUCCESS;
}

/**
 * @brief  This function check if the arguments are correct.
 *
 * @param  fs_name the file system's name
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
int valid_fs_name(char* fs_name) {
	FILE* fd = fopen(fs_name, "r");
	CHECK_ERR(fd == NULL, "Error: File system with name \"%s\" doesn't exist\n", fs_name)
	return EXIT_SUCCESS;
}

#include "tools.h"
#include "../common/common.h"

/**
 * @brief  This function compute the percentage of file system space in use.
 *
 * @param  total the total size of fs
 * @param  partial the size of fs used
 * @return the percentage of file system space in use
 */
static float percentage(int total, int partial) {
	return (float)partial * 100.0 / (float)total;
}

/**
 * @brief  Entry point of program, load differents structures and print infos about fs.
 *
 * @param  argc the number of arguments
 * @param  argc the array of arguments
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
int main(int argc, char *argv[]) {
	if (argc == 2) {
		CHECK_ERR(valid_fs_name(argv[1]), "invalid arguments\n")

		FILE* fd = fopen(argv[1], "r");
		CHECK_ERR(fd == NULL, "Failure in opening file \"%s\" (in r mode)!\n", argv[1])

		super_block_t* sb;
		CHECK_ERR(load_super_block(fd, &sb), "Failure in allocating super block!\n")

		int* fat;
		CHECK_ERR(load_fat(fd, sb, &fat), "Failure in allocating fat!\n")

		int used_blocks_nb = 0;
		int free_blocks_nb = 0;
		for (int i = sb->first_entry; i < sb->fat_len; i++) {
			if (fat[i] == -1) {
				free_blocks_nb++;
			} else {
				used_blocks_nb++;
			}
		}

		printf("\nUsed blocks number: %7d \t %10d B\t", used_blocks_nb, used_blocks_nb * sb->block_size);
		printf("%9.5f%%\n", percentage(sb->fat_len - sb->first_entry, used_blocks_nb));
		printf("Free blocks number: %7d \t %10d B\t", free_blocks_nb, free_blocks_nb * sb->block_size);
		printf("%9.5f%%\n", percentage(sb->fat_len - sb->first_entry, free_blocks_nb));

		fclose(fd);
		return EXIT_SUCCESS;
	}
	else {
		fprintf(stderr, "\nfs_info <fs_name>\n\n"
			"• fs_name : File system name\n\n"
			"Example: fs_info fs.img\n\n");
		return EXIT_FAILURE;
	}
}

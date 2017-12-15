#include "tools.h"
#include "../common/common.h"

static int valid_arguments(char* fs_name) {
	FILE* fd = fopen(fs_name, "r");
	CHECK_ERR(fd == NULL, "error: File system with name \"%s\" doesn't exist\n", fs_name)
	return EXIT_SUCCESS;
}

int get_dir_entry(FILE* fd, int metadata_block_offset, int dir_entry_offset, int block_size, dir_entry_t* de) {
	CHECK_ERR(fseek(fd, metadata_block_offset * block_size + dir_entry_offset, SEEK_SET) != 0, 
		"Seeking file to get dir_entry failed!\n")

	int buffer_real_size = 0;
	CHECK_ERR((buffer_real_size = fread(de, sizeof(dir_entry_t), 1, fd)) == 0, 
			"Failure in reading dir_entry\n")
	return EXIT_SUCCESS;
}

float percentage(int total, int partial) {
	// printf("%f\n", (float)partial);
	// printf("%f\n", (float)total);
	return (float)partial * 100.0 / (float)total;
}

int main(int argc, char *argv[]) {
	if (argc == 2) {
		// check arguments
		char* fs_name = malloc(sizeof(char) * strlen(argv[1]));
		CHECK_ERR(fs_name == NULL, "Failure in allocating memory!!\n")
		strcpy(fs_name, argv[1]);

		CHECK_ERR(valid_arguments(fs_name), "invalid arguments\n")

		FILE* fd = fopen(fs_name, "r");
		CHECK_ERR(fd == NULL, "Failure in opening file \"%s\" (in r mode)!\n", fs_name)

		// read super block
		super_block_t* sb = malloc(sizeof(super_block_t));
		CHECK_ERR(sb == NULL, "Failure in allocating memory!!\n")
		CHECK_ERR(fread(sb, sizeof(super_block_t), 1, fd) == 0, "Failure in reading super block\n")
		print_super_block(sb);

		// load the fat
		int fat_pos = sb->block_size;
		CHECK_ERR(fseek(fd, fat_pos, SEEK_SET) != 0, "Seeking file failed!\n")
		int* fat = malloc(sb->fat_len * sizeof(int));
		CHECK_ERR(fat == NULL, "Failure in allocating memory!!\n")
		CHECK_ERR(fread(fat, sizeof(int), sb->fat_len, fd) == 0, "Failure in reading fat table\n")
		// print_fat(fat, sb->fat_len);

		int used_blocks_nb = 0;
		int free_blocks_nb = 0;
		for (int i = sb->first_dir_entry; i < sb->fat_len; i++) {
			if (fat[i] == -1) {
				free_blocks_nb++;
			} else {
				used_blocks_nb++;
			}
		}

		printf("\nUsed blocks number: %7d \t %10d B\t", used_blocks_nb, used_blocks_nb * sb->block_size);
		printf("%9.5f%%\n", percentage(sb->fat_len - sb->first_dir_entry, used_blocks_nb));
		printf("Free blocks number: %7d \t %10d B\t", free_blocks_nb, free_blocks_nb * sb->block_size);
		printf("%9.5f%%\n", percentage(sb->fat_len - sb->first_dir_entry, free_blocks_nb));

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

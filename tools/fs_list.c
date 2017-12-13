#include "tools.h"

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

int main(int argc, char *argv[]) {
	if (argc == 2) {
		// check arguments
		char* fs_name = malloc(sizeof(char) * strlen(argv[1]));
		CHECK_ERR(fs_name == NULL, "Failure in allocating memory!!\n")
		strcpy(fs_name, argv[1]);

		CHECK_ERR(valid_arguments(fs_name), "invalid arguments\n")

		FILE* fd = fopen(fs_name, "r+");
		CHECK_ERR(fd == NULL, "Failure in opening file \"%s\" (in r+ mode)!\n", fs_name)

		// read super block
		super_block_t* sb = malloc(sizeof(super_block_t));
		CHECK_ERR(sb == NULL, "Failure in allocating memory!!\n")
		CHECK_ERR(fread(sb, sizeof(super_block_t), 1, fd) == 0, "Failure in reading super block\n")
		// display_super_block(sb);

		// load the fat
		int fat_pos = sb->block_size;
		CHECK_ERR(fseek(fd, fat_pos, SEEK_SET) != 0, "Seeking file failed!\n")
		int* fat = malloc(sb->fat_len * sizeof(int));
		CHECK_ERR(fat == NULL, "Failure in allocating memory!!\n")
		CHECK_ERR(fread(fat, sizeof(int), sb->fat_len, fd) == 0, "Failure in reading fat table\n")
		// print_fat(fat, sb->fat_len);

		// check for dir entries
		dir_entry_t* dir_entry = malloc(sizeof(dir_entry_t));
		CHECK_ERR(dir_entry == NULL, "Failure in allocating memory!!\n")
		int files_count = 0;
		int metadata_block_offset = sb->first_dir_entry;
		printf("current metadata block: %d\t", metadata_block_offset);
		printf("next metadata block: %d\n", fat[metadata_block_offset]);
		do {
			printf("\nFile name \t\t\t\tsize (byte)\t\tFirst block num\n");
			int dir_entry_offset = 0;
			do {
				get_dir_entry(fd, metadata_block_offset, dir_entry_offset, sb->block_size, dir_entry);
				if (fat[dir_entry->start] != -1) {
					printf("%s\t\t%d\t\t\t%d\n", dir_entry->name, dir_entry->size, dir_entry->start);
					files_count++;
				}
				dir_entry_offset += sizeof(dir_entry_t);
			} while (dir_entry_offset != sb->block_size);
			metadata_block_offset = fat[metadata_block_offset];
			printf("\n\ncurrent metadata block: %d\t", metadata_block_offset);
			printf("next metadata block: %d\n", fat[metadata_block_offset]);
		} while (fat[metadata_block_offset] != -1);

		printf("\nFiles count: %d\n", files_count);

		fclose(fd);
		return EXIT_SUCCESS;
	}
	else {
		fprintf(stderr, "\nfs_list <fs_name>\n\n"
					"• fs_name : File system name\n\n"
					"Example: fs_list fs.img\n\n");
		return EXIT_FAILURE;
	}
}

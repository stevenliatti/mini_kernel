/**
 * @file 		fs_list.c
 * @brief 		Tool for listing files in image file system.
 *
 * @author 		Steven Liatti
 * @author 		Raed Abdennadher
 * @bug 		No known bugs.
 * @date 		December 16, 2017
 * @version		1.0
 */

#include "tools.h"
#include "../common/common.h"

/**
 * @brief  This function get the metadata of metadata block index given.
 *
 * @param  fd the file descriptor of image
 * @param  metadata_block_index index of block of current metadata
 * @param  entry_offset offset in bytes in current block of metadata
 * @param  block_size the block size
 * @param  de a pointer to entry_t filled with metadata
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
static int get_entry(FILE* fd, int metadata_block_index, int entry_offset,
	int block_size, entry_t* de) {

	CHECK_ERR(fseek(fd, metadata_block_index * block_size + entry_offset, SEEK_SET) != 0,
		"Seeking file to get entry failed!\n")

	int buffer_real_size = 0;
	CHECK_ERR((buffer_real_size = fread(de, sizeof(entry_t), 1, fd)) == 0,
		"Failure in reading entry\n")
	return EXIT_SUCCESS;
}

/**
 * @brief  This function print an horizontal line and format it.
 */
static void print_horizontal_line() {
	char border[] = "---------------------------------";
	printf(" %.*s", 26, border);
	printf(" %.*s", 17, border);
	printf(" %.*s\n", 17, border);
}

/**
 * @brief  This function print a header formated.
 */
static void print_table_header() {
	print_horizontal_line();
	printf("| %24s | %15s | %15s |\n", "File name", "size (byte)", "First block num");
	print_horizontal_line();
}

/**
 * @brief  Entry point of program, load differents structures and list the files in fs.
 *
 * @param  argc the number of arguments
 * @param  argc the array of arguments
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
int main(int argc, char *argv[]) {
	if (argc == 2) {
		CHECK_ERR(valid_fs_name(argv[1]), "invalid arguments\n")

		FILE* fd = fopen(argv[1], "r+");
		CHECK_ERR(fd == NULL, "Failure in opening file \"%s\" (in r+ mode)!\n", argv[1])

		super_block_t* sb;
		CHECK_ERR(load_super_block(fd, &sb), "Failure in allocating super block!\n")

		int* fat;
		CHECK_ERR(load_fat(fd, sb, &fat), "Failure in allocating fat!\n")

		// check for dir entries
		entry_t* entry = malloc(sizeof(entry_t));
		CHECK_ERR(entry == NULL, "Failure in allocating memory!!\n")
		int files_count = 0;
		int metadata_block_index = sb->first_entry;
		printf("current metadata block: %10d\t", metadata_block_index);
		printf("next metadata block: %10d\n", fat[metadata_block_index]);
		do {
			print_table_header();

			int entry_offset = 0;
			do {
				get_entry(fd, metadata_block_index, entry_offset, sb->block_size, entry);
				if (fat[entry->start] != -1) {
					printf("| %24s | %15d | %15d |\n", entry->name, entry->size, entry->start);
					files_count++;
				}
				entry_offset += sizeof(entry_t);
			} while (entry_offset != sb->block_size);
			metadata_block_index = fat[metadata_block_index];

			print_horizontal_line();

			printf("\ncurrent metadata block: %10d\t", metadata_block_index);
			printf("next metadata block: %10d\n", fat[metadata_block_index]);
		} while (fat[metadata_block_index] != -1);

		printf("\nFiles count: %d\n", files_count);

		print_fat(fat, sb->blocks_count);
		
		free(sb);
		free(fat);
		free(entry);
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

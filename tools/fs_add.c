#include "tools.h"
#include "../common/common.h"

/**
 * @brief  This function check if the arguments are correct.
 *
 * @param  file_name the file's name to add
 * @param  fs_name the file system's name
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
static int valid_arguments(char* file_name, char* fs_name) {
	CHECK_ERR(strlen(basename(file_name)) >= ENTRY_NAME_SIZE, "file_name too long, must be <= %d\n",
		ENTRY_NAME_SIZE - 1)
	FILE* fd = fopen(fs_name, "r");
	CHECK_ERR(fd == NULL, "Error: File system with name \"%s\" doesn't exist\n", fs_name)
	fclose(fd);

	fd = fopen(file_name, "r");
	CHECK_ERR(fd == NULL, "Error: File with name \"%s\" doesn't exist\n", file_name)
	fclose(fd);
	return EXIT_SUCCESS;
}

/**
 * @brief  This function return the index of next available block.
 *
 * @param  fat our fat
 * @param  fat_len the fat length
 * @param  first the index of first entry
 * @return the index of next available block, or zero if failed
 */
static int get_next_available_block(int* fat, int fat_len, int first) {
	for (int i = first + 1; i < fat_len; i++) {
		if (fat[i] == (int)0xffffffff) {
			return i;
		}
	}
	return 0;
}

/**
 * @brief  This function fill the array of needed blocks.
 *
 * @param  needed_block the array of index of blocks
 * @param  fat our fat
 * @param  first the index of first entry
 * @param  fat_len the fat length
 * @param  needed_block_nb the number of needed blocks (for the file added)
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or lack of blocks
 */
static int get_available_blocks(int* needed_block, int* fat, int first, int fat_len,
	int needed_block_nb) {
	int j = 0;
	int i = first;
	while (j < needed_block_nb && (i = get_next_available_block(fat, fat_len, i)) != 0) {
		needed_block[j] = i;
		j++;
	}
	CHECK_ERR(i == 0, "No space available for data!\n")
	return EXIT_SUCCESS;
}

/**
 * @brief  This function count the number of needed block for the current file.
 *
 * @param  file_name the file's name to add
 * @param  sb the super block
 * @return the number of needed block for the current file
 */
static int get_needed_block(char* file_name, super_block_t* sb) {
	int file_size = get_file_size(file_name);
	printf("File size: %d bytes\n", file_size);
	int file_needed_block = file_size / sb->block_size + (file_size % sb->block_size == 0 ? 0 : 1);
	printf("Number of needed blocks: %d\n", file_needed_block);
	return file_needed_block;
}

/**
 * @brief  This function search a place to put a new entry of meta-data.
 * We iterate on the first bloc (with fread) until we reach the end of it.
 * If we reach the last block of meta-data and the place is already taken by
 * an entry, we search a new empty block in FAT to extends entries.
 *
 * @param  file_name the file's name to add
 * @param  fat our fat
 * @param  sb the super block
 * @param  fd the file descriptor of the file
 * @param  temp a temporary struct entry for the loop
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
static int find_empty_entry(char* file_name, int* fat, super_block_t* sb, FILE* fd,
	entry_t* temp) {
	int pos = sb->first_entry * sb->block_size;
	int last_pos = 0;
	int readed_data = 0;
	bool empty_space_found = false;
	int empty_space_offset = 0;
	do {
		readed_data = 0;
		last_pos = pos / sb->block_size;
		CHECK_ERR(fseek(fd, pos, SEEK_SET) != 0, "Seeking file failed!\n")

		do {
			int data_len;
			CHECK_ERR((data_len = fread(temp, sizeof(entry_t), 1, fd)) == 0,
				"Failure in reading entry\n")
			readed_data += sizeof(entry_t);
			CHECK_ERR(strcmp(temp->name, file_name) == 0 && fat[temp->start] != -1,
				"File with name \"%s\" already exists in file system!\n", file_name)

			if (fat[temp->start] == -1 && !empty_space_found) {
				empty_space_found = true;
				empty_space_offset = pos + readed_data - sizeof(entry_t);
			}

		} while (fat[temp->start] != -1 && readed_data < sb->block_size);
		pos = sb->block_size * fat[last_pos];
	} while (pos != 0);

	if (readed_data == sb->block_size && pos == 0 && fat[temp->start] != -1) {
		int next_index = get_next_available_block(fat, sb->fat_len, sb->first_entry);
		CHECK_ERR(next_index == 0, "No space available for the meta-data!\n")

		fat[last_pos] = next_index;
		fat[next_index] = 0;
		CHECK_ERR(fseek(fd, next_index * sb->block_size, SEEK_SET) != 0,
			"Seeking file failed!\n")
	}
	else if (empty_space_found) {
		CHECK_ERR(fseek(fd, empty_space_offset, SEEK_SET) != 0,
			"Seeking file failed!\n")
	}
	else {
		CHECK_ERR(fseek(fd, -sizeof(entry_t), SEEK_CUR) != 0, "Seeking file failed!\n")
	}
	return EXIT_SUCCESS;
}

/**
 * @brief  This function browse the fat and update it.
 *
 * @param  file_needed_block the number of needed blocks (for the file added)
 * @param  available_blocks the array of index of blocks
 * @param  sb the super block
 * @param  fat our fat
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
static int update_fat(int file_needed_block, int* available_blocks,
	super_block_t* sb, int* fat) {

	CHECK_ERR(get_available_blocks(available_blocks, fat, sb->first_entry,
		sb->fat_len, file_needed_block), "No blocks available!\n")
	printf("File content will be added in blocks:\n");
	printf("|");
	for (int i = 0; i < file_needed_block; i++)	{
		printf("%d|", available_blocks[i]);
	}
	printf("\n\n");

	int i;
	for (i = 0; i < file_needed_block - 1; i++) {
		fat[available_blocks[i]] = available_blocks[i + 1];
	}
	fat[available_blocks[i]] = 0;

	return EXIT_SUCCESS;
}

/**
 * @brief  This function update fat (via update_fat()) and write the new entry,
 * the fat and the content file in file system.
 *
 * @param  sb the super block
 * @param  fd the file descriptor of fs file
 * @param  fat our fat
 * @param  file_name the name of file to add
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
static int write_all(super_block_t* sb, FILE* fd, int* fat, char* file_name) {
	printf("File name: %s\n", basename(file_name));
	int file_needed_block = get_needed_block(file_name, sb);
	int available_blocks[file_needed_block];
	CHECK_ERR(update_fat(file_needed_block, available_blocks, sb, fat),
		"Error in update_fat\n")

	// write the new entry in the fs
	entry_t* entry = malloc(sizeof(entry_t));
	CHECK_ERR(entry == NULL, "Failure in allocating memory!!\n")
	strcpy(entry->name, basename(file_name));
	entry->size = get_file_size(file_name);
	entry->start = available_blocks[0];
	CHECK_ERR(fwrite(entry, sizeof(entry_t), 1, fd) == 0, "Failure in writing data!!\n")

	// write the fat in the fs
	int pos = sb->block_size;
	CHECK_ERR(fseek(fd, pos, SEEK_SET) != 0, "Seeking file failed!\n")
	CHECK_ERR(fwrite(fat, sizeof(int), sb->fat_len, fd) == 0, "Failure in writing data!\n")

	// write the file content in the fs
	FILE* fd2 = fopen(file_name, "r");
	CHECK_ERR(fd2 == NULL, "Failure in opening file \"%s\" (in r+ mode)!\n", file_name)

	for (int i = 0; i < file_needed_block; i++) {
		char buffer[sb->block_size];
		int buffer_real_size;
		CHECK_ERR((buffer_real_size = fread(buffer, sizeof(char), sb->block_size, fd2)) == 0,
			"Failure in reading fat table\n")

		pos = sb->block_size * available_blocks[i];
		CHECK_ERR(fseek(fd, pos, SEEK_SET) != 0, "Seeking file failed!\n")
		CHECK_ERR(fwrite(buffer, sizeof(char), buffer_real_size, fd) == 0,
			"Failure in writing data!!\n")
	}

	fclose(fd2);
	return EXIT_SUCCESS;
}

/**
 * @brief  Entry point of program, load differents structures and add the file to fs.
 *
 * @param  argc the number of arguments
 * @param  argc the array of arguments
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
int main(int argc, char *argv[]) {
	if (argc == 3) {
		CHECK_ERR(valid_arguments(argv[1], argv[2]), "invalid arguments\n")

		FILE* fd = fopen(argv[2], "r+");
		CHECK_ERR(fd == NULL, "Failure in opening file \"%s\" (in r+ mode)!\n", argv[2])

		super_block_t* sb;
		CHECK_ERR(load_super_block(fd, &sb), "Failure in allocating super block!\n")

		int* fat;
		CHECK_ERR(load_fat(fd, sb, &fat), "Failure in allocating fat!\n")

		entry_t* temp = malloc(sizeof(entry_t));
		CHECK_ERR(temp == NULL, "Failure in allocating memory!!\n")
		CHECK_ERR(find_empty_entry(basename(argv[1]), fat, sb, fd, temp),
			"Error in find_empty_entry\n")

		CHECK_ERR(write_all(sb, fd, fat, argv[1]), "Error in write_all\n")

		fclose(fd);
		return EXIT_SUCCESS;
	}
	else {
		fprintf(stderr, "\nfs_add <file> <fs_name>\n\n"
			"• file : Name of the file to be added in the fs\n"
			"• fs_name : File system name\n\n"
			"Example: fs_add hello.txt fs.img\n\n");
		return EXIT_FAILURE;
	}
}

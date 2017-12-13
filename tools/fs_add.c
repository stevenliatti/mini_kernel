#include "tools.h"

static int valid_arguments(char* file_name, char* fs_name) {
	FILE* fd = fopen(fs_name, "r");
	CHECK_ERR(fd == NULL, "error: File system with name \"%s\" doesn't exist\n", fs_name)
	fd = fopen(file_name, "r");
	CHECK_ERR(fd == NULL, "error: File with name \"%s\" doesn't exist\n", file_name)
	return EXIT_SUCCESS;
}

static int get_next_available_block(int* fat, int fat_len, int first) {
	for (int i = first + 1; i < fat_len; i++) {
		if (fat[i] == (int)0xffffffff) {
			return i;
		}
	}
	return 0;
}

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

static int get_needed_block(char* file_name, super_block_t* sb) {
	int file_size = get_file_size(file_name);
	printf("file_size: %d\n", file_size);
	int file_needed_block = file_size / sb->block_size + (file_size % sb->block_size == 0 ? 0 : 1);
	printf("file_needed_block: %d\n", file_needed_block);
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
 * @param  temp a temporary struct dir_entry for the loop
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
static int find_empty_entry(char* file_name, int* fat, super_block_t* sb, FILE* fd, 
	dir_entry_t* temp) {
	int last_pos = 0;
	int pos = sb->first_dir_entry * sb->block_size;
	int readed_data = 0;
	do {
		last_pos = pos / sb->block_size;
		printf("pos: %d\n", pos);
		CHECK_ERR(fseek(fd, pos, SEEK_SET) != 0, "Seeking file failed!\n")
		
		do {
			int data_len;
			CHECK_ERR((data_len = fread(temp, sizeof(dir_entry_t), 1, fd)) == 0, 
				"Failure in reading dir_entry\n")
			readed_data += sizeof(dir_entry_t);
			printf("dir:\n\tname: %s\n\tstart: %d\n", temp->name, temp->start);
			CHECK_ERR(strcmp(temp->name, file_name) == 0 && fat[temp->start] != -1, 
				"File with name \"%s\" already exists in file system!\n", file_name)
		} // fat[temp->start] is -1 means that the place is available
		while (fat[temp->start] != -1 && readed_data < sb->block_size);
		if (fat[temp->start] == -1) {
			break;
		}
		pos = sb->block_size * fat[last_pos];
	} while (pos != 0);
	
	printf("readed_data: %d\n", readed_data);
	printf("pos: %d\n", pos);
	printf("fat[temp->start]: %d\n", fat[temp->start]);
	// si on arrive au bout du dernier block de meta-data et que l'emplacement est déjà occupé par un dir_entry
	if (readed_data == sb->block_size && pos == 0 && fat[temp->start] != -1) {
		int next_index = get_next_available_block(fat, sb->fat_len, sb->first_dir_entry);
		CHECK_ERR(next_index == 0, "No space available for the meta-data!\n")
		
		fat[last_pos] = next_index;
		fat[next_index] = 0;
		CHECK_ERR(fseek(fd, next_index * sb->block_size, SEEK_SET) != 0, 
		"Seeking file failed!\n")
	}
	else {
		CHECK_ERR(fseek(fd, -sizeof(dir_entry_t), SEEK_CUR) != 0, "Seeking file failed!\n")
	}
	return EXIT_SUCCESS;
}

static int update_fat_and_dir_entry(int file_needed_block, int* available_blocks, 
	char* file_name, super_block_t* sb, int* fat, FILE* fd) {
		CHECK_ERR(get_available_blocks(available_blocks, fat, sb->first_dir_entry, 
		sb->fat_len, file_needed_block), "No blocks available!\n")
	for (int i = 0; i < file_needed_block; i++)	{
		printf("%d|", available_blocks[i]);
	}
	printf("\n");

	int i = 0;
	for (; i < file_needed_block - 1; i++) {
		fat[available_blocks[i]] = available_blocks[i + 1];
	}
	fat[available_blocks[i]] = 0;
	print_fat(fat, sb->fat_len);

	// write the new dir_entry in the fs
	dir_entry_t* dir_entry = malloc(sizeof(dir_entry_t));
	CHECK_ERR(dir_entry == NULL, "Failure in allocating memory!!\n")
	
	strcpy(dir_entry->name, file_name);
	dir_entry->size = get_file_size(file_name);
	dir_entry->start = available_blocks[0];
	CHECK_ERR(fwrite(dir_entry, sizeof(dir_entry_t), 1, fd) == 0, "Failure in writing data!!\n")

	return EXIT_SUCCESS;
}

static int write_all(super_block_t* sb, FILE* fd, int* fat, char* file_name) {
	// update fat and dir_entry
	int file_needed_block = get_needed_block(file_name, sb);
	int available_blocks[file_needed_block];
	CHECK_ERR(update_fat_and_dir_entry(file_needed_block, available_blocks, file_name, sb, fat, fd), 
		"error in update_fat_and_dir_entry\n")
	
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

		printf("buffer_real_size: %d\n", buffer_real_size);
		pos = sb->block_size * available_blocks[i];
		CHECK_ERR(fseek(fd, pos, SEEK_SET) != 0, "Seeking file failed!\n")
		CHECK_ERR(fwrite(buffer, sizeof(char), buffer_real_size, fd) == 0, 
			"Failure in writing data!!\n")
	}

	fclose(fd2);
	return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
	if (argc == 3) {
		// check arguments
		char* file_name = malloc(sizeof(char) * strlen(argv[1]));
		CHECK_ERR(file_name == NULL, "Failure in allocating memory!!\n")
		strcpy(file_name, argv[1]);

		char* fs_name = malloc(sizeof(char) * strlen(argv[2]));
		CHECK_ERR(fs_name == NULL, "Failure in allocating memory!!\n")
		strcpy(fs_name, argv[2]);

		CHECK_ERR(valid_arguments(file_name, fs_name), "invalid arguments\n")

		FILE* fd = fopen(fs_name, "r+");
		CHECK_ERR(fd == NULL, "Failure in opening file \"%s\" (in r+ mode)!\n", file_name)

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
		print_fat(fat, sb->fat_len);

		// find the first empty place to write the new dir_entry and try to set it
		dir_entry_t* temp = malloc(sizeof(dir_entry_t));
		CHECK_ERR(temp == NULL, "Failure in allocating memory!!\n")
		CHECK_ERR(find_empty_entry(file_name, fat, sb, fd, temp), "error in find_empty_entry\n")

		// write fat and file content in the file system
		CHECK_ERR(write_all(sb, fd, fat, file_name), 
			"error in write_all\n")

		fclose(fd);
		return EXIT_SUCCESS;
	}
	else {
		fprintf(stderr, "\nfs_add <file> <fs_name>\n\n"
					"• file : Name of the file to be added to the file system\n"
					"• fs_name : File system name\n\n"
					"Example: fs_add hello.txt fs.img\n\n");
		return EXIT_FAILURE;
	}
}

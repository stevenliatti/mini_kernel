#include "fs.h"
#include <sys/stat.h>

static int valid_arguments(char* file_name, char* fs_name) {
	FILE* fd = fopen(fs_name, "r");
	CHECK_ERR(fd == NULL, "error: File system with name \"%s\" doesn't exist\n", fs_name)
	fd = fopen(file_name, "r");
	CHECK_ERR(fd == NULL, "error: File with name \"%s\" doesn't exist\n", file_name)
	return EXIT_SUCCESS;
}

static int get_file_size(const char * file_name) {
	struct stat sb;
	CHECK_ERR(stat(file_name, & sb) != 0, "error: stat failed for \"%s\"\n", file_name)
	return sb.st_size;
}

static int get_next_available_block(int* fat, int fat_len, int first) {
	for (int i = first + 1; i < fat_len; i++) {
		if (fat[i] == (int)0xffffffff) {
			return i;
		}
	}
	return 0;
}

static int get_available_blocks(int* needed_block, int* fat, int first, int fat_len, int needed_block_nb) {
	int j = 0;
	int i = first;
	while (j < needed_block_nb && (i = get_next_available_block(fat, fat_len, i)) != 0) {
		needed_block[j] = i;
		j++;
	}
	CHECK_ERR(i == 0, "No space available for data!\n")
	return EXIT_SUCCESS;
}

static void update_fat(int* fat, int* available, int size) {
	int i = 0;
	for (; i < size - 1; i++) {
		fat[available[i]] = available[i + 1];
	}
	fat[available[i]] = 0;
}

static void print_fat(int* fat, int fat_len) {
	printf("fat table: \n|");
	for (int i = 0; i < fat_len; i++) {
		printf("%d|", fat[i]);
	}
	printf("\n");
}

int main(int argc, char *argv[]) {
	if (argc == 3) {
		CHECK_ERR(valid_arguments(argv[1], argv[2]), "invalid arguments\n")
		FILE* fd = fopen(argv[2], "r+");
		CHECK_ERR(fd == NULL, "Failure in opening file \"%s\" (in r+ mode)!\n", argv[1])

		super_block_t* sb = malloc(sizeof(super_block_t));
		CHECK_ERR(fread(sb, sizeof(super_block_t), 1, fd) == 0, "Failure in reading super block\n")
		display_super_block(sb);

		// load the fat
		int fat_pos = sb->block_size;
		CHECK_ERR(fseek(fd, fat_pos, SEEK_SET) != 0, "Seeking file failed!\n")
		int* fat = malloc(sb->fat_len * sizeof(int));
		CHECK_ERR(fread(fat, sizeof(int), sb->fat_len, fd) == 0, "Failure in reading fat table\n")
		print_fat(fat, sb->fat_len);

		// find the first empty place to write the new dir_entry
		dir_entry_t* temp = malloc(sizeof(dir_entry_t));
		CHECK_ERR(temp == NULL, "Failure in allocating memory!!\n")
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
				CHECK_ERR(strcmp(temp->name, argv[1]) == 0, "File with name \"%s\" \
					already exists in file system!\n", argv[1])
			} // temp->start is zero means that the place is available
			while (temp->start != 0 && readed_data < sb->block_size);
			if (temp->start == 0) {
				break;
			}
			pos = sb->block_size * fat[last_pos];
		} while (pos != 0);

		printf("readed_data: %d\n", readed_data);
		printf("pos: %d\n", pos);
		printf("temp->start: %d\n", temp->start);
		// si on arrive au bout du dernier block de meta-data et que l'emplacement n'est pas disponible
		if (readed_data == sb->block_size && pos == 0 && temp->start != 0) {
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

		// updating the fat
		int file_size = get_file_size(argv[1]);
		printf("file_size: %d\n", file_size);
		int file_needed_block = file_size / sb->block_size + (file_size % sb->block_size == 0 ? 0 : 1);
		printf("file_needed_block: %d\n", file_needed_block);

		int available_blocks[file_needed_block];
		CHECK_ERR(get_available_blocks(available_blocks, fat, sb->first_dir_entry, 
			sb->fat_len, file_needed_block), "No blocks available!\n")
		for (int i = 0; i < file_needed_block; i++)	{
			printf("%d|", available_blocks[i]);
		}
		printf("\n");
		update_fat(fat, available_blocks, file_needed_block);
		print_fat(fat, sb->fat_len);

		// write the new dir_entry in the fs
		dir_entry_t* dir_entry = malloc(sizeof(dir_entry_t));
		CHECK_ERR(dir_entry == NULL, "Failure in allocating memory!!\n")
		
		strcpy(dir_entry->name, argv[1]);
		dir_entry->size = file_size;
		dir_entry->start = available_blocks[0];
		CHECK_ERR(fwrite(dir_entry, sizeof(dir_entry_t), 1, fd) == 0, "Failure in writing data!!\n")

		// write the fat in the fs
		pos = sb->block_size;
		CHECK_ERR(fseek(fd, pos, SEEK_SET) != 0, "Seeking file failed!\n")
		CHECK_ERR(fwrite(fat, sizeof(int), sb->fat_len, fd) == 0, "Failure in writing data!\n")

		// write the file content in the fs
		FILE* fd2 = fopen(argv[1], "r");
		CHECK_ERR(fd2 == NULL, "Failure in opening file \"%s\" (in r+ mode)!\n", argv[1])

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

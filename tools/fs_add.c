#include "fs.h"
#include <sys/stat.h>

int valid_arguments(char* file_name, char* fs_name) {
	// check the fs_name
	FILE* fd = fopen(fs_name, "r");
	if (fd == NULL) {// file already exist
		fprintf(stderr, "error: File system with name \"%s\" doesn't exist\n", fs_name);
		return 0;
	}
	// check the file_name
	fd = fopen(file_name, "r");
	if (fd == NULL) {// file already exist
		fprintf(stderr, "error: File system with name \"%s\" doesn't exist\n", file_name);
		return 0;
	}
	return 1;
}

static unsigned get_file_size (const char * file_name) {
	struct stat sb;
	if (stat (file_name, & sb) != 0) {
		fprintf(stderr, "error: stat failed for \"%s\"\n", file_name);
		exit (EXIT_FAILURE);
	}
	return sb.st_size;
}

int get_next_available_block(int* fat, int fat_len, int first) {
	for (int i = first + 1; i < fat_len; i++) {
		if (fat[i] == (int)0xffffffff) {
			return i;
		}
	}
	return 0;
}

void get_available_blocks(int* needed_block, int* fat, int first, int fat_len, int needed_block_nb) {
	// int i = first;
	// for (int j = 0; j < needed_block_nb; i++) {
	// 	if (fat[i] == (int)0xffffffff) {
	// 		needed_block[j] = i;
	// 		j++;
	// 	}
	// }
	int j = 0;
	int i = first;
	while (j < needed_block_nb && (i = get_next_available_block(fat, fat_len, i)) != 0) {
		needed_block[j] = i;
		j++;
	}
	if (i == 0) {
		fprintf(stderr, "No space available for data!\n");
		exit (EXIT_FAILURE);
	}
}

void update_fat(int* fat, int* available, int size) {
	int i = 0;
	for (; i < size - 1; i++) {
		fat[available[i]] = available[i + 1];
	}
	fat[available[i]] = 0;
}

int main(int argc, char *argv[]) {
	if (argc == 3) {
		if (!valid_arguments(argv[1], argv[2])) {
			return EXIT_FAILURE;
		}

		FILE* fd = fopen(argv[2], "r+");
		if (fd == NULL) {
			fprintf(stderr, "Failure in opening file \"%s\" (in r+ mode)!\n", argv[1]);
		}

		super_block_t* sb = malloc(sizeof(super_block_t));
		if (fread(sb, sizeof(super_block_t), 1, fd) == 0) {
			fprintf(stderr, "Failure in reading super block\n");
			return EXIT_FAILURE;
		}

		display_super_block(sb);

		// load the fat
		int fat_pos = sb->block_size;
		if (fseek(fd, fat_pos, SEEK_SET) != 0) {
			fprintf(stderr, "Failure in seeking in file\n");
			return EXIT_FAILURE;
		}
		int* fat = malloc(sb->fat_len * sizeof(int));
		if (fread(fat, sizeof(int), sb->fat_len, fd) == 0) {
			fprintf(stderr, "Failure in reading fat table\n");
			return EXIT_FAILURE;
		}
		printf("fat table: \n|");
		for (int i = 0; i < sb->fat_len; i++) {
			printf("%d|", fat[i]);
		}
		printf("\n");


		// find the first empty place to write the new dir_entry
		dir_entry_t* temp = malloc(sizeof(dir_entry_t));
		if (temp == NULL) {
			fprintf(stderr, "Failure in allocating memory!!\n");
			return 0;
		}
		int last_pos = 0;
		int pos = sb->first_dir_entry * sb->block_size;
		int readed_data = 0;
		do {
			// int pos = sb->block_size * (fat[current_block_idx] == 0 ? current_block_idx : fat[current_block_idx]);
			last_pos = pos / sb->block_size;
			printf("pos: %d\n", pos);
			if (fseek(fd, pos, SEEK_SET) != 0) {
				fprintf(stderr, "Failure in seeking in file\n");
				return EXIT_FAILURE;
			}
			do {
				int data_len;
				if ((data_len = fread(temp, sizeof(dir_entry_t), 1, fd)) == 0) {
					fprintf(stderr, "Failure in reading dir_entry\n");
					return EXIT_FAILURE;
				}
				readed_data += sizeof(dir_entry_t);
				printf("dir:\n\tname: %s\n\tstart: %d\n", temp->name, temp->start);
				if (strcmp(temp->name, argv[1]) == 0) {
					fprintf(stderr, "File with name \"%s\" already exists in the file system!\n", argv[1]);
					exit (EXIT_FAILURE);
				}
			} while (temp->start != 0 && readed_data < sb->block_size); // temp->start is zero means that the place is available
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
			int next_idx = get_next_available_block(fat, sb->fat_len, sb->first_dir_entry);
			if (next_idx == 0) {
				fprintf(stderr, "No space available for the meta-data!\n");
				exit (EXIT_FAILURE);
			}
			fat[last_pos] = next_idx;
			fat[next_idx] = 0;
			if (fseek(fd, next_idx * sb->block_size, SEEK_SET) != 0) {
				fprintf(stderr, "Failure in seeking in file\n");
				return EXIT_FAILURE;
			}
		} else {
			if (fseek(fd, -sizeof(dir_entry_t), SEEK_CUR) != 0) {
				fprintf(stderr, "Failure in seeking in file\n");
				return EXIT_FAILURE;
			}
		}


		// updating the fat
		int file_size = get_file_size(argv[1]);
		printf("file_size: %d\n", file_size);
		int file_needed_block = file_size / sb->block_size + (file_size % sb->block_size == 0 ? 0 : 1);
		printf("file_needed_block: %d\n", file_needed_block);

		int available_blocks[file_needed_block]; 
		get_available_blocks(available_blocks, fat, sb->first_dir_entry, sb->fat_len, file_needed_block);
		for (int i = 0; i < file_needed_block; i++)	{
			printf("%d|", available_blocks[i]);
		}
		printf("\n");

		update_fat(fat, available_blocks, file_needed_block);

		printf("fat table: \n|");
		for (int i = 0; i < sb->fat_len; i++) {
			printf("%d|", fat[i]);
		}
		printf("\n");




		// write the new dir_entry in the fs
		dir_entry_t* dir_entry = malloc(sizeof(dir_entry_t));
		if (dir_entry == NULL) {
			fprintf(stderr, "Failure in allocating memory!!\n");
			return 0;
		}
		strcpy(dir_entry->name, argv[1]);
		dir_entry->size = file_size;
		dir_entry->start = available_blocks[0];

		if (fwrite(dir_entry, sizeof(dir_entry_t), 1, fd) == 0) {
			fprintf(stderr, "Failure in writing data!!\n");
			return 0;
		}

		// write the fat in the fs
		pos = sb->block_size;
		if (fseek(fd, pos, SEEK_SET) != 0) {
			fprintf(stderr, "Failure in seeking in file\n");
			return EXIT_FAILURE;
		}
		if (fwrite(fat, sizeof(int), sb->fat_len, fd) == 0) {
			fprintf(stderr, "Failure in writing data!\n");
			return 0;
		}

		// write the file content in the fs
		// need to be ameliorated because it write only in one block
		FILE* fd2 = fopen(argv[1], "r");
		if (fd2 == NULL) {
			fprintf(stderr, "Failure in opening file \"%s\" (in r+ mode)!\n", argv[1]);
		}

		for (int i = 0; i < file_needed_block; i++) {
			char buffer[sb->block_size];
			int buffer_real_size;
			if ((buffer_real_size = fread(buffer, sizeof(char), sb->block_size, fd2)) == 0) {
				fprintf(stderr, "Failure in reading fat table\n");
				return EXIT_FAILURE;
			}
			printf("buffer_real_size: %d\n", buffer_real_size);
			pos = sb->block_size * available_blocks[i];
			if (fseek(fd, pos, SEEK_SET) != 0) {
				fprintf(stderr, "Failure in seeking in file\n");
				return EXIT_FAILURE;
			}
			if (fwrite(buffer, sizeof(char), buffer_real_size, fd) == 0) {
				fprintf(stderr, "Failure in writing data!!\n");
				return 0;
			}
		}

		fclose(fd);

		return EXIT_SUCCESS;

	} else {

		fprintf(stderr, "\nfs_add <file> <fs_name>\n\n"
					"• file : Name of the file to be added to the file system\n"
					"• fs_name : File system name\n\n"
					"Example: fs_add hello.txt fs.img\n\n");
		return EXIT_FAILURE;
	}
}
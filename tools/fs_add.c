#include "fs.h"

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

int main(int argc, char *argv[]) {
	if (argc == 3) {
		if (!valid_arguments(argv[1], argv[2])) {
			return EXIT_FAILURE;
		}

		FILE* fd = fopen(argv[2], "r+");
		if (fd == NULL) {
			fprintf(stderr, "Failure in opening file \"%s\" (in r+ mode)!\n", argv[1]);
		}

		super_block_t* super_block = malloc(sizeof(super_block_t));
		if (fread(super_block, sizeof(super_block_t), 1, fd) == 0) {
			fprintf(stderr, "Failure in reading super_block\n");
			return EXIT_FAILURE;
		}

		display_super_block(super_block);

		int fat_pos = super_block->block_size;
		if (fseek(fd, fat_pos, SEEK_SET) != 0) {
			fprintf(stderr, "Failure in seeking in file\n");
			return EXIT_FAILURE;
		}
		int* fat = malloc(super_block->fat_len * sizeof(int));
		if (fread(fat, sizeof(int), super_block->fat_len, fd) == 0) {
			fprintf(stderr, "Failure in reading fat table\n");
			return EXIT_FAILURE;
		}
		printf("fat table: \n|");
		for (int i = 0; i < super_block->fat_len; i++) {
			printf("%d|", fat[i]);
		}
		printf("\n");

		dir_entry_t* dir_entry = malloc(sizeof(dir_entry_t));
		if (dir_entry == NULL) {
			fprintf(stderr, "Failure in allocating memory!!\n");
			return 0;
		}
		strcpy(dir_entry->name, argv[1]);
		int i = super_block->first_dir_entry + 1;
		for (; i < super_block->fat_len; i++) {
			if (fat[i] == (int)0xffffffff) {
				dir_entry->start = i;
				break;
			}
		}
		if (i == super_block->fat_len) {
			fprintf(stderr, "Failure in adding file : No space remains\n");
			return EXIT_FAILURE;
		}
		fat[dir_entry->start] = 0;
		printf("dir_entry->start: %d\n", dir_entry->start);

		int pos = super_block->block_size;
		if (fseek(fd, pos, SEEK_SET) != 0) {
			fprintf(stderr, "Failure in seeking in file\n");
			return EXIT_FAILURE;
		}
		if (fwrite(fat, sizeof(int), super_block->fat_len, fd) == 0) {
			fprintf(stderr, "Failure in writing data!\n");
			return 0;
		}
		printf("fat table: \n|");
		for (int i = 0; i < super_block->fat_len; i++) {
			printf("%d|", fat[i]);
		}
		printf("\n");

		// make the cursor in the first empty place in the block
		pos = super_block->block_size * super_block->first_dir_entry;
		printf("pos: %d\n", pos);
		if (fseek(fd, pos, SEEK_SET) != 0) {
			fprintf(stderr, "Failure in seeking in file\n");
			return EXIT_FAILURE;
		}
		dir_entry_t* temp = malloc(sizeof(dir_entry_t));
		if (temp == NULL) {
			fprintf(stderr, "Failure in allocating memory!!\n");
			return 0;
		}
		do {
			if (fread(temp, sizeof(dir_entry_t), 1, fd) == 0) {
				fprintf(stderr, "Failure in reading fat table\n");
				return EXIT_FAILURE;
			}
			printf("dir:\n\tname: %s\n\tstart: %d\n", temp->name, temp->start);
		} while (temp->start != 0);
		if (fseek(fd, -sizeof(dir_entry_t), SEEK_CUR) != 0) {
			fprintf(stderr, "Failure in seeking in file\n");
			return EXIT_FAILURE;
		}
		if (fwrite(dir_entry, sizeof(dir_entry_t), 1, fd) == 0) {
			fprintf(stderr, "Failure in writing data!!\n");
			return 0;
		}

		// write the file content in the fs
		// need to be ameliorated because it write only in one block
		FILE* fd2 = fopen(argv[1], "r");
		if (fd2 == NULL) {
			fprintf(stderr, "Failure in opening file \"%s\" (in r+ mode)!\n", argv[1]);
		}
		char buffer[super_block->block_size];
		int buffer_real_size;
		if ((buffer_real_size = fread(buffer, sizeof(char), super_block->block_size, fd2)) == 0) {
			fprintf(stderr, "Failure in reading fat table\n");
			return EXIT_FAILURE;
		}
		pos = super_block->block_size * dir_entry->start;
		if (fseek(fd, pos, SEEK_SET) != 0) {
			fprintf(stderr, "Failure in seeking in file\n");
			return EXIT_FAILURE;
		}
		if (fwrite(buffer, sizeof(char), buffer_real_size, fd) == 0) {
			fprintf(stderr, "Failure in writing data!!\n");
			return 0;
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
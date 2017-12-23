/**
 * @file 		fs_del.c
 * @brief 		Tool for deleting one file in image file system.
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
 * @brief  Entry point of program, load differents structures and delete the file to fs.
 *
 * @param  argc the number of arguments
 * @param  argc the array of arguments
 * @return EXIT_SUCCESS or EXIT_FAILURE in case of success or failure
 */
int main(int argc, char *argv[]) {
	if (argc == 3) {
		CHECK_ERR(strlen(argv[1]) >= ENTRY_NAME_SIZE, "file_name too long, must be <= %d\n",
			ENTRY_NAME_SIZE - 1)

		FILE* fd = fopen(argv[2], "r+");
		CHECK_ERR(fd == NULL, "Failure in opening file \"%s\" (in r+ mode)!\n", argv[2])

		super_block_t* sb;
		CHECK_ERR(load_super_block(fd, &sb), "Failure in allocating super block!\n")

		int* fat;
		CHECK_ERR(load_fat(fd, sb, &fat), "Failure in allocating fat!\n")

		// Read through the fat
		int pos = sb->first_entry * sb->block_size;
		int last_pos = 0;
		int readed_data = 0;
		bool found = false;
		entry_t* entry = malloc(sizeof(entry_t));
		CHECK_ERR(entry == NULL, "Failure in allocating memory!!\n")
		do {
			readed_data = 0;
			last_pos = pos / sb->block_size;
			CHECK_ERR(fseek(fd, pos, SEEK_SET) != 0, "Seeking file failed!\n")

			do {
				int data_len;
				CHECK_ERR((data_len = fread(entry, sizeof(entry_t), 1, fd)) == 0,
					"Failure in reading entry\n")
				readed_data += sizeof(entry_t);

				// Delete blocks in fat
				if (fat[entry->start] != -1 && strcmp(entry->name, argv[1]) == 0) {
					found = true;
					int index = entry->start;
					int new_index;
					do {
						new_index = fat[index];
						fat[index] = -1;
						index = new_index;
					} while (new_index != 0);
				}
			} while (entry->start != 0 && readed_data < sb->block_size && !found);
			pos = sb->block_size * fat[last_pos];
		} while (pos != 0 && !found);

		if (found) {
			printf("%s has been deleted ! (size: %d bytes, start block: %d)\n", argv[1], 
				entry->size, entry->start);
		}
		else {
			printf("%s not found in file system !\n", argv[1]);
		}

		free(entry);

		// Write the fat in the fs
		pos = sb->block_size;
		CHECK_ERR(fseek(fd, pos, SEEK_SET) != 0, "Seeking file failed!\n")
		CHECK_ERR(fwrite(fat, sizeof(int), sb->blocks_count, fd) == 0, "Failure in writing data!\n")

		free(sb);
		free(fat);
		fclose(fd);
		return EXIT_SUCCESS;
	}
	else {
		fprintf(stderr, "\nfs_del <file> <fs_name>\n\n"
			"• file : Name of the file to be deleted in the fs\n"
			"• fs_name : File system name\n\n"
			"Example: fs_del hello.txt fs.img\n\n");
		return EXIT_FAILURE;
	}
}

#include "tools.h"

int main(int argc, char *argv[]) {
    if (argc == 3) {
        // check arguments
		char* file_name = malloc(sizeof(char) * strlen(argv[1]));
		CHECK_ERR(file_name == NULL, "Failure in allocating memory!!\n")
		strcpy(file_name, argv[1]);

		char* fs_name = malloc(sizeof(char) * strlen(argv[2]));
		CHECK_ERR(fs_name == NULL, "Failure in allocating memory!!\n")
		strcpy(fs_name, argv[2]);

        // FILE* fd = fopen(fs_name, "r");
        // CHECK_ERR(fd == NULL, "error: File system with name \"%s\" doesn't exist\n", fs_name)

		FILE* fd = fopen(fs_name, "r+");
		CHECK_ERR(fd == NULL, "Failure in opening file \"%s\" (in r+ mode)!\n", fs_name)

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
        
        return EXIT_SUCCESS;
    }
    else {
        fprintf(stderr, "\nfs_del <file> <fs_name>\n\n"
                "• file : Name of the file to be deleted in the fs\n"
                "• fs_name : File system name\n\n"
                "Example: fs_add hello.txt fs.img\n\n");
        return EXIT_FAILURE;
    }
}

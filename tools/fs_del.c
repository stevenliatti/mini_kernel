#include "tools.h"

int main(int argc, char *argv[]) {
    if (argc == 3) {
        // check arguments
        CHECK_ERR(strlen(argv[1]) >= ENTRY_NAME_SIZE, "file_name too long, must be <= %d\n", 
		    ENTRY_NAME_SIZE - 1)
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


        int last_pos = 0;
        int pos = sb->first_dir_entry * sb->block_size;
        int readed_data = 0;
        bool found = false;
        dir_entry_t* entry = malloc(sizeof(dir_entry_t));
        CHECK_ERR(entry == NULL, "Failure in allocating memory!!\n")
        do {
            readed_data = 0;
            last_pos = pos / sb->block_size;
            printf("pos: %d\n", pos);
            CHECK_ERR(fseek(fd, pos, SEEK_SET) != 0, "Seeking file failed!\n")
            
            do {
                int data_len;
                CHECK_ERR((data_len = fread(entry, sizeof(dir_entry_t), 1, fd)) == 0, 
                    "Failure in reading dir_entry\n")
                readed_data += sizeof(dir_entry_t);
                printf("dir:\n\tname: %s\n\tstart: %d\n", entry->name, entry->start);
                
                // Delete file
                if (fat[entry->start] != -1 && strcmp(entry->name, file_name) == 0) {
                    found = true;
                    int index = entry->start;
                    int new_index;
                    do {
                        new_index = fat[index];
                        fat[index] = -1;
                        index = new_index;
                    } while(new_index != 0);
                    // entry->start = 0;
                    // CHECK_ERR(fwrite(entry, sizeof(dir_entry_t), 1, fd) == 0, "Failure in writing data!!\n")
                }
            } while (fat[entry->start] != -1 && readed_data < sb->block_size && !found);
            pos = sb->block_size * fat[last_pos];
        } while (pos != 0 && !found);

        // write the fat in the fs
        pos = sb->block_size;
        CHECK_ERR(fseek(fd, pos, SEEK_SET) != 0, "Seeking file failed!\n")
        CHECK_ERR(fwrite(fat, sizeof(int), sb->fat_len, fd) == 0, "Failure in writing data!\n")
        print_fat(fat, sb->fat_len);
        
        fclose(fd);
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

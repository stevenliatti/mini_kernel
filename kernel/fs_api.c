/**
 * @file 		fs_api.c
 * @brief 		File system API functions.
 *
 * @author 		Steven Liatti
 * @author 		Raed Abdennadher
 * @bug 		No known bugs.
 * @date 		December 23, 2017
 * @version		1.0
 */

#include "fs_api.h"
#include "ide.h"
#include "screen.h"
#include "base.h"

// from kernel.c
extern super_block_t sb;
extern int* fat;
extern char sector_per_block;
file_descriptor_t file_descriptor[DESCRIPTORS_NB];

/**
 * @brief  Init array of file descriptor.
 * 
 * @return value in memory video
 */
void init_file_descriptor() {
	for (int i = 0; i < DESCRIPTORS_NB; i++) {
		file_descriptor[i].is_free = true;
		file_descriptor[i].readed_bytes = 0;
	}
}

/**
 * @brief  Calculate and return the next offset of next entry.
 * 
 * @param  it a pointer on file_iterator
 * @return the next offset of next entry or -1 if there is not anymore entries
 */
static int get_next_entry_offset(file_iterator_t* it) {
	int entry_offset_in_block = it->entry_offset_in_current_block + sizeof(entry_t);
	int block_index = it->current_block;

	if (entry_offset_in_block == sb.block_size) {
		block_index = fat[it->current_block];
		entry_offset_in_block = 0;
	}
	if (block_index == 0) {
		return -1;
	}

	do {
		int sector_index = block_index * sector_per_block;
		int start_sector = entry_offset_in_block / SECTOR_SIZE;

		for (int i = start_sector; i < sector_per_block; i++) {
			char buffer[SECTOR_SIZE];
			read_sector(sector_index + i, buffer);

			int entry_offset_in_sector = entry_offset_in_block % SECTOR_SIZE;
			for (int j = entry_offset_in_sector; j < SECTOR_SIZE; j += sizeof(entry_t)) {
				entry_t temp_entry;
				memcpy(&temp_entry, buffer + j, sizeof(entry_t));
				if (fat[temp_entry.start] != -1) {
					return block_index * sb.block_size + j + (i * SECTOR_SIZE);
				}
			}
		}
		block_index = fat[block_index];
		entry_offset_in_block = 0;
	} while (block_index != 0);
	return -1;
}

/**
 * @brief  Return the entry at given offset
 * 
 * @param  offset in bytes
 * @return the entry at given offset
 */
static entry_t get_entry(int offset) {
	char buffer[SECTOR_SIZE];
	read_sector(offset / SECTOR_SIZE, buffer);
	int entry_offset_in_sector = offset % SECTOR_SIZE;
	entry_t entry;
	memcpy(&entry, buffer + entry_offset_in_sector, sizeof(entry_t));
	return entry;
}

/**
 * @brief  Create an iterator which allows to iterate on file system's files.
 * 
 * @return an iterator
 */
file_iterator_t file_iterator() {
	file_iterator_t it;
	it.entry_offset_in_current_block = -sizeof(entry_t);
	it.current_block = sb.first_entry;
	return it;
}

/**
 * @brief  Return true if there is still one file to iterate over.
 * 
 * @param  it a pointer on file_iterator
 * @return true if there is still one file to iterate over, false otherwise
 */
bool file_has_next(file_iterator_t *it) {
	int next_offset = get_next_entry_offset(it);
	return next_offset != -1;
}

/**
 * @brief  Copy in filename the name of next file pointed by it.
 * 
 * @param  filename the file name
 * @param  it a pointer on file_iterator
 */
void file_next(char *filename, file_iterator_t *it) {
	int next_offset = get_next_entry_offset(it);
    if (next_offset == -1)
        memset(filename, 0, ENTRY_NAME_SIZE);
    else {
        entry_t next_entry = get_entry(next_offset);
        memcpy(filename, next_entry.name, ENTRY_NAME_SIZE);
    }
    it->entry_offset_in_current_block = next_offset % sb.block_size;
    it->current_block = next_offset / sb.block_size;
}

/**
 * @brief  Return the number of occupied blocks by a file.
 * 
 * @param  start index of first block
 * @return the number of occupied blocks by a file
 */
static int get_file_blocks_nb(int start) {
	int blocks_count = 1;
	while (fat[start] != 0) {
		start = fat[start];
		blocks_count++;
	}
	return blocks_count;
}

/**
 * @brief  Fill stat with infos about the file in argument.
 * 
 * @param  filename the file name
 * @param  stat a structure to store infos about file
 * @return 0 in case of success, -1 otherwise
 */
int file_stat(char *filename, stat_t *stat) {
	file_iterator_t it = file_iterator();
	int next_offset = 0;
	while ((next_offset = get_next_entry_offset(&it)) != -1) {
		entry_t entry = get_entry(next_offset);
		if (strcmp(filename, entry.name) == 0) {
			stat->size = entry.size;
			memcpy(stat->name, filename, ENTRY_NAME_SIZE);
			stat->used_blocks_nb = get_file_blocks_nb(entry.start);
			stat->start = entry.start;
			return 0;
		}
		it.entry_offset_in_current_block = next_offset % sb.block_size;
		it.current_block = next_offset / sb.block_size;
	}
	return -1;
}

/**
 * @brief  Return true if the file exist.
 * 
 * @param  filename the file name
 * @return true if the file exist, false otherwise
 */
bool file_exists(char *filename) {
	file_iterator_t it = file_iterator();
	int next_offset = 0;
	while ((next_offset = get_next_entry_offset(&it)) != -1) {
		entry_t entry = get_entry(next_offset);
		if (strcmp(filename, entry.name) == 0) {
			return true;
		}
		it.entry_offset_in_current_block = next_offset % sb.block_size;
		it.current_block = next_offset / sb.block_size;
	}
	return false;
}

/**
 * @brief  Return the next file descriptor availaible.
 * 
 * @return the next file descriptor availaible, -1 otherwise
 */
static int get_free_fd() {
	for (int i = 0; i < DESCRIPTORS_NB; i++) {
		if (file_descriptor[i].is_free) {
			return i;
		}
	}
	return -1;
}

/**
 * @brief  Open a file and return a file descriptor to access it.
 * 
 * @param  filename the file name
 * @return a file descriptor to access it, -1 otherwise
 */
int file_open(char *filename) {
	if (file_exists(filename)) {
		stat_t st;
		if (file_stat(filename, &st) == -1) {
			printf("Error in file_stat\n");
			return -1;
		}
		int fd = get_free_fd();
		file_descriptor[fd].is_free = false;
		file_descriptor[fd].start_block = st.start;
		file_descriptor[fd].current_offset_in_block = 0;
		file_descriptor[fd].current_block = st.start;
		file_descriptor[fd].file_size = st.size;
		return fd;
	}
	return -1;
}

/**
 * @brief  Try to read count bytes from file pointed by fd and put it in buf.
 * Return the number of readed bytes, zero if we are at the end of file or -1 if error.
 * 
 * @param  fd the file descriptor
 * @param  buf a buffer to store file data
 * @param  count the number of desired bytes to read
 * @return the number of readed bytes, zero if we are at the end of file or -1 if error
 */
int file_read(int fd, void *buf, uint count) {
	if (fd >= 0 && fd < DESCRIPTORS_NB) {
		if (file_descriptor[fd].is_free) {
			printf("The file descriptor in unknown (file_read)\n");
			return -1;
		}

        int buf_size = count;
		uint rest_bytes = (uint) (file_descriptor[fd].file_size - file_descriptor[fd].readed_bytes);
		if (rest_bytes == 0) {
			return 0;
		}

		// if count is greater than the rest of the file
		if (count > rest_bytes) {
			count = rest_bytes;
		}
		int bytes_count = count;
		int buf_index = 0;
		int current_offset_in_block = file_descriptor[fd].current_offset_in_block;

		while (count > 0) {
			int sector_index = file_descriptor[fd].current_block * sector_per_block + 
				current_offset_in_block / SECTOR_SIZE;
			int offset_in_sector = current_offset_in_block % SECTOR_SIZE;

			char buffer[SECTOR_SIZE];
			read_sector(sector_index, buffer);
			if (offset_in_sector + count >= SECTOR_SIZE) {
				int difference = SECTOR_SIZE - offset_in_sector;
				memcpy(buf + buf_index, buffer + offset_in_sector, difference);
				count -= difference;
				buf_index += difference;
				current_offset_in_block += difference;
			}
			else {
				memcpy(buf + buf_index, buffer + offset_in_sector, count);
				buf_index += count;
				current_offset_in_block += count;
				count = 0;
			}
			if (current_offset_in_block % sb.block_size == 0) {
				file_descriptor[fd].current_block = fat[file_descriptor[fd].current_block];
				current_offset_in_block = 0;
			}
		}
        if (bytes_count < buf_size) {
            memset(buf + bytes_count, 0, buf_size - bytes_count);
        }
		file_descriptor[fd].current_offset_in_block = current_offset_in_block;
		file_descriptor[fd].readed_bytes += bytes_count;
		return bytes_count;
	}
	return -1;
}

/**
 * @brief  Set position of cursor on file to offset in argument compared to the begin of file.
 * Return the new position or -1 of fail.
 * 
 * @param  fd the file descriptor
 * @param  offset the offset where to place the cursor
 * @return the new position or -1 of fail
 */
int file_seek(int fd, uint offset) {
	if (fd >= 0 && fd < DESCRIPTORS_NB) {
		if (file_descriptor[fd].is_free) {
			printf("The file descriptor in unknown (file_seek)\n");
			return -1;
		}
		if (offset > (uint) file_descriptor[fd].file_size) {
			offset = file_descriptor[fd].file_size;
		}

		int seek_block = offset / sb.block_size;
		int index = file_descriptor[fd].start_block;
		for (int i = 0; i < seek_block; i++) {
			index = fat[index];
		}
		file_descriptor[fd].current_block = index;
		file_descriptor[fd].current_offset_in_block = offset % sb.block_size;
		file_descriptor[fd].readed_bytes = offset;
		return offset;
	}
	return -1;
}

/**
 * @brief  Close the file pointed by fd.
 *
 * @param  fd the file descriptor
 */
void file_close(int fd) {
	if (fd >= 0 && fd < DESCRIPTORS_NB) {
		file_descriptor[fd].is_free = true;
	}
	else {
		printf("Error in closing file descriptor\n");
	}
}

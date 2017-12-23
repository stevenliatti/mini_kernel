/**
 * @file 		fs_api.h
 * @brief 		Header of fs api functions.
 *
 * @author 		Steven Liatti
 * @author 		Raed Abdennadher
 * @bug 		No known bugs.
 * @date 		December 23, 2017
 * @version		1.0
 */

#ifndef _FS_API_H_
#define _FS_API_H_

#include "../common/common.h"
#include "../common/types.h"

#define DESCRIPTORS_NB 100

/**
 * @brief This structure represent the stat's file, with name, size,
 * blocks occupied and the first block of data (start).
 */
typedef struct stat_st {
	char name[ENTRY_NAME_SIZE];
	int size;
	int used_blocks_nb;
	int start;
} __attribute__((packed)) stat_t;

/**
 * @brief This structure represent a file iterator, with the offset of entry 
 * in bytes inside in a block and the current block.
 */
typedef struct file_iterator_st {
	int entry_offset_in_current_block;
	int current_block;
} __attribute__((packed)) file_iterator_t;

/**
 * @brief This structure represent a file descriptor, with the first data's block 
 * of file, the current offset (in bytes) in current block, the current block, the 
 * file's size, a boolean if it's free or not and the number of bytes already readed.
 */
typedef struct file_descriptor_st {
	int start_block;
	int current_offset_in_block;
	int current_block;
	int file_size;
	bool is_free;
	int readed_bytes;
} __attribute__((packed)) file_descriptor_t;

extern void init_file_descriptor();
extern file_iterator_t file_iterator();
extern bool file_has_next(file_iterator_t *it);
extern void file_next(char *filename, file_iterator_t *it);
extern int file_stat(char *filename, stat_t *stat);
extern bool file_exists(char *filename);
extern int file_open(char *filename);
extern int file_read(int fd, void *buf, uint count);
extern int file_seek(int fd, uint offset);
extern void file_close(int fd);

#endif

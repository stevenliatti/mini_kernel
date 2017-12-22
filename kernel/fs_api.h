#ifndef _FS_API_H_
#define _FS_API_H_

#include "../common/common.h"
#include "../common/types.h"

typedef struct stat_st {
	char name[ENTRY_NAME_SIZE];
	int size;
	int used_blocks_nb;
	int start;
	int block_size;
} __attribute__((packed)) stat_t;

typedef struct file_iterator_st {
	int entry_offset_in_current_block;		// offset of the current entry in the current block
	int current_block;
} __attribute__((packed)) file_iterator_t;

file_iterator_t file_iterator();
bool file_has_next(file_iterator_t *it);
void file_next(char *filename, file_iterator_t *it);
int file_stat(char *filename, stat_t *stat);
bool file_exists(char *filename);
int file_open(char *filename);
int file_read(int fd, void *buf, uint count);
int file_seek(int fd, uint offset);
void file_close(int fd);

#endif

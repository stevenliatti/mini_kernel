#ifndef _FS_API_H_
#define _FS_API_H_

#include "../common/common.h"
#include "../common/types.h"

typedef entry_t stat_t;

typedef struct file_iterator_st {
	int current_entry_offset;		// offset of the current entry in the fs
	int next_entry_offset;			// offset of the next entry in the fs
} __attribute__((packed)) file_iterator_t;

void load_super_block();
void load_fat();

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

#ifndef _FS_API_H_
#define _FS_API_H_

#include "../common/common.h"
#include "../common/types.h"

typedef dir_entry_t stat_t;
typedef dir_entry_t file_iterator_t;

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
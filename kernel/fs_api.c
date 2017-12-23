#include "fs_api.h"
#include "ide.h"
#include "screen.h"
#include "base.h"

// from kernel.c
extern super_block_t sb;
extern int* fat;
extern char sector_per_block;
static file_descriptor_t file_descriptor[DESCRIPTORS_NB];

void init_file_descriptor() {
	for (int i = 0; i < DESCRIPTORS_NB; i++) {
		file_descriptor[i].is_free = true;
		file_descriptor[i].readed_bytes = 0;
	}
}

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

static entry_t get_entry(int offset) {
	char buffer[SECTOR_SIZE];
	read_sector(offset / SECTOR_SIZE, buffer);
	int entry_offset_in_sector = offset % SECTOR_SIZE;
	entry_t entry;
	memcpy(&entry, buffer + entry_offset_in_sector, sizeof(entry_t));
	return entry;
}

// Créé un itérateur permettant d'itérer sur les fichiers du système de fichiers.
file_iterator_t file_iterator() {
	// create iterator
	file_iterator_t it;
	it.entry_offset_in_current_block = -sizeof(entry_t);
	it.current_block = sb.first_entry;
	return it;
}

// Renvoie true si il y a encore un fichier sur lequel itérer.
bool file_has_next(file_iterator_t *it) {
	int next_offset = get_next_entry_offset(it);
	return next_offset != -1;
}

// Copie dans filename le nom du prochain fichier pointé par l’itérateur.
void file_next(char *filename, file_iterator_t *it) {
	int next_offset = get_next_entry_offset(it);
	entry_t next_entry = get_entry(next_offset);
	memcpy(filename, next_entry.name, ENTRY_NAME_SIZE);
	it->entry_offset_in_current_block = next_offset % sb.block_size;
	it->current_block = next_offset / sb.block_size;
}

static int get_file_blocks_nb(int start) {
	int blocks_count = 1;
	while (fat[start] != 0) {
		start = fat[start];
		blocks_count++;
	}
	return blocks_count;
}

// Renvoie dans stat les méta-informations liées au fichier passé en argument ; la structure
// stat_t doit contenir au minimum le champ size qui est la taille du fichier. Retourne 0 en
// cas de succès et -1 en cas d'échec.
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

// Renvoie true si le fichier passé en argument existe.
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

static int get_free_fd() {
	for (int i = 0; i < DESCRIPTORS_NB; i++) {
		if (file_descriptor[i].is_free) {
			return i;
		}
	}
	return -1;
}

// Ouvre un fichier et renvoie un descripteur de fichier pour y accéder ou -1 en cas d'échec.
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

// Essaie de lire count bytes depuis le fichier référencé par fd et les place dans le buffer
// buf. Renvoie le nombre de bytes lus, ou 0 en cas de fin de fichier, ou -1 en cas d’erreur.
int file_read(int fd, void *buf, uint count) {
	if (fd >= 0 && fd < DESCRIPTORS_NB) {
		if (file_descriptor[fd].is_free) {
			printf("The file descriptor in unknown (file_read)\n");
			return -1;
		}
		uint rest_bytes = (uint) (file_descriptor[fd].file_size - file_descriptor[fd].readed_bytes);
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
			if (count >= SECTOR_SIZE) {
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
			if (buf_index % sb.block_size == 0) {
				file_descriptor[fd].current_block = fat[file_descriptor[fd].current_block];
				current_offset_in_block = 0;
			}
		}
		file_descriptor[fd].current_offset_in_block += buf_index;
		file_descriptor[fd].readed_bytes += bytes_count;
		return bytes_count;
	}
	return -1;
}

// Positionne la position pointeur du fichier ouvert (référencé par le descripteur fd) à offset
// par rapport au début du fichier. Renvoie la nouvelle position ou -1 en cas d’échec.
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

// Ferme le fichier référencé par le descripteur fd.
void file_close(int fd) {
	if (fd >= 0 && fd < DESCRIPTORS_NB) {
		file_descriptor[fd].is_free = true;
	}
	else {
		printf("Error in closing file descriptor\n");
	}
}

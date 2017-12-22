#include "fs_api.h"
#include "ide.h"
#include "screen.h"
#include "base.h"

#define DESCRIPTORS_NB 100

// from kernel.c
extern super_block_t sb;
extern int* fat;
extern char sector_per_block;

typedef struct file_descriptor_st {
	int current_offset_in_block;
	int current_block;
	stat_t st;
} file_descriptor_t;

static file_descriptor_t file_descriptor[DESCRIPTORS_NB];

void init_file_descriptor() {
	for (int i = 0; i < DESCRIPTORS_NB; i++) {
		file_descriptor[i].current_offset_in_block = -1;
		file_descriptor[i].current_block = -1;
		file_descriptor[i].st.start = -1;
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

entry_t get_entry(int offset) {
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

int get_file_blocks_nb(int start) {
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
		if (strncmp(filename, entry.name, ENTRY_NAME_SIZE) == 0) {
			stat->size = entry.size;
			stat->block_size = sb.block_size;
			memcpy(stat->name, filename, ENTRY_NAME_SIZE);
			stat->used_blocks_nb = get_file_blocks_nb(entry.start);
			return 0;
		}
        it.entry_offset_in_current_block = next_offset % sb.block_size;
        it.current_block = next_offset / sb.block_size;
	}
	return -1;
}

// Renvoie true si le fichier passé en argument existe.
bool file_exists(char *filename) {
	return false;
}

// Ouvre un fichier et renvoie un descripteur de fichier pour y accéder ou -1 en cas d'échec.
int file_open(char *filename) {
	if (file_exists(filename)) {
		stat_t st;
		file_iterator_t it = file_iterator();
		bool found = false;
		stat_t temp_stat;
		while (file_has_next(&it)) {
			file_next(filename, &it);
			file_stat(filename, &st); // TODO: check file_stat or not ?
			if (strncmp(filename, st.name, ENTRY_NAME_SIZE) == 0) {
				found = true;
				temp_stat.size = st.size;
				temp_stat.start = st.start;
			}
		}

		if (found) {
			for (int i = 0; i < DESCRIPTORS_NB; i++) {
				if (file_descriptor[i].current_offset_in_block == -1) {
					file_descriptor[i].current_offset_in_block = 0;
					file_descriptor[i].current_block = temp_stat.start;
					memcpy(file_descriptor[i].st.name, filename, ENTRY_NAME_SIZE);
					file_descriptor[i].st.size = temp_stat.size;
					file_descriptor[i].st.start = temp_stat.start;
					return i;
				}
			}
		}
	}
	return -1;
}

// Essaie de lire count bytes depuis le fichier référencé par fd et les place dans le buffer
// buf. Renvoie le nombre de bytes lus, ou 0 en cas de fin de fichier, ou -1 en cas d’erreur.
int file_read(int fd, void *buf, uint count) {
	// TODO:
	if (fd >= 0 && fd < DESCRIPTORS_NB) {
		int readed_blocks = count / sb.block_size;
		char buffer[SECTOR_SIZE];
		// read_sector(file_descriptor[fd].current_block, buffer);
		// memcpy(buf, buffer + file_descriptor[fd].current_offset_in_block, count);
	}
	return -1;
}

// Positionne la position pointeur du fichier ouvert (référencé par le descripteur fd) à temp_stat
// par rapport au début du fichier. Renvoie la nouvelle position ou -1 en cas d’échec.
int file_seek(int fd, uint temp_stat) {
	if (fd >= 0 && fd < DESCRIPTORS_NB) { // TODO: check if fd already opened ?
		// TODO: use current_block and current_offset_in_block relative
		// file_descriptor[fd].current_offset_in_block = file_descriptor[fd].start_block + temp_stat;
		return file_descriptor[fd].current_offset_in_block;
	}
	return -1;
}

// Ferme le fichier référencé par le descripteur fd.
void file_close(int fd) {
	if (fd >= 0 && fd < DESCRIPTORS_NB) {
		file_descriptor[fd].current_offset_in_block = -1;
		// file_descriptor[fd].current_block = -1;
	}
}

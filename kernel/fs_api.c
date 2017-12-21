#include "fs_api.h"
#include "ide.h"
#include "screen.h"
#include "base.h"

#define DESCRIPTORS_NB 100

// from kernel.c
extern super_block_t sb;
extern int* fat;
extern char sector_per_block;

// typedef struct file_descriptor_st {
// 	int id;
	
// } __attribute__((packed)) file_descriptor_t;

static int file_descriptor[DESCRIPTORS_NB];

void init_file_descriptor() {
	for (int i = 0; i < DESCRIPTORS_NB; i++) {
		file_descriptor[i] = -1;
	}
}

static int get_current_entry_offset(int current_offset, int block_size) {
	int block_index = current_offset / block_size;
	int entry_offset_in_block = current_offset % block_size;
	int sector_index = block_index * sector_per_block;

	while(fat[block_index] != -1) {
		for (int i = 0; i < sector_per_block; i++) {
			char buffer[SECTOR_SIZE];
			read_sector(sector_index + i, buffer);
			for (int j = entry_offset_in_block; j < SECTOR_SIZE; j += sizeof(entry_t)) {
				entry_t temp;
				memcpy(&temp, buffer + j, sizeof(entry_t));
				if (fat[temp.start] != -1) {
					return block_index * block_size + j;
				}
			}
		}
		block_index = fat[block_index];
	}
	return -1;
}

// Créé un itérateur permettant d'itérer sur les fichiers du système de fichiers.
file_iterator_t file_iterator() {
	// create iterator
	file_iterator_t it;
	it.current_entry_offset = sb.first_entry * sb.block_size - sizeof(entry_t);
	return it;
}

// Renvoie true si il y a encore un fichier sur lequel itérer.
bool file_has_next(file_iterator_t *it) {
	int next_offset = get_current_entry_offset(it->current_entry_offset + sizeof(entry_t), sb.block_size);
	return next_offset != -1;
}

// Copie dans filename le nom du prochain fichier pointé par l’itérateur.
void file_next(char *filename, file_iterator_t *it) {
	int next_offset = get_current_entry_offset(it->current_entry_offset + sizeof(entry_t), sb.block_size);
	char buffer[SECTOR_SIZE];
	read_sector(next_offset / SECTOR_SIZE, buffer);
	int entry_offset_in_sector = next_offset % SECTOR_SIZE;
	entry_t next_entry;
	memcpy(&next_entry, buffer + entry_offset_in_sector, sizeof(entry_t));
	memcpy(filename, &(next_entry.name), ENTRY_NAME_SIZE);
	it->current_entry_offset = next_offset;
}

// Renvoie dans stat les méta-informations liées au fichier passé en argument ; la structure
// stat_t doit contenir au minimum le champ size qui est la taille du fichier. Retourne 0 en
// cas de succès et -1 en cas d'échec.
int file_stat(char *filename, stat_t *stat) {
	return 0;
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
		int offset = 0;
		while (file_has_next(&it)) {
			file_next(filename, &it);
			file_stat(filename, &st); // TODO: check file_stat or not ?
			if (strncmp(filename, st.name, ENTRY_NAME_SIZE) == 0 && fat[st.start] =! -1) {
				found = true;
				offset = sector_per_block * st.start;
			}
		}

		if (found) {
			for (int i = 0; i < DESCRIPTORS_NB; i++) {
				if (file_descriptor[i] == -1) {
					file_descriptor[i] = offset;
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
	return 0;
}

// Positionne la position pointeur du fichier ouvert (référencé par le descripteur fd) à offset
// par rapport au début du fichier. Renvoie la nouvelle position ou -1 en cas d’échec.
int file_seek(int fd, uint offset) {
	return 0;
}

// Ferme le fichier référencé par le descripteur fd.
void file_close(int fd) {
	if (fd >= 0 && fd < DESCRIPTORS_NB) {
		file_descriptor[fd] = -1;
	}
}

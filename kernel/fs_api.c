#include "fs_api.h"
#include "ide.h"
#include "screen.h"
#include "base.h"

// from kernel.c
extern super_block_t sb;
extern int* fat;
extern char sector_per_block;

static int get_next_entry_offset(file_iterator_t* it, char* comm) {
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
	int next_offset = get_next_entry_offset(it, "file_has_next");
	return next_offset != -1;
}

// Copie dans filename le nom du prochain fichier pointé par l’itérateur.
void file_next(char *filename, file_iterator_t *it) {
	int next_offset = get_next_entry_offset(it, "file_next");
	char buffer[SECTOR_SIZE];
	read_sector(next_offset / SECTOR_SIZE, buffer);
	int entry_offset_in_sector = next_offset % SECTOR_SIZE;
	entry_t next_entry;
	memcpy(&next_entry, buffer + entry_offset_in_sector, sizeof(entry_t));
	memcpy(filename, &(next_entry.name), ENTRY_NAME_SIZE);
	it->entry_offset_in_current_block = next_offset % sb.block_size;
	it->current_block = next_offset / sb.block_size;
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
	return 0;
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

}

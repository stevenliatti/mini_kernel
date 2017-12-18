#include "fs_api.h"
#include "ide.h"
#include "screen.h"
#include "base.h"

super_block_t sb;
int** fat;
char sector_per_block;

void load_super_block() {
	char buffer[SECTOR_SIZE];
	read_sector(0, buffer);
	memcpy(&sb, buffer, sizeof(super_block_t));
	sector_per_block = sb.block_size / SECTOR_SIZE;
}

void load_fat(int** fat_buffer) {
	int fat_sector = sector_per_block;		// just after the super block
	int sector_per_fat = sb.fat_block_nb * sector_per_block;

	char buffer[sector_per_fat * SECTOR_SIZE];
	for (int i = 0; i < sector_per_fat; i++) {
		read_sector(fat_sector + i, buffer + i * SECTOR_SIZE);
	}
	memcpy(*fat_buffer, buffer, sb.blocks_count * sizeof(int));
	fat = fat_buffer; // make fat point on the address of the first element of fat_buffer
}

static int get_next_entry_offset(int current_offset, int block_size, char sector_per_block) {
	int block_index = current_offset / block_size + (current_offset % block_size == 0 ? 0 : 1);
	// printf("block_index: %d\n", block_index);
	int sector_index = block_index * block_size / SECTOR_SIZE;
	// printf("sector_index: %d\n", sector_index);

	while(fat[block_index] != -1) {
		for (int i = 0; i < sector_per_block; i++) {
			char buffer[SECTOR_SIZE];
			read_sector(sector_index + i, buffer);
			for (int j = 0; j < SECTOR_SIZE; j += sizeof(entry_t)) {
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
	it.current_entry_offset = sb.first_entry * sb.block_size;
	it.next_entry_offset = get_next_entry_offset(it.current_entry_offset, sb.block_size, sector_per_block);
	return it;
}

// Renvoie true si il y a encore un fichier sur lequel itérer.
bool file_has_next(file_iterator_t *it) {
	int next_offset = get_next_entry_offset(it->current_entry_offset, sb.block_size, sector_per_block);
	return next_offset != -1;
}

// Copie dans filename le nom du prochain fichier pointé par l’itérateur.
void file_next(char *filename, file_iterator_t *it) {

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

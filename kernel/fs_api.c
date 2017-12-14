#include "fs_api.h"
#include "ide.h"
#include "screen.h"

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

// Créé un itérateur permettant d'itérer sur les fichiers du système de fichiers.
// file_iterator_t file_iterator();

// Renvoie true si il y a encore un fichier sur lequel itérer.
// bool file_has_next(file_iterator_t *it) {
// 	return false;
// }

// Copie dans filename le nom du prochain fichier pointé par l’itérateur.
// void file_next(char *filename, file_iterator_t *it) {

// }

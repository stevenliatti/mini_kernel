/**
 * @file 		kernel.c
 * @brief 		Entry point of kernel.
 *
 * @author 		Steven Liatti
 * @author 		Raed Abdennadher
 * @bug 		No known bugs.
 * @date 		November 3, 2017
 * @version		1.0
 */

#include "multiboot.h"
#include "gdt.h"
#include "screen.h"
#include "pic.h"
#include "timer.h"
#include "idt.h"
#include "x86.h"
#include "keyboard.h"
#include "test.h"
#include "../common/common.h"
#include "ide.h"
#include "fs_api.h"

super_block_t sb;
char sector_per_block;
int* fat;

void load_super_block() {
	char buffer[SECTOR_SIZE];
	read_sector(0, buffer);
	memcpy(&sb, buffer, sizeof(super_block_t));
}

void load_fat() {
	int fat_sector = sector_per_block;		// just after the super block
	int sector_per_fat = sb.fat_block_nb * sector_per_block;

	char buffer[sector_per_fat * SECTOR_SIZE];
	for (int i = 0; i < sector_per_fat; i++) {
		read_sector(fat_sector + i, buffer + i * SECTOR_SIZE);
	}
	memcpy(fat, buffer, sb.blocks_count * sizeof(int));
}

/**
 * @brief entry point of kernel. Mode test available
 * 
 * @param boot_info structure with informations about boot process
 */
void kernel_entry(multiboot_info_t* boot_info) {
	scr_init();
	gdt_init();
	idt_init();
	pic_init();
	timer_init(75);
	sti();

	#ifdef TEST_SCREEN
	test_screen();
	#elif TEST_TIMER
	test_timer();
	#else

	printf("Screen has been initialized.\n");
	printf("GDT has been initialized.\n");
	printf("IDT has been initialized.\n");
	printf("PIC has been initialized.\n");
	printf("Timer has been initialized.\n");
	printf("Memory upper : %d\n", boot_info->mem_upper);

	// load and display the super block
	load_super_block();
	print_super_block(sb);

	sector_per_block = (char) (sb.block_size / SECTOR_SIZE);
	
	// load and display the fat table
	load_fat();
	print_fat(fat, sb.blocks_count);

	file_iterator_t it = file_iterator();
	stat_t st;
	char filename[ENTRY_NAME_SIZE];
	while (file_has_next(&it)) {
		file_next(filename, &it);
		file_stat(filename, &st);
		printf("File name: %s\n", st.name);
		printf("Size: %d bytes      Used blocks nb: %d      Block size: %d\n", st.size, st.used_blocks_nb, sb.block_size);
	}

	char file1[] = "hello.txt";
	if (file_exists(file1)) {
		printf("File \"%s\" exists\n", file1);
	} else {
		printf("File \"%s\" don't exist\n", file1);
	}
	char file2[] = "x.txt";
	if (file_exists(file2)) {
		printf("File \"%s\" exists\n", file2);
	} else {
		printf("File \"%s\" don't exist\n", file2);
	}

	init_file_descriptor();

	int fd1 = -1;
	if ((fd1 = file_open(file1)) == -1) {
		printf("Error in opening file \"%s\"\n", file1);
	} else {
		printf("File \"%s\" oppened and fd = %d\n", file1, fd1);
		file_stat(file1, &st);
		printf("File name: %s\n", st.name);
		printf("Size: %d bytes      Used blocks nb: %d      Block size: %d\n", st.size, st.used_blocks_nb, sb.block_size);
	}

	int fd2 = -1;
	if ((fd2 = file_open("raed.txt")) == -1) {
		printf("Error in opening file \"%s\"\n", "raed.txt");
	} else {
		printf("File \"%s\" oppened and fd = %d\n", "raed.txt", fd2);
	}

	char str1[514]; // +1 for the character \0
	int bytes = file_read(fd1, str1, 513);
	printf("Readed (%d bytes):\n", bytes);
	printf("%s\n", str1);

	char str2[2001]; // +1 for the character \0
	bytes = file_read(fd1, str2, 2000);
	printf("Readed (%d bytes):\n", bytes);
	printf("%s\n", str2);
	
	while (1) {
		uchar c = getc();
		if (c == 'Q') {
			printf("\nShutdown");
			halt();
		}
		else {

			if (c != ND) {
				switch(c) {
					case ESC:
						printf("ESC\n");
						break;
					case TAB:
						printf("TAB\n");
						break;
					case CTRL:
						printf("CTRL\n");
						break;
					case ALT:
						printf("ALT\n");
						break;
					case ARROW_UP:
						printf("ARROW_UP\n");
						break;
					case ARROW_LEFT:
						printf("ARROW_LEFT\n");
						break;
					case ARROW_RIGHT:
						printf("ARROW_RIGHT\n");
						break;
					case ARROW_DOWN:
						printf("ARROW_DOWN\n");
						break;
					default:
						printf("%c", c);
						break;
				}
			}
		}
	}

	#endif
}

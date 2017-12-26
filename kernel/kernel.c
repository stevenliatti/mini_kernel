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

/**
 * @brief load the super block
 */
static void load_super_block() {
	char buffer[SECTOR_SIZE];
	read_sector(0, buffer);
	memcpy(&sb, buffer, sizeof(super_block_t));
}

/**
 * @brief load the FAT
 */
static void load_fat() {
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
	
	load_super_block();
	sector_per_block = (char) (sb.block_size / SECTOR_SIZE);
	load_fat();
	init_file_descriptor();

	#ifdef TEST_SCREEN
	test_screen();
	#elif TEST_TIMER
	test_timer();
	#elif TEST_FS
	test_fs();
	#else

	printf("Screen has been initialized.\n");
	printf("GDT has been initialized.\n");
	printf("IDT has been initialized.\n");
	printf("PIC has been initialized.\n");
	printf("Timer has been initialized.\n");
	printf("Memory upper : %d\n", boot_info->mem_upper);
	sleep(1000);

	printf("\nSuper block loaded\n");
	print_super_block(sb);
	sleep(1000);
	printf("\nFAT loaded\n");
	print_fat(fat, sb.blocks_count);
	
	for (int i = 4; i >= 0; i--) {
		printf("\nSplash screen in %d seconds ...\n", i);
		sleep(1000);
		scr_xy_t cursor_pos = get_cursor_pos();
		move_cursor(0, cursor_pos.y - 2);
	}
	
	char file1[] = "splash.txt";
	char file2[] = "splash2.txt";
	if (file_exists(file1) && file_exists(file2)) {
		int fd1 = -1;
		if ((fd1 = file_open(file1)) == -1) {
			printf("Error in opening file \"%s\"\n", file1);
		}
		else {
			printf("File \"%s\" oppened and fd = %d\n", file1, fd1);
		}
		int fd2 = -1;
		if ((fd2 = file_open(file2)) == -1) {
			printf("Error in opening file \"%s\"\n", file2);
		}
		else {
			printf("File \"%s\" oppened and fd = %d\n", file2, fd2);
		}
		clr_scr();

		stat_t st;
		int res = file_stat(file1, &st);
		char str1[st.size + 1]; // +1 for the character \0
		if (res != 0) {
			printf("Error in retrieving file stat\n");
		} 
		else {
			int bytes = file_read(fd1, str1, st.size);
			if (bytes == -1) {
				printf("Error in reading file\n");
			}
		}
		res = file_stat(file2, &st);
		char str2[st.size + 1]; // +1 for the character \0
		if (res != 0) {
			printf("Error in retrieving file stat\n");
		} 
		else {
			int bytes = file_read(fd2, str2, st.size);
			if (bytes == -1) {
				printf("Error in reading file\n");
			}
		}

		int i = 0;
		while (i < 3) {
			printf("%s\n", str1);
			sleep(500);
			clr_scr();
			
			printf("%s\n", str2);
			sleep(500);
			clr_scr();
			i++;
		}
		printf("%s\n", str1);
		file_close(fd1);
		file_close(fd2);
	}

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

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

/**
 * @brief entry point of kernel. Mode test available
 * 
 * @param boot_info structure with informations about boot process
 */
void kernel_entry(multiboot_info_t* boot_info) {
	init_scr();

	#ifdef TEST

	test_screen();

	#else

	pic_init();	
	gdt_init();
	idt_init();
	timer_init(18);
	sti();

	printf("Screen has been initialized.\n");
	printf("PIC has been initialized.\n");	
	printf("GDT has been initialized.\n");
	printf("IDT has been initialized.\n");
	printf("Timer has been initialized.\n");
	printf("Memory upper : %d\n", boot_info->mem_upper);

	// for(int i = 0; i < 5; i++) {
	// 	int ticks = get_ticks();
	// 	sleep(1000);		
	// 	printf("%d sec, %d (ticks), ticks diff = %d\n", i, get_ticks(), get_ticks() - ticks);
	// }
	
	while (1) {
		char c = (char)(getc());
		if ((int)(c) >= 0) {
			if (c == 'Q') {
				printf("\nShutdown");
				halt();
			}
			else {
				printf("%c", c);
			}
		}
	}

	#endif
}

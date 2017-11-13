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

	for(int i = 0; i < 20; i++) {
		int ticks = get_ticks();
		sleep(1000);		
		printf("%d sec, %d (ticks), ticks diff = %d\n", i, get_ticks(), get_ticks() - ticks);
	}

	// printf("Screen has been initialized.\n");
	// printf("GDT has been initialized.\n");
	// printf("Memory upper : %d", boot_info->mem_upper);

	#endif

	while (1) {}
}

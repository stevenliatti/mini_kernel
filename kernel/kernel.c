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

	gdt_init();
	printf("Screen has been initialized.\n");
	printf("GDT has been initialized.\n");
	printf("Memory upper : %d", boot_info->mem_upper);
	
	#endif

	while (1) {}
}

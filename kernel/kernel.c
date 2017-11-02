
#include "multiboot.h"
#include "gdt.h"
#include "screen.h"

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

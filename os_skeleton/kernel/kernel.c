
#include "multiboot.h"
#include "gdt.h"
#include "screen.h"

uint kernel_entry(multiboot_info_t* boot_info) {
	init_scr();

	#ifdef TEST

	set_theme(LIGHT_GREEN, RED);
	for (int i = 0; i < 26; i++) {
		printf("line %d : defined\n", i);
	}
	// printf("raed\n");

	#else

	gdt_init();
	printf("Screen has been initialised.\n");
	printf("GDT has been initialised.\n");
	printf("Memory upper : %d", boot_info->mem_upper);
	
	#endif

	// print_char_on_cursor('r');
	// move_cursor(10, 10);
	// print_string_on_cursor("raed");
	while (1) {
	}
}

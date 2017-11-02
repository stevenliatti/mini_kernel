
#include "multiboot.h"
#include "gdt.h"
#include "screen.h"

uint kernel_entry(multiboot_info_t* boot_info) {
	init_scr();

	#ifdef TEST

	set_theme(LIGHT_GREEN, RED);
	for (int i = 0; i < 85; i++) {
		printf("line %d : defined\n", i);
	}
	print_char_on_cursor('r');

	set_theme(BROWN, LIGHT_BLUE);
	move_cursor(40, 10);
	print_string_on_cursor("Raed");
	move_cursor(55, 10);
	print_string_on_cursor("Steven");
	move_cursor(40, 11);
	print_string_on_cursor("Abdennadher");
	move_cursor(55, 11);
	print_string_on_cursor("Liatti");

	#else

	gdt_init();
	printf("Screen has been initialized.\n");
	printf("GDT has been initialized.\n");
	printf("Memory upper : %d", boot_info->mem_upper);
	
	#endif

	while (1) {}
}

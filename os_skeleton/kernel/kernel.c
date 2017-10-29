
#include "multiboot.h"
#include "gdt.h"
#include "screen.h"

uint kernel_entry(multiboot_info_t* boot_info) {
	init_scr();
	printf("Screen has been initialised.\\n");
	gdt_init();
	printf("GDT has been initialised.\\n");
	// set_theme(RED, LIGHT_GREEN);
	// print_char_on_cursor('r');
	// move_cursor(10, 10);
	// print_string_on_cursor("raed");
	// printf("%s%c%s %c %d = %x %g", "Raed", ' ', "Abr", ':', 10, 10);
	printf("Memory : %d", boot_info->mem_upper);
	while (1) {
	}
}

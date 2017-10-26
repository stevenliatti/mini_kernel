
#include "multiboot.h"
#include "gdt.h"
#include "screen.h"

uint kernel_entry(multiboot_info_t* boot_info) {
	gdt_init();
	init_scr();
	set_theme(RED, LIGHT_GREEN);
	// print_char_on_cursor('r');
	// move_cursor(10, 10);
	// print_string_on_cursor("raed");
	printf("%s%c%s %c %d = %x %g", "Raed", ' ', "Abr", ':', 10, 10);
	while (1) {
	}
}


#include "multiboot.h"
#include "gdt.h"
#include "screen.h"

uint kernel_entry(multiboot_info_t* boot_info) {
	gdt_init();
	init_scr();
	write("STEVEN");
	set_theme(RED, LIGHT_GREEN);
	set_theme(get_fg_color(), get_bg_color());
	// set_theme(LIGHT_GREEN, RED);
	while (1) {
	}
}

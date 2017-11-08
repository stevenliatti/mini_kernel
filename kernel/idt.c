#include "base.h"
#include "idt.h"
#include "screen.h"
#include "pic.h"
#include "x86.h"
#include "keyboard.h"
#include "timer.h"

#define IDT_SIZE 256

// Declaration of IDT
static idt_entry_t idt[IDT_SIZE];

// Pointer on the IDT
static idt_ptr_t idt_ptr;

// CPU context used when saving/restoring context from an interrupt
typedef struct regs_st {
	uint32_t gs, fs, es, ds;
	uint32_t ebp, edi, esi;
	uint32_t edx, ecx, ebx, eax;
	uint32_t number, error_code;
	uint32_t eip, cs, eflags, esp, ss;
} regs_t;

// Build and return an IDT entry.
// selector is the code segment selector to access the ISR
// offset is the address of the ISR (for task gates, offset must be 0)
// type indicates the IDT entry type
// dpl is the privilege level required to call the associated ISR
static idt_entry_t idt_build_entry(uint16_t selector, uint32_t offset, uint8_t type, uint8_t dpl) {
	idt_entry_t entry;
	entry.offset15_0 = offset & 0xffff;
	entry.selector = selector;
	entry.reserved = 0;
	entry.type = type;
	entry.dpl = dpl;
	entry.p = 1;
	entry.offset31_16 = (offset >> 16) & 0xffff;
	return entry;
}

// Exception handler
void exception_handler(regs_t *regs) {
	clr_scr();
	set_theme(BLACK, RED);
	switch (regs->number) {
		case 0:
			printf("Exception 0 - Divide Error");
			break;
		case 1:
			printf("Exception 1 - RESERVED");
			break;
		case 2:
			printf("Exception 2 - NMI Interrupt");
			break;
		case 3:
			printf("Exception 3 - Breakpoint");
			break;
		case 4:
			printf("Exception 4 - Overflow");
			break;
		case 5:
			printf("Exception 5 - BOUND Range Exceeded");
			break;
		case 6:
			printf("Exception 6 - Invalid Opcode (Undefined Opcode)");
			break;
		case 7:
			printf("Exception 7 - Device Not Available (No Math Coprocessor)");
			break;
		case 8:
			printf("Exception 8 - Double Fault");
			break;
		case 9:
			printf("Exception 9 - Coprocessor Segment Overrun (reserved)");
			break;
		case 10:
			printf("Exception 10 - Invalid TSS");
			break;
		case 11:
			printf("Exception 11 - Segment Not Present");
			break;
		case 12:
			printf("Exception 12 - Stack-Segment Fault");
			break;
		case 13:
			printf("Exception 13 - General Protection");
			break;
		case 14:
			printf("Exception 14 - Page Fault");
			break;
		case 15:
			printf("Exception 15 - (Inter reserved. Do not use.)");
			break;
		case 16:
			printf("Exception 16 - x87 FPU Floating-Point Error (Math Fault)");
			break;
		case 17:
			printf("Exception 17 - Alignment Check");
			break;
		case 18:
			printf("Exception 18 - Machine Check");
			break;
		case 19:
			printf("Exception 19 - SIMD Floating-Point Exception");
			break;
		case 20:
			printf("Exception 20 - Virtualization Exception");
			break;
		default:
			break;
	}
	halt();
}

// IRQ handler
void irq_handler(regs_t *regs) {
	switch (regs->number) {
		case 0:
			// printf("System timer (PIT)");
			timer_handler();
			break;
		case 1:
			// printf("Keyboard");
			keyboard_handler();
			break;
		case 2:
			printf("Redirected to slave PIC");
			break;
		case 3:
			printf("Serial port (COM2/COM4)");
			break;
		case 4:
			printf("Serial port (COM1/COM3)");
			break;
		case 5:
			printf("Sound card");
			break;
		case 6:
			printf("Floppy disk controller");
			break;
		case 7:
			printf("Parallel port");
			break;
		case 8:
			printf("Real time clock");
			break;
		case 9:
			printf("Redirected to IRQ2");
			break;
		case 10:
			printf("Reserved");
			break;
		case 11:
			printf("Reserved");
			break;
		case 12:
			printf("PS/2 mouse");
			break;
		case 13:
			printf("Math coprocessor");
			break;
		case 14:
			printf("Hard disk controller");
			break;
		case 15:
			printf("Reserved");
			break;
		default:
			break;
	}
	pic_eoi(regs->number);
}

void idt_init() {
	for (int i = 0; i < IDT_SIZE; i++) {
		idt[i] = idt_build_entry(0, 0, 0, 0);
	}

	// CPU exceptions
	idt[0] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_0, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[1] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_1, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[2] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_2, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[3] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_3, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[4] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_4, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[5] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_5, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[6] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_6, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[7] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_7, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[8] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_8, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[9] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_9, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[10] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_10, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[11] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_11, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[12] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_12, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[13] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_13, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[14] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_14, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[15] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_15, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[16] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_16, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[17] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_17, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[18] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_18, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[19] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_19, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[20] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_exception_20, TYPE_INTERRUPT_GATE, DPL_KERNEL);

	// IRQ
	idt[32] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_0, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[33] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_1, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[34] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_2, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[35] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_3, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[36] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_4, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[37] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_5, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[38] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_6, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[39] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_7, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[40] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_8, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[41] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_9, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[42] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_10, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[43] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_11, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[44] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_12, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[45] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_13, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[46] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_14, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	idt[47] = idt_build_entry(GDT_KERNEL_CODE_SELECTOR, (uint32_t) &_irq_15, TYPE_INTERRUPT_GATE, DPL_KERNEL);
	
	idt_ptr.base = (uint32_t) idt;	 
	idt_ptr.limit = IDT_SIZE - 1;

	idt_load(&idt_ptr);
}

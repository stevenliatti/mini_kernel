#ifndef _IDT_H_
#define _IDT_H_

#include "../common/types.h"

// Structure of an IDT descriptor. There are 3 types of descriptors:
// a task-gate, an interrupt-gate, and a trap-gate.
// See 5.11 of Intel 64 & IA32 architectures software developer's manual for more details.
// For task gates, offset must be 0.
typedef struct idt_entry_st {
	uint16_t offset15_0;   // only used by trap and interrupt gates
	uint16_t selector;     // segment selector for trap and interrupt gates; TSS segment selector for task gates
	uint16_t reserved : 8;
	uint16_t type : 5;
	uint16_t dpl : 2;
	uint16_t p : 1;

	uint16_t offset31_16;  // only used by trap and interrupt gates
} __attribute__((packed)) idt_entry_t;

// Structure describing a pointer to the IDT gate table.
// This format is required by the lgdt instruction.
typedef struct idt_ptr_st {
	uint16_t limit;   // Limit of the table (ie. its size)
	uint32_t base;    // Address of the first entry
} __attribute__((packed)) idt_ptr_t;

extern void idt_init();
extern void idt_load();

// Exception handler
extern void _exception_0();
extern void _exception_1();
extern void _exception_2();
extern void _exception_3();
extern void _exception_4();
extern void _exception_5();
extern void _exception_6();
extern void _exception_7();
extern void _exception_8();
extern void _exception_9();
extern void _exception_10();
extern void _exception_11();
extern void _exception_12();
extern void _exception_13();
extern void _exception_14();
extern void _exception_15();
extern void _exception_16();
extern void _exception_17();
extern void _exception_18();
extern void _exception_19();
extern void _exception_20();

// Interruption handler
extern void _irq_0();
extern void _irq_1();
extern void _irq_2();
extern void _irq_3();
extern void _irq_4();
extern void _irq_5();
extern void _irq_6();
extern void _irq_7();
extern void _irq_8();
extern void _irq_9();
extern void _irq_10();
extern void _irq_11();
extern void _irq_12();
extern void _irq_13();
extern void _irq_14();
extern void _irq_15();

#endif

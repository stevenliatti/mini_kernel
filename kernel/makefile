KERNEL_NAME=kernel
SRCASM=$(wildcard *.s)
SRCC=$(wildcard *.c)
OBJC=$(SRCC:%.c=%.o)
OBJASM=$(SRCASM:%.s=%.o)
DEPS=$(OBJC:%.o=%.d)
ARGS=-Wall -Wextra -g -static -std=gnu11 -m32 -ffreestanding -nostdlib -lgcc -MMD
COMMON=../common

$(KERNEL_NAME).elf: $(OBJC) $(OBJASM) $(COMMON)/common_kernel.o
	gcc -g -T $(KERNEL_NAME).ld $(ARGS) $^ -o $@

test_screen:
	make TEST_MODE=-DTEST_SCREEN

test_timer:
	make TEST_MODE=-DTEST_TIMER

$(COMMON)/common_kernel.o:
	$(MAKE) common_kernel.o -C $(COMMON)

%.o: %.c
	gcc -g $(TEST_MODE) $(ARGS) -c $< -o $@

%_asm.o: %_asm.s
	nasm -g -f elf32 $^

clean:
	rm -rf $(KERNEL_NAME).elf *.o *.d

rebuild: clean $(KERNEL_NAME).elf

.PHONY: clean rebuild

-include $(DEPS)

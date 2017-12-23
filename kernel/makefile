KERNEL_NAME=kernel
SRCASM=$(wildcard *.s)
SRCC=$(wildcard *.c)
OBJC=$(SRCC:%.c=%.o)
OBJASM=$(SRCASM:%.s=%.o)
DEPS=$(OBJC:%.o=%.d)
ARGS=-Wall -Wextra -g -static -std=gnu11 -m32 -ffreestanding -nostdlib -lgcc -MMD
COMMON=../common
COMMON_KERNEL=$(COMMON)/common_kernel.o

$(KERNEL_NAME).elf: $(OBJC) $(OBJASM) $(COMMON_KERNEL)
	gcc -g -T $(KERNEL_NAME).ld $(ARGS) $^ -o $@

test_screen:
	make TEST_MODE=-DTEST_SCREEN

test_timer:
	make TEST_MODE=-DTEST_TIMER

test_fs:
	make TEST_MODE=-DTEST_FS

$(COMMON_KERNEL):
	$(MAKE) common_kernel.o -C $(COMMON)

%.o: %.c
	gcc -g $(TEST_MODE) $(ARGS) -c $< -o $@

%_asm.o: %_asm.s
	nasm -g -f elf32 $^

clean:
	rm -rf $(KERNEL_NAME).elf *.o *.d

rebuild: clean $(KERNEL_NAME).elf

help:
	@echo "Available targets :"
	@echo "\thelp : this help"
	@echo "\t$(KERNEL_NAME).elf (default) : compile $(KERNEL_NAME).elf"
	@echo "\t$(COMMON_KERNEL) : compile the common.o file with the screen.h"
	@echo "\ttest_screen : compile kernel to test screen"
	@echo "\ttest_timer : compile kernel to test timer"
	@echo "\ttest_fs : compile kernel to test file system"
	@echo "\tclean : remove $(KERNEL_NAME).elf, *.o and *.d files"
	@echo "\trebuild : call clean and $(KERNEL_NAME).elf"

.PHONY: clean help rebuild

-include $(DEPS)

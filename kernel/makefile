KERNEL_NAME=kernel
SRCASM=$(wildcard *.s)
SRCC=$(wildcard *.c)
OBJC=$(SRCC:%.c=%.o)
OBJASM=$(SRCASM:%.s=%.o)
DEPS=$(OBJC:%.o=%.d)
ARGS=-Wall -Wextra -g -static -std=gnu11 -m32 -ffreestanding -nostdlib -lgcc -MMD

$(KERNEL_NAME).elf: $(OBJC) $(OBJASM)
	gcc -T $(KERNEL_NAME).ld $(ARGS) $^ -o $@

test_kernel:
	make TEST_MODE=-DTEST

%.o: %.c
	gcc $(TEST_MODE) $(ARGS) -c $< -o $@

%_asm.o: %_asm.s
	nasm -f elf32 $^

clean:
	rm -rf $(KERNEL_NAME).elf *.o *.d

rebuild: clean $(KERNEL_NAME).elf

.PHONY: clean rebuild

-include $(DEPS)

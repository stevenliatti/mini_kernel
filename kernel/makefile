SRCASM=$(wildcard *.s)
SRCC=$(wildcard *.c)
OBJC=$(SRCC:%.c=%.o)
OBJASM=$(SRCASM:%.s=%.o)
ARGS=-Wall -Wextra -g -static -std=gnu11 -m32 -ffreestanding -nostdlib -lgcc

kernel.elf: $(OBJC) $(OBJASM)
	gcc -T kernel.ld $(ARGS) $^ -o $@

test_kernel:
	make TEST_MODE=-DTEST

%.o: %.c
	gcc $(TEST_MODE) $(ARGS) -c $^

%_asm.o: %_asm.s
	nasm -f elf32 $^

clean:
	rm -rf kernel.elf *.o

rebuild: clean run

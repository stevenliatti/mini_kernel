SRCS=$(wildcard *.c)
OBJC=$(SRCS:%.c=%.o)
ARGSLIBC=-Wall -Wextra -g -std=gnu11 -m32
ARGSKERNEL=-Wall -Wextra -g -static -m32 -ffreestanding -nostdlib -lgcc -MMD

.SECONDARY:

all: $(OBJC)

common_libc: common.c
	gcc -DLIBC $(ARGSLIBC) -c $< -o common.o

common_kernel: common.c
	gcc -DKERNEL $(ARGSKERNEL) -c $< -o common.o

clean:
	rm -rf *.o *.d

rebuild: clean all

.PHONY: clean rebuild

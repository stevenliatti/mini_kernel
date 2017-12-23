SRCS=$(wildcard *.c)
OBJC=$(SRCS:%.c=%.o)
DEPS=common_libc.d common_kernel.d
ARGSLIBC=-Wall -Wextra -g -std=gnu11 -m32 -MMD
ARGSKERNEL=-Wall -Wextra -g -static -m32 -ffreestanding -nostdlib -lgcc -MMD

.SECONDARY:

all: common_libc.o common_kernel.o

common_libc.o: common.c
	gcc -DLIBC $(ARGSLIBC) -c $< -o $@

common_kernel.o: common.c
	gcc -DKERNEL $(ARGSKERNEL) -c $< -o $@

clean:
	rm -rf *.o *.d

rebuild: clean all

help:
	@echo "Available targets :"
	@echo "\thelp : this help"
	@echo "\tall (default) : compile common_libc.o and common_kernel.o"
	@echo "\tcommon_libc.o  : compile common.c with stdio"
	@echo "\tcommon_kernel.o : compile common.c with screen.h"
	@echo "\tclean : remove *.o and *.d files"
	@echo "\trebuild : call clean and all"

.PHONY: clean help rebuild

-include $(DEPS)

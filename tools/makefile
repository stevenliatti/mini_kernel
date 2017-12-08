SRCS=$(wildcard *.c)

OUT=$(SRCS:%.c=%)
ARGS=-Wall -Wextra -g -std=gnu11 -m32 -lgcc

all: $(OUT)

%: %.c
	gcc $(ARGS) $< -o $@

clean: clean_img
	rm -rf $(OUT) *.o *.d

clean_img: 
	rm -rf *.img

test_fs_create: all clean_img
	./fs_create doge_fs 512 fs.img 5120

test_fs_add: all
	./fs_add hello.txt fs.img

rebuild: clean all

.PHONY: clean rebuild
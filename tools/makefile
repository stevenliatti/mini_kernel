SRCS=$(wildcard fs_*.c)
OBJC=$(SRCS:%.c=%.o)
OUT=$(OBJC:%.o=%)
ARGS=-Wall -Wextra -g -std=gnu11 -m32

.SECONDARY:

all: $(OUT)

fs_%: fs_%.o tools.o
	gcc $(ARGS) $^ -o $@

%.o: %.c
	gcc $(ARGS) -c $<

clean:
	rm -rf $(OUT) *.o *.d *.img

clean_img:
	rm -rf *.img

test_fs_create: fs_create clean_img
	./fs_create doge_fs 512 fs.img 307200

test_fs_add: fs_add
	./fs_add txt_files/hello.txt fs.img

test_fs_list: fs_list
	./fs_list fs.img

test_fs_del: fs_del
	./fs_del hello.txt fs.img

rebuild: clean all

.PHONY: clean rebuild

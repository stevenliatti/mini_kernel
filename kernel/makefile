SRCASM=$(wildcard *.s)
SRCC=$(wildcard *.c)
OBJC=$(SRCC:%.c=%.o)
OBJASM=$(SRCASM:%.s=%.o)
ARGS=-Wall -Wextra -g -static -std=gnu11 -m32 -ffreestanding -nostdlib 

run: myos.iso
	qemu-system-i386 -monitor stdio -cdrom myos.iso

test:
	make TEST_MODE=-DTEST

myos.iso: kernel.elf ../grub/menu.lst
	mkdir -p myos/boot/grub
	cp ../grub/menu.lst ../grub/stage2_eltorito myos/boot/grub/
	cp kernel.elf myos/boot
	genisoimage -R -b boot/grub/stage2_eltorito -input-charset utf8 -no-emul-boot -boot-info-table -o myos.iso myos

kernel.elf: $(OBJC) $(OBJASM)
	gcc -T kernel.ld $(ARGS) $^ -o $@

%.o: %.c
	gcc $(TEST_MODE) $(ARGS) -c $^

%_asm.o: %_asm.s
	nasm -f elf32 $^

clean:
	rm -rf kernel.elf *.o myos/ myos.iso

rebuild: clean run

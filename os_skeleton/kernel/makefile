CC=gcc
NASM=nasm
OPTIONS=-Wall -Wextra -std=gnu11 -m32
SRCS=$(wildcard *.c)
ASSEM=$(wildcard *.s)
OBJSC=$(SRCS:.c=.o)
OBJSA=$(ASSEM:.s=.o)
KERNEL=kernel.elf
LINKER=kernel.ld

all : iso

iso : $(KERNEL)
	mkdir -p myos/boot/grub
	cp ../grub/menu.lst ../grub/stage2_eltorito myos/boot/grub
	cp kernel.elf myos/boot
	genisoimage -R -b boot/grub/stage2_eltorito -input-charset utf8 -no-emul-boot -boot-info-table -o myos.iso myos

$(KERNEL) : $(OBJSA) $(OBJSC)
	$(CC) $(OBJSA) $(OBJSC) -T $(LINKER) -m32 -ffreestanding -nostdlib -o $@ -static

$(OBJSA) : $(ASSEM)
	$(NASM) -f elf32 $(ASSEM)

$(OBJSC) : $(SRCS)
	$(CC) $(OPTIONS) -c $(SRCS)

run :
	qemu-system-i386 -cdrom $(ISO)

clean:
	rm -rf $(OBJSA) $(OBJSC) $(KERNEL) $(ISO)
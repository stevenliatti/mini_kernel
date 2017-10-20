run: myos.iso
	qemu-system-i386 -cdrom myos.iso

myos.iso: kernel.elf ../grub/menu.lst
	mkdir -p myos/boot/grub
	cp ../grub/menu.lst ../grub/stage2_eltorito myos/boot/grub/
	cp kernel.elf myos/boot
	genisoimage -R -b boot/grub/stage2_eltorito -input-charset utf8 -no-emul-boot -boot-info-table -o myos.iso myos

kernel.elf: base.o gdt.o kernel.o bootstrap_asm.o gdt_asm.o
	gcc -T kernel.ld -static -m32 -ffreestanding -nostdlib $^ -o $@

gdt.o: gdt.c
	gcc -Wall -Wextra -g -std=gnu11 -m32 -ffreestanding -nostdlib -c $^

base.o: base.c
	gcc -Wall -Wextra -g -std=gnu11 -m32 -ffreestanding -nostdlib -c $^

kernel.o: kernel.c
	gcc -Wall -Wextra -g -std=gnu11 -m32 -ffreestanding -nostdlib -c $^

bootstrap_asm.o: bootstrap_asm.s
	nasm -f elf32 $^

gdt_asm.o: gdt_asm.s
	nasm -f elf32 $^

clean:
	rm -rf kernel.elf *.o myos/ myos.iso

rebuild: clean run

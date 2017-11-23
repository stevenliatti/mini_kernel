KERNEL_NAME=kernel
KERNEL_FOLDER=$(KERNEL_NAME)
OS_NAME=dogeos
KERNEL_ELF=$(OS_NAME)/boot/$(KERNEL_NAME).elf

$(OS_NAME).iso: $(KERNEL_ELF) grub/*
	genisoimage -R -b boot/grub/stage2_eltorito -input-charset utf8 -no-emul-boot -boot-info-table -o $(OS_NAME).iso $(OS_NAME)

run: $(OS_NAME).iso
	qemu-system-i386 -monitor stdio -cdrom $(OS_NAME).iso

$(KERNEL_ELF):
	$(MAKE) $(KERNEL_NAME).elf -C $(KERNEL_FOLDER)
	make cp_kernel

test_screen:
	$(MAKE) $@ -C $(KERNEL_FOLDER)
	make cp_kernel

test_timer:
	$(MAKE) $@ -C $(KERNEL_FOLDER)
	make cp_kernel

cp_kernel:
	mkdir -p $(OS_NAME)/boot/grub
	cp grub/menu.lst grub/stage2_eltorito $(OS_NAME)/boot/grub/
	cp $(KERNEL_FOLDER)/$(KERNEL_NAME).elf $(OS_NAME)/boot

clean:
	rm -rf $(OS_NAME)/ $(OS_NAME).iso
	$(MAKE) clean -C $(KERNEL_FOLDER)

help:
	@echo "Available targets :"
	@echo "\thelp : this help"
	@echo "\t$(KERNEL_ELF) : compile the kernel"
	@echo "\ttest_screen : compile kernel to test screen"
	@echo "\t$(OS_NAME).iso (default) : create iso file of OS"
	@echo "\trun : lauch the kernel"
	@echo "\tclean : remove $(OS_NAME).iso and call clean targets of subfolders"

.PHONY: clean help

KERNEL_NAME=kernel
KERNEL_FOLDER=$(KERNEL_NAME)
OS_NAME=myos

$(OS_NAME).iso: $(KERNEL_NAME).elf grub/menu.lst
	mkdir -p $(OS_NAME)/boot/grub
	cp grub/menu.lst grub/stage2_eltorito $(OS_NAME)/boot/grub/
	cp $(KERNEL_FOLDER)/$(KERNEL_NAME).elf $(OS_NAME)/boot
	genisoimage -R -b boot/grub/stage2_eltorito -input-charset utf8 -no-emul-boot -boot-info-table -o $(OS_NAME).iso $(OS_NAME)

run: $(OS_NAME).iso
	qemu-system-i386 -monitor stdio -cdrom $(OS_NAME).iso

$(KERNEL_NAME).elf:
	$(MAKE) $@ -C $(KERNEL_FOLDER)

test_kernel:
	$(MAKE) $@ -C $(KERNEL_FOLDER)

clean:
	rm -rf $(OS_NAME)/ $(OS_NAME).iso
	$(MAKE) clean -C $(KERNEL_FOLDER)

help:
	@echo "Available targets :"
	@echo "\thelp : this help"
	@echo "\t$(KERNEL_NAME).elf : compile the kernel"
	@echo "\ttest_kernel : compile kernel in test mode"
	@echo "\t$(OS_NAME).iso (default) : create iso file of OS"
	@echo "\trun : lauch the kernel"
	@echo "\tclean : remove $(OS_NAME).iso and call clean targets of subfolders"

.PHONY: clean help

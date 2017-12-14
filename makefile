KERNEL_NAME=kernel
KERNEL_FOLDER=$(KERNEL_NAME)
OS_NAME=dogeos
KERNEL_BUILD=$(KERNEL_FOLDER)/$(KERNEL_NAME).elf
KERNEL_BOOT=$(OS_NAME)/boot/$(KERNEL_NAME).elf
TOOLS_FOLDER=tools
COMMON_FOLDER=common
FS_NAME=$(TOOLS_FOLDER)/fs

default: kernel_rule $(OS_NAME).iso

run: kernel_rule fs_rule $(OS_NAME).iso
	qemu-system-i386 -monitor stdio -hda $(FS_NAME).img -cdrom $(OS_NAME).iso

debug: kernel_rule fs_rule $(OS_NAME).iso
	qemu-system-i386 -monitor stdio -hda $(FS_NAME).img -cdrom $(OS_NAME).iso -s -S

fs_rule: common_libc
	$(MAKE) test_fs_create_medium test_fs_add_files -C $(TOOLS_FOLDER)
	$(MAKE) clean -C $(COMMON_FOLDER)

$(OS_NAME).iso: $(KERNEL_BOOT)
	genisoimage -R -b boot/grub/stage2_eltorito -input-charset utf8 -no-emul-boot -boot-info-table -o $(OS_NAME).iso $(OS_NAME)

$(KERNEL_BOOT): $(KERNEL_BUILD) grub/*
	mkdir -p $(OS_NAME)/boot/grub
	cp grub/menu.lst grub/stage2_eltorito $(OS_NAME)/boot/grub/
	cp $(KERNEL_BUILD) $(OS_NAME)/boot

kernel_rule: common_kernel
	$(MAKE) $(KERNEL_NAME).elf -C $(KERNEL_FOLDER)
	$(MAKE) clean -C $(COMMON_FOLDER)

common_libc:
	$(MAKE) common_libc -C $(COMMON_FOLDER)

common_kernel:
	$(MAKE) common_kernel -C $(COMMON_FOLDER)

test_screen:
	$(MAKE) $@ -C $(KERNEL_FOLDER)

test_timer:
	$(MAKE) $@ -C $(KERNEL_FOLDER)

clean:
	rm -rf $(OS_NAME)/ $(OS_NAME).iso
	$(MAKE) clean -C $(KERNEL_FOLDER)
	$(MAKE) clean -C $(TOOLS_FOLDER)

help:
	@echo "Available targets :"
	@echo "\thelp : this help"
	@echo "\tkernel_rule : compile the kernel"
	@echo "\ttest_screen : compile kernel to test screen"
	@echo "\ttest_timer : compile kernel to test timer"
	@echo "\t$(OS_NAME).iso (default) : create iso file of OS"
	@echo "\trun : lauch the kernel"
	@echo "\tclean : remove $(OS_NAME).iso and call clean targets of subfolders"

.PHONY: clean help

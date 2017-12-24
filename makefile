KERNEL_NAME=kernel
KERNEL_FOLDER=$(KERNEL_NAME)
OS_NAME=dogeos
KERNEL_BUILD=$(KERNEL_FOLDER)/$(KERNEL_NAME).elf
KERNEL_BOOT=$(OS_NAME)/boot/$(KERNEL_NAME).elf
TOOLS_FOLDER=tools
COMMON_FOLDER=common
FS_NAME=fs.img
FS_FULL=$(TOOLS_FOLDER)/$(FS_NAME)

default: kernel_rule $(FS_FULL) $(OS_NAME).iso

run: default
	qemu-system-i386 -monitor stdio -hda $(FS_FULL) -cdrom $(OS_NAME).iso

debug: default
	qemu-system-i386 -monitor stdio -hda $(FS_FULL) -cdrom $(OS_NAME).iso -s -S

$(FS_FULL): common_libc
	$(MAKE) $(FS_NAME) -C $(TOOLS_FOLDER)

$(OS_NAME).iso: $(KERNEL_BOOT)
	genisoimage -R -b boot/grub/stage2_eltorito -input-charset utf8 -no-emul-boot -boot-info-table -o $(OS_NAME).iso $(OS_NAME)

$(KERNEL_BOOT): $(KERNEL_BUILD) grub/*
	mkdir -p $(OS_NAME)/boot/grub
	cp grub/menu.lst grub/stage2_eltorito $(OS_NAME)/boot/grub/
	cp $(KERNEL_BUILD) $(OS_NAME)/boot

kernel_rule: common_kernel
	$(MAKE) $(KERNEL_NAME).elf -C $(KERNEL_FOLDER)

common_libc:
	$(MAKE) $@.o -C $(COMMON_FOLDER)

common_kernel:
	$(MAKE) $@.o -C $(COMMON_FOLDER)

test_screen:
	$(MAKE) $@ -C $(KERNEL_FOLDER)

test_timer:
	$(MAKE) $@ -C $(KERNEL_FOLDER)

test_fs: common_kernel common_libc
	$(MAKE) $@ -C $(KERNEL_FOLDER)
	$(MAKE) test_fs_create_medium test_fs_add_files -C $(TOOLS_FOLDER)

clean:
	rm -rf $(OS_NAME)/ $(OS_NAME).iso
	$(MAKE) clean -C $(KERNEL_FOLDER)
	$(MAKE) clean -C $(COMMON_FOLDER)
	$(MAKE) clean -C $(TOOLS_FOLDER)

rebuild: clean default

help:
	@echo "Available targets :"
	@echo "\tdefault : compile kernel, create iso file of OS and file system image"
	@echo "\thelp : this help"
	@echo "\tkernel_rule : compile the kernel"
	@echo "\t$(OS_NAME).iso : create iso file"
	@echo "\t$(FS_FULL) : create img file and add some files in"
	@echo "\ttest_screen : compile kernel to test screen"
	@echo "\ttest_timer : compile kernel to test timer"
	@echo "\ttest_fs : compile kernel to test file system"
	@echo "\trun : launch the kernel"
	@echo "\tdebug : launch the kernel in debug mode"
	@echo "\tclean : remove $(OS_NAME).iso and folder and call clean targets of subfolders"

.PHONY: clean help rebuild

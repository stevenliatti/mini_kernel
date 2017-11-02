kernel:
	$(MAKE) myos.iso -C kernel

run:
	$(MAKE) run -C kernel

help:
	@echo "Targets available : \n\thelp : this help\n\tkernel : compile and run the kernel"
	@echo "\ttest_kernel : do visual tests for kernel\n\tclean : call clean targets of subfolders"

test_kernel:
	$(MAKE) test -C kernel

clean:
	$(MAKE) clean -C kernel

.PHONY: kernel clean

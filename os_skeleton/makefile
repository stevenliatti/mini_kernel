help:
	@echo "Targets available : \n\thelp : this help\n\tkernel : compile and run kernel"
	@echo "\ttest_kernel : do visual tests for kernel\n\tclean : call clean targets of subfolders"

kernel:
	$(MAKE) -C $@

test_kernel:
	$(MAKE) clean -C kernel
	$(MAKE) test -C kernel

clean:
	$(MAKE) clean -C kernel

.PHONY: kernel clean
obj-m += hello-world.o


KERNEL_TREE ?= /lib/modules/$(shell uname -r)/build

all:
	make -C $(KERNEL_TREE) M=$(PWD) modules
clean:
	make -C $(KERNEL_TREE) M=$(PWD) clean

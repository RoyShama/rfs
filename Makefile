
obj-m := rfs.o
rfs-objs := inode.o super.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	rm inode.o
	rm Module.symvers
	rm modules.order
	rm rfs.ko
	rm rfs.mod
	rm rfs.mod.c
	rm rfs.o
	rm super.o
	rm rfs.mod.o

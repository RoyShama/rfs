ccflags-y := -o3 $(CCFLAGS)
obj-m := rfs.o
rfs-objs := inode.o super.o

all:
	$(MAKE) -C '$(LINUX_DIR)' M='$(M)'

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

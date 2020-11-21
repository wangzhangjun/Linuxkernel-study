WRAPFS_VERSION="0.1"
EXTRA_CFLAGS += -DWRAPFS_VERSION=\"$(WRAPFS_VERSION)\"
KERNELDIR=/lib/modules/$(shell uname -r)/build 

PWD:=$(shell pwd)

obj-m := wrapfs.o
wrapfs-objs := dentry.o file.o inode.o main.o super.o lookup.o mmap.o

default:
	make -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -rf *.o .*.o.cmd  .cmd *.ko *.mod.c .tmp_versions *.order  *.sym*
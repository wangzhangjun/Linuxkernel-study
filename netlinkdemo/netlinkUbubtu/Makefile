obj-m := mynlkern.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	gcc mynluser.c -o mynluser 
clean:
	rm -rf *.ko *.o *.order *.symvers *.unsigned *.mod.c mynluser

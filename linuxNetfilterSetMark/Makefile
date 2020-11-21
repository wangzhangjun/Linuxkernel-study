TARGET=networkfilter
TARGET_BIN=$(TARGET).ko
obj-m += $(TARGET).o
$(TARGET)-objs := net_filter.o
EXTRA_CFLAGS += -std=gnu99 -Wno-declaration-after-statement -g -DDBUG
KERNELDIR=/lib/modules/$(shell uname -r)/kernel
KERNEL_SRC=/lib/modules/$(shell uname -r)/build
all:
	make -C $(KERNEL_SRC) M=$(PWD) modules
clean:
	make -C $(KERNEL_SRC) M=$(PWD) clean
	rm -rfv *.mod.c *.mod.o *.o *.ko modules.order

CC = gcc
PWD := $(shell pwd) 
KVERSION := $(shell uname -r)
KERNEL_DIR   = /usr/src/linux-headers-$(KVERSION)/
MODULE_NAME = page_table_driver
obj-m := $(MODULE_NAME).o
EXE = page_table server client

all: $(EXE)
	make -C $(KERNEL_DIR) M=$(PWD) modules

$(EXE): %: %.c
	$(CC) -o $@ $^

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
	rm -rf $(EXE)

install:
	insmod $(MODULE_NAME).ko
	mknod /dev/os c 200 0

remove:
	rmmod -f $(MODULE_NAME).ko
	rm -rf /dev/os

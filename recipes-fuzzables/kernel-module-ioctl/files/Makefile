obj-m += ioctl_string_parse.o
SRC := $(shell pwd)
EXTRA_CFLAGS += -DDEBUG
all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean

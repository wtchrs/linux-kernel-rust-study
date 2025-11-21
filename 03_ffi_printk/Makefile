KDIR ?= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD)

modules_install: all
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install

rust-analyzer:
	$(MAKE) -C $(KDIR) M=$(PWD) rust-analyzer

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

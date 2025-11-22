KDIR ?= /lib/modules/$(shell uname -r)/build

default:
	make -C $(KDIR) M=$(PWD) modules

preprocess:
	make -C $(KDIR) M=$(PWD) hello_world.i

clean:
	make -C $(KDIR) M=$(PWD) clean

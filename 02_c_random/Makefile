KDIR ?= /lib/modules/$(shell uname -r)/build

default:
	make -C $(KDIR) M=$(PWD) modules

preprocess:
	make -C $(KDIR) M=$(PWD) simple_random.i

clean:
	make -C $(KDIR) M=$(PWD) clean

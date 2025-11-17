#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wtchrs");
MODULE_DESCRIPTION("A simple Linux driver");
MODULE_VERSION("0.1");

static int __init hello_world_init(void) {
  printk(KERN_INFO "Hello, World! from Kernel Module!");
  return 0;
}

static void __exit hello_world_exit(void) {
  printk(KERN_INFO "Goodbye, World! from Kernel Module!");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

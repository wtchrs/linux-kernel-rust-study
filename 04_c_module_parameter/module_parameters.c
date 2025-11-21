#include "linux/moduleparam.h"
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wtchrs");
MODULE_DESCRIPTION("A simple Linux module parameters");
MODULE_VERSION("0.1");

/* Declare variables to use as module parameters and their default values */
static bool my_bool = true;
static int my_int = 42;
static char *my_str = "default";

// module_param(name, type, perm)
// name: parameter name to be used in kernel
// type: type of variable
// perm: sysfs permissions (0644 is rw-r--r--), same as S_IRUGO | S_IWUSR.

/* Register as kernel module parameters using `module_param` macro */
module_param(my_bool, bool, 0);
MODULE_PARM_DESC(my_bool, "A bool type module parameter");

module_param(my_int, int, 0644);
MODULE_PARM_DESC(my_int, "An int type module parameter");

module_param(my_str, charp, 0644);
MODULE_PARM_DESC(my_str, "A string type module parameter");

static int __init module_parameters_init(void) {
  pr_info("module_parameters (init)");
  pr_info("Parameters:\n");
  pr_info(" my_bool: %s\n", my_bool ? "true" : "false");
  pr_info(" my_int:  %d\n", my_int);
  pr_info(" my_int:  %s\n", my_str);
  return 0;
}

static void __exit module_parameters_exit(void) {
  pr_info("module_parameters (exit)");
}

module_init(module_parameters_init);
module_exit(module_parameters_exit);

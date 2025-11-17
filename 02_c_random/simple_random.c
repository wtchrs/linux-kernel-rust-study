#include <linux/bitmap.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kern_levels.h>
#include <linux/module.h>
#include <linux/mutex_types.h>
#include <linux/printk.h>
#include <linux/random.h>
#include <linux/types.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wtchrs");
MODULE_DESCRIPTION("A simple /dev/random-like kernel module");
MODULE_VERSION("0.1");

#define DEVICE_NAME "simple_random"
#define MAX_OUTPUT_SIZE 16

struct simple_random_dev {
  int major_num;
  dev_t dev_num;
  struct class *p_class;
  struct device *p_dev;

  atomic_t open_count;
  struct mutex read_mutex;
} dev_state;

static int device_open(struct inode *inode, struct file *file) {
  if (atomic_cmpxchg(&dev_state.open_count, 0, 1) != 0)
    return -EBUSY;
  try_module_get(THIS_MODULE);
  return 0;
}

static int device_close(struct inode *inode, struct file *file) {
  atomic_set(&dev_state.open_count, 0);
  module_put(THIS_MODULE);
  return 0;
}

static ssize_t device_read(struct file *fp, char *buffer, size_t len,
                           loff_t *offset) {
  char rand_num;
  char output[MAX_OUTPUT_SIZE];
  size_t output_len;

  // EOF
  if (*offset > 0)
    return 0;

  // Generate random number
  get_random_bytes(&rand_num, sizeof(rand_num));
  output_len = scnprintf(output, sizeof(output), "%d\n", (int)rand_num);

  // Check buffer size
  if (len < output_len)
    return -EFAULT;

  // Copy to user space
  if (copy_to_user(buffer, output, output_len) != 0)
    return -EFAULT;

  *offset += output_len;
  return output_len;
}

static struct file_operations file_ops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .open = device_open,
    .release = device_close,
};

static void cleanup(int stage) {
  switch (stage) {
  case 3:
    device_destroy(dev_state.p_class, dev_state.dev_num);
    fallthrough;
  case 2:
    class_destroy(dev_state.p_class);
    fallthrough;
  case 1:
    unregister_chrdev(dev_state.major_num, DEVICE_NAME);
    fallthrough;
  default:
    break;
  }
}

/*
 * Creating device can be performed manually by shell command
 *  `mknod /dev/simple_random c <major_num> <minor_num>`
 *  `mknod /dev/simple_random c 247 0`
 */
static int __init simple_random_init(void) {
  // Initialize device state
  memset(&dev_state, 0, sizeof(dev_state));
  atomic_set(&dev_state.open_count, 0);
  mutex_init(&dev_state.read_mutex);

  // Register character device
  dev_state.major_num = register_chrdev(0, DEVICE_NAME, &file_ops);
  if (dev_state.major_num < 0) {
    pr_alert("Could not register device: %d\n", dev_state.major_num);
    return dev_state.major_num;
  }

  dev_state.dev_num = MKDEV(dev_state.major_num, 0);

  // Create device class
  dev_state.p_class = class_create(DEVICE_NAME);
  if (IS_ERR(dev_state.p_class)) {
    pr_err("Failed to create device class");
    cleanup(1);
    return PTR_ERR(dev_state.p_class);
  }

  // Create device
  dev_state.p_dev = device_create(dev_state.p_class, NULL, dev_state.dev_num,
                                  NULL, DEVICE_NAME);
  if (IS_ERR(dev_state.p_dev)) {
    pr_err("Failed to create device");
    cleanup(2);
    return PTR_ERR(dev_state.p_dev);
  }

  pr_info("simple_random module loaded with device major number %d\n",
          dev_state.major_num);
  return 0;
}

static void __exit simple_random_exit(void) {
  cleanup(3);
  pr_info("Successfully unregistered simple_random!");
}

module_init(simple_random_init);
module_exit(simple_random_exit);

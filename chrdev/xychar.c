#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "xychardev"
#define CLASS_NAME "xychar"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xy");
MODULE_DESCRIPTION("Just a test for char dev");
MODULE_VERSION("0.1");

static int major;
static char msg[256] = {0};
static short msg_sz;
static int open_times = 0;
static struct class *xy_class = NULL;
static struct device *xy_device = NULL;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

static int __init xychar_init(void){
	printk(KERN_INFO "xychar: init xy char module\n");

	// dynamically allocate a major number
	major = register_chrdev(0, DEVICE_NAME, &fops);
	if (major < 0) {
		printk(KERN_ALERT "xychar: failed to register a major number\n");
		return major;
	}
	printk(KERN_INFO "xychar: registered correctly with major number %d\n", major);

	// register the device class
	xy_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(xy_class)) {
		unregister_chrdev(major, DEVICE_NAME);
		printk(KERN_ALERT "xychar: failed to register device class\n");
		return PTR_ERR(xy_class);
	}
	printk(KERN_INFO "xychar: device class registered correctly\n");

	// register the device driver
	xy_device = device_create(xy_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
	if (IS_ERR(xy_device)) {
		class_destroy(xy_class);
		unregister_chrdev(major, DEVICE_NAME);
		printk(KERN_ALERT "xychar: failed to create the device\n");
		return PTR_ERR(xy_device);
	}
	printk(KERN_INFO "xy_char: device class created correctly\n");
	return 0;
}

static void __exit xychar_exit(void){
	device_destroy(xy_class, MKDEV(major, 0));
	class_unregister(xy_class);
	class_destroy(xy_class);
	unregister_chrdev(major, DEVICE_NAME);
	printk(KERN_INFO "xychar: goodbye!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
	open_times++;
	printk(KERN_INFO "xychar: device has been opened %d time(s)\n", open_times);
	return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset){
	ssize_t ret = -EFAULT;

	if (len > msg_sz)
		len = msg_sz;

	if (!copy_to_user(buffer, msg, len)) {
		printk(KERN_INFO "xychar: sent %lu characters to the user\n", len);
		ret = len;
		msg_sz = 0;
	} else {
		printk(KERN_INFO "xychar: failed to send %lu characters to the user\n", len);
	}

	return ret;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset){
	int i;
	char tmp;
	ssize_t ret = -EFAULT;

	if (len > 255) {
		len = 255;
	}

        if (!copy_from_user(msg, buffer, len)) {
		printk(KERN_INFO "xychar: got %lu chars from user\n", len);
		msg[len] = '\0';
		ret = len;
	} else {
		printk(KERN_INFO "xychar: failed to get %lu chars from user\n", len);
		goto out;
	}

	msg_sz = strlen(msg);
	for (i = 0; i < (msg_sz >> 1); ++i) {
		tmp = msg[i];
		msg[i] = msg[msg_sz - 1 - i];
		msg[msg_sz - 1 - i] = tmp;
	}

	printk(KERN_INFO "xychar: revert message: %s\n", msg);
out:
	return ret;
}

static int dev_release(struct inode *inodep, struct file *filep){
	printk(KERN_INFO "xychar: device successfully closed\n");
	return 0;
}

module_init(xychar_init);
module_exit(xychar_exit);

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
static char buf[256] = {0};
static short size_of_message;
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

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	ssize_t ret = -EFAULT;
	// copy_to_user has the format ( * to, *from, size) and returns 0 on success
	if (!copy_to_user(buffer, buf, size_of_message)) {
		printk(KERN_INFO "xychar: sent %d characters to the user\n", size_of_message);
		ret = size_of_message;
		size_of_message = 0;
	} else {
		printk(KERN_INFO "xychar: failed to send %d characters to the user\n", size_of_message);
	}
	return ret;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	int i;
	char tmp;
	if (len > 255) {
		len = 255;
	}

	memcpy(buf, buffer, len);
	buf[len] = '\0';

	size_of_message = strlen(buf);
	for (i = 0; i < (size_of_message >> 1); ++i) {
		tmp = buf[i];
		buf[i] = buf[size_of_message - 1 - i];
		buf[size_of_message - 1 - i] = tmp;
	}


	printk(KERN_INFO "xychar: revert message: %s\n", buf);
	return len;
}

static int dev_release(struct inode *inodep, struct file *filep){
	printk(KERN_INFO "xychar: device successfully closed\n");
	return 0;
}

module_init(xychar_init);
module_exit(xychar_exit);

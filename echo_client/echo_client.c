#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Wang Xinyong");
MODULE_DESCRIPTION("Echo Client");
MODULE_VERSION("0.1");

static int __init echo_client_init(void){
	printk(KERN_INFO "echo client: init\n");
	// TODO:
	return 0;
}

static void __exit echo_client_exit(void){
	// TODO:
	printk(KERN_INFO "echo client: goodbye!\n");
}

module_init(echo_client_init);
module_exit(echo_client_exit);

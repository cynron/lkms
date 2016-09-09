#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Wang Xinyong");
MODULE_DESCRIPTION("Echo Server");
MODULE_VERSION("0.1");

static int __init echo_server_init(void){
	printk(KERN_INFO "echo server: init\n");
	// TODO:
	return 0;
}

static void __exit echo_server_exit(void){
	// TODO:
	printk(KERN_INFO "echo server: goodbye!\n");
}

module_init(echo_server_init);
module_exit(echo_server_exit);

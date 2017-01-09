#include <linux/kernel.h>
#include <linux/module.h>

void cleanup_module(void){
	printk(KERN_INFO "Short is life of a kernel module\n");
}

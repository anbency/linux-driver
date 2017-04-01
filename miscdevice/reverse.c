#include <linux/init.h>		/* __init and __exit macroses */
#include <linux/kernel.h>	/* KERN_INFO macros */
#include <linux/module.h>	/* required for all kernel modules */
#include <linux/moduleparam.h>	/* module_param() and MODULE_PARM_DESC() */



MODULE_LICENSE("GPL");
MODULE_AUTHOR("anbency");
MODULE_DESCRIPTION("In-kernel phrase reverser");

static unsigned long buffer_size = 8192;
module_param(buffer_size, ulong, (S_IRUSR | S_IRGRP | S_IROTH));
MODULE_PARM_DESC(buffer_size, "Internal buffer size");


static int __init reverse_init(void)
{
	if(!buffer_size)
		return -1;
	printk(KERN_INFO "reverse device has been registeredbuffer size is %lu bytes\n",buffer_size);
	return 0;
}

static void __exit reverse_exit(void)
{
	printk(KERN_INFO "reverse device has been unregistered\n");	
}

module_init(reverse_init);
module_exit(reverse_exit);
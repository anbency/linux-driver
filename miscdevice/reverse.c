#include <linux/init.h>		/* __init and __exit macroses */
#include <linux/kernel.h>	/* KERN_INFO macros */
#include <linux/module.h>	/* required for all kernel modules */
#include <linux/moduleparam.h>	/* module_param() and MODULE_PARM_DESC() */

#include <linux/fs.h>		/* struct file_operations, struct file */
#include <linux/miscdevice.h>	/* struct miscdevice and misc_[de]register() */


MODULE_LICENSE("GPL");
MODULE_AUTHOR("anbency");
MODULE_DESCRIPTION("In-kernel phrase reverser");

static unsigned long buffer_size = 8192;
module_param(buffer_size, ulong, (S_IRUSR | S_IRGRP | S_IROTH));
MODULE_PARM_DESC(buffer_size, "Internal buffer size");

static int reverse_open(struct inode *inode, struct file *file)
{
	int err = 0;

out:
	return err;
}
static ssize_t reverse_read(struct file *file, char __user *out, size_t size, loff_t *off)
{
	ssize_t result;

out:
	return result;
}
static ssize_t reverse_write(struct file *file, const char *in, size_t size, loff_t *off)
{
	ssize_t result;

out:
	return result;
}
static int reverse_close(struct inode *inode, struct file *file)
{
	int err = 0;

out:
	return err;
}
static struct file_operations reverse_fops =
{
	.owner = THIS_MODULE,
	.open = reverse_open,
	.read = reverse_read,
	.write = reverse_write,
	.release = reverse_close,
	.llseek = noop_llseek
};

static struct miscdevice reverse_misc_device = 
{
	.minor = MISC_DYNAMIC_MINOR,
	.name = "reverse",
	.fops = &reverse_fops
};

static int __init reverse_init(void)
{
	if(!buffer_size)
		return -1;
	misc_register(&reverse_misc_device);
	printk(KERN_INFO "reverse device has been registeredbuffer size is %lu bytes\n",buffer_size);
	return 0;
}

static void __exit reverse_exit(void)
{
	misc_deregister(&reverse_misc_device);
	printk(KERN_INFO "reverse device has been unregistered\n");	
}

module_init(reverse_init);
module_exit(reverse_exit);
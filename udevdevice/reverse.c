#include <linux/init.h>		/* __init and __exit macroses */
#include <linux/kernel.h>	/* KERN_INFO macros */
#include <linux/module.h>	/* required for all kernel modules */
#include <linux/moduleparam.h>	/* module_param() and MODULE_PARM_DESC() */

#include <linux/fs.h>		/* struct file_operations, struct file */
#include <linux/miscdevice.h>	/* struct miscdevice and misc_[de]register() */
#include <linux/device.h> /* devfs head file*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("anbency");
MODULE_DESCRIPTION("In-kernel phrase reverser");

static unsigned long buffer_size = 8192;
module_param(buffer_size, ulong, (S_IRUSR | S_IRGRP | S_IROTH));
MODULE_PARM_DESC(buffer_size, "Internal buffer size");

#define DRY_MAJOR 0
#define DRY_MINOR 0
#define MY_DEVICE_CLASS_NAME "drvee"
#define MY_DEVICE_FILE_NAME "drvee"
#define MY_DEVICE_NODE_NAME  "drvee" 
static struct class* drvee_class = NULL;
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

static int __init reverse_init(void)
{
	int err = -1;
    struct device* temp = NULL;
    //dev_t dev = 0;
	if(!buffer_size)
		return -1;

	err = register_chrdev(DRY_MAJOR, MY_DEVICE_NODE_NAME, &reverse_fops);
	if(err < 0) {
		printk(KERN_INFO"Failed to alloc char dev region.\n");
		goto fail;
	}
	printk(KERN_INFO"register_chrdev success\n");
	
	/*在/sys/class/目录下创建设备类别目录hello*/
	drvee_class = class_create(THIS_MODULE, MY_DEVICE_CLASS_NAME);
	if(IS_ERR(drvee_class)) {
		err = PTR_ERR(drvee_class);
		printk(KERN_INFO"Failed to create drvee class.\n");
		goto destroy_cdev;
	}        
	printk(KERN_INFO"class_create success\n");
	/*在/dev/目录和/sys/class/hello目录下分别创建设备文件hello*/
	temp = device_create(drvee_class, NULL, MKDEV(DRY_MAJOR, DRY_MINOR), "%s", MY_DEVICE_FILE_NAME);
	if(IS_ERR(temp)) {
		err = PTR_ERR(temp);
		printk(KERN_INFO"Failed to create hello device.");
		goto destroy_class;
	}        
    printk(KERN_INFO " device has been registeredbuffer size is %lu bytes\n",buffer_size);
	
	return 0;

destroy_class:
	class_destroy(drvee_class);

destroy_cdev:
	unregister_chrdev(DRY_MAJOR, MY_DEVICE_NODE_NAME);

fail:
	return err;
}

static void __exit reverse_exit(void)
{
	unregister_chrdev(DRY_MAJOR, MY_DEVICE_NODE_NAME);
	if(drvee_class) {
		device_destroy(drvee_class, MKDEV(DRY_MAJOR, DRY_MINOR));
		class_destroy(drvee_class);
	}  

    printk(KERN_INFO "drvee device has been unregistered\n");	
}

module_init(reverse_init);
module_exit(reverse_exit);

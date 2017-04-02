#include <linux/init.h>		/* __init and __exit macroses */
#include <linux/kernel.h>	/* KERN_INFO macros */
#include <linux/module.h>	/* required for all kernel modules */
#include <linux/moduleparam.h>	/* module_param() and MODULE_PARM_DESC() */

#include <linux/fs.h>		/* struct file_operations, struct file */

#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("anbency");
MODULE_DESCRIPTION("In-kernel phrase reverser");

static unsigned long buffer_size = 8192;
module_param(buffer_size, ulong, (S_IRUSR | S_IRGRP | S_IROTH));
MODULE_PARM_DESC(buffer_size, "Internal buffer size");


#define DRVPP_DEVICE_PROC_NAME "drvpp"
static struct proc_dir_entry *pr_root;  
#define USER_ROOT_DIR "pr_debug"

static ssize_t drvpp_proc_read(char* page, char** start, off_t off, int count, int* eof, void* data)
{
	ssize_t result;

out:
	return result;
}
static ssize_t drvpp_proc_write(struct file* filp, const char __user *buff, unsigned long len, void* data)
{
	ssize_t result;

out:
	return result;
}

/*创建/proc/drvpp文件*/
static int drvpp_create_proc(void) {
	struct proc_dir_entry* entry;

    pr_root =proc_mkdir(USER_ROOT_DIR, NULL); 

    if (NULL==pr_root) 
    { 
            printk(KERN_ALERT"Create dir /proc/%s error!\n", USER_ROOT_DIR); 
            return -1; 
    } 	
	entry = create_proc_entry(DRVPP_DEVICE_PROC_NAME, 0666, pr_root);
	if(entry) {
		entry->read_proc = drvpp_proc_read;
		entry->write_proc = drvpp_proc_write;
	}
	else
	{
    	printk(KERN_ALERT"Create entry %s under /proc/%s error!\n", DRVPP_DEVICE_PROC_NAME,USER_ROOT_DIR); 

		goto err_out; 
	}
	return 0;

err_out: 
     entry->read_proc =NULL; 
     entry->write_proc= NULL;
     remove_proc_entry(USER_ROOT_DIR,pr_root); 
     return -1; 
}

/*删除/proc/drvpp文件*/
static void drvpp_remove_proc(void) 
{
	remove_proc_entry(DRVPP_DEVICE_PROC_NAME, pr_root);
	remove_proc_entry(USER_ROOT_DIR, NULL);
}

static int __init drvpp_init(void)
{
	int err = -1;
    //dev_t dev = 0;
	if(!buffer_size)
		return -1;

	/*创建/proc/drvpp文件*/
	drvpp_create_proc();

    printk(KERN_INFO " device has been registeredbuffer size is %lu bytes\n",buffer_size);
	
	return 0;

fail:
	return err;
}

static void __exit drvpp_exit(void)
{
	/*删除/proc/drvpp文件*/
	drvpp_remove_proc(); 

    printk(KERN_INFO "drvee device has been unregistered\n");	
}

module_init(drvpp_init);
module_exit(drvpp_exit);

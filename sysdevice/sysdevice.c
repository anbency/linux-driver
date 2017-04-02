#include <linux/init.h>  
#include <linux/kernel.h>  
#include <linux/module.h>  
#include <linux/cdev.h>  
#include <linux/fs.h>  
#include <asm/uaccess.h>  
#include <asm/io.h>  
#include <linux/device.h>  
#include <linux/platform_device.h>  
#include <linux/sysfs.h>  
  
MODULE_LICENSE("GPL");  
MODULE_AUTHOR("anbency");  
MODULE_DESCRIPTION("create sys file for device");    
static struct kobject *sys_kobj;  
static char kbuf[1024] = {0};  
/* 读sys文件，例如cat sys_test */  
static ssize_t sys_test_show(struct kobject *kobj, struct kobj_attribute *attr,  
        char *buf)  
{  
    char info[]="my_show is called\n";
    printk("user_buf:%s\n",buf);
    return scnprintf(buf,sizeof(info),info);
}  
  
/* 写入sys文件,例如echo xxx sys_test*/  
static ssize_t sys_test_store(struct kobject *kobj, struct kobj_attribute  
        *attr, const char *buf, size_t count)  
{  
    printk("%s is called\n",__func__);
    strncpy(kbuf,buf,count);
    printk("user_buf:%s,count:%u|after copy,kbuf:%s\n",buf,count,kbuf);
    return count;
}  
  
/* 定义创建sys文件的属性 */   
static struct kobj_attribute my_sysfs_read =__ATTR(show, 0775, sys_test_show, NULL);
static struct kobj_attribute my_sysfs_write =__ATTR(write, 0775, NULL,sys_test_store);

static struct attribute *sys_test_attr[] = {  
    &my_sysfs_read.attr,
    &my_sysfs_write.attr,  
    NULL,  
};  
  
/* 定义一个group */  
static struct attribute_group sys_file_test_group = {  
     .name = "sys_file_test",  
     .attrs = sys_test_attr,  
};  
  
  
/* /fs/sysfs/group.c */  
static int __init create_sys_file_init(void)  
{  
    int ret;  
    sys_kobj = kobject_create_and_add("sys_test_group", NULL);  
    ret = sysfs_create_group(sys_kobj, &sys_file_test_group);  
    if(ret) {  
        printk(KERN_ERR "sysfs_create_group error\n");  
        return -1;  
    }  
    printk(KERN_ERR "sysfs_create_group ok\n");   
  
    return 0;  
}  
  
static void  __exit create_sys_file_exit(void)  
{  
    sysfs_remove_group(sys_kobj, &sys_file_test_group);  
    printk( KERN_ERR "sysfs_remove_group\n");  
}  
  
module_init(create_sys_file_init);  
module_exit(create_sys_file_exit);  

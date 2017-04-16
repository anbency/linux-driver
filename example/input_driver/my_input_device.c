#include <linux/device.h>  
#include <linux/string.h>  
#include <linux/platform_device.h>  
#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/fs.h>  
#include <linux/init.h>  
#include <linux/delay.h>  
#include <linux/poll.h>  
#include <linux/irq.h>  
#include <asm/irq.h>  
#include <linux/interrupt.h>  
#include <asm/uaccess.h>  
//#include <mach/regs-gpio.h>  
#include <mach/hardware.h>  
#include <linux/cdev.h>  
#include <linux/miscdevice.h>  
#include <linux/sched.h>  
#include <linux/gpio.h>  
  
static struct resource key_resource[]=  
{     
    [0] = {  
        .start = 0x10009000,  
        .end = 0x10009000,  
        .flags = IORESOURCE_IRQ,  
    },   
};  
  
struct platform_device *my_input_dev;  
  
static int __init platform_dev_init(void)  
{  
    int ret;  
      
    my_input_dev = platform_device_alloc("my_input", -1);  
      
    platform_device_add_resources(my_input_dev,key_resource,1);//添加资源一定要用该函数，不能使用对platform_device->resource幅值  
                                                                //否则会导致platform_device_unregister调用失败，内核异常。  
      
    ret = platform_device_add(my_input_dev);  
      
    if(ret)  
        platform_device_put(my_input_dev);  
      
    return ret;  
}  
  
static void __exit platform_dev_exit(void)  
{  
    platform_device_unregister(my_input_dev);  
}  
  
module_init(platform_dev_init);  
module_exit(platform_dev_exit);  
  
MODULE_AUTHOR("anbency");  
MODULE_LICENSE("GPL");
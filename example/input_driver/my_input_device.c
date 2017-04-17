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
#include <linux/slab.h>
#include <mach/motherboard.h>

static struct resource key_resource[]=  
{     
    [0] = {
        .start = (0x0020000),
        .end = (0x0020000) + (0x0008000),
        .flags = IORESOURCE_MEM,
    },
    [1] = {  
        .start = IRQ_V2M_CLCD,  
        .end = IRQ_V2M_CLCD,  
        .flags = IORESOURCE_IRQ,  
    },
};
static struct my_input_data {
    int wires;
    int x_plate_resistance;
};  
struct platform_device *my_input_dev;
  
static int __init platform_dev_init(void)  
{  
    int ret;
    struct my_input_data *pdata;

    pdata = kzalloc(sizeof(*pdata), GFP_KERNEL);
    if (!pdata)
        return -ENOMEM;
       
    pdata->wires = 4;
    pdata->x_plate_resistance = 0;

    my_input_dev = platform_device_alloc("my_input", -1);  
      
    platform_device_add_resources(my_input_dev,key_resource,2);//添加资源一定要用该函数，不能使用对platform_device->resource幅值  

    my_input_dev->dev.platform_data = pdata;

    ret = platform_device_add(my_input_dev);  
      
    if(ret)  
        platform_device_put(my_input_dev);  
    
    kfree(pdata);

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
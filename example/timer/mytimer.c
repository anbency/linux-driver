#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>//jiffies在此头文件中定义
#include <linux/init.h>
#include <linux/timer.h>
struct timer_list mytimer;//定义一个定时器
void  mytimer_ok(unsigned long arg)
{
	printk("Mytimer is ok\n");
	printk("receive data from timer: %d\n",arg);
	mytimer.expires = jiffies+100;//设定超时时间，100代表1秒
	mytimer.data = 250;    //传递给定时器超时函数的值
	mytimer.function = mytimer_ok;//设置定时器超时函数
	mod_timer(&mytimer, jiffies + 100);
}
 
static int __init hello_init (void)
{
    printk("hello,world\n");
    init_timer(&mytimer);     //初始化定时器
    mytimer.expires = jiffies+100;//设定超时时间，100代表1秒
    mytimer.data = 250;    //传递给定时器超时函数的值
    mytimer.function = mytimer_ok;//设置定时器超时函数
    add_timer(&mytimer); //添加定时器，定时器开始生效
    return 0;
}
   
static void __exit hello_exit (void)
 
{
    del_timer(&mytimer);//卸载模块时，删除定时器
    printk("Hello module exit\n");
}
 
module_init(hello_init);
module_exit(hello_exit);
MODULE_AUTHOR("CXF");
MODULE_LICENSE("Dual BSD/GPL");
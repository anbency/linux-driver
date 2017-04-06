 /* my_bus.c   */
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>

#include "mybus.h"

MODULE_LICENSE("Dual BSD/GPL");

#define MYBUS "mybus: "
#define PRINT(x...) printk(KERN_ALERT MYBUS x);

static char *Version = "$Revision: 1.9 $";

static ssize_t show_bus_version(struct bus_type *bus, char *buf)
{
    PRINT("%s\n", __func__);
    return snprintf(buf, PAGE_SIZE, "my_bus: %s\n", Version);
}

static BUS_ATTR(version, S_IRUGO, show_bus_version, NULL);

static void my_bus_release(struct device *dev)
{
    PRINT("%s\n", __func__);
}

static int my_bus_match(struct device *dev, struct device_driver *drv)
{
    struct my_device *device = to_my_device(dev);
    PRINT("%s\n", __func__);

    return !strncmp(device->name, drv->name, strlen(drv->name));
}

static int my_bus_hotplug(struct device *dev, struct kobj_uevent_env *env){
    PRINT("%s\n", __func__);
    return 0;
}

struct bus_type my_bus_type = {
    .name    = "my_bus",      //总线名称.
    .match   = my_bus_match,  //用来匹配设备和驱动的函数.
    .uevent  = my_bus_hotplug //用来发送uevent到用户空间.
};

static struct device my_bus = {
    .init_name = "my_bus0", //my_bus设备的名称.在device_add函数里会将init_name复制到kobject中.
    .release = my_bus_release //取消注册的设备的时候要调用的函数.
};

/* interface to device. */
int register_my_device(struct my_device *device)
{
    PRINT("%s\n", __func__);
    device->device.bus = &my_bus_type;
    device->device.parent = &my_bus;
    device->device.release = my_bus_release;

    //strncpy(device->device.bus_id, device->name, BUS_ID_SIZE);

    return device_register(&device->device);
}
EXPORT_SYMBOL(register_my_device);

void unregister_my_device(struct my_device *device)
{
    PRINT("%s\n", __func__);
    device_unregister(&device->device);
}
EXPORT_SYMBOL(unregister_my_device);

static ssize_t show_version(struct device_driver *driver, char *buf)
{
    struct my_driver *drv = to_my_driver(driver);

    PRINT("%s\n", __func__);
    sprintf(buf, "%s\n", drv->version);

    return strlen(buf);
}

/* interface to driver. */
int register_my_driver(struct my_driver *driver)
{
    int ret = 0;
    
    PRINT("%s\n", __func__);

    driver->driver.bus = &my_bus_type;
    ret = driver_register(&driver->driver);
    if (ret) {
        PRINT("%s, driver_register %s failed!!!\n", __func__, driver->driver.name);
        return ret;
    }

    driver->version_attr.attr.name = "version";
    //driver->version_attr.attr.owner = driver->module;
    driver->version_attr.attr.mode = S_IRUGO;
    driver->version_attr.show = show_version;

    return driver_create_file(&driver->driver, &driver->version_attr);
}
EXPORT_SYMBOL(register_my_driver);

void unregister_my_driver(struct my_driver *driver)
{
    PRINT("%s\n", __func__);
    driver_unregister(&driver->driver);
}
EXPORT_SYMBOL(unregister_my_driver);

static int __init my_bus_init(void)
{
    int ret = 0;

    ret = bus_register(&my_bus_type); //注册my_bus_type到Linux设备驱动模型中.
    if (ret) {
        PRINT("%s, bus_register failed!\n", __func__);
        goto bus_register_failed;
    }

    ret = bus_create_file(&my_bus_type, &bus_attr_version);//创建my_bus的一个属性.在sysfs中体现.
    if (ret) {
        PRINT("%s, bus_create_file failure...!\n", __func__);
        goto bus_create_file_failed;
    }

    ret = device_register(&my_bus); //注册my_bus到Linux设备驱动模型中.
    if (ret) {
        PRINT("%s, device_register failure...!\n", __func__);
        goto device_register_failed;
    }

    PRINT("%s, bus & device register succeed!\n", __func__);
    return 0;
    
device_register_failed:
bus_create_file_failed:
    bus_unregister(&my_bus_type);
bus_register_failed:
    return ret;
}

static void __exit my_bus_exit(void)
{
    PRINT("%s!\n", __func__);
    device_unregister(&my_bus);
    bus_unregister(&my_bus_type);
}

module_init(my_bus_init);
module_exit(my_bus_exit);
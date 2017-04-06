/* my_bus.h */

extern struct bus_type my_bus_type;

/* 我的驱动 */
struct my_driver {
    char *version; //my_driver的版本.用来输出到sysfs文件系统的.
    struct module *module;//基本每个数据结构都要有的.
    struct device_driver driver;//Linux设备驱动模型中,用来管理驱动的.
    struct driver_attribute version_attr;//my_driver的一个属性.用来输出到sysfs.
};

//通过 device_driver 得到 my_driver.
#define to_my_driver(drv) \
    container_of(drv, struct my_driver, driver)

/* interface to driver. */
extern int register_my_driver(struct my_driver *); //将my_driver注册到my_bus上.
extern void unregister_my_driver(struct my_driver *);//将my_driver从my_bus上取消注册.

/* 我的设备 */
struct my_device {
    char *name; //my_device的名称. 用来在 my_driver 匹配时用.
    struct my_driver *driver; //my_device绑定的my_driver指针.
    struct device device; //Linux设备驱动模型中,用来管理设备.
};

/* 通过device得到my_device结构.*/
#define to_my_device(dev) \
    container_of(dev, struct my_device, device)

/* interface to device. */
extern int register_my_device(struct my_device *); //将my_device注册到my_bus上.
extern void unregister_my_device(struct my_device *);//将my_device从my_bus上取消注册.
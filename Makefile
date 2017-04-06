##### add this to driver/Makefile
obj-$(CONFIG_MY_MISC) += miscdevice/
obj-$(CONFIG_MY_UDEV) += udevdevice/
obj-$(CONFIG_MY_PROC_DEV) += procdevice/
obj-$(CONFIG_MY_SYS_DEV) += sysdevice/
obj-$(CONFIG_MY_BUS) += drivingmodel/

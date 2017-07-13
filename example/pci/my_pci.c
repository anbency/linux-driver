#include <linux/module.h>  
#include <linux/kernel.h>       /**< printk() */  
#include <linux/init.h>  
  
#include <linux/cdev.h>         /**< cdev_* */  
#include <linux/fs.h>  
#include <asm/uaccess.h>        /**< copy_*_user */  
  
#include <linux/types.h>        /**< size_t */  
#include <linux/errno.h>        /**< error codes */  
#include <linux/string.h>  
  
#include <linux/slab.h>  
#include <linux/pci.h>          /**< pci... */  
  
#ifndef LL_DEBUG  
#define LL_DEBUG  
#endif  
  
#ifdef LL_DEBUG  
/* KERN_INFO */  
#define ll_debug(fmt, ...) printk(KERN_INFO "[++LL debug: %d @ %s]: " fmt, __LINE__, __func__, ##__VA_ARGS__)  
#else  
#define ll_debug(fmt, ...)  
#endif  
  
// #define TEST_CHAR  
#ifdef TEST_CHAR  
extern int foodrv_probe(void);  
extern int foodrv_remove(void);  
#endif  
  
// SMBus Port  
#define PCI_DEVICE_ID_INTEL_BAYTRAIL_SMBUS  0x0F12  
// LPC: Bridge to Intel Legacy Block  
#define PCI_DEVICE_ID_INTEL_BAYTRAIL_LPC    0x0F1C  
// I211  
#define E1000_DEV_ID_I211_COPPER  0x1539  
  
// PCI标准寄存器，共64字节  
#define PCI_STD_HEADER_SIZEOF 64  
#define PCI_VENDOR_ID  0x00 /* 16 bits */  
#define PCI_DEVICE_ID  0x02 /* 16 bits */  
#define PCI_COMMAND  0x04 /* 16 bits */  
#define PCI_STATUS  0x06 /* 16 bits */  
#define PCI_CLASS_REVISION 0x08 /* High 24 bits are class, low 8 revision */  
#define PCI_CACHE_LINE_SIZE 0x0c /* 8 bits */  
#define PCI_LATENCY_TIMER 0x0d /* 8 bits */  
#define PCI_HEADER_TYPE  0x0e /* 8 bits */  
#define PCI_BIST  0x0f /* 8 bits */  
/* 
 * Base addresses specify locations in memory or I/O space. 
 * Decoded size can be determined by writing a value of 
 * 0xffffffff to the register, and reading it back.  Only 
 * 1 bits are decoded. 
 */  
#define PCI_BASE_ADDRESS_0 0x10 /* 32 bits */  
#define PCI_BASE_ADDRESS_1 0x14 /* 32 bits [htype 0,1 only] */  
#define PCI_BASE_ADDRESS_2 0x18 /* 32 bits [htype 0 only] */  
#define PCI_BASE_ADDRESS_3 0x1c /* 32 bits */  
#define PCI_BASE_ADDRESS_4 0x20 /* 32 bits */  
#define PCI_BASE_ADDRESS_5 0x24 /* 32 bits */  
#define PCI_CARDBUS_CIS  0x28  
#define PCI_SUBSYSTEM_VENDOR_ID 0x2c  
#define PCI_SUBSYSTEM_ID 0x2e  
#define PCI_ROM_ADDRESS  0x30 /* Bits 31..11 are address, 10..1 reserved */  
/* 0x35-0x3b are reserved */  
#define PCI_INTERRUPT_LINE 0x3c /* 8 bits */  
#define PCI_INTERRUPT_PIN 0x3d /* 8 bits */  
#define PCI_MIN_GNT  0x3e /* 8 bits */  
#define PCI_MAX_LAT  0x3f /* 8 bits */  
// PCI标准寄存器 结束  
///////////////////////////////////////////////////  
  
/* SMBus 基地址在bar 4 */  
#define SMBBAR        4  
  
/* PCI Address Constants */  
#define SMBCOM  0x004  
#define SMBBA  0x014  
#define SMBATPC  0x05B /* used to unlock xxxBA registers */  
#define SMBHSTCFG 0x0E0  
#define SMBSLVC  0x0E1  
#define SMBCLK  0x0E2  
#define SMBREV  0x008  
/////////////////////////////////////////////////////////////////////////  
  
// 这里是SMBUS io访问的地址  
/* SMBus address offsets */  
#define SMBHSTSTS(p)    (0 + (p)->smba)  
#define SMBHSTCNT(p)    (2 + (p)->smba)  
#define SMBHSTCMD(p)    (3 + (p)->smba)  
#define SMBHSTADD(p)    (4 + (p)->smba)  
#define SMBHSTDAT0(p)   (5 + (p)->smba)  
#define SMBHSTDAT1(p)   (6 + (p)->smba)  
#define SMBBLKDAT(p)    (7 + (p)->smba)  
#define SMBPEC(p)       (8 + (p)->smba)        /* ICH3 and later */  
#define SMBAUXSTS(p)    (12 + (p)->smba)    /* ICH4 and later */  
#define SMBAUXCTL(p)    (13 + (p)->smba)    /* ICH4 and later */  
  
#define SMBSMBC(p)      (0xF + (p)->smba)  
  
//////////////////////////////////////////////////////////////////////////  
// LPC  
/* PCI config registers in LPC bridge. LPC桥(这是一个PCI设备)上的外设偏移地址*/  
#define REVID  0x08  
#define ABASE  0x40  
#define PBASE  0x44  
#define GBASE  0x48  
#define IOBASE  0x4c  
#define IBASE  0x50  
#define SBASE  0x54 // SPI的  
#define MPBASE  0x58  
#define PUBASE  0x5c  
#define UART_CONT 0x80  
#define RCBA  0xf0  
  
//////////////////////////////////////////////////////////////////////////  
  
struct foo_priv {  
    u8 *name;  
    int type;  
    u32 smba;  
    u32 mem;  
    struct pci_dev *pci_dev;  
      
};  
  
static const struct pci_device_id foo_ids[] = {  
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_BAYTRAIL_SMBUS) },  
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_BAYTRAIL_LPC) },  
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, E1000_DEV_ID_I211_COPPER) }, // 不懂为何，这个认不出  
    { 0, }  
};  
  
/* 
会调用2次，由PCI总线根据本驱动的id来决定 
*/  
static int foo_probe(struct pci_dev *dev, const struct pci_device_id *id)  
{  
    unsigned char temp;  
    int err;  
    struct foo_priv *priv;  
  
    ll_debug("MARK Start of probe....\n");  
    priv = kzalloc(sizeof(*priv), GFP_KERNEL);  
    if (!priv)  
        return -ENOMEM;  
  
    priv->pci_dev = dev;  
    switch (dev->device) {  
    case PCI_DEVICE_ID_INTEL_BAYTRAIL_SMBUS: // 0000:00:1f.3  
        ll_debug("Got SMBUS.\n");  
        priv->name = "SMBUS";  
        priv->type = 0;  
        break;  
    case PCI_DEVICE_ID_INTEL_BAYTRAIL_LPC: // 0000:00:1f.0  
        ll_debug("Got LPC.\n");  
        priv->name = "LPC";  
        priv->type = 1;  
        break;  
    case E1000_DEV_ID_I211_COPPER: // 0000:01:00.0  
        ll_debug("Got I211.\n");  
        priv->name = "I211";  
        priv->type = 2;  
        break;  
    default:  
        priv->name = "Unknown";  
        break;  
    }  
  
    err = pci_enable_device(dev); // 使能PCI设备  
    if (err) {  
        dev_err(&dev->dev, "Failed to enable SMBus PCI device (%d)\n",  
            err);  
        goto exit;  
    }  
  
    /* 从SMBus手册知，偏移量0x20为bar 4，5~15比特为IO基地址 
    最低位为只读，值为1，表示SMB逻辑为IO映射(如手工读则要将基地址移位) 
    但pci_resource_start返回的是正确的地址，可直接使用 
    */  
    if (priv->type == 0) {  
        priv->smba = (u32)pci_resource_start(dev, SMBBAR);  // io bar  
        if (!priv->smba) {  
            dev_err(&dev->dev, "SMBus base address uninitialized, "  
                "upgrade BIOS\n");  
            err = -ENODEV;  
            goto exit;  
        }  
#if 0  
        err = pci_request_region(dev, SMBBAR, priv->name);  
        if (err) {  
            dev_err(&dev->dev, "Failed to request SMBus region "  
                "0x%x-0x%Lx\n", priv->smba,  
                (unsigned long long)pci_resource_end(dev, SMBBAR));  
            goto exit;  
        }  
#endif  
        ll_debug("got %s smba: 0x%x\n", priv->name, priv->smba);  
        // 这是测试0xf寄存器的值，默认为7  
        temp = inb_p(SMBSMBC(priv));  
        ll_debug("read SMBC: 0x%x\n", temp);  
          
        // 读bar0  
        priv->smba = (u32)pci_resource_start(dev, 0); // memory bar  
        ll_debug("got %s bar 0(0x10): 0x%x\n", priv->name, priv->smba);  
    }  
  
    // LPC桥的BAR没有赋值，读的话全为0  
    // 但其上有许多外设的基地址，要根据偏移量来计算，见上面的宏定义  
    if (priv->type == 1)  
    {  
        pci_read_config_dword(priv->pci_dev, SBASE, &priv->smba);  
        priv->smba &= ~0x1FF; // 为何要对齐？  
        priv->mem = (u32)ioremap(priv->smba, 4);  
        ll_debug("SPI flash base addr: 0x%x map mem: 0x%x\n", priv->smba, priv->mem);  
          
        // 读个寄存器试试  
        temp = readl((void*)(priv->mem+0xF8));  
        ll_debug("temp: 0x%x WPEN: %d WPST: %d\n", temp, (temp>>7)&0x1, (temp>>6)&0x1);  
  
        #if 0  
        int i;  
        for (i = 0x10; i < 0x90; i+=4) {  
            temp = readl((void*)(priv->mem+i));  
            ll_debug("reg[%x] value: 0x%x\n", i, temp);  
        }  
        #endif  
    }  
    if (priv->type == 2)  
    {  
        priv->smba = (u32)pci_iomap(priv->pci_dev, 0, 0);  
        ll_debug("I211 base addr: 0x%x\n", priv->smba);  
    }  
  
    pci_set_drvdata(dev, priv);  
  
#ifdef TEST_CHAR  
    // 这里可以注册cdev设备  
    if (priv->type == 0)  
        foodrv_probe();  
#endif  
  
    return 0;  
  
exit:  
    kfree(priv);  
    return err;  
}  
  
static void foo_remove(struct pci_dev *dev)  
{  
    struct foo_priv *priv = pci_get_drvdata(dev);  
  
    ll_debug("remove %s\n", priv->name);  
  
    kfree(priv);  
  
#ifdef TEST_CHAR  
    if (priv->type == 0)  
        foodrv_remove();  
#endif  
}  
  
#define foo_suspend NULL  
#define foo_resume NULL  
  
static struct pci_driver foo_driver = {  
    .name        = "foo_bus",  
    .id_table    = foo_ids,  
    .probe        = foo_probe,  
    .remove        = foo_remove,  
    //.suspend    = foo_suspend,  
    //.resume        = foo_resume,  
};  
  
static int __init foo_init(void)  
{  
    ll_debug("+++++++++++++++++++++++++++Start of Test...\n");  
    return pci_register_driver(&foo_driver);  
}  
  
static void __exit foo_exit(void)  
{  
    ll_debug("MARK...\n");  
    pci_unregister_driver(&foo_driver);  
      
    ll_debug("+++++++++++++++++++++++++++End of Exit...\n");  
}  
  
module_init(foo_init);  
module_exit(foo_exit);  
  
MODULE_AUTHOR("Jim Kent");  
MODULE_DESCRIPTION("SMBus driver");  
MODULE_LICENSE("GPL");  
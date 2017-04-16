#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/io.h>

//#define __devexit
//#define __devinitdata
//#define __devinit
//#define __devexit_p

#define MAX_12BIT       ((1 << 12) - 1)

struct my_input {
    struct input_dev    *input;
    //void __iomem        *my_input_base;
    //unsigned int        irq;
    unsigned int        wires;
    unsigned int        x_plate_resistance;
};

struct my_input_data {
    int wires;
    int x_plate_resistance;
};

static int __devinit my_input_probe(struct platform_device *pdev)
{
    const struct my_input_data *pdata = pdev->dev.platform_data;
    //struct resource *res;
    struct my_input *my_input_dev;
    struct input_dev *input_dev;
    int err;
    printk("my input device probe start\n");

    /* Allocate memory for device */
    my_input_dev = kzalloc(sizeof(struct my_input), GFP_KERNEL);
    input_dev = input_allocate_device();
    if (!my_input_dev || !input_dev) {
        dev_err(&pdev->dev, "failed to allocate memory.\n");
        err = -ENOMEM;
        goto err_free_mem;
    }

    my_input_dev->input = input_dev;
    //my_input_dev->irq = irq;
    //It will get kernel panic if no platform data - pdata
    //my_input_dev->wires = pdata->wires;
    //my_input_dev->x_plate_resistance = pdata->x_plate_resistance;
    input_dev->name = "my_input";
    input_dev->dev.parent = &pdev->dev;
    input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
    input_dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);

    input_set_abs_params(input_dev, ABS_X, 0, MAX_12BIT, 0, 0);
    input_set_abs_params(input_dev, ABS_Y, 0, MAX_12BIT, 0, 0);
    input_set_abs_params(input_dev, ABS_PRESSURE, 0, MAX_12BIT, 0, 0);

    /* register to the input system */
    err = input_register_device(input_dev);
    if (err)
        goto err_disable_clk;

    platform_set_drvdata(pdev, my_input_dev);
    printk("my input device probe successful\n");
    return 0;

err_disable_clk:
    //clk_disable(my_input_dev->tsc_ick);
    //clk_put(my_input_dev->tsc_ick);
err_free_irq:
    //free_irq(my_input_dev->irq, ts_dev);
err_unmap_regs:
    //iounmap(my_input_dev->tsc_base);
err_release_mem_region:
    //release_mem_region(res->start, resource_size(res));
err_free_mem:
    printk("my input device probe fail\n");
    input_free_device(input_dev);
    kfree(my_input_dev);
    return err;
}

static int __devexit my_input_remove(struct platform_device *pdev)
{
    struct my_input *my_input_dev = platform_get_drvdata(pdev);
    //struct resource *res;

    //free_irq(my_input_dev->irq, my_input_dev);

    input_unregister_device(my_input_dev->input);

    //res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    //iounmap(my_input_dev->tsc_base);
    //release_mem_region(res->start, resource_size(res));


    kfree(my_input_dev);

    platform_set_drvdata(pdev, NULL);
    printk("my input device probe my_input_remove\n");
    return 0;
}

static struct platform_driver my_input_driver = {
    .probe  = my_input_probe,
    .remove = __devexit_p(my_input_remove),
    .driver = {
        .name   = "my_input",
        .owner  = THIS_MODULE,
    },
};

module_platform_driver(my_input_driver);

MODULE_DESCRIPTION("My Input controller driver");
MODULE_AUTHOR("anbency");
MODULE_LICENSE("GPL");

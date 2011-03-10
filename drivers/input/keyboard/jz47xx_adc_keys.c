/*
 *  Driver for keys using ADC on Ingenic JZ47xx SoCs.
 *  Copyright (C) 2011 Peter Zotov <whitequark@whitequark.org>
 *
 *  Based on JZ4740 SoC HWMON driver:
 *    Copyright (C) 2009-2010, Lars-Peter Clausen <lars@metafoo.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/input-polldev.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/mfd/core.h>
#include <linux/hwmon.h>
#include <linux/completion.h>
#include <linux/mutex.h>

#include <jz47xx_adc_keys.h>

static struct jz_adc_keys_platform_data* jz47xx_adc_keys_config;

struct jz_adc_keys_dev {
	struct input_polled_dev *poll_dev;
	struct device *dev;
	struct platform_device *pdev;
	struct jz_adc_keys_platform_data *pdata;

	struct resource *mem;
	void __iomem *base;

	int irq;

	struct mfd_cell *cell;

	struct completion read_completion;
	struct mutex lock;

	int last_button;
};

static irqreturn_t jz_adc_keys_irq(int irq, void *data)
{
	struct jz_adc_keys_dev *bdev = data;

	complete(&bdev->read_completion);

	return IRQ_HANDLED;
}

static void jz_adc_keys_poll(struct input_polled_dev *dev)
{
	struct jz_adc_keys_dev *bdev = dev->private;
	struct completion *completion = &bdev->read_completion;
	int i, new_button;
	struct jz_adc_button* btn;
	unsigned long t;
	unsigned long val;

	mutex_lock(&bdev->lock);

	INIT_COMPLETION(*completion);

	enable_irq(bdev->irq);
	bdev->cell->enable(bdev->pdev);

	t = wait_for_completion_interruptible_timeout(completion, HZ / 10);

	if (t > 0) {
		val = readw(bdev->base) & 0xfff;
		val = (val * 3300) >> 12;

		new_button = -1;
		for(i = 0; i < bdev->pdata->nbuttons; i++) {
			btn = &bdev->pdata->buttons[i];
			if(val >= btn->value_min && val <= btn->value_max) {
				new_button = i;
				break;
			}
		}

		if(new_button != -1)
			btn = &bdev->pdata->buttons[new_button];
		else if(bdev->last_button != -1)
			btn = &bdev->pdata->buttons[bdev->last_button];
		else
			btn = NULL;

		if(btn) {
			input_event(dev->input, btn->type ?: EV_KEY, btn->code,
				(new_button != -1));
			input_sync(dev->input);
		}

		bdev->last_button = new_button;
	}

	bdev->cell->disable(bdev->pdev);
	disable_irq(bdev->irq);

	mutex_unlock(&bdev->lock);
}

void jz47xx_adc_keys_set_config(struct jz_adc_keys_platform_data* config) {
	jz47xx_adc_keys_config = config;
}

static int __devinit jz_adc_keys_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct jz_adc_keys_dev *bdev;
	struct input_polled_dev *poll_dev;
	struct input_dev *input;
	int i, ret;

	if (!jz47xx_adc_keys_config)
		return -EINVAL;

	bdev = kzalloc(sizeof(struct jz_adc_keys_dev), GFP_KERNEL);
	if (!bdev) {
		dev_err(dev, "No memory for private data\n");
		return -ENOMEM;
	}

	bdev->pdata = jz47xx_adc_keys_config;
	bdev->cell = pdev->dev.platform_data;

	bdev->irq = platform_get_irq(pdev, 0);
	if (bdev->irq < 0) {
		ret = bdev->irq;
		dev_err(dev, "Failed to get platform irq: %d\n", ret);
		goto err_free_bdev;
	}

	bdev->mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!bdev->mem) {
		ret = -ENOENT;
		dev_err(dev, "Failed to get platform mmio resource\n");
		goto err_free_bdev;
	}

	bdev->mem = request_mem_region(bdev->mem->start,
			resource_size(bdev->mem), pdev->name);
	if (!bdev->mem) {
		ret = -EBUSY;
		dev_err(dev, "Failed to request mmio memory region\n");
		goto err_free_bdev;
	}

	bdev->base = ioremap_nocache(bdev->mem->start,
			resource_size(bdev->mem));
	if (!bdev->base) {
		ret = -EBUSY;
		dev_err(dev, "Failed to ioremap mmio memory\n");
		goto err_release_mem_region;
	}

	ret = request_irq(bdev->irq, jz_adc_keys_irq, 0, pdev->name, bdev);
	if (ret) {
		dev_err(dev, "Failed to request irq: %d\n", ret);
		goto err_iounmap;
	}
	disable_irq(bdev->irq);

	poll_dev = input_allocate_polled_device();
	if (IS_ERR(poll_dev)) {
		dev_err(dev, "Cannot allocate input device\n");
		ret = PTR_ERR(poll_dev);
		goto err_free_irq;
	}

	init_completion(&bdev->read_completion);
	mutex_init(&bdev->lock);

	poll_dev->private = bdev;
	poll_dev->poll_interval = bdev->pdata->poll_interval;
	poll_dev->poll_interval_min = 10;
	poll_dev->poll_interval_max = 1000;
	poll_dev->poll = jz_adc_keys_poll;

	input = poll_dev->input;

	input->evbit[0] = BIT(EV_KEY);
	if (bdev->pdata->rep)
		__set_bit(EV_REP, input->evbit);

	input->name = pdev->name;
	input->phys = "jz-adc-keys/input0";
	input->dev.parent = &pdev->dev;

	input->id.bustype = BUS_HOST;
	input->id.vendor = 0x0001;
	input->id.product = 0x0001;
	input->id.version = 0x0100;

	input_set_drvdata(input, bdev);

	bdev->poll_dev = poll_dev;
	bdev->dev = dev;
	bdev->pdev = pdev;
	platform_set_drvdata(pdev, bdev);

	for(i = 0; i < bdev->pdata->nbuttons; i++) {
		struct jz_adc_button* btn = &bdev->pdata->buttons[i];
		int type = btn->type ?: EV_KEY;

		input_set_capability(input, type, btn->code);
	}

	ret = input_register_polled_device(poll_dev);
	if (ret) {
		dev_err(dev, "Unable to register input device: %d\n", ret);
		goto err_free_dev;
	}

	return 0;

err_free_dev:
	input_free_polled_device(poll_dev);
err_free_irq:
	free_irq(bdev->irq, bdev);
err_iounmap:
	platform_set_drvdata(pdev, NULL);
	iounmap(bdev->base);
err_release_mem_region:
	release_mem_region(bdev->mem->start, resource_size(bdev->mem));
err_free_bdev:
	platform_set_drvdata(pdev, NULL);
	kfree(bdev);

	return ret;
}

static int __devexit jz_adc_keys_remove(struct platform_device *pdev)
{
	struct jz_adc_keys_dev *bdev = platform_get_drvdata(pdev);

	input_unregister_polled_device(bdev->poll_dev);
	input_free_polled_device(bdev->poll_dev);

	free_irq(bdev->irq, bdev);

	iounmap(bdev->base);
	release_mem_region(bdev->mem->start, resource_size(bdev->mem));

	platform_set_drvdata(pdev, NULL);
	kfree(bdev);

	return 0;
}

static struct platform_driver jz_adc_keys_driver = {
	.probe	= jz_adc_keys_probe,
	.remove	= __devexit_p(jz_adc_keys_remove),
	.driver	= {
		.name	= "jz47xx-adc-keys",
		.owner	= THIS_MODULE,
	},
};

static int __init jz_adc_keys_init(void)
{
	return platform_driver_register(&jz_adc_keys_driver);
}

static void __exit jz_adc_keys_exit(void)
{
	platform_driver_unregister(&jz_adc_keys_driver);
}

module_init(jz_adc_keys_init);
module_exit(jz_adc_keys_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Peter Zotov <whitequark@whitequark.org>");
MODULE_DESCRIPTION("JZ47xx ADC buttons driver");
MODULE_ALIAS("platform:jz47xx-adc-keys");


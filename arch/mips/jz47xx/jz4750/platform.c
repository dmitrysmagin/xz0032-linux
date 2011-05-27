/*
 *  Copyright (C) 2011 Peter Zotov <whitequark@whitequark.org>
 *  JZ4750 platform devices
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under  the terms of the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/resource.h>

#include <linux/dma-mapping.h>

#include <jz4750/platform.h>
#include <jz4750/irq.h>

#include <asm/mach-jz47xx/base.h>

#include <linux/serial_core.h>
#include <linux/serial_8250.h>

#include "../serial.h"
#include "../clock.h"

/* UDC (USB gadget controller) */
static struct resource jz4750_usb_gdt_resources[] = {
	{
		.start  = JZ47XX_UDC_BASE_ADDR,
		.end    = JZ47XX_UDC_BASE_ADDR + 0x1000 - 1,
		.flags  = IORESOURCE_MEM,
	},
	{
		.start  = JZ4750_IRQ_UDC,
		.end    = JZ4750_IRQ_UDC,
		.flags  = IORESOURCE_IRQ,
        },
};

struct platform_device jz4750_udc_device = {
	.name           = "jz-udc",
	.id             = -1,
	.dev = {
		.dma_mask = &jz4750_udc_device.dev.coherent_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
	.num_resources  = ARRAY_SIZE(jz4750_usb_gdt_resources),
	.resource       = jz4750_usb_gdt_resources,
};

/* NAND controller */
static struct resource jz4750_nand_resources[] = {
	{
		.name   = "mmio",
		.start  = JZ47XX_EMC_BASE_ADDR,
		.end    = JZ47XX_EMC_BASE_ADDR + 0x1000 - 1,
		.flags  = IORESOURCE_MEM,
	},
	{
		.name   = "bch",
		.start  = JZ47XX_BCH_BASE_ADDR,
		.end    = JZ47XX_BCH_BASE_ADDR + 0x40 - 1,
		.flags  = IORESOURCE_MEM,
	},
	{
		.name   = "bank",
		.start  = 0x18000000,
		.end    = 0x180C0000 - 1,
		.flags  = IORESOURCE_MEM,
	},
};

struct platform_device jz4750_nand_device = {
	.name = "jz4750-nand",
	.num_resources = ARRAY_SIZE(jz4750_nand_resources),
	.resource = jz4750_nand_resources,
};

/* MMC/SD controllers */
static struct resource jz4750_mmc0_resources[] = {
	{
		.start	= JZ47XX_MSC0_BASE_ADDR,
		.end	= JZ47XX_MSC0_BASE_ADDR + 0x1000 - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= JZ4750_IRQ_MSC0,
		.end	= JZ4750_IRQ_MSC0,
		.flags	= IORESOURCE_IRQ,
	}
};

struct platform_device jz4750_mmc0_device = {
	.name		= "jz4740-mmc",
	.id		= 0,
	.dev = {
		.dma_mask = &jz4750_mmc0_device.dev.coherent_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
	.num_resources  = ARRAY_SIZE(jz4750_mmc0_resources),
	.resource	= jz4750_mmc0_resources,
};

static struct resource jz4750_mmc1_resources[] = {
	{
		.start	= JZ47XX_MSC1_BASE_ADDR,
		.end	= JZ47XX_MSC1_BASE_ADDR + 0x1000 - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= JZ4750_IRQ_MSC1,
		.end	= JZ4750_IRQ_MSC1,
		.flags	= IORESOURCE_IRQ,
	}
};

struct platform_device jz4750_mmc1_device = {
	.name		= "jz4740-mmc",
	.id		= 1,
	.dev = {
		.dma_mask = &jz4750_mmc1_device.dev.coherent_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
	.num_resources  = ARRAY_SIZE(jz4750_mmc1_resources),
	.resource	= jz4750_mmc1_resources,
};


/* LCD controller */
static struct resource jz4750_framebuffer_resources[] = {
	{
		.start  = JZ47XX_LCD_BASE_ADDR,
		.end    = JZ47XX_LCD_BASE_ADDR + 0x1000 - 1,
		.flags  = IORESOURCE_MEM,
	},
};

struct platform_device jz4750_framebuffer_device = {
	.name           = "jz4740-fb",
	.id             = -1,
	.num_resources  = ARRAY_SIZE(jz4750_framebuffer_resources),
	.resource       = jz4750_framebuffer_resources,
	.dev = {
		.dma_mask = &jz4750_framebuffer_device.dev.coherent_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
};

/* RTC controller */
static struct resource jz4750_rtc_resources[] = {
	{
		.start  = JZ47XX_RTC_BASE_ADDR,
		.end    = JZ47XX_RTC_BASE_ADDR + 0x38 - 1,
		.flags  = IORESOURCE_MEM,
	},
	{
		.start  = JZ4750_IRQ_RTC,
		.end    = JZ4750_IRQ_RTC,
		.flags  = IORESOURCE_IRQ,
	},
};

struct platform_device jz4750_rtc_device = {
	.name           = "jz4740-rtc",
	.id             = -1,
	.num_resources  = ARRAY_SIZE(jz4750_rtc_resources),
	.resource       = jz4750_rtc_resources,
};

/* ADC controller */
static struct resource jz4750_adc_resources[] = {
	{
		.start	= JZ47XX_SADC_BASE_ADDR,
		.end	= JZ47XX_SADC_BASE_ADDR + 0x30,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= JZ4750_IRQ_SADC,
		.end	= JZ4750_IRQ_SADC,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= JZ4750_IRQ_ADC_BASE,
		.end	= JZ4750_IRQ_ADC_BASE,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device jz4750_adc_device = {
	.name		= "jz4740-adc",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(jz4750_adc_resources),
	.resource	= jz4750_adc_resources,
};

/* I2C controller */
static struct resource jz4750_i2c_resources[] = {
	{
		.start	= JZ47XX_I2C_BASE_ADDR,
		.end	= JZ47XX_I2C_BASE_ADDR + 0x1000 - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= JZ4750_IRQ_I2C,
		.end	= JZ4750_IRQ_I2C,
		.flags	= IORESOURCE_IRQ,
	}
};

struct platform_device jz4750_i2c_device = {
	.name		= "jz47xx-i2c",
	.id		= 0,
	.num_resources  = ARRAY_SIZE(jz4750_i2c_resources),
	.resource	= jz4750_i2c_resources,
};

#ifdef CONFIG_MACH_JZ4750L
/* IPU */
static struct resource jz4750l_ipu_resources[] = {
	{
		.start	= JZ47XX_IPU_BASE_ADDR,
		.end	= JZ47XX_IPU_BASE_ADDR + 0xA4 - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= JZ4750_IRQ_IPU,
		.end	= JZ4750_IRQ_IPU,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device jz4750l_ipu_device = {
	.name		= "jz4750l-ipu",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(jz4750l_ipu_resources),
	.resource	= jz4750l_ipu_resources,
};
#endif

/* Serial */
#define JZ4750_UART_DATA(_id) \
	{ \
		.flags = UPF_SKIP_TEST | UPF_IOREMAP | UPF_FIXED_TYPE, \
		.iotype = UPIO_MEM, \
		.regshift = 2, \
		.serial_out = jz4740_serial_out, \
		.type = PORT_16550, \
		.mapbase = JZ47XX_UART ## _id ## _BASE_ADDR, \
		.irq = JZ4750_IRQ_UART ## _id, \
	}

static struct plat_serial8250_port jz4750_uart_data[] = {
	JZ4750_UART_DATA(0),
	{},
};

static struct platform_device jz4740_uart_device = {
	.name = "serial8250",
	.id = 0,
	.dev = {
		.platform_data = jz4750_uart_data,
	},
};

void jz4750_serial_device_register(void)
{
	struct plat_serial8250_port *p;

	for (p = jz4750_uart_data; p->flags != 0; ++p)
		p->uartclk = jz47xx_clock_bdata.ext_rate;

	platform_device_register(&jz4740_uart_device);
}


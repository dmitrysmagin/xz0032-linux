/*
 *  Copyright (C) 2009-2010, Lars-Peter Clausen <lars@metafoo.de>
 *  JZ4740 platform devices
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

#include <jz4760/platform.h>
#include <jz4760/irq.h>

#include <asm/mach-jz47xx/jz4760/base.h>

#include <linux/serial_core.h>
#include <linux/serial_8250.h>

#include "../serial.h"
#include "../clock.h"

/* NAND controller */
static struct resource jz4760_nand_resources[] = {
	{
		.name   = "mmio",
		.start  = JZ47XX_EMC_BASE_ADDR,
		.end    = JZ47XX_EMC_BASE_ADDR + 0x1000 - 1,
		.flags  = IORESOURCE_MEM,
	},
	{
		.name   = "bch",
		.start  = JZ4760_BCH_BASE_ADDR,
		.end    = JZ4760_BCH_BASE_ADDR + 0x10000 - 1,
		.flags  = IORESOURCE_MEM,
	},
	{
		.name   = "bank",
		.start  = 0x1A000000,
		.end    = 0x1A0C0000 - 1,
		.flags  = IORESOURCE_MEM,
	},
};

struct platform_device jz4760_nand_device = {
	.name = "jz4750-nand",
	.num_resources = ARRAY_SIZE(jz4760_nand_resources),
	.resource = jz4760_nand_resources,
};

/* RTC controller */
static struct resource jz4760_rtc_resources[] = {
	{
		.start	= JZ47XX_RTC_BASE_ADDR,
		.end	= JZ47XX_RTC_BASE_ADDR + 0x38 - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start  = JZ4760_IRQ_RTC,
		.end	= JZ4760_IRQ_RTC,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device jz4760_rtc_device = {
	.name		= "jz4740-rtc",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(jz4760_rtc_resources),
	.resource	= jz4760_rtc_resources,
};

/* OHCI controller */
static struct resource jz4760_usb_ohci_resources[] = {
	{
		.start	= JZ4760_UHC_BASE_ADDR,
		.end	= JZ4760_UHC_BASE_ADDR + 0x1000 - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start  = JZ4760_IRQ_UHC,
		.end	= JZ4760_IRQ_UHC,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device jz4760_usb_ohci_device = {
	.name		= "jz4740-ohci",
	.id		= -1,
	.dev = {
		.dma_mask = &jz4760_usb_ohci_device.dev.coherent_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
	.num_resources	= ARRAY_SIZE(jz4760_usb_ohci_resources),
	.resource	= jz4760_usb_ohci_resources,
};

/* MMC/SD controller */
static struct resource jz4760_mmc0_resources[] = {
	{
		.start	= JZ47XX_MSC0_BASE_ADDR,
		.end	= JZ47XX_MSC0_BASE_ADDR + 0x1000 - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= JZ4760_IRQ_MSC0,
		.end	= JZ4760_IRQ_MSC0,
		.flags	= IORESOURCE_IRQ,
	}
};

struct platform_device jz4760_mmc0_device = {
	.name		= "jz4740-mmc",
	.id		= 0,
	.dev = {
		.dma_mask = &jz4760_mmc0_device.dev.coherent_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
	.num_resources  = ARRAY_SIZE(jz4760_mmc0_resources),
	.resource	= jz4760_mmc0_resources,
};

static struct resource jz4760_mmc1_resources[] = {
	{
		.start	= JZ47XX_MSC1_BASE_ADDR,
		.end	= JZ47XX_MSC1_BASE_ADDR + 0x1000 - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= JZ4760_IRQ_MSC1,
		.end	= JZ4760_IRQ_MSC1,
		.flags	= IORESOURCE_IRQ,
	}
};

struct platform_device jz4760_mmc1_device = {
	.name		= "jz4740-mmc",
	.id		= 1,
	.dev = {
		.dma_mask = &jz4760_mmc1_device.dev.coherent_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
	.num_resources  = ARRAY_SIZE(jz4760_mmc1_resources),
	.resource	= jz4760_mmc1_resources,
};

static struct resource jz4760_mmc2_resources[] = {
	{
		.start	= JZ47XX_MSC2_BASE_ADDR,
		.end	= JZ47XX_MSC2_BASE_ADDR + 0x1000 - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= JZ4760_IRQ_MSC2,
		.end	= JZ4760_IRQ_MSC2,
		.flags	= IORESOURCE_IRQ,
	}
};


struct platform_device jz4760_mmc2_device = {
	.name		= "jz4740-mmc",
	.id		= 2,
	.dev = {
		.dma_mask = &jz4760_mmc2_device.dev.coherent_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
	.num_resources  = ARRAY_SIZE(jz4760_mmc2_resources),
	.resource	= jz4760_mmc2_resources,
};

/* LCD controller */
static struct resource jz4760_framebuffer_resources[] = {
	{
		.start	= JZ47XX_LCD_BASE_ADDR,
		.end	= JZ47XX_LCD_BASE_ADDR + 0x1000 - 1,
		.flags	= IORESOURCE_MEM,
	},
};

struct platform_device jz4760_framebuffer_device = {
	.name		= "jz4740-fb",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(jz4760_framebuffer_resources),
	.resource	= jz4760_framebuffer_resources,
	.dev = {
		.dma_mask = &jz4760_framebuffer_device.dev.coherent_dma_mask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
	},
};

/* ADC controller */
static struct resource jz4760_adc_resources[] = {
	{
		.start	= JZ47XX_SADC_BASE_ADDR,
		.end	= JZ47XX_SADC_BASE_ADDR + 0x30,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= JZ4760_IRQ_SADC,
		.end	= JZ4760_IRQ_SADC,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= JZ4760_IRQ_ADC_BASE,
		.end	= JZ4760_IRQ_ADC_BASE,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device jz4760_adc_device = {
	.name		= "jz4740-adc",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(jz4760_adc_resources),
	.resource	= jz4760_adc_resources,
};

/* Serial */
#define JZ4760_UART_DATA(_id) \
	{ \
		.flags = UPF_SKIP_TEST | UPF_IOREMAP | UPF_FIXED_TYPE, \
		.iotype = UPIO_MEM, \
		.regshift = 2, \
		.serial_out = jz4740_serial_out, \
		.type = PORT_16550, \
		.mapbase = JZ47XX_UART ## _id ## _BASE_ADDR, \
		.irq = JZ4760_IRQ_UART ## _id, \
	}

static struct plat_serial8250_port jz4760_uart_data[] = {
	JZ4760_UART_DATA(0),
	JZ4760_UART_DATA(1),
	{},
};

static struct platform_device jz4760_uart_device = {
	.name = "serial8250",
	.id = 0,
	.dev = {
		.platform_data = jz4760_uart_data,
	},
};

void jz4760_serial_device_register(void)
{
	struct plat_serial8250_port *p;

	for (p = jz4760_uart_data; p->flags != 0; ++p)
		p->uartclk = jz47xx_clock_bdata.ext_rate;

	platform_device_register(&jz4760_uart_device);
}

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
#include <jz4760/base.h>
#include <jz4760/irq.h>

#include <linux/serial_core.h>
#include <linux/serial_8250.h>

#include "../serial.h"
#include "../clock.h"

/* Serial */
#define JZ4760_UART_DATA(_id) \
	{ \
		.flags = UPF_SKIP_TEST | UPF_IOREMAP | UPF_FIXED_TYPE, \
		.iotype = UPIO_MEM, \
		.regshift = 2, \
		.serial_out = jz4740_serial_out, \
		.type = PORT_16550, \
		.mapbase = JZ4740_UART ## _id ## _BASE_ADDR, \
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
		p->uartclk = jz4740_clock_bdata.ext_rate;

	platform_device_register(&jz4760_uart_device);
}

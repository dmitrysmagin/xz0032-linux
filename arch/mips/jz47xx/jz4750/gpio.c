/*
 *  Copyright (C) 2011 Peter Zotov <whitequark@whitequark.org>
 *  JZ4750 GPIO config
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

#include <linux/init.h>

#include <asm/mach-jz47xx/jz4750/irq.h>
#include <asm/mach-jz47xx/gpio.h>

static struct jz_gpio_chip jz4750_gpio_chips[] = {
	JZ47XX_GPIO_CHIP("A", 16, JZ4750_IRQ_GPIO0),
	JZ47XX_GPIO_CHIP("B", 32, JZ4750_IRQ_GPIO1),
	JZ47XX_GPIO_CHIP("C", 32, JZ4750_IRQ_GPIO2), /* except PB15, PB22, PB23, PB26~PE29 */
	JZ47XX_GPIO_CHIP("D", 32, JZ4750_IRQ_GPIO3),
};

static int __init jz4750_gpio_init(void)
{
	jz47xx_gpio_init(jz4750_gpio_chips, ARRAY_SIZE(jz4750_gpio_chips),
			JZ4750_IRQ_GPIO(0));
	return 0;
}
arch_initcall(jz4750_gpio_init);

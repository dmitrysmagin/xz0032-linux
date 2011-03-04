/*
 *  Copyright (C) 2011 Peter Zotov <whitequark@whitequark.org>
 *  XZ0032 JZ4725B board support
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

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>

#include <jz4750/platform.h>

#include "../clock.h"

static struct platform_device *jz_platform_devices[] __initdata = {
};

static int __init xz0032_init_platform_devices(void)
{
	jz4750_serial_device_register();

	return platform_add_devices(jz_platform_devices,
					ARRAY_SIZE(jz_platform_devices));

}

struct jz4740_clock_board_data jz4740_clock_bdata = {
	.ext_rate = 12000000,
	.rtc_rate = 32768,
};

static int __init xz0032_board_setup(void)
{
	printk(KERN_INFO "XZ0032 (JZ4725B) setup\n");

	if (xz0032_init_platform_devices())
		panic("Failed to initalize platform devices\n");

	return 0;
}
arch_initcall(xz0032_board_setup);

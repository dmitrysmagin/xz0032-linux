#include <linux/kernel.h>
#include <linux/init.h>

#include <jz4760/platform.h>

#include "../clock.h"

static int __init lepus_init_platform_devices(void)
{
	jz4760_serial_device_register();

	return 0;
}

struct jz4740_clock_board_data jz4740_clock_bdata = {
	.ext_rate = 12000000,
	.rtc_rate = 32768,
};

static int __init lepus_board_setup(void)
{
	if (lepus_init_platform_devices())
		panic("Failed to initalize platform devices\n");

	return 0;
}
arch_initcall(lepus_board_setup);

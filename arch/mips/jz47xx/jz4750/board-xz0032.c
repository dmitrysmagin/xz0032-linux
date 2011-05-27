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
#include <linux/gpio_keys.h>
#include <linux/power_supply.h>
#include <linux/power/gpio-charger.h>
#include <linux/power/jz4740-battery.h>
#include <linux/pwm_backlight.h>
#include <linux/input.h>

#include <jz4750/platform.h>

#include "../clock.h"

#include <jz4750_nand.h>
#include <jz4740_fb.h>
#include <jz4740_mmc.h>
#include <jz47xx_adc_keys.h>

static long xz0032_panic_blink(int state) {
	gpio_set_value(JZ_GPIO_PORTC(15), state ? 1 : 0);
	return 0;
}

/* NAND */
static struct nand_ecclayout xz0032_ecclayout = {
	.eccbytes = 52,
	.eccpos = {
		3,  4,  5,  6,  7,  8,  9,  10,
		11, 12, 13, 14, 15, 16, 17, 18,
		19, 20, 21, 22, 23, 24, 25, 26,
		27, 28, 29, 30, 31, 32, 33, 34,
		35, 36, 37, 38, 39, 40, 41, 42,
		43, 44, 45, 46, 47, 48, 49, 50,
		51, 52, 53, 54, 55,
	},
	.oobfree = {
		{ .offset = 0,  .length = 3 },
		{ .offset = 56, .length = 8 },
	},
};

static struct mtd_partition xz0032_partitions[] = {
	{
		.name   = "u-boot",
		.offset = 0,
		.size   = 1  * 0x100000,
	},
	{
		.name   = "kernel",
		.offset = 1  * 0x100000,
		.size   = 15 * 0x100000,
	},
	{
		.name   = "rootfs",
		.offset = 16 * 0x100000,
		.size   = (1024 - 16) * 0x100000,
	},
};

static struct jz_nand_platform_data xz0032_nand_pdata = {
	.bch_8bit       = 1,
	.ecc_layout     = &xz0032_ecclayout,
	.partitions     = xz0032_partitions,
	.num_partitions = ARRAY_SIZE(xz0032_partitions),
	.busy_gpio      = 91,
};

/* MMC controller */

static struct jz_gpio_bulk_request xz0032_mmc1_pins[] = {
	JZ_GPIO_BULK_PIN(MSC1_CMD),
	JZ_GPIO_BULK_PIN(MSC1_CLK),
	JZ_GPIO_BULK_PIN(MSC1_DATA0),
};

static struct jz4740_mmc_platform_data xz0032_mmc1_pdata = {
	.gpio_card_detect	= JZ_GPIO_PORTC(20),
	.gpio_read_only		= -1,
	.gpio_power		= -1,

	.card_detect_active_low	= 1,

	.data_1bit		= 1,

	.mmc_pins 		= xz0032_mmc1_pins,
	.mmc_pin_count		= ARRAY_SIZE(xz0032_mmc1_pins),
};

/* LCD */

static struct fb_videomode xz0032_video_modes[] = {
	{
		.name = "480x272",
		.xres = 480,
		.yres = 272,
		.refresh = 60,
		.left_margin = 2,
		.right_margin = 2,
		.upper_margin = 2,
		.lower_margin = 2,
		.hsync_len = 41,
		.vsync_len = 10,
		.sync = 0,
		.vmode = FB_VMODE_NONINTERLACED,
	},
};

static struct jz4740_fb_platform_data xz0032_fb_pdata = {
	.width          = 90,
	.height         = 52,
	.num_modes      = ARRAY_SIZE(xz0032_video_modes),
	.modes          = xz0032_video_modes,
	.bpp            = 16,
	.lcd_type       = JZ_LCD_TYPE_GENERIC_16_BIT,
	.pixclk_falling_edge = 1,

	.gpio_reset	= JZ_GPIO_PORTD(24),
	.reset_active_low = 1,
};

/* Battery */
static struct jz_battery_platform_data xz0032_battery_pdata = {
	.gpio_charge = -1,
	.info = {
		.name = "battery",
		.technology = POWER_SUPPLY_TECHNOLOGY_LIPO,
		.voltage_max_design = 4100000,
		.voltage_min_design = 2700000,
	},
};

/* Charger */
static char *xz0032_batteries[] = {
	"battery",
};

static struct gpio_charger_platform_data xz0032_charger_pdata = {
	.name = "usb",
	.type = POWER_SUPPLY_TYPE_USB,
	.gpio = JZ_GPIO_PORTD(16),
	.gpio_active_low = 0,
	.supplied_to = xz0032_batteries,
	.num_supplicants = ARRAY_SIZE(xz0032_batteries),
};

static struct platform_device xz0032_charger_device = {
	.name = "gpio-charger",
	.dev = {
		.platform_data = &xz0032_charger_pdata,
	},
};

/* PWM controlled backlight */
static struct platform_pwm_backlight_data xz0032_backlight_data = {
	.pwm_id         = 5,
	.max_brightness = 100,
	.dft_brightness = 100,
	.pwm_period_ns  = 10000000,
};

static struct platform_device xz0032_pwm_backlight_device = {
	.name   = "pwm-backlight",
	.dev    = {
		.platform_data  = &xz0032_backlight_data,
	}
};

/* Buttons */
static struct gpio_keys_button xz0032_gpio_keys_buttons[] = {
#ifdef CONFIG_JZ4750L_XZ0032_UART0_RX_PIN_AS_KEY
	{
		.code		= KEY_A,
		.gpio		= JZ_GPIO_PORTC(31),
		.active_low	= 1,
		.desc		= "K1",
	},
#endif
	{
		.code		= KEY_B,
		.gpio		= JZ_GPIO_PORTD(23),
		.active_low	= 1,
		.desc		= "K2",
	},
#ifdef CONFIG_JZ4750L_XZ0032_I2C_PINS_AS_KEYS
	{
		.code		= KEY_VOLUMEUP,
		.gpio		= JZ_GPIO_PORTC(10),
		.active_low	= 1,
		.desc		= "VOL+",
	},
	{
		.code		= KEY_VOLUMEDOWN,
		.gpio		= JZ_GPIO_PORTC(11),
		.active_low	= 1,
		.desc		= "VOL-",
	},
#endif
	{
		.code		= KEY_POWER,
		.gpio		= JZ_GPIO_PORTB(31),
		.active_low	= 1,
		.desc		= "POWER",
	},
};

static struct gpio_keys_platform_data xz0032_gpio_keys_data = {
	.nbuttons = ARRAY_SIZE(xz0032_gpio_keys_buttons),
	.buttons  = xz0032_gpio_keys_buttons,
	.rep      = 1,
};

static struct platform_device xz0032_gpio_keys_device = {
	.name =	"gpio-keys",
	.id =	-1,
	.dev = {
		.platform_data = &xz0032_gpio_keys_data,
	}
};

/* ADC buttons */
#ifdef CONFIG_KEYBOARD_JZ47XX_ADC
static struct jz_adc_button xz0032_adc_keys[] = {
	{
		.code      = KEY_C,
		.value_min = 550,
		.value_max = 750,
		.desc      = "K3",
	},
	{
		.code      = KEY_D,
		.value_min = 950,
		.value_max = 1150,
		.desc      = "K4",
	},
	{
		.code      = KEY_E,
		.value_min = 1450,
		.value_max = 1650,
		.desc      = "K5",
	},
	{
		.code      = KEY_F,
		.value_min = 1950,
		.value_max = 2150,
		.desc      = "K6",
	},
};

static struct jz_adc_keys_platform_data xz0032_adc_keys_pdata = {
	.buttons  = xz0032_adc_keys,
	.nbuttons = ARRAY_SIZE(xz0032_adc_keys),
	.rep      = 1,
	.poll_interval = 50,
};
#endif

static struct platform_device *jz_platform_devices[] __initdata = {
	&jz4750_rtc_device,
	&jz4750_udc_device,
	&jz4750_nand_device,
	&jz4750_framebuffer_device,
#ifndef CONFIG_JZ4750L_XZ0032_I2C_GPIO_AS_KEYS
	&jz4750_i2c_device,
#endif
	&jz4750_adc_device,
	&jz4750_mmc1_device,
	&xz0032_charger_device,
	&xz0032_pwm_backlight_device,
	&xz0032_gpio_keys_device,
	&jz4750l_ipu_device
};

static int __init xz0032_init_platform_devices(void)
{
	jz4750_nand_device.dev.platform_data = &xz0032_nand_pdata;
	jz4750_framebuffer_device.dev.platform_data = &xz0032_fb_pdata;
	jz4750_adc_device.dev.platform_data = &xz0032_battery_pdata;
	jz4750_mmc1_device.dev.platform_data = &xz0032_mmc1_pdata;

#ifdef CONFIG_KEYBOARD_JZ47XX_ADC
	jz47xx_adc_keys_set_config(&xz0032_adc_keys_pdata);
#endif

	panic_blink = xz0032_panic_blink;

	jz4750_serial_device_register();

	return platform_add_devices(jz_platform_devices,
					ARRAY_SIZE(jz_platform_devices));

}

struct jz47xx_clock_board_data jz47xx_clock_bdata = {
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

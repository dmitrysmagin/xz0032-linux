/*
 *  Copyright (C) 2009, Lars-Peter Clausen <lars@metafoo.de>
 *  JZ4740 GPIO pin definitions
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under  the terms of the GNU General Public License as published by the
 *  Free Software Foundation;  either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __ASM_MACH_JZ47XX_GPIO_H__
#define __ASM_MACH_JZ47XX_GPIO_H__

#include <linux/types.h>

enum jz_gpio_function {
    JZ_GPIO_FUNC_NONE,
    JZ_GPIO_FUNC1,
    JZ_GPIO_FUNC2,
    JZ_GPIO_FUNC3,
};

/*
 Usually a driver for a SoC component has to request several gpio pins and
 configure them as funcion pins.
 jz_gpio_bulk_request can be used to ease this process.
 Usually one would do something like:

 const static struct jz_gpio_bulk_request i2c_pins[] = {
	JZ_GPIO_BULK_PIN(I2C_SDA),
	JZ_GPIO_BULK_PIN(I2C_SCK),
 };

 inside the probe function:

    ret = jz_gpio_bulk_request(i2c_pins, ARRAY_SIZE(i2c_pins));
    if (ret) {
	...

 inside the remove function:

    jz_gpio_bulk_free(i2c_pins, ARRAY_SIZE(i2c_pins));


*/
struct jz_gpio_bulk_request {
	int gpio;
	const char *name;
	enum jz_gpio_function function;
};

#define JZ_GPIO_BULK_PIN(pin) { \
    .gpio = JZ_GPIO_ ## pin, \
    .name = #pin, \
    .function = JZ_GPIO_FUNC_ ## pin \
}

int jz_gpio_bulk_request(const struct jz_gpio_bulk_request *request, size_t num);
void jz_gpio_bulk_free(const struct jz_gpio_bulk_request *request, size_t num);
void jz_gpio_bulk_suspend(const struct jz_gpio_bulk_request *request, size_t num);
void jz_gpio_bulk_resume(const struct jz_gpio_bulk_request *request, size_t num);
void jz_gpio_enable_pullup(unsigned gpio);
void jz_gpio_disable_pullup(unsigned gpio);
int jz_gpio_set_function(int gpio, enum jz_gpio_function function);

int jz_gpio_port_direction_input(int port, uint32_t mask);
int jz_gpio_port_direction_output(int port, uint32_t mask);
void jz_gpio_port_set_value(int port, uint32_t value, uint32_t mask);
uint32_t jz_gpio_port_get_value(int port, uint32_t mask);

#include <asm/mach-generic/gpio.h>

#define JZ_GPIO_PORTA(x) ((x) + 32 * 0)
#define JZ_GPIO_PORTB(x) ((x) + 32 * 1)
#define JZ_GPIO_PORTC(x) ((x) + 32 * 2)
#define JZ_GPIO_PORTD(x) ((x) + 32 * 3)

#endif

#include <linux/init.h>
#include <asm/mach-jz47xx/pwm.h>
#include <asm/mach-jz47xx/jz4760/gpio.h>

static struct pwm_device jz4760_pwm_list[] = {
	{ 0, JZ_GPIO_PORTE(0) },
	{ 1, JZ_GPIO_PORTE(1) },
	{ 2, JZ_GPIO_PORTE(2) },
	{ 3, JZ_GPIO_PORTE(3) },
	{ 4, JZ_GPIO_PORTD(4) },
	{ 5, JZ_GPIO_PORTD(5) },
	{ 6, JZ_GPIO_PORTD(10) },
	{ 7, JZ_GPIO_PORTD(11) },
};

static int __init jz4760_pwm_init(void)
{
	return jz47xx_pwm_init(jz4760_pwm_list, ARRAY_SIZE(jz4760_pwm_list));
}
subsys_initcall(jz4760_pwm_init);

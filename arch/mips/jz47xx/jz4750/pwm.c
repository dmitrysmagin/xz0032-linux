#include <linux/init.h>
#include <asm/mach-jz47xx/pwm.h>
#include <asm/mach-jz47xx/jz4750/gpio.h>

static struct pwm_device jz4750_pwm_list[] = {
	{ 0, JZ_GPIO_PWM0 },
	{ 1, JZ_GPIO_PWM1 },
	{ 2, JZ_GPIO_PWM2 },
	{ 3, JZ_GPIO_PWM3 },
	{ 4, JZ_GPIO_PWM4 },
	{ 5, JZ_GPIO_PWM5 },
};

static int __init jz4750_pwm_init(void)
{
	return jz47xx_pwm_init(jz4750_pwm_list, ARRAY_SIZE(jz4750_pwm_list));
}
subsys_initcall(jz4750_pwm_init);

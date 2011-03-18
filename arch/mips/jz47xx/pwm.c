/*
 *  Copyright (C) 2010, Lars-Peter Clausen <lars@metafoo.de>
 *  JZ47xx platform PWM support
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

#include <linux/clk.h>
#include <linux/err.h>
#include <linux/pwm.h>
#include <linux/gpio.h>

#include <asm/mach-jz47xx/pwm.h>
#include "timer.h"

static struct clk *jz47xx_pwm_clk;

static struct pwm_device* jz47xx_pwm_list;
static int jz47xx_pwm_count;

DEFINE_MUTEX(jz47xx_pwm_mutex);

struct pwm_device *pwm_request(int id, const char *label)
{
	int ret = 0, i;
	struct pwm_device *pwm = NULL;

	if (!jz47xx_pwm_clk)
		return ERR_PTR(-ENODEV);

	mutex_lock(&jz47xx_pwm_mutex);

	for(i = 0; i < jz47xx_pwm_count; i++) {
		if(jz47xx_pwm_list[i].id == id) {
			pwm = &jz47xx_pwm_list[i];
			break;
		}
	}

	if(!pwm)
		return ERR_PTR(-ENODEV);

	if (pwm->used)
		ret = -EBUSY;
	else
		pwm->used = true;

	mutex_unlock(&jz47xx_pwm_mutex);

	if (ret)
		return ERR_PTR(ret);

	ret = gpio_request(pwm->gpio, label);

	if (ret) {
		printk(KERN_ERR "Failed to request pwm gpio: %d\n", ret);
		pwm->used = false;
		return ERR_PTR(ret);
	}

	/* All JZ47XX SoCs use FUNC1 for their PWM pins, if this changes the
	 * function type has to be added to the pwm_device. */
	jz_gpio_set_function(pwm->gpio, JZ_GPIO_FUNC1);
	gpio_direction_output(pwm->gpio, 1);

	jz4740_timer_start(id);

	return pwm;
}

void pwm_free(struct pwm_device *pwm)
{
	pwm_disable(pwm);
	jz4740_timer_set_ctrl(pwm->id, 0);

	gpio_direction_input(pwm->gpio);
	jz_gpio_set_function(pwm->gpio, JZ_GPIO_FUNC_NONE);
	gpio_free(pwm->gpio);

	jz4740_timer_stop(pwm->id);

	pwm->used = false;
}

int pwm_config(struct pwm_device *pwm, int duty_ns, int period_ns)
{
	unsigned long long tmp;
	unsigned long period, duty;
	unsigned int prescaler = 0;
	unsigned int id = pwm->id;
	uint16_t ctrl;
	bool is_enabled;

	if (duty_ns < 0 || duty_ns > period_ns)
		return -EINVAL;

	tmp = (unsigned long long)clk_get_rate(jz47xx_pwm_clk) * period_ns;
	do_div(tmp, 1000000000);
	period = tmp;

	while (period > 0xffff && prescaler < 6) {
		period >>= 2;
		++prescaler;
	}

	if (prescaler == 6)
		return -EINVAL;

	tmp = (unsigned long long)period * duty_ns;
	do_div(tmp, period_ns);
	duty = period - tmp;

	if (duty >= period)
		duty = period - 1;

	is_enabled = jz4740_timer_is_enabled(id);
	if (is_enabled)
		pwm_disable(pwm);

	jz4740_timer_set_count(id, 0);
	jz4740_timer_set_duty(id, duty);
	jz4740_timer_set_period(id, period);

	ctrl = JZ_TIMER_CTRL_PRESCALER(prescaler) | JZ_TIMER_CTRL_SRC_EXT |
		JZ_TIMER_CTRL_PWM_ABBRUPT_SHUTDOWN;

	jz4740_timer_set_ctrl(id, ctrl);

	if (is_enabled)
		pwm_enable(pwm);

	return 0;
}

int pwm_enable(struct pwm_device *pwm)
{
	uint32_t ctrl = jz4740_timer_get_ctrl(pwm->id);

	ctrl |= JZ_TIMER_CTRL_PWM_ENABLE;
	jz4740_timer_set_ctrl(pwm->id, ctrl);
	jz4740_timer_enable(pwm->id);

	return 0;
}

void pwm_disable(struct pwm_device *pwm)
{
	uint32_t ctrl = jz4740_timer_get_ctrl(pwm->id);

	ctrl &= ~JZ_TIMER_CTRL_PWM_ENABLE;
	jz4740_timer_set_ctrl(pwm->id, ctrl);
	jz4740_timer_disable(pwm->id);
}

int __init jz47xx_pwm_init(struct pwm_device* pwm_list, int pwm_count)
{
	int ret = 0;

	jz47xx_pwm_clk = clk_get(NULL, "ext");

	if (IS_ERR(jz47xx_pwm_clk)) {
		ret = PTR_ERR(jz47xx_pwm_clk);
		jz47xx_pwm_clk = NULL;
	}

	jz47xx_pwm_list = pwm_list;
	jz47xx_pwm_count = pwm_count;

	return ret;
}

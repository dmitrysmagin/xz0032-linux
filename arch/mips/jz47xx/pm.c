/*
 *  Copyright (C) 2010, Lars-Peter Clausen <lars@metafoo.de>
 *	JZ4740 SoC power management support
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
#include <linux/pm.h>
#include <linux/delay.h>
#include <linux/suspend.h>

#include <clock.h>

#include "clock.h"
#include "irq.h"

static int jz47xx_pm_enter(suspend_state_t state)
{
	jz4740_intc_suspend();
	jz47xx_clock_suspend();

	jz47xx_clock_set_wait_mode(JZ47XX_WAIT_MODE_SLEEP);

	__asm__(".set\tmips3\n\t"
		"wait\n\t"
		".set\tmips0");

	jz47xx_clock_set_wait_mode(JZ47XX_WAIT_MODE_IDLE);

	jz47xx_clock_resume();
	jz4740_intc_resume();

	return 0;
}

static const struct platform_suspend_ops jz47xx_pm_ops = {
	.valid		= suspend_valid_only_mem,
	.enter		= jz47xx_pm_enter,
};

static int __init jz47xx_pm_init(void)
{
	suspend_set_ops(&jz47xx_pm_ops);
	return 0;

}
late_initcall(jz47xx_pm_init);

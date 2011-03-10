/*
 *  Copyright (C) 2011 Peter Zotov <whitequark@whitequark.org>
 *  JZ47xx PWM definitions
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

#ifndef __ASM_MACH_JZ47XX_PWM_H__
#define __ASM_MACH_JZ47XX_PWM_H__

#include <linux/kernel.h>
#include <linux/pwm.h>

struct pwm_device {
	unsigned int id;
	unsigned int gpio;
	bool used;
};

extern int __init jz47xx_pwm_init(struct pwm_device *pwm_list, int pwm_count);

#endif

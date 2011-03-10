/*
 *  Copyright (C) 2011, Peter Zotov <whitequark@whitequark.org>
 *  JZ47xx ADC keys input driver
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __ASM_MACH_JZ47XX_ADC_KEYS_H__
#define __ASM_MACH_JZ47XX_ADC_KEYS_H__

#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

struct jz_adc_button {
	unsigned int value_min;
	unsigned int value_max;

	int code;		/* input event code (KEY_*, SW_*) */
	char *desc;
	int type;		/* input event type (EV_KEY, EV_SW) */
};

struct jz_adc_keys_platform_data {
	int			nbuttons;
	struct jz_adc_button*	buttons;

	unsigned int poll_interval;
	unsigned int rep:1;		/* enable input subsystem auto repeat */
};

extern void jz47xx_adc_keys_set_config(struct jz_adc_keys_platform_data* config);

#endif

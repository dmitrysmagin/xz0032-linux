/*
 *  Copyright (C) 2011 Peter Zotov <whitequark@whitequark.org>
 *  JZ4750 timer setup
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

#include <asm/time.h>
#include <asm/mach-jz47xx/jz4750/irq.h>

#include "../timer.h"
#include "../timer-cevt.h"

void __init plat_time_init(void)
{
	jz4740_timer_init();
	jz47xx_timer_cevt_init(JZ4750_IRQ_TCU1, 0);
}

/*
 *  Copyright (C) 2009-2010, Lars-Peter Clausen <lars@metafoo.de>
 *  JZ47xx IRQ definitions
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

#ifndef __ASM_MACH_JZ47XX_IRQ_H__
#define __ASM_MACH_JZ47XX_IRQ_H__

#define MIPS_CPU_IRQ_BASE 0
#define JZ47XX_IRQ_BASE 8

/* 1st-level interrupts */
#define JZ47XX_IRQ(x)		(JZ47XX_IRQ_BASE + (x))

/* 2nd-level interrupts */
#define JZ47XX_IRQ_DMA(x)	(JZ47XX_IRQ(32) + (x))

#define JZ47XX_IRQ_INTC_GPIO(x) (JZ47XX_IRQ_GPIO(0) - (x))
#define JZ47XX_IRQ_GPIO(x)	(JZ47XX_IRQ(48) + (x))

#define NR_IRQS			182

#endif

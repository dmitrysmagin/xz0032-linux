/*
 *  Copyright (C) 2011 Peter Zotov <whitequark@whitequark.org>
 *  JZ4750 IRQ definitions
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

#ifndef __ASM_MACH_JZ4750_IRQ_H__
#define __ASM_MACH_JZ4750_IRQ_H__

#include <asm/mach-jz47xx/irq.h>

/* 1st-level interrupts */
#define JZ4750_IRQ_SFT		JZ47XX_IRQ(4)
#define JZ4750_IRQ_I2C		JZ47XX_IRQ(5)
#define JZ4750_IRQ_RTC		JZ47XX_IRQ(6)
#define JZ4750_IRQ_UART0	JZ47XX_IRQ(9)
#define JZ4750_IRQ_AIC		JZ47XX_IRQ(10)
#define JZ4750_IRQ_GPIO3	JZ47XX_IRQ(13)
#define JZ4750_IRQ_GPIO2	JZ47XX_IRQ(14)
#define JZ4750_IRQ_GPIO1	JZ47XX_IRQ(15)
#define JZ4750_IRQ_GPIO0	JZ47XX_IRQ(16)
#define JZ4750_IRQ_BCH		JZ47XX_IRQ(17)
#define JZ4750_IRQ_SADC	JZ47XX_IRQ(18)
#define JZ4750_IRQ_CIM		JZ47XX_IRQ(19)
#define JZ4750_IRQ_TCU2	JZ47XX_IRQ(21)
#define JZ4750_IRQ_TCU1	JZ47XX_IRQ(22)
#define JZ4750_IRQ_TCU0	JZ47XX_IRQ(23)
#define JZ4750_IRQ_MSC1	JZ47XX_IRQ(24)
#define JZ4750_IRQ_MSC0	JZ47XX_IRQ(25)
#define JZ4750_IRQ_SSI		JZ47XX_IRQ(26)
#define JZ4750_IRQ_UDC		JZ47XX_IRQ(27)
#define JZ4750_IRQ_DMA0	JZ47XX_IRQ(29)
#define JZ4750_IRQ_IPU		JZ47XX_IRQ(30)
#define JZ4750_IRQ_LCD		JZ47XX_IRQ(31)

/* 2nd-level interrupts */
#define JZ47XX_IRQ_DMA(x)	(JZ47XX_IRQ(32) + (x))

#define JZ4750_IRQ_GPIO(x)	(JZ47XX_IRQ(48) + (x))

#define JZ4750_IRQ_ADC_BASE	JZ47XX_IRQ(176)

#define NR_IRQS			JZ47XX_IRQ(182)

#endif

/*
 *  Copyright (C) 2009-2010, Lars-Peter Clausen <lars@metafoo.de>
 *  JZ4740 IRQ definitions
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

#ifndef __ASM_MACH_JZ4740_IRQ_H__
#define __ASM_MACH_JZ4740_IRQ_H__

#include <asm/mach-jz47xx/irq.h>

/* 1st-level interrupts */
#define JZ4740_IRQ_I2C		JZ47XX_IRQ(1)
#define JZ4740_IRQ_UHC		JZ47XX_IRQ(3)
#define JZ4740_IRQ_UART1	JZ47XX_IRQ(8)
#define JZ4740_IRQ_UART0	JZ47XX_IRQ(9)
#define JZ4740_IRQ_SADC		JZ47XX_IRQ(12)
#define JZ4740_IRQ_MSC		JZ47XX_IRQ(14)
#define JZ4740_IRQ_RTC		JZ47XX_IRQ(15)
#define JZ4740_IRQ_SSI		JZ47XX_IRQ(16)
#define JZ4740_IRQ_CIM		JZ47XX_IRQ(17)
#define JZ4740_IRQ_AIC		JZ47XX_IRQ(18)
#define JZ4740_IRQ_ETH		JZ47XX_IRQ(19)
#define JZ4740_IRQ_DMAC		JZ47XX_IRQ(20)
#define JZ4740_IRQ_TCU2		JZ47XX_IRQ(21)
#define JZ4740_IRQ_TCU1		JZ47XX_IRQ(22)
#define JZ4740_IRQ_TCU0		JZ47XX_IRQ(23)
#define JZ4740_IRQ_UDC		JZ47XX_IRQ(24)
#define JZ4740_IRQ_GPIO3	JZ47XX_IRQ(25)
#define JZ4740_IRQ_GPIO2	JZ47XX_IRQ(26)
#define JZ4740_IRQ_GPIO1	JZ47XX_IRQ(27)
#define JZ4740_IRQ_GPIO0	JZ47XX_IRQ(28)
#define JZ4740_IRQ_IPU		JZ47XX_IRQ(29)
#define JZ4740_IRQ_LCD		JZ47XX_IRQ(30)

/* 2nd-level interrupts */
#define JZ47XX_IRQ_DMA(x)	(JZ47XX_IRQ(32) + (x))

#define JZ4740_IRQ_GPIO(x)	(JZ47XX_IRQ(48) + (x))

#define JZ4740_IRQ_ADC_BASE	JZ47XX_IRQ(176)

#define NR_IRQS			182


#endif

/*
 *  Copyright (C) 2011, Lars-Peter Clausen <lars@metafoo.de>
 *  JZ4760 IRQ definitions
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

#ifndef __ASM_MACH_JZ4760_IRQ_H__
#define __ASM_MACH_JZ4760_IRQ_H__

#include <asm/mach-jz47xx/irq.h>

/* 1st-level interrupts */
#define JZ4760_IRQ_I2C1		JZ47XX_IRQ(0)
#define JZ4760_IRQ_I2C9		JZ47XX_IRQ(1)
#define JZ4760_IRQ_UART3	JZ47XX_IRQ(2)
#define JZ4760_IRQ_UART2	JZ47XX_IRQ(3)
#define JZ4760_IRQ_UART1	JZ47XX_IRQ(4)
#define JZ4760_IRQ_UART0	JZ47XX_IRQ(5)
#define JZ4760_IRQ_GPU		JZ47XX_IRQ(6)
#define JZ4760_IRQ_SSI1		JZ47XX_IRQ(7)
#define JZ4760_IRQ_SSI0		JZ47XX_IRQ(8)
#define JZ4760_IRQ_TSSI		JZ47XX_IRQ(9)
#define JZ4760_IRQ_BDMA		JZ47XX_IRQ(10)
#define JZ4760_IRQ_KBC		JZ47XX_IRQ(11)
#define JZ4760_IRQ_GPIO5	JZ47XX_IRQ(12)
#define JZ4760_IRQ_GPIO4	JZ47XX_IRQ(13)
#define JZ4760_IRQ_GPIO3	JZ47XX_IRQ(14)
#define JZ4760_IRQ_GPIO2	JZ47XX_IRQ(15)
#define JZ4760_IRQ_GPIO1	JZ47XX_IRQ(16)
#define JZ4760_IRQ_GPIO0	JZ47XX_IRQ(17)
#define JZ4760_IRQ_SADC		JZ47XX_IRQ(18)
#define JZ4760_IRQ_ETH		JZ47XX_IRQ(19)
#define JZ4760_IRQ_UDC		JZ47XX_IRQ(20)
#define JZ4760_IRQ_OTG		JZ47XX_IRQ(21)
#define JZ4760_IRQ_MDMA		JZ47XX_IRQ(22)
#define JZ4760_IRQ_DMA1		JZ47XX_IRQ(23)
#define JZ4760_IRQ_DMA0		JZ47XX_IRQ(24)
#define JZ4760_IRQ_TCU2		JZ47XX_IRQ(25)
#define JZ4760_IRQ_TCU1		JZ47XX_IRQ(26)
#define JZ4760_IRQ_TCU0		JZ47XX_IRQ(27)
#define JZ4760_IRQ_GPS		JZ47XX_IRQ(28)
#define JZ4760_IRQ_IPU		JZ47XX_IRQ(29)
#define JZ4760_IRQ_CIM		JZ47XX_IRQ(30)
#define JZ4760_IRQ_LCD		JZ47XX_IRQ(31)
#define JZ4760_IRQ_RTC		JZ47XX_IRQ(32)
#define JZ4760_IRQ_OWI		JZ47XX_IRQ(33)
#define JZ4760_IRQ_AIC		JZ47XX_IRQ(34)
#define JZ4760_IRQ_MSC2		JZ47XX_IRQ(35)
#define JZ4760_IRQ_MSC1		JZ47XX_IRQ(36)
#define JZ4760_IRQ_MSC0		JZ47XX_IRQ(37)
#define JZ4760_IRQ_SCC		JZ47XX_IRQ(38)
#define JZ4760_IRQ_BCH		JZ47XX_IRQ(39)
#define JZ4760_IRQ_PCM		JZ47XX_IRQ(40)
#define JZ4760_IRQ_HARB0	JZ47XX_IRQ(41)
#define JZ4760_IRQ_HARB2	JZ47XX_IRQ(42)

/* 2nd-level interrupts */
#define JZ47XX_IRQ_DMA(x)	(JZ47XX_IRQ(64) + (x))

#define JZ4760_IRQ_GPIO(x)	(JZ47XX_IRQ(79) + (x))

#define JZ4760_IRQ_ADC_BASE	JZ47XX_IRQ(239)

#define NR_IRQS			270

#endif

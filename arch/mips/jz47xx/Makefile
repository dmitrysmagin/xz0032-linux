#
# Makefile for the Ingenic JZ4740.
#

# Object file lists.

obj-y += prom.o irq.o reset.o setup.o \
	timer.o pwm.o serial.o clock.o

obj-$(CONFIG_JZ47XX_TIMER_CSRC) += timer-csrc.o
obj-$(CONFIG_JZ47XX_TIMER_CEVT) += timer-cevt.o
obj-$(CONFIG_JZ47XX_OST) += ost.o
obj-$(CONFIG_JZ47XX_GPIOV2) += gpiov2.o
obj-$(CONFIG_DEBUG_FS) += clock-debugfs.o

# PM support

obj-$(CONFIG_PM) += pm.o

EXTRA_CFLAGS += -Werror -Wall

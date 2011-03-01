#
# Makefile for the Ingenic JZ4740.
#

# Object file lists.

obj-y += prom.o irq.o reset.o setup.o dma.o \
	timer.o pwm.o serial.o

obj-$(CONFIG_JZ47XX_TIMER_CSRC) += time.o
obj-$(CONFIG_JZ47XX_GPIOV2) += gpiov2.o
obj-$(CONFIG_DEBUG_FS) += clock-debugfs.o

# PM support

obj-$(CONFIG_PM) += pm.o

EXTRA_CFLAGS += -Werror -Wall

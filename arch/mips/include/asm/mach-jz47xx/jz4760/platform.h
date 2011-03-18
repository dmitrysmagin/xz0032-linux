/*
 *  Copyright (C) 2009-2010, Lars-Peter Clausen <lars@metafoo.de>
 *  JZ4740 platform device definitions
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


#ifndef __JZ4740_PLATFORM_H
#define __JZ4740_PLATFORM_H

#include <linux/platform_device.h>

extern struct platform_device jz4760_rtc_device;
extern struct platform_device jz4760_nand_device;
extern struct platform_device jz4760_usb_ohci_device;
extern struct platform_device jz4760_mmc0_device;
extern struct platform_device jz4760_mmc1_device;
extern struct platform_device jz4760_mmc2_device;
extern struct platform_device jz4760_framebuffer_device;
extern struct platform_device jz4760_adc_device;
void jz4760_serial_device_register(void);

#endif

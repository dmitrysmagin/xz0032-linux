#ifndef __ASM_MACH_JZ47XX_CLKDEV_H__
#define __ASM_MACH_JZ47XX_CLKDEV_H__

#include <linux/slab.h>
#define __clk_get(clk) ({ 1; })
#define __clk_put(clk) do { } while (0)

static inline struct clk_lookup_alloc *__clkdev_alloc(size_t size)
{
	return kzalloc(size, GFP_KERNEL);
}

#endif

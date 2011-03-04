#include <linux/init.h>

extern void __init jz47xx_intc_init(unsigned int num_banks);

void __init arch_init_irq(void)
{
	jz47xx_intc_init(2);
}

#include <linux/init.h>
#include <asm/mach-jz47xx/irq.h>

void __init arch_init_irq(void)
{
	jz47xx_intc_init(1);
}

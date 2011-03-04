
#include <asm/time.h>
#include <asm/mach-jz47xx/jz4760/irq.h>

#include "../timer.h"
#include "../ost.h"

void __init plat_time_init(void)
{
	jz4740_timer_init();
	jz47xx_ost_init(JZ4760_IRQ_TCU0);
}

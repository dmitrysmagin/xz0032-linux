

#include <asm/time.h>
#include <asm/mach-jz47xx/jz4740/irq.h>

#include "../timer.h"
#include "../timer-csrc.h"
#include "../timer-cevt.h"

void __init plat_time_init(void)
{
	jz4740_timer_init();
	jz47xx_timer_csrc_init(1);
	jz47xx_timer_cevt_init(JZ4740_IRQ_TCU0, 0);
}

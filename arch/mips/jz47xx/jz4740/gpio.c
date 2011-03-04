#include <linux/init.h>

#include <asm/mach-jz47xx/jz4740/irq.h>
#include <asm/mach-jz47xx/gpio.h>


static struct jz_gpio_chip jz4740_gpio_chips[] = {
	JZ47XX_GPIO_CHIP("A", 32, JZ4740_IRQ_GPIO0),
	JZ47XX_GPIO_CHIP("B", 32, JZ4740_IRQ_GPIO1),
	JZ47XX_GPIO_CHIP("C", 31, JZ4740_IRQ_GPIO2),
	JZ47XX_GPIO_CHIP("D", 32, JZ4740_IRQ_GPIO3),
};

static int __init jz4740_gpio_init(void)
{
	jz47xx_gpio_init(jz4740_gpio_chips, ARRAY_SIZE(jz4740_gpio_chips),
			JZ4740_IRQ_GPIO(0));
	return 0;
}
arch_initcall(jz4740_gpio_init);



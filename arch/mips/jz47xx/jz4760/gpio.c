#include <linux/init.h>

#include <asm/mach-jz47xx/jz4760/irq.h>
#include <asm/mach-jz47xx/gpio.h>


static struct jz_gpio_chip jz4760_gpio_chips[] = {
	JZ47XX_GPIO_CHIP("A", 32, JZ4760_IRQ_GPIO0),
	JZ47XX_GPIO_CHIP("B", 32, JZ4760_IRQ_GPIO1),
	JZ47XX_GPIO_CHIP("C", 32, JZ4760_IRQ_GPIO2),
	JZ47XX_GPIO_CHIP("D", 32, JZ4760_IRQ_GPIO3),
	JZ47XX_GPIO_CHIP("E", 32, JZ4760_IRQ_GPIO4),
	JZ47XX_GPIO_CHIP("F", 12, JZ4760_IRQ_GPIO5),
};

static int __init jz4760_gpio_init(void)
{
	jz47xx_gpio_init(jz4760_gpio_chips, ARRAY_SIZE(jz4760_gpio_chips),
			JZ4760_IRQ_GPIO(0));
	return 0;
}
arch_initcall(jz4760_gpio_init);



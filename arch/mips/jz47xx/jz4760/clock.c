/*
 *  Copyright (C) 2010, Lars-Peter Clausen <lars@metafoo.de>
 *  JZ4740 SoC clock support
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

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/clk.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/err.h>

#include <asm/mach-jz47xx/clock.h>
#include <asm/mach-jz47xx/base.h>

#include "../clock.h"

#define JZ_REG_CLOCK_CTRL	0x00
#define JZ_REG_CLOCK_LOW_POWER	0x04
#define JZ_REG_CLOCK_PLL	0x10
#define JZ_REG_CLOCK_GATE0	0x20
#define JZ_REG_CLOCK_GATE1	0x28
#define JZ_REG_CLOCK_SLEEP_CTRL	0x24
#define JZ_REG_CLOCK_I2S	0x60
#define JZ_REG_CLOCK_LCD	0x64
#define JZ_REG_CLOCK_MSC	0x68
#define JZ_REG_CLOCK_UHC	0x6C
#define JZ_REG_CLOCK_SSI	0x74

#define JZ_CLOCK_CTRL_CHANGE_ENABLE	BIT(22)
#define JZ_CLOCK_CTRL_PLL_HALF		BIT(21)
#define JZ_CLOCK_CTRL_LDIV_MASK		0x001f0000
#define JZ_CLOCK_CTRL_SDIV_OFFSET	24
#define JZ_CLOCK_CTRL_H2DIV_OFFSET	16
#define JZ_CLOCK_CTRL_MDIV_OFFSET	12
#define JZ_CLOCK_CTRL_PDIV_OFFSET	 8
#define JZ_CLOCK_CTRL_HDIV_OFFSET	 4
#define JZ_CLOCK_CTRL_CDIV_OFFSET	 0

#define JZ_CLOCK_GATE0_NMEC	BIT(0)
#define JZ_CLOCK_GATE0_BCH	BIT(1)
#define JZ_CLOCK_GATE0_OTG	BIT(2)
#define JZ_CLOCK_GATE0_MSC0	BIT(3)
#define JZ_CLOCK_GATE0_SSI0	BIT(4)
#define JZ_CLOCK_GATE0_I2C0	BIT(5)
#define JZ_CLOCK_GATE0_I2C1	BIT(6)
#define JZ_CLOCK_GATE0_SCC	BIT(7)
#define JZ_CLOCK_GATE0_AIC	BIT(8)
#define JZ_CLOCK_GATE0_TSSI	BIT(9)
#define JZ_CLOCK_GATE0_OWI	BIT(10)
#define JZ_CLOCK_GATE0_MSC1	BIT(11)
#define JZ_CLOCK_GATE0_MSC2	BIT(12)
#define JZ_CLOCK_GATE0_KBC	BIT(13)
#define JZ_CLOCK_GATE0_SADC	BIT(14)
#define JZ_CLOCK_GATE0_UART0	BIT(15)
#define JZ_CLOCK_GATE0_UART1	BIT(16)
#define JZ_CLOCK_GATE0_UART2	BIT(17)
#define JZ_CLOCK_GATE0_UART3	BIT(18)
#define JZ_CLOCK_GATE0_SSI1	BIT(19)
#define JZ_CLOCK_GATE0_SSI2	BIT(20)
#define JZ_CLOCK_GATE0_DMAC	BIT(21)
#define JZ_CLOCK_GATE0_GPS	BIT(22)
#define JZ_CLOCK_GATE0_MAC	BIT(23)
#define JZ_CLOCK_GATE0_UHC	BIT(24)
#define JZ_CLOCK_GATE0_MDMA	BIT(25)
#define JZ_CLOCK_GATE0_CIM	BIT(26)
#define JZ_CLOCK_GATE0_TVE	BIT(27)
#define JZ_CLOCK_GATE0_LCD	BIT(28)
#define JZ_CLOCK_GATE0_IPU	BIT(29)
#define JZ_CLOCK_GATE0_DDR	BIT(30)
#define JZ_CLOCK_GATE0_EMC	BIT(31)

#define JZ_CLOCK_GATE1_GPU	BIT(9)
#define JZ_CLOCK_GATE1_PCM	BIT(8)
#define JZ_CLOCK_GATE1_AHB1	BIT(7)
#define JZ_CLOCK_GATE1_CABAC	BIT(6)
#define JZ_CLOCK_GATE1_SRAM	BIT(5)
#define JZ_CLOCK_GATE1_DCT	BIT(4)
#define JZ_CLOCK_GATE1_ME	BIT(3)
#define JZ_CLOCK_GATE1_DBLK	BIT(2)
#define JZ_CLOCK_GATE1_MC	BIT(1)
#define JZ_CLOCK_GATE1_BDMA	BIT(0)


#define JZ_CLOCK_I2S_SRC_PLL		BIT(31)
#define JZ_CLOCK_I2S_SRC_PLL1		BIT(30)
#define JZ_CLOCK_I2S_DIV_MASK		0x01ff

#define JZ_CLOCK_UDC_SRC_PLL		BIT(31)
#define JZ_CLOCK_UDC_SRC_PLL1		BIT(30)
#define JZ_CLOCK_UDC_DIV_MASK		0x003f

#define JZ_CLOCK_LCD_SRC_PLL1		BIT(29)
#define JZ_CLOCK_LCD_DIV_MASK		0x07ff

#define JZ_CLOCK_MSC_SRC_PLL0		BIT(31)
#define JZ_CLOCK_MSC_DIV_MASK		0x001f

#define JZ_CLOCK_UHC_SRC_PLL1		BIT(31)
#define JZ_CLOCK_UHC_DIV_MASK		0x000f

#define JZ_CLOCK_SSI_SRC_PLL0		BIT(31)
#define JZ_CLOCK_SSI_DIV_MASK		0x001f

#define JZ_CLOCK_PLL_M_MASK		0x003f
#define JZ_CLOCK_PLL_N_MASK		0x001f
#define JZ_CLOCK_PLL_OD_MASK		0x0003
#define JZ_CLOCK_PLL_STABLE		BIT(10)
#define JZ_CLOCK_PLL_BYPASS		BIT(9)
#define JZ_CLOCK_PLL_ENABLED		BIT(8)
#define JZ_CLOCK_PLL_STABLIZE_MASK	0x000f
#define JZ_CLOCK_PLL_M_OFFSET		23
#define JZ_CLOCK_PLL_N_OFFSET		18
#define JZ_CLOCK_PLL_OD_OFFSET		16

#define JZ_CLOCK_LOW_POWER_MODE_DOZE BIT(2)
#define JZ_CLOCK_LOW_POWER_MODE_SLEEP BIT(0)

#define JZ_CLOCK_SLEEP_CTRL_SUSPEND_UHC BIT(5)
#define JZ_CLOCK_SLEEP_CTRL_ENABLE_UDC BIT(7)

static void __iomem *jz_clock_base;
static spinlock_t jz_clock_lock;

static uint32_t jz_clk_reg_read(int reg)
{
	return readl(jz_clock_base + reg);
}

static void jz_clk_reg_write_mask(int reg, uint32_t val, uint32_t mask)
{
	uint32_t val2;

	spin_lock(&jz_clock_lock);
	val2 = readl(jz_clock_base + reg);
	val2 &= ~mask;
	val2 |= val;
	writel(val2, jz_clock_base + reg);
	spin_unlock(&jz_clock_lock);
}

static void jz_clk_reg_set_bits(int reg, uint32_t mask)
{
	uint32_t val;

	spin_lock(&jz_clock_lock);
	val = readl(jz_clock_base + reg);
	val |= mask;
	writel(val, jz_clock_base + reg);
	spin_unlock(&jz_clock_lock);
}

static void jz_clk_reg_clear_bits(int reg, uint32_t mask)
{
	uint32_t val;

	spin_lock(&jz_clock_lock);
	val = readl(jz_clock_base + reg);
	val &= ~mask;
	writel(val, jz_clock_base + reg);
	spin_unlock(&jz_clock_lock);
}

static int jz_clk_enable_gating(struct clk *clk, unsigned int reg)
{
	if (clk->gate_bit == JZ47XX_CLK_NOT_GATED)
		return -EINVAL;

	jz_clk_reg_clear_bits(reg, clk->gate_bit);
	return 0;
}

static int jz_clk_disable_gating(struct clk *clk, unsigned int reg)
{
	if (clk->gate_bit == JZ47XX_CLK_NOT_GATED)
		return -EINVAL;

	jz_clk_reg_set_bits(reg, clk->gate_bit);
	return 0;
}

static int jz_clk_is_enabled_gating(struct clk *clk, unsigned int reg)
{
	if (clk->gate_bit == JZ47XX_CLK_NOT_GATED)
		return 1;

	return !(jz_clk_reg_read(reg) & clk->gate_bit);
}

static int jz_clk_enable_gating0(struct clk *clk)
{
	return jz_clk_enable_gating(clk, JZ_REG_CLOCK_GATE0);
}

static int jz_clk_disable_gating0(struct clk *clk)
{
	return jz_clk_disable_gating(clk, JZ_REG_CLOCK_GATE0);
}

static int jz_clk_is_enabled_gating0(struct clk *clk)
{
	return jz_clk_is_enabled_gating(clk, JZ_REG_CLOCK_GATE0);
}

static int jz_clk_enable_gating1(struct clk *clk)
{
	return jz_clk_enable_gating(clk, JZ_REG_CLOCK_GATE1);
}

static int jz_clk_disable_gating1(struct clk *clk)
{
	return jz_clk_disable_gating(clk, JZ_REG_CLOCK_GATE1);
}

static int jz_clk_is_enabled_gating1(struct clk *clk)
{
	return jz_clk_is_enabled_gating(clk, JZ_REG_CLOCK_GATE1);
}

static unsigned long jz_clk_static_get_rate(struct clk *clk)
{
	return ((struct static_clk *)clk)->rate;
}

static unsigned long jz_clk_pll_get_rate(struct clk *clk)
{
	unsigned long rate;
	uint32_t val;
	int m;
	int n;
	int od;

	val = jz_clk_reg_read(JZ_REG_CLOCK_PLL);

	if (val & JZ_CLOCK_PLL_BYPASS)
		return clk_get_rate(clk->parent);

	m = ((val >> 23) & 0xfe);
	n = ((val >> 18) & 0xf);
	od = (val >> 16) & 0x3;

	rate = ((clk_get_rate(clk->parent) / n) * m) / (1 << od);

	printk("pll rate: %d %d %d %lu\n", m, n, od, rate);

	return rate;
}

static unsigned long jz_clk_pll_half_get_rate(struct clk *clk)
{
	uint32_t reg;

	reg = jz_clk_reg_read(JZ_REG_CLOCK_CTRL);
	if (reg & JZ_CLOCK_CTRL_PLL_HALF)
		return jz_clk_pll_get_rate(clk->parent);
	return jz_clk_pll_get_rate(clk->parent) >> 1;
}

static const int jz_clk_main_divs[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

static unsigned long jz_clk_main_round_rate(struct clk *clk, unsigned long rate)
{
	unsigned long parent_rate = jz_clk_pll_get_rate(clk->parent);
	int div;

	div = parent_rate / rate;
	if (div > 32)
		return parent_rate / 32;
	else if (div < 1)
		return parent_rate;

	div &= (0x3 << (ffs(div) - 1));

	return parent_rate / div;
}

static unsigned long jz_clk_main_get_rate(struct clk *clk)
{
	struct main_clk *mclk = (struct main_clk *)clk;
	uint32_t div;

	div = jz_clk_reg_read(JZ_REG_CLOCK_CTRL);

	div >>= mclk->div_offset;
	div &= 0xf;

	if (div >= ARRAY_SIZE(jz_clk_main_divs))
		div = ARRAY_SIZE(jz_clk_main_divs) - 1;

	return jz_clk_pll_get_rate(clk->parent) / jz_clk_main_divs[div];
}

static int jz_clk_main_set_rate(struct clk *clk, unsigned long rate)
{
	struct main_clk *mclk = (struct main_clk *)clk;
	int i;
	int div;
	unsigned long parent_rate = jz_clk_pll_get_rate(clk->parent);

	rate = jz_clk_main_round_rate(clk, rate);

	div = parent_rate / rate;

	i = (ffs(div) - 1) << 1;
	if (i > 0 && !(div & BIT(i-1)))
		i -= 1;

	jz_clk_reg_write_mask(JZ_REG_CLOCK_CTRL, i << mclk->div_offset,
				0xf << mclk->div_offset);

	return 0;
}

static struct clk_ops jz_clk_static_ops = {
	.get_rate = jz_clk_static_get_rate,
	.enable = jz_clk_enable_gating0,
	.disable = jz_clk_disable_gating0,
	.is_enabled = jz_clk_is_enabled_gating0,
};

static struct static_clk jz_clk_ext = {
	.clk = {
		.name = "ext",
		.gate_bit = JZ47XX_CLK_NOT_GATED,
		.ops = &jz_clk_static_ops,
	},
};

static struct clk_ops jz_clk_pll_ops = {
	.get_rate = jz_clk_pll_get_rate,
};

static struct clk jz_clk_pll = {
	.name = "pll",
	.parent = &jz_clk_ext.clk,
	.ops = &jz_clk_pll_ops,
};

static struct clk_ops jz_clk_pll_half_ops = {
	.get_rate = jz_clk_pll_half_get_rate,
};

static struct clk jz_clk_pll_half = {
	.name = "pll half",
	.parent = &jz_clk_pll,
	.ops = &jz_clk_pll_half_ops,
};

static const struct clk_ops jz_clk_main_ops = {
	.get_rate = jz_clk_main_get_rate,
	.set_rate = jz_clk_main_set_rate,
	.round_rate = jz_clk_main_round_rate,
};

static struct main_clk jz_clk_cpu = {
	.clk = {
		.name = "cclk",
		.parent = &jz_clk_pll,
		.ops = &jz_clk_main_ops,
	},
	.div_offset = JZ_CLOCK_CTRL_CDIV_OFFSET,
};

static struct main_clk jz_clk_memory = {
	.clk = {
		.name = "mclk",
		.parent = &jz_clk_pll,
		.ops = &jz_clk_main_ops,
	},
	.div_offset = JZ_CLOCK_CTRL_MDIV_OFFSET,
};

static struct main_clk jz_clk_high_speed_peripheral = {
	.clk = {
		.name = "hclk",
		.parent = &jz_clk_pll,
		.ops = &jz_clk_main_ops,
	},
	.div_offset = JZ_CLOCK_CTRL_HDIV_OFFSET,
};

static struct main_clk jz_clk_low_speed_peripheral = {
	.clk = {
		.name = "pclk",
		.parent = &jz_clk_pll,
		.ops = &jz_clk_main_ops,
	},
	.div_offset = JZ_CLOCK_CTRL_PDIV_OFFSET,
};

static struct main_clk jz_clk_high_speed_peripheral2 = {
	.clk = {
		.name = "h2clk",
		.parent = &jz_clk_pll,
		.ops = &jz_clk_main_ops,
	},
	.div_offset = JZ_CLOCK_CTRL_H2DIV_OFFSET,
};

static struct main_clk jz_clk_static_peripheral = {
	.clk = {
		.name = "sclk",
		.parent = &jz_clk_pll,
		.ops = &jz_clk_main_ops,
	},
	.div_offset = JZ_CLOCK_CTRL_SDIV_OFFSET,
};

static unsigned long jz_clk_divided_get_rate(struct clk *clk)
{
	struct divided_clk *dclk = (struct divided_clk *)clk;
	int div;

	if (clk->parent == &jz_clk_ext.clk)
		return clk_get_rate(clk->parent);

	div = (jz_clk_reg_read(dclk->reg) & dclk->mask) + 1;

	return clk_get_rate(clk->parent) / div;
}

static int jz_clk_divided_set_rate(struct clk *clk, unsigned long rate)
{
	struct divided_clk *dclk = (struct divided_clk *)clk;
	int div;

	if (clk->parent == &jz_clk_ext.clk)
		return -EINVAL;

	div = clk_get_rate(clk->parent) / rate - 1;

	if (div < 0)
		div = 0;
	else if (div > dclk->mask)
		div = dclk->mask;

	jz_clk_reg_write_mask(dclk->reg, div, dclk->mask);

	printk("%s rate: %lu %lu\n", clk->name, rate, clk_get_rate(clk));

	return 0;
}

static const struct clk_ops jz_clk_divided_ops = {
	.set_rate = jz_clk_divided_set_rate,
	.get_rate = jz_clk_divided_get_rate,
	.enable = jz_clk_enable_gating0,
	.disable = jz_clk_disable_gating0,
	.is_enabled = jz_clk_is_enabled_gating0,
};

static struct divided_clk jz_clk_i2s = {
	.clk = {
		.name = "i2s",
		.parent = &jz_clk_ext.clk,
		.gate_bit = JZ47XX_CLK_NOT_GATED,
		.ops = &jz_clk_divided_ops,
	},
	.reg = JZ_REG_CLOCK_I2S,
	.mask = JZ_CLOCK_I2S_DIV_MASK,
};

static struct divided_clk jz_clk_lcd_pclk = {
	.clk = {
		.name = "lcd_pclk",
		.parent = &jz_clk_pll_half,
		.gate_bit = JZ47XX_CLK_NOT_GATED,
		.ops = &jz_clk_divided_ops,
	},
	.reg = JZ_REG_CLOCK_LCD,
	.mask = JZ_CLOCK_LCD_DIV_MASK,
};

static struct divided_clk jz_clk_msc = {
	.clk = {
		.name = "msc",
		.parent = &jz_clk_ext.clk,
		.gate_bit = JZ47XX_CLK_NOT_GATED,
		.ops = &jz_clk_divided_ops,
	},
	.reg = JZ_REG_CLOCK_MSC,
	.mask = JZ_CLOCK_MSC_DIV_MASK,
};

static struct divided_clk jz_clk_uhc = {
	.clk = {
		.name = "uhc",
		.parent = &jz_clk_pll_half,
		.gate_bit = JZ_CLOCK_GATE0_UHC,
		.ops = &jz_clk_divided_ops,
	},
	.reg = JZ_REG_CLOCK_UHC,
	.mask = JZ_CLOCK_UHC_DIV_MASK,
};

static struct divided_clk jz_clk_ssi = {
	.clk = {
		.name = "ssi",
		.parent = &jz_clk_pll_half,
		.gate_bit = JZ47XX_CLK_NOT_GATED,
		.ops = &jz_clk_divided_ops,
	},
	.reg = JZ_REG_CLOCK_SSI,
	.mask = JZ_CLOCK_SSI_DIV_MASK,
};

static const struct clk_ops jz_clk_simple0_ops = {
	.enable		= jz_clk_enable_gating0,
	.disable	= jz_clk_disable_gating0,
	.is_enabled	= jz_clk_is_enabled_gating0,
};

static const struct clk_ops jz_clk_simple1_ops = {
	.enable		= jz_clk_enable_gating1,
	.disable	= jz_clk_disable_gating1,
	.is_enabled	= jz_clk_is_enabled_gating1,
};

#define DEFINE_GATED_CLOCK0(_name, _parent, _gate_bit) \
	struct clk _name = { \
		.parent = parent, \
		.gate_bit = _gate_bit, \
		.ops = jz_clk_simple0_ops, \
	};


static struct clk jz_clk_bch = {
	.name = "bch",
	.parent = &jz_clk_ext.clk,
	.gate_bit = JZ_CLOCK_GATE0_BCH,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_emc = {
	.name = "emc",
	.parent = &jz_clk_ext.clk,
	.gate_bit = JZ_CLOCK_GATE0_EMC,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_ssi0 = {
	.name = "ssi0",
	.parent = &jz_clk_ssi.clk,
	.gate_bit = JZ_CLOCK_GATE0_SSI0,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_ssi1 = {
	.name = "ssi1",
	.parent = &jz_clk_ssi.clk,
	.gate_bit = JZ_CLOCK_GATE0_SSI1,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_ssi2 = {
	.name = "ssi2",
	.parent = &jz_clk_ssi.clk,
	.gate_bit = JZ_CLOCK_GATE0_SSI2,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_msc0 = {
	.name = "msc0",
	.parent = &jz_clk_msc.clk,
	.gate_bit = JZ_CLOCK_GATE0_MSC0,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_msc1 = {
	.name = "msc1",
	.parent = &jz_clk_msc.clk,
	.gate_bit = JZ_CLOCK_GATE0_MSC1,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_msc2 = {
	.name = "msc2",
	.parent = &jz_clk_msc.clk,
	.gate_bit = JZ_CLOCK_GATE0_MSC2,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_uart0 = {
	.name = "uart0",
	.parent = &jz_clk_ext.clk,
	.gate_bit = JZ_CLOCK_GATE0_UART0,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_uart1 = {
	.name = "uart1",
	.parent = &jz_clk_ext.clk,
	.gate_bit = JZ_CLOCK_GATE0_UART1,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_uart2 = {
	.name = "uart2",
	.parent = &jz_clk_ext.clk,
	.gate_bit = JZ_CLOCK_GATE0_UART2,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_uart3 = {
	.name = "uart3",
	.parent = &jz_clk_ext.clk,
	.gate_bit = JZ_CLOCK_GATE0_UART3,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_dma = {
	.name = "dma",
	.parent = &jz_clk_high_speed_peripheral.clk,
	.gate_bit = JZ_CLOCK_GATE0_DMAC,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_ipu = {
	.name = "ipu",
	.parent = &jz_clk_high_speed_peripheral.clk,
	.gate_bit = JZ_CLOCK_GATE0_IPU,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_adc = {
	.name = "adc",
	.parent = &jz_clk_ext.clk,
	.gate_bit = JZ_CLOCK_GATE0_SADC,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_i2c0 = {
	.name = "i2c0",
	.parent = &jz_clk_ext.clk,
	.gate_bit = JZ_CLOCK_GATE0_I2C0,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_i2c1 = {
	.name = "i2c1",
	.parent = &jz_clk_ext.clk,
	.gate_bit = JZ_CLOCK_GATE0_I2C1,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_aic = {
	.name = "aic",
	.parent = &jz_clk_ext.clk,
	.gate_bit = JZ_CLOCK_GATE0_AIC,
	.ops = &jz_clk_simple0_ops,
};

static struct clk jz_clk_ld = {
	.name = "lcd",
	.parent = &jz_clk_ext.clk,
	.gate_bit = JZ_CLOCK_GATE0_AIC,
	.ops = &jz_clk_simple0_ops,
};

static struct static_clk jz_clk_rtc = {
	.clk = {
		.name = "rtc",
		.gate_bit = JZ47XX_CLK_NOT_GATED,
		.ops = &jz_clk_static_ops,
	},
	.rate = 32768,
};

#define INIT_CLOCK(_dev, _con, _clk) \
	{ .dev_id = _dev, .con_id = _con, .clk = _clk, }

static struct clk_lookup jz4760_clk_table[] = {
	INIT_CLOCK(NULL, "ext", &jz_clk_ext.clk),
	INIT_CLOCK(NULL, "pll", &jz_clk_pll),
	INIT_CLOCK(NULL, "pll half", &jz_clk_pll_half),
	INIT_CLOCK(NULL, "cclk", &jz_clk_cpu.clk),
	INIT_CLOCK(NULL, "mclk", &jz_clk_memory.clk),
	INIT_CLOCK(NULL, "hclk", &jz_clk_high_speed_peripheral.clk),
	INIT_CLOCK(NULL, "pclk", &jz_clk_low_speed_peripheral.clk),
	INIT_CLOCK(NULL, "h2clk", &jz_clk_high_speed_peripheral2.clk),
	INIT_CLOCK(NULL, "sclk", &jz_clk_static_peripheral.clk),

	INIT_CLOCK("jz4750-nand", "bch", &jz_clk_bch),
	INIT_CLOCK("jz4750-nand", "emc", &jz_clk_emc),

	INIT_CLOCK("jz4740-rtc", "rtc", &jz_clk_rtc.clk),
	INIT_CLOCK("jz4740-fb", "lcd", &jz_clk_ld),
	INIT_CLOCK("jz4740-fb", "lcd_pclk", &jz_clk_lcd_pclk.clk),
	INIT_CLOCK(NULL, "ssi", &jz_clk_ssi.clk),
	INIT_CLOCK(NULL, "ssi0", &jz_clk_ssi0),
	INIT_CLOCK(NULL, "ssi1", &jz_clk_ssi1),
	INIT_CLOCK(NULL, "ssi2", &jz_clk_ssi2),

	INIT_CLOCK(NULL, "i2c0", &jz_clk_i2c0),
	INIT_CLOCK(NULL, "i2c1", &jz_clk_i2c1),

	INIT_CLOCK("jz4740-i2s", "i2s", &jz_clk_i2s.clk),
	INIT_CLOCK("jz4740-i2s", "aic", &jz_clk_aic),

	INIT_CLOCK("jz4740-adc", "adc", &jz_clk_adc),
/*	INIT_CLOCK("jz-udc", "udc", &jz_clk_udc),
	INIT_CLOCK("jz-udc", "udc-phy", &jz_clk_udc_phy),*/
	INIT_CLOCK(NULL, "msc", &jz_clk_msc.clk),
	INIT_CLOCK("jz4740-mmc.0", "mmc", &jz_clk_msc0),
	INIT_CLOCK("jz4740-mmc.1", "mmc", &jz_clk_msc1),
	INIT_CLOCK("jz4740-mmc.2", "mmc", &jz_clk_msc2),
	INIT_CLOCK("jz4740-ohci", "uhc", &jz_clk_uhc.clk),
	INIT_CLOCK("jz4740-ohci", "uhc-ohy", &jz_clk_uhc.clk),

	INIT_CLOCK(NULL, "uart0", &jz_clk_uart0),
	INIT_CLOCK(NULL, "uart1", &jz_clk_uart1),
	INIT_CLOCK(NULL, "uart1", &jz_clk_uart2),
	INIT_CLOCK(NULL, "uart1", &jz_clk_uart3),
	INIT_CLOCK(NULL, "dma", &jz_clk_dma),
	INIT_CLOCK(NULL, "ipu", &jz_clk_ipu),
};


static void __init clk_register_clks(void)
{
	jz47xx_clock_add_table(jz4760_clk_table, ARRAY_SIZE(jz4760_clk_table));
}

void jz47xx_clock_set_wait_mode(enum jz47xx_wait_mode mode)
{
	switch (mode) {
	case JZ47XX_WAIT_MODE_IDLE:
		jz_clk_reg_clear_bits(JZ_REG_CLOCK_LOW_POWER, JZ_CLOCK_LOW_POWER_MODE_SLEEP);
		break;
	case JZ47XX_WAIT_MODE_SLEEP:
		jz_clk_reg_set_bits(JZ_REG_CLOCK_LOW_POWER, JZ_CLOCK_LOW_POWER_MODE_SLEEP);
		break;
	}
}

void jz47xx_clock_suspend(void)
{
	jz_clk_reg_clear_bits(JZ_REG_CLOCK_PLL, JZ_CLOCK_PLL_ENABLED);
}

void jz47xx_clock_resume(void)
{
	uint32_t pll;

	jz_clk_reg_set_bits(JZ_REG_CLOCK_PLL, JZ_CLOCK_PLL_ENABLED);

	do {
		pll = jz_clk_reg_read(JZ_REG_CLOCK_PLL);
	} while (!(pll & JZ_CLOCK_PLL_STABLE));
}

static int __init jz4760_clock_init(void)
{
	uint32_t val;

	jz_clock_base = ioremap(JZ47XX_CPM_BASE_ADDR, 0x100);
	if (!jz_clock_base)
		return -EBUSY;

	spin_lock_init(&jz_clock_lock);

	jz_clk_ext.rate = jz47xx_clock_bdata.ext_rate;
	jz_clk_rtc.rate = jz47xx_clock_bdata.rtc_rate;

	val = jz_clk_reg_read(JZ_REG_CLOCK_I2S);
	if (val & JZ_CLOCK_I2S_SRC_PLL) {
		if (val & JZ_CLOCK_I2S_SRC_PLL1)
			jz_clk_i2s.clk.parent = &jz_clk_pll_half;
		else
			jz_clk_i2s.clk.parent = &jz_clk_pll_half;
	}

	val = jz_clk_reg_read(JZ_REG_CLOCK_SSI);
	if (val & JZ_CLOCK_SSI_SRC_PLL0)
		jz_clk_ssi.clk.parent = &jz_clk_pll_half;

	val = jz_clk_reg_read(JZ_REG_CLOCK_MSC);
	if (!(val & JZ_CLOCK_MSC_SRC_PLL0))
		jz_clk_msc.clk.parent = &jz_clk_pll_half;

	clk_set_rate(&jz_clk_msc.clk, 12000000);

	jz4740_clock_debugfs_init();

	jz_clk_reg_clear_bits(JZ_REG_CLOCK_SLEEP_CTRL,
			JZ_CLOCK_SLEEP_CTRL_SUSPEND_UHC);


	clk_register_clks();
	printk("register clks\n");


	return 0;
}
arch_initcall(jz4760_clock_init);

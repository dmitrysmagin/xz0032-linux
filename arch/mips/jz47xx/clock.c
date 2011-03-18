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

#include "clock.h"

int clk_enable(struct clk *clk)
{
	if (!clk->ops->enable)
		return -EINVAL;

	return clk->ops->enable(clk);
}
EXPORT_SYMBOL_GPL(clk_enable);

void clk_disable(struct clk *clk)
{
	if (clk->ops->disable)
		clk->ops->disable(clk);
}
EXPORT_SYMBOL_GPL(clk_disable);

int clk_is_enabled(struct clk *clk)
{
	if (clk->ops->is_enabled)
		return clk->ops->is_enabled(clk);

	return 1;
}

unsigned long clk_get_rate(struct clk *clk)
{
	if (clk->ops->get_rate)
		return clk->ops->get_rate(clk);
	if (clk->parent)
		return clk_get_rate(clk->parent);

	return -EINVAL;
}
EXPORT_SYMBOL_GPL(clk_get_rate);

int clk_set_rate(struct clk *clk, unsigned long rate)
{
	if (!clk->ops->set_rate)
		return -EINVAL;

	return clk->ops->set_rate(clk, rate);
}
EXPORT_SYMBOL_GPL(clk_set_rate);

long clk_round_rate(struct clk *clk, unsigned long rate)
{
	if (!clk->ops->round_rate)
		return -EINVAL;

	return clk->ops->round_rate(clk, rate);
}
EXPORT_SYMBOL_GPL(clk_round_rate);

int clk_set_parent(struct clk *clk, struct clk *parent)
{
	int ret;
	int enabled;

	if (!clk->ops->set_parent)
		return -EINVAL;

	enabled = clk_is_enabled(clk);
	if (enabled)
		clk_disable(clk);
	ret = clk->ops->set_parent(clk, parent);
	if (enabled)
		clk_enable(clk);

	jz4740_clock_debugfs_update_parent(clk);

	return ret;
}
EXPORT_SYMBOL_GPL(clk_set_parent);

void __init jz47xx_clock_add_table(struct clk_lookup *table, size_t num)
{
	size_t i;

	clkdev_add_table(table, num);

	for (i = 0; i < num; ++i)
		jz4740_clock_debugfs_add_clk(table[i].clk);
}

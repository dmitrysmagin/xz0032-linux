/*
 * Copyright (c) 2011, Peter Zotov <whitequark@whitequark.org>
 *
 * Based on QI LB60 driver:
 * Copyright (C) 2009, Lars-Peter Clausen <lars@metafoo.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <linux/gpio.h>

static const struct snd_soc_dapm_widget xz0032_widgets[] = {
	SND_SOC_DAPM_SPK("Speaker", NULL),
};

static const struct snd_soc_dapm_route xz0032_routes[] = {
	{"Speaker", NULL, "LOUT"},
};

#define XZ0032_DAIFMT (SND_SOC_DAIFMT_I2S | \
			SND_SOC_DAIFMT_NB_NF | \
			SND_SOC_DAIFMT_CBM_CFM)

static int xz0032_codec_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	struct snd_soc_dapm_context *dapm = &codec->dapm;
	int ret;

	ret = snd_soc_dai_set_fmt(cpu_dai, XZ0032_DAIFMT);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set cpu dai format: %d\n", ret);
		return ret;
	}

	snd_soc_dapm_new_controls(dapm, xz0032_widgets, ARRAY_SIZE(xz0032_widgets));
	snd_soc_dapm_add_routes(dapm, xz0032_routes, ARRAY_SIZE(xz0032_routes));
	snd_soc_dapm_sync(dapm);

	return 0;
}

static struct snd_soc_dai_link xz0032_dai = {
	.name = "jz4740",
	.stream_name = "jz4740",
	.cpu_dai_name = "jz4740-i2s",
	.platform_name = "jz4740-pcm-audio",
	.codec_dai_name = "jz4750-hifi",
	.codec_name = "jz4750-codec",
	.init = xz0032_codec_init,
};

static struct snd_soc_card xz0032 = {
	.name = "XZ0032",
	.dai_link = &xz0032_dai,
	.num_links = 1,
};

static struct platform_device *xz0032_snd_device;

static int __init xz0032_init(void)
{
	int ret;

	xz0032_snd_device = platform_device_alloc("soc-audio", -1);

	if (!xz0032_snd_device)
		return -ENOMEM;

	platform_set_drvdata(xz0032_snd_device, &xz0032);

	ret = platform_device_add(xz0032_snd_device);
	if (ret) {
		pr_err("xz0032 snd: Failed to add snd soc device: %d\n", ret);
		goto err_unset_pdata;
	}

	 return 0;

err_unset_pdata:
	platform_set_drvdata(xz0032_snd_device, NULL);
	platform_device_put(xz0032_snd_device);

	return ret;
}
module_init(xz0032_init);

static void __exit xz0032_exit(void)
{
	platform_device_unregister(xz0032_snd_device);
}
module_exit(xz0032_exit);

MODULE_AUTHOR("Peter Zotov <whitequark@whitequark.org>");
MODULE_DESCRIPTION("ALSA SoC XZ0032 Audio support");
MODULE_LICENSE("GPL v2");

/*
 * Copyright (C) 2011 Peter Zotov <whitequark@whitequark.org>
 *
 * Based on jz4740 CODEC driver:
 *   Copyright (C) 2009-2010, Lars-Peter Clausen <lars@metafoo.de>
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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include <linux/delay.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/initval.h>
#include <sound/soc.h>

#define JZ4750_CODEC_REG_RGADW		0x0
#define JZ4750_CODEC_REG_RGDATA		0x4

#define JZ4750_CODEC_RGWR		BIT(16)

#define JZ4750_CODEC_RGADDR_MASK	0x7f00
#define JZ4750_CODEC_RGADDR_OFFSET	8

#define JZ4750_CODEC_RGDIN_MASK		0xff
#define JZ4750_CODEC_RGDOUT_MASK	0xff

#define JZ4750_CODEC_IREG_AICR		0x00
#define JZ4750_CODEC_IREG_CR1		0x01
#define JZ4750_CODEC_IREG_CR2		0x02
#define JZ4750_CODEC_IREG_CCR1		0x03
#define JZ4750_CODEC_IREG_CCR2		0x04
#define JZ4750_CODEC_IREG_PMR1		0x05
#define JZ4750_CODEC_IREG_PMR2		0x06
#define JZ4750_CODEC_IREG_CRR		0x07
#define JZ4750_CODEC_IREG_ICR		0x08
#define JZ4750_CODEC_IREG_IFR		0x09
#define JZ4750_CODEC_IREG_CGR(x)	(0x0a+(x)-1)
#define JZ4750_CODEC_IREG_CR3		0x16
#define JZ4750_CODEC_IREG_AGC(x)	(0x17+(x)-1)

#define JZ4750_CODEC_AICR_VALUE			0x0f

#define JZ4750_CODEC_CR1_SB_MICBIAS		BIT(7)
#define JZ4750_CODEC_CR1_MONO			BIT(6)

#define JZ4750_CODEC_CR1_DAC_MUTE_OFFSET	5
#define JZ4750_CODEC_CR1_HP_DIS_OFFSET		4
#define JZ4750_CODEC_CR1_DACSEL_OFFSET		3
#define JZ4750_CODEC_CR1_BYPASS_OFFSET		2

#define JZ4750_CODEC_CCR1_VALUE			0x00

#define JZ4750_CODEC_CCR2_DAC_RATE_OFFSET	4
#define JZ4750_CODEC_CCR2_DAC_RATE_MASK		0xf0
#define JZ4750_CODEC_CCR2_ADC_RATE_OFFSET	0
#define JZ4750_CODEC_CCR2_ADC_RATE_MASK		0x0f

#define JZ4750_CODEC_PMR1_SB_DAC_OFFSET		7
#define JZ4750_CODEC_PMR1_SB_OUT_OFFSET		6
#define JZ4750_CODEC_PMR1_SB_MIX_OFFSET		5
#define JZ4750_CODEC_PMR1_SB_ADC_OFFSET		4
#define JZ4750_CODEC_PMR1_SB_LIN_OFFSET		3
#define JZ4750_CODEC_PMR1_SB_IND_OFFSET		0

static const uint8_t jz4750_codec_regs[] = {
	0x0c, 0xaa, 0x78, 0x00, 0x00, 0xff, 0x03, 0x51,
	0x3f, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x0a, 0x0a, 0x00, 0x00, 0xc0, 0x34, 0x07,
	0x44, 0x1f, 0x00,
};

struct jz4750_codec {
	void __iomem *base;
	struct resource *mem;
};

static unsigned int jz4750_codec_sync(struct snd_soc_codec *codec)
{
	struct jz4750_codec *jz4750_codec = snd_soc_codec_get_drvdata(codec);

	// TODO: Add a timeout here. PM says max time is 0.17uS + 1 PCLK.
	while(readl(jz4750_codec->base + JZ4750_CODEC_REG_RGADW) &
			JZ4750_CODEC_RGWR);

	return 0;
}

static unsigned int jz4750_codec_read(struct snd_soc_codec *codec,
	unsigned int reg)
{
	struct jz4750_codec *jz4750_codec = snd_soc_codec_get_drvdata(codec);

	if(jz4750_codec_sync(codec) != 0)
		return 0; /* Timeout */

	writel(reg << JZ4750_CODEC_RGADDR_OFFSET, jz4750_codec->base +
			JZ4750_CODEC_REG_RGADW);

	return readl(jz4750_codec->base + JZ4750_CODEC_REG_RGDATA) &
			JZ4750_CODEC_RGDOUT_MASK;
}

static int jz4750_codec_write(struct snd_soc_codec *codec, unsigned int reg,
	unsigned int val)
{
	struct jz4750_codec *jz4750_codec = snd_soc_codec_get_drvdata(codec);
	u32 *cache = codec->reg_cache;
	unsigned int val_verify;

	if(jz4750_codec_sync(codec) != 0)
		return -EBUSY; /* Timeout */

	writel((reg << JZ4750_CODEC_RGADDR_OFFSET) | JZ4750_CODEC_RGWR | val,
			jz4750_codec->base + JZ4750_CODEC_REG_RGADW);

	/* PM recommends to check the value after writing. Maybe this should
	 * only be enabled on debugging */
	val_verify = jz4750_codec_read(codec, reg);
	if(val_verify != val) {
		dev_err(codec->dev, "Failed to write register: reg %04x write %08x read %08x\n",
			reg, val, val_verify);

		return -EAGAIN;
	}

	return 0;
}

static const struct snd_kcontrol_new jz4750_codec_controls[] = {
	SOC_DOUBLE_R("Master Playback Volume", JZ4750_CODEC_IREG_CGR(9),
			JZ4750_CODEC_IREG_CGR(8), 0, 32, 0),
	SOC_SINGLE("Master Playback Switch", JZ4750_CODEC_IREG_CR1,
			JZ4750_CODEC_CR1_HP_DIS_OFFSET, 1, 1),
};

static const struct snd_kcontrol_new jz4750_codec_output_controls[] = {
	SOC_DAPM_SINGLE("DAC Switch", JZ4750_CODEC_IREG_CR1,
			JZ4750_CODEC_CR1_DACSEL_OFFSET, 1, 0),
	SOC_DAPM_SINGLE("Bypass Switch", JZ4750_CODEC_IREG_CR1,
			JZ4750_CODEC_CR1_BYPASS_OFFSET, 1, 0),
};

static const struct snd_soc_dapm_widget jz4750_codec_dapm_widgets[] = {
	SND_SOC_DAPM_DAC("DAC", "Playback", JZ4750_CODEC_IREG_PMR1,
			JZ4750_CODEC_PMR1_SB_DAC_OFFSET, 1),

	SND_SOC_DAPM_MIXER("Output Mixer", JZ4750_CODEC_IREG_PMR1,
			JZ4750_CODEC_PMR1_SB_MIX_OFFSET, 1,
			jz4750_codec_output_controls,
			ARRAY_SIZE(jz4750_codec_output_controls)),

	SND_SOC_DAPM_OUTPUT("LOUT"),
	SND_SOC_DAPM_OUTPUT("ROUT"),
};

static const struct snd_soc_dapm_route jz4750_codec_dapm_routes[] = {
	{"Output Mixer", "DAC Switch", "DAC"},

	{"LOUT", NULL, "Output Mixer"},
	{"ROUT", NULL, "Output Mixer"},
};

static int jz4750_codec_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
	uint32_t val;
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;

	switch (params_rate(params)) {
	case 8000:
		val = 10;
		break;
	case 9600:
		val = 9;
		break;
	case 11025:
		val = 8;
		break;
	case 12000:
		val = 7;
		break;
	case 16000:
		val = 6;
		break;
	case 22050:
		val = 5;
		break;
	case 24000:
		val = 4;
		break;
	case 32000:
		val = 3;
		break;
	case 44100:
		val = 2;
		break;
	case 48000:
		val = 1;
		break;
	case 96000:
		val = 0;
		break;
	default:
		return -EINVAL;
	}

	if(substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
		val <<= JZ4750_CODEC_CCR2_ADC_RATE_OFFSET;
		snd_soc_update_bits(codec, JZ4750_CODEC_IREG_CCR2,
				JZ4750_CODEC_CCR2_ADC_RATE_MASK, val);
	} else {
		val <<= JZ4750_CODEC_CCR2_DAC_RATE_OFFSET;
		snd_soc_update_bits(codec, JZ4750_CODEC_IREG_CCR2,
				JZ4750_CODEC_CCR2_DAC_RATE_MASK, val);
	}

	return 0;
}

static struct snd_soc_dai_ops jz4750_codec_dai_ops = {
	.hw_params = jz4750_codec_hw_params,
};

static struct snd_soc_dai_driver jz4750_codec_dai = {
	.name = "jz4750-hifi",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_8000_96000,
		.formats = SNDRV_PCM_FMTBIT_S16_LE,
	},
	.ops = &jz4750_codec_dai_ops,
	.symmetric_rates = 1,
};

static int jz4750_codec_set_bias_level(struct snd_soc_codec *codec,
	enum snd_soc_bias_level level)
{
	unsigned int mask;
	unsigned int value;

/*	switch (level) {
	case SND_SOC_BIAS_ON:
		break;
	case SND_SOC_BIAS_PREPARE:
		mask = jz4750_CODEC_1_VREF_DISABLE |
				jz4750_CODEC_1_VREF_AMP_DISABLE |
				jz4750_CODEC_1_HEADPHONE_POWERDOWN_M;
		value = 0;

		snd_soc_update_bits(codec, jz4750_REG_CODEC_1, mask, value);
		break;
	case SND_SOC_BIAS_STANDBY:
		/* The only way to clear the suspend flag is to reset the codec //
		if (codec->bias_level == SND_SOC_BIAS_OFF)
			jz4750_codec_wakeup(codec);

		mask = jz4750_CODEC_1_VREF_DISABLE |
			jz4750_CODEC_1_VREF_AMP_DISABLE |
			jz4750_CODEC_1_HEADPHONE_POWERDOWN_M;
		value = jz4750_CODEC_1_VREF_DISABLE |
			jz4750_CODEC_1_VREF_AMP_DISABLE |
			jz4750_CODEC_1_HEADPHONE_POWERDOWN_M;

		snd_soc_update_bits(codec, jz4750_REG_CODEC_1, mask, value);
		break;
	case SND_SOC_BIAS_OFF:
		mask = jz4750_CODEC_1_SUSPEND;
		value = jz4750_CODEC_1_SUSPEND;

		snd_soc_update_bits(codec, jz4750_REG_CODEC_1, mask, value);
		break;
	default:
		break;
	}

	codec->bias_level = level;*/

	return 0;
}

static int jz4750_codec_dev_probe(struct snd_soc_codec *codec)
{
	struct snd_soc_dapm_context* dapm = &codec->dapm;

	/* Write CONFIGn (n=1,4) magic values */
	snd_soc_write(codec, JZ4750_CODEC_IREG_AICR, JZ4750_CODEC_AICR_VALUE);
	snd_soc_write(codec, JZ4750_CODEC_IREG_CCR1, JZ4750_CODEC_CCR1_VALUE);

	// power up
	snd_soc_update_bits(codec, JZ4750_CODEC_IREG_PMR1, 0xe0, 0);
	snd_soc_update_bits(codec, JZ4750_CODEC_IREG_PMR2, 0x03, 0);

	snd_soc_add_controls(codec, jz4750_codec_controls,
		ARRAY_SIZE(jz4750_codec_controls));

	snd_soc_dapm_new_controls(dapm, jz4750_codec_dapm_widgets,
		ARRAY_SIZE(jz4750_codec_dapm_widgets));

	snd_soc_dapm_add_routes(dapm, jz4750_codec_dapm_routes,
		ARRAY_SIZE(jz4750_codec_dapm_routes));

	jz4750_codec_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

	return 0;
}

static int jz4750_codec_dev_remove(struct snd_soc_codec *codec)
{
	jz4750_codec_set_bias_level(codec, SND_SOC_BIAS_OFF);

	return 0;
}

#ifdef CONFIG_PM_SLEEP

static int jz4750_codec_suspend(struct snd_soc_codec *codec, pm_message_t state)
{
	return jz4750_codec_set_bias_level(codec, SND_SOC_BIAS_OFF);
}

static int jz4750_codec_resume(struct snd_soc_codec *codec)
{
	return jz4750_codec_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
}

#else
#define jz4750_codec_suspend NULL
#define jz4750_codec_resume NULL
#endif

static struct snd_soc_codec_driver soc_codec_dev_jz4750_codec = {
	.probe 		= jz4750_codec_dev_probe,
	.remove		= jz4750_codec_dev_remove,
	.suspend	= jz4750_codec_suspend,
	.resume		= jz4750_codec_resume,
	.read		= jz4750_codec_read,
	.write		= jz4750_codec_write,
	.set_bias_level	= jz4750_codec_set_bias_level,

	.reg_cache_default	= jz4750_codec_regs,
	.reg_word_size		= sizeof(u8),
	.reg_cache_size		= ARRAY_SIZE(jz4750_codec_regs),
};

static int __devinit jz4750_codec_probe(struct platform_device *pdev)
{
	int ret;
	struct jz4750_codec *jz4750_codec;
	struct resource *mem;

	jz4750_codec = kzalloc(sizeof(*jz4750_codec), GFP_KERNEL);
	if (!jz4750_codec)
		return -ENOMEM;

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem) {
		dev_err(&pdev->dev, "Failed to get mmio memory resource\n");
		ret = -ENOENT;
		goto err_free_codec;
	}

	mem = request_mem_region(mem->start, resource_size(mem), pdev->name);
	if (!mem) {
		dev_err(&pdev->dev, "Failed to request mmio memory region\n");
		ret = -EBUSY;
		goto err_free_codec;
	}

	jz4750_codec->base = ioremap(mem->start, resource_size(mem));
	if (!jz4750_codec->base) {
		dev_err(&pdev->dev, "Failed to ioremap mmio memory\n");
		ret = -EBUSY;
		goto err_release_mem_region;
	}
	jz4750_codec->mem = mem;

	platform_set_drvdata(pdev, jz4750_codec);

	ret = snd_soc_register_codec(&pdev->dev,
			&soc_codec_dev_jz4750_codec, &jz4750_codec_dai, 1);
	if (ret) {
		dev_err(&pdev->dev, "Failed to register codec\n");
		goto err_iounmap;
	}

	return 0;

err_iounmap:
	iounmap(jz4750_codec->base);
err_release_mem_region:
	release_mem_region(mem->start, resource_size(mem));
err_free_codec:
	kfree(jz4750_codec);

	return ret;
}

static int __devexit jz4750_codec_remove(struct platform_device *pdev)
{
	struct jz4750_codec *jz4750_codec = platform_get_drvdata(pdev);
	struct resource *mem = jz4750_codec->mem;

	snd_soc_unregister_codec(&pdev->dev);

	iounmap(jz4750_codec->base);
	release_mem_region(mem->start, resource_size(mem));

	platform_set_drvdata(pdev, NULL);
	kfree(jz4750_codec);

	return 0;
}

static struct platform_driver jz4750_codec_driver = {
	.probe = jz4750_codec_probe,
	.remove = __devexit_p(jz4750_codec_remove),
	.driver = {
		.name = "jz4750-codec",
		.owner = THIS_MODULE,
	},
};

static int __init jz4750_codec_init(void)
{
	return platform_driver_register(&jz4750_codec_driver);
}
module_init(jz4750_codec_init);

static void __exit jz4750_codec_exit(void)
{
	platform_driver_unregister(&jz4750_codec_driver);
}
module_exit(jz4750_codec_exit);

MODULE_DESCRIPTION("JZ4750 SoC internal codec driver");
MODULE_AUTHOR("Peter Zotov <whitequark@whitequark.org>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:jz4750-codec");

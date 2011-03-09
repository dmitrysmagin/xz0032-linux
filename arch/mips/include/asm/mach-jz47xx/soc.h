#ifndef __ASM_MACH_JZ47XX_SOC_H__
#define __ASM_MACH_JZ47XX_SOC_H__

static inline bool soc_is_jz4740(void)
{
#ifdef CONFIG_MACH_JZ4740
	return true;
#else
	return false;
#endif
}

static inline bool soc_is_jz4750l(void)
{
#ifdef CONFIG_MACH_JZ4750L
	return true;
#else
	return false;
#endif
}

static inline bool soc_is_jz4760(void)
{
#ifdef CONFIG_MACH_JZ4760
	return true;
#else
	return false;
#endif
}


#endif

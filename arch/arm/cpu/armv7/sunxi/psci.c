/*
 * Copyright (C) 2016
 * Author: Chen-Yu Tsai <wens@csie.org>
 *
 * Based on assembly code by Marc Zyngier <marc.zyngier@arm.com>,
 * which was based on code by Carl van Schaik <carl@ok-labs.com>.
 *
 * SPDX-License-Identifier:	GPL-2.0
 */
#include <config.h>
#include <common.h>

#include <asm/arch/cpu.h>
#include <asm/arch/cpucfg.h>
#include <asm/arch/prcm.h>
#include <asm/armv7.h>
#include <asm/gic.h>
#include <asm/io.h>
#include <asm/psci.h>
#include <asm/system.h>

#include <linux/bitops.h>

#define __secure	__attribute__ ((section ("._secure.text")))
#define __irq		__attribute__ ((interrupt ("IRQ")))

#define	GICD_BASE	(SUNXI_GIC400_BASE + GIC_DIST_OFFSET)
#define	GICC_BASE	(SUNXI_GIC400_BASE + GIC_CPU_OFFSET_A15)

static void __secure cp15_write_cntp_tval(u32 tval)
{
	asm volatile ("mcr p15, 0, %0, c14, c2, 0" : : "r" (tval));
}

static void __secure cp15_write_cntp_ctl(u32 val)
{
	asm volatile ("mcr p15, 0, %0, c14, c2, 1" : : "r" (val));
}

static u32 __secure cp15_read_cntp_ctl(void)
{
	u32 val;

	asm volatile ("mrc p15, 0, %0, c14, c2, 1" : "=r" (val));

	return val;
}

#define ONE_MS (CONFIG_TIMER_CLK_FREQ / 1000)

static void __secure __mdelay(u32 ms)
{
	u32 reg = ONE_MS * ms;

	cp15_write_cntp_tval(reg);
	ISB;
	cp15_write_cntp_ctl(3);

	do {
		ISB;
		reg = cp15_read_cntp_ctl();
	} while (!(reg & BIT(2)));

	cp15_write_cntp_ctl(0);
	ISB;
}

static void __secure clamp_release(u32 __maybe_unused *clamp)
{
#if defined(CONFIG_MACH_SUN6I) || defined(CONFIG_MACH_SUN7I) || \
	defined(CONFIG_MACH_SUN8I_H3)
	u32 tmp = 0x1ff;
	do {
		tmp >>= 1;
		writel(tmp, clamp);
	} while (tmp);

	__mdelay(10);
#endif
}

static void __secure clamp_set(u32 __maybe_unused *clamp)
{
#if defined(CONFIG_MACH_SUN6I) || defined(CONFIG_MACH_SUN7I) || \
	defined(CONFIG_MACH_SUN8I_H3)
	writel(0xff, clamp);
#endif
}

static void __secure sunxi_power_switch(u32 *clamp, u32 *pwroff, bool on,
					int cpu)
{
	if (on) {
		/* Release power clamp */
		clamp_release(clamp);

		/* Clear power gating */
		clrbits_le32(pwroff, BIT(cpu));
	} else {
		/* Set power gating */
		setbits_le32(pwroff, BIT(cpu));

		/* Activate power clamp */
		clamp_set(clamp);
	}
}

#ifdef CONFIG_MACH_SUN7I
/* sun7i (A20) is different from other single cluster SoCs */
static void __secure sunxi_cpu_set_power(int __always_unused cpu, bool on)
{
	struct sunxi_cpucfg_reg *cpucfg =
		(struct sunxi_cpucfg_reg *)SUNXI_CPUCFG_BASE;

	sunxi_power_switch(&cpucfg->cpu1_pwr_clamp, &cpucfg->cpu1_pwroff,
			   on, 0);
}
#else /* ! CONFIG_MACH_SUN7I */
static void __secure sunxi_cpu_set_power(int cpu, bool on)
{
	struct sunxi_prcm_reg *prcm =
		(struct sunxi_prcm_reg *)SUNXI_PRCM_BASE;

	sunxi_power_switch(&prcm->cpu_pwr_clamp[cpu], &prcm->cpu_pwroff,
			   on, cpu);
}
#endif /* CONFIG_MACH_SUN7I */

void __secure sunxi_cpu_power_off(u32 cpuid)
{
	struct sunxi_cpucfg_reg *cpucfg =
		(struct sunxi_cpucfg_reg *)SUNXI_CPUCFG_BASE;
	u32 cpu = cpuid & 0x3;

	/* Wait for the core to enter WFI */
	while (1) {
		if (readl(&cpucfg->cpu[cpu].status) & BIT(2))
			break;
		__mdelay(1);
	}

	/* Assert reset on target CPU */
	writel(0, &cpucfg->cpu[cpu].rst);

	/* Lock CPU (Disable external debug access) */
	clrbits_le32(&cpucfg->dbg_ctrl1, BIT(cpu));

	/* Power down CPU */
	sunxi_cpu_set_power(cpuid, false);

	/* Unlock CPU (Disable external debug access) */
	setbits_le32(&cpucfg->dbg_ctrl1, BIT(cpu));
}

static u32 __secure cp15_read_scr(void)
{
	u32 scr;

	asm volatile ("mrc p15, 0, %0, c1, c1, 0" : "=r" (scr));

	return scr;
}

static void __secure cp15_write_scr(u32 scr)
{
	asm volatile ("mcr p15, 0, %0, c1, c1, 0" : : "r" (scr));
	ISB;
}

/*
 * Although this is an FIQ handler, the FIQ is processed in monitor mode,
 * which means there's no FIQ banked registers. This is the same as IRQ
 * mode, so use the IRQ attribute to ask the compiler to handler entry
 * and return.
 */
void __secure __irq psci_fiq_enter(void)
{
	u32 scr, reg, cpu;

	/* Switch to secure mode */
	scr = cp15_read_scr();
	cp15_write_scr(scr & ~BIT(0));

	/* Validate reason based on IAR and acknowledge */
	reg = readl(GICC_BASE + GICC_IAR);

	/* Skip spurious interrupts 1022 and 1023 */
	if (reg == 1023 || reg == 1022)
		goto out;

	/* End of interrupt */
	writel(reg, GICC_BASE + GICC_EOIR);
	DSB;

	/* Get CPU number */
	cpu = (reg >> 10) & 0x7;

	/* Power off the CPU */
	sunxi_cpu_power_off(cpu);

out:
	/* Restore security level */
	cp15_write_scr(scr);
}

int __secure psci_cpu_on(u32 __always_unused unused, u32 mpidr, u32 pc)
{
	struct sunxi_cpucfg_reg *cpucfg =
		(struct sunxi_cpucfg_reg *)SUNXI_CPUCFG_BASE;
	u32 cpu = (mpidr & 0x3);

	/* store target PC at target CPU stack top */
	writel(pc, psci_get_cpu_stack_top(cpu));
	DSB;

	/* Set secondary core power on PC */
	writel((u32)&psci_cpu_entry, &cpucfg->priv0);

	/* Assert reset on target CPU */
	writel(0, &cpucfg->cpu[cpu].rst);

	/* Invalidate L1 cache */
	clrbits_le32(&cpucfg->gen_ctrl, BIT(cpu));

	/* Lock CPU (Disable external debug access) */
	clrbits_le32(&cpucfg->dbg_ctrl1, BIT(cpu));

	/* Power up target CPU */
	sunxi_cpu_set_power(cpu, true);

	/* De-assert reset on target CPU */
	writel(BIT(1) | BIT(0), &cpucfg->cpu[cpu].rst);

	/* Unlock CPU (Disable external debug access) */
	setbits_le32(&cpucfg->dbg_ctrl1, BIT(cpu));

	return ARM_PSCI_RET_SUCCESS;
}

void __secure psci_cpu_off(void)
{
	psci_cpu_off_common();

	/* Ask CPU0 via SGI15 to pull the rug... */
	writel(BIT(16) | 15, GICD_BASE + GICD_SGIR);
	DSB;

	/* Wait to be turned off */
	while (1)
		wfi();
}

void __secure psci_arch_init(void)
{
	u32 reg;

	/* SGI15 as Group-0 */
	clrbits_le32(GICD_BASE + GICD_IGROUPRn, BIT(15));

	/* Set SGI15 priority to 0 */
	writeb(0, GICD_BASE + GICD_IPRIORITYRn + 15);

	/* Be cool with non-secure */
	writel(0xff, GICC_BASE + GICC_PMR);

	/* Switch FIQEn on */
	setbits_le32(GICC_BASE + GICC_CTLR, BIT(3));

	reg = cp15_read_scr();
	reg |= BIT(2);  /* Enable FIQ in monitor mode */
	reg &= ~BIT(0); /* Secure mode */
	cp15_write_scr(reg);
}

/* dummy entry for end of psci text */
void __secure psci_text_end(void)
{
}

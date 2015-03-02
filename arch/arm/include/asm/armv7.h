/*
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 * Aneesh V <aneesh@ti.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef ARMV7_H
#define ARMV7_H

/* Cortex-A9 revisions */
#define MIDR_CORTEX_A9_R0P1	0x410FC091
#define MIDR_CORTEX_A9_R1P2	0x411FC092
#define MIDR_CORTEX_A9_R1P3	0x411FC093
#define MIDR_CORTEX_A9_R2P10	0x412FC09A

/* Cortex-A15 revisions */
#define MIDR_CORTEX_A15_R0P0	0x410FC0F0
#define MIDR_CORTEX_A15_R2P2	0x412FC0F2

/* Cortex-A7 revisions */
#define MIDR_CORTEX_A7_R0P0	0x410FC070

#define MIDR_PRIMARY_PART_MASK	0xFF0FFFF0

/* ID_PFR1 feature fields */
#define CPUID_ARM_SEC_SHIFT		4
#define CPUID_ARM_SEC_MASK		(0xF << CPUID_ARM_SEC_SHIFT)
#define CPUID_ARM_VIRT_SHIFT		12
#define CPUID_ARM_VIRT_MASK		(0xF << CPUID_ARM_VIRT_SHIFT)
#define CPUID_ARM_GENTIMER_SHIFT	16
#define CPUID_ARM_GENTIMER_MASK		(0xF << CPUID_ARM_GENTIMER_SHIFT)

/* valid bits in CBAR register / PERIPHBASE value */
#define CBAR_MASK			0xFFFF8000

/* CCSIDR */
#define CCSIDR_LINE_SIZE_OFFSET		0
#define CCSIDR_LINE_SIZE_MASK		0x7
#define CCSIDR_ASSOCIATIVITY_OFFSET	3
#define CCSIDR_ASSOCIATIVITY_MASK	(0x3FF << 3)
#define CCSIDR_NUM_SETS_OFFSET		13
#define CCSIDR_NUM_SETS_MASK		(0x7FFF << 13)

/*
 * Values for InD field in CSSELR
 * Selects the type of cache
 */
#define ARMV7_CSSELR_IND_DATA_UNIFIED	0
#define ARMV7_CSSELR_IND_INSTRUCTION	1

/* Values for Ctype fields in CLIDR */
#define ARMV7_CLIDR_CTYPE_NO_CACHE		0
#define ARMV7_CLIDR_CTYPE_INSTRUCTION_ONLY	1
#define ARMV7_CLIDR_CTYPE_DATA_ONLY		2
#define ARMV7_CLIDR_CTYPE_INSTRUCTION_DATA	3
#define ARMV7_CLIDR_CTYPE_UNIFIED		4

#ifndef __ASSEMBLY__
#include <linux/types.h>

/*
 * CP15 Barrier instructions
 * Please note that we have separate barrier instructions in ARMv7
 * However, we use the CP15 based instructtions because we use
 * -march=armv5 in U-Boot
 */
#define CP15ISB	asm volatile ("mcr     p15, 0, %0, c7, c5, 4" : : "r" (0))
#define CP15DSB	asm volatile ("mcr     p15, 0, %0, c7, c10, 4" : : "r" (0))
#define CP15DMB	asm volatile ("mcr     p15, 0, %0, c7, c10, 5" : : "r" (0))

/*
 * Workaround for ARM errata # 798870
 * Set L2ACTLR[7] to reissue any memory transaction in the L2 that has been
 * stalled for 1024 cycles to verify that its hazard condition still exists.
 */
static inline void v7_enable_l2_hazard_detect(void)
{
	uint32_t val;

	/* L2ACTLR[7]: Enable hazard detect timeout */
	asm volatile ("mrc     p15, 1, %0, c15, c0, 0\n\t" : "=r"(val));
	val |= (1 << 7);
	asm volatile ("mcr     p15, 1, %0, c15, c0, 0\n\t" : : "r"(val));
}

/*
 * Workaround for ARM errata # 799270
 * Ensure that the L2 logic has been used within the previous 256 cycles
 * before modifying the ACTLR.SMP bit. This is required during boot before
 * MMU has been enabled, or during a specified reset or power down sequence.
 */
static inline void v7_enable_smp(uint32_t address)
{
	uint32_t temp, val;

	/* Read auxiliary control register */
	asm volatile ("mrc     p15, 0, %0, c1, c0, 1\n\t" : "=r"(val));

	/* Enable SMP */
	val |= (1 << 6);

	/* Dummy read to assure L2 access */
	temp = readl(address);
	temp &= 0;
	val |= temp;

	/* Write auxiliary control register */
	asm volatile ("mcr     p15, 0, %0, c1, c0, 1\n\t" : : "r"(val));

	CP15DSB;
	CP15ISB;
}

void v7_en_l2_hazard_detect(void);
void v7_outer_cache_enable(void);
void v7_outer_cache_disable(void);
void v7_outer_cache_flush_all(void);
void v7_outer_cache_inval_all(void);
void v7_outer_cache_flush_range(u32 start, u32 end);
void v7_outer_cache_inval_range(u32 start, u32 end);

#if defined(CONFIG_ARMV7_NONSEC) || defined(CONFIG_ARMV7_VIRT)

int armv7_init_nonsec(void);
int armv7_update_dt(void *fdt);
bool armv7_boot_nonsec(void);

/* defined in assembly file */
unsigned int _nonsec_init(void);
void _do_nonsec_entry(void *target_pc, unsigned long r0,
		      unsigned long r1, unsigned long r2);
void _smp_pen(void);

extern char __secure_start[];
extern char __secure_end[];

#endif /* CONFIG_ARMV7_NONSEC || CONFIG_ARMV7_VIRT */

#endif /* ! __ASSEMBLY__ */

#endif

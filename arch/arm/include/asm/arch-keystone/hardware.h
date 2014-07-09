/*
 * Keystone2: Common SoC definitions, structures etc.
 *
 * (C) Copyright 2012-2014
 *     Texas Instruments Incorporated, <www.ti.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

#include <config.h>

#ifndef __ASSEMBLY__

#include <linux/sizes.h>
#include <asm/io.h>

#define	REG(addr)        (*(volatile unsigned int *)(addr))
#define REG_P(addr)      ((volatile unsigned int *)(addr))

typedef volatile unsigned int   dv_reg;
typedef volatile unsigned int   *dv_reg_p;

#endif

#define		BIT(x)	(1 << (x))

#define KS2_DDRPHY_PIR_OFFSET           0x04
#define KS2_DDRPHY_PGCR0_OFFSET         0x08
#define KS2_DDRPHY_PGCR1_OFFSET         0x0C
#define KS2_DDRPHY_PGSR0_OFFSET         0x10
#define KS2_DDRPHY_PGSR1_OFFSET         0x14
#define KS2_DDRPHY_PLLCR_OFFSET         0x18
#define KS2_DDRPHY_PTR0_OFFSET          0x1C
#define KS2_DDRPHY_PTR1_OFFSET          0x20
#define KS2_DDRPHY_PTR2_OFFSET          0x24
#define KS2_DDRPHY_PTR3_OFFSET          0x28
#define KS2_DDRPHY_PTR4_OFFSET          0x2C
#define KS2_DDRPHY_DCR_OFFSET           0x44

#define KS2_DDRPHY_DTPR0_OFFSET         0x48
#define KS2_DDRPHY_DTPR1_OFFSET         0x4C
#define KS2_DDRPHY_DTPR2_OFFSET         0x50

#define KS2_DDRPHY_MR0_OFFSET           0x54
#define KS2_DDRPHY_MR1_OFFSET           0x58
#define KS2_DDRPHY_MR2_OFFSET           0x5C
#define KS2_DDRPHY_DTCR_OFFSET          0x68
#define KS2_DDRPHY_PGCR2_OFFSET         0x8C

#define KS2_DDRPHY_ZQ0CR1_OFFSET        0x184
#define KS2_DDRPHY_ZQ1CR1_OFFSET        0x194
#define KS2_DDRPHY_ZQ2CR1_OFFSET        0x1A4
#define KS2_DDRPHY_ZQ3CR1_OFFSET        0x1B4

#define KS2_DDRPHY_DATX8_8_OFFSET       0x3C0

#define IODDRM_MASK                     0x00000180
#define ZCKSEL_MASK                     0x01800000
#define CL_MASK                         0x00000072
#define WR_MASK                         0x00000E00
#define BL_MASK                         0x00000003
#define RRMODE_MASK                     0x00040000
#define UDIMM_MASK                      0x20000000
#define BYTEMASK_MASK                   0x0003FC00
#define MPRDQ_MASK                      0x00000080
#define PDQ_MASK                        0x00000070
#define NOSRA_MASK                      0x08000000
#define ECC_MASK                        0x00000001

#define KS2_DDR3_MIDR_OFFSET            0x00
#define KS2_DDR3_STATUS_OFFSET          0x04
#define KS2_DDR3_SDCFG_OFFSET           0x08
#define KS2_DDR3_SDRFC_OFFSET           0x10
#define KS2_DDR3_SDTIM1_OFFSET          0x18
#define KS2_DDR3_SDTIM2_OFFSET          0x1C
#define KS2_DDR3_SDTIM3_OFFSET          0x20
#define KS2_DDR3_SDTIM4_OFFSET          0x28
#define KS2_DDR3_PMCTL_OFFSET           0x38
#define KS2_DDR3_ZQCFG_OFFSET           0xC8

#define KS2_DDR3_PLLCTRL_PHY_RESET	0x80000000

#define KS2_UART0_BASE                	0x02530c00
#define KS2_UART1_BASE                	0x02531000

/* PSC */
#define KS2_PSC_BASE			0x02350000
#define KS2_LPSC_GEM_0			15
#define KS2_LPSC_TETRIS			52
#define KS2_TETRIS_PWR_DOMAIN		31

/* AEMIF */
#define KS2_AEMIF_CNTRL_BASE       	0x21000a00
#define DAVINCI_ASYNC_EMIF_CNTRL_BASE   KS2_AEMIF_CNTRL_BASE

/* Flag from ks2_debug options to check if DSPs need to stay ON */
#define DBG_LEAVE_DSPS_ON		0x1

#ifdef CONFIG_SOC_K2HK
#include <asm/arch/hardware-k2hk.h>
#endif

#ifndef __ASSEMBLY__
static inline int cpu_is_k2hk(void)
{
	unsigned int jtag_id	= __raw_readl(JTAG_ID_REG);
	unsigned int part_no	= (jtag_id >> 12) & 0xffff;

	return (part_no == 0xb981) ? 1 : 0;
}

static inline int cpu_revision(void)
{
	unsigned int jtag_id	= __raw_readl(JTAG_ID_REG);
	unsigned int rev	= (jtag_id >> 28) & 0xf;

	return rev;
}

void share_all_segments(int priv_id);
int cpu_to_bus(u32 *ptr, u32 length);
void sdelay(unsigned long);

#endif

#endif /* __ASM_ARCH_HARDWARE_H */

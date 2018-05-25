/*
 * (C) Copyright 2018 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#ifndef _ASM_ARCH_CRU_RK3308_H
#define _ASM_ARCH_CRU_RK3308_H

#include <common.h>

#define MHz		1000000
#define OSC_HZ		(24 * MHz)

#define CORE_ACLK_HZ	408000000
#define CORE_DBG_HZ	204000000

#define BUS_ACLK_HZ	200000000
#define BUS_HCLK_HZ	100000000
#define BUS_PCLK_HZ	100000000

#define PERI_ACLK_HZ	200000000
#define PERI_HCLK_HZ	100000000
#define PERI_PCLK_HZ	100000000

#define AUDIO_HCLK_HZ	100000000
#define AUDIO_PCLK_HZ	100000000

/* RK3308 pll id */
enum rk3308_pll_id {
	APLL,
	DPLL,
	VPLL0,
	VPLL1,
	PLL_COUNT,
};

struct pll_div {
	u32 refdiv;
	u32 fbdiv;
	u32 postdiv1;
	u32 postdiv2;
	u32 frac;
};

/* Private data for the clock driver - used by rockchip_get_cru() */
struct rk3308_clk_priv {
	struct rk3308_cru *cru;
	ulong apll_hz;
	ulong dpll_hz;
	ulong vpll0_hz;
	ulong vpll1_hz;
};

struct rk3308_cru {
	struct rk3308_pll {
		unsigned int con0;
		unsigned int con1;
		unsigned int con2;
		unsigned int con3;
		unsigned int con4;
		unsigned int reserved0[3];
	} pll[4];
	unsigned int reserved1[8];
	unsigned int mode;
	unsigned int misc;
	unsigned int reserved2[2];
	unsigned int glb_cnt_th;
	unsigned int glb_rst_st;
	unsigned int glb_srst_fst;
	unsigned int glb_srst_snd;
	unsigned int glb_rst_con;
	unsigned int pll_lock;
	unsigned int reserved3[6];
	unsigned int hwffc_con0;
	unsigned int reserved4;
	unsigned int hwffc_th;
	unsigned int hwffc_intst;
	unsigned int apll_con0_s;
	unsigned int apll_con1_s;
	unsigned int clksel_con0_s;
	unsigned int reserved5;
	unsigned int clksel_con[74];
	unsigned int reserved6[54];
	unsigned int clkgate_con[15];
	unsigned int reserved7[(0x380 - 0x338) / 4 - 1];
	unsigned int ssgtbl[32];
	unsigned int softrst_con[10];
	unsigned int reserved8[(0x480 - 0x424) / 4 - 1];
	unsigned int sdmmc_con[2];
	unsigned int sdio_con[2];
	unsigned int emmc_con[2];
};

enum {
	/* PLLCON0*/
	PLL_BP_SHIFT		= 15,
	PLL_POSTDIV1_SHIFT	= 12,
	PLL_POSTDIV1_MASK	= 7 << PLL_POSTDIV1_SHIFT,
	PLL_FBDIV_SHIFT		= 0,
	PLL_FBDIV_MASK		= 0xfff,

	/* PLLCON1 */
	PLL_PDSEL_SHIFT		= 15,
	PLL_PD1_SHIFT		= 14,
	PLL_PD_SHIFT		= 13,
	PLL_PD_MASK		= 1 << PLL_PD_SHIFT,
	PLL_DSMPD_SHIFT		= 12,
	PLL_DSMPD_MASK		= 1 << PLL_DSMPD_SHIFT,
	PLL_LOCK_STATUS_SHIFT	= 10,
	PLL_LOCK_STATUS_MASK	= 1 << PLL_LOCK_STATUS_SHIFT,
	PLL_POSTDIV2_SHIFT	= 6,
	PLL_POSTDIV2_MASK	= 7 << PLL_POSTDIV2_SHIFT,
	PLL_REFDIV_SHIFT	= 0,
	PLL_REFDIV_MASK		= 0x3f,

	/* PLLCON2 */
	PLL_FOUT4PHASEPD_SHIFT	= 27,
	PLL_FOUTVCOPD_SHIFT	= 26,
	PLL_FOUTPOSTDIVPD_SHIFT	= 25,
	PLL_DACPD_SHIFT		= 24,
	PLL_FRAC_DIV	= 0xffffff,

	/* CRU_MODE */
	PLLMUX_FROM_XIN24M	= 0,
	PLLMUX_FROM_PLL,
	PLLMUX_FROM_RTC32K,
	USBPHY480M_MODE_SHIFT	= 8,
	USBPHY480M_MODE_MASK	= 3 << USBPHY480M_MODE_SHIFT,
	VPLL1_MODE_SHIFT		= 6,
	VPLL1_MODE_MASK		= 3 << VPLL1_MODE_SHIFT,
	VPLL0_MODE_SHIFT		= 4,
	VPLL0_MODE_MASK		= 3 << VPLL0_MODE_SHIFT,
	DPLL_MODE_SHIFT		= 2,
	DPLL_MODE_MASK		= 3 << DPLL_MODE_SHIFT,
	APLL_MODE_SHIFT		= 0,
	APLL_MODE_MASK		= 3 << APLL_MODE_SHIFT,

	/* CRU_CLK_SEL0_CON */
	CORE_ACLK_DIV_SHIFT	= 12,
	CORE_ACLK_DIV_MASK	= 0x7 << CORE_ACLK_DIV_SHIFT,
	CORE_DBG_DIV_SHIFT	= 8,
	CORE_DBG_DIV_MASK	= 0xf << CORE_DBG_DIV_SHIFT,
	CORE_CLK_PLL_SEL_SHIFT	= 6,
	CORE_CLK_PLL_SEL_MASK	= 0x3 << CORE_CLK_PLL_SEL_SHIFT,
	CORE_CLK_PLL_SEL_APLL	= 0,
	CORE_CLK_PLL_SEL_VPLL0,
	CORE_CLK_PLL_SEL_VPLL1,
	CORE_DIV_CON_SHIFT	= 0,
	CORE_DIV_CON_MASK	= 0x0f << CORE_DIV_CON_SHIFT,

	/* CRU_CLK_SEL5_CON */
	BUS_PLL_SEL_SHIFT	= 6,
	BUS_PLL_SEL_MASK	= 0x3 << BUS_PLL_SEL_SHIFT,
	BUS_PLL_SEL_DPLL	= 0,
	BUS_PLL_SEL_VPLL0,
	BUS_PLL_SEL_VPLL1,
	BUS_ACLK_DIV_SHIFT	= 0,
	BUS_ACLK_DIV_MASK	= 0x1f << BUS_ACLK_DIV_SHIFT,

	/* CRU_CLK_SEL6_CON */
	BUS_PCLK_DIV_SHIFT	= 8,
	BUS_PCLK_DIV_MASK	= 0x1f << BUS_PCLK_DIV_SHIFT,
	BUS_HCLK_DIV_SHIFT	= 0,
	BUS_HCLK_DIV_MASK	= 0x1f << BUS_HCLK_DIV_SHIFT,

	/* CRU_CLK_SEL8_CON */
	DCLK_VOP_SEL_SHIFT	= 14,
	DCLK_VOP_SEL_MASK	= 0x3 << DCLK_VOP_SEL_SHIFT,
	DCLK_VOP_SEL_DIVOUT	= 0,
	DCLK_VOP_SEL_FRACOUT,
	DCLK_VOP_SEL_24M,
	DCLK_VOP_PLL_SEL_SHIFT	= 10,
	DCLK_VOP_PLL_SEL_MASK	= 0x3 << DCLK_VOP_PLL_SEL_SHIFT,
	DCLK_VOP_PLL_SEL_DPLL	= 0,
	DCLK_VOP_PLL_SEL_VPLL0,
	DCLK_VOP_PLL_SEL_VPLL1,
	DCLK_VOP_DIV_SHIFT	= 0,
	DCLK_VOP_DIV_MASK	= 0xff,

	/* CRU_CLK_SEL25_CON */
	/* CRU_CLK_SEL26_CON */
	/* CRU_CLK_SEL27_CON */
	/* CRU_CLK_SEL28_CON */
	CLK_I2C_PLL_SEL_SHIFT		= 14,
	CLK_I2C_PLL_SEL_MASK		= 0x3 << CLK_I2C_PLL_SEL_SHIFT,
	CLK_I2C_PLL_SEL_DPLL		= 0,
	CLK_I2C_PLL_SEL_VPLL0,
	CLK_I2C_PLL_SEL_24M,
	CLK_I2C_DIV_CON_SHIFT		= 0,
	CLK_I2C_DIV_CON_MASK		= 0x7f << CLK_I2C_DIV_CON_SHIFT,

	/* CRU_CLK_SEL29_CON */
	CLK_PWM_PLL_SEL_SHIFT		= 14,
	CLK_PWM_PLL_SEL_MASK		= 0x3 << CLK_PWM_PLL_SEL_SHIFT,
	CLK_PWM_PLL_SEL_DPLL		= 0,
	CLK_PWM_PLL_SEL_VPLL0,
	CLK_PWM_PLL_SEL_24M,
	CLK_PWM_DIV_CON_SHIFT		= 0,
	CLK_PWM_DIV_CON_MASK		= 0x7f << CLK_PWM_DIV_CON_SHIFT,

	/* CRU_CLK_SEL30_CON */
	/* CRU_CLK_SEL31_CON */
	/* CRU_CLK_SEL32_CON */
	CLK_SPI_PLL_SEL_SHIFT		= 14,
	CLK_SPI_PLL_SEL_MASK		= 0x3 << CLK_SPI_PLL_SEL_SHIFT,
	CLK_SPI_PLL_SEL_DPLL		= 0,
	CLK_SPI_PLL_SEL_VPLL0,
	CLK_SPI_PLL_SEL_24M,
	CLK_SPI_DIV_CON_SHIFT		= 0,
	CLK_SPI_DIV_CON_MASK		= 0x7f << CLK_SPI_DIV_CON_SHIFT,

	/* CRU_CLK_SEL34_CON */
	CLK_SARADC_DIV_CON_SHIFT	= 0,
	CLK_SARADC_DIV_CON_MASK		= 0x7ff << CLK_SARADC_DIV_CON_SHIFT,

	/* CRU_CLK_SEL36_CON */
	PERI_PLL_SEL_SHIFT	= 6,
	PERI_PLL_SEL_MASK	= 0x3 << PERI_PLL_SEL_SHIFT,
	PERI_PLL_DPLL		= 0,
	PERI_PLL_VPLL0,
	PERI_PLL_VPLL1,
	PERI_ACLK_DIV_SHIFT	= 0,
	PERI_ACLK_DIV_MASK	= 0x1f << PERI_ACLK_DIV_SHIFT,

	/* CRU_CLK_SEL37_CON */
	PERI_PCLK_DIV_SHIFT	= 8,
	PERI_PCLK_DIV_MASK	= 0x1f << PERI_PCLK_DIV_SHIFT,
	PERI_HCLK_DIV_SHIFT	= 0,
	PERI_HCLK_DIV_MASK	= 0x1f << PERI_HCLK_DIV_SHIFT,

	/* CRU_CLKSEL41_CON */
	EMMC_CLK_SEL_SHIFT	= 15,
	EMMC_CLK_SEL_MASK	= 1 << EMMC_CLK_SEL_SHIFT,
	EMMC_CLK_SEL_EMMC	= 0,
	EMMC_CLK_SEL_EMMC_DIV50,
	EMMC_PLL_SHIFT		= 8,
	EMMC_PLL_MASK		= 0x3 << EMMC_PLL_SHIFT,
	EMMC_SEL_DPLL		= 0,
	EMMC_SEL_VPLL0,
	EMMC_SEL_VPLL1,
	EMMC_SEL_24M,
	EMMC_DIV_SHIFT		= 0,
	EMMC_DIV_MASK		= 0xff << EMMC_DIV_SHIFT,
};

check_member(rk3308_cru, emmc_con[1], 0x494);

#endif

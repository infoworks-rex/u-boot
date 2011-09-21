/*
 *  (C) Copyright 2010,2011
 *  NVIDIA Corporation <www.nvidia.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <ns16550.h>
#include <asm/io.h>
#include <asm/arch/tegra2.h>
#include <asm/arch/sys_proto.h>

#include <asm/arch/clk_rst.h>
#include <asm/arch/clock.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/uart.h>
#include "board.h"

#ifdef CONFIG_TEGRA2_MMC
#include <mmc.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

const struct tegra2_sysinfo sysinfo = {
	CONFIG_TEGRA2_BOARD_STRING
};

/*
 * Routine: timer_init
 * Description: init the timestamp and lastinc value
 */
int timer_init(void)
{
	return 0;
}

static void enable_uart(enum periph_id pid)
{
	/* Assert UART reset and enable clock */
	reset_set_enable(pid, 1);
	clock_enable(pid);
	clock_ll_set_source(pid, 0);	/* UARTx_CLK_SRC = 00, PLLP_OUT0 */

	/* wait for 2us */
	udelay(2);

	/* De-assert reset to UART */
	reset_set_enable(pid, 0);
}

/*
 * Routine: clock_init_uart
 * Description: init the PLL and clock for the UART(s)
 */
static void clock_init_uart(void)
{
#if defined(CONFIG_TEGRA2_ENABLE_UARTA)
	enable_uart(PERIPH_ID_UART1);
#endif	/* CONFIG_TEGRA2_ENABLE_UARTA */
#if defined(CONFIG_TEGRA2_ENABLE_UARTD)
	enable_uart(PERIPH_ID_UART4);
#endif	/* CONFIG_TEGRA2_ENABLE_UARTD */
}

/*
 * Routine: pin_mux_uart
 * Description: setup the pin muxes/tristate values for the UART(s)
 */
static void pin_mux_uart(void)
{
	struct pmux_tri_ctlr *pmt = (struct pmux_tri_ctlr *)NV_PA_APB_MISC_BASE;
	u32 reg;

#if defined(CONFIG_TEGRA2_ENABLE_UARTA)
	reg = readl(&pmt->pmt_ctl_c);
	reg &= 0xFFF0FFFF;	/* IRRX_/IRTX_SEL [19:16] = 00 UARTA */
	writel(reg, &pmt->pmt_ctl_c);

	pinmux_tristate_disable(PINGRP_IRRX);
	pinmux_tristate_disable(PINGRP_IRTX);
#endif	/* CONFIG_TEGRA2_ENABLE_UARTA */
#if defined(CONFIG_TEGRA2_ENABLE_UARTD)
	reg = readl(&pmt->pmt_ctl_b);
	reg &= 0xFFFFFFF3;	/* GMC_SEL [3:2] = 00, UARTD */
	writel(reg, &pmt->pmt_ctl_b);

	pinmux_tristate_disable(PINGRP_GMC);
#endif	/* CONFIG_TEGRA2_ENABLE_UARTD */
}

#ifdef CONFIG_TEGRA2_MMC
/*
 * Routine: clock_init_mmc
 * Description: init the PLL and clocks for the SDMMC controllers
 */
static void clock_init_mmc(void)
{
	clock_start_periph_pll(PERIPH_ID_SDMMC4, CLOCK_ID_PERIPH, 20000000);
	clock_start_periph_pll(PERIPH_ID_SDMMC3, CLOCK_ID_PERIPH, 20000000);
}

/*
 * Routine: pin_mux_mmc
 * Description: setup the pin muxes/tristate values for the SDMMC(s)
 */
static void pin_mux_mmc(void)
{
	struct pmux_tri_ctlr *pmt = (struct pmux_tri_ctlr *)NV_PA_APB_MISC_BASE;
	u32 reg;

	/* SDMMC4 */
	/* config 2, x8 on 2nd set of pins */
	reg = readl(&pmt->pmt_ctl_a);
	reg |= (3 << 16);	/* ATB_SEL [17:16] = 11 SDIO4 */
	writel(reg, &pmt->pmt_ctl_a);
	reg = readl(&pmt->pmt_ctl_b);
	reg |= (3 << 0);	/* GMA_SEL [1:0] = 11 SDIO4 */
	writel(reg, &pmt->pmt_ctl_b);
	reg = readl(&pmt->pmt_ctl_d);
	reg |= (3 << 0);	/* GME_SEL [1:0] = 11 SDIO4 */
	writel(reg, &pmt->pmt_ctl_d);

	pinmux_tristate_disable(PINGRP_ATB);
	pinmux_tristate_disable(PINGRP_GMA);
	pinmux_tristate_disable(PINGRP_GME);

	/* SDMMC3 */
	/* SDIO3_CLK, SDIO3_CMD, SDIO3_DAT[3:0] */
	reg = readl(&pmt->pmt_ctl_d);
	reg &= 0xFFFF03FF;
	reg |= (2 << 10);	/* SDB_SEL [11:10] = 01 SDIO3 */
	reg |= (2 << 12);	/* SDC_SEL [13:12] = 01 SDIO3 */
	reg |= (2 << 14);	/* SDD_SEL [15:14] = 01 SDIO3 */
	writel(reg, &pmt->pmt_ctl_d);

	pinmux_tristate_disable(PINGRP_SDC);
	pinmux_tristate_disable(PINGRP_SDD);
	pinmux_tristate_disable(PINGRP_SDB);
}
#endif

/*
 * Routine: board_init
 * Description: Early hardware init.
 */
int board_init(void)
{
	clock_init();
	clock_verify();

	/* boot param addr */
	gd->bd->bi_boot_params = (NV_PA_SDRAM_BASE + 0x100);

	return 0;
}

#ifdef CONFIG_TEGRA2_MMC
/* this is a weak define that we are overriding */
int board_mmc_init(bd_t *bd)
{
	debug("board_mmc_init called\n");
	/* Enable clocks, muxes, etc. for SDMMC controllers */
	clock_init_mmc();
	pin_mux_mmc();

	debug("board_mmc_init: init eMMC\n");
	/* init dev 0, eMMC chip, with 4-bit bus */
	tegra2_mmc_init(0, 4);

	debug("board_mmc_init: init SD slot\n");
	/* init dev 1, SD slot, with 4-bit bus */
	tegra2_mmc_init(1, 4);

	return 0;
}

/* this is a weak define that we are overriding */
int board_mmc_getcd(u8 *cd, struct mmc *mmc)
{
	debug("board_mmc_getcd called\n");
	/*
	 * Hard-code CD presence for now. Need to add GPIO inputs
	 * for Seaboard & Harmony (& Kaen/Aebl/Wario?)
	 */
	*cd = 1;
	return 0;
}
#endif

#ifdef CONFIG_BOARD_EARLY_INIT_F
int board_early_init_f(void)
{
	/* Initialize essential common plls */
	clock_early_init();

	/* Initialize UART clocks */
	clock_init_uart();

	/* Initialize periph pinmuxes */
	pin_mux_uart();

	/* Initialize periph GPIOs */
	gpio_config_uart();

	/* Init UART, scratch regs, and start CPU */
	tegra2_start();
	return 0;
}
#endif	/* EARLY_INIT */

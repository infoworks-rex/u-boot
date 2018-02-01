/*
 * Copyright (C) 2018 Rockchip Electronics Co., Ltd
 * Author: Zhihuan He <huan.he@rock-chips.com>
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <ram.h>
#include <asm/arch/sdram_common.h>
#include <asm/arch/hardware.h>
#include <asm/arch/grf_rv1108.h>

DECLARE_GLOBAL_DATA_PTR;

#define GRF_BASE		0x10300000

void board_debug_uart_init(void)
{
#ifdef CONFIG_SPL_BUILD
	struct rv1108_grf *grf = (void *)GRF_BASE;

#if defined(CONFIG_DEBUG_UART_BASE) && (CONFIG_DEBUG_UART_BASE == 0x10230000)
	rk_clrsetreg(&grf->gpio3a_iomux,	/* UART0 */
		     GPIO3A6_MASK | GPIO3A5_MASK,
		     GPIO3A6_UART1_SOUT << GPIO3A6_SHIFT |
		     GPIO3A5_UART1_SIN << GPIO3A5_SHIFT);
#else
	rk_clrsetreg(&grf->gpio2d_iomux,	/* UART2 */
		     GPIO2D2_MASK | GPIO2D1_MASK,
		     GPIO2D2_UART2_SOUT_M0 << GPIO2D2_SHIFT |
		     GPIO2D1_UART2_SIN_M0 << GPIO2D1_SHIFT);
#endif
#endif /*CONFIG_SPL_BUILD*/
}

int dram_init(void)
{
	struct rv1108_grf *grf = (void *)GRF_BASE;

	gd->ram_size = rockchip_sdram_size((phys_addr_t)&grf->os_reg2);

	return 0;
}

int dram_init_banksize(void)
{
	size_t max_size = min((unsigned long)(gd->ram_size +
			      CONFIG_SYS_SDRAM_BASE), gd->ram_top);

	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size = max_size - gd->bd->bi_dram[0].start;

	return 0;
}


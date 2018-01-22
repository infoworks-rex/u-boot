/*
 * Copyright (c) 2017 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#include <common.h>
#include <asm/io.h>
#include <asm/arch/grf_px30.h>
#include <asm/arch/hardware.h>
#include <asm/armv8/mmu.h>

static struct mm_region px30_mem_map[] = {
	{
		.virt = 0x0UL,
		.phys = 0x0UL,
		.size = 0xff000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	}, {
		.virt = 0xff000000UL,
		.phys = 0xff000000UL,
		.size = 0x01000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	}, {
		/* List terminator */
		0,
	}
};

struct mm_region *mem_map = px30_mem_map;

#define SGRF_DDR_CON0 0x10150000

int arch_cpu_init(void)
{
#ifdef CONFIG_SPL_BUILD
	/* We do some SoC one time setting here. */
	/* Disable the ddr secure region setting to make it non-secure */
	rk_clrreg(SGRF_DDR_CON0, 0x4000);
#endif
	return 0;
}
#define GRF_BASE	0x11000000
void board_debug_uart_init(void)
{
static struct px30_grf * const grf = (void *)GRF_BASE;
	/* Enable early UART2 channel m0 on the px30 */
	rk_clrsetreg(&grf->gpio1dl_iomux,
		     GPIO1D3_MASK | GPIO1D2_MASK,
		     GPIO1D3_UART2_RXM0 << GPIO1D3_SHIFT |
		     GPIO1D2_UART2_TXM0 << GPIO1D2_SHIFT);
	/* Set channel C as UART2 input */
	rk_clrsetreg(&grf->iofunc_con0,
		     CON_IOMUX_UART2SEL_MASK,
		     CON_IOMUX_UART2SEL_M0 << CON_IOMUX_UART2SEL_SHIFT);
}

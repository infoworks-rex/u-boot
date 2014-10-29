/* Copyright 2013 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:    GPL-2.0+
 */

#include <common.h>
#include <malloc.h>
#include <ns16550.h>
#include <nand.h>
#include <i2c.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <spi_flash.h>
#include <asm/mpc85xx_gpio.h>

DECLARE_GLOBAL_DATA_PTR;

phys_size_t get_effective_memsize(void)
{
	return CONFIG_SYS_L3_SIZE;
}

unsigned long get_board_sys_clk(void)
{
	return CONFIG_SYS_CLK_FREQ;
}

unsigned long get_board_ddr_clk(void)
{
	return CONFIG_DDR_CLK_FREQ;
}

#define FSL_CORENET_CCSR_PORSR1_RCW_MASK	0xFF800000
void board_init_f(ulong bootflag)
{
	u32 plat_ratio, sys_clk, uart_clk;
#if defined(CONFIG_SPL_NAND_BOOT) && defined(CONFIG_A008044_WORKAROUND)
	u32 porsr1, pinctl;
#endif
	ccsr_gur_t *gur = (void *)CONFIG_SYS_MPC85xx_GUTS_ADDR;

#if defined(CONFIG_SPL_NAND_BOOT) && defined(CONFIG_A008044_WORKAROUND)
	/*
	 * There is T1040 SoC issue where NOR, FPGA are inaccessible during
	 * NAND boot because IFC signals > IFC_AD7 are not enabled.
	 * This workaround changes RCW source to make all signals enabled.
	 */
	porsr1 = in_be32(&gur->porsr1);
	pinctl = ((porsr1 & ~(FSL_CORENET_CCSR_PORSR1_RCW_MASK)) | 0x24800000);
	out_be32((unsigned int *)(CONFIG_SYS_DCSRBAR + 0x20000), pinctl);
#endif

	/* Memcpy existing GD at CONFIG_SPL_GD_ADDR */
	memcpy((void *)CONFIG_SPL_GD_ADDR, (void *)gd, sizeof(gd_t));

	/* Update GD pointer */
	gd = (gd_t *)(CONFIG_SPL_GD_ADDR);

#ifdef CONFIG_DEEP_SLEEP
	/* disable the console if boot from deep sleep */
	if (in_be32(&gur->scrtsr[0]) & (1 << 3))
		gd->flags |= GD_FLG_SILENT | GD_FLG_DISABLE_CONSOLE;
#endif
	/* compiler optimization barrier needed for GCC >= 3.4 */
	__asm__ __volatile__("" : : : "memory");

	console_init_f();

	/* initialize selected port with appropriate baud rate */
	sys_clk = get_board_sys_clk();
	plat_ratio = (in_be32(&gur->rcwsr[0]) >> 25) & 0x1f;
	uart_clk = sys_clk * plat_ratio / 2;

	NS16550_init((NS16550_t)CONFIG_SYS_NS16550_COM1,
		     uart_clk / 16 / CONFIG_BAUDRATE);

	relocate_code(CONFIG_SPL_RELOC_STACK, (gd_t *)CONFIG_SPL_GD_ADDR, 0x0);
}

void board_init_r(gd_t *gd, ulong dest_addr)
{
	bd_t *bd;

	bd = (bd_t *)(gd + sizeof(gd_t));
	memset(bd, 0, sizeof(bd_t));
	gd->bd = bd;
	bd->bi_memstart = CONFIG_SYS_INIT_L3_ADDR;
	bd->bi_memsize = CONFIG_SYS_L3_SIZE;

	probecpu();
	get_clocks();
	mem_malloc_init(CONFIG_SPL_RELOC_MALLOC_ADDR,
			CONFIG_SPL_RELOC_MALLOC_SIZE);

#ifdef CONFIG_SPL_MMC_BOOT
	mmc_initialize(bd);
#endif

	/* relocate environment function pointers etc. */
#ifdef CONFIG_SPL_NAND_BOOT
	nand_spl_load_image(CONFIG_ENV_OFFSET, CONFIG_ENV_SIZE,
			    (uchar *)CONFIG_ENV_ADDR);
#endif
#ifdef CONFIG_SPL_MMC_BOOT
	mmc_spl_load_image(CONFIG_ENV_OFFSET, CONFIG_ENV_SIZE,
			   (uchar *)CONFIG_ENV_ADDR);
#endif
#ifdef CONFIG_SPL_SPI_BOOT
	spi_spl_load_image(CONFIG_ENV_OFFSET, CONFIG_ENV_SIZE,
			   (uchar *)CONFIG_ENV_ADDR);
#endif
	gd->env_addr  = (ulong)(CONFIG_ENV_ADDR);
	gd->env_valid = 1;

	i2c_init_all();

	puts("\n\n");

	gd->ram_size = initdram(0);

#ifdef CONFIG_SPL_MMC_BOOT
	mmc_boot();
#elif defined(CONFIG_SPL_SPI_BOOT)
	spi_boot();
#elif defined(CONFIG_SPL_NAND_BOOT)
	nand_boot();
#endif
}

#ifdef CONFIG_DEEP_SLEEP
void board_mem_sleep_setup(void)
{
	void __iomem *cpld_base = (void *)CONFIG_SYS_CPLD_BASE;

	/* does not provide HW signals for power management */
	clrbits_8(cpld_base + 0x17, 0x40);
	/* Disable MCKE isolation */
	gpio_set_value(2, 0);
	udelay(1);
}
#endif

/*
 * (C) Copyright 2005, Psyent Corporation <www.psyent.com>
 * Scott McNutt <smcnutt@psyent.com>
 * (C) Copyright 2010, Thomas Chou <thomas@wytron.com.tw>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <netdev.h>
#if defined(CONFIG_CFI_FLASH_MTD)
#include <mtd/cfi_flash.h>
#endif
#include <asm/io.h>
#include <asm/gpio.h>

#if defined(CONFIG_ENV_IS_IN_FLASH) && defined(CONFIG_ENV_ADDR) && \
    defined(CONFIG_CFI_FLASH_MTD)
static void __early_flash_cmd_reset(void)
{
	/* reset flash before we read env */
	writeb(AMD_CMD_RESET, CONFIG_ENV_ADDR);
	writeb(FLASH_CMD_RESET, CONFIG_ENV_ADDR);
}
void early_flash_cmd_reset(void)
	__attribute__((weak,alias("__early_flash_cmd_reset")));
#endif

int board_early_init_f(void)
{
#if defined(CONFIG_ENV_IS_IN_FLASH) && defined(CONFIG_ENV_ADDR) && \
    defined(CONFIG_CFI_FLASH_MTD)
	early_flash_cmd_reset();
#endif
	return 0;
}

int checkboard(void)
{
#ifdef CONFIG_ALTERA_SYSID
	display_sysid();
#endif
	printf("BOARD: %s\n", CONFIG_BOARD_NAME);
	return 0;
}

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_SMC91111
	rc += smc91111_initialize(0, CONFIG_SMC91111_BASE);
#endif
#ifdef CONFIG_DRIVER_DM9000
	rc += dm9000_initialize(bis);
#endif
#ifdef CONFIG_ALTERA_TSE
	rc += altera_tse_initialize(0,
				    CONFIG_SYS_ALTERA_TSE_MAC_BASE,
				    CONFIG_SYS_ALTERA_TSE_SGDMA_RX_BASE,
				    CONFIG_SYS_ALTERA_TSE_SGDMA_TX_BASE,
#if defined(CONFIG_SYS_ALTERA_TSE_SGDMA_DESC_BASE) && \
	(CONFIG_SYS_ALTERA_TSE_SGDMA_DESC_SIZE > 0)
				    CONFIG_SYS_ALTERA_TSE_SGDMA_DESC_BASE,
				    CONFIG_SYS_ALTERA_TSE_SGDMA_DESC_SIZE);
#else
				    0,
				    0);
#endif
#endif
#ifdef CONFIG_ETHOC
	rc += ethoc_initialize(0, CONFIG_SYS_ETHOC_BASE);
#endif
	return rc;
}
#endif

/*
 * (C) Copyright 2005, Psyent Corporation <www.psyent.com>
 * Scott McNutt <smcnutt@psyent.com>
 * (C) Copyright 2010, Thomas Chou <thomas@wytron.com.tw>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * BOARD/CPU
 */
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO_LATE

/*
 * SERIAL
 */
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_CONSOLE_INFO_QUIET	/* Suppress console info */

/*
 * CFI Flash
 */
#define CONFIG_SYS_FLASH_BASE		0xe0000000
#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_SYS_CFI_FLASH_STATUS_POLL /* fix amd flash issue */
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE
#define CONFIG_SYS_FLASH_PROTECTION
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_MAX_FLASH_SECT	512

/*
 * MII/PHY
 */
#define CONFIG_CMD_MII			1
#define CONFIG_PHY_GIGE			1
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN	1
#define CONFIG_PHY_MARVELL		1

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME

/*
 * FDT options
 */
#define CONFIG_OF_LIBFDT
#define CONFIG_OF_BOARD_SETUP
#define CONFIG_LMB

/*
 * ENVIRONMENT -- Put environment in sector CONFIG_SYS_MONITOR_LEN above
 * CONFIG_SYS_RESET_ADDR, since we assume the monitor is stored at the
 * reset address, no? This will keep the environment in user region
 * of flash. NOTE: the monitor length must be multiple of sector size
 * (which is common practice).
 */
#define CONFIG_ENV_IS_IN_FLASH

#define CONFIG_ENV_SIZE			0x20000	/* 128k, 1 sector */
#define CONFIG_ENV_OVERWRITE		/* Serial change Ok	*/
#define CONFIG_ENV_ADDR			0xe2840000

/*
 * MEMORY ORGANIZATION
 * -Monitor at top of sdram.
 * -The heap is placed below the monitor
 * -The stack is placed below the heap (&grows down).
 */
#define CONFIG_SYS_SDRAM_BASE		0xD0000000
#define CONFIG_SYS_SDRAM_SIZE		0x08000000
#define CONFIG_MONITOR_IS_IN_RAM
#define CONFIG_SYS_MONITOR_LEN		0x40000	/* Reserve 256k */
#define CONFIG_SYS_MONITOR_BASE	(CONFIG_SYS_SDRAM_BASE + \
					 CONFIG_SYS_SDRAM_SIZE - \
					 CONFIG_SYS_MONITOR_LEN)
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 0x20000)
#define CONFIG_SYS_MALLOC_BASE		(CONFIG_SYS_MONITOR_BASE - \
					 CONFIG_SYS_MALLOC_LEN)
#define CONFIG_SYS_INIT_SP		CONFIG_SYS_MALLOC_BASE

/*
 * MISC
 */
#define CONFIG_SYS_LONGHELP		/* Provide extended help */
#define CONFIG_SYS_CBSIZE		256	/* Console I/O buf size */
#define CONFIG_SYS_MAXARGS		16	/* Max command args	*/
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE /* Bootarg buf size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT) + \
					 16)	/* Print buf size */
#define CONFIG_SYS_LOAD_ADDR		CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_INIT_SP - 0x20000)
#define CONFIG_CMDLINE_EDITING
#define CONFIG_CMD_GPIO

#endif /* __CONFIG_H */

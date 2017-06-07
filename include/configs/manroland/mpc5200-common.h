/*
 * (C) Copyright 2009
 * Heiko Schocher, DENX Software Engineering, hs@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __MANROLAND_MPC52XX__COMMON_H
#define __MANROLAND_MPC52XX__COMMON_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_MPC5200		1	/* MPC5200 CPU */

/* ... running at 33.000000MHz	*/
#define CONFIG_SYS_MPC5XXX_CLKIN	33000000

#define CONFIG_HIGH_BATS	1	/* High BATs supported		*/

/*
 * Serial console configuration
 */
#define CONFIG_PSC_CONSOLE	1	/* console is on PSC1	*/
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200,\
					 230400 }

#if (CONFIG_SYS_TEXT_BASE == 0xFFF00000) /* Boot low */
#   define CONFIG_SYS_LOWBOOT		1
#endif

/*
 * IPB Bus clocking configuration.
 */
#define CONFIG_SYS_IPBCLK_EQUALS_XLBCLK		/* define for 133MHz speed */

/*
 * Flash configuration
 */
#define CONFIG_SYS_FLASH_BASE		0xFF800000

#define CONFIG_SYS_FLASH_SIZE		0x00800000 /* 8 MByte */

#define CONFIG_ENV_ADDR	(CONFIG_SYS_TEXT_BASE+0x40000) /* second sector */
#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max num of flash banks
					   (= chip selects) */
#define CONFIG_SYS_FLASH_ERASE_TOUT	240000	/* Flash Erase Timeout [ms]*/
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Flash Write Timeout [ms]*/

#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_SYS_FLASH_EMPTY_INFO
#define CONFIG_SYS_FLASH_CFI_AMD_RESET

/*
 * Environment settings
 */
#define CONFIG_ENV_IS_IN_FLASH	1
#define CONFIG_ENV_SIZE		0x4000
#define CONFIG_ENV_OFFSET_REDUND   (CONFIG_ENV_OFFSET+CONFIG_ENV_SECT_SIZE)
#define CONFIG_ENV_SIZE_REDUND     (CONFIG_ENV_SIZE)

/*
 * Memory map
 */
#define CONFIG_SYS_MBAR		0xF0000000
#define CONFIG_SYS_DEFAULT_MBAR	0x80000000

#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_SIZE -\
					 GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

#define CONFIG_SYS_SDRAM_BASE		0x00000000
#define CONFIG_SYS_SRAM_BASE	0x80100000	/* CS 1 */
#define CONFIG_SYS_DISPLAY_BASE	0x80600000	/* CS 3 */

/* Settings for XLB = 132 MHz */
#define SDRAM_DDR	 1
#define SDRAM_MODE      0x018D0000
#define SDRAM_EMODE     0x40090000
#define SDRAM_CONTROL   0x714f0f00
#define SDRAM_CONFIG1   0x73722930
#define SDRAM_CONFIG2   0x47770000
#define SDRAM_TAPDELAY  0x10000000

/* Use ON-Chip SRAM until RAM will be available */
#define CONFIG_SYS_INIT_RAM_ADDR	MPC5XXX_SRAM
#ifdef CONFIG_POST
/* preserve space for the post_word at end of on-chip SRAM */
#define CONFIG_SYS_INIT_RAM_SIZE	MPC5XXX_SRAM_POST_SIZE
#else
#define CONFIG_SYS_INIT_RAM_SIZE	MPC5XXX_SRAM_SIZE
#endif

#define CONFIG_SYS_MONITOR_BASE    CONFIG_SYS_TEXT_BASE
#if (CONFIG_SYS_MONITOR_BASE < CONFIG_SYS_FLASH_BASE)
#   define CONFIG_SYS_RAMBOOT		1
#endif

#define CONFIG_SYS_MONITOR_LEN		(192 << 10)
#define CONFIG_SYS_MALLOC_LEN		(512 << 10)
#define CONFIG_SYS_BOOTMAPSZ		(8 << 20)

/*
 * Ethernet configuration
 */
#define CONFIG_MPC5xxx_FEC	1
#define CONFIG_MPC5xxx_FEC_MII100
#define CONFIG_PHY_ADDR		0x00
#define CONFIG_MII		1

/*use  Hardware WDT */
#define CONFIG_HW_WATCHDOG

#define CONFIG_SYS_CACHELINE_SIZE	32	/* For MPC5xxx CPUs	*/
#if defined(CONFIG_CMD_KGDB)
#  define CONFIG_SYS_CACHELINE_SHIFT	5 /* log base 2 of the above value */
#endif

/*
 * Various low-level settings
 */
#define CONFIG_SYS_HID0_INIT		HID0_ICE | HID0_ICFI
#define CONFIG_SYS_HID0_FINAL		HID0_ICE

#define CONFIG_SYS_BOOTCS_START	CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_BOOTCS_SIZE		CONFIG_SYS_FLASH_SIZE
#define CONFIG_SYS_CS0_START		CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_CS0_SIZE		CONFIG_SYS_FLASH_SIZE

/* 8Mbit SRAM @0x80100000 */
#define CONFIG_SYS_CS1_START		CONFIG_SYS_SRAM_BASE

#define CONFIG_SYS_CS_BURST		0x00000000
#define CONFIG_SYS_CS_DEADCYCLE	0x33333333

/*-----------------------------------------------------------------------
 * IDE/ATA stuff Supports IDE harddisk
 *-----------------------------------------------------------------------
 */

#undef	CONFIG_IDE_LED			/* LED   for ide not supported	*/

#define CONFIG_SYS_IDE_MAXBUS		1	/* max. 1 IDE bus	*/

#define CONFIG_IDE_PREINIT	1

#define CONFIG_SYS_ATA_IDE0_OFFSET	0x0000

#define CONFIG_SYS_ATA_BASE_ADDR	MPC5XXX_ATA

/* Offset for data I/O			*/
#define CONFIG_SYS_ATA_DATA_OFFSET	(0x0060)

/* Offset for normal register accesses	*/
#define CONFIG_SYS_ATA_REG_OFFSET	(CONFIG_SYS_ATA_DATA_OFFSET)

/* Offset for alternate registers	*/
#define CONFIG_SYS_ATA_ALT_OFFSET	(0x005C)

/* Interval between registers	*/
#define CONFIG_SYS_ATA_STRIDE          4

#define CONFIG_ATAPI            1

#define OF_CPU			"PowerPC,5200@0"
#define OF_SOC			"soc5200@f0000000"
#define OF_TBCLK		(bd->bi_busfreq / 4)
#define OF_STDOUT_PATH		"/soc5200@f0000000/serial@2000"
#define CONFIG_OF_IDE_FIXUP

#endif /* __MANROLAND_MPC52XX__COMMON_H */

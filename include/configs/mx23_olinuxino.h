/*
 * Copyright (C) 2013 Marek Vasut <marex@denx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef __MX23_OLINUXINO_CONFIG_H__
#define __MX23_OLINUXINO_CONFIG_H__

/*
 * SoC configurations
 */
#define	CONFIG_MX23				/* i.MX23 SoC */
#define	CONFIG_MXS_GPIO				/* GPIO control */
#define	CONFIG_SYS_HZ		1000		/* Ticks per second */

#define	CONFIG_MACH_TYPE	4105

#include <asm/arch/regs-base.h>

#define	CONFIG_SYS_NO_FLASH
#define	CONFIG_BOARD_EARLY_INIT_F
#define	CONFIG_ARCH_MISC_INIT

/*
 * SPL
 */
#define	CONFIG_SPL
#define	CONFIG_SPL_NO_CPU_SUPPORT_CODE
#define	CONFIG_SPL_START_S_PATH		"arch/arm/cpu/arm926ejs/mxs"
#define	CONFIG_SPL_LDSCRIPT	"arch/arm/cpu/arm926ejs/mxs/u-boot-spl.lds"
#define	CONFIG_SPL_LIBCOMMON_SUPPORT
#define	CONFIG_SPL_LIBGENERIC_SUPPORT
#define	CONFIG_SPL_GPIO_SUPPORT

/*
 * U-Boot Commands
 */
#include <config_cmd_default.h>
#define	CONFIG_DISPLAY_CPUINFO
#define	CONFIG_DOS_PARTITION

#define	CONFIG_CMD_CACHE
#define	CONFIG_CMD_FAT
#define	CONFIG_CMD_GPIO

/*
 * Memory configurations
 */
#define	CONFIG_NR_DRAM_BANKS		1		/* 1 bank of DRAM */
#define	PHYS_SDRAM_1			0x40000000	/* Base address */
#define	PHYS_SDRAM_1_SIZE		0x08000000	/* Max 128 MB RAM */
#define	CONFIG_SYS_MALLOC_LEN		0x00400000	/* 4 MB for malloc */
#define	CONFIG_SYS_GBL_DATA_SIZE	128		/* Initial data */
#define	CONFIG_SYS_MEMTEST_START	0x40000000	/* Memtest start adr */
#define	CONFIG_SYS_MEMTEST_END		0x40400000	/* 4 MB RAM test */
#define	CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1
/* Point initial SP in SRAM so SPL can use it too. */

#define CONFIG_SYS_INIT_RAM_ADDR	0x00000000
#define CONFIG_SYS_INIT_RAM_SIZE	(128 * 1024)

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)
/*
 * We need to sacrifice first 4 bytes of RAM here to avoid triggering some
 * strange BUG in ROM corrupting first 4 bytes of RAM when loading U-Boot
 * binary. In case there was more of this mess, 0x100 bytes are skipped.
 */
#define	CONFIG_SYS_TEXT_BASE		0x40000100

/*
 * U-Boot general configurations
 */
#define	CONFIG_SYS_LONGHELP
#define	CONFIG_SYS_PROMPT	"=> "
#define	CONFIG_SYS_CBSIZE	1024		/* Console I/O buffer size */
#define	CONFIG_SYS_PBSIZE	\
	(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
						/* Print buffer size */
#define	CONFIG_SYS_MAXARGS	32		/* Max number of command args */
#define	CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE
						/* Boot argument buffer size */
#define	CONFIG_VERSION_VARIABLE			/* U-BOOT version */
#define	CONFIG_AUTO_COMPLETE			/* Command auto complete */
#define	CONFIG_CMDLINE_EDITING			/* Command history etc */
#define	CONFIG_SYS_HUSH_PARSER

/*
 * Serial Driver
 */
#define	CONFIG_PL011_SERIAL
#define	CONFIG_PL011_CLOCK		24000000
#define	CONFIG_PL01x_PORTS		{ (void *)MXS_UARTDBG_BASE }
#define	CONFIG_CONS_INDEX		0
#define	CONFIG_BAUDRATE			115200	/* Default baud rate */

/*
 * APBH DMA
 */
#define CONFIG_APBH_DMA

/*
 * ENV -- nowhere so far
 */
#define	CONFIG_ENV_SIZE			(16 * 1024)
#define	CONFIG_ENV_IS_NOWHERE

/*
 * Boot Linux
 */
#define	CONFIG_CMDLINE_TAG
#define	CONFIG_SETUP_MEMORY_TAGS
#define	CONFIG_BOOTDELAY	3
#define	CONFIG_BOOTFILE		"uImage"
#define	CONFIG_BOOTARGS		"console=ttyAMA0,115200n8 "
#define	CONFIG_LOADADDR		0x42000000
#define	CONFIG_SYS_LOAD_ADDR	CONFIG_LOADADDR
#define	CONFIG_OF_LIBFDT

#endif /* __MX23_OLINUXINO_CONFIG_H__ */

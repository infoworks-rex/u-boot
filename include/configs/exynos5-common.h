/*
 * Copyright (C) 2013 Samsung Electronics
 *
 * Configuration settings for the SAMSUNG EXYNOS5 board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_EXYNOS5_COMMON_H
#define __CONFIG_EXYNOS5_COMMON_H

#define CONFIG_EXYNOS5			/* Exynos5 Family */

#include "exynos-common.h"

#define CONFIG_SYS_CACHELINE_SIZE	64
#define CONFIG_ARCH_EARLY_INIT_R
#define CONFIG_EXYNOS_SPL

/* Allow tracing to be enabled */
#define CONFIG_TRACE
#define CONFIG_CMD_TRACE
#define CONFIG_TRACE_BUFFER_SIZE	(16 << 20)
#define CONFIG_TRACE_EARLY_SIZE		(8 << 20)
#define CONFIG_TRACE_EARLY
#define CONFIG_TRACE_EARLY_ADDR		0x50000000


/* Enable ACE acceleration for SHA1 and SHA256 */
#define CONFIG_EXYNOS_ACE_SHA
#define CONFIG_SHA_HW_ACCEL

/* Power Down Modes */
#define S5P_CHECK_SLEEP			0x00000BAD
#define S5P_CHECK_DIDLE			0xBAD00000
#define S5P_CHECK_LPA			0xABAD0000

/* Offset for inform registers */
#define INFORM0_OFFSET			0x800
#define INFORM1_OFFSET			0x804
#define INFORM2_OFFSET			0x808
#define INFORM3_OFFSET			0x80c

/* select serial console configuration */
#define CONFIG_BAUDRATE			115200
#define EXYNOS5_DEFAULT_UART_OFFSET	0x010000
#define CONFIG_SILENT_CONSOLE
#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#define CONFIG_CONSOLE_MUX

#define EXYNOS_DEVICE_SETTINGS \
		"stdin=serial\0" \
		"stdout=serial\0" \
		"stderr=serial\0"

#define CONFIG_EXTRA_ENV_SETTINGS \
	EXYNOS_DEVICE_SETTINGS

#define CONFIG_CMD_PING
#define CONFIG_CMD_ELF
#define CONFIG_CMD_NET
#define CONFIG_CMD_HASH

/* Thermal Management Unit */
#define CONFIG_EXYNOS_TMU
#define CONFIG_CMD_DTT
#define CONFIG_TMU_CMD_DTT

/* TPM */
#define CONFIG_TPM
#define CONFIG_CMD_TPM
#define CONFIG_TPM_TIS_I2C
#define CONFIG_TPM_TIS_I2C_BUS_NUMBER	3
#define CONFIG_TPM_TIS_I2C_SLAVE_ADDR	0x20

/* MMC SPL */
#define COPY_BL2_FNPTR_ADDR	0x02020030
#define CONFIG_SUPPORT_EMMC_BOOT

#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_GPIO_SUPPORT

/* specific .lds file */
#define CONFIG_SPL_LDSCRIPT	"board/samsung/common/exynos-uboot-spl.lds"

/* Miscellaneous configurable options */
#define CONFIG_DEFAULT_CONSOLE		"console=ttySAC1,115200n8\0"
/* Boot Argument Buffer Size */
/* memtest works on */
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_SDRAM_BASE + 0x5E00000)
#define CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_SDRAM_BASE + 0x3E00000)

#define CONFIG_RD_LVL

#define PHYS_SDRAM_1		CONFIG_SYS_SDRAM_BASE
#define PHYS_SDRAM_1_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_2		(CONFIG_SYS_SDRAM_BASE + SDRAM_BANK_SIZE)
#define PHYS_SDRAM_2_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_3		(CONFIG_SYS_SDRAM_BASE + (2 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_3_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_4		(CONFIG_SYS_SDRAM_BASE + (3 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_4_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_5		(CONFIG_SYS_SDRAM_BASE + (4 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_5_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_6		(CONFIG_SYS_SDRAM_BASE + (5 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_6_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_7		(CONFIG_SYS_SDRAM_BASE + (6 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_7_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_8		(CONFIG_SYS_SDRAM_BASE + (7 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_8_SIZE	SDRAM_BANK_SIZE

#define CONFIG_SYS_MONITOR_BASE	0x00000000

#define CONFIG_SYS_MMC_ENV_DEV		0

#define CONFIG_SECURE_BL1_ONLY

/* Secure FW size configuration */
#ifdef CONFIG_SECURE_BL1_ONLY
#define CONFIG_SEC_FW_SIZE (8 << 10) /* 8KB */
#else
#define CONFIG_SEC_FW_SIZE 0
#endif

/* Configuration of BL1, BL2, ENV Blocks on mmc */
#define CONFIG_RES_BLOCK_SIZE	(512)
#define CONFIG_BL1_SIZE	(16 << 10) /*16 K reserved for BL1*/
#define CONFIG_BL2_SIZE	(512UL << 10UL) /* 512 KB */
#define CONFIG_ENV_SIZE	(16 << 10) /* 16 KB */

#define CONFIG_BL1_OFFSET	(CONFIG_RES_BLOCK_SIZE + CONFIG_SEC_FW_SIZE)
#define CONFIG_BL2_OFFSET	(CONFIG_BL1_OFFSET + CONFIG_BL1_SIZE)

/* Store environment at the end of a 4 MB SPI flash */
#define FLASH_SIZE		(0x4 << 20)
#define CONFIG_ENV_OFFSET	(FLASH_SIZE - CONFIG_BL2_SIZE)

/* U-boot copy size from boot Media to DRAM.*/
#define BL2_START_OFFSET	(CONFIG_BL2_OFFSET/512)
#define BL2_SIZE_BLOC_COUNT	(CONFIG_BL2_SIZE/512)

#define CONFIG_SPI_BOOTING
#define EXYNOS_COPY_SPI_FNPTR_ADDR	0x02020058
#define SPI_FLASH_UBOOT_POS	(CONFIG_SEC_FW_SIZE + CONFIG_BL1_SIZE)

/* I2C */
#define CONFIG_SYS_I2C_INIT_BOARD
#define CONFIG_SYS_I2C
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C_S3C24X0_SPEED	100000		/* 100 Kbps */
#define CONFIG_SYS_I2C_S3C24X0
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_SYS_I2C_S3C24X0_SLAVE    0x0
#define CONFIG_I2C_EDID

/* SPI */
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_SPI_FLASH
#define CONFIG_ENV_SPI_BASE	0x12D30000

#ifdef CONFIG_SPI_FLASH
#define CONFIG_EXYNOS_SPI
#define CONFIG_CMD_SF
#define CONFIG_CMD_SPI
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_SPI_FLASH_GIGADEVICE
#define CONFIG_SF_DEFAULT_MODE		SPI_MODE_0
#define CONFIG_SF_DEFAULT_SPEED		50000000
#define EXYNOS5_SPI_NUM_CONTROLLERS	5
#define CONFIG_OF_SPI
#endif

#ifdef CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_SPI_MODE	SPI_MODE_0
#define CONFIG_ENV_SECT_SIZE	CONFIG_ENV_SIZE
#define CONFIG_ENV_SPI_BUS	1
#define CONFIG_ENV_SPI_MAX_HZ	50000000
#endif

/* Ethernet Controllor Driver */
#ifdef CONFIG_CMD_NET
#define CONFIG_SMC911X
#define CONFIG_SMC911X_BASE		0x5000000
#define CONFIG_SMC911X_16_BIT
#define CONFIG_ENV_SROM_BANK		1
#endif /*CONFIG_CMD_NET*/

/* Enable PXE Support */
#ifdef CONFIG_CMD_NET
#define CONFIG_CMD_PXE
#define CONFIG_MENU
#endif

/* SHA hashing */
#define CONFIG_CMD_HASH
#define CONFIG_HASH_VERIFY
#define CONFIG_SHA1
#define CONFIG_SHA256

/* Enable Time Command */
#define CONFIG_CMD_TIME

#define CONFIG_CMD_BOOTZ

#define CONFIG_CMD_GPIO

/* USB boot mode */
#define CONFIG_USB_BOOTING
#define EXYNOS_COPY_USB_FNPTR_ADDR	0x02020070
#define EXYNOS_USB_SECONDARY_BOOT	0xfeed0002
#define EXYNOS_IRAM_SECONDARY_BASE	0x02020018

/* Enable FIT support and comparison */
#define CONFIG_FIT
#define CONFIG_FIT_BEST_MATCH

#endif	/* __CONFIG_EXYNOS5_COMMON_H */

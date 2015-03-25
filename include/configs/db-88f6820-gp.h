/*
 * Copyright (C) 2014 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _CONFIG_DB_88F6820_GP_H
#define _CONFIG_DB_88F6820_GP_H

/*
 * High Level Configuration Options (easy to change)
 */
#define CONFIG_ARMADA_XP		/* SOC Family Name */
#define CONFIG_ARMADA_38X
#define CONFIG_DB_88F6820_GP		/* Board target name for DDR training */

#define CONFIG_SYS_L2_PL310

#define CONFIG_SKIP_LOWLEVEL_INIT	/* disable board lowlevel_init */
#define CONFIG_SYS_GENERIC_BOARD
#define CONFIG_DISPLAY_BOARDINFO_LATE

#define	CONFIG_SYS_TEXT_BASE	0x04000000
#define CONFIG_SYS_TCLK		250000000	/* 250MHz */

/*
 * Commands configuration
 */
#define CONFIG_SYS_NO_FLASH		/* Declare no flash (NOR/SPI) */
#define CONFIG_CMD_CACHE
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ENV
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_FAT
#define CONFIG_CMD_FS_GENERIC
#define CONFIG_CMD_I2C
#define CONFIG_CMD_MMC
#define CONFIG_CMD_PING
#define CONFIG_CMD_SCSI
#define CONFIG_CMD_SF
#define CONFIG_CMD_SPI
#define CONFIG_CMD_TFTPPUT
#define CONFIG_CMD_TIME
#define CONFIG_CMD_USB

/* I2C */
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MVTWSI
#define CONFIG_I2C_MVTWSI_BASE0		MVEBU_TWSI_BASE
#define CONFIG_SYS_I2C_SLAVE		0x0
#define CONFIG_SYS_I2C_SPEED		100000

/* SPI NOR flash default params, used by sf commands */
#define CONFIG_SF_DEFAULT_SPEED		1000000
#define CONFIG_SF_DEFAULT_MODE		SPI_MODE_3
#define CONFIG_SPI_FLASH_STMICRO

/*
 * SDIO/MMC Card Configuration
 */
#define CONFIG_MMC
#define CONFIG_MMC_SDMA
#define CONFIG_GENERIC_MMC
#define CONFIG_SDHCI
#define CONFIG_MV_SDHCI
#define CONFIG_SYS_MMC_BASE		MVEBU_SDIO_BASE

/*
 * SATA/SCSI/AHCI configuration
 */
#define CONFIG_LIBATA
#define CONFIG_SCSI_AHCI
#define CONFIG_SCSI_AHCI_PLAT
#define CONFIG_SYS_SCSI_MAX_SCSI_ID	2
#define CONFIG_SYS_SCSI_MAX_LUN		1
#define CONFIG_SYS_SCSI_MAX_DEVICE	(CONFIG_SYS_SCSI_MAX_SCSI_ID * \
					 CONFIG_SYS_SCSI_MAX_LUN)

/* Partition support */
#define CONFIG_DOS_PARTITION
#define CONFIG_EFI_PARTITION

/* Additional FS support/configuration */
#define CONFIG_SUPPORT_VFAT

/* USB/EHCI configuration */
#define CONFIG_USB_EHCI
#define CONFIG_USB_STORAGE
#define CONFIG_USB_EHCI_MARVELL
#define CONFIG_EHCI_IS_TDI

/* Environment in SPI NOR flash */
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_OFFSET		(1 << 20) /* 1MiB in */
#define CONFIG_ENV_SIZE			(64 << 10) /* 64KiB */
#define CONFIG_ENV_SECT_SIZE		(256 << 10) /* 256KiB sectors */

#define CONFIG_PHY_MARVELL		/* there is a marvell phy */
#define CONFIG_PHY_ADDR			{ 1, 0 }
#define CONFIG_SYS_NETA_INTERFACE_TYPE	PHY_INTERFACE_MODE_RGMII
#define PHY_ANEG_TIMEOUT	8000	/* PHY needs a longer aneg time */

#define CONFIG_SYS_CONSOLE_INFO_QUIET	/* don't print console @ startup */
#define CONFIG_SYS_ALT_MEMTEST

/* Keep device tree and initrd in lower memory so the kernel can access them */
#define CONFIG_EXTRA_ENV_SETTINGS	\
	"fdt_high=0x10000000\0"		\
	"initrd_high=0x10000000\0"

/* SPL */
/* Defines for SPL */
#define CONFIG_SPL_FRAMEWORK
#define CONFIG_SPL_SIZE			(140 << 10)
#define CONFIG_SPL_TEXT_BASE		0x40000030
#define CONFIG_SPL_MAX_SIZE		(CONFIG_SPL_SIZE - 0x0030)

#define CONFIG_SPL_BSS_START_ADDR	(0x40000000 + CONFIG_SPL_SIZE)
#define CONFIG_SPL_BSS_MAX_SIZE		(16 << 10)

#define CONFIG_SYS_SPL_MALLOC_START	(CONFIG_SPL_BSS_START_ADDR + \
					 CONFIG_SPL_BSS_MAX_SIZE)
#define CONFIG_SYS_SPL_MALLOC_SIZE	(16 << 10)

#define CONFIG_SPL_STACK		(0x40000000 + ((192 - 16) << 10))
#define CONFIG_SPL_BOOTROM_SAVE		(CONFIG_SPL_STACK + 4)

#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_LIBGENERIC_SUPPORT
#define CONFIG_SPL_SERIAL_SUPPORT
#define CONFIG_SPL_I2C_SUPPORT

/* SPL related SPI defines */
#define CONFIG_SPL_SPI_SUPPORT
#define CONFIG_SPL_SPI_FLASH_SUPPORT
#define CONFIG_SPL_SPI_LOAD
#define CONFIG_SPL_SPI_BUS		0
#define CONFIG_SPL_SPI_CS		0
#define CONFIG_SYS_SPI_U_BOOT_OFFS	0x20000

/* Enable DDR support in SPL (DDR3 training from Marvell bin_hdr) */
#define CONFIG_SYS_MVEBU_DDR_A38X
#define CONFIG_DDR3

/*
 * mv-common.h should be defined after CMD configs since it used them
 * to enable certain macros
 */
#include "mv-common.h"

#endif /* _CONFIG_DB_88F6820_GP_H */

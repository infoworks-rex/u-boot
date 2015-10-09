/*
 * include/configs/gose.h
 *
 * Copyright (C) 2014 Renesas Electronics Corporation
 *
 * SPDX-License-Identifier: GPL-2.0
 */

#ifndef __GOSE_H
#define __GOSE_H

#undef DEBUG
#define CONFIG_R8A7793
#define CONFIG_ARCH_RMOBILE_BOARD_STRING "Gose"

#include "rcar-gen2-common.h"

#if defined(CONFIG_ARCH_RMOBILE_EXTRAM_BOOT)
#define CONFIG_SYS_TEXT_BASE	0x70000000
#else
#define CONFIG_SYS_TEXT_BASE	0xE6304000
#endif

/* STACK */
#if defined(CONFIG_ARCH_RMOBILE_EXTRAM_BOOT)
#define CONFIG_SYS_INIT_SP_ADDR		0x7003FFFC
#else
#define CONFIG_SYS_INIT_SP_ADDR		0xE633FFFC
#endif

#define STACK_AREA_SIZE			0xC000
#define LOW_LEVEL_MERAM_STACK	\
	(CONFIG_SYS_INIT_SP_ADDR + STACK_AREA_SIZE - 4)

/* MEMORY */
#define RCAR_GEN2_SDRAM_BASE		0x40000000
#define RCAR_GEN2_SDRAM_SIZE		0x40000000
#define RCAR_GEN2_UBOOT_SDRAM_SIZE	0x20000000

/* SCIF */
#define CONFIG_SCIF_CONSOLE

/* FLASH */
#define CONFIG_SYS_NO_FLASH
#define CONFIG_SPI
#define CONFIG_SH_QSPI

/* SH Ether */
#define CONFIG_SH_ETHER
#define CONFIG_SH_ETHER_USE_PORT	0
#define CONFIG_SH_ETHER_PHY_ADDR	0x1
#define CONFIG_SH_ETHER_PHY_MODE PHY_INTERFACE_MODE_RMII
#define CONFIG_SH_ETHER_CACHE_WRITEBACK
#define CONFIG_SH_ETHER_CACHE_INVALIDATE
#define CONFIG_PHYLIB
#define CONFIG_PHY_MICREL
#define CONFIG_BITBANGMII
#define CONFIG_BITBANGMII_MULTI
#define CONFIG_SH_ETHER_ALIGNE_SIZE	64

/* Board Clock */
#define RMOBILE_XTAL_CLK	20000000u
#define CONFIG_SYS_CLK_FREQ	RMOBILE_XTAL_CLK
#define CONFIG_SH_TMU_CLK_FREQ	(CONFIG_SYS_CLK_FREQ / 2)
#define CONFIG_SYS_TMU_CLK_DIV	4

/* I2C */
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_SH
#define CONFIG_SYS_I2C_SLAVE	0x7F
#define CONFIG_SYS_I2C_SH_NUM_CONTROLLERS	3
#define CONFIG_SYS_I2C_SH_SPEED0	400000
#define CONFIG_SYS_I2C_SH_SPEED1	400000
#define CONFIG_SYS_I2C_SH_SPEED2	400000
#define CONFIG_SH_I2C_DATA_HIGH	4
#define CONFIG_SH_I2C_DATA_LOW	5
#define CONFIG_SH_I2C_CLOCK	10000000

#define CONFIG_SYS_I2C_POWERIC_ADDR 0x58 /* da9063 */

/* USB */
#define CONFIG_USB_STORAGE
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_RMOBILE
#define CONFIG_USB_MAX_CONTROLLER_COUNT	2

/* Module stop status bits */
/* INTC-RT */
#define CONFIG_SMSTP0_ENA	0x00400000
/* MSIF */
#define CONFIG_SMSTP2_ENA	0x00002000
/* INTC-SYS, IRQC */
#define CONFIG_SMSTP4_ENA	0x00000180
/* SCIF0 */
#define CONFIG_SMSTP7_ENA	0x00200000

/* SDHI */
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_SH_SDHI_FREQ		97500000

#endif	/* __GOSE_H */

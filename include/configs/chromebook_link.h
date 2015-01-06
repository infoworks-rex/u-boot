/*
 * Copyright (c) 2011 The Chromium OS Authors.
 * (C) Copyright 2008
 * Graeme Russ, graeme.russ@gmail.com.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * board/config.h - configuration options, board specific
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <configs/x86-common.h>


#define CONFIG_SYS_MONITOR_LEN			(1 << 20)

#define CONFIG_DCACHE_RAM_MRC_VAR_SIZE		0x4000
#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_DISPLAY_CPUINFO

#define CONFIG_NR_DRAM_BANKS			8
#define CONFIG_X86_MRC_ADDR			0xfffa0000
#define CONFIG_CACHE_MRC_SIZE_KB		512

#define CONFIG_X86_SERIAL

#define CONFIG_SCSI_DEV_LIST		{PCI_VENDOR_ID_INTEL, \
			PCI_DEVICE_ID_INTEL_NM10_AHCI},	      \
	{PCI_VENDOR_ID_INTEL,		\
			PCI_DEVICE_ID_INTEL_COUGARPOINT_AHCI_MOBILE}, \
	{PCI_VENDOR_ID_INTEL, \
			PCI_DEVICE_ID_INTEL_COUGARPOINT_AHCI_SERIES6}, \
	{PCI_VENDOR_ID_INTEL,		\
			PCI_DEVICE_ID_INTEL_PANTHERPOINT_AHCI_MOBILE}

#define CONFIG_X86_OPTION_ROM_FILE		pci8086,0166.bin
#define CONFIG_X86_OPTION_ROM_ADDR		0xfff90000
#define CONFIG_VIDEO_X86

#define CONFIG_PCI_MEM_BUS	0xe0000000
#define CONFIG_PCI_MEM_PHYS	CONFIG_PCI_MEM_BUS
#define CONFIG_PCI_MEM_SIZE	0x10000000

#define CONFIG_PCI_PREF_BUS	0xd0000000
#define CONFIG_PCI_PREF_PHYS	CONFIG_PCI_PREF_BUS
#define CONFIG_PCI_PREF_SIZE	0x10000000

#define CONFIG_PCI_IO_BUS	0x1000
#define CONFIG_PCI_IO_PHYS	CONFIG_PCI_IO_BUS
#define CONFIG_PCI_IO_SIZE	0xefff

#define CONFIG_SYS_EARLY_PCI_INIT
#define CONFIG_PCI_PNP

#define CONFIG_BIOSEMU
#define VIDEO_IO_OFFSET				0
#define CONFIG_X86EMU_RAW_IO

#define CONFIG_CROS_EC
#define CONFIG_CROS_EC_LPC
#define CONFIG_CMD_CROS_EC
#define CONFIG_ARCH_EARLY_INIT_R

#define CONFIG_STD_DEVICES_SETTINGS     "stdin=usbkbd,vga,serial\0" \
					"stdout=vga,serial\0" \
					"stderr=vga,serial\0"

#endif	/* __CONFIG_H */

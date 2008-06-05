/*
 * Copyright 2008 Freescale Semiconductor, Inc.
 *
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/fsl_law.h>
#include <asm/mmu.h>

/*
 * LAW(Local Access Window) configuration:
 *
 * 0x0000_0000	   0x7fff_ffff	   DDR			   2G
 * 0x8000_0000	   0x9fff_ffff	   PCI1 MEM		   512M
 * 0xc000_0000	   0xdfff_ffff	   RapidIO or PCI express  512M
 * 0xe000_0000	   0xe000_ffff	   CCSR			   1M
 * 0xe200_0000	   0xe2ff_ffff	   PCI1 IO		   16M
 * 0xe300_0000	   0xe3ff_ffff	   CAN			   16M
 * 0xef00_0000	   0xefff_ffff     PCI express IO          16M
 * 0xfe00_0000	   0xffff_ffff	   FLASH (boot bank)	   32M
 *
 * Notes:
 *    CCSRBAR and L2-as-SRAM don't need a configured Local Access Window.
 *    If flash is 8M at default position (last 8M), no LAW needed.
 */

struct law_entry law_table[] = {
	SET_LAW_ENTRY (1, CFG_DDR_SDRAM_BASE, LAW_SIZE_512M, LAW_TRGT_IF_DDR),
	SET_LAW_ENTRY (2, CFG_PCI1_MEM_PHYS, LAW_SIZE_512M, LAW_TRGT_IF_PCI),
	SET_LAW_ENTRY (3, CFG_LBC_FLASH_BASE, LAW_SIZE_128M, LAW_TRGT_IF_LBC),
	SET_LAW_ENTRY (4, CFG_PCI1_IO_PHYS, LAW_SIZE_16M, LAW_TRGT_IF_PCI),
#ifdef CONFIG_PCIE1
	SET_LAW_ENTRY (5, CFG_PCIE1_MEM_BASE, LAW_SIZE_512M, LAW_TRGT_IF_PCIE_1),
#else /* !CONFIG_PCIE1 */
	SET_LAW_ENTRY (5, CFG_RIO_MEM_BASE, LAW_SIZE_512M, LAW_TRGT_IF_RIO),
#endif /* CONFIG_PCIE1 */
#ifdef CONFIG_CAN_DRIVER
	SET_LAW_ENTRY (6, CFG_CAN_BASE, LAW_SIZE_16M, LAW_TRGT_IF_LBC),
#endif /* CONFIG_CAN_DRIVER */
#ifdef CONFIG_PCIE1
	SET_LAW_ENTRY (7, CFG_PCIE1_IO_BASE, LAW_SIZE_16M, LAW_TRGT_IF_PCIE_1),
#endif /* CONFIG_PCIE */
};

int num_law_entries = ARRAY_SIZE (law_table);

/*
 * Copyright 2007,2009 Wind River Systems, Inc. <www.windriver.com>
 *
 * Copyright 2007 Embedded Specialties, Inc.
 *
 * Copyright 2004, 2007 Freescale Semiconductor.
 *
 * (C) Copyright 2002 Scott McNutt <smcnutt@artesyncp.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <pci.h>
#include <asm/processor.h>
#include <asm/immap_85xx.h>
#include <asm/fsl_pci.h>
#include <asm/fsl_ddr_sdram.h>
#include <spd_sdram.h>
#include <netdev.h>
#include <tsec.h>
#include <miiphy.h>
#include <libfdt.h>
#include <fdt_support.h>

DECLARE_GLOBAL_DATA_PTR;

void local_bus_init(void);
void sdram_init(void);
long int fixed_sdram (void);

int board_early_init_f (void)
{
	return 0;
}

int checkboard (void)
{
	volatile ccsr_local_ecm_t *ecm = (void *)(CONFIG_SYS_MPC85xx_ECM_ADDR);
	volatile u_char *rev= (void *)CONFIG_SYS_BD_REV;

	printf ("Board: Wind River SBC8548 Rev. 0x%01x\n",
			in_8(rev) >> 4);

	/*
	 * Initialize local bus.
	 */
	local_bus_init ();

	out_be32(&ecm->eedr, 0xffffffff);	/* clear ecm errors */
	out_be32(&ecm->eeer, 0xffffffff);	/* enable ecm errors */
	return 0;
}

phys_size_t
initdram(int board_type)
{
	long dram_size = 0;

	puts("Initializing\n");

#if defined(CONFIG_DDR_DLL)
	{
		/*
		 * Work around to stabilize DDR DLL MSYNC_IN.
		 * Errata DDR9 seems to have been fixed.
		 * This is now the workaround for Errata DDR11:
		 *    Override DLL = 1, Course Adj = 1, Tap Select = 0
		 */

		volatile ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);

		out_be32(&gur->ddrdllcr, 0x81000000);
		asm("sync;isync;msync");
		udelay(200);
	}
#endif

#if defined(CONFIG_SPD_EEPROM)
	dram_size = fsl_ddr_sdram();
	dram_size = setup_ddr_tlbs(dram_size / 0x100000);
	dram_size *= 0x100000;
#else
	dram_size = fixed_sdram ();
#endif

	/*
	 * SDRAM Initialization
	 */
	sdram_init();

	puts("    DDR: ");
	return dram_size;
}

/*
 * Initialize Local Bus
 */
void
local_bus_init(void)
{
	volatile ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
	volatile ccsr_lbc_t *lbc = (void *)(CONFIG_SYS_MPC85xx_LBC_ADDR);

	uint clkdiv;
	uint lbc_hz;
	sys_info_t sysinfo;

	get_sys_info(&sysinfo);
	clkdiv = (in_be32(&lbc->lcrr) & LCRR_CLKDIV) * 2;
	lbc_hz = sysinfo.freqSystemBus / 1000000 / clkdiv;

	out_be32(&gur->lbiuiplldcr1, 0x00078080);
	if (clkdiv == 16) {
		out_be32(&gur->lbiuiplldcr0, 0x7c0f1bf0);
	} else if (clkdiv == 8) {
		out_be32(&gur->lbiuiplldcr0, 0x6c0f1bf0);
	} else if (clkdiv == 4) {
		out_be32(&gur->lbiuiplldcr0, 0x5c0f1bf0);
	}

	setbits_be32(&lbc->lcrr, 0x00030000);

	asm("sync;isync;msync");

	out_be32(&lbc->ltesr, 0xffffffff);	/* Clear LBC error IRQs */
	out_be32(&lbc->lteir, 0xffffffff);	/* Enable LBC error IRQs */
}

/*
 * Initialize SDRAM memory on the Local Bus.
 */
void
sdram_init(void)
{
#if defined(CONFIG_SYS_OR3_PRELIM) && defined(CONFIG_SYS_BR3_PRELIM)

	uint idx;
	volatile ccsr_lbc_t *lbc = (void *)(CONFIG_SYS_MPC85xx_LBC_ADDR);
	uint *sdram_addr = (uint *)CONFIG_SYS_LBC_SDRAM_BASE;
	uint lsdmr_common;

	puts("    SDRAM: ");

	print_size (CONFIG_SYS_LBC_SDRAM_SIZE * 1024 * 1024, "\n");

	/*
	 * Setup SDRAM Base and Option Registers
	 */
	out_be32(&lbc->or3, CONFIG_SYS_OR3_PRELIM);
	asm("msync");

	out_be32(&lbc->br3, CONFIG_SYS_BR3_PRELIM);
	asm("msync");

	out_be32(&lbc->lbcr, CONFIG_SYS_LBC_LBCR);
	asm("msync");


	out_be32(&lbc->lsrt,  CONFIG_SYS_LBC_LSRT);
	out_be32(&lbc->mrtpr, CONFIG_SYS_LBC_MRTPR);
	asm("msync");

	/*
	 * MPC8548 uses "new" 15-16 style addressing.
	 */
	lsdmr_common = CONFIG_SYS_LBC_LSDMR_COMMON;
	lsdmr_common |= LSDMR_BSMA1516;

	/*
	 * Issue PRECHARGE ALL command.
	 */
	out_be32(&lbc->lsdmr, lsdmr_common | LSDMR_OP_PCHALL);
	asm("sync;msync");
	*sdram_addr = 0xff;
	ppcDcbf((unsigned long) sdram_addr);
	udelay(100);

	/*
	 * Issue 8 AUTO REFRESH commands.
	 */
	for (idx = 0; idx < 8; idx++) {
		out_be32(&lbc->lsdmr, lsdmr_common | LSDMR_OP_ARFRSH);
		asm("sync;msync");
		*sdram_addr = 0xff;
		ppcDcbf((unsigned long) sdram_addr);
		udelay(100);
	}

	/*
	 * Issue 8 MODE-set command.
	 */
	out_be32(&lbc->lsdmr, lsdmr_common | LSDMR_OP_MRW);
	asm("sync;msync");
	*sdram_addr = 0xff;
	ppcDcbf((unsigned long) sdram_addr);
	udelay(100);

	/*
	 * Issue NORMAL OP command.
	 */
	out_be32(&lbc->lsdmr, lsdmr_common | LSDMR_OP_NORMAL);
	asm("sync;msync");
	*sdram_addr = 0xff;
	ppcDcbf((unsigned long) sdram_addr);
	udelay(200);    /* Overkill. Must wait > 200 bus cycles */

#endif	/* enable SDRAM init */
}

#if defined(CONFIG_SYS_DRAM_TEST)
int
testdram(void)
{
	uint *pstart = (uint *) CONFIG_SYS_MEMTEST_START;
	uint *pend = (uint *) CONFIG_SYS_MEMTEST_END;
	uint *p;

	printf("Testing DRAM from 0x%08x to 0x%08x\n",
	       CONFIG_SYS_MEMTEST_START,
	       CONFIG_SYS_MEMTEST_END);

	printf("DRAM test phase 1:\n");
	for (p = pstart; p < pend; p++)
		*p = 0xaaaaaaaa;

	for (p = pstart; p < pend; p++) {
		if (*p != 0xaaaaaaaa) {
			printf ("DRAM test fails at: %08x\n", (uint) p);
			return 1;
		}
	}

	printf("DRAM test phase 2:\n");
	for (p = pstart; p < pend; p++)
		*p = 0x55555555;

	for (p = pstart; p < pend; p++) {
		if (*p != 0x55555555) {
			printf ("DRAM test fails at: %08x\n", (uint) p);
			return 1;
		}
	}

	printf("DRAM test passed.\n");
	return 0;
}
#endif

#if !defined(CONFIG_SPD_EEPROM)
#define CONFIG_SYS_DDR_CONTROL 0xc300c000
/*************************************************************************
 *  fixed_sdram init -- doesn't use serial presence detect.
 *  assumes 256MB DDR2 SDRAM SODIMM, without ECC, running at DDR400 speed.
 ************************************************************************/
long int fixed_sdram (void)
{
	volatile ccsr_ddr_t *ddr = (void *)(CONFIG_SYS_MPC85xx_DDR_ADDR);

	out_be32(&ddr->cs0_bnds, 0x0000007f);
	out_be32(&ddr->cs1_bnds, 0x008000ff);
	out_be32(&ddr->cs2_bnds, 0x00000000);
	out_be32(&ddr->cs3_bnds, 0x00000000);
	out_be32(&ddr->cs0_config, 0x80010101);
	out_be32(&ddr->cs1_config, 0x80010101);
	out_be32(&ddr->cs2_config, 0x00000000);
	out_be32(&ddr->cs3_config, 0x00000000);
	out_be32(&ddr->timing_cfg_3, 0x00000000);
	out_be32(&ddr->timing_cfg_0, 0x00220802);
	out_be32(&ddr->timing_cfg_1, 0x38377322);
	out_be32(&ddr->timing_cfg_2, 0x0fa044C7);
	out_be32(&ddr->sdram_cfg, 0x4300C000);
	out_be32(&ddr->sdram_cfg_2, 0x24401000);
	out_be32(&ddr->sdram_mode, 0x23C00542);
	out_be32(&ddr->sdram_mode_2, 0x00000000);
	out_be32(&ddr->sdram_interval, 0x05080100);
	out_be32(&ddr->sdram_md_cntl, 0x00000000);
	out_be32(&ddr->sdram_data_init, 0x00000000);
	out_be32(&ddr->sdram_clk_cntl, 0x03800000);
	asm("sync;isync;msync");
	udelay(500);

	#if defined (CONFIG_DDR_ECC)
	  /* Enable ECC checking */
	  out_be32(&ddr->sdram_cfg, CONFIG_SYS_DDR_CONTROL | 0x20000000);
	#else
	  out_be32(&ddr->sdram_cfg, CONFIG_SYS_DDR_CONTROL);
	#endif

	return CONFIG_SYS_SDRAM_SIZE * 1024 * 1024;
}
#endif

#ifdef CONFIG_PCI1
static struct pci_controller pci1_hose;
#endif	/* CONFIG_PCI1 */

#ifdef CONFIG_PCIE1
static struct pci_controller pcie1_hose;
#endif	/* CONFIG_PCIE1 */

int first_free_busno=0;

void
pci_init_board(void)
{
	volatile ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);

#ifdef CONFIG_PCI1
{
	volatile ccsr_fsl_pci_t *pci = (ccsr_fsl_pci_t *) CONFIG_SYS_PCI1_ADDR;
	struct pci_controller *hose = &pci1_hose;
	struct pci_region *r = hose->regions;

	uint pci_32 = gur->pordevsr & MPC85xx_PORDEVSR_PCI1_PCI32;	/* PORDEVSR[15] */
	uint pci_arb = gur->pordevsr & MPC85xx_PORDEVSR_PCI1_ARB;	/* PORDEVSR[14] */
	uint pci_clk_sel = gur->porpllsr & MPC85xx_PORDEVSR_PCI1_SPD;	/* PORPLLSR[16] */

	uint pci_speed = CONFIG_SYS_CLK_FREQ;	/* get_clock_freq() */

	if (!(gur->devdisr & MPC85xx_DEVDISR_PCI1)) {
		printf ("    PCI host: %d bit, %s MHz, %s, %s\n",
			(pci_32) ? 32 : 64,
			(pci_speed == 33000000) ? "33" :
			(pci_speed == 66000000) ? "66" : "unknown",
			pci_clk_sel ? "sync" : "async",
			pci_arb ? "arbiter" : "external-arbiter"
			);

		/* outbound memory */
		pci_set_region(r++,
			       CONFIG_SYS_PCI1_MEM_BASE,
			       CONFIG_SYS_PCI1_MEM_PHYS,
			       CONFIG_SYS_PCI1_MEM_SIZE,
			       PCI_REGION_MEM);

		/* outbound io */
		pci_set_region(r++,
			       CONFIG_SYS_PCI1_IO_BASE,
			       CONFIG_SYS_PCI1_IO_PHYS,
			       CONFIG_SYS_PCI1_IO_SIZE,
			       PCI_REGION_IO);
		hose->region_count = r - hose->regions;

		hose->first_busno=first_free_busno;

		fsl_pci_init(hose, (u32)&pci->cfg_addr, (u32)&pci->cfg_data);
		first_free_busno=hose->last_busno+1;
		printf ("PCI on bus %02x - %02x\n",hose->first_busno,hose->last_busno);
#ifdef CONFIG_PCIX_CHECK
		if (!(gur->pordevsr & MPC85xx_PORDEVSR_PCI1)) {
			/* PCI-X init */
			if (CONFIG_SYS_CLK_FREQ < 66000000)
				printf("PCI-X will only work at 66 MHz\n");

			reg16 = PCI_X_CMD_MAX_SPLIT | PCI_X_CMD_MAX_READ
				| PCI_X_CMD_ERO | PCI_X_CMD_DPERR_E;
			pci_hose_write_config_word(hose, bus, PCIX_COMMAND, reg16);
		}
#endif
	} else {
		printf ("    PCI: disabled\n");
	}
}
#else
	gur->devdisr |= MPC85xx_DEVDISR_PCI1; /* disable */
#endif

	gur->devdisr |= MPC85xx_DEVDISR_PCI2; /* disable PCI2 */

#ifdef CONFIG_PCIE1
{
	volatile ccsr_fsl_pci_t *pci = (ccsr_fsl_pci_t *) CONFIG_SYS_PCIE1_ADDR;
	struct pci_controller *hose = &pcie1_hose;
	struct pci_region *r = hose->regions;

	int pcie_configured = is_fsl_pci_cfg(LAW_TRGT_IF_PCIE_1, io_sel);

	if (pcie_configured && !(gur->devdisr & MPC85xx_DEVDISR_PCIE)){
		printf ("\n    PCIE at base address %x",
			(uint)pci);

		if (pci->pme_msg_det) {
			pci->pme_msg_det = 0xffffffff;
			debug (" with errors.  Clearing.  Now 0x%08x",pci->pme_msg_det);
		}
		printf ("\n");

		/* outbound memory */
		pci_set_region(r++,
			       CONFIG_SYS_PCIE1_MEM_BASE,
			       CONFIG_SYS_PCIE1_MEM_PHYS,
			       CONFIG_SYS_PCIE1_MEM_SIZE,
			       PCI_REGION_MEM);

		/* outbound io */
		pci_set_region(r++,
			       CONFIG_SYS_PCIE1_IO_BASE,
			       CONFIG_SYS_PCIE1_IO_PHYS,
			       CONFIG_SYS_PCIE1_IO_SIZE,
			       PCI_REGION_IO);

		hose->region_count = r - hose->regions;

		hose->first_busno=first_free_busno;

		fsl_pci_init(hose, (u32)&pci->cfg_addr, (u32)&pci->cfg_data);
		printf ("PCIE on bus %d - %d\n",hose->first_busno,hose->last_busno);

		first_free_busno=hose->last_busno+1;

	} else {
		printf ("    PCIE: disabled\n");
	}
 }
#else
	gur->devdisr |= MPC85xx_DEVDISR_PCIE; /* disable */
#endif

}

int board_eth_init(bd_t *bis)
{
	tsec_standard_init(bis);
	pci_eth_init(bis);
	return 0;	/* otherwise cpu_eth_init gets run */
}

int last_stage_init(void)
{
	return 0;
}

#if defined(CONFIG_OF_BOARD_SETUP)
void ft_board_setup(void *blob, bd_t *bd)
{
	ft_cpu_setup(blob, bd);
#ifdef CONFIG_PCI1
	ft_fsl_pci_setup(blob, "pci0", &pci1_hose);
#endif
#ifdef CONFIG_PCIE1
	ft_fsl_pci_setup(blob, "pci1", &pcie1_hose);
#endif
}
#endif

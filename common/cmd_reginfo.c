/*
 * (C) Copyright 2000
 * Subodh Nijsure, SkyStream Networks, snijsure@skystream.com
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
#include <command.h>
#if defined(CONFIG_8xx)
#include <mpc8xx.h>
#elif defined (CONFIG_405GP) || defined(CONFIG_405EP)
#include <asm/processor.h>
#elif defined (CONFIG_5xx)
#include <mpc5xx.h>
#elif defined (CONFIG_MPC5200)
#include <mpc5xxx.h>
#elif defined (CONFIG_MPC86xx)
extern void mpc86xx_reginfo(void);
#endif

int do_reginfo (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#if defined(CONFIG_8xx)
	volatile immap_t     *immap  = (immap_t *)CONFIG_SYS_IMMR;
	volatile memctl8xx_t *memctl = &immap->im_memctl;
	volatile sysconf8xx_t *sysconf = &immap->im_siu_conf;
	volatile sit8xx_t *timers = &immap->im_sit;

	/* Hopefully more PowerPC  knowledgable people will add code to display
	 * other useful registers
	 */

	printf ("\nSystem Configuration registers\n"

		"\tIMMR\t0x%08X\n", get_immr(0));

	printf("\tSIUMCR\t0x%08X", sysconf->sc_siumcr);
	printf("\tSYPCR\t0x%08X\n",sysconf->sc_sypcr);

	printf("\tSWT\t0x%08X",    sysconf->sc_swt);
	printf("\tSWSR\t0x%04X\n", sysconf->sc_swsr);

	printf("\tSIPEND\t0x%08X\tSIMASK\t0x%08X\n",
		sysconf->sc_sipend, sysconf->sc_simask);
	printf("\tSIEL\t0x%08X\tSIVEC\t0x%08X\n",
		sysconf->sc_siel, sysconf->sc_sivec);
	printf("\tTESR\t0x%08X\tSDCR\t0x%08X\n",
		sysconf->sc_tesr, sysconf->sc_sdcr);

	printf ("Memory Controller Registers\n"

		"\tBR0\t0x%08X\tOR0\t0x%08X \n", memctl->memc_br0, memctl->memc_or0);
	printf("\tBR1\t0x%08X\tOR1\t0x%08X \n", memctl->memc_br1, memctl->memc_or1);
	printf("\tBR2\t0x%08X\tOR2\t0x%08X \n", memctl->memc_br2, memctl->memc_or2);
	printf("\tBR3\t0x%08X\tOR3\t0x%08X \n", memctl->memc_br3, memctl->memc_or3);
	printf("\tBR4\t0x%08X\tOR4\t0x%08X \n", memctl->memc_br4, memctl->memc_or4);
	printf("\tBR5\t0x%08X\tOR5\t0x%08X \n", memctl->memc_br5, memctl->memc_or5);
	printf("\tBR6\t0x%08X\tOR6\t0x%08X \n", memctl->memc_br6, memctl->memc_or6);
	printf("\tBR7\t0x%08X\tOR7\t0x%08X \n", memctl->memc_br7, memctl->memc_or7);
	printf ("\n"
		"\tmamr\t0x%08X\tmbmr\t0x%08X \n",
		memctl->memc_mamr, memctl->memc_mbmr );
	printf("\tmstat\t0x%08X\tmptpr\t0x%08X \n",
		memctl->memc_mstat, memctl->memc_mptpr );
	printf("\tmdr\t0x%08X \n", memctl->memc_mdr);

	printf ("\nSystem Integration Timers\n"
		"\tTBSCR\t0x%08X\tRTCSC\t0x%08X \n",
		timers->sit_tbscr, timers->sit_rtcsc);
	printf("\tPISCR\t0x%08X \n", timers->sit_piscr);

	/*
	 * May be some CPM info here?
	 */

#elif defined (CONFIG_405GP)
	printf ("\n405GP registers; MSR=%08x\n",mfmsr());
	printf ("\nUniversal Interrupt Controller Regs\n"
	    "UIC0SR    UIC0ER    UIC0CR    UIC0PR    UIC0TR    UIC0MSR   UIC0VR    UIC0VCR"
	    "\n"
	    "%08x %08x %08x %08x %08x %08x %08x %08x\n",
	mfdcr(UIC0SR),
	mfdcr(UIC0ER),
	mfdcr(UIC0CR),
	mfdcr(UIC0PR),
	mfdcr(UIC0TR),
	mfdcr(UIC0MSR),
	mfdcr(UIC0VR),
	mfdcr(UIC0VCR));

	puts ("\nMemory (SDRAM) Configuration\n"
	    "besra    besrsa   besrb    besrsb   bear     mcopt1   rtr      pmit\n");

	mtdcr(SDRAM0_CFGADDR,mem_besra);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_besrsa);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_besrb);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_besrsb);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_bear);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_mcopt1);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_rtr);		printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_pmit);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));

	puts ("\n"
	    "mb0cf    mb1cf    mb2cf    mb3cf    sdtr1    ecccf    eccerr\n");
	mtdcr(SDRAM0_CFGADDR,mem_mb0cf);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_mb1cf);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_mb2cf);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_mb3cf);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_sdtr1);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_ecccf);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_eccerr);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));

	printf ("\n\n"
	    "DMA Channels\n"
	    "DMASR    DMASGC   DMAADR\n"
	    "%08x %08x %08x\n"
	    "dmacr_0  dmact_0  dmada_0  dmasa_0  dmasb_0\n"
	    "%08x %08x %08x %08x %08x\n"
	    "dmacr_1  dmact_1  dmada_1  dmasa_1  dmasb_1\n"
	    "%08x %08x %08x %08x %08x\n",
	mfdcr(DMASR),  mfdcr(DMASGC),mfdcr(DMAADR),
	mfdcr(DMACR0), mfdcr(DMACT0),mfdcr(DMADA0), mfdcr(DMASA0), mfdcr(DMASB0),
	mfdcr(DMACR1), mfdcr(DMACT1),mfdcr(DMADA1), mfdcr(DMASA1), mfdcr(DMASB1));

	printf (
	    "dmacr_2  dmact_2  dmada_2  dmasa_2  dmasb_2\n"	"%08x %08x %08x %08x %08x\n"
	    "dmacr_3  dmact_3  dmada_3  dmasa_3  dmasb_3\n"	"%08x %08x %08x %08x %08x\n",
	mfdcr(DMACR2), mfdcr(DMACT2),mfdcr(DMADA2), mfdcr(DMASA2), mfdcr(DMASB2),
	mfdcr(DMACR3), mfdcr(DMACT3),mfdcr(DMADA3), mfdcr(DMASA3), mfdcr(DMASB3) );

	puts ("\n"
	    "External Bus\n"
	    "PBEAR    PBESR0   PBESR1   EBC0_CFG\n");
	mtdcr(EBC0_CFGADDR,PBEAR);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PBESR0);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PBESR1);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,EBC0_CFG);	printf ("%08x ", mfdcr(EBC0_CFGDATA));

	puts ("\n"
	    "PB0CR    PB0AP    PB1CR    PB1AP    PB2CR    PB2AP    PB3CR    PB3AP\n");
	mtdcr(EBC0_CFGADDR,PB0CR);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB0AP);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB1CR);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB1AP);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB2CR);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB2AP);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB3CR);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB3AP);	printf ("%08x ", mfdcr(EBC0_CFGDATA));

	puts ("\n"
	    "PB4CR    PB4AP    PB5CR    bp5ap    PB6CR    PB6AP    PB7CR    PB7AP\n");
	mtdcr(EBC0_CFGADDR,PB4CR);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB4AP);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB5CR);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB5AP);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB6CR);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB6AP);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB7CR);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB7AP);	printf ("%08x ", mfdcr(EBC0_CFGDATA));

	puts ("\n\n");

#elif defined(CONFIG_405EP)
	printf ("\n405EP registers; MSR=%08x\n",mfmsr());
	printf ("\nUniversal Interrupt Controller Regs\n"
	    "UIC0SR    UIC0ER    UIC0CR    UIC0PR    UIC0TR    UIC0MSR   UIC0VR    UIC0VCR"
	    "\n"
	    "%08x %08x %08x %08x %08x %08x %08x %08x\n",
	mfdcr(UIC0SR),
	mfdcr(UIC0ER),
	mfdcr(UIC0CR),
	mfdcr(UIC0PR),
	mfdcr(UIC0TR),
	mfdcr(UIC0MSR),
	mfdcr(UIC0VR),
	mfdcr(UIC0VCR));

	puts ("\nMemory (SDRAM) Configuration\n"
	    "mcopt1   rtr      pmit     mb0cf    mb1cf    sdtr1\n");

	mtdcr(SDRAM0_CFGADDR,mem_mcopt1);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_rtr);		printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_pmit);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_mb0cf);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_mb1cf);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));
	mtdcr(SDRAM0_CFGADDR,mem_sdtr1);	printf ("%08x ", mfdcr(SDRAM0_CFGDATA));

	printf ("\n\n"
	    "DMA Channels\n"
	    "DMASR    DMASGC   DMAADR\n"			"%08x %08x %08x\n"
	    "dmacr_0  dmact_0  dmada_0  dmasa_0  dmasb_0\n"	"%08x %08x %08x %08x %08x\n"
	    "dmacr_1  dmact_1  dmada_1  dmasa_1  dmasb_1\n"	"%08x %08x %08x %08x %08x\n",
	mfdcr(DMASR),  mfdcr(DMASGC),mfdcr(DMAADR),
	mfdcr(DMACR0), mfdcr(DMACT0),mfdcr(DMADA0), mfdcr(DMASA0), mfdcr(DMASB0),
	mfdcr(DMACR1), mfdcr(DMACT1),mfdcr(DMADA1), mfdcr(DMASA1), mfdcr(DMASB1));

	printf (
	    "dmacr_2  dmact_2  dmada_2  dmasa_2  dmasb_2\n"	"%08x %08x %08x %08x %08x\n"
	    "dmacr_3  dmact_3  dmada_3  dmasa_3  dmasb_3\n"	"%08x %08x %08x %08x %08x\n",
	mfdcr(DMACR2), mfdcr(DMACT2),mfdcr(DMADA2), mfdcr(DMASA2), mfdcr(DMASB2),
	mfdcr(DMACR3), mfdcr(DMACT3),mfdcr(DMADA3), mfdcr(DMASA3), mfdcr(DMASB3) );

	puts ("\n"
	    "External Bus\n"
	    "PBEAR    PBESR0   PBESR1   EBC0_CFG\n");
	mtdcr(EBC0_CFGADDR,PBEAR);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PBESR0);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PBESR1);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,EBC0_CFG);	printf ("%08x ", mfdcr(EBC0_CFGDATA));

	puts ("\n"
	    "PB0CR    PB0AP    PB1CR    PB1AP    PB2CR    PB2AP    PB3CR    PB3AP\n");
	mtdcr(EBC0_CFGADDR,PB0CR);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB0AP);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB1CR);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB1AP);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB2CR);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB2AP);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB3CR);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB3AP);	printf ("%08x ", mfdcr(EBC0_CFGDATA));

	puts ("\n"
	    "PB4CR    PB4AP\n");
	mtdcr(EBC0_CFGADDR,PB4CR);	printf ("%08x ", mfdcr(EBC0_CFGDATA));
	mtdcr(EBC0_CFGADDR,PB4AP);	printf ("%08x ", mfdcr(EBC0_CFGDATA));

	puts ("\n\n");
#elif defined(CONFIG_5xx)

	volatile immap_t	*immap  = (immap_t *)CONFIG_SYS_IMMR;
	volatile memctl5xx_t	*memctl = &immap->im_memctl;
	volatile sysconf5xx_t	*sysconf = &immap->im_siu_conf;
	volatile sit5xx_t	*timers = &immap->im_sit;
	volatile car5xx_t	*car = &immap->im_clkrst;
	volatile uimb5xx_t	*uimb = &immap->im_uimb;

	puts ("\nSystem Configuration registers\n");
	printf("\tIMMR\t0x%08X\tSIUMCR\t0x%08X \n", get_immr(0), sysconf->sc_siumcr);
	printf("\tSYPCR\t0x%08X\tSWSR\t0x%04X \n" ,sysconf->sc_sypcr, sysconf->sc_swsr);
	printf("\tSIPEND\t0x%08X\tSIMASK\t0x%08X \n", sysconf->sc_sipend, sysconf->sc_simask);
	printf("\tSIEL\t0x%08X\tSIVEC\t0x%08X \n", sysconf->sc_siel, sysconf->sc_sivec);
	printf("\tTESR\t0x%08X\n", sysconf->sc_tesr);

	puts ("\nMemory Controller Registers\n");
	printf("\tBR0\t0x%08X\tOR0\t0x%08X \n", memctl->memc_br0, memctl->memc_or0);
	printf("\tBR1\t0x%08X\tOR1\t0x%08X \n", memctl->memc_br1, memctl->memc_or1);
	printf("\tBR2\t0x%08X\tOR2\t0x%08X \n", memctl->memc_br2, memctl->memc_or2);
	printf("\tBR3\t0x%08X\tOR3\t0x%08X \n", memctl->memc_br3, memctl->memc_or3);
	printf("\tDMBR\t0x%08X\tDMOR\t0x%08X \n", memctl->memc_dmbr, memctl->memc_dmor );
	printf("\tMSTAT\t0x%08X\n", memctl->memc_mstat);

	puts ("\nSystem Integration Timers\n");
	printf("\tTBSCR\t0x%08X\tRTCSC\t0x%08X \n", timers->sit_tbscr, timers->sit_rtcsc);
	printf("\tPISCR\t0x%08X \n", timers->sit_piscr);

	puts ("\nClocks and Reset\n");
	printf("\tSCCR\t0x%08X\tPLPRCR\t0x%08X \n", car->car_sccr, car->car_plprcr);

	puts ("\nU-Bus to IMB3 Bus Interface\n");
	printf("\tUMCR\t0x%08X\tUIPEND\t0x%08X \n", uimb->uimb_umcr, uimb->uimb_uipend);
	puts ("\n\n");

#elif defined(CONFIG_MPC5200)
	puts ("\nMPC5200 registers\n");
	printf ("MBAR=%08x\n", CONFIG_SYS_MBAR);
	puts ("Memory map registers\n");
	printf ("\tCS0: start %08lX\tstop %08lX\tconfig %08lX\ten %d\n",
		*(volatile ulong*)MPC5XXX_CS0_START,
		*(volatile ulong*)MPC5XXX_CS0_STOP,
		*(volatile ulong*)MPC5XXX_CS0_CFG,
		(*(volatile ulong*)MPC5XXX_ADDECR & 0x00010000) ? 1 : 0);
	printf ("\tCS1: start %08lX\tstop %08lX\tconfig %08lX\ten %d\n",
		*(volatile ulong*)MPC5XXX_CS1_START,
		*(volatile ulong*)MPC5XXX_CS1_STOP,
		*(volatile ulong*)MPC5XXX_CS1_CFG,
		(*(volatile ulong*)MPC5XXX_ADDECR & 0x00020000) ? 1 : 0);
	printf ("\tCS2: start %08lX\tstop %08lX\tconfig %08lX\ten %d\n",
		*(volatile ulong*)MPC5XXX_CS2_START,
		*(volatile ulong*)MPC5XXX_CS2_STOP,
		*(volatile ulong*)MPC5XXX_CS2_CFG,
		(*(volatile ulong*)MPC5XXX_ADDECR & 0x00040000) ? 1 : 0);
	printf ("\tCS3: start %08lX\tstop %08lX\tconfig %08lX\ten %d\n",
		*(volatile ulong*)MPC5XXX_CS3_START,
		*(volatile ulong*)MPC5XXX_CS3_STOP,
		*(volatile ulong*)MPC5XXX_CS3_CFG,
		(*(volatile ulong*)MPC5XXX_ADDECR & 0x00080000) ? 1 : 0);
	printf ("\tCS4: start %08lX\tstop %08lX\tconfig %08lX\ten %d\n",
		*(volatile ulong*)MPC5XXX_CS4_START,
		*(volatile ulong*)MPC5XXX_CS4_STOP,
		*(volatile ulong*)MPC5XXX_CS4_CFG,
		(*(volatile ulong*)MPC5XXX_ADDECR & 0x00100000) ? 1 : 0);
	printf ("\tCS5: start %08lX\tstop %08lX\tconfig %08lX\ten %d\n",
		*(volatile ulong*)MPC5XXX_CS5_START,
		*(volatile ulong*)MPC5XXX_CS5_STOP,
		*(volatile ulong*)MPC5XXX_CS5_CFG,
		(*(volatile ulong*)MPC5XXX_ADDECR & 0x00200000) ? 1 : 0);
	printf ("\tCS6: start %08lX\tstop %08lX\tconfig %08lX\ten %d\n",
		*(volatile ulong*)MPC5XXX_CS6_START,
		*(volatile ulong*)MPC5XXX_CS6_STOP,
		*(volatile ulong*)MPC5XXX_CS6_CFG,
		(*(volatile ulong*)MPC5XXX_ADDECR & 0x04000000) ? 1 : 0);
	printf ("\tCS7: start %08lX\tstop %08lX\tconfig %08lX\ten %d\n",
		*(volatile ulong*)MPC5XXX_CS7_START,
		*(volatile ulong*)MPC5XXX_CS7_STOP,
		*(volatile ulong*)MPC5XXX_CS7_CFG,
		(*(volatile ulong*)MPC5XXX_ADDECR & 0x08000000) ? 1 : 0);
	printf ("\tBOOTCS: start %08lX\tstop %08lX\tconfig %08lX\ten %d\n",
		*(volatile ulong*)MPC5XXX_BOOTCS_START,
		*(volatile ulong*)MPC5XXX_BOOTCS_STOP,
		*(volatile ulong*)MPC5XXX_BOOTCS_CFG,
		(*(volatile ulong*)MPC5XXX_ADDECR & 0x02000000) ? 1 : 0);
	printf ("\tSDRAMCS0: %08lX\n",
		*(volatile ulong*)MPC5XXX_SDRAM_CS0CFG);
	printf ("\tSDRAMCS1: %08lX\n",
		*(volatile ulong*)MPC5XXX_SDRAM_CS1CFG);
#elif defined(CONFIG_MPC86xx)
	mpc86xx_reginfo();

#elif defined(CONFIG_BLACKFIN)
	puts("\nSystem Configuration registers\n");

	puts("\nPLL Registers\n");
	printf("\tPLL_DIV:   0x%04x   PLL_CTL:      0x%04x\n",
		bfin_read_PLL_DIV(), bfin_read_PLL_CTL());
	printf("\tPLL_STAT:  0x%04x   PLL_LOCKCNT:  0x%04x\n",
		bfin_read_PLL_STAT(), bfin_read_PLL_LOCKCNT());
	printf("\tVR_CTL:    0x%04x\n", bfin_read_VR_CTL());

	puts("\nEBIU AMC Registers\n");
	printf("\tEBIU_AMGCTL:   0x%04x\n", bfin_read_EBIU_AMGCTL());
	printf("\tEBIU_AMBCTL0:  0x%08x   EBIU_AMBCTL1:  0x%08x\n",
		bfin_read_EBIU_AMBCTL0(), bfin_read_EBIU_AMBCTL1());
# ifdef EBIU_MODE
	printf("\tEBIU_MBSCTL:   0x%08x   EBIU_ARBSTAT:  0x%08x\n",
		bfin_read_EBIU_MBSCTL(), bfin_read_EBIU_ARBSTAT());
	printf("\tEBIU_MODE:     0x%08x   EBIU_FCTL:     0x%08x\n",
		bfin_read_EBIU_MODE(), bfin_read_EBIU_FCTL());
# endif

# ifdef EBIU_RSTCTL
	puts("\nEBIU DDR Registers\n");
	printf("\tEBIU_DDRCTL0:  0x%08x   EBIU_DDRCTL1:  0x%08x\n",
		bfin_read_EBIU_DDRCTL0(), bfin_read_EBIU_DDRCTL1());
	printf("\tEBIU_DDRCTL2:  0x%08x   EBIU_DDRCTL3:  0x%08x\n",
		bfin_read_EBIU_DDRCTL2(), bfin_read_EBIU_DDRCTL3());
	printf("\tEBIU_DDRQUE:   0x%08x   EBIU_RSTCTL    0x%04x\n",
		bfin_read_EBIU_DDRQUE(), bfin_read_EBIU_RSTCTL());
	printf("\tEBIU_ERRADD:   0x%08x   EBIU_ERRMST:   0x%04x\n",
		bfin_read_EBIU_ERRADD(), bfin_read_EBIU_ERRMST());
# else
	puts("\nEBIU SDC Registers\n");
	printf("\tEBIU_SDRRC:   0x%04x   EBIU_SDBCTL:  0x%04x\n",
		bfin_read_EBIU_SDRRC(), bfin_read_EBIU_SDBCTL());
	printf("\tEBIU_SDSTAT:  0x%04x   EBIU_SDGCTL:  0x%08x\n",
		bfin_read_EBIU_SDSTAT(), bfin_read_EBIU_SDGCTL());
# endif

#endif /* CONFIG_BLACKFIN */

	return 0;
}

 /**************************************************/

#if defined(CONFIG_CMD_REGINFO)
U_BOOT_CMD(
	reginfo,	2,	1,	do_reginfo,
	"print register information",
	""
);
#endif

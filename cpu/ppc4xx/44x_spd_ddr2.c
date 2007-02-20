/*
 * cpu/ppc4xx/44x_spd_ddr2.c
 * This SPD SDRAM detection code supports AMCC PPC44x cpu's with a
 * DDR2 controller (non Denali Core). Those are 440SP/SPe.
 *
 * (C) Copyright 2007
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
 *
 * COPYRIGHT   AMCC   CORPORATION 2004
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
 *
 */

/* define DEBUG for debugging output (obviously ;-)) */
#if 0
#define DEBUG
#endif

#include <common.h>
#include <ppc4xx.h>
#include <i2c.h>
#include <asm/io.h>
#include <asm/processor.h>
#include <asm/mmu.h>

#if defined(CONFIG_SPD_EEPROM) &&				\
	(defined(CONFIG_440SP) || defined(CONFIG_440SPE))

#ifndef	TRUE
#define TRUE            1
#endif
#ifndef FALSE
#define FALSE           0
#endif

#define SDRAM_DDR1	1
#define SDRAM_DDR2	2
#define SDRAM_NONE	0

#define MAXDIMMS 	2
#define MAXRANKS 	4
#define MAXBXCF		4
#define MAX_SPD_BYTES	256   /* Max number of bytes on the DIMM's SPD EEPROM */

#define ONE_BILLION	1000000000

#define MULDIV64(m1, m2, d)	(u32)(((u64)(m1) * (u64)(m2)) / (u64)(d))

#if defined(DEBUG)
static void ppc440sp_sdram_register_dump(void);
#endif

/*-----------------------------------------------------------------------------+
 * Defines
 *-----------------------------------------------------------------------------*/
/* Defines for the Read Cycle Delay test */
#define NUMMEMTESTS 8
#define NUMMEMWORDS 8

/* Private Structure Definitions */

/* enum only to ease code for cas latency setting */
typedef enum ddr_cas_id {
	DDR_CAS_2      = 20,
	DDR_CAS_2_5    = 25,
	DDR_CAS_3      = 30,
	DDR_CAS_4      = 40,
	DDR_CAS_5      = 50
} ddr_cas_id_t;

/*-----------------------------------------------------------------------------+
 * Prototypes
 *-----------------------------------------------------------------------------*/
static unsigned long sdram_memsize(void);
void program_tlb(u32 start, u32 size);
static void get_spd_info(unsigned long *dimm_populated,
			 unsigned char *iic0_dimm_addr,
			 unsigned long num_dimm_banks);
static void check_mem_type(unsigned long *dimm_populated,
			   unsigned char *iic0_dimm_addr,
			   unsigned long num_dimm_banks);
static void check_frequency(unsigned long *dimm_populated,
			    unsigned char *iic0_dimm_addr,
			    unsigned long num_dimm_banks);
static void check_rank_number(unsigned long *dimm_populated,
			      unsigned char *iic0_dimm_addr,
			      unsigned long num_dimm_banks);
static void check_voltage_type(unsigned long *dimm_populated,
			       unsigned char *iic0_dimm_addr,
			       unsigned long num_dimm_banks);
static void program_memory_queue(unsigned long *dimm_populated,
				 unsigned char *iic0_dimm_addr,
				 unsigned long num_dimm_banks);
static void program_codt(unsigned long *dimm_populated,
			 unsigned char *iic0_dimm_addr,
			 unsigned long num_dimm_banks);
static void program_mode(unsigned long *dimm_populated,
			 unsigned char *iic0_dimm_addr,
			 unsigned long num_dimm_banks,
                         ddr_cas_id_t *selected_cas);
static void program_tr(unsigned long *dimm_populated,
		       unsigned char *iic0_dimm_addr,
		       unsigned long num_dimm_banks);
static void program_rtr(unsigned long *dimm_populated,
			unsigned char *iic0_dimm_addr,
			unsigned long num_dimm_banks);
static void program_bxcf(unsigned long *dimm_populated,
			 unsigned char *iic0_dimm_addr,
			 unsigned long num_dimm_banks);
static void program_copt1(unsigned long *dimm_populated,
			  unsigned char *iic0_dimm_addr,
			  unsigned long num_dimm_banks);
static void program_initplr(unsigned long *dimm_populated,
			    unsigned char *iic0_dimm_addr,
			    unsigned long num_dimm_banks,
                            ddr_cas_id_t selected_cas);
static unsigned long is_ecc_enabled(void);
static void program_ecc(unsigned long *dimm_populated,
			unsigned char *iic0_dimm_addr,
			unsigned long num_dimm_banks);
static void program_ecc_addr(unsigned long start_address,
			     unsigned long num_bytes);

#ifdef HARD_CODED_DQS /* calibration test with hardvalues */
static void          test(void);
#else
static void          DQS_calibration_process(void);
#endif
static void program_DQS_calibration(unsigned long *dimm_populated,
				    unsigned char *iic0_dimm_addr,
				    unsigned long num_dimm_banks);

static u32 mfdcr_any(u32 dcr)
{
	u32 val;

	switch (dcr) {
	case SDRAM_R0BAS + 0:
		val = mfdcr(SDRAM_R0BAS + 0);
		break;
	case SDRAM_R0BAS + 1:
		val = mfdcr(SDRAM_R0BAS + 1);
		break;
	case SDRAM_R0BAS + 2:
		val = mfdcr(SDRAM_R0BAS + 2);
		break;
	case SDRAM_R0BAS + 3:
		val = mfdcr(SDRAM_R0BAS + 3);
		break;
	default:
		printf("DCR %d not defined in case statement!!!\n", dcr);
		val = 0; /* just to satisfy the compiler */
	}

	return val;
}

static void mtdcr_any(u32 dcr, u32 val)
{
	switch (dcr) {
	case SDRAM_R0BAS + 0:
		mtdcr(SDRAM_R0BAS + 0, val);
		break;
	case SDRAM_R0BAS + 1:
		mtdcr(SDRAM_R0BAS + 1, val);
		break;
	case SDRAM_R0BAS + 2:
		mtdcr(SDRAM_R0BAS + 2, val);
		break;
	case SDRAM_R0BAS + 3:
		mtdcr(SDRAM_R0BAS + 3, val);
		break;
	default:
		printf("DCR %d not defined in case statement!!!\n", dcr);
	}
}

static void wait_ddr_idle(void)
{
	u32 val;

	do {
		mfsdram(SDRAM_MCSTAT, val);
	} while ((val & SDRAM_MCSTAT_IDLE_MASK) == SDRAM_MCSTAT_IDLE_NOT);
}

static unsigned char spd_read(uchar chip, uint addr)
{
	unsigned char data[2];

	if (i2c_probe(chip) == 0)
		if (i2c_read(chip, addr, 1, data, 1) == 0)
			return data[0];

	return 0;
}

/*-----------------------------------------------------------------------------+
 * sdram_memsize
 *-----------------------------------------------------------------------------*/
static unsigned long sdram_memsize(void)
{
	unsigned long mem_size;
	unsigned long mcopt2;
	unsigned long mcstat;
	unsigned long mb0cf;
	unsigned long sdsz;
	unsigned long i;

	mem_size = 0;

	mfsdram(SDRAM_MCOPT2, mcopt2);
	mfsdram(SDRAM_MCSTAT, mcstat);

	/* DDR controller must be enabled and not in self-refresh. */
	/* Otherwise memsize is zero. */
	if (((mcopt2 & SDRAM_MCOPT2_DCEN_MASK) == SDRAM_MCOPT2_DCEN_ENABLE)
	    && ((mcopt2 & SDRAM_MCOPT2_SREN_MASK) == SDRAM_MCOPT2_SREN_EXIT)
	    && ((mcstat & (SDRAM_MCSTAT_MIC_MASK | SDRAM_MCSTAT_SRMS_MASK))
		== (SDRAM_MCSTAT_MIC_COMP | SDRAM_MCSTAT_SRMS_NOT_SF))) {
		for (i = 0; i < 4; i++) {
			mfsdram(SDRAM_MB0CF + (i << 2), mb0cf);
			/* Banks enabled */
			if ((mb0cf & SDRAM_BXCF_M_BE_MASK) == SDRAM_BXCF_M_BE_ENABLE) {
				sdsz = mfdcr_any(SDRAM_R0BAS + i) & SDRAM_RXBAS_SDSZ_MASK;

				switch(sdsz) {
				case SDRAM_RXBAS_SDSZ_8:
					mem_size+=8;
					break;
				case SDRAM_RXBAS_SDSZ_16:
					mem_size+=16;
					break;
				case SDRAM_RXBAS_SDSZ_32:
					mem_size+=32;
					break;
				case SDRAM_RXBAS_SDSZ_64:
					mem_size+=64;
					break;
				case SDRAM_RXBAS_SDSZ_128:
					mem_size+=128;
					break;
				case SDRAM_RXBAS_SDSZ_256:
					mem_size+=256;
					break;
				case SDRAM_RXBAS_SDSZ_512:
					mem_size+=512;
					break;
				case SDRAM_RXBAS_SDSZ_1024:
					mem_size+=1024;
					break;
				case SDRAM_RXBAS_SDSZ_2048:
					mem_size+=2048;
					break;
				case SDRAM_RXBAS_SDSZ_4096:
					mem_size+=4096;
					break;
				default:
					mem_size=0;
					break;
				}
			}
		}
	}

	mem_size *= 1024 * 1024;
	return(mem_size);
}

/*-----------------------------------------------------------------------------+
 * initdram.  Initializes the 440SP Memory Queue and DDR SDRAM controller.
 * Note: This routine runs from flash with a stack set up in the chip's
 * sram space.  It is important that the routine does not require .sbss, .bss or
 * .data sections.  It also cannot call routines that require these sections.
 *-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Function:     initdram
 * Description:  Configures SDRAM memory banks for DDR operation.
 *               Auto Memory Configuration option reads the DDR SDRAM EEPROMs
 *               via the IIC bus and then configures the DDR SDRAM memory
 *               banks appropriately. If Auto Memory Configuration is
 *               not used, it is assumed that no DIMM is plugged
 *-----------------------------------------------------------------------------*/
long int initdram(int board_type)
{
	unsigned char spd0[MAX_SPD_BYTES];
	unsigned char spd1[MAX_SPD_BYTES];
	unsigned char *dimm_spd[MAXDIMMS];
	unsigned long dimm_populated[MAXDIMMS];
	unsigned char iic0_dimm_addr[MAXDIMMS];
	unsigned long num_dimm_banks;		    /* on board dimm banks */
	unsigned long val;
	ddr_cas_id_t  selected_cas;
	unsigned long dram_size = 0;

	num_dimm_banks = sizeof(iic0_dimm_addr);

	/*------------------------------------------------------------------
	 * Set up an array of SPD matrixes.
	 *-----------------------------------------------------------------*/
	dimm_spd[0] = spd0;
	dimm_spd[1] = spd1;

	/*------------------------------------------------------------------
	 * Set up an array of iic0 dimm addresses.
	 *-----------------------------------------------------------------*/
	iic0_dimm_addr[0] = IIC0_DIMM0_ADDR;
	iic0_dimm_addr[1] = IIC0_DIMM1_ADDR;

	/*------------------------------------------------------------------
	 * Reset the DDR-SDRAM controller.
	 *-----------------------------------------------------------------*/
	mtsdr(SDR0_SRST, 0x00200000);
	mtsdr(SDR0_SRST, 0x00000000);

	/*
	 * Make sure I2C controller is initialized
	 * before continuing.
	 */

	/* switch to correct I2C bus */
	I2C_SET_BUS(CFG_SPD_BUS_NUM);
	i2c_init(CFG_I2C_SPEED, CFG_I2C_SLAVE);

	/*------------------------------------------------------------------
	 * Clear out the serial presence detect buffers.
	 * Perform IIC reads from the dimm.  Fill in the spds.
	 * Check to see if the dimm slots are populated
	 *-----------------------------------------------------------------*/
	get_spd_info(dimm_populated, iic0_dimm_addr, num_dimm_banks);

	/*------------------------------------------------------------------
	 * Check the memory type for the dimms plugged.
	 *-----------------------------------------------------------------*/
	check_mem_type(dimm_populated, iic0_dimm_addr, num_dimm_banks);

	/*------------------------------------------------------------------
	 * Check the frequency supported for the dimms plugged.
	 *-----------------------------------------------------------------*/
	check_frequency(dimm_populated, iic0_dimm_addr, num_dimm_banks);

	/*------------------------------------------------------------------
	 * Check the total rank number.
	 *-----------------------------------------------------------------*/
	check_rank_number(dimm_populated, iic0_dimm_addr, num_dimm_banks);

	/*------------------------------------------------------------------
	 * Check the voltage type for the dimms plugged.
	 *-----------------------------------------------------------------*/
	check_voltage_type(dimm_populated, iic0_dimm_addr, num_dimm_banks);

	/*------------------------------------------------------------------
	 * Program SDRAM controller options 2 register
	 * Except Enabling of the memory controller.
	 *-----------------------------------------------------------------*/
	mfsdram(SDRAM_MCOPT2, val);
	mtsdram(SDRAM_MCOPT2,
		(val &
		 ~(SDRAM_MCOPT2_SREN_MASK | SDRAM_MCOPT2_PMEN_MASK |
		   SDRAM_MCOPT2_IPTR_MASK | SDRAM_MCOPT2_XSRP_MASK |
		   SDRAM_MCOPT2_ISIE_MASK))
		| (SDRAM_MCOPT2_SREN_ENTER | SDRAM_MCOPT2_PMEN_DISABLE |
		   SDRAM_MCOPT2_IPTR_IDLE | SDRAM_MCOPT2_XSRP_ALLOW |
		   SDRAM_MCOPT2_ISIE_ENABLE));

	/*------------------------------------------------------------------
	 * Program SDRAM controller options 1 register
	 * Note: Does not enable the memory controller.
	 *-----------------------------------------------------------------*/
	program_copt1(dimm_populated, iic0_dimm_addr, num_dimm_banks);

	/*------------------------------------------------------------------
	 * Set the SDRAM Controller On Die Termination Register
	 *-----------------------------------------------------------------*/
	program_codt(dimm_populated, iic0_dimm_addr, num_dimm_banks);

	/*------------------------------------------------------------------
	 * Program SDRAM refresh register.
	 *-----------------------------------------------------------------*/
	program_rtr(dimm_populated, iic0_dimm_addr, num_dimm_banks);

	/*------------------------------------------------------------------
	 * Program SDRAM mode register.
	 *-----------------------------------------------------------------*/
	program_mode(dimm_populated, iic0_dimm_addr, num_dimm_banks, &selected_cas);

	/*------------------------------------------------------------------
	 * Set the SDRAM Write Data/DM/DQS Clock Timing Reg
	 *-----------------------------------------------------------------*/
	mfsdram(SDRAM_WRDTR, val);
	mtsdram(SDRAM_WRDTR, (val & ~(SDRAM_WRDTR_LLWP_MASK | SDRAM_WRDTR_WTR_MASK)) |
		(SDRAM_WRDTR_LLWP_1_CYC | SDRAM_WRDTR_WTR_90_DEG_ADV));

	/*------------------------------------------------------------------
	 * Set the SDRAM Clock Timing Register
	 *-----------------------------------------------------------------*/
	mfsdram(SDRAM_CLKTR, val);
	mtsdram(SDRAM_CLKTR, (val & ~SDRAM_CLKTR_CLKP_MASK) | SDRAM_CLKTR_CLKP_0_DEG);

	/*------------------------------------------------------------------
	 * Program the BxCF registers.
	 *-----------------------------------------------------------------*/
	program_bxcf(dimm_populated, iic0_dimm_addr, num_dimm_banks);

	/*------------------------------------------------------------------
	 * Program SDRAM timing registers.
	 *-----------------------------------------------------------------*/
	program_tr(dimm_populated, iic0_dimm_addr, num_dimm_banks);

	/*------------------------------------------------------------------
	 * Set the Extended Mode register
	 *-----------------------------------------------------------------*/
	mfsdram(SDRAM_MEMODE, val);
	mtsdram(SDRAM_MEMODE,
		(val & ~(SDRAM_MEMODE_DIC_MASK  | SDRAM_MEMODE_DLL_MASK |
			 SDRAM_MEMODE_RTT_MASK | SDRAM_MEMODE_DQS_MASK)) |
		(SDRAM_MEMODE_DIC_NORMAL | SDRAM_MEMODE_DLL_ENABLE
		 | SDRAM_MEMODE_RTT_75OHM | SDRAM_MEMODE_DQS_ENABLE));

	/*------------------------------------------------------------------
	 * Program Initialization preload registers.
	 *-----------------------------------------------------------------*/
	program_initplr(dimm_populated, iic0_dimm_addr, num_dimm_banks,
			selected_cas);

	/*------------------------------------------------------------------
	 * Delay to ensure 200usec have elapsed since reset.
	 *-----------------------------------------------------------------*/
	udelay(400);

	/*------------------------------------------------------------------
	 * Set the memory queue core base addr.
	 *-----------------------------------------------------------------*/
	program_memory_queue(dimm_populated, iic0_dimm_addr, num_dimm_banks);

	/*------------------------------------------------------------------
	 * Program SDRAM controller options 2 register
	 * Enable the memory controller.
	 *-----------------------------------------------------------------*/
	mfsdram(SDRAM_MCOPT2, val);
	mtsdram(SDRAM_MCOPT2,
		(val & ~(SDRAM_MCOPT2_SREN_MASK | SDRAM_MCOPT2_DCEN_MASK |
			 SDRAM_MCOPT2_IPTR_MASK | SDRAM_MCOPT2_ISIE_MASK)) |
		(SDRAM_MCOPT2_DCEN_ENABLE | SDRAM_MCOPT2_IPTR_EXECUTE));

	/*------------------------------------------------------------------
	 * Wait for SDRAM_CFG0_DC_EN to complete.
	 *-----------------------------------------------------------------*/
	do {
		mfsdram(SDRAM_MCSTAT, val);
	} while ((val & SDRAM_MCSTAT_MIC_MASK) == SDRAM_MCSTAT_MIC_NOTCOMP);

	/* get installed memory size */
	dram_size = sdram_memsize();

	/* and program tlb entries for this size (dynamic) */
	program_tlb(0, dram_size);

#if 1 /* TODO: ECC support will come later */
	/*------------------------------------------------------------------
	 * If ecc is enabled, initialize the parity bits.
	 *-----------------------------------------------------------------*/
	program_ecc(dimm_populated, iic0_dimm_addr, num_dimm_banks);
#endif

	/*------------------------------------------------------------------
	 * DQS calibration.
	 *-----------------------------------------------------------------*/
	program_DQS_calibration(dimm_populated, iic0_dimm_addr, num_dimm_banks);

#ifdef DEBUG
	ppc440sp_sdram_register_dump();
#endif

	return dram_size;
}

static void get_spd_info(unsigned long *dimm_populated,
			 unsigned char *iic0_dimm_addr,
			 unsigned long num_dimm_banks)
{
	unsigned long dimm_num;
	unsigned long dimm_found;
	unsigned char num_of_bytes;
	unsigned char total_size;

	dimm_found = FALSE;
	for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
		num_of_bytes = 0;
		total_size = 0;

		num_of_bytes = spd_read(iic0_dimm_addr[dimm_num], 0);
		debug("\nspd_read(0x%x) returned %d\n",
		      iic0_dimm_addr[dimm_num], num_of_bytes);
		total_size = spd_read(iic0_dimm_addr[dimm_num], 1);
		debug("spd_read(0x%x) returned %d\n",
		      iic0_dimm_addr[dimm_num], total_size);

		if ((num_of_bytes != 0) && (total_size != 0)) {
			dimm_populated[dimm_num] = TRUE;
			dimm_found = TRUE;
			debug("DIMM slot %lu: populated\n", dimm_num);
		} else {
			dimm_populated[dimm_num] = FALSE;
			debug("DIMM slot %lu: Not populated\n", dimm_num);
		}
	}

	if (dimm_found == FALSE) {
		printf("ERROR - No memory installed. Install a DDR-SDRAM DIMM.\n\n");
		hang();
	}
}

#ifdef CONFIG_ADD_RAM_INFO
void board_add_ram_info(int use_default)
{
        if (is_ecc_enabled())
                puts(" (ECC enabled)");
        else
                puts(" (ECC not enabled)");
}
#endif

/*------------------------------------------------------------------
 * For the memory DIMMs installed, this routine verifies that they
 * really are DDR specific DIMMs.
 *-----------------------------------------------------------------*/
static void check_mem_type(unsigned long *dimm_populated,
			   unsigned char *iic0_dimm_addr,
			   unsigned long num_dimm_banks)
{
	unsigned long dimm_num;
	unsigned long dimm_type;

	for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
		if (dimm_populated[dimm_num] == TRUE) {
			dimm_type = spd_read(iic0_dimm_addr[dimm_num], 2);
			switch (dimm_type) {
			case 1:
				printf("ERROR: Standard Fast Page Mode DRAM DIMM detected in "
				       "slot %d.\n", (unsigned int)dimm_num);
				printf("Only DDR and DDR2 SDRAM DIMMs are supported.\n");
				printf("Replace the DIMM module with a supported DIMM.\n\n");
				hang();
				break;
			case 2:
				printf("ERROR: EDO DIMM detected in slot %d.\n",
				       (unsigned int)dimm_num);
				printf("Only DDR and DDR2 SDRAM DIMMs are supported.\n");
				printf("Replace the DIMM module with a supported DIMM.\n\n");
				hang();
				break;
			case 3:
				printf("ERROR: Pipelined Nibble DIMM detected in slot %d.\n",
				       (unsigned int)dimm_num);
				printf("Only DDR and DDR2 SDRAM DIMMs are supported.\n");
				printf("Replace the DIMM module with a supported DIMM.\n\n");
				hang();
				break;
			case 4:
				printf("ERROR: SDRAM DIMM detected in slot %d.\n",
				       (unsigned int)dimm_num);
				printf("Only DDR and DDR2 SDRAM DIMMs are supported.\n");
				printf("Replace the DIMM module with a supported DIMM.\n\n");
				hang();
				break;
			case 5:
				printf("ERROR: Multiplexed ROM DIMM detected in slot %d.\n",
				       (unsigned int)dimm_num);
				printf("Only DDR and DDR2 SDRAM DIMMs are supported.\n");
				printf("Replace the DIMM module with a supported DIMM.\n\n");
				hang();
				break;
			case 6:
				printf("ERROR: SGRAM DIMM detected in slot %d.\n",
				       (unsigned int)dimm_num);
				printf("Only DDR and DDR2 SDRAM DIMMs are supported.\n");
				printf("Replace the DIMM module with a supported DIMM.\n\n");
				hang();
				break;
			case 7:
				debug("DIMM slot %d: DDR1 SDRAM detected\n", dimm_num);
				dimm_populated[dimm_num] = SDRAM_DDR1;
				break;
			case 8:
				debug("DIMM slot %d: DDR2 SDRAM detected\n", dimm_num);
				dimm_populated[dimm_num] = SDRAM_DDR2;
				break;
			default:
				printf("ERROR: Unknown DIMM detected in slot %d.\n",
				       (unsigned int)dimm_num);
				printf("Only DDR1 and DDR2 SDRAM DIMMs are supported.\n");
				printf("Replace the DIMM module with a supported DIMM.\n\n");
				hang();
				break;
			}
		}
	}
	for (dimm_num = 1; dimm_num < num_dimm_banks; dimm_num++) {
		if ((dimm_populated[dimm_num-1] != SDRAM_NONE)
		    && (dimm_populated[dimm_num]   != SDRAM_NONE)
		    && (dimm_populated[dimm_num-1] != dimm_populated[dimm_num])) {
			printf("ERROR: DIMM's DDR1 and DDR2 type can not be mixed.\n");
			hang();
		}
	}
}

/*------------------------------------------------------------------
 * For the memory DIMMs installed, this routine verifies that
 * frequency previously calculated is supported.
 *-----------------------------------------------------------------*/
static void check_frequency(unsigned long *dimm_populated,
			    unsigned char *iic0_dimm_addr,
			    unsigned long num_dimm_banks)
{
	unsigned long dimm_num;
	unsigned long tcyc_reg;
	unsigned long cycle_time;
	unsigned long calc_cycle_time;
	unsigned long sdram_freq;
	unsigned long sdr_ddrpll;
	PPC440_SYS_INFO board_cfg;

	/*------------------------------------------------------------------
	 * Get the board configuration info.
	 *-----------------------------------------------------------------*/
	get_sys_info(&board_cfg);

	mfsdr(sdr_ddr0, sdr_ddrpll);
	sdram_freq = ((board_cfg.freqPLB) * SDR0_DDR0_DDRM_DECODE(sdr_ddrpll));

	/*
	 * calc_cycle_time is calculated from DDR frequency set by board/chip
	 * and is expressed in multiple of 10 picoseconds
	 * to match the way DIMM cycle time is calculated below.
	 */
	calc_cycle_time = MULDIV64(ONE_BILLION, 100, sdram_freq);

	for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
		if (dimm_populated[dimm_num] != SDRAM_NONE) {
			tcyc_reg = spd_read(iic0_dimm_addr[dimm_num], 9);
			/*
			 * Byte 9, Cycle time for CAS Latency=X, is split into two nibbles:
			 * the higher order nibble (bits 4-7) designates the cycle time
			 * to a granularity of 1ns;
			 * the value presented by the lower order nibble (bits 0-3)
			 * has a granularity of .1ns and is added to the value designated
			 * by the higher nibble. In addition, four lines of the lower order
			 * nibble are assigned to support +.25,+.33, +.66 and +.75.
			 */
			 /* Convert from hex to decimal */
			if ((tcyc_reg & 0x0F) == 0x0D)
				cycle_time = (((tcyc_reg & 0xF0) >> 4) * 100) + 75;
			else if ((tcyc_reg & 0x0F) == 0x0C)
				cycle_time = (((tcyc_reg & 0xF0) >> 4) * 100) + 66;
			else if ((tcyc_reg & 0x0F) == 0x0B)
				cycle_time = (((tcyc_reg & 0xF0) >> 4) * 100) + 33;
			else if ((tcyc_reg & 0x0F) == 0x0A)
				cycle_time = (((tcyc_reg & 0xF0) >> 4) * 100) + 25;
			else
				cycle_time = (((tcyc_reg & 0xF0) >> 4) * 100) +
					((tcyc_reg & 0x0F)*10);

			if  (cycle_time > (calc_cycle_time + 10)) {
				/*
				 * the provided sdram cycle_time is too small
				 * for the available DIMM cycle_time.
				 * The additionnal 100ps is here to accept a small incertainty.
				 */
				printf("ERROR: DRAM DIMM detected with cycle_time %d ps in "
				       "slot %d \n while calculated cycle time is %d ps.\n",
				       (unsigned int)(cycle_time*10),
				       (unsigned int)dimm_num,
				       (unsigned int)(calc_cycle_time*10));
				printf("Replace the DIMM, or change DDR frequency via "
				       "strapping bits.\n\n");
				hang();
			}
		}
	}
}

/*------------------------------------------------------------------
 * For the memory DIMMs installed, this routine verifies two
 * ranks/banks maximum are availables.
 *-----------------------------------------------------------------*/
static void check_rank_number(unsigned long *dimm_populated,
			      unsigned char *iic0_dimm_addr,
			      unsigned long num_dimm_banks)
{
	unsigned long dimm_num;
	unsigned long dimm_rank;
	unsigned long total_rank = 0;

	for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
		if (dimm_populated[dimm_num] != SDRAM_NONE) {
			dimm_rank = spd_read(iic0_dimm_addr[dimm_num], 5);
			if (((unsigned long)spd_read(iic0_dimm_addr[dimm_num], 2)) == 0x08)
				dimm_rank = (dimm_rank & 0x0F) +1;
			else
				dimm_rank = dimm_rank & 0x0F;


			if (dimm_rank > MAXRANKS) {
				printf("ERROR: DRAM DIMM detected with %d ranks in "
				       "slot %d is not supported.\n", dimm_rank, dimm_num);
				printf("Only %d ranks are supported for all DIMM.\n", MAXRANKS);
				printf("Replace the DIMM module with a supported DIMM.\n\n");
				hang();
			} else
				total_rank += dimm_rank;
		}
		if (total_rank > MAXRANKS) {
			printf("ERROR: DRAM DIMM detected with a total of %d ranks "
			       "for all slots.\n", (unsigned int)total_rank);
			printf("Only %d ranks are supported for all DIMM.\n", MAXRANKS);
			printf("Remove one of the DIMM modules.\n\n");
			hang();
		}
	}
}

/*------------------------------------------------------------------
 * only support 2.5V modules.
 * This routine verifies this.
 *-----------------------------------------------------------------*/
static void check_voltage_type(unsigned long *dimm_populated,
			       unsigned char *iic0_dimm_addr,
			       unsigned long num_dimm_banks)
{
	unsigned long dimm_num;
	unsigned long voltage_type;

	for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
		if (dimm_populated[dimm_num] != SDRAM_NONE) {
			voltage_type = spd_read(iic0_dimm_addr[dimm_num], 8);
			switch (voltage_type) {
			case 0x00:
				printf("ERROR: Only DIMMs DDR 2.5V or DDR2 1.8V are supported.\n");
				printf("This DIMM is 5.0 Volt/TTL.\n");
				printf("Replace the DIMM module in slot %d with a supported DIMM.\n\n",
				       (unsigned int)dimm_num);
				hang();
				break;
			case 0x01:
				printf("ERROR: Only DIMMs DDR 2.5V or DDR2 1.8V are supported.\n");
				printf("This DIMM is LVTTL.\n");
				printf("Replace the DIMM module in slot %d with a supported DIMM.\n\n",
				       (unsigned int)dimm_num);
				hang();
				break;
			case 0x02:
				printf("ERROR: Only DIMMs DDR 2.5V or DDR2 1.8V are supported.\n");
				printf("This DIMM is 1.5 Volt.\n");
				printf("Replace the DIMM module in slot %d with a supported DIMM.\n\n",
				       (unsigned int)dimm_num);
				hang();
				break;
			case 0x03:
				printf("ERROR: Only DIMMs DDR 2.5V or DDR2 1.8V are supported.\n");
				printf("This DIMM is 3.3 Volt/TTL.\n");
				printf("Replace the DIMM module in slot %d with a supported DIMM.\n\n",
				       (unsigned int)dimm_num);
				hang();
				break;
			case 0x04:
				/* 2.5 Voltage only for DDR1 */
				break;
			case 0x05:
				/* 1.8 Voltage only for DDR2 */
				break;
			default:
				printf("ERROR: Only DIMMs DDR 2.5V or DDR2 1.8V are supported.\n");
				printf("Replace the DIMM module in slot %d with a supported DIMM.\n\n",
				       (unsigned int)dimm_num);
				hang();
				break;
			}
		}
	}
}

/*-----------------------------------------------------------------------------+
 * program_copt1.
 *-----------------------------------------------------------------------------*/
static void program_copt1(unsigned long *dimm_populated,
			  unsigned char *iic0_dimm_addr,
			  unsigned long num_dimm_banks)
{
	unsigned long dimm_num;
	unsigned long mcopt1;
	unsigned long ecc_enabled;
	unsigned long ecc = 0;
	unsigned long data_width = 0;
	unsigned long dimm_32bit;
	unsigned long dimm_64bit;
	unsigned long registered = 0;
	unsigned long attribute = 0;
	unsigned long buf0, buf1; /* TODO: code to be changed for IOP1.6 to support 4 DIMMs */
	unsigned long bankcount;
	unsigned long ddrtype;
	unsigned long val;

	ecc_enabled = TRUE;
	dimm_32bit = FALSE;
	dimm_64bit = FALSE;
	buf0 = FALSE;
	buf1 = FALSE;

	/*------------------------------------------------------------------
	 * Set memory controller options reg 1, SDRAM_MCOPT1.
	 *-----------------------------------------------------------------*/
	mfsdram(SDRAM_MCOPT1, val);
	mcopt1 = val & ~(SDRAM_MCOPT1_MCHK_MASK | SDRAM_MCOPT1_RDEN_MASK |
			 SDRAM_MCOPT1_PMU_MASK  | SDRAM_MCOPT1_DMWD_MASK |
			 SDRAM_MCOPT1_UIOS_MASK | SDRAM_MCOPT1_BCNT_MASK |
			 SDRAM_MCOPT1_DDR_TYPE_MASK | SDRAM_MCOPT1_RWOO_MASK |
			 SDRAM_MCOPT1_WOOO_MASK | SDRAM_MCOPT1_DCOO_MASK |
			 SDRAM_MCOPT1_DREF_MASK);

	mcopt1 |= SDRAM_MCOPT1_QDEP;
	mcopt1 |= SDRAM_MCOPT1_PMU_OPEN;
	mcopt1 |= SDRAM_MCOPT1_RWOO_DISABLED;
	mcopt1 |= SDRAM_MCOPT1_WOOO_DISABLED;
	mcopt1 |= SDRAM_MCOPT1_DCOO_DISABLED;
	mcopt1 |= SDRAM_MCOPT1_DREF_NORMAL;

	for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
		if (dimm_populated[dimm_num] != SDRAM_NONE) {
			/* test ecc support */
			ecc = (unsigned long)spd_read(iic0_dimm_addr[dimm_num], 11);
			if (ecc != 0x02) /* ecc not supported */
				ecc_enabled = FALSE;

			/* test bank count */
			bankcount = (unsigned long)spd_read(iic0_dimm_addr[dimm_num], 17);
			if (bankcount == 0x04) /* bank count = 4 */
				mcopt1 |= SDRAM_MCOPT1_4_BANKS;
			else /* bank count = 8 */
				mcopt1 |= SDRAM_MCOPT1_8_BANKS;

			/* test DDR type */
			ddrtype = (unsigned long)spd_read(iic0_dimm_addr[dimm_num], 2);
			/* test for buffered/unbuffered, registered, differential clocks */
			registered = (unsigned long)spd_read(iic0_dimm_addr[dimm_num], 20);
			attribute = (unsigned long)spd_read(iic0_dimm_addr[dimm_num], 21);

			/* TODO: code to be changed for IOP1.6 to support 4 DIMMs */
			if (dimm_num == 0) {
				if (dimm_populated[dimm_num] == SDRAM_DDR1) /* DDR1 type */
					mcopt1 |= SDRAM_MCOPT1_DDR1_TYPE;
				if (dimm_populated[dimm_num] == SDRAM_DDR2) /* DDR2 type */
					mcopt1 |= SDRAM_MCOPT1_DDR2_TYPE;
				if (registered == 1) { /* DDR2 always buffered */
					/* TODO: what about above  comments ? */
					mcopt1 |= SDRAM_MCOPT1_RDEN;
					buf0 = TRUE;
				} else {
					/* TODO: the mask 0x02 doesn't match Samsung def for byte 21. */
					if ((attribute & 0x02) == 0x00) {
						/* buffered not supported */
						buf0 = FALSE;
					} else {
						mcopt1 |= SDRAM_MCOPT1_RDEN;
						buf0 = TRUE;
					}
				}
			}
			else if (dimm_num == 1) {
				if (dimm_populated[dimm_num] == SDRAM_DDR1) /* DDR1 type */
					mcopt1 |= SDRAM_MCOPT1_DDR1_TYPE;
				if (dimm_populated[dimm_num] == SDRAM_DDR2) /* DDR2 type */
					mcopt1 |= SDRAM_MCOPT1_DDR2_TYPE;
				if (registered == 1) {
					/* DDR2 always buffered */
					mcopt1 |= SDRAM_MCOPT1_RDEN;
					buf1 = TRUE;
				} else {
					if ((attribute & 0x02) == 0x00) {
						/* buffered not supported */
						buf1 = FALSE;
					} else {
						mcopt1 |= SDRAM_MCOPT1_RDEN;
						buf1 = TRUE;
					}
				}
			}

			/* Note that for DDR2 the byte 7 is reserved, but OK to keep code as is. */
			data_width = (unsigned long)spd_read(iic0_dimm_addr[dimm_num], 6) +
				(((unsigned long)spd_read(iic0_dimm_addr[dimm_num], 7)) << 8);

			switch (data_width) {
			case 72:
			case 64:
				dimm_64bit = TRUE;
				break;
			case 40:
			case 32:
				dimm_32bit = TRUE;
				break;
			default:
				printf("WARNING: Detected a DIMM with a data width of %d bits.\n",
				       data_width);
				printf("Only DIMMs with 32 or 64 bit DDR-SDRAM widths are supported.\n");
				break;
			}
		}
	}

	/* verify matching properties */
	if ((dimm_populated[0] != SDRAM_NONE) && (dimm_populated[1] != SDRAM_NONE)) {
		if (buf0 != buf1) {
			printf("ERROR: DIMM's buffered/unbuffered, registered, clocking don't match.\n");
			hang();
		}
	}

	if ((dimm_64bit == TRUE) && (dimm_32bit == TRUE)) {
		printf("ERROR: Cannot mix 32 bit and 64 bit DDR-SDRAM DIMMs together.\n");
		hang();
	}
	else if ((dimm_64bit == TRUE) && (dimm_32bit == FALSE)) {
		mcopt1 |= SDRAM_MCOPT1_DMWD_64;
	} else if ((dimm_64bit == FALSE) && (dimm_32bit == TRUE)) {
		mcopt1 |= SDRAM_MCOPT1_DMWD_32;
	} else {
		printf("ERROR: Please install only 32 or 64 bit DDR-SDRAM DIMMs.\n\n");
		hang();
	}

	if (ecc_enabled == TRUE)
		mcopt1 |= SDRAM_MCOPT1_MCHK_GEN;
	else
		mcopt1 |= SDRAM_MCOPT1_MCHK_NON;

	mtsdram(SDRAM_MCOPT1, mcopt1);
}

/*-----------------------------------------------------------------------------+
 * program_codt.
 *-----------------------------------------------------------------------------*/
static void program_codt(unsigned long *dimm_populated,
			 unsigned char *iic0_dimm_addr,
			 unsigned long num_dimm_banks)
{
	unsigned long codt;
	unsigned long modt0 = 0;
	unsigned long modt1 = 0;
	unsigned long modt2 = 0;
	unsigned long modt3 = 0;
	unsigned char dimm_num;
	unsigned char dimm_rank;
	unsigned char total_rank = 0;
	unsigned char total_dimm = 0;
	unsigned char dimm_type = 0;
	unsigned char firstSlot = 0;

	/*------------------------------------------------------------------
	 * Set the SDRAM Controller On Die Termination Register
	 *-----------------------------------------------------------------*/
	mfsdram(SDRAM_CODT, codt);
	codt |= (SDRAM_CODT_IO_NMODE
		 & (~SDRAM_CODT_DQS_SINGLE_END
		    & ~SDRAM_CODT_CKSE_SINGLE_END
		    & ~SDRAM_CODT_FEEBBACK_RCV_SINGLE_END
		    & ~SDRAM_CODT_FEEBBACK_DRV_SINGLE_END));

	for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
		if (dimm_populated[dimm_num] != SDRAM_NONE) {
			dimm_rank = (unsigned long)spd_read(iic0_dimm_addr[dimm_num], 5);
			if (((unsigned long)spd_read(iic0_dimm_addr[dimm_num], 2)) == 0x08) {
				dimm_rank = (dimm_rank & 0x0F) + 1;
				dimm_type = SDRAM_DDR2;
			} else {
				dimm_rank = dimm_rank & 0x0F;
				dimm_type = SDRAM_DDR1;
			}

			total_rank +=  dimm_rank;
			total_dimm ++;
			if ((dimm_num == 0) && (total_dimm == 1))
				firstSlot = TRUE;
			else
				firstSlot = FALSE;
		}
	}
	if (dimm_type == SDRAM_DDR2) {
		codt |= SDRAM_CODT_DQS_1_8_V_DDR2;
		if ((total_dimm == 1) && (firstSlot == TRUE)) {
			if (total_rank == 1) {
				codt |= 0x00800000;
				modt0 = 0x01000000;
				modt1 = 0x00000000;
				modt2 = 0x00000000;
				modt3 = 0x00000000;
			}
			if (total_rank == 2) {
				codt |= 0x02800000;
				modt0 = 0x06000000;
				modt1 = 0x01800000;
				modt2 = 0x00000000;
				modt3 = 0x00000000;
			}
		} else {
			if (total_rank == 1) {
				codt |= 0x00800000;
				modt0 = 0x01000000;
				modt1 = 0x00000000;
				modt2 = 0x00000000;
				modt3 = 0x00000000;
			}
			if (total_rank == 2) {
				codt |= 0x02800000;
				modt0 = 0x06000000;
				modt1 = 0x01800000;
				modt2 = 0x00000000;
				modt3 = 0x00000000;
			}
		}
		if (total_dimm == 2) {
			if (total_rank == 2) {
				codt |= 0x08800000;
				modt0 = 0x18000000;
				modt1 = 0x00000000;
				modt2 = 0x01800000;
				modt3 = 0x00000000;
			}
			if (total_rank == 4) {
				codt |= 0x2a800000;
				modt0 = 0x18000000;
				modt1 = 0x18000000;
				modt2 = 0x01800000;
				modt3 = 0x01800000;
			}
		}
  	} else {
		codt |= SDRAM_CODT_DQS_2_5_V_DDR1;
		modt0 = 0x00000000;
		modt1 = 0x00000000;
		modt2 = 0x00000000;
		modt3 = 0x00000000;

		if (total_dimm == 1) {
			if (total_rank == 1)
				codt |= 0x00800000;
			if (total_rank == 2)
				codt |= 0x02800000;
		}
		if (total_dimm == 2) {
			if (total_rank == 2)
				codt |= 0x08800000;
			if (total_rank == 4)
				codt |= 0x2a800000;
		}
	}

	debug("nb of dimm %d\n", total_dimm);
	debug("nb of rank %d\n", total_rank);
	if (total_dimm == 1)
		debug("dimm in slot %d\n", firstSlot);

	mtsdram(SDRAM_CODT, codt);
	mtsdram(SDRAM_MODT0, modt0);
	mtsdram(SDRAM_MODT1, modt1);
	mtsdram(SDRAM_MODT2, modt2);
	mtsdram(SDRAM_MODT3, modt3);
}

/*-----------------------------------------------------------------------------+
 * program_initplr.
 *-----------------------------------------------------------------------------*/
static void program_initplr(unsigned long *dimm_populated,
			    unsigned char *iic0_dimm_addr,
			    unsigned long num_dimm_banks,
                            ddr_cas_id_t selected_cas)
{
	unsigned long MR_CAS_value = 0;

	/******************************************************
	 ** Assumption: if more than one DIMM, all DIMMs are the same
	 **             as already checked in check_memory_type
	 ******************************************************/

	if ((dimm_populated[0] == SDRAM_DDR1) || (dimm_populated[1] == SDRAM_DDR1)) {
		mtsdram(SDRAM_INITPLR0, 0x81B80000);
		mtsdram(SDRAM_INITPLR1, 0x81900400);
		mtsdram(SDRAM_INITPLR2, 0x81810000);
		mtsdram(SDRAM_INITPLR3, 0xff800162);
		mtsdram(SDRAM_INITPLR4, 0x81900400);
		mtsdram(SDRAM_INITPLR5, 0x86080000);
		mtsdram(SDRAM_INITPLR6, 0x86080000);
		mtsdram(SDRAM_INITPLR7, 0x81000062);
	} else if ((dimm_populated[0] == SDRAM_DDR2) || (dimm_populated[1] == SDRAM_DDR2)) {
		switch (selected_cas) {
			/*
			 * The CAS latency is a field of the Mode Reg
			 * that need to be set from caller input.
			 * CAS bits in Mode Reg are starting at bit 4 at least for the Micron DDR2
			 * this is the reason of the shift.
			 */
		case DDR_CAS_3:
			MR_CAS_value = 3 << 4;
			break;
		case DDR_CAS_4:
			MR_CAS_value = 4 << 4;
			break;
		case DDR_CAS_5:
			MR_CAS_value = 5 << 4;
			break;
		default:
			printf("ERROR: ucode error on selected_cas value %d", (unsigned char)selected_cas);
			hang();
			break;
		}

		mtsdram(SDRAM_INITPLR0,  0xB5380000);			/* NOP */
		mtsdram(SDRAM_INITPLR1,  0x82100400);			/* precharge 8 DDR clock cycle */
		mtsdram(SDRAM_INITPLR2,  0x80820000);			/* EMR2 */
		mtsdram(SDRAM_INITPLR3,  0x80830000);			/* EMR3 */
		mtsdram(SDRAM_INITPLR4,  0x80810000);			/* EMR DLL ENABLE */
		mtsdram(SDRAM_INITPLR5,  0x80800502 | MR_CAS_value);	/* MR w/ DLL reset */
		mtsdram(SDRAM_INITPLR6,  0x82100400);			/* precharge 8 DDR clock cycle */
		mtsdram(SDRAM_INITPLR7,  0x8a080000);			/* Refresh  50 DDR clock cycle */
		mtsdram(SDRAM_INITPLR8,  0x8a080000);			/* Refresh  50 DDR clock cycle */
		mtsdram(SDRAM_INITPLR9,  0x8a080000);			/* Refresh  50 DDR clock cycle */
		mtsdram(SDRAM_INITPLR10, 0x8a080000);			/* Refresh  50 DDR clock cycle */
		mtsdram(SDRAM_INITPLR11, 0x80800402 | MR_CAS_value);	/* MR w/o DLL reset */
		mtsdram(SDRAM_INITPLR12, 0x80810380);			/* EMR OCD Default */
		mtsdram(SDRAM_INITPLR13, 0x80810000);			/* EMR OCD Exit */
	} else {
		printf("ERROR: ucode error as unknown DDR type in program_initplr");
		hang();
	}
}

/*------------------------------------------------------------------
 * This routine programs the SDRAM_MMODE register.
 * the selected_cas is an output parameter, that will be passed
 * by caller to call the above program_initplr( )
 *-----------------------------------------------------------------*/
static void program_mode(unsigned long *dimm_populated,
			 unsigned char *iic0_dimm_addr,
			 unsigned long num_dimm_banks,
			 ddr_cas_id_t *selected_cas)
{
	unsigned long dimm_num;
	unsigned long sdram_ddr1;
	unsigned long t_wr_ns;
	unsigned long t_wr_clk;
	unsigned long cas_bit;
	unsigned long cas_index;
	unsigned long sdram_freq;
	unsigned long ddr_check;
	unsigned long mmode;
	unsigned long tcyc_reg;
	unsigned long cycle_2_0_clk;
	unsigned long cycle_2_5_clk;
	unsigned long cycle_3_0_clk;
	unsigned long cycle_4_0_clk;
	unsigned long cycle_5_0_clk;
	unsigned long max_2_0_tcyc_ns_x_100;
	unsigned long max_2_5_tcyc_ns_x_100;
	unsigned long max_3_0_tcyc_ns_x_100;
	unsigned long max_4_0_tcyc_ns_x_100;
	unsigned long max_5_0_tcyc_ns_x_100;
	unsigned long cycle_time_ns_x_100[3];
	PPC440_SYS_INFO board_cfg;
	unsigned char cas_2_0_available;
	unsigned char cas_2_5_available;
	unsigned char cas_3_0_available;
	unsigned char cas_4_0_available;
	unsigned char cas_5_0_available;
	unsigned long sdr_ddrpll;

	/*------------------------------------------------------------------
	 * Get the board configuration info.
	 *-----------------------------------------------------------------*/
	get_sys_info(&board_cfg);

	mfsdr(sdr_ddr0, sdr_ddrpll);
	sdram_freq = MULDIV64((board_cfg.freqPLB), SDR0_DDR0_DDRM_DECODE(sdr_ddrpll), 1);

	/*------------------------------------------------------------------
	 * Handle the timing.  We need to find the worst case timing of all
	 * the dimm modules installed.
	 *-----------------------------------------------------------------*/
	t_wr_ns = 0;
	cas_2_0_available = TRUE;
	cas_2_5_available = TRUE;
	cas_3_0_available = TRUE;
	cas_4_0_available = TRUE;
	cas_5_0_available = TRUE;
	max_2_0_tcyc_ns_x_100 = 10;
	max_2_5_tcyc_ns_x_100 = 10;
	max_3_0_tcyc_ns_x_100 = 10;
	max_4_0_tcyc_ns_x_100 = 10;
	max_5_0_tcyc_ns_x_100 = 10;
	sdram_ddr1 = TRUE;

	/* loop through all the DIMM slots on the board */
	for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
		/* If a dimm is installed in a particular slot ... */
		if (dimm_populated[dimm_num] != SDRAM_NONE) {
			if (dimm_populated[dimm_num] == SDRAM_DDR1)
				sdram_ddr1 = TRUE;
			else
				sdram_ddr1 = FALSE;

			/* t_wr_ns = max(t_wr_ns, (unsigned long)dimm_spd[dimm_num][36] >> 2); */ /*  not used in this loop. */
			cas_bit = spd_read(iic0_dimm_addr[dimm_num], 18);

			/* For a particular DIMM, grab the three CAS values it supports */
			for (cas_index = 0; cas_index < 3; cas_index++) {
				switch (cas_index) {
				case 0:
					tcyc_reg = spd_read(iic0_dimm_addr[dimm_num], 9);
					break;
				case 1:
					tcyc_reg = spd_read(iic0_dimm_addr[dimm_num], 23);
					break;
				default:
					tcyc_reg = spd_read(iic0_dimm_addr[dimm_num], 25);
					break;
				}

				if ((tcyc_reg & 0x0F) >= 10) {
					if ((tcyc_reg & 0x0F) == 0x0D) {
						/* Convert from hex to decimal */
						cycle_time_ns_x_100[cas_index] = (((tcyc_reg & 0xF0) >> 4) * 100) + 75;
					} else {
						printf("ERROR: SPD reported Tcyc is incorrect for DIMM "
						       "in slot %d\n", (unsigned int)dimm_num);
						hang();
					}
				} else {
					/* Convert from hex to decimal */
					cycle_time_ns_x_100[cas_index] = (((tcyc_reg & 0xF0) >> 4) * 100) +
						((tcyc_reg & 0x0F)*10);
				}
			}

			/* The rest of this routine determines if CAS 2.0, 2.5, 3.0, 4.0 and 5.0 are */
			/* supported for a particular DIMM. */
			cas_index = 0;

			if (sdram_ddr1) {
				/*
				 * DDR devices use the following bitmask for CAS latency:
				 *  Bit   7    6    5    4    3    2    1    0
				 *       TBD  4.0  3.5  3.0  2.5  2.0  1.5  1.0
				 */
				if (((cas_bit & 0x40) == 0x40) && (cas_index < 3) && (cycle_time_ns_x_100[cas_index] != 0)) {
					max_4_0_tcyc_ns_x_100 = max(max_4_0_tcyc_ns_x_100, cycle_time_ns_x_100[cas_index]);
					cas_index++;
				} else {
					if (cas_index != 0)
						cas_index++;
					cas_4_0_available = FALSE;
				}

				if (((cas_bit & 0x10) == 0x10) && (cas_index < 3) && (cycle_time_ns_x_100[cas_index] != 0)) {
					max_3_0_tcyc_ns_x_100 = max(max_3_0_tcyc_ns_x_100, cycle_time_ns_x_100[cas_index]);
					cas_index++;
				} else {
					if (cas_index != 0)
						cas_index++;
					cas_3_0_available = FALSE;
				}

				if (((cas_bit & 0x08) == 0x08) && (cas_index < 3) && (cycle_time_ns_x_100[cas_index] != 0)) {
					max_2_5_tcyc_ns_x_100 = max(max_2_5_tcyc_ns_x_100, cycle_time_ns_x_100[cas_index]);
					cas_index++;
				} else {
					if (cas_index != 0)
						cas_index++;
					cas_2_5_available = FALSE;
				}

				if (((cas_bit & 0x04) == 0x04) && (cas_index < 3) && (cycle_time_ns_x_100[cas_index] != 0)) {
					max_2_0_tcyc_ns_x_100 = max(max_2_0_tcyc_ns_x_100, cycle_time_ns_x_100[cas_index]);
					cas_index++;
				} else {
					if (cas_index != 0)
						cas_index++;
					cas_2_0_available = FALSE;
				}
			} else {
				/*
				 * DDR2 devices use the following bitmask for CAS latency:
				 *  Bit   7    6    5    4    3    2    1    0
				 *       TBD  6.0  5.0  4.0  3.0  2.0  TBD  TBD
				 */
				if (((cas_bit & 0x20) == 0x20) && (cas_index < 3) && (cycle_time_ns_x_100[cas_index] != 0)) {
					max_5_0_tcyc_ns_x_100 = max(max_5_0_tcyc_ns_x_100, cycle_time_ns_x_100[cas_index]);
					cas_index++;
				} else {
					if (cas_index != 0)
						cas_index++;
					cas_5_0_available = FALSE;
				}

				if (((cas_bit & 0x10) == 0x10) && (cas_index < 3) && (cycle_time_ns_x_100[cas_index] != 0)) {
					max_4_0_tcyc_ns_x_100 = max(max_4_0_tcyc_ns_x_100, cycle_time_ns_x_100[cas_index]);
					cas_index++;
				} else {
					if (cas_index != 0)
						cas_index++;
					cas_4_0_available = FALSE;
				}

				if (((cas_bit & 0x08) == 0x08) && (cas_index < 3) && (cycle_time_ns_x_100[cas_index] != 0)) {
					max_3_0_tcyc_ns_x_100 = max(max_3_0_tcyc_ns_x_100, cycle_time_ns_x_100[cas_index]);
					cas_index++;
				} else {
					if (cas_index != 0)
						cas_index++;
					cas_3_0_available = FALSE;
				}
			}
		}
	}

	/*------------------------------------------------------------------
	 * Set the SDRAM mode, SDRAM_MMODE
	 *-----------------------------------------------------------------*/
	mfsdram(SDRAM_MMODE, mmode);
	mmode = mmode & ~(SDRAM_MMODE_WR_MASK | SDRAM_MMODE_DCL_MASK);

	cycle_2_0_clk = MULDIV64(ONE_BILLION, 100, max_2_0_tcyc_ns_x_100);
	cycle_2_5_clk = MULDIV64(ONE_BILLION, 100, max_2_5_tcyc_ns_x_100);
	cycle_3_0_clk = MULDIV64(ONE_BILLION, 100, max_3_0_tcyc_ns_x_100);
	cycle_4_0_clk = MULDIV64(ONE_BILLION, 100, max_4_0_tcyc_ns_x_100);
	cycle_5_0_clk = MULDIV64(ONE_BILLION, 100, max_5_0_tcyc_ns_x_100);

	if (sdram_ddr1 == TRUE) { /* DDR1 */
		if ((cas_2_0_available == TRUE) && (sdram_freq <= cycle_2_0_clk)) {
			mmode |= SDRAM_MMODE_DCL_DDR1_2_0_CLK;
			*selected_cas = DDR_CAS_2;
		} else if ((cas_2_5_available == TRUE) && (sdram_freq <= cycle_2_5_clk)) {
			mmode |= SDRAM_MMODE_DCL_DDR1_2_5_CLK;
			*selected_cas = DDR_CAS_2_5;
		} else if ((cas_3_0_available == TRUE) && (sdram_freq <= cycle_3_0_clk)) {
			mmode |= SDRAM_MMODE_DCL_DDR1_3_0_CLK;
			*selected_cas = DDR_CAS_3;
		} else {
			printf("ERROR: Cannot find a supported CAS latency with the installed DIMMs.\n");
			printf("Only DIMMs DDR1 with CAS latencies of 2.0, 2.5, and 3.0 are supported.\n");
			printf("Make sure the PLB speed is within the supported range of the DIMMs.\n\n");
			hang();
		}
	} else { /* DDR2 */
		if ((cas_3_0_available == TRUE) && (sdram_freq <= cycle_3_0_clk)) {
			mmode |= SDRAM_MMODE_DCL_DDR2_3_0_CLK;
			*selected_cas = DDR_CAS_3;
		} else if ((cas_4_0_available == TRUE) && (sdram_freq <= cycle_4_0_clk)) {
			mmode |= SDRAM_MMODE_DCL_DDR2_4_0_CLK;
			*selected_cas = DDR_CAS_4;
		} else if ((cas_5_0_available == TRUE) && (sdram_freq <= cycle_5_0_clk)) {
			mmode |= SDRAM_MMODE_DCL_DDR2_5_0_CLK;
			*selected_cas = DDR_CAS_5;
		} else {
			printf("ERROR: Cannot find a supported CAS latency with the installed DIMMs.\n");
			printf("Only DIMMs DDR2 with CAS latencies of 3.0, 4.0, and 5.0 are supported.\n");
			printf("Make sure the PLB speed is within the supported range of the DIMMs.\n\n");
			hang();
		}
	}

	if (sdram_ddr1 == TRUE)
		mmode |= SDRAM_MMODE_WR_DDR1;
	else {

		/* loop through all the DIMM slots on the board */
		for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
			/* If a dimm is installed in a particular slot ... */
			if (dimm_populated[dimm_num] != SDRAM_NONE)
				t_wr_ns = max(t_wr_ns,
					      spd_read(iic0_dimm_addr[dimm_num], 36) >> 2);
		}

		/*
		 * convert from nanoseconds to ddr clocks
		 * round up if necessary
		 */
		t_wr_clk = MULDIV64(sdram_freq, t_wr_ns, ONE_BILLION);
		ddr_check = MULDIV64(ONE_BILLION, t_wr_clk, t_wr_ns);
		if (sdram_freq != ddr_check)
			t_wr_clk++;

		switch (t_wr_clk) {
		case 0:
		case 1:
		case 2:
		case 3:
			mmode |= SDRAM_MMODE_WR_DDR2_3_CYC;
			break;
		case 4:
			mmode |= SDRAM_MMODE_WR_DDR2_4_CYC;
			break;
		case 5:
			mmode |= SDRAM_MMODE_WR_DDR2_5_CYC;
			break;
		default:
			mmode |= SDRAM_MMODE_WR_DDR2_6_CYC;
			break;
		}
	}

	mtsdram(SDRAM_MMODE, mmode);
}

/*-----------------------------------------------------------------------------+
 * program_rtr.
 *-----------------------------------------------------------------------------*/
static void program_rtr(unsigned long *dimm_populated,
			unsigned char *iic0_dimm_addr,
			unsigned long num_dimm_banks)
{
	PPC440_SYS_INFO board_cfg;
	unsigned long max_refresh_rate;
	unsigned long dimm_num;
	unsigned long refresh_rate_type;
	unsigned long refresh_rate;
	unsigned long rint;
	unsigned long sdram_freq;
	unsigned long sdr_ddrpll;
	unsigned long val;

	/*------------------------------------------------------------------
	 * Get the board configuration info.
	 *-----------------------------------------------------------------*/
	get_sys_info(&board_cfg);

	/*------------------------------------------------------------------
	 * Set the SDRAM Refresh Timing Register, SDRAM_RTR
	 *-----------------------------------------------------------------*/
	mfsdr(sdr_ddr0, sdr_ddrpll);
	sdram_freq = ((board_cfg.freqPLB) * SDR0_DDR0_DDRM_DECODE(sdr_ddrpll));

	max_refresh_rate = 0;
	for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
		if (dimm_populated[dimm_num] != SDRAM_NONE) {

			refresh_rate_type = spd_read(iic0_dimm_addr[dimm_num], 12);
			refresh_rate_type &= 0x7F;
			switch (refresh_rate_type) {
			case 0:
				refresh_rate =  15625;
				break;
			case 1:
				refresh_rate =   3906;
				break;
			case 2:
				refresh_rate =   7812;
				break;
			case 3:
				refresh_rate =  31250;
				break;
			case 4:
				refresh_rate =  62500;
				break;
			case 5:
				refresh_rate = 125000;
				break;
			default:
				refresh_rate = 0;
				printf("ERROR: DIMM %d unsupported refresh rate/type.\n",
				       (unsigned int)dimm_num);
				printf("Replace the DIMM module with a supported DIMM.\n\n");
				hang();
				break;
			}

			max_refresh_rate = max(max_refresh_rate, refresh_rate);
		}
	}

	rint = MULDIV64(sdram_freq, max_refresh_rate, ONE_BILLION);
	mfsdram(SDRAM_RTR, val);
	mtsdram(SDRAM_RTR, (val & ~SDRAM_RTR_RINT_MASK) |
		(SDRAM_RTR_RINT_ENCODE(rint)));
}

/*------------------------------------------------------------------
 * This routine programs the SDRAM_TRx registers.
 *-----------------------------------------------------------------*/
static void program_tr(unsigned long *dimm_populated,
		       unsigned char *iic0_dimm_addr,
		       unsigned long num_dimm_banks)
{
	unsigned long dimm_num;
	unsigned long sdram_ddr1;
	unsigned long t_rp_ns;
	unsigned long t_rcd_ns;
	unsigned long t_rrd_ns;
	unsigned long t_ras_ns;
	unsigned long t_rc_ns;
	unsigned long t_rfc_ns;
	unsigned long t_wpc_ns;
	unsigned long t_wtr_ns;
	unsigned long t_rpc_ns;
	unsigned long t_rp_clk;
	unsigned long t_rcd_clk;
	unsigned long t_rrd_clk;
	unsigned long t_ras_clk;
	unsigned long t_rc_clk;
	unsigned long t_rfc_clk;
	unsigned long t_wpc_clk;
	unsigned long t_wtr_clk;
	unsigned long t_rpc_clk;
	unsigned long sdtr1, sdtr2, sdtr3;
	unsigned long ddr_check;
	unsigned long sdram_freq;
	unsigned long sdr_ddrpll;

	PPC440_SYS_INFO board_cfg;

	/*------------------------------------------------------------------
	 * Get the board configuration info.
	 *-----------------------------------------------------------------*/
	get_sys_info(&board_cfg);

	mfsdr(sdr_ddr0, sdr_ddrpll);
	sdram_freq = ((board_cfg.freqPLB) * SDR0_DDR0_DDRM_DECODE(sdr_ddrpll));

	/*------------------------------------------------------------------
	 * Handle the timing.  We need to find the worst case timing of all
	 * the dimm modules installed.
	 *-----------------------------------------------------------------*/
	t_rp_ns = 0;
	t_rrd_ns = 0;
	t_rcd_ns = 0;
	t_ras_ns = 0;
	t_rc_ns = 0;
	t_rfc_ns = 0;
	t_wpc_ns = 0;
	t_wtr_ns = 0;
	t_rpc_ns = 0;
	sdram_ddr1 = TRUE;

	/* loop through all the DIMM slots on the board */
	for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
		/* If a dimm is installed in a particular slot ... */
		if (dimm_populated[dimm_num] != SDRAM_NONE) {
			if (dimm_populated[dimm_num] == SDRAM_DDR2)
				sdram_ddr1 = TRUE;
			else
				sdram_ddr1 = FALSE;

			t_rcd_ns = max(t_rcd_ns, spd_read(iic0_dimm_addr[dimm_num], 29) >> 2);
			t_rrd_ns = max(t_rrd_ns, spd_read(iic0_dimm_addr[dimm_num], 28) >> 2);
			t_rp_ns  = max(t_rp_ns,  spd_read(iic0_dimm_addr[dimm_num], 27) >> 2);
			t_ras_ns = max(t_ras_ns, spd_read(iic0_dimm_addr[dimm_num], 30));
			t_rc_ns  = max(t_rc_ns,  spd_read(iic0_dimm_addr[dimm_num], 41));
			t_rfc_ns = max(t_rfc_ns, spd_read(iic0_dimm_addr[dimm_num], 42));
		}
	}

	/*------------------------------------------------------------------
	 * Set the SDRAM Timing Reg 1, SDRAM_TR1
	 *-----------------------------------------------------------------*/
	mfsdram(SDRAM_SDTR1, sdtr1);
	sdtr1 &= ~(SDRAM_SDTR1_LDOF_MASK | SDRAM_SDTR1_RTW_MASK |
		   SDRAM_SDTR1_WTWO_MASK | SDRAM_SDTR1_RTRO_MASK);

	/* default values */
	sdtr1 |= SDRAM_SDTR1_LDOF_2_CLK;
	sdtr1 |= SDRAM_SDTR1_RTW_2_CLK;

	/* normal operations */
	sdtr1 |= SDRAM_SDTR1_WTWO_0_CLK;
	sdtr1 |= SDRAM_SDTR1_RTRO_1_CLK;

	mtsdram(SDRAM_SDTR1, sdtr1);

	/*------------------------------------------------------------------
	 * Set the SDRAM Timing Reg 2, SDRAM_TR2
	 *-----------------------------------------------------------------*/
	mfsdram(SDRAM_SDTR2, sdtr2);
	sdtr2 &= ~(SDRAM_SDTR2_RCD_MASK  | SDRAM_SDTR2_WTR_MASK |
		   SDRAM_SDTR2_XSNR_MASK | SDRAM_SDTR2_WPC_MASK |
		   SDRAM_SDTR2_RPC_MASK  | SDRAM_SDTR2_RP_MASK  |
		   SDRAM_SDTR2_RRD_MASK);

	/*
	 * convert t_rcd from nanoseconds to ddr clocks
	 * round up if necessary
	 */
	t_rcd_clk = MULDIV64(sdram_freq, t_rcd_ns, ONE_BILLION);
	ddr_check = MULDIV64(ONE_BILLION, t_rcd_clk, t_rcd_ns);
	if (sdram_freq != ddr_check)
		t_rcd_clk++;

	switch (t_rcd_clk) {
	case 0:
	case 1:
		sdtr2 |= SDRAM_SDTR2_RCD_1_CLK;
		break;
	case 2:
		sdtr2 |= SDRAM_SDTR2_RCD_2_CLK;
		break;
	case 3:
		sdtr2 |= SDRAM_SDTR2_RCD_3_CLK;
		break;
	case 4:
		sdtr2 |= SDRAM_SDTR2_RCD_4_CLK;
		break;
	default:
		sdtr2 |= SDRAM_SDTR2_RCD_5_CLK;
		break;
	}

	if (sdram_ddr1 == TRUE) { /* DDR1 */
		if (sdram_freq < 200000000) {
			sdtr2 |= SDRAM_SDTR2_WTR_1_CLK;
			sdtr2 |= SDRAM_SDTR2_WPC_2_CLK;
			sdtr2 |= SDRAM_SDTR2_RPC_2_CLK;
		} else {
			sdtr2 |= SDRAM_SDTR2_WTR_2_CLK;
			sdtr2 |= SDRAM_SDTR2_WPC_3_CLK;
			sdtr2 |= SDRAM_SDTR2_RPC_2_CLK;
		}
	} else { /* DDR2 */
		/* loop through all the DIMM slots on the board */
		for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
			/* If a dimm is installed in a particular slot ... */
			if (dimm_populated[dimm_num] != SDRAM_NONE) {
				t_wpc_ns = max(t_wtr_ns, spd_read(iic0_dimm_addr[dimm_num], 36) >> 2);
				t_wtr_ns = max(t_wtr_ns, spd_read(iic0_dimm_addr[dimm_num], 37) >> 2);
				t_rpc_ns = max(t_rpc_ns, spd_read(iic0_dimm_addr[dimm_num], 38) >> 2);
			}
		}

		/*
		 * convert from nanoseconds to ddr clocks
		 * round up if necessary
		 */
		t_wpc_clk = MULDIV64(sdram_freq, t_wpc_ns, ONE_BILLION);
		ddr_check = MULDIV64(ONE_BILLION, t_wpc_clk, t_wpc_ns);
		if (sdram_freq != ddr_check)
			t_wpc_clk++;

		switch (t_wpc_clk) {
		case 0:
		case 1:
		case 2:
			sdtr2 |= SDRAM_SDTR2_WPC_2_CLK;
			break;
		case 3:
			sdtr2 |= SDRAM_SDTR2_WPC_3_CLK;
			break;
		case 4:
			sdtr2 |= SDRAM_SDTR2_WPC_4_CLK;
			break;
		case 5:
			sdtr2 |= SDRAM_SDTR2_WPC_5_CLK;
			break;
		default:
			sdtr2 |= SDRAM_SDTR2_WPC_6_CLK;
			break;
		}

		/*
		 * convert from nanoseconds to ddr clocks
		 * round up if necessary
		 */
		t_wtr_clk = MULDIV64(sdram_freq, t_wtr_ns, ONE_BILLION);
		ddr_check = MULDIV64(ONE_BILLION, t_wtr_clk, t_wtr_ns);
		if (sdram_freq != ddr_check)
			t_wtr_clk++;

		switch (t_wtr_clk) {
		case 0:
		case 1:
			sdtr2 |= SDRAM_SDTR2_WTR_1_CLK;
			break;
		case 2:
			sdtr2 |= SDRAM_SDTR2_WTR_2_CLK;
			break;
		case 3:
			sdtr2 |= SDRAM_SDTR2_WTR_3_CLK;
			break;
		default:
			sdtr2 |= SDRAM_SDTR2_WTR_4_CLK;
			break;
		}

		/*
		 * convert from nanoseconds to ddr clocks
		 * round up if necessary
		 */
		t_rpc_clk = MULDIV64(sdram_freq, t_rpc_ns, ONE_BILLION);
		ddr_check = MULDIV64(ONE_BILLION, t_rpc_clk, t_rpc_ns);
		if (sdram_freq != ddr_check)
			t_rpc_clk++;

		switch (t_rpc_clk) {
		case 0:
		case 1:
		case 2:
			sdtr2 |= SDRAM_SDTR2_RPC_2_CLK;
			break;
		case 3:
			sdtr2 |= SDRAM_SDTR2_RPC_3_CLK;
			break;
		default:
			sdtr2 |= SDRAM_SDTR2_RPC_4_CLK;
			break;
		}
	}

	/* default value */
	sdtr2 |= SDRAM_SDTR2_XSNR_16_CLK;

	/*
	 * convert t_rrd from nanoseconds to ddr clocks
	 * round up if necessary
	 */
	t_rrd_clk = MULDIV64(sdram_freq, t_rrd_ns, ONE_BILLION);
	ddr_check = MULDIV64(ONE_BILLION, t_rrd_clk, t_rrd_ns);
	if (sdram_freq != ddr_check)
		t_rrd_clk++;

	if (t_rrd_clk == 3)
		sdtr2 |= SDRAM_SDTR2_RRD_3_CLK;
	else
		sdtr2 |= SDRAM_SDTR2_RRD_2_CLK;

	/*
	 * convert t_rp from nanoseconds to ddr clocks
	 * round up if necessary
	 */
	t_rp_clk = MULDIV64(sdram_freq, t_rp_ns, ONE_BILLION);
	ddr_check = MULDIV64(ONE_BILLION, t_rp_clk, t_rp_ns);
	if (sdram_freq != ddr_check)
		t_rp_clk++;

	switch (t_rp_clk) {
	case 0:
	case 1:
	case 2:
	case 3:
		sdtr2 |= SDRAM_SDTR2_RP_3_CLK;
		break;
	case 4:
		sdtr2 |= SDRAM_SDTR2_RP_4_CLK;
		break;
	case 5:
		sdtr2 |= SDRAM_SDTR2_RP_5_CLK;
		break;
	case 6:
		sdtr2 |= SDRAM_SDTR2_RP_6_CLK;
		break;
	default:
		sdtr2 |= SDRAM_SDTR2_RP_7_CLK;
		break;
	}

	mtsdram(SDRAM_SDTR2, sdtr2);

	/*------------------------------------------------------------------
	 * Set the SDRAM Timing Reg 3, SDRAM_TR3
	 *-----------------------------------------------------------------*/
	mfsdram(SDRAM_SDTR3, sdtr3);
	sdtr3 &= ~(SDRAM_SDTR3_RAS_MASK  | SDRAM_SDTR3_RC_MASK |
		   SDRAM_SDTR3_XCS_MASK | SDRAM_SDTR3_RFC_MASK);

	/*
	 * convert t_ras from nanoseconds to ddr clocks
	 * round up if necessary
	 */
	t_ras_clk = MULDIV64(sdram_freq, t_ras_ns, ONE_BILLION);
	ddr_check = MULDIV64(ONE_BILLION, t_ras_clk, t_ras_ns);
	if (sdram_freq != ddr_check)
		t_ras_clk++;

	sdtr3 |= SDRAM_SDTR3_RAS_ENCODE(t_ras_clk);

	/*
	 * convert t_rc from nanoseconds to ddr clocks
	 * round up if necessary
	 */
	t_rc_clk = MULDIV64(sdram_freq, t_rc_ns, ONE_BILLION);
	ddr_check = MULDIV64(ONE_BILLION, t_rc_clk, t_rc_ns);
	if (sdram_freq != ddr_check)
		t_rc_clk++;

	sdtr3 |= SDRAM_SDTR3_RC_ENCODE(t_rc_clk);

	/* default xcs value */
	sdtr3 |= SDRAM_SDTR3_XCS;

	/*
	 * convert t_rfc from nanoseconds to ddr clocks
	 * round up if necessary
	 */
	t_rfc_clk = MULDIV64(sdram_freq, t_rfc_ns, ONE_BILLION);
	ddr_check = MULDIV64(ONE_BILLION, t_rfc_clk, t_rfc_ns);
	if (sdram_freq != ddr_check)
		t_rfc_clk++;

	sdtr3 |= SDRAM_SDTR3_RFC_ENCODE(t_rfc_clk);

	mtsdram(SDRAM_SDTR3, sdtr3);
}

/*-----------------------------------------------------------------------------+
 * program_bxcf.
 *-----------------------------------------------------------------------------*/
static void program_bxcf(unsigned long *dimm_populated,
			 unsigned char *iic0_dimm_addr,
			 unsigned long num_dimm_banks)
{
	unsigned long dimm_num;
	unsigned long num_col_addr;
	unsigned long num_ranks;
	unsigned long num_banks;
	unsigned long mode;
	unsigned long ind_rank;
	unsigned long ind;
	unsigned long ind_bank;
	unsigned long bank_0_populated;

	/*------------------------------------------------------------------
	 * Set the BxCF regs.  First, wipe out the bank config registers.
	 *-----------------------------------------------------------------*/
	mtdcr(SDRAMC_CFGADDR, SDRAM_MB0CF);
	mtdcr(SDRAMC_CFGDATA, 0x00000000);
	mtdcr(SDRAMC_CFGADDR, SDRAM_MB1CF);
	mtdcr(SDRAMC_CFGDATA, 0x00000000);
	mtdcr(SDRAMC_CFGADDR, SDRAM_MB2CF);
	mtdcr(SDRAMC_CFGDATA, 0x00000000);
	mtdcr(SDRAMC_CFGADDR, SDRAM_MB3CF);
	mtdcr(SDRAMC_CFGDATA, 0x00000000);

	mode = SDRAM_BXCF_M_BE_ENABLE;

	bank_0_populated = 0;

	for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
		if (dimm_populated[dimm_num] != SDRAM_NONE) {
			num_col_addr = spd_read(iic0_dimm_addr[dimm_num], 4);
			num_ranks = spd_read(iic0_dimm_addr[dimm_num], 5);
			if ((spd_read(iic0_dimm_addr[dimm_num], 2)) == 0x08)
				num_ranks = (num_ranks & 0x0F) +1;
			else
				num_ranks = num_ranks & 0x0F;

			num_banks = spd_read(iic0_dimm_addr[dimm_num], 17);

			for (ind_bank = 0; ind_bank < 2; ind_bank++) {
				if (num_banks == 4)
					ind = 0;
				else
					ind = 5;
				switch (num_col_addr) {
				case 0x08:
					mode |= (SDRAM_BXCF_M_AM_0 + ind);
					break;
				case 0x09:
					mode |= (SDRAM_BXCF_M_AM_1 + ind);
					break;
				case 0x0A:
					mode |= (SDRAM_BXCF_M_AM_2 + ind);
					break;
				case 0x0B:
					mode |= (SDRAM_BXCF_M_AM_3 + ind);
					break;
				case 0x0C:
					mode |= (SDRAM_BXCF_M_AM_4 + ind);
					break;
				default:
					printf("DDR-SDRAM: DIMM %d BxCF configuration.\n",
					       (unsigned int)dimm_num);
					printf("ERROR: Unsupported value for number of "
					       "column addresses: %d.\n", (unsigned int)num_col_addr);
					printf("Replace the DIMM module with a supported DIMM.\n\n");
					hang();
				}
			}

			if ((dimm_populated[dimm_num] != SDRAM_NONE)&& (dimm_num ==1))
				bank_0_populated = 1;

			for (ind_rank = 0; ind_rank < num_ranks; ind_rank++) {
				mtdcr(SDRAMC_CFGADDR, SDRAM_MB0CF + ((dimm_num + bank_0_populated + ind_rank) << 2));
				mtdcr(SDRAMC_CFGDATA, mode);
			}
		}
	}
}

/*------------------------------------------------------------------
 * program memory queue.
 *-----------------------------------------------------------------*/
static void program_memory_queue(unsigned long *dimm_populated,
				 unsigned char *iic0_dimm_addr,
				 unsigned long num_dimm_banks)
{
	unsigned long dimm_num;
	unsigned long rank_base_addr;
	unsigned long rank_reg;
	unsigned long rank_size_bytes;
	unsigned long rank_size_id;
	unsigned long num_ranks;
	unsigned long baseadd_size;
	unsigned long i;
	unsigned long bank_0_populated = 0;

	/*------------------------------------------------------------------
	 * Reset the rank_base_address.
	 *-----------------------------------------------------------------*/
	rank_reg   = SDRAM_R0BAS;

	rank_base_addr = 0x00000000;

	for (dimm_num = 0; dimm_num < num_dimm_banks; dimm_num++) {
		if (dimm_populated[dimm_num] != SDRAM_NONE) {
			num_ranks = spd_read(iic0_dimm_addr[dimm_num], 5);
			if ((spd_read(iic0_dimm_addr[dimm_num], 2)) == 0x08)
				num_ranks = (num_ranks & 0x0F) + 1;
			else
				num_ranks = num_ranks & 0x0F;

			rank_size_id = spd_read(iic0_dimm_addr[dimm_num], 31);

			/*------------------------------------------------------------------
			 * Set the sizes
			 *-----------------------------------------------------------------*/
			baseadd_size = 0;
			rank_size_bytes = 1024 * 1024 * rank_size_id;
			switch (rank_size_id) {
			case 0x02:
				baseadd_size |= SDRAM_RXBAS_SDSZ_8;
				break;
			case 0x04:
				baseadd_size |= SDRAM_RXBAS_SDSZ_16;
				break;
			case 0x08:
				baseadd_size |= SDRAM_RXBAS_SDSZ_32;
				break;
			case 0x10:
				baseadd_size |= SDRAM_RXBAS_SDSZ_64;
				break;
			case 0x20:
				baseadd_size |= SDRAM_RXBAS_SDSZ_128;
				break;
			case 0x40:
				baseadd_size |= SDRAM_RXBAS_SDSZ_256;
				break;
			case 0x80:
				baseadd_size |= SDRAM_RXBAS_SDSZ_512;
				break;
			default:
				printf("DDR-SDRAM: DIMM %d memory queue configuration.\n",
				       (unsigned int)dimm_num);
				printf("ERROR: Unsupported value for the banksize: %d.\n",
				       (unsigned int)rank_size_id);
				printf("Replace the DIMM module with a supported DIMM.\n\n");
				hang();
			}

			if ((dimm_populated[dimm_num] != SDRAM_NONE) && (dimm_num == 1))
				bank_0_populated = 1;

			for (i = 0; i < num_ranks; i++)	{
				mtdcr_any(rank_reg+i+dimm_num+bank_0_populated,
					  (rank_base_addr & SDRAM_RXBAS_SDBA_MASK) |
					  baseadd_size);
				rank_base_addr += rank_size_bytes;
			}
		}
	}
}

/*-----------------------------------------------------------------------------+
 * is_ecc_enabled.
 *-----------------------------------------------------------------------------*/
static unsigned long is_ecc_enabled(void)
{
	unsigned long dimm_num;
	unsigned long ecc;
	unsigned long val;

	ecc = 0;
	/* loop through all the DIMM slots on the board */
	for (dimm_num = 0; dimm_num < MAXDIMMS; dimm_num++) {
		mfsdram(SDRAM_MCOPT1, val);
		ecc = max(ecc, SDRAM_MCOPT1_MCHK_CHK_DECODE(val));
	}

	return(ecc);
}

/*-----------------------------------------------------------------------------+
 * program_ecc.
 *-----------------------------------------------------------------------------*/
static void program_ecc(unsigned long *dimm_populated,
			unsigned char *iic0_dimm_addr,
			unsigned long num_dimm_banks)
{
	unsigned long mcopt1;
	unsigned long mcopt2;
	unsigned long mcstat;
	unsigned long dimm_num;
	unsigned long ecc;

	ecc = 0;
	/* loop through all the DIMM slots on the board */
	for (dimm_num = 0; dimm_num < MAXDIMMS; dimm_num++) {
		/* If a dimm is installed in a particular slot ... */
		if (dimm_populated[dimm_num] != SDRAM_NONE)
			ecc = max(ecc, spd_read(iic0_dimm_addr[dimm_num], 11));
	}
	if (ecc == 0)
		return;

	mfsdram(SDRAM_MCOPT1, mcopt1);
	mfsdram(SDRAM_MCOPT2, mcopt2);

	if ((mcopt1 & SDRAM_MCOPT1_MCHK_MASK) != SDRAM_MCOPT1_MCHK_NON) {
		/* DDR controller must be enabled and not in self-refresh. */
		mfsdram(SDRAM_MCSTAT, mcstat);
		if (((mcopt2 & SDRAM_MCOPT2_DCEN_MASK) == SDRAM_MCOPT2_DCEN_ENABLE)
		    && ((mcopt2 & SDRAM_MCOPT2_SREN_MASK) == SDRAM_MCOPT2_SREN_EXIT)
		    && ((mcstat & (SDRAM_MCSTAT_MIC_MASK | SDRAM_MCSTAT_SRMS_MASK))
			== (SDRAM_MCSTAT_MIC_COMP | SDRAM_MCSTAT_SRMS_NOT_SF))) {

			program_ecc_addr(0, sdram_memsize());
		}
	}

	return;
}

/*-----------------------------------------------------------------------------+
 * program_ecc_addr.
 *-----------------------------------------------------------------------------*/
static void program_ecc_addr(unsigned long start_address,
			     unsigned long num_bytes)
{
	unsigned long current_address;
	unsigned long end_address;
	unsigned long address_increment;
	unsigned long mcopt1;

	current_address = start_address;
	mfsdram(SDRAM_MCOPT1, mcopt1);
	if ((mcopt1 & SDRAM_MCOPT1_MCHK_MASK) != SDRAM_MCOPT1_MCHK_NON) {
		mtsdram(SDRAM_MCOPT1,
			(mcopt1 & ~SDRAM_MCOPT1_MCHK_MASK) | SDRAM_MCOPT1_MCHK_GEN);
		sync();
		eieio();
		wait_ddr_idle();

		/* ECC bit set method for non-cached memory */
		if ((mcopt1 & SDRAM_MCOPT1_DMWD_MASK) == SDRAM_MCOPT1_DMWD_32)
			address_increment = 4;
		else
			address_increment = 8;
		end_address = current_address + num_bytes;

		while (current_address < end_address) {
			*((unsigned long *)current_address) = 0x00000000;
			current_address += address_increment;
		}
		sync();
		eieio();
		wait_ddr_idle();

		mtsdram(SDRAM_MCOPT1,
			(mcopt1 & ~SDRAM_MCOPT1_MCHK_MASK) | SDRAM_MCOPT1_MCHK_CHK);
		sync();
		eieio();
		wait_ddr_idle();
	}
}

/*-----------------------------------------------------------------------------+
 * program_DQS_calibration.
 *-----------------------------------------------------------------------------*/
static void program_DQS_calibration(unsigned long *dimm_populated,
				    unsigned char *iic0_dimm_addr,
				    unsigned long num_dimm_banks)
{
	unsigned long val;

#ifdef HARD_CODED_DQS /* calibration test with hardvalues */
	mtsdram(SDRAM_RQDC, 0x80000037);
	mtsdram(SDRAM_RDCC, 0x40000000);
	mtsdram(SDRAM_RFDC, 0x000001DF);

	test();
#else
	/*------------------------------------------------------------------
	 * Program RDCC register
	 * Read sample cycle auto-update enable
	 *-----------------------------------------------------------------*/

	/*
	 * Modified for the Katmai platform:  with some DIMMs, the DDR2
	 * controller automatically selects the T2 read cycle, but this
	 * proves unreliable.  Go ahead and force the DDR2 controller
	 * to use the T4 sample and disable the automatic update of the
	 * RDSS field.
	 */
	mfsdram(SDRAM_RDCC, val);
	mtsdram(SDRAM_RDCC,
		(val & ~(SDRAM_RDCC_RDSS_MASK | SDRAM_RDCC_RSAE_MASK))
		| (SDRAM_RDCC_RDSS_T4 | SDRAM_RDCC_RSAE_DISABLE));

	/*------------------------------------------------------------------
	 * Program RQDC register
	 * Internal DQS delay mechanism enable
	 *-----------------------------------------------------------------*/
	mtsdram(SDRAM_RQDC, (SDRAM_RQDC_RQDE_ENABLE|SDRAM_RQDC_RQFD_ENCODE(0x38)));

	/*------------------------------------------------------------------
	 * Program RFDC register
	 * Set Feedback Fractional Oversample
	 * Auto-detect read sample cycle enable
	 *-----------------------------------------------------------------*/
	mfsdram(SDRAM_RFDC, val);
	mtsdram(SDRAM_RFDC,
		(val & ~(SDRAM_RFDC_ARSE_MASK | SDRAM_RFDC_RFOS_MASK |
			 SDRAM_RFDC_RFFD_MASK))
		| (SDRAM_RFDC_ARSE_ENABLE | SDRAM_RFDC_RFOS_ENCODE(0) |
		   SDRAM_RFDC_RFFD_ENCODE(0)));

	DQS_calibration_process();
#endif
}

static u32 short_mem_test(void)
{
	u32 *membase;
	u32 bxcr_num;
	u32 bxcf;
	int i;
	int j;
	u32 test[NUMMEMTESTS][NUMMEMWORDS] = {
		{0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
		 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
		{0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000,
		 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000},
		{0xAAAAAAAA, 0xAAAAAAAA, 0x55555555, 0x55555555,
		 0xAAAAAAAA, 0xAAAAAAAA, 0x55555555, 0x55555555},
		{0x55555555, 0x55555555, 0xAAAAAAAA, 0xAAAAAAAA,
		 0x55555555, 0x55555555, 0xAAAAAAAA, 0xAAAAAAAA},
		{0xA5A5A5A5, 0xA5A5A5A5, 0x5A5A5A5A, 0x5A5A5A5A,
		 0xA5A5A5A5, 0xA5A5A5A5, 0x5A5A5A5A, 0x5A5A5A5A},
		{0x5A5A5A5A, 0x5A5A5A5A, 0xA5A5A5A5, 0xA5A5A5A5,
		 0x5A5A5A5A, 0x5A5A5A5A, 0xA5A5A5A5, 0xA5A5A5A5},
		{0xAA55AA55, 0xAA55AA55, 0x55AA55AA, 0x55AA55AA,
		 0xAA55AA55, 0xAA55AA55, 0x55AA55AA, 0x55AA55AA},
		{0x55AA55AA, 0x55AA55AA, 0xAA55AA55, 0xAA55AA55,
		 0x55AA55AA, 0x55AA55AA, 0xAA55AA55, 0xAA55AA55} };

	for (bxcr_num = 0; bxcr_num < MAXBXCF; bxcr_num++) {
		mfsdram(SDRAM_MB0CF + (bxcr_num << 2), bxcf);

		/* Banks enabled */
		if ((bxcf & SDRAM_BXCF_M_BE_MASK) == SDRAM_BXCF_M_BE_ENABLE) {

			/* Bank is enabled */
			membase = (u32 *)(SDRAM_RXBAS_SDBA_DECODE(mfdcr_any(SDRAM_R0BAS+bxcr_num)));

			/*------------------------------------------------------------------
			 * Run the short memory test.
			 *-----------------------------------------------------------------*/
			for (i = 0; i < NUMMEMTESTS; i++) {
				for (j = 0; j < NUMMEMWORDS; j++) {
					membase[j] = test[i][j];
					ppcDcbf((u32)&(membase[j]));
				}
				sync();
				for (j = 0; j < NUMMEMWORDS; j++) {
					if (membase[j] != test[i][j]) {
						ppcDcbf((u32)&(membase[j]));
						break;
					}
					ppcDcbf((u32)&(membase[j]));
				}
				sync();
				if (j < NUMMEMWORDS)
					break;
			}
			if (i < NUMMEMTESTS)
				break;
		}	/* if bank enabled */
	}		/* for bxcf_num */

	return bxcr_num;
}

#ifndef HARD_CODED_DQS
/*-----------------------------------------------------------------------------+
 * DQS_calibration_process.
 *-----------------------------------------------------------------------------*/
static void DQS_calibration_process(void)
{
	unsigned long ecc_temp;
	unsigned long rfdc_reg;
	unsigned long rffd;
	unsigned long rqdc_reg;
	unsigned long rqfd;
	unsigned long bxcr_num;
	unsigned long val;
	long rqfd_average;
	long rffd_average;
	long max_start;
	long min_end;
	unsigned long begin_rqfd[MAXRANKS];
	unsigned long begin_rffd[MAXRANKS];
	unsigned long end_rqfd[MAXRANKS];
	unsigned long end_rffd[MAXRANKS];
	char window_found;
	unsigned long dlycal;
	unsigned long dly_val;
	unsigned long max_pass_length;
	unsigned long current_pass_length;
	unsigned long current_fail_length;
	unsigned long current_start;
	long max_end;
	unsigned char fail_found;
	unsigned char pass_found;

	/*------------------------------------------------------------------
	 * Test to determine the best read clock delay tuning bits.
	 *
	 * Before the DDR controller can be used, the read clock delay needs to be
	 * set.  This is SDRAM_RQDC[RQFD] and SDRAM_RFDC[RFFD].
	 * This value cannot be hardcoded into the program because it changes
	 * depending on the board's setup and environment.
	 * To do this, all delay values are tested to see if they
	 * work or not.  By doing this, you get groups of fails with groups of
	 * passing values.  The idea is to find the start and end of a passing
	 * window and take the center of it to use as the read clock delay.
	 *
	 * A failure has to be seen first so that when we hit a pass, we know
	 * that it is truely the start of the window.  If we get passing values
	 * to start off with, we don't know if we are at the start of the window.
	 *
	 * The code assumes that a failure will always be found.
	 * If a failure is not found, there is no easy way to get the middle
	 * of the passing window.  I guess we can pretty much pick any value
	 * but some values will be better than others.  Since the lowest speed
	 * we can clock the DDR interface at is 200 MHz (2x 100 MHz PLB speed),
	 * from experimentation it is safe to say you will always have a failure.
	 *-----------------------------------------------------------------*/
	mfsdram(SDRAM_MCOPT1, ecc_temp);
	ecc_temp &= SDRAM_MCOPT1_MCHK_MASK;
	mfsdram(SDRAM_MCOPT1, val);
	mtsdram(SDRAM_MCOPT1, (val & ~SDRAM_MCOPT1_MCHK_MASK) |
		SDRAM_MCOPT1_MCHK_NON);

	max_start = 0;
	min_end = 0;
	begin_rqfd[0] = 0;
	begin_rffd[0] = 0;
	begin_rqfd[1] = 0;
	begin_rffd[1] = 0;
	end_rqfd[0] = 0;
	end_rffd[0] = 0;
	end_rqfd[1] = 0;
	end_rffd[1] = 0;
	window_found = FALSE;

	max_pass_length = 0;
	max_start = 0;
	max_end = 0;
	current_pass_length = 0;
	current_fail_length = 0;
	current_start = 0;
	window_found = FALSE;
	fail_found = FALSE;
	pass_found = FALSE;

	/* first fix RQDC[RQFD] to an average of 80 degre phase shift to find RFDC[RFFD] */
	/* rqdc_reg = mfsdram(SDRAM_RQDC) & ~(SDRAM_RQDC_RQFD_MASK); */

	/*
	 * get the delay line calibration register value
	 */
	mfsdram(SDRAM_DLCR, dlycal);
	dly_val = SDRAM_DLYCAL_DLCV_DECODE(dlycal) << 2;

	for (rffd = 0; rffd <= SDRAM_RFDC_RFFD_MAX; rffd++) {
		mfsdram(SDRAM_RFDC, rfdc_reg);
		rfdc_reg &= ~(SDRAM_RFDC_RFFD_MASK);

		/*------------------------------------------------------------------
		 * Set the timing reg for the test.
		 *-----------------------------------------------------------------*/
		mtsdram(SDRAM_RFDC, rfdc_reg | SDRAM_RFDC_RFFD_ENCODE(rffd));

		/* do the small memory test */
		bxcr_num = short_mem_test();

		/*------------------------------------------------------------------
		 * See if the rffd value passed.
		 *-----------------------------------------------------------------*/
		if (bxcr_num == MAXBXCF) {
			if (fail_found == TRUE) {
				pass_found = TRUE;
				if (current_pass_length == 0)
					current_start = rffd;

				current_fail_length = 0;
				current_pass_length++;

				if (current_pass_length > max_pass_length) {
					max_pass_length = current_pass_length;
					max_start = current_start;
					max_end = rffd;
				}
			}
		} else {
			current_pass_length = 0;
			current_fail_length++;

			if (current_fail_length >= (dly_val >> 2)) {
				if (fail_found == FALSE) {
					fail_found = TRUE;
				} else if (pass_found == TRUE) {
					window_found = TRUE;
					break;
				}
			}
		}
	}		/* for rffd */


	/*------------------------------------------------------------------
	 * Set the average RFFD value
	 *-----------------------------------------------------------------*/
	rffd_average = ((max_start + max_end) >> 1);

	if (rffd_average < 0)
		rffd_average = 0;

	if (rffd_average > SDRAM_RFDC_RFFD_MAX)
		rffd_average = SDRAM_RFDC_RFFD_MAX;
	/* now fix RFDC[RFFD] found and find RQDC[RQFD] */
	mtsdram(SDRAM_RFDC, rfdc_reg | SDRAM_RFDC_RFFD_ENCODE(rffd_average));

	max_pass_length = 0;
	max_start = 0;
	max_end = 0;
	current_pass_length = 0;
	current_fail_length = 0;
	current_start = 0;
	window_found = FALSE;
	fail_found = FALSE;
	pass_found = FALSE;

	for (rqfd = 0; rqfd <= SDRAM_RQDC_RQFD_MAX; rqfd++) {
		mfsdram(SDRAM_RQDC, rqdc_reg);
		rqdc_reg &= ~(SDRAM_RQDC_RQFD_MASK);

		/*------------------------------------------------------------------
		 * Set the timing reg for the test.
		 *-----------------------------------------------------------------*/
		mtsdram(SDRAM_RQDC, rqdc_reg | SDRAM_RQDC_RQFD_ENCODE(rqfd));

		/* do the small memory test */
		bxcr_num = short_mem_test();

		/*------------------------------------------------------------------
		 * See if the rffd value passed.
		 *-----------------------------------------------------------------*/
		if (bxcr_num == MAXBXCF) {
			if (fail_found == TRUE) {
				pass_found = TRUE;
				if (current_pass_length == 0)
					current_start = rqfd;

				current_fail_length = 0;
				current_pass_length++;

				if (current_pass_length > max_pass_length) {
					max_pass_length = current_pass_length;
					max_start = current_start;
					max_end = rqfd;
				}
			}
		} else {
			current_pass_length = 0;
			current_fail_length++;

			if (fail_found == FALSE) {
				fail_found = TRUE;
			} else if (pass_found == TRUE) {
				window_found = TRUE;
				break;
			}
		}
	}

	/*------------------------------------------------------------------
	 * Make sure we found the valid read passing window.  Halt if not
	 *-----------------------------------------------------------------*/
	if (window_found == FALSE) {
		printf("ERROR: Cannot determine a common read delay for the "
		       "DIMM(s) installed.\n");
		debug("%s[%d] ERROR : \n", __FUNCTION__,__LINE__);
		hang();
	}

	rqfd_average = ((max_start + max_end) >> 1);

	if (rqfd_average < 0)
		rqfd_average = 0;

	if (rqfd_average > SDRAM_RQDC_RQFD_MAX)
		rqfd_average = SDRAM_RQDC_RQFD_MAX;

	/*------------------------------------------------------------------
	 * Restore the ECC variable to what it originally was
	 *-----------------------------------------------------------------*/
	mfsdram(SDRAM_MCOPT1, val);
	mtsdram(SDRAM_MCOPT1, (val & ~SDRAM_MCOPT1_MCHK_MASK) | ecc_temp);

	mtsdram(SDRAM_RQDC,
		(rqdc_reg & ~SDRAM_RQDC_RQFD_MASK) |
		SDRAM_RQDC_RQFD_ENCODE(rqfd_average));

	mfsdram(SDRAM_DLCR, val);
	debug("%s[%d] DLCR: 0x%08X\n", __FUNCTION__, __LINE__, val);
	mfsdram(SDRAM_RQDC, val);
	debug("%s[%d] RQDC: 0x%08X\n", __FUNCTION__, __LINE__, val);
	mfsdram(SDRAM_RFDC, val);
	debug("%s[%d] RFDC: 0x%08X\n", __FUNCTION__, __LINE__, val);
}
#else /* calibration test with hardvalues */
/*-----------------------------------------------------------------------------+
 * DQS_calibration_process.
 *-----------------------------------------------------------------------------*/
static void test(void)
{
	unsigned long dimm_num;
	unsigned long ecc_temp;
	unsigned long i, j;
	unsigned long *membase;
	unsigned long bxcf[MAXRANKS];
	unsigned long val;
	char window_found;
	char begin_found[MAXDIMMS];
	char end_found[MAXDIMMS];
	char search_end[MAXDIMMS];
	unsigned long test[NUMMEMTESTS][NUMMEMWORDS] = {
		{0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
		 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF},
		{0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000,
		 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000},
		{0xAAAAAAAA, 0xAAAAAAAA, 0x55555555, 0x55555555,
		 0xAAAAAAAA, 0xAAAAAAAA, 0x55555555, 0x55555555},
		{0x55555555, 0x55555555, 0xAAAAAAAA, 0xAAAAAAAA,
		 0x55555555, 0x55555555, 0xAAAAAAAA, 0xAAAAAAAA},
		{0xA5A5A5A5, 0xA5A5A5A5, 0x5A5A5A5A, 0x5A5A5A5A,
		 0xA5A5A5A5, 0xA5A5A5A5, 0x5A5A5A5A, 0x5A5A5A5A},
		{0x5A5A5A5A, 0x5A5A5A5A, 0xA5A5A5A5, 0xA5A5A5A5,
		 0x5A5A5A5A, 0x5A5A5A5A, 0xA5A5A5A5, 0xA5A5A5A5},
		{0xAA55AA55, 0xAA55AA55, 0x55AA55AA, 0x55AA55AA,
		 0xAA55AA55, 0xAA55AA55, 0x55AA55AA, 0x55AA55AA},
		{0x55AA55AA, 0x55AA55AA, 0xAA55AA55, 0xAA55AA55,
		 0x55AA55AA, 0x55AA55AA, 0xAA55AA55, 0xAA55AA55} };

	/*------------------------------------------------------------------
	 * Test to determine the best read clock delay tuning bits.
	 *
	 * Before the DDR controller can be used, the read clock delay needs to be
	 * set.  This is SDRAM_RQDC[RQFD] and SDRAM_RFDC[RFFD].
	 * This value cannot be hardcoded into the program because it changes
	 * depending on the board's setup and environment.
	 * To do this, all delay values are tested to see if they
	 * work or not.  By doing this, you get groups of fails with groups of
	 * passing values.  The idea is to find the start and end of a passing
	 * window and take the center of it to use as the read clock delay.
	 *
	 * A failure has to be seen first so that when we hit a pass, we know
	 * that it is truely the start of the window.  If we get passing values
	 * to start off with, we don't know if we are at the start of the window.
	 *
	 * The code assumes that a failure will always be found.
	 * If a failure is not found, there is no easy way to get the middle
	 * of the passing window.  I guess we can pretty much pick any value
	 * but some values will be better than others.  Since the lowest speed
	 * we can clock the DDR interface at is 200 MHz (2x 100 MHz PLB speed),
	 * from experimentation it is safe to say you will always have a failure.
	 *-----------------------------------------------------------------*/
	mfsdram(SDRAM_MCOPT1, ecc_temp);
	ecc_temp &= SDRAM_MCOPT1_MCHK_MASK;
	mfsdram(SDRAM_MCOPT1, val);
	mtsdram(SDRAM_MCOPT1, (val & ~SDRAM_MCOPT1_MCHK_MASK) |
		SDRAM_MCOPT1_MCHK_NON);

	window_found = FALSE;
	begin_found[0] = FALSE;
	end_found[0] = FALSE;
	search_end[0] = FALSE;
	begin_found[1] = FALSE;
	end_found[1] = FALSE;
	search_end[1] = FALSE;

	for (dimm_num = 0; dimm_num < MAXDIMMS; dimm_num++) {
		mfsdram(SDRAM_MB0CF + (bxcr_num << 2), bxcf[bxcr_num]);

		/* Banks enabled */
		if ((bxcf[dimm_num] & SDRAM_BXCF_M_BE_MASK) == SDRAM_BXCF_M_BE_ENABLE) {

			/* Bank is enabled */
			membase =
				(unsigned long*)(SDRAM_RXBAS_SDBA_DECODE(mfdcr_any(SDRAM_R0BAS+dimm_num)));

			/*------------------------------------------------------------------
			 * Run the short memory test.
			 *-----------------------------------------------------------------*/
			for (i = 0; i < NUMMEMTESTS; i++) {
				for (j = 0; j < NUMMEMWORDS; j++) {
					membase[j] = test[i][j];
					ppcDcbf((u32)&(membase[j]));
				}
				sync();
				for (j = 0; j < NUMMEMWORDS; j++) {
					if (membase[j] != test[i][j]) {
						ppcDcbf((u32)&(membase[j]));
						break;
					}
					ppcDcbf((u32)&(membase[j]));
				}
				sync();
				if (j < NUMMEMWORDS)
					break;
			}

			/*------------------------------------------------------------------
			 * See if the rffd value passed.
			 *-----------------------------------------------------------------*/
			if (i < NUMMEMTESTS) {
				if ((end_found[dimm_num] == FALSE) &&
				    (search_end[dimm_num] == TRUE)) {
					end_found[dimm_num] = TRUE;
				}
				if ((end_found[0] == TRUE) &&
				    (end_found[1] == TRUE))
					break;
			} else {
				if (begin_found[dimm_num] == FALSE) {
					begin_found[dimm_num] = TRUE;
					search_end[dimm_num] = TRUE;
				}
			}
		} else {
			begin_found[dimm_num] = TRUE;
			end_found[dimm_num] = TRUE;
		}
	}

	if ((begin_found[0] == TRUE) && (begin_found[1] == TRUE))
		window_found = TRUE;

	/*------------------------------------------------------------------
	 * Make sure we found the valid read passing window.  Halt if not
	 *-----------------------------------------------------------------*/
	if (window_found == FALSE) {
		printf("ERROR: Cannot determine a common read delay for the "
		       "DIMM(s) installed.\n");
		hang();
	}

	/*------------------------------------------------------------------
	 * Restore the ECC variable to what it originally was
	 *-----------------------------------------------------------------*/
	mtsdram(SDRAM_MCOPT1,
		(ppcMfdcr_sdram(SDRAM_MCOPT1) & ~SDRAM_MCOPT1_MCHK_MASK)
		| ecc_temp);
}
#endif

#if defined(DEBUG)
static void ppc440sp_sdram_register_dump(void)
{
	unsigned int sdram_reg;
	unsigned int sdram_data;
	unsigned int dcr_data;

	printf("\n  Register Dump:\n");
	sdram_reg = SDRAM_MCSTAT;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_MCSTAT    = 0x%08X", sdram_data);
	sdram_reg = SDRAM_MCOPT1;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_MCOPT1    = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_MCOPT2;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_MCOPT2    = 0x%08X", sdram_data);
	sdram_reg = SDRAM_MODT0;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_MODT0     = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_MODT1;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_MODT1     = 0x%08X", sdram_data);
	sdram_reg = SDRAM_MODT2;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_MODT2     = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_MODT3;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_MODT3     = 0x%08X", sdram_data);
	sdram_reg = SDRAM_CODT;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_CODT      = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_VVPR;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_VVPR      = 0x%08X", sdram_data);
	sdram_reg = SDRAM_OPARS;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_OPARS     = 0x%08X\n", sdram_data);
	/*
	 * OPAR2 is only used as a trigger register.
	 * No data is contained in this register, and reading or writing
	 * to is can cause bad things to happen (hangs).  Just skip it
	 * and report NA
	 * sdram_reg = SDRAM_OPAR2;
	 * mfsdram(sdram_reg, sdram_data);
	 * printf("        SDRAM_OPAR2     = 0x%08X\n", sdram_data);
	 */
	printf("        SDRAM_OPART     = N/A       ");
	sdram_reg = SDRAM_RTR;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_RTR       = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_MB0CF;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_MB0CF     = 0x%08X", sdram_data);
	sdram_reg = SDRAM_MB1CF;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_MB1CF     = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_MB2CF;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_MB2CF     = 0x%08X", sdram_data);
	sdram_reg = SDRAM_MB3CF;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_MB3CF     = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_INITPLR0;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR0  = 0x%08X", sdram_data);
	sdram_reg = SDRAM_INITPLR1;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR1  = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_INITPLR2;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR2  = 0x%08X", sdram_data);
	sdram_reg = SDRAM_INITPLR3;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR3  = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_INITPLR4;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR4  = 0x%08X", sdram_data);
	sdram_reg = SDRAM_INITPLR5;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR5  = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_INITPLR6;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR6  = 0x%08X", sdram_data);
	sdram_reg = SDRAM_INITPLR7;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR7  = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_INITPLR8;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR8  = 0x%08X", sdram_data);
	sdram_reg = SDRAM_INITPLR9;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR9  = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_INITPLR10;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR10 = 0x%08X", sdram_data);
	sdram_reg = SDRAM_INITPLR11;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR11 = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_INITPLR12;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR12 = 0x%08X", sdram_data);
	sdram_reg = SDRAM_INITPLR13;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR13 = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_INITPLR14;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR14 = 0x%08X", sdram_data);
	sdram_reg = SDRAM_INITPLR15;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_INITPLR15 = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_RQDC;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_RQDC      = 0x%08X", sdram_data);
	sdram_reg = SDRAM_RFDC;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_RFDC      = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_RDCC;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_RDCC      = 0x%08X", sdram_data);
	sdram_reg = SDRAM_DLCR;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_DLCR      = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_CLKTR;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_CLKTR     = 0x%08X", sdram_data);
	sdram_reg = SDRAM_WRDTR;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_WRDTR     = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_SDTR1;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_SDTR1     = 0x%08X", sdram_data);
	sdram_reg = SDRAM_SDTR2;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_SDTR2     = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_SDTR3;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_SDTR3     = 0x%08X", sdram_data);
	sdram_reg = SDRAM_MMODE;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_MMODE     = 0x%08X\n", sdram_data);
	sdram_reg = SDRAM_MEMODE;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_MEMODE    = 0x%08X", sdram_data);
	sdram_reg = SDRAM_ECCCR;
	mfsdram(sdram_reg, sdram_data);
	printf("        SDRAM_ECCCR     = 0x%08X\n\n", sdram_data);

	dcr_data = mfdcr(SDRAM_R0BAS);
	printf("        MQ0_B0BAS       = 0x%08X", dcr_data);
	dcr_data = mfdcr(SDRAM_R1BAS);
	printf("        MQ1_B0BAS       = 0x%08X\n", dcr_data);
	dcr_data = mfdcr(SDRAM_R2BAS);
	printf("        MQ2_B0BAS       = 0x%08X", dcr_data);
	dcr_data = mfdcr(SDRAM_R3BAS);
	printf("        MQ3_B0BAS       = 0x%08X\n", dcr_data);
}
#endif
#endif /* CONFIG_SPD_EEPROM */

/*
 * (C) Copyright 2003
 * MuLogic B.V.
 *
 * (C) Copyright 2002
 * Simple Network Magic Corporation, dnevil@snmc.com
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
#include <asm/u-boot.h>
#include <commproc.h>
#include "mpc8xx.h"

/* ------------------------------------------------------------------------- */

static long  int dram_size (long int, long int *, long int);

/* ------------------------------------------------------------------------- */

const uint sdram_table[] =
{
	/*
	 * Single Read. (Offset 0 in UPMA RAM)
	 */
	0x0f07cc04,  0x00adcc04,  0x00a74c00,  0x00bfcc04,
	0x1fffcc05,  0xffffcc05,  0xffffcc05,  0xffffcc05,
	/*
	 * Burst Read. (Offset 8 in UPMA RAM)
	 */
	0x0ff7fc04,  0x0ffffc04,  0x00bdfc04,  0x00fffc00,
	0x00fffc00,  0x00fffc00,  0x0ff77c00,  0x1ffffc05,
	0x1ffffc05,  0x1ffffc05,  0x1ffffc05,  0x1ffffc05,
	0x1ffffc05,  0x1ffffc05,  0x1ffffc05,  0x1ffffc05,
	/*
	 * Single Write. (Offset 18 in UPMA RAM)
	 */
	0x0f07cc04,  0x0fafcc00,  0x01ad0c04,  0x1ff74c07,
	0xffffcc05,  0xffffcc05,  0xffffcc05,  0xffffcc05,
	/*
	 * Burst Write. (Offset 20 in UPMA RAM)
	 */
	0x0ff7fc04,  0x0ffffc00,  0x00bd7c00,  0x00fffc00,
	0x00fffc00,  0x00fffc00,  0x0ffffc04,  0x0ff77c04,
	0x1ffffc05,  0x1ffffc05,  0x1ffffc05,  0x1ffffc05,
	0x1ffffc05,  0x1ffffc05,  0x1ffffc05,  0x1ffffc05,
	/*
	 * Refresh  (Offset 30 in UPMA RAM)
	 */
	0xffffcc04,  0x1ff5cc84,  0xffffcc04,  0xffffcc04,
	0xffffcc84,  0xffffcc05,  0xffffcc04,  0xffffcc04,
	0xffffcc04,  0xffffcc04,  0xffffcc04,  0xffffcc04,
	/*
	 * Exception. (Offset 3c in UPMA RAM)
	 */
	0x1ff74c04,  0xffffcc07,  0xffffaa34,  0x1fb54a37
};

/* ------------------------------------------------------------------------- */


/*
 * Check Board Identity:
 *
 * Test ID string (QS850, QS823, ...)
 *
 * Always return 1
 */

int checkboard (void)
{
	unsigned char *s, *e;
	unsigned char buf[64];
	int i;

	i = getenv_r("serial#", buf, sizeof(buf));
	s = (i>0) ? buf : NULL;

#ifdef CONFIG_QS850
	if (!s || strncmp(s, "QS850", 5)) {
		puts ("### No HW ID - assuming QS850");
#endif
#ifdef CONFIG_QS823
	if (!s || strncmp(s, "QS823", 5)) {
		puts ("### No HW ID - assuming QS823");
#endif
	} else {
		for (e=s; *e; ++e) {
		if (*e == ' ')
		break;
	}

	for ( ; s<e; ++s) {
		putc (*s);
		}
	}
	putc ('\n');

	return (0);
}

/* ------------------------------------------------------------------------- */
/* SDRAM Mode Register Definitions */

/* Set SDRAM Burst Length to 4 (010) */
/* See Motorola MPC850 User Manual, Page 13-14 */
#define SDRAM_BURST_LENGTH      (2)

/* Set Wrap Type to Sequential (0) */
/* See Motorola MPC850 User Manual, Page 13-14 */
#define SDRAM_WRAP_TYPE         (0 << 3)

/* Set /CAS Latentcy to 2 clocks */
#define SDRAM_CAS_LATENTCY      (2 << 4)

/* The Mode Register value must be shifted left by 2, since it is */
/* placed on the address bus, and the 2 LSBs are ignored for 32-bit accesses */
#define SDRAM_MODE_REG  ((SDRAM_BURST_LENGTH|SDRAM_WRAP_TYPE|SDRAM_CAS_LATENTCY) << 2)

#define UPMA_RUN(loops,index)   (0x80002000 + (loops<<8) + index)

/* Please note a value of zero = 16 loops */
#define REFRESH_INIT_LOOPS (0)


long int initdram (int board_type)
{
	volatile immap_t     *immap  = (immap_t *)CFG_IMMR;
	volatile memctl8xx_t *memctl = &immap->im_memctl;
	long int size;

	upmconfig(UPMA, (uint *)sdram_table, sizeof(sdram_table)/sizeof(uint));

	/*
	* Prescaler for refresh
	*/
	memctl->memc_mptpr = CFG_MPTPR;

	/*
	* Map controller bank 1 to the SDRAM address
	*/
	memctl->memc_or1 = CFG_OR1;
	memctl->memc_br1 = CFG_BR1;
	udelay(1000);

	/* perform SDRAM initialization sequence */
	memctl->memc_mamr = CFG_16M_MAMR;
	udelay(100);

	/* Program the SDRAM's Mode Register */
	memctl->memc_mar  = SDRAM_MODE_REG;

	/* Run the Prechard Pattern at 0x3C */
	memctl->memc_mcr  = UPMA_RUN(1,0x3c);
	udelay(1);

	/* Run the Refresh program residing at MAD index 0x30 */
	/* This contains the CBR Refresh command with a loop */
	/* The SDRAM must be refreshed at least 2 times */
	/* Please note a value of zero = 16 loops */
	memctl->memc_mcr  = UPMA_RUN(REFRESH_INIT_LOOPS,0x30);
	udelay(1);

	/* Run the Exception program residing at MAD index 0x3E */
	/* This contains the Write Mode Register command */
	/* The Write Mode Register command uses the value written to MAR */
	memctl->memc_mcr  = UPMA_RUN(1,0x3e);

	udelay (1000);

	/*
	* Check for 32M SDRAM Memory Size
	*/
	size = dram_size(CFG_32M_MAMR|MAMR_PTAE,
	(ulong *)SDRAM_BASE, SDRAM_32M_MAX_SIZE);
	udelay (1000);

	/*
	* Check for 16M SDRAM Memory Size
	*/
	if (size != SDRAM_32M_MAX_SIZE) {
	size = dram_size(CFG_16M_MAMR|MAMR_PTAE,
	(ulong *)SDRAM_BASE, SDRAM_16M_MAX_SIZE);
	udelay (1000);
	}

	udelay(10000);
	return (size);
}

/* ------------------------------------------------------------------------- */

/*
 * Check memory range for valid RAM. A simple memory test determines
 * the actually available RAM size between addresses `base' and
 * `base + maxsize'. Some (not all) hardware errors are detected:
 * - short between address lines
 * - short between data lines
 */

static long int dram_size (long int mamr_value, long int *base, long int maxsize)
{
	volatile immap_t *immap = (immap_t *)CFG_IMMR;
	volatile memctl8xx_t *memctl = &immap->im_memctl;
	volatile long int *addr;
	long int cnt, val;

	memctl->memc_mamr = mamr_value;

	for (cnt = maxsize/sizeof(long); cnt > 0; cnt >>= 1) {
		addr = base + cnt; /* pointer arith! */
		*addr = ~cnt;
	}

	/* write 0 to base address */
	addr = base;
	*addr = 0;

	/* check at base address */
	if ((val = *addr) != 0) {
		return (0);
	}

	for (cnt = 1; ; cnt <<= 1) {
		addr = base + cnt; /* pointer arith! */
		val = *addr;

		if (val != (~cnt)) {
			return (cnt * sizeof(long));
		}
	}
	/* NOTREACHED */
}

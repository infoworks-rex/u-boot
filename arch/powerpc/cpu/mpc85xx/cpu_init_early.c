/*
 * Copyright 2009 Freescale Semiconductor, Inc
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
#include <asm/processor.h>
#include <asm/mmu.h>
#include <asm/fsl_law.h>

DECLARE_GLOBAL_DATA_PTR;

/* We run cpu_init_early_f in AS = 1 */
void cpu_init_early_f(void)
{
	u32 mas0, mas1, mas2, mas3, mas7;
	int i;

	/* Pointer is writable since we allocated a register for it */
	gd = (gd_t *) (CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_GBL_DATA_OFFSET);

	/*
	 * Clear initial global data
	 *   we don't use memset so we can share this code with NAND_SPL
	 */
	for (i = 0; i < sizeof(gd_t); i++)
		((char *)gd)[i] = 0;

	mas0 = MAS0_TLBSEL(0) | MAS0_ESEL(0);
	mas1 = MAS1_VALID | MAS1_TID(0) | MAS1_TS | MAS1_TSIZE(BOOKE_PAGESZ_4K);
	mas2 = FSL_BOOKE_MAS2(CONFIG_SYS_CCSRBAR, MAS2_I|MAS2_G);
	mas3 = FSL_BOOKE_MAS3(CONFIG_SYS_CCSRBAR_PHYS, 0, MAS3_SW|MAS3_SR);
	mas7 = FSL_BOOKE_MAS7(CONFIG_SYS_CCSRBAR_PHYS);

	write_tlb(mas0, mas1, mas2, mas3, mas7);

	init_laws();
	invalidate_tlb(0);
	init_tlbs();
}

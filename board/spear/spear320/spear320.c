/*
 * (C) Copyright 2009
 * Ryan Chen, ST Micoelectronics, ryan.chen@st.com.
 * Vipin Kumar, ST Micoelectronics, vipin.kumar@st.com.
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
#include <nand.h>
#include <asm/io.h>
#include <linux/mtd/fsmc_nand.h>
#include <asm/arch/hardware.h>
#include <asm/arch/spr_defs.h>
#include <asm/arch/spr_misc.h>

static struct nand_chip nand_chip[CONFIG_SYS_MAX_NAND_DEVICE];

int board_init(void)
{
	return spear_board_init(MACH_TYPE_SPEAR320);
}

/*
 * board_nand_init - Board specific NAND initialization
 * @nand:	mtd private chip structure
 *
 * Called by nand_init_chip to initialize the board specific functions
 */

void board_nand_init()
{
	struct misc_regs *const misc_regs_p =
	    (struct misc_regs *)CONFIG_SPEAR_MISCBASE;
	struct nand_chip *nand = &nand_chip[0];

#if defined(CONFIG_NAND_FSMC)
	if (((readl(&misc_regs_p->auto_cfg_reg) & MISC_SOCCFGMSK) ==
	     MISC_SOCCFG30) ||
	    ((readl(&misc_regs_p->auto_cfg_reg) & MISC_SOCCFGMSK) ==
	     MISC_SOCCFG31)) {

		fsmc_nand_init(nand);
	}
#endif

	return;
}

/*
 * Copyright 2008 Extreme Engineering Solutions, Inc.
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

/*
 * Return SYSCLK input frequency - 50 MHz or 66 MHz depending on POR config
 */
unsigned long get_board_sys_clk(ulong dummy)
{
	volatile ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
	u32 gpporcr = gur->gpporcr;

	if (gpporcr & 0x10000)
		return 66666666;
	else
		return 50000000;
}

/*
 * Return DDR input clock - synchronous with SYSCLK or 66 MHz
 */
unsigned long get_board_ddr_clk(ulong dummy)
{
	volatile ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
	u32 ddr_ratio = ((gur->porpllsr) & 0x00003e00) >> 9;

	if (ddr_ratio == 0x7)
		return get_board_sys_clk(dummy);

	return 66666666;
}

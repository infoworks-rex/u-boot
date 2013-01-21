/*
 * Olimex MX23 Olinuxino Boot setup
 *
 * Copyright (C) 2013 Marek Vasut <marex@denx.de>
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
#include <config.h>
#include <asm/io.h>
#include <asm/arch/iomux-mx23.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/sys_proto.h>

#define	MUX_CONFIG_EMI	(MXS_PAD_3V3 | MXS_PAD_16MA | MXS_PAD_PULLUP)

const iomux_cfg_t iomux_setup[] = {
	/* DUART */
	MX23_PAD_PWM0__DUART_RX,
	MX23_PAD_PWM1__DUART_TX,

	/* EMI */
	MX23_PAD_EMI_D00__EMI_D00 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_D01__EMI_D01 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_D02__EMI_D02 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_D03__EMI_D03 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_D04__EMI_D04 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_D05__EMI_D05 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_D06__EMI_D06 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_D07__EMI_D07 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_D08__EMI_D08 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_D09__EMI_D09 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_D10__EMI_D10 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_D11__EMI_D11 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_D12__EMI_D12 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_D13__EMI_D13 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_D14__EMI_D14 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_D15__EMI_D15 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_DQM0__EMI_DQM0 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_DQM1__EMI_DQM1 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_DQS0__EMI_DQS0 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_DQS1__EMI_DQS1 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_CLK__EMI_CLK | MUX_CONFIG_EMI,
	MX23_PAD_EMI_CLKN__EMI_CLKN | MUX_CONFIG_EMI,

	MX23_PAD_EMI_A00__EMI_A00 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_A01__EMI_A01 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_A02__EMI_A02 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_A03__EMI_A03 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_A04__EMI_A04 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_A05__EMI_A05 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_A06__EMI_A06 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_A07__EMI_A07 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_A08__EMI_A08 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_A09__EMI_A09 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_A10__EMI_A10 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_A11__EMI_A11 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_A12__EMI_A12 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_BA0__EMI_BA0 | MUX_CONFIG_EMI,
	MX23_PAD_EMI_BA1__EMI_BA1 | MUX_CONFIG_EMI,

	MX23_PAD_EMI_CASN__EMI_CASN | MUX_CONFIG_EMI,
	MX23_PAD_EMI_CE0N__EMI_CE0N | MUX_CONFIG_EMI,
	MX23_PAD_EMI_CE1N__EMI_CE1N | MUX_CONFIG_EMI,
	MX23_PAD_EMI_CKE__EMI_CKE | MUX_CONFIG_EMI,
	MX23_PAD_EMI_RASN__EMI_RASN | MUX_CONFIG_EMI,
	MX23_PAD_EMI_WEN__EMI_WEN | MUX_CONFIG_EMI,
};

void board_init_ll(void)
{
	mxs_common_spl_init(iomux_setup, ARRAY_SIZE(iomux_setup));
}

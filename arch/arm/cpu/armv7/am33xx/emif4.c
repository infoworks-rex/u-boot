/*
 * emif4.c
 *
 * AM33XX emif4 configuration file
 *
 * Copyright (C) 2011, Texas Instruments, Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/arch/ddr_defs.h>
#include <asm/arch/hardware.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>
#include <asm/io.h>
#include <asm/emif.h>

DECLARE_GLOBAL_DATA_PTR;

struct ddr_regs *ddrregs = (struct ddr_regs *)DDR_PHY_BASE_ADDR;
struct vtp_reg *vtpreg = (struct vtp_reg *)VTP0_CTRL_ADDR;
struct ddr_ctrl *ddrctrl = (struct ddr_ctrl *)DDR_CTRL_ADDR;

int dram_init(void)
{
	/* dram_init must store complete ramsize in gd->ram_size */
	gd->ram_size = get_ram_size(
			(void *)CONFIG_SYS_SDRAM_BASE,
			CONFIG_MAX_RAM_BANK_SIZE);
	return 0;
}

void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size = gd->ram_size;
}


#ifdef CONFIG_SPL_BUILD
static const struct ddr_data ddr2_data = {
	.datardsratio0 = ((DDR2_RD_DQS<<30)|(DDR2_RD_DQS<<20)
				|(DDR2_RD_DQS<<10)|(DDR2_RD_DQS<<0)),
	.datawdsratio0 = ((DDR2_WR_DQS<<30)|(DDR2_WR_DQS<<20)
				|(DDR2_WR_DQS<<10)|(DDR2_WR_DQS<<0)),
	.datawiratio0 = ((DDR2_PHY_WRLVL<<30)|(DDR2_PHY_WRLVL<<20)
				|(DDR2_PHY_WRLVL<<10)|(DDR2_PHY_WRLVL<<0)),
	.datagiratio0 = ((DDR2_PHY_GATELVL<<30)|(DDR2_PHY_GATELVL<<20)
				|(DDR2_PHY_GATELVL<<10)|(DDR2_PHY_GATELVL<<0)),
	.datafwsratio0 = ((DDR2_PHY_FIFO_WE<<30)|(DDR2_PHY_FIFO_WE<<20)
				|(DDR2_PHY_FIFO_WE<<10)|(DDR2_PHY_FIFO_WE<<0)),
	.datawrsratio0 = ((DDR2_PHY_WR_DATA<<30)|(DDR2_PHY_WR_DATA<<20)
				|(DDR2_PHY_WR_DATA<<10)|(DDR2_PHY_WR_DATA<<0)),
	.datauserank0delay = DDR2_PHY_RANK0_DELAY,
	.datadldiff0 = PHY_DLL_LOCK_DIFF,
};

static const struct cmd_control ddr2_cmd_ctrl_data = {
	.cmd0csratio = DDR2_RATIO,
	.cmd0dldiff = DDR2_DLL_LOCK_DIFF,
	.cmd0iclkout = DDR2_INVERT_CLKOUT,

	.cmd1csratio = DDR2_RATIO,
	.cmd1dldiff = DDR2_DLL_LOCK_DIFF,
	.cmd1iclkout = DDR2_INVERT_CLKOUT,

	.cmd2csratio = DDR2_RATIO,
	.cmd2dldiff = DDR2_DLL_LOCK_DIFF,
	.cmd2iclkout = DDR2_INVERT_CLKOUT,
};

static const struct emif_regs ddr2_emif_reg_data = {
	.sdram_config = DDR2_EMIF_SDCFG,
	.ref_ctrl = DDR2_EMIF_SDREF,
	.sdram_tim1 = DDR2_EMIF_TIM1,
	.sdram_tim2 = DDR2_EMIF_TIM2,
	.sdram_tim3 = DDR2_EMIF_TIM3,
	.emif_ddr_phy_ctlr_1 = DDR2_EMIF_READ_LATENCY,
};

static void config_vtp(void)
{
	writel(readl(&vtpreg->vtp0ctrlreg) | VTP_CTRL_ENABLE,
			&vtpreg->vtp0ctrlreg);
	writel(readl(&vtpreg->vtp0ctrlreg) & (~VTP_CTRL_START_EN),
			&vtpreg->vtp0ctrlreg);
	writel(readl(&vtpreg->vtp0ctrlreg) | VTP_CTRL_START_EN,
			&vtpreg->vtp0ctrlreg);

	/* Poll for READY */
	while ((readl(&vtpreg->vtp0ctrlreg) & VTP_CTRL_READY) !=
			VTP_CTRL_READY)
		;
}

void config_ddr(short ddr_type)
{
	enable_emif_clocks();

	if (ddr_type == EMIF_REG_SDRAM_TYPE_DDR2) {
		ddr_pll_config(266);
		config_vtp();

		config_cmd_ctrl(&ddr2_cmd_ctrl_data);

		config_ddr_data(0, &ddr2_data);
		config_ddr_data(1, &ddr2_data);

		config_io_ctrl(DDR2_IOCTRL_VALUE);

		/* Set CKE to be controlled by EMIF/DDR PHY */
		writel(DDR_CKE_CTRL_NORMAL, &ddrctrl->ddrckectrl);

		/* Program EMIF instance */
		config_ddr_phy(&ddr2_emif_reg_data);
		set_sdram_timings(&ddr2_emif_reg_data);
		config_sdram(&ddr2_emif_reg_data);
	}
}
#endif

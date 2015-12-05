/*
 * Copyright 2015 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch/clock.h>
#include <asm/io.h>
#include <asm/arch/immap_ls102xa.h>
#include <asm/arch/ls102xa_soc.h>

unsigned int get_soc_major_rev(void)
{
	struct ccsr_gur __iomem *gur = (void *)(CONFIG_SYS_FSL_GUTS_ADDR);
	unsigned int svr, major;

	svr = in_be32(&gur->svr);
	major = SVR_MAJ(svr);

	return major;
}

int arch_soc_init(void)
{
	struct ccsr_scfg *scfg = (struct ccsr_scfg *)CONFIG_SYS_FSL_SCFG_ADDR;
	struct ccsr_cci400 *cci = (struct ccsr_cci400 *)CONFIG_SYS_CCI400_ADDR;
	unsigned int major;

#ifdef CONFIG_FSL_QSPI
	out_be32(&scfg->qspi_cfg, SCFG_QSPI_CLKSEL);
#endif

#ifdef CONFIG_FSL_DCU_FB
	out_be32(&scfg->pixclkcr, SCFG_PIXCLKCR_PXCKEN);
#endif

	/* Configure Little endian for SAI, ASRC and SPDIF */
	out_be32(&scfg->endiancr, SCFG_ENDIANCR_LE);

	/*
	 * Enable snoop requests and DVM message requests for
	 * Slave insterface S4 (A7 core cluster)
	 */
	out_le32(&cci->slave[4].snoop_ctrl,
		 CCI400_DVM_MESSAGE_REQ_EN | CCI400_SNOOP_REQ_EN);

	major = get_soc_major_rev();
	if (major == SOC_MAJOR_VER_1_0) {
		/*
		 * Set CCI-400 Slave interface S1, S2 Shareable Override
		 * Register All transactions are treated as non-shareable
		 */
		out_le32(&cci->slave[1].sha_ord, CCI400_SHAORD_NON_SHAREABLE);
		out_le32(&cci->slave[2].sha_ord, CCI400_SHAORD_NON_SHAREABLE);

		/* Workaround for the issue that DDR could not respond to
		 * barrier transaction which is generated by executing DSB/ISB
		 * instruction. Set CCI-400 control override register to
		 * terminate the barrier transaction. After DDR is initialized,
		 * allow barrier transaction to DDR again */
		out_le32(&cci->ctrl_ord, CCI400_CTRLORD_TERM_BARRIER);
	}

	/* Enable all the snoop signal for various masters */
	out_be32(&scfg->snpcnfgcr, SCFG_SNPCNFGCR_SEC_RD_WR |
				SCFG_SNPCNFGCR_DCU_RD_WR |
				SCFG_SNPCNFGCR_SATA_RD_WR |
				SCFG_SNPCNFGCR_USB3_RD_WR |
				SCFG_SNPCNFGCR_DBG_RD_WR |
				SCFG_SNPCNFGCR_EDMA_SNP);

	return 0;
}

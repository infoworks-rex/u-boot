/*
 * Copyright 2014-2015 Freescale Semiconductor
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <fsl_ifc.h>
#include <ahci.h>
#include <scsi.h>
#include <asm/arch/soc.h>
#include <asm/io.h>
#include <asm/global_data.h>
#include <asm/arch-fsl-layerscape/config.h>
#ifdef CONFIG_CHAIN_OF_TRUST
#include <fsl_validate.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_LS2080A) || defined(CONFIG_LS2085A)
/*
 * This erratum requires setting a value to eddrtqcr1 to
 * optimal the DDR performance.
 */
static void erratum_a008336(void)
{
	u32 *eddrtqcr1;

#ifdef CONFIG_SYS_FSL_ERRATUM_A008336
#ifdef CONFIG_SYS_FSL_DCSR_DDR_ADDR
	eddrtqcr1 = (void *)CONFIG_SYS_FSL_DCSR_DDR_ADDR + 0x800;
	out_le32(eddrtqcr1, 0x63b30002);
#endif
#ifdef CONFIG_SYS_FSL_DCSR_DDR2_ADDR
	eddrtqcr1 = (void *)CONFIG_SYS_FSL_DCSR_DDR2_ADDR + 0x800;
	out_le32(eddrtqcr1, 0x63b30002);
#endif
#endif
}

/*
 * This erratum requires a register write before being Memory
 * controller 3 being enabled.
 */
static void erratum_a008514(void)
{
	u32 *eddrtqcr1;

#ifdef CONFIG_SYS_FSL_ERRATUM_A008514
#ifdef CONFIG_SYS_FSL_DCSR_DDR3_ADDR
	eddrtqcr1 = (void *)CONFIG_SYS_FSL_DCSR_DDR3_ADDR + 0x800;
	out_le32(eddrtqcr1, 0x63b20002);
#endif
#endif
}
#ifdef CONFIG_SYS_FSL_ERRATUM_A009635
#define PLATFORM_CYCLE_ENV_VAR	"a009635_interval_val"

static unsigned long get_internval_val_mhz(void)
{
	char *interval = getenv(PLATFORM_CYCLE_ENV_VAR);
	/*
	 *  interval is the number of platform cycles(MHz) between
	 *  wake up events generated by EPU.
	 */
	ulong interval_mhz = get_bus_freq(0) / (1000 * 1000);

	if (interval)
		interval_mhz = simple_strtoul(interval, NULL, 10);

	return interval_mhz;
}

void erratum_a009635(void)
{
	u32 val;
	unsigned long interval_mhz = get_internval_val_mhz();

	if (!interval_mhz)
		return;

	val = in_le32(DCSR_CGACRE5);
	writel(val | 0x00000200, DCSR_CGACRE5);

	val = in_le32(EPU_EPCMPR5);
	writel(interval_mhz, EPU_EPCMPR5);
	val = in_le32(EPU_EPCCR5);
	writel(val | 0x82820000, EPU_EPCCR5);
	val = in_le32(EPU_EPSMCR5);
	writel(val | 0x002f0000, EPU_EPSMCR5);
	val = in_le32(EPU_EPECR5);
	writel(val | 0x20000000, EPU_EPECR5);
	val = in_le32(EPU_EPGCR);
	writel(val | 0x80000000, EPU_EPGCR);
}
#endif	/* CONFIG_SYS_FSL_ERRATUM_A009635 */

static void erratum_a008751(void)
{
#ifdef CONFIG_SYS_FSL_ERRATUM_A008751
	u32 __iomem *scfg = (u32 __iomem *)SCFG_BASE;

	writel(0x27672b2a, scfg + SCFG_USB3PRM1CR / 4);
#endif
}

static void erratum_rcw_src(void)
{
#if defined(CONFIG_SPL)
	u32 __iomem *dcfg_ccsr = (u32 __iomem *)DCFG_BASE;
	u32 __iomem *dcfg_dcsr = (u32 __iomem *)DCFG_DCSR_BASE;
	u32 val;

	val = in_le32(dcfg_ccsr + DCFG_PORSR1 / 4);
	val &= ~DCFG_PORSR1_RCW_SRC;
	val |= DCFG_PORSR1_RCW_SRC_NOR;
	out_le32(dcfg_dcsr + DCFG_DCSR_PORCR1 / 4, val);
#endif
}

#define I2C_DEBUG_REG 0x6
#define I2C_GLITCH_EN 0x8
/*
 * This erratum requires setting glitch_en bit to enable
 * digital glitch filter to improve clock stability.
 */
static void erratum_a009203(void)
{
	u8 __iomem *ptr;
#ifdef CONFIG_SYS_I2C
#ifdef I2C1_BASE_ADDR
	ptr = (u8 __iomem *)(I2C1_BASE_ADDR + I2C_DEBUG_REG);

	writeb(I2C_GLITCH_EN, ptr);
#endif
#ifdef I2C2_BASE_ADDR
	ptr = (u8 __iomem *)(I2C2_BASE_ADDR + I2C_DEBUG_REG);

	writeb(I2C_GLITCH_EN, ptr);
#endif
#ifdef I2C3_BASE_ADDR
	ptr = (u8 __iomem *)(I2C3_BASE_ADDR + I2C_DEBUG_REG);

	writeb(I2C_GLITCH_EN, ptr);
#endif
#ifdef I2C4_BASE_ADDR
	ptr = (u8 __iomem *)(I2C4_BASE_ADDR + I2C_DEBUG_REG);

	writeb(I2C_GLITCH_EN, ptr);
#endif
#endif
}
void bypass_smmu(void)
{
	u32 val;
	val = (in_le32(SMMU_SCR0) | SCR0_CLIENTPD_MASK) & ~(SCR0_USFCFG_MASK);
	out_le32(SMMU_SCR0, val);
	val = (in_le32(SMMU_NSCR0) | SCR0_CLIENTPD_MASK) & ~(SCR0_USFCFG_MASK);
	out_le32(SMMU_NSCR0, val);
}
void fsl_lsch3_early_init_f(void)
{
	erratum_a008751();
	erratum_rcw_src();
	init_early_memctl_regs();	/* tighten IFC timing */
	erratum_a009203();
	erratum_a008514();
	erratum_a008336();
#ifdef CONFIG_CHAIN_OF_TRUST
	/* In case of Secure Boot, the IBR configures the SMMU
	* to allow only Secure transactions.
	* SMMU must be reset in bypass mode.
	* Set the ClientPD bit and Clear the USFCFG Bit
	*/
	if (fsl_check_boot_mode_secure() == 1)
		bypass_smmu();
#endif
}

#ifdef CONFIG_SCSI_AHCI_PLAT
int sata_init(void)
{
	struct ccsr_ahci __iomem *ccsr_ahci;

	ccsr_ahci  = (void *)CONFIG_SYS_SATA2;
	out_le32(&ccsr_ahci->ppcfg, AHCI_PORT_PHY_1_CFG);
	out_le32(&ccsr_ahci->ptc, AHCI_PORT_TRANS_CFG);

	ccsr_ahci  = (void *)CONFIG_SYS_SATA1;
	out_le32(&ccsr_ahci->ppcfg, AHCI_PORT_PHY_1_CFG);
	out_le32(&ccsr_ahci->ptc, AHCI_PORT_TRANS_CFG);

	ahci_init((void __iomem *)CONFIG_SYS_SATA1);
	scsi_scan(0);

	return 0;
}
#endif

#elif defined(CONFIG_LS1043A)
#ifdef CONFIG_SCSI_AHCI_PLAT
int sata_init(void)
{
	struct ccsr_ahci __iomem *ccsr_ahci = (void *)CONFIG_SYS_SATA;

	out_le32(&ccsr_ahci->ppcfg, AHCI_PORT_PHY_1_CFG);
	out_le32(&ccsr_ahci->pp2c, AHCI_PORT_PHY_2_CFG);
	out_le32(&ccsr_ahci->pp3c, AHCI_PORT_PHY_3_CFG);
	out_le32(&ccsr_ahci->ptc, AHCI_PORT_TRANS_CFG);

	ahci_init((void __iomem *)CONFIG_SYS_SATA);
	scsi_scan(0);

	return 0;
}
#endif

static void erratum_a009929(void)
{
#ifdef CONFIG_SYS_FSL_ERRATUM_A009929
	struct ccsr_gur *gur = (void *)CONFIG_SYS_FSL_GUTS_ADDR;
	u32 __iomem *dcsr_cop_ccp = (void *)CONFIG_SYS_DCSR_COP_CCP_ADDR;
	u32 rstrqmr1 = gur_in32(&gur->rstrqmr1);

	rstrqmr1 |= 0x00000400;
	gur_out32(&gur->rstrqmr1, rstrqmr1);
	writel(0x01000000, dcsr_cop_ccp);
#endif
}

/*
 * This erratum requires setting a value to eddrtqcr1 to optimal
 * the DDR performance. The eddrtqcr1 register is in SCFG space
 * of LS1043A and the offset is 0x157_020c.
 */
#if defined(CONFIG_SYS_FSL_ERRATUM_A009660) \
	&& defined(CONFIG_SYS_FSL_ERRATUM_A008514)
#error A009660 and A008514 can not be both enabled.
#endif

static void erratum_a009660(void)
{
#ifdef CONFIG_SYS_FSL_ERRATUM_A009660
	u32 *eddrtqcr1 = (void *)CONFIG_SYS_FSL_SCFG_ADDR + 0x20c;
	out_be32(eddrtqcr1, 0x63b20042);
#endif
}

void fsl_lsch2_early_init_f(void)
{
	struct ccsr_cci400 *cci = (struct ccsr_cci400 *)CONFIG_SYS_CCI400_ADDR;
	struct ccsr_scfg *scfg = (struct ccsr_scfg *)CONFIG_SYS_FSL_SCFG_ADDR;

#ifdef CONFIG_FSL_IFC
	init_early_memctl_regs();	/* tighten IFC timing */
#endif

#if defined(CONFIG_FSL_QSPI) && !defined(CONFIG_QSPI_BOOT)
	out_be32(&scfg->qspi_cfg, SCFG_QSPI_CLKSEL);
#endif
	/* Make SEC reads and writes snoopable */
	setbits_be32(&scfg->snpcnfgcr, SCFG_SNPCNFGCR_SECRDSNP |
		     SCFG_SNPCNFGCR_SECWRSNP);

	/*
	 * Enable snoop requests and DVM message requests for
	 * Slave insterface S4 (A53 core cluster)
	 */
	out_le32(&cci->slave[4].snoop_ctrl,
		 CCI400_DVM_MESSAGE_REQ_EN | CCI400_SNOOP_REQ_EN);

	/* Erratum */
	erratum_a009929();
	erratum_a009660();
}
#endif

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
#ifdef CONFIG_SCSI_AHCI_PLAT
	sata_init();
#endif
#ifdef CONFIG_CHAIN_OF_TRUST
	fsl_setenv_chain_of_trust();
#endif

	return 0;
}
#endif

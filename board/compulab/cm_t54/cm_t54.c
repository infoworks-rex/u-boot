/*
 * Board functions for Compulab CM-T54 board
 *
 * Copyright (C) 2014, Compulab Ltd - http://compulab.co.il/
 *
 * Author: Dmitry Lifshitz <lifshitz@compulab.co.il>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <usb.h>
#include <mmc.h>
#include <palmas.h>

#include <asm/gpio.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/clock.h>
#include <asm/arch/ehci.h>
#include <asm/ehci-omap.h>

#define DIE_ID_REG_BASE		(OMAP54XX_L4_CORE_BASE + 0x2000)
#define DIE_ID_REG_OFFSET	0x200

DECLARE_GLOBAL_DATA_PTR;

#if !defined(CONFIG_SPL_BUILD)
inline void set_muxconf_regs_essential(void){};
#endif

const struct omap_sysinfo sysinfo = {
	"Board: CM-T54\n"
};

/*
 * Routine: board_init
 * Description: hardware init.
 */
int board_init(void)
{
	gd->bd->bi_boot_params = (CONFIG_SYS_SDRAM_BASE + 0x100); /* boot param addr */

	return 0;
}

/*
 * Routine: cm_t54_palmas_regulator_set
 * Description:  select voltage and turn on/off Palmas PMIC regulator.
 */
static int cm_t54_palmas_regulator_set(u8 vreg, u8 vval, u8 creg, u8 cval)
{
	int err;

	/* Setup voltage */
	err = palmas_i2c_write_u8(TWL603X_CHIP_P1, vreg, vval);
	if (err) {
		printf("cm_t54: could not set regulator 0x%02x voltage : %d\n",
		       vreg, err);
		return err;
	}

	/* Turn on/off regulator */
	err = palmas_i2c_write_u8(TWL603X_CHIP_P1, creg, cval);
	if (err) {
		printf("cm_t54: could not turn on/off regulator 0x%02x : %d\n",
		       creg, err);
		return err;
	}

	return 0;
}

#if defined(CONFIG_GENERIC_MMC) && !defined(CONFIG_SPL_BUILD)
#define SB_T54_CD_GPIO 228
#define SB_T54_WP_GPIO 229

int board_mmc_getcd(struct mmc *mmc)
{
	return !gpio_get_value(SB_T54_CD_GPIO);
}

int board_mmc_init(bd_t *bis)
{
	int ret0, ret1;

	ret0 = omap_mmc_init(0, 0, 0, -1, SB_T54_WP_GPIO);
	if (ret0)
		printf("cm_t54: failed to initialize mmc0\n");

	ret1 = omap_mmc_init(1, 0, 0, -1, -1);
	if (ret1)
		printf("cm_t54: failed to initialize mmc1\n");

	if (ret0 && ret1)
		return -1;

	return 0;
}
#endif

#ifdef CONFIG_USB_EHCI
static struct omap_usbhs_board_data usbhs_bdata = {
	.port_mode[0] = OMAP_USBHS_PORT_MODE_UNUSED,
	.port_mode[1] = OMAP_EHCI_PORT_MODE_HSIC,
	.port_mode[2] = OMAP_EHCI_PORT_MODE_HSIC,
};

static void setup_host_clocks(bool enable)
{
	int usbhost_clk = OPTFCLKEN_HSIC60M_P3_CLK |
			  OPTFCLKEN_HSIC480M_P3_CLK |
			  OPTFCLKEN_HSIC60M_P2_CLK |
			  OPTFCLKEN_HSIC480M_P2_CLK |
			  OPTFCLKEN_UTMI_P3_CLK |
			  OPTFCLKEN_UTMI_P2_CLK;

	int usbtll_clk = OPTFCLKEN_USB_CH1_CLK_ENABLE |
			 OPTFCLKEN_USB_CH2_CLK_ENABLE;

	int usbhub_clk = CKOBUFFER_CLK_ENABLE_MASK;

	if (enable) {
		/* Enable port 2 and 3 clocks*/
		setbits_le32((*prcm)->cm_l3init_hsusbhost_clkctrl, usbhost_clk);
		/* Enable port 2 and 3 usb host ports tll clocks*/
		setbits_le32((*prcm)->cm_l3init_hsusbtll_clkctrl, usbtll_clk);
		/* Request FREF_XTAL_CLK clock for HSIC USB Hub */
		setbits_le32((*ctrl)->control_ckobuffer, usbhub_clk);
	} else {
		clrbits_le32((*ctrl)->control_ckobuffer, usbhub_clk);
		clrbits_le32((*prcm)->cm_l3init_hsusbtll_clkctrl, usbtll_clk);
		clrbits_le32((*prcm)->cm_l3init_hsusbhost_clkctrl, usbhost_clk);
	}
}

int ehci_hcd_init(int index, enum usb_init_type init,
		struct ehci_hccr **hccr, struct ehci_hcor **hcor)
{
	int ret;

	/* VCC_3V3_ETH */
	cm_t54_palmas_regulator_set(SMPS9_VOLTAGE, SMPS_VOLT_3V3, SMPS9_CTRL,
				    SMPS_MODE_SLP_AUTO | SMPS_MODE_ACT_AUTO);

	setup_host_clocks(true);

	ret = omap_ehci_hcd_init(index, &usbhs_bdata, hccr, hcor);
	if (ret < 0)
		printf("cm_t54: Failed to initialize ehci : %d\n", ret);

	return ret;
}

int ehci_hcd_stop(void)
{
	int ret = omap_ehci_hcd_stop();

	setup_host_clocks(false);

	cm_t54_palmas_regulator_set(SMPS9_VOLTAGE, SMPS_VOLT_OFF,
				    SMPS9_CTRL, SMPS_MODE_SLP_AUTO);

	return ret;
}

void usb_hub_reset_devices(int port)
{
	/* The LAN9730 needs to be reset after the port power has been set. */
	if (port == 3) {
		gpio_direction_output(CONFIG_OMAP_EHCI_PHY3_RESET_GPIO, 0);
		udelay(10);
		gpio_direction_output(CONFIG_OMAP_EHCI_PHY3_RESET_GPIO, 1);
	}
}
#endif


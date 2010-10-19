/*
 * (C) Copyright 2010
 * Stefano Babic, DENX Software Engineering, sbabic@denx.de.
 *
 * (C) Copyright 2009 Freescale Semiconductor, Inc.
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
#include <asm/io.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/mx5x_pins.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/iomux.h>
#include <mxc_gpio.h>
#include <asm/arch/sys_proto.h>
#include <asm/errno.h>
#include <i2c.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <fsl_pmic.h>
#include <mc13892.h>

DECLARE_GLOBAL_DATA_PTR;

static u32 system_rev;

#ifdef CONFIG_HW_WATCHDOG
#include <watchdog.h>

void hw_watchdog_reset(void)
{
	int val;

	/* toggle watchdog trigger pin */
	val = mxc_gpio_get(66);
	val = val ? 0 : 1;
	mxc_gpio_set(66, val);
}
#endif

static void init_drive_strength(void)
{
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_PKEDDR, PAD_CTL_DDR_INPUT_CMOS);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_PKEADDR, PAD_CTL_PKE_ENABLE);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDRAPKS, PAD_CTL_PUE_KEEPER);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDRAPUS, PAD_CTL_100K_PU);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDR_SR_A1, PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDR_A0, PAD_CTL_DRV_HIGH);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDR_A1, PAD_CTL_DRV_HIGH);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_RAS,
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_CAS,
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_PKEDDR, PAD_CTL_PKE_ENABLE);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDRPKS, PAD_CTL_PUE_KEEPER);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_HYSDDR0, PAD_CTL_HYS_NONE);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_HYSDDR1, PAD_CTL_HYS_NONE);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_HYSDDR2, PAD_CTL_HYS_NONE);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_HYSDDR3, PAD_CTL_HYS_NONE);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDR_SR_B0, PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDR_SR_B1, PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDR_SR_B2, PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDR_SR_B4, PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDRPUS, PAD_CTL_100K_PU);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_INMODE1, PAD_CTL_DDR_INPUT_CMOS);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DRAM_B0, PAD_CTL_DRV_MEDIUM);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DRAM_B1, PAD_CTL_DRV_MEDIUM);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DRAM_B2, PAD_CTL_DRV_MEDIUM);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DRAM_B4, PAD_CTL_DRV_MEDIUM);

	/* Setting pad options */
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDWE,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDCKE0,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDCKE1,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDCLK,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDQS0,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDQS1,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDQS2,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDQS3,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_CS0,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_CS1,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_DQM0,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_DQM1,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_DQM2,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_DQM3,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
}

u32 get_board_rev(void)
{
	system_rev = get_cpu_rev();

	return system_rev;
}

int dram_init(void)
{
#ifdef CONFIG_SYS_ARM_WITHOUT_RELOC
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = get_ram_size((long *)PHYS_SDRAM_1,
		PHYS_SDRAM_1_SIZE);
#if (CONFIG_NR_DRAM_BANKS > 1)
	gd->bd->bi_dram[1].start = PHYS_SDRAM_2;
	gd->bd->bi_dram[1].size = get_ram_size((long *)PHYS_SDRAM_2,
		PHYS_SDRAM_2_SIZE);
#endif
#else
	gd->ram_size = get_ram_size((long *)PHYS_SDRAM_1,
		PHYS_SDRAM_1_SIZE);
#endif

	return 0;
}

static void setup_weim(void)
{
	struct weim  *pweim = (struct weim *)WEIM_BASE_ADDR;

	pweim->csgcr1 = 0x004100b9;
	pweim->csgcr2 = 0x00000001;
	pweim->csrcr1 = 0x0a018000;
	pweim->csrcr2 = 0;
	pweim->cswcr1 = 0x0704a240;
}

static void setup_uart(void)
{
	unsigned int pad = PAD_CTL_HYS_ENABLE | PAD_CTL_PKE_ENABLE |
			 PAD_CTL_PUE_PULL | PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST;
	/* console RX on Pin EIM_D25 */
	mxc_request_iomux(MX51_PIN_EIM_D25, IOMUX_CONFIG_ALT3);
	mxc_iomux_set_pad(MX51_PIN_EIM_D25, pad);
	/* console TX on Pin EIM_D26 */
	mxc_request_iomux(MX51_PIN_EIM_D26, IOMUX_CONFIG_ALT3);
	mxc_iomux_set_pad(MX51_PIN_EIM_D26, pad);
}

#ifdef CONFIG_MXC_SPI
void spi_io_init(void)
{
	/* 000: Select mux mode: ALT0 mux port: MOSI of instance: ecspi1 */
	mxc_request_iomux(MX51_PIN_CSPI1_MOSI, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_CSPI1_MOSI,
		PAD_CTL_HYS_ENABLE | PAD_CTL_DRV_MAX | PAD_CTL_SRE_FAST);

	/* 000: Select mux mode: ALT0 mux port: MISO of instance: ecspi1. */
	mxc_request_iomux(MX51_PIN_CSPI1_MISO, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_CSPI1_MISO,
		PAD_CTL_HYS_ENABLE | PAD_CTL_DRV_MAX | PAD_CTL_SRE_FAST);

	/* 000: Select mux mode: ALT0 mux port: SS0 of instance: ecspi1. */
	mxc_request_iomux(MX51_PIN_CSPI1_SS0, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_CSPI1_SS0,
		PAD_CTL_HYS_ENABLE | PAD_CTL_PKE_ENABLE |
		PAD_CTL_DRV_MAX | PAD_CTL_SRE_FAST);

	/*
	 * SS1 will be used as GPIO because of uninterrupted
	 * long SPI transmissions (GPIO4_25)
	 */
	mxc_request_iomux(MX51_PIN_CSPI1_SS1, IOMUX_CONFIG_ALT3);
	mxc_iomux_set_pad(MX51_PIN_CSPI1_SS1,
		PAD_CTL_HYS_ENABLE | PAD_CTL_PKE_ENABLE |
		PAD_CTL_DRV_MAX | PAD_CTL_SRE_FAST);

	/* 000: Select mux mode: ALT0 mux port: SS2 of instance: ecspi1. */
	mxc_request_iomux(MX51_PIN_DI1_PIN11, IOMUX_CONFIG_ALT7);
	mxc_iomux_set_pad(MX51_PIN_DI1_PIN11,
		PAD_CTL_HYS_ENABLE | PAD_CTL_PKE_ENABLE |
		PAD_CTL_DRV_MAX | PAD_CTL_SRE_FAST);

	/* 000: Select mux mode: ALT0 mux port: SCLK of instance: ecspi1. */
	mxc_request_iomux(MX51_PIN_CSPI1_SCLK, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_CSPI1_SCLK,
		PAD_CTL_HYS_ENABLE | PAD_CTL_DRV_MAX | PAD_CTL_SRE_FAST);
}

static void reset_peripherals(int reset)
{
	if (reset) {

		/* reset_n is on NANDF_D15 */
		mxc_gpio_set(89, 0);
		mxc_gpio_direction(89, MXC_GPIO_DIRECTION_OUT);

#ifdef CONFIG_VISION2_HW_1_0
		/*
		 * set FEC Configuration lines
		 * set levels of FEC config lines
		 */
		mxc_gpio_set(75, 0);
		mxc_gpio_set(74, 1);
		mxc_gpio_set(95, 1);
		mxc_gpio_direction(75, MXC_GPIO_DIRECTION_OUT);
		mxc_gpio_direction(74, MXC_GPIO_DIRECTION_OUT);
		mxc_gpio_direction(95, MXC_GPIO_DIRECTION_OUT);

		/* set direction of FEC config lines */
		mxc_gpio_set(59, 0);
		mxc_gpio_set(60, 0);
		mxc_gpio_set(61, 0);
		mxc_gpio_set(55, 1);
		mxc_gpio_direction(59, MXC_GPIO_DIRECTION_OUT);
		mxc_gpio_direction(60, MXC_GPIO_DIRECTION_OUT);
		mxc_gpio_direction(61, MXC_GPIO_DIRECTION_OUT);
		mxc_gpio_direction(55, MXC_GPIO_DIRECTION_OUT);

		/* FEC_RXD1 - sel GPIO (2-23) for configuration -> 1 */
		mxc_request_iomux(MX51_PIN_EIM_EB3, IOMUX_CONFIG_ALT1);
		/* FEC_RXD2 - sel GPIO (2-27) for configuration -> 0 */
		mxc_request_iomux(MX51_PIN_EIM_CS2, IOMUX_CONFIG_ALT1);
		/* FEC_RXD3 - sel GPIO (2-28) for configuration -> 0 */
		mxc_request_iomux(MX51_PIN_EIM_CS3, IOMUX_CONFIG_ALT1);
		/* FEC_RXER - sel GPIO (2-29) for configuration -> 0 */
		mxc_request_iomux(MX51_PIN_EIM_CS4, IOMUX_CONFIG_ALT1);
		/* FEC_COL  - sel GPIO (3-10) for configuration -> 1 */
		mxc_request_iomux(MX51_PIN_NANDF_RB2, IOMUX_CONFIG_ALT3);
		/* FEC_RCLK - sel GPIO (3-11) for configuration -> 0 */
		mxc_request_iomux(MX51_PIN_NANDF_RB3, IOMUX_CONFIG_ALT3);
		/* FEC_RXD0 - sel GPIO (3-31) for configuration -> 1 */
		mxc_request_iomux(MX51_PIN_NANDF_D9, IOMUX_CONFIG_ALT3);
#endif

		/*
		 * activate reset_n pin
		 * Select mux mode: ALT3 mux port: NAND D15
		 */
		mxc_request_iomux(MX51_PIN_NANDF_D15, IOMUX_CONFIG_ALT3);
		mxc_iomux_set_pad(MX51_PIN_NANDF_D15,
			PAD_CTL_DRV_VOT_HIGH | PAD_CTL_DRV_MAX);
	} else {
		/* set FEC Control lines */
		mxc_gpio_direction(89, MXC_GPIO_DIRECTION_IN);
		udelay(500);

#ifdef CONFIG_VISION2_HW_1_0
		/* FEC RDATA[3] */
		mxc_request_iomux(MX51_PIN_EIM_CS3, IOMUX_CONFIG_ALT3);
		mxc_iomux_set_pad(MX51_PIN_EIM_CS3, 0x180);

		/* FEC RDATA[2] */
		mxc_request_iomux(MX51_PIN_EIM_CS2, IOMUX_CONFIG_ALT3);
		mxc_iomux_set_pad(MX51_PIN_EIM_CS2, 0x180);

		/* FEC RDATA[1] */
		mxc_request_iomux(MX51_PIN_EIM_EB3, IOMUX_CONFIG_ALT3);
		mxc_iomux_set_pad(MX51_PIN_EIM_EB3, 0x180);

		/* FEC RDATA[0] */
		mxc_request_iomux(MX51_PIN_NANDF_D9, IOMUX_CONFIG_ALT2);
		mxc_iomux_set_pad(MX51_PIN_NANDF_D9, 0x2180);

		/* FEC RX_CLK */
		mxc_request_iomux(MX51_PIN_NANDF_RB3, IOMUX_CONFIG_ALT1);
		mxc_iomux_set_pad(MX51_PIN_NANDF_RB3, 0x2180);

		/* FEC RX_ER */
		mxc_request_iomux(MX51_PIN_EIM_CS4, IOMUX_CONFIG_ALT3);
		mxc_iomux_set_pad(MX51_PIN_EIM_CS4, 0x180);

		/* FEC COL */
		mxc_request_iomux(MX51_PIN_NANDF_RB2, IOMUX_CONFIG_ALT1);
		mxc_iomux_set_pad(MX51_PIN_NANDF_RB2, 0x2180);
#endif
	}
}

static void power_init_mx51(void)
{
	unsigned int val;

	/* Write needed to Power Gate 2 register */
	val = pmic_reg_read(REG_POWER_MISC);

	/* enable VCAM with 2.775V to enable read from PMIC */
	val = VCAMCONFIG | VCAMEN;
	pmic_reg_write(REG_MODE_1, val);

	/*
	 * Set switchers in Auto in NORMAL mode & STANDBY mode
	 * Setup the switcher mode for SW1 & SW2
	 */
	val = pmic_reg_read(REG_SW_4);
	val = (val & ~((SWMODE_MASK << SWMODE1_SHIFT) |
		(SWMODE_MASK << SWMODE2_SHIFT)));
	val |= (SWMODE_AUTO_AUTO << SWMODE1_SHIFT) |
		(SWMODE_AUTO_AUTO << SWMODE2_SHIFT);
	pmic_reg_write(REG_SW_4, val);

	/* Setup the switcher mode for SW3 & SW4 */
	val = pmic_reg_read(REG_SW_5);
	val &= ~((SWMODE_MASK << SWMODE4_SHIFT) |
		(SWMODE_MASK << SWMODE3_SHIFT));
	val |= (SWMODE_AUTO_AUTO << SWMODE4_SHIFT) |
		(SWMODE_AUTO_AUTO << SWMODE3_SHIFT);
	pmic_reg_write(REG_SW_5, val);


	/* Set VGEN3 to 1.8V, VCAM to 3.0V */
	val = pmic_reg_read(REG_SETTING_0);
	val &= ~(VCAM_MASK | VGEN3_MASK);
	val |= VCAM_3_0;
	pmic_reg_write(REG_SETTING_0, val);

	/* Set VVIDEO to 2.775V, VAUDIO to 3V0, VSD to 1.8V */
	val = pmic_reg_read(REG_SETTING_1);
	val &= ~(VVIDEO_MASK | VSD_MASK | VAUDIO_MASK);
	val |= VVIDEO_2_775 | VAUDIO_3_0 | VSD_1_8;
	pmic_reg_write(REG_SETTING_1, val);

	/* Configure VGEN3 and VCAM regulators to use external PNP */
	val = VGEN3CONFIG | VCAMCONFIG;
	pmic_reg_write(REG_MODE_1, val);
	udelay(200);

	/* Enable VGEN3, VCAM, VAUDIO, VVIDEO, VSD regulators */
	val = VGEN3EN | VGEN3CONFIG | VCAMEN | VCAMCONFIG |
		VVIDEOEN | VAUDIOEN  | VSDEN;
	pmic_reg_write(REG_MODE_1, val);

	val = pmic_reg_read(REG_POWER_CTL2);
	val |= WDIRESET;
	pmic_reg_write(REG_POWER_CTL2, val);

	udelay(2500);

}
#endif

static void setup_gpios(void)
{
	unsigned int i;

	/* CAM_SUP_DISn, GPIO1_7 */
	mxc_request_iomux(MX51_PIN_GPIO1_7, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_GPIO1_7, 0x82);

	/* DAB Display EN, GPIO3_1 */
	mxc_request_iomux(MX51_PIN_DI1_PIN12, IOMUX_CONFIG_ALT4);
	mxc_iomux_set_pad(MX51_PIN_DI1_PIN12, 0x82);

	/* WDOG_TRIGGER, GPIO3_2 */
	mxc_request_iomux(MX51_PIN_DI1_PIN13, IOMUX_CONFIG_ALT4);
	mxc_iomux_set_pad(MX51_PIN_DI1_PIN13, 0x82);

	/* Now we need to trigger the watchdog */
	WATCHDOG_RESET();

	/* Display2 TxEN, GPIO3_3 */
	mxc_request_iomux(MX51_PIN_DI1_D0_CS, IOMUX_CONFIG_ALT4);
	mxc_iomux_set_pad(MX51_PIN_DI1_D0_CS, 0x82);

	/* DAB Light EN, GPIO3_4 */
	mxc_request_iomux(MX51_PIN_DI1_D1_CS, IOMUX_CONFIG_ALT4);
	mxc_iomux_set_pad(MX51_PIN_DI1_D1_CS, 0x82);

	/* AUDIO_MUTE, GPIO3_5 */
	mxc_request_iomux(MX51_PIN_DISPB2_SER_DIN, IOMUX_CONFIG_ALT4);
	mxc_iomux_set_pad(MX51_PIN_DISPB2_SER_DIN, 0x82);

	/* SPARE_OUT, GPIO3_6 */
	mxc_request_iomux(MX51_PIN_DISPB2_SER_DIO, IOMUX_CONFIG_ALT4);
	mxc_iomux_set_pad(MX51_PIN_DISPB2_SER_DIO, 0x82);

	/* BEEPER_EN, GPIO3_26 */
	mxc_request_iomux(MX51_PIN_NANDF_D14, IOMUX_CONFIG_ALT3);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D14, 0x82);

	/* POWER_OFF, GPIO3_27 */
	mxc_request_iomux(MX51_PIN_NANDF_D13, IOMUX_CONFIG_ALT3);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D13, 0x82);

	/* FRAM_WE, GPIO3_30 */
	mxc_request_iomux(MX51_PIN_NANDF_D10, IOMUX_CONFIG_ALT3);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D10, 0x82);

	/* EXPANSION_EN, GPIO4_26 */
	mxc_request_iomux(MX51_PIN_CSPI1_RDY, IOMUX_CONFIG_ALT3);
	mxc_iomux_set_pad(MX51_PIN_CSPI1_RDY, 0x82);

	/*
	 * Set GPIO1_4 to high and output; it is used to reset
	 * the system on reboot
	 */
	mxc_gpio_set(4, 1);
	mxc_gpio_direction(4, MXC_GPIO_DIRECTION_OUT);

	mxc_gpio_set(7, 0);
	mxc_gpio_direction(7, MXC_GPIO_DIRECTION_OUT);
	for (i = 65; i < 71; i++) {
		mxc_gpio_set(i, 0);
		mxc_gpio_direction(i, MXC_GPIO_DIRECTION_OUT);
	}

	mxc_gpio_set(94, 0);
	mxc_gpio_direction(94, MXC_GPIO_DIRECTION_OUT);

	/* Set POWER_OFF high */
	mxc_gpio_set(91, 1);
	mxc_gpio_direction(91, MXC_GPIO_DIRECTION_OUT);

	mxc_gpio_set(90, 0);
	mxc_gpio_direction(90, MXC_GPIO_DIRECTION_OUT);

	mxc_gpio_set(122, 0);
	mxc_gpio_direction(122, MXC_GPIO_DIRECTION_OUT);

	mxc_gpio_set(121, 1);
	mxc_gpio_direction(121, MXC_GPIO_DIRECTION_OUT);

	WATCHDOG_RESET();
}

static void setup_fec(void)
{
	/*FEC_MDIO*/
	mxc_request_iomux(MX51_PIN_EIM_EB2, IOMUX_CONFIG_ALT3);
	mxc_iomux_set_pad(MX51_PIN_EIM_EB2, 0x1FD);

	/*FEC_MDC*/
	mxc_request_iomux(MX51_PIN_NANDF_CS3, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_NANDF_CS3, 0x2004);

	/* FEC RDATA[3] */
	mxc_request_iomux(MX51_PIN_EIM_CS3, IOMUX_CONFIG_ALT3);
	mxc_iomux_set_pad(MX51_PIN_EIM_CS3, 0x180);

	/* FEC RDATA[2] */
	mxc_request_iomux(MX51_PIN_EIM_CS2, IOMUX_CONFIG_ALT3);
	mxc_iomux_set_pad(MX51_PIN_EIM_CS2, 0x180);

	/* FEC RDATA[1] */
	mxc_request_iomux(MX51_PIN_EIM_EB3, IOMUX_CONFIG_ALT3);
	mxc_iomux_set_pad(MX51_PIN_EIM_EB3, 0x180);

	/* FEC RDATA[0] */
	mxc_request_iomux(MX51_PIN_NANDF_D9, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D9, 0x2180);

	/* FEC TDATA[3] */
	mxc_request_iomux(MX51_PIN_NANDF_CS6, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_NANDF_CS6, 0x2004);

	/* FEC TDATA[2] */
	mxc_request_iomux(MX51_PIN_NANDF_CS5, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_NANDF_CS5, 0x2004);

	/* FEC TDATA[1] */
	mxc_request_iomux(MX51_PIN_NANDF_CS4, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_NANDF_CS4, 0x2004);

	/* FEC TDATA[0] */
	mxc_request_iomux(MX51_PIN_NANDF_D8, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D8, 0x2004);

	/* FEC TX_EN */
	mxc_request_iomux(MX51_PIN_NANDF_CS7, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_CS7, 0x2004);

	/* FEC TX_ER */
	mxc_request_iomux(MX51_PIN_NANDF_CS2, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_NANDF_CS2, 0x2004);

	/* FEC TX_CLK */
	mxc_request_iomux(MX51_PIN_NANDF_RDY_INT, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_RDY_INT, 0x2180);

	/* FEC TX_COL */
	mxc_request_iomux(MX51_PIN_NANDF_RB2, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_RB2, 0x2180);

	/* FEC RX_CLK */
	mxc_request_iomux(MX51_PIN_NANDF_RB3, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_RB3, 0x2180);

	/* FEC RX_CRS */
	mxc_request_iomux(MX51_PIN_EIM_CS5, IOMUX_CONFIG_ALT3);
	mxc_iomux_set_pad(MX51_PIN_EIM_CS5, 0x180);

	/* FEC RX_ER */
	mxc_request_iomux(MX51_PIN_EIM_CS4, IOMUX_CONFIG_ALT3);
	mxc_iomux_set_pad(MX51_PIN_EIM_CS4, 0x180);

	/* FEC RX_DV */
	mxc_request_iomux(MX51_PIN_NANDF_D11, IOMUX_CONFIG_ALT2);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D11, 0x2180);
}

struct fsl_esdhc_cfg esdhc_cfg[1] = {
	{MMC_SDHC1_BASE_ADDR, 1},
};

int get_mmc_getcd(u8 *cd, struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;

	if (cfg->esdhc_base == MMC_SDHC1_BASE_ADDR)
		*cd = mxc_gpio_get(0);
	else
		*cd = 0;

	return 0;
}

#ifdef CONFIG_FSL_ESDHC
int board_mmc_init(bd_t *bis)
{
	mxc_request_iomux(MX51_PIN_SD1_CMD,
		IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
	mxc_request_iomux(MX51_PIN_SD1_CLK,
		IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
	mxc_request_iomux(MX51_PIN_SD1_DATA0,
		IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
	mxc_request_iomux(MX51_PIN_SD1_DATA1,
		IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
	mxc_request_iomux(MX51_PIN_SD1_DATA2,
		IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
	mxc_request_iomux(MX51_PIN_SD1_DATA3,
		IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
	mxc_iomux_set_pad(MX51_PIN_SD1_CMD,
		PAD_CTL_DRV_MAX | PAD_CTL_DRV_VOT_HIGH |
		PAD_CTL_HYS_ENABLE | PAD_CTL_47K_PU |
		PAD_CTL_PUE_PULL |
		PAD_CTL_PKE_ENABLE | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_SD1_CLK,
		PAD_CTL_DRV_MAX | PAD_CTL_DRV_VOT_HIGH |
		PAD_CTL_HYS_NONE | PAD_CTL_47K_PU |
		PAD_CTL_PUE_PULL |
		PAD_CTL_PKE_ENABLE | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_SD1_DATA0,
		PAD_CTL_DRV_MAX | PAD_CTL_DRV_VOT_HIGH |
		PAD_CTL_HYS_ENABLE | PAD_CTL_47K_PU |
		PAD_CTL_PUE_PULL |
		PAD_CTL_PKE_ENABLE | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_SD1_DATA1,
		PAD_CTL_DRV_MAX | PAD_CTL_DRV_VOT_HIGH |
		PAD_CTL_HYS_ENABLE | PAD_CTL_47K_PU |
		PAD_CTL_PUE_PULL |
		PAD_CTL_PKE_ENABLE | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_SD1_DATA2,
		PAD_CTL_DRV_MAX | PAD_CTL_DRV_VOT_HIGH |
		PAD_CTL_HYS_ENABLE | PAD_CTL_47K_PU |
		PAD_CTL_PUE_PULL |
		PAD_CTL_PKE_ENABLE | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_SD1_DATA3,
		PAD_CTL_DRV_MAX | PAD_CTL_DRV_VOT_HIGH |
		PAD_CTL_HYS_ENABLE | PAD_CTL_100K_PD |
		PAD_CTL_PUE_PULL |
		PAD_CTL_PKE_ENABLE | PAD_CTL_SRE_FAST);
	mxc_request_iomux(MX51_PIN_GPIO1_0,
		IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
	mxc_iomux_set_pad(MX51_PIN_GPIO1_0,
		PAD_CTL_HYS_ENABLE);
	mxc_request_iomux(MX51_PIN_GPIO1_1,
		IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
	mxc_iomux_set_pad(MX51_PIN_GPIO1_1,
		PAD_CTL_HYS_ENABLE);

	return fsl_esdhc_initialize(bis, &esdhc_cfg[0]);
}
#endif

int board_early_init_f(void)
{


	init_drive_strength();

	/* Setup debug led */
	mxc_gpio_set(6, 0);
	mxc_gpio_direction(6, MXC_GPIO_DIRECTION_OUT);
	mxc_request_iomux(MX51_PIN_GPIO1_6, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_GPIO1_6, PAD_CTL_DRV_MAX | PAD_CTL_SRE_FAST);

	/* wait a little while to give the pll time to settle */
	sdelay(100000);

	setup_weim();
	setup_uart();
	setup_fec();
	setup_gpios();

	spi_io_init();

	return 0;
}

int board_init(void)
{
#ifdef CONFIG_SYS_ARM_WITHOUT_RELOC
	board_early_init_f();
#endif
	gd->bd->bi_arch_number = MACH_TYPE_TTC_VISION2;	/* board id for linux */
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

	return 0;
}

int board_late_init(void)
{
	power_init_mx51();

	reset_peripherals(1);
	udelay(2000);
	reset_peripherals(0);
	udelay(2000);

	/* Early revisions require a second reset */
#ifdef CONFIG_VISION2_HW_1_0
	reset_peripherals(1);
	udelay(2000);
	reset_peripherals(0);
	udelay(2000);
#endif

	return 0;
}

int checkboard(void)
{
	u32 system_rev = get_cpu_rev();
	u32 cause;
	struct src *src_regs = (struct src *)SRC_BASE_ADDR;

	puts("Board: TTControl Vision II CPU V");

	switch (system_rev & 0xff) {
	case CHIP_REV_3_0:
		puts("3.0 [");
		break;
	case CHIP_REV_2_5:
		puts("2.5 [");
		break;
	case CHIP_REV_2_0:
		puts("2.0 [");
		break;
	case CHIP_REV_1_1:
		puts("1.1 [");
		break;
	case CHIP_REV_1_0:
	default:
		puts("1.0 [");
		break;
	}

	cause = src_regs->srsr;
	switch (cause) {
	case 0x0001:
		puts("POR");
		break;
	case 0x0009:
		puts("RST");
		break;
	case 0x0010:
	case 0x0011:
		puts("WDOG");
		break;
	default:
		printf("unknown 0x%x", cause);
	}
	puts("]\n");

	return 0;
}


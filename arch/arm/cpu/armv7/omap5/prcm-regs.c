/*
 *
 * HW regs data for OMAP5 Soc
 *
 * (C) Copyright 2013
 * Texas Instruments, <www.ti.com>
 *
 * Sricharan R <r.sricharan@ti.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <asm/omap_common.h>

struct prcm_regs const omap5_es1_prcm = {
	/* cm1.ckgen */
	.cm_clksel_core = 0x4a004100,
	.cm_clksel_abe = 0x4a004108,
	.cm_dll_ctrl = 0x4a004110,
	.cm_clkmode_dpll_core = 0x4a004120,
	.cm_idlest_dpll_core = 0x4a004124,
	.cm_autoidle_dpll_core = 0x4a004128,
	.cm_clksel_dpll_core = 0x4a00412c,
	.cm_div_m2_dpll_core = 0x4a004130,
	.cm_div_m3_dpll_core = 0x4a004134,
	.cm_div_h11_dpll_core = 0x4a004138,
	.cm_div_h12_dpll_core = 0x4a00413c,
	.cm_div_h13_dpll_core = 0x4a004140,
	.cm_div_h14_dpll_core = 0x4a004144,
	.cm_ssc_deltamstep_dpll_core = 0x4a004148,
	.cm_ssc_modfreqdiv_dpll_core = 0x4a00414c,
	.cm_emu_override_dpll_core = 0x4a004150,
	.cm_div_h22_dpllcore = 0x4a004154,
	.cm_div_h23_dpll_core = 0x4a004158,
	.cm_clkmode_dpll_mpu = 0x4a004160,
	.cm_idlest_dpll_mpu = 0x4a004164,
	.cm_autoidle_dpll_mpu = 0x4a004168,
	.cm_clksel_dpll_mpu = 0x4a00416c,
	.cm_div_m2_dpll_mpu = 0x4a004170,
	.cm_ssc_deltamstep_dpll_mpu = 0x4a004188,
	.cm_ssc_modfreqdiv_dpll_mpu = 0x4a00418c,
	.cm_bypclk_dpll_mpu = 0x4a00419c,
	.cm_clkmode_dpll_iva = 0x4a0041a0,
	.cm_idlest_dpll_iva = 0x4a0041a4,
	.cm_autoidle_dpll_iva = 0x4a0041a8,
	.cm_clksel_dpll_iva = 0x4a0041ac,
	.cm_div_h11_dpll_iva = 0x4a0041b8,
	.cm_div_h12_dpll_iva = 0x4a0041bc,
	.cm_ssc_deltamstep_dpll_iva = 0x4a0041c8,
	.cm_ssc_modfreqdiv_dpll_iva = 0x4a0041cc,
	.cm_bypclk_dpll_iva = 0x4a0041dc,
	.cm_clkmode_dpll_abe = 0x4a0041e0,
	.cm_idlest_dpll_abe = 0x4a0041e4,
	.cm_autoidle_dpll_abe = 0x4a0041e8,
	.cm_clksel_dpll_abe = 0x4a0041ec,
	.cm_div_m2_dpll_abe = 0x4a0041f0,
	.cm_div_m3_dpll_abe = 0x4a0041f4,
	.cm_ssc_deltamstep_dpll_abe = 0x4a004208,
	.cm_ssc_modfreqdiv_dpll_abe = 0x4a00420c,
	.cm_clkmode_dpll_ddrphy = 0x4a004220,
	.cm_idlest_dpll_ddrphy = 0x4a004224,
	.cm_autoidle_dpll_ddrphy = 0x4a004228,
	.cm_clksel_dpll_ddrphy = 0x4a00422c,
	.cm_div_m2_dpll_ddrphy = 0x4a004230,
	.cm_div_h11_dpll_ddrphy = 0x4a004238,
	.cm_div_h12_dpll_ddrphy = 0x4a00423c,
	.cm_div_h13_dpll_ddrphy = 0x4a004240,
	.cm_ssc_deltamstep_dpll_ddrphy = 0x4a004248,
	.cm_shadow_freq_config1 = 0x4a004260,
	.cm_mpu_mpu_clkctrl = 0x4a004320,

	/* cm1.dsp */
	.cm_dsp_clkstctrl = 0x4a004400,
	.cm_dsp_dsp_clkctrl = 0x4a004420,

	/* cm1.abe */
	.cm1_abe_clkstctrl = 0x4a004500,
	.cm1_abe_l4abe_clkctrl = 0x4a004520,
	.cm1_abe_aess_clkctrl = 0x4a004528,
	.cm1_abe_pdm_clkctrl = 0x4a004530,
	.cm1_abe_dmic_clkctrl = 0x4a004538,
	.cm1_abe_mcasp_clkctrl = 0x4a004540,
	.cm1_abe_mcbsp1_clkctrl = 0x4a004548,
	.cm1_abe_mcbsp2_clkctrl = 0x4a004550,
	.cm1_abe_mcbsp3_clkctrl = 0x4a004558,
	.cm1_abe_slimbus_clkctrl = 0x4a004560,
	.cm1_abe_timer5_clkctrl = 0x4a004568,
	.cm1_abe_timer6_clkctrl = 0x4a004570,
	.cm1_abe_timer7_clkctrl = 0x4a004578,
	.cm1_abe_timer8_clkctrl = 0x4a004580,
	.cm1_abe_wdt3_clkctrl = 0x4a004588,

	/* cm2.ckgen */
	.cm_clksel_mpu_m3_iss_root = 0x4a008100,
	.cm_clksel_usb_60mhz = 0x4a008104,
	.cm_scale_fclk = 0x4a008108,
	.cm_core_dvfs_perf1 = 0x4a008110,
	.cm_core_dvfs_perf2 = 0x4a008114,
	.cm_core_dvfs_perf3 = 0x4a008118,
	.cm_core_dvfs_perf4 = 0x4a00811c,
	.cm_core_dvfs_current = 0x4a008124,
	.cm_iva_dvfs_perf_tesla = 0x4a008128,
	.cm_iva_dvfs_perf_ivahd = 0x4a00812c,
	.cm_iva_dvfs_perf_abe = 0x4a008130,
	.cm_iva_dvfs_current = 0x4a008138,
	.cm_clkmode_dpll_per = 0x4a008140,
	.cm_idlest_dpll_per = 0x4a008144,
	.cm_autoidle_dpll_per = 0x4a008148,
	.cm_clksel_dpll_per = 0x4a00814c,
	.cm_div_m2_dpll_per = 0x4a008150,
	.cm_div_m3_dpll_per = 0x4a008154,
	.cm_div_h11_dpll_per = 0x4a008158,
	.cm_div_h12_dpll_per = 0x4a00815c,
	.cm_div_h14_dpll_per = 0x4a008164,
	.cm_ssc_deltamstep_dpll_per = 0x4a008168,
	.cm_ssc_modfreqdiv_dpll_per = 0x4a00816c,
	.cm_emu_override_dpll_per = 0x4a008170,
	.cm_clkmode_dpll_usb = 0x4a008180,
	.cm_idlest_dpll_usb = 0x4a008184,
	.cm_autoidle_dpll_usb = 0x4a008188,
	.cm_clksel_dpll_usb = 0x4a00818c,
	.cm_div_m2_dpll_usb = 0x4a008190,
	.cm_ssc_deltamstep_dpll_usb = 0x4a0081a8,
	.cm_ssc_modfreqdiv_dpll_usb = 0x4a0081ac,
	.cm_clkdcoldo_dpll_usb = 0x4a0081b4,
	.cm_clkmode_dpll_unipro = 0x4a0081c0,
	.cm_idlest_dpll_unipro = 0x4a0081c4,
	.cm_autoidle_dpll_unipro = 0x4a0081c8,
	.cm_clksel_dpll_unipro = 0x4a0081cc,
	.cm_div_m2_dpll_unipro = 0x4a0081d0,
	.cm_ssc_deltamstep_dpll_unipro = 0x4a0081e8,
	.cm_ssc_modfreqdiv_dpll_unipro = 0x4a0081ec,

	/* cm2.core */
	.cm_coreaon_bandgap_clkctrl = 0x4a008648,
	.cm_coreaon_io_srcomp_clkctrl = 0x4a008650,
	.cm_l3_1_clkstctrl = 0x4a008700,
	.cm_l3_1_dynamicdep = 0x4a008708,
	.cm_l3_1_l3_1_clkctrl = 0x4a008720,
	.cm_l3_2_clkstctrl = 0x4a008800,
	.cm_l3_2_dynamicdep = 0x4a008808,
	.cm_l3_2_l3_2_clkctrl = 0x4a008820,
	.cm_l3_gpmc_clkctrl = 0x4a008828,
	.cm_l3_2_ocmc_ram_clkctrl = 0x4a008830,
	.cm_mpu_m3_clkstctrl = 0x4a008900,
	.cm_mpu_m3_staticdep = 0x4a008904,
	.cm_mpu_m3_dynamicdep = 0x4a008908,
	.cm_mpu_m3_mpu_m3_clkctrl = 0x4a008920,
	.cm_sdma_clkstctrl = 0x4a008a00,
	.cm_sdma_staticdep = 0x4a008a04,
	.cm_sdma_dynamicdep = 0x4a008a08,
	.cm_sdma_sdma_clkctrl = 0x4a008a20,
	.cm_memif_clkstctrl = 0x4a008b00,
	.cm_memif_dmm_clkctrl = 0x4a008b20,
	.cm_memif_emif_fw_clkctrl = 0x4a008b28,
	.cm_memif_emif_1_clkctrl = 0x4a008b30,
	.cm_memif_emif_2_clkctrl = 0x4a008b38,
	.cm_memif_dll_clkctrl = 0x4a008b40,
	.cm_memif_emif_h1_clkctrl = 0x4a008b50,
	.cm_memif_emif_h2_clkctrl = 0x4a008b58,
	.cm_memif_dll_h_clkctrl = 0x4a008b60,
	.cm_c2c_clkstctrl = 0x4a008c00,
	.cm_c2c_staticdep = 0x4a008c04,
	.cm_c2c_dynamicdep = 0x4a008c08,
	.cm_c2c_sad2d_clkctrl = 0x4a008c20,
	.cm_c2c_modem_icr_clkctrl = 0x4a008c28,
	.cm_c2c_sad2d_fw_clkctrl = 0x4a008c30,
	.cm_l4cfg_clkstctrl = 0x4a008d00,
	.cm_l4cfg_dynamicdep = 0x4a008d08,
	.cm_l4cfg_l4_cfg_clkctrl = 0x4a008d20,
	.cm_l4cfg_hw_sem_clkctrl = 0x4a008d28,
	.cm_l4cfg_mailbox_clkctrl = 0x4a008d30,
	.cm_l4cfg_sar_rom_clkctrl = 0x4a008d38,
	.cm_l3instr_clkstctrl = 0x4a008e00,
	.cm_l3instr_l3_3_clkctrl = 0x4a008e20,
	.cm_l3instr_l3_instr_clkctrl = 0x4a008e28,
	.cm_l3instr_intrconn_wp1_clkctrl = 0x4a008e40,

	/* cm2.ivahd */
	.cm_ivahd_clkstctrl = 0x4a008f00,
	.cm_ivahd_ivahd_clkctrl = 0x4a008f20,
	.cm_ivahd_sl2_clkctrl = 0x4a008f28,

	/* cm2.cam */
	.cm_cam_clkstctrl = 0x4a009000,
	.cm_cam_iss_clkctrl = 0x4a009020,
	.cm_cam_fdif_clkctrl = 0x4a009028,

	/* cm2.dss */
	.cm_dss_clkstctrl = 0x4a009100,
	.cm_dss_dss_clkctrl = 0x4a009120,

	/* cm2.sgx */
	.cm_sgx_clkstctrl = 0x4a009200,
	.cm_sgx_sgx_clkctrl = 0x4a009220,

	/* cm2.l3init */
	.cm_l3init_clkstctrl = 0x4a009300,
	.cm_l3init_hsmmc1_clkctrl = 0x4a009328,
	.cm_l3init_hsmmc2_clkctrl = 0x4a009330,
	.cm_l3init_hsi_clkctrl = 0x4a009338,
	.cm_l3init_hsusbhost_clkctrl = 0x4a009358,
	.cm_l3init_hsusbotg_clkctrl = 0x4a009360,
	.cm_l3init_hsusbtll_clkctrl = 0x4a009368,
	.cm_l3init_p1500_clkctrl = 0x4a009378,
	.cm_l3init_fsusb_clkctrl = 0x4a0093d0,
	.cm_l3init_ocp2scp1_clkctrl = 0x4a0093e0,

	/* cm2.l4per */
	.cm_l4per_clkstctrl = 0x4a009400,
	.cm_l4per_dynamicdep = 0x4a009408,
	.cm_l4per_adc_clkctrl = 0x4a009420,
	.cm_l4per_gptimer10_clkctrl = 0x4a009428,
	.cm_l4per_gptimer11_clkctrl = 0x4a009430,
	.cm_l4per_gptimer2_clkctrl = 0x4a009438,
	.cm_l4per_gptimer3_clkctrl = 0x4a009440,
	.cm_l4per_gptimer4_clkctrl = 0x4a009448,
	.cm_l4per_gptimer9_clkctrl = 0x4a009450,
	.cm_l4per_elm_clkctrl = 0x4a009458,
	.cm_l4per_gpio2_clkctrl = 0x4a009460,
	.cm_l4per_gpio3_clkctrl = 0x4a009468,
	.cm_l4per_gpio4_clkctrl = 0x4a009470,
	.cm_l4per_gpio5_clkctrl = 0x4a009478,
	.cm_l4per_gpio6_clkctrl = 0x4a009480,
	.cm_l4per_hdq1w_clkctrl = 0x4a009488,
	.cm_l4per_hecc1_clkctrl = 0x4a009490,
	.cm_l4per_hecc2_clkctrl = 0x4a009498,
	.cm_l4per_i2c1_clkctrl = 0x4a0094a0,
	.cm_l4per_i2c2_clkctrl = 0x4a0094a8,
	.cm_l4per_i2c3_clkctrl = 0x4a0094b0,
	.cm_l4per_i2c4_clkctrl = 0x4a0094b8,
	.cm_l4per_l4per_clkctrl = 0x4a0094c0,
	.cm_l4per_mcasp2_clkctrl = 0x4a0094d0,
	.cm_l4per_mcasp3_clkctrl = 0x4a0094d8,
	.cm_l4per_mgate_clkctrl = 0x4a0094e8,
	.cm_l4per_mcspi1_clkctrl = 0x4a0094f0,
	.cm_l4per_mcspi2_clkctrl = 0x4a0094f8,
	.cm_l4per_mcspi3_clkctrl = 0x4a009500,
	.cm_l4per_mcspi4_clkctrl = 0x4a009508,
	.cm_l4per_gpio7_clkctrl = 0x4a009510,
	.cm_l4per_gpio8_clkctrl = 0x4a009518,
	.cm_l4per_mmcsd3_clkctrl = 0x4a009520,
	.cm_l4per_mmcsd4_clkctrl = 0x4a009528,
	.cm_l4per_msprohg_clkctrl = 0x4a009530,
	.cm_l4per_slimbus2_clkctrl = 0x4a009538,
	.cm_l4per_uart1_clkctrl = 0x4a009540,
	.cm_l4per_uart2_clkctrl = 0x4a009548,
	.cm_l4per_uart3_clkctrl = 0x4a009550,
	.cm_l4per_uart4_clkctrl = 0x4a009558,
	.cm_l4per_mmcsd5_clkctrl = 0x4a009560,
	.cm_l4per_i2c5_clkctrl = 0x4a009568,
	.cm_l4per_uart5_clkctrl = 0x4a009570,
	.cm_l4per_uart6_clkctrl = 0x4a009578,
	.cm_l4sec_clkstctrl = 0x4a009580,
	.cm_l4sec_staticdep = 0x4a009584,
	.cm_l4sec_dynamicdep = 0x4a009588,
	.cm_l4sec_aes1_clkctrl = 0x4a0095a0,
	.cm_l4sec_aes2_clkctrl = 0x4a0095a8,
	.cm_l4sec_des3des_clkctrl = 0x4a0095b0,
	.cm_l4sec_pkaeip29_clkctrl = 0x4a0095b8,
	.cm_l4sec_rng_clkctrl = 0x4a0095c0,
	.cm_l4sec_sha2md51_clkctrl = 0x4a0095c8,
	.cm_l4sec_cryptodma_clkctrl = 0x4a0095d8,

	/* l4 wkup regs */
	.cm_abe_pll_ref_clksel = 0x4ae0610c,
	.cm_sys_clksel = 0x4ae06110,
	.cm_wkup_clkstctrl = 0x4ae07800,
	.cm_wkup_l4wkup_clkctrl = 0x4ae07820,
	.cm_wkup_wdtimer1_clkctrl = 0x4ae07828,
	.cm_wkup_wdtimer2_clkctrl = 0x4ae07830,
	.cm_wkup_gpio1_clkctrl = 0x4ae07838,
	.cm_wkup_gptimer1_clkctrl = 0x4ae07840,
	.cm_wkup_gptimer12_clkctrl = 0x4ae07848,
	.cm_wkup_synctimer_clkctrl = 0x4ae07850,
	.cm_wkup_usim_clkctrl = 0x4ae07858,
	.cm_wkup_sarram_clkctrl = 0x4ae07860,
	.cm_wkup_keyboard_clkctrl = 0x4ae07878,
	.cm_wkup_rtc_clkctrl = 0x4ae07880,
	.cm_wkup_bandgap_clkctrl = 0x4ae07888,
	.cm_wkupaon_scrm_clkctrl = 0x4ae07890,
	.cm_wkupaon_io_srcomp_clkctrl = 0x4ae07898,
	.prm_rstctrl = 0x4ae07b00,
	.prm_rstst = 0x4ae07b04,
	.prm_vc_val_bypass = 0x4ae07ba0,
	.prm_vc_cfg_i2c_mode = 0x4ae07bb4,
	.prm_vc_cfg_i2c_clk = 0x4ae07bb8,
	.prm_sldo_core_setup = 0x4ae07bc4,
	.prm_sldo_core_ctrl = 0x4ae07bc8,
	.prm_sldo_mpu_setup = 0x4ae07bcc,
	.prm_sldo_mpu_ctrl = 0x4ae07bd0,
	.prm_sldo_mm_setup = 0x4ae07bd4,
	.prm_sldo_mm_ctrl = 0x4ae07bd8,
};

struct omap_sys_ctrl_regs const omap5_ctrl = {
	.control_status				= 0x4A002134,
	.control_paconf_global			= 0x4A002DA0,
	.control_paconf_mode			= 0x4A002DA4,
	.control_smart1io_padconf_0		= 0x4A002DA8,
	.control_smart1io_padconf_1		= 0x4A002DAC,
	.control_smart1io_padconf_2		= 0x4A002DB0,
	.control_smart2io_padconf_0		= 0x4A002DB4,
	.control_smart2io_padconf_1		= 0x4A002DB8,
	.control_smart2io_padconf_2		= 0x4A002DBC,
	.control_smart3io_padconf_0		= 0x4A002DC0,
	.control_smart3io_padconf_1		= 0x4A002DC4,
	.control_pbias				= 0x4A002E00,
	.control_i2c_0				= 0x4A002E04,
	.control_camera_rx			= 0x4A002E08,
	.control_hdmi_tx_phy			= 0x4A002E0C,
	.control_uniportm			= 0x4A002E10,
	.control_dsiphy				= 0x4A002E14,
	.control_mcbsplp			= 0x4A002E18,
	.control_usb2phycore			= 0x4A002E1C,
	.control_hdmi_1				= 0x4A002E20,
	.control_hsi				= 0x4A002E24,
	.control_ddr3ch1_0			= 0x4A002E30,
	.control_ddr3ch2_0			= 0x4A002E34,
	.control_ddrch1_0			= 0x4A002E38,
	.control_ddrch1_1			= 0x4A002E3C,
	.control_ddrch2_0			= 0x4A002E40,
	.control_ddrch2_1			= 0x4A002E44,
	.control_lpddr2ch1_0			= 0x4A002E48,
	.control_lpddr2ch1_1			= 0x4A002E4C,
	.control_ddrio_0			= 0x4A002E50,
	.control_ddrio_1			= 0x4A002E54,
	.control_ddrio_2			= 0x4A002E58,
	.control_hyst_1				= 0x4A002E5C,
	.control_usbb_hsic_control		= 0x4A002E60,
	.control_c2c				= 0x4A002E64,
	.control_core_control_spare_rw		= 0x4A002E68,
	.control_core_control_spare_r		= 0x4A002E6C,
	.control_core_control_spare_r_c0	= 0x4A002E70,
	.control_srcomp_north_side		= 0x4A002E74,
	.control_srcomp_south_side		= 0x4A002E78,
	.control_srcomp_east_side		= 0x4A002E7C,
	.control_srcomp_west_side		= 0x4A002E80,
	.control_srcomp_code_latch		= 0x4A002E84,
	.control_port_emif1_sdram_config	= 0x4AE0C110,
	.control_port_emif1_lpddr2_nvm_config	= 0x4AE0C114,
	.control_port_emif2_sdram_config	= 0x4AE0C118,
	.control_emif1_sdram_config_ext		= 0x4AE0C144,
	.control_emif2_sdram_config_ext		= 0x4AE0C148,
	.control_smart1nopmio_padconf_0		= 0x4AE0CDA0,
	.control_smart1nopmio_padconf_1		= 0x4AE0CDA4,
	.control_padconf_mode			= 0x4AE0CDA8,
	.control_xtal_oscillator		= 0x4AE0CDAC,
	.control_i2c_2				= 0x4AE0CDB0,
	.control_ckobuffer			= 0x4AE0CDB4,
	.control_wkup_control_spare_rw		= 0x4AE0CDB8,
	.control_wkup_control_spare_r		= 0x4AE0CDBC,
	.control_wkup_control_spare_r_c0	= 0x4AE0CDC0,
	.control_srcomp_east_side_wkup		= 0x4AE0CDC4,
	.control_efuse_1			= 0x4AE0CDC8,
	.control_efuse_2			= 0x4AE0CDCC,
	.control_efuse_3			= 0x4AE0CDD0,
	.control_efuse_4			= 0x4AE0CDD4,
	.control_efuse_5			= 0x4AE0CDD8,
	.control_efuse_6			= 0x4AE0CDDC,
	.control_efuse_7			= 0x4AE0CDE0,
	.control_efuse_8			= 0x4AE0CDE4,
	.control_efuse_9			= 0x4AE0CDE8,
	.control_efuse_10			= 0x4AE0CDEC,
	.control_efuse_11			= 0x4AE0CDF0,
	.control_efuse_12			= 0x4AE0CDF4,
	.control_efuse_13			= 0x4AE0CDF8,
};

struct prcm_regs const omap5_es2_prcm = {
	/* cm1.ckgen */
	.cm_clksel_core = 0x4a004100,
	.cm_clksel_abe = 0x4a004108,
	.cm_dll_ctrl = 0x4a004110,
	.cm_clkmode_dpll_core = 0x4a004120,
	.cm_idlest_dpll_core = 0x4a004124,
	.cm_autoidle_dpll_core = 0x4a004128,
	.cm_clksel_dpll_core = 0x4a00412c,
	.cm_div_m2_dpll_core = 0x4a004130,
	.cm_div_m3_dpll_core = 0x4a004134,
	.cm_div_h11_dpll_core = 0x4a004138,
	.cm_div_h12_dpll_core = 0x4a00413c,
	.cm_div_h13_dpll_core = 0x4a004140,
	.cm_div_h14_dpll_core = 0x4a004144,
	.cm_ssc_deltamstep_dpll_core = 0x4a004148,
	.cm_ssc_modfreqdiv_dpll_core = 0x4a00414c,
	.cm_div_h21_dpll_core = 0x4a004150,
	.cm_div_h22_dpllcore = 0x4a004154,
	.cm_div_h23_dpll_core = 0x4a004158,
	.cm_div_h24_dpll_core = 0x4a00415c,
	.cm_clkmode_dpll_mpu = 0x4a004160,
	.cm_idlest_dpll_mpu = 0x4a004164,
	.cm_autoidle_dpll_mpu = 0x4a004168,
	.cm_clksel_dpll_mpu = 0x4a00416c,
	.cm_div_m2_dpll_mpu = 0x4a004170,
	.cm_ssc_deltamstep_dpll_mpu = 0x4a004188,
	.cm_ssc_modfreqdiv_dpll_mpu = 0x4a00418c,
	.cm_bypclk_dpll_mpu = 0x4a00419c,
	.cm_clkmode_dpll_iva = 0x4a0041a0,
	.cm_idlest_dpll_iva = 0x4a0041a4,
	.cm_autoidle_dpll_iva = 0x4a0041a8,
	.cm_clksel_dpll_iva = 0x4a0041ac,
	.cm_div_h11_dpll_iva = 0x4a0041b8,
	.cm_div_h12_dpll_iva = 0x4a0041bc,
	.cm_ssc_deltamstep_dpll_iva = 0x4a0041c8,
	.cm_ssc_modfreqdiv_dpll_iva = 0x4a0041cc,
	.cm_bypclk_dpll_iva = 0x4a0041dc,
	.cm_clkmode_dpll_abe = 0x4a0041e0,
	.cm_idlest_dpll_abe = 0x4a0041e4,
	.cm_autoidle_dpll_abe = 0x4a0041e8,
	.cm_clksel_dpll_abe = 0x4a0041ec,
	.cm_div_m2_dpll_abe = 0x4a0041f0,
	.cm_div_m3_dpll_abe = 0x4a0041f4,
	.cm_ssc_deltamstep_dpll_abe = 0x4a004208,
	.cm_ssc_modfreqdiv_dpll_abe = 0x4a00420c,
	.cm_clkmode_dpll_ddrphy = 0x4a004220,
	.cm_idlest_dpll_ddrphy = 0x4a004224,
	.cm_autoidle_dpll_ddrphy = 0x4a004228,
	.cm_clksel_dpll_ddrphy = 0x4a00422c,
	.cm_div_m2_dpll_ddrphy = 0x4a004230,
	.cm_div_h11_dpll_ddrphy = 0x4a004238,
	.cm_div_h12_dpll_ddrphy = 0x4a00423c,
	.cm_div_h13_dpll_ddrphy = 0x4a004240,
	.cm_ssc_deltamstep_dpll_ddrphy = 0x4a004248,
	.cm_shadow_freq_config1 = 0x4a004260,
	.cm_mpu_mpu_clkctrl = 0x4a004320,

	/* cm1.dsp */
	.cm_dsp_clkstctrl = 0x4a004400,
	.cm_dsp_dsp_clkctrl = 0x4a004420,

	/* cm1.abe */
	.cm1_abe_clkstctrl = 0x4a004500,
	.cm1_abe_l4abe_clkctrl = 0x4a004520,
	.cm1_abe_aess_clkctrl = 0x4a004528,
	.cm1_abe_pdm_clkctrl = 0x4a004530,
	.cm1_abe_dmic_clkctrl = 0x4a004538,
	.cm1_abe_mcasp_clkctrl = 0x4a004540,
	.cm1_abe_mcbsp1_clkctrl = 0x4a004548,
	.cm1_abe_mcbsp2_clkctrl = 0x4a004550,
	.cm1_abe_mcbsp3_clkctrl = 0x4a004558,
	.cm1_abe_slimbus_clkctrl = 0x4a004560,
	.cm1_abe_timer5_clkctrl = 0x4a004568,
	.cm1_abe_timer6_clkctrl = 0x4a004570,
	.cm1_abe_timer7_clkctrl = 0x4a004578,
	.cm1_abe_timer8_clkctrl = 0x4a004580,
	.cm1_abe_wdt3_clkctrl = 0x4a004588,



	/* cm2.ckgen */
	.cm_clksel_mpu_m3_iss_root = 0x4a008100,
	.cm_clksel_usb_60mhz = 0x4a008104,
	.cm_scale_fclk = 0x4a008108,
	.cm_core_dvfs_perf1 = 0x4a008110,
	.cm_core_dvfs_perf2 = 0x4a008114,
	.cm_core_dvfs_perf3 = 0x4a008118,
	.cm_core_dvfs_perf4 = 0x4a00811c,
	.cm_core_dvfs_current = 0x4a008124,
	.cm_iva_dvfs_perf_tesla = 0x4a008128,
	.cm_iva_dvfs_perf_ivahd = 0x4a00812c,
	.cm_iva_dvfs_perf_abe = 0x4a008130,
	.cm_iva_dvfs_current = 0x4a008138,
	.cm_clkmode_dpll_per = 0x4a008140,
	.cm_idlest_dpll_per = 0x4a008144,
	.cm_autoidle_dpll_per = 0x4a008148,
	.cm_clksel_dpll_per = 0x4a00814c,
	.cm_div_m2_dpll_per = 0x4a008150,
	.cm_div_m3_dpll_per = 0x4a008154,
	.cm_div_h11_dpll_per = 0x4a008158,
	.cm_div_h12_dpll_per = 0x4a00815c,
	.cm_div_h13_dpll_per = 0x4a008160,
	.cm_div_h14_dpll_per = 0x4a008164,
	.cm_ssc_deltamstep_dpll_per = 0x4a008168,
	.cm_ssc_modfreqdiv_dpll_per = 0x4a00816c,
	.cm_emu_override_dpll_per = 0x4a008170,
	.cm_clkmode_dpll_usb = 0x4a008180,
	.cm_idlest_dpll_usb = 0x4a008184,
	.cm_autoidle_dpll_usb = 0x4a008188,
	.cm_clksel_dpll_usb = 0x4a00818c,
	.cm_div_m2_dpll_usb = 0x4a008190,
	.cm_ssc_deltamstep_dpll_usb = 0x4a0081a8,
	.cm_ssc_modfreqdiv_dpll_usb = 0x4a0081ac,
	.cm_clkdcoldo_dpll_usb = 0x4a0081b4,
	.cm_clkmode_dpll_unipro = 0x4a0081c0,
	.cm_idlest_dpll_unipro = 0x4a0081c4,
	.cm_autoidle_dpll_unipro = 0x4a0081c8,
	.cm_clksel_dpll_unipro = 0x4a0081cc,
	.cm_div_m2_dpll_unipro = 0x4a0081d0,
	.cm_ssc_deltamstep_dpll_unipro = 0x4a0081e8,
	.cm_ssc_modfreqdiv_dpll_unipro = 0x4a0081ec,
	.cm_coreaon_bandgap_clkctrl = 0x4a008648,
	.cm_coreaon_io_srcomp_clkctrl = 0x4a008650,

	/* cm2.core */
	.cm_l3_1_clkstctrl = 0x4a008700,
	.cm_l3_1_dynamicdep = 0x4a008708,
	.cm_l3_1_l3_1_clkctrl = 0x4a008720,
	.cm_l3_2_clkstctrl = 0x4a008800,
	.cm_l3_2_dynamicdep = 0x4a008808,
	.cm_l3_2_l3_2_clkctrl = 0x4a008820,
	.cm_l3_gpmc_clkctrl = 0x4a008828,
	.cm_l3_2_ocmc_ram_clkctrl = 0x4a008830,
	.cm_mpu_m3_clkstctrl = 0x4a008900,
	.cm_mpu_m3_staticdep = 0x4a008904,
	.cm_mpu_m3_dynamicdep = 0x4a008908,
	.cm_mpu_m3_mpu_m3_clkctrl = 0x4a008920,
	.cm_sdma_clkstctrl = 0x4a008a00,
	.cm_sdma_staticdep = 0x4a008a04,
	.cm_sdma_dynamicdep = 0x4a008a08,
	.cm_sdma_sdma_clkctrl = 0x4a008a20,
	.cm_memif_clkstctrl = 0x4a008b00,
	.cm_memif_dmm_clkctrl = 0x4a008b20,
	.cm_memif_emif_fw_clkctrl = 0x4a008b28,
	.cm_memif_emif_1_clkctrl = 0x4a008b30,
	.cm_memif_emif_2_clkctrl = 0x4a008b38,
	.cm_memif_dll_clkctrl = 0x4a008b40,
	.cm_memif_emif_h1_clkctrl = 0x4a008b50,
	.cm_memif_emif_h2_clkctrl = 0x4a008b58,
	.cm_memif_dll_h_clkctrl = 0x4a008b60,
	.cm_c2c_clkstctrl = 0x4a008c00,
	.cm_c2c_staticdep = 0x4a008c04,
	.cm_c2c_dynamicdep = 0x4a008c08,
	.cm_c2c_sad2d_clkctrl = 0x4a008c20,
	.cm_c2c_modem_icr_clkctrl = 0x4a008c28,
	.cm_c2c_sad2d_fw_clkctrl = 0x4a008c30,
	.cm_l4cfg_clkstctrl = 0x4a008d00,
	.cm_l4cfg_dynamicdep = 0x4a008d08,
	.cm_l4cfg_l4_cfg_clkctrl = 0x4a008d20,
	.cm_l4cfg_hw_sem_clkctrl = 0x4a008d28,
	.cm_l4cfg_mailbox_clkctrl = 0x4a008d30,
	.cm_l4cfg_sar_rom_clkctrl = 0x4a008d38,
	.cm_l3instr_clkstctrl = 0x4a008e00,
	.cm_l3instr_l3_3_clkctrl = 0x4a008e20,
	.cm_l3instr_l3_instr_clkctrl = 0x4a008e28,
	.cm_l3instr_intrconn_wp1_clkctrl = 0x4a008e40,
	.cm_l4per_clkstctrl = 0x4a009000,
	.cm_l4per_dynamicdep = 0x4a009008,
	.cm_l4per_adc_clkctrl = 0x4a009020,
	.cm_l4per_gptimer10_clkctrl = 0x4a009028,
	.cm_l4per_gptimer11_clkctrl = 0x4a009030,
	.cm_l4per_gptimer2_clkctrl = 0x4a009038,
	.cm_l4per_gptimer3_clkctrl = 0x4a009040,
	.cm_l4per_gptimer4_clkctrl = 0x4a009048,
	.cm_l4per_gptimer9_clkctrl = 0x4a009050,
	.cm_l4per_elm_clkctrl = 0x4a009058,
	.cm_l4per_gpio2_clkctrl = 0x4a009060,
	.cm_l4per_gpio3_clkctrl = 0x4a009068,
	.cm_l4per_gpio4_clkctrl = 0x4a009070,
	.cm_l4per_gpio5_clkctrl = 0x4a009078,
	.cm_l4per_gpio6_clkctrl = 0x4a009080,
	.cm_l4per_hdq1w_clkctrl = 0x4a009088,
	.cm_l4per_hecc1_clkctrl = 0x4a009090,
	.cm_l4per_hecc2_clkctrl = 0x4a009098,
	.cm_l4per_i2c1_clkctrl = 0x4a0090a0,
	.cm_l4per_i2c2_clkctrl = 0x4a0090a8,
	.cm_l4per_i2c3_clkctrl = 0x4a0090b0,
	.cm_l4per_i2c4_clkctrl = 0x4a0090b8,
	.cm_l4per_l4per_clkctrl = 0x4a0090c0,
	.cm_l4per_mcasp2_clkctrl = 0x4a0090d0,
	.cm_l4per_mcasp3_clkctrl = 0x4a0090d8,
	.cm_l4per_mgate_clkctrl = 0x4a0090e8,
	.cm_l4per_mcspi1_clkctrl = 0x4a0090f0,
	.cm_l4per_mcspi2_clkctrl = 0x4a0090f8,
	.cm_l4per_mcspi3_clkctrl = 0x4a009100,
	.cm_l4per_mcspi4_clkctrl = 0x4a009108,
	.cm_l4per_gpio7_clkctrl = 0x4a009110,
	.cm_l4per_gpio8_clkctrl = 0x4a009118,
	.cm_l4per_mmcsd3_clkctrl = 0x4a009120,
	.cm_l4per_mmcsd4_clkctrl = 0x4a009128,
	.cm_l4per_msprohg_clkctrl = 0x4a009130,
	.cm_l4per_slimbus2_clkctrl = 0x4a009138,
	.cm_l4per_uart1_clkctrl = 0x4a009140,
	.cm_l4per_uart2_clkctrl = 0x4a009148,
	.cm_l4per_uart3_clkctrl = 0x4a009150,
	.cm_l4per_uart4_clkctrl = 0x4a009158,
	.cm_l4per_mmcsd5_clkctrl = 0x4a009160,
	.cm_l4per_i2c5_clkctrl = 0x4a009168,
	.cm_l4per_uart5_clkctrl = 0x4a009170,
	.cm_l4per_uart6_clkctrl = 0x4a009178,
	.cm_l4sec_clkstctrl = 0x4a009180,
	.cm_l4sec_staticdep = 0x4a009184,
	.cm_l4sec_dynamicdep = 0x4a009188,
	.cm_l4sec_aes1_clkctrl = 0x4a0091a0,
	.cm_l4sec_aes2_clkctrl = 0x4a0091a8,
	.cm_l4sec_des3des_clkctrl = 0x4a0091b0,
	.cm_l4sec_pkaeip29_clkctrl = 0x4a0091b8,
	.cm_l4sec_rng_clkctrl = 0x4a0091c0,
	.cm_l4sec_sha2md51_clkctrl = 0x4a0091c8,
	.cm_l4sec_cryptodma_clkctrl = 0x4a0091d8,

	/* cm2.ivahd */
	.cm_ivahd_clkstctrl = 0x4a009200,
	.cm_ivahd_ivahd_clkctrl = 0x4a009220,
	.cm_ivahd_sl2_clkctrl = 0x4a009228,

	/* cm2.cam */
	.cm_cam_clkstctrl = 0x4a009300,
	.cm_cam_iss_clkctrl = 0x4a009320,
	.cm_cam_fdif_clkctrl = 0x4a009328,

	/* cm2.dss */
	.cm_dss_clkstctrl = 0x4a009400,
	.cm_dss_dss_clkctrl = 0x4a009420,

	/* cm2.sgx */
	.cm_sgx_clkstctrl = 0x4a009500,
	.cm_sgx_sgx_clkctrl = 0x4a009520,

	/* cm2.l3init */
	.cm_l3init_clkstctrl = 0x4a009600,

	/* cm2.l3init */
	.cm_l3init_hsmmc1_clkctrl = 0x4a009628,
	.cm_l3init_hsmmc2_clkctrl = 0x4a009630,
	.cm_l3init_hsi_clkctrl = 0x4a009638,
	.cm_l3init_hsusbhost_clkctrl = 0x4a009658,
	.cm_l3init_hsusbotg_clkctrl = 0x4a009660,
	.cm_l3init_hsusbtll_clkctrl = 0x4a009668,
	.cm_l3init_p1500_clkctrl = 0x4a009678,
	.cm_l3init_fsusb_clkctrl = 0x4a0096d0,
	.cm_l3init_ocp2scp1_clkctrl = 0x4a0096e0,

	/* l4 wkup regs */
	.cm_abe_pll_ref_clksel = 0x4ae0610c,
	.cm_sys_clksel = 0x4ae06110,
	.cm_wkup_clkstctrl = 0x4ae07900,
	.cm_wkup_l4wkup_clkctrl = 0x4ae07920,
	.cm_wkup_wdtimer1_clkctrl = 0x4ae07928,
	.cm_wkup_wdtimer2_clkctrl = 0x4ae07930,
	.cm_wkup_gpio1_clkctrl = 0x4ae07938,
	.cm_wkup_gptimer1_clkctrl = 0x4ae07940,
	.cm_wkup_gptimer12_clkctrl = 0x4ae07948,
	.cm_wkup_synctimer_clkctrl = 0x4ae07950,
	.cm_wkup_usim_clkctrl = 0x4ae07958,
	.cm_wkup_sarram_clkctrl = 0x4ae07960,
	.cm_wkup_keyboard_clkctrl = 0x4ae07978,
	.cm_wkup_rtc_clkctrl = 0x4ae07980,
	.cm_wkup_bandgap_clkctrl = 0x4ae07988,
	.cm_wkupaon_scrm_clkctrl = 0x4ae07990,
	.cm_wkupaon_io_srcomp_clkctrl = 0x4ae07998,
	.prm_rstctrl = 0x4ae07c00,
	.prm_rstst = 0x4ae07c04,
	.prm_vc_val_bypass = 0x4ae07ca0,
	.prm_vc_cfg_i2c_mode = 0x4ae07cb4,
	.prm_vc_cfg_i2c_clk = 0x4ae07cb8,

	.prm_sldo_core_setup = 0x4ae07cc4,
	.prm_sldo_core_ctrl = 0x4ae07cc8,
	.prm_sldo_mpu_setup = 0x4ae07ccc,
	.prm_sldo_mpu_ctrl = 0x4ae07cd0,
	.prm_sldo_mm_setup = 0x4ae07cd4,
	.prm_sldo_mm_ctrl = 0x4ae07cd8,
};

struct prcm_regs const dra7xx_prcm = {
	/* cm1.ckgen */
	.cm_clksel_core				= 0x4a005100,
	.cm_clksel_abe				= 0x4a005108,
	.cm_dll_ctrl				= 0x4a005110,
	.cm_clkmode_dpll_core			= 0x4a005120,
	.cm_idlest_dpll_core			= 0x4a005124,
	.cm_autoidle_dpll_core			= 0x4a005128,
	.cm_clksel_dpll_core			= 0x4a00512c,
	.cm_div_m2_dpll_core			= 0x4a005130,
	.cm_div_m3_dpll_core			= 0x4a005134,
	.cm_div_h11_dpll_core			= 0x4a005138,
	.cm_div_h12_dpll_core			= 0x4a00513c,
	.cm_div_h13_dpll_core			= 0x4a005140,
	.cm_div_h14_dpll_core			= 0x4a005144,
	.cm_ssc_deltamstep_dpll_core		= 0x4a005148,
	.cm_ssc_modfreqdiv_dpll_core		= 0x4a00514c,
	.cm_div_h21_dpll_core			= 0x4a005150,
	.cm_div_h22_dpllcore			= 0x4a005154,
	.cm_div_h23_dpll_core			= 0x4a005158,
	.cm_div_h24_dpll_core			= 0x4a00515c,
	.cm_clkmode_dpll_mpu			= 0x4a005160,
	.cm_idlest_dpll_mpu			= 0x4a005164,
	.cm_autoidle_dpll_mpu			= 0x4a005168,
	.cm_clksel_dpll_mpu			= 0x4a00516c,
	.cm_div_m2_dpll_mpu			= 0x4a005170,
	.cm_ssc_deltamstep_dpll_mpu		= 0x4a005188,
	.cm_ssc_modfreqdiv_dpll_mpu		= 0x4a00518c,
	.cm_bypclk_dpll_mpu			= 0x4a00519c,
	.cm_clkmode_dpll_iva			= 0x4a0051a0,
	.cm_idlest_dpll_iva			= 0x4a0051a4,
	.cm_autoidle_dpll_iva			= 0x4a0051a8,
	.cm_clksel_dpll_iva			= 0x4a0051ac,
	.cm_ssc_deltamstep_dpll_iva		= 0x4a0051c8,
	.cm_ssc_modfreqdiv_dpll_iva		= 0x4a0051cc,
	.cm_bypclk_dpll_iva			= 0x4a0051dc,
	.cm_clkmode_dpll_abe			= 0x4a0051e0,
	.cm_idlest_dpll_abe			= 0x4a0051e4,
	.cm_autoidle_dpll_abe			= 0x4a0051e8,
	.cm_clksel_dpll_abe			= 0x4a0051ec,
	.cm_div_m2_dpll_abe			= 0x4a0051f0,
	.cm_div_m3_dpll_abe			= 0x4a0051f4,
	.cm_ssc_deltamstep_dpll_abe		= 0x4a005208,
	.cm_ssc_modfreqdiv_dpll_abe		= 0x4a00520c,
	.cm_clkmode_dpll_ddrphy			= 0x4a005210,
	.cm_idlest_dpll_ddrphy			= 0x4a005214,
	.cm_autoidle_dpll_ddrphy		= 0x4a005218,
	.cm_clksel_dpll_ddrphy			= 0x4a00521c,
	.cm_div_m2_dpll_ddrphy			= 0x4a005220,
	.cm_div_h11_dpll_ddrphy			= 0x4a005228,
	.cm_ssc_deltamstep_dpll_ddrphy		= 0x4a00522c,
	.cm_clkmode_dpll_dsp			= 0x4a005234,
	.cm_shadow_freq_config1			= 0x4a005260,

	/* cm1.mpu */
	.cm_mpu_mpu_clkctrl			= 0x4a005320,

	/* cm1.dsp */
	.cm_dsp_clkstctrl			= 0x4a005400,
	.cm_dsp_dsp_clkctrl			= 0x4a005420,

	/* cm2.ckgen */
	.cm_clksel_usb_60mhz			= 0x4a008104,
	.cm_clkmode_dpll_per			= 0x4a008140,
	.cm_idlest_dpll_per			= 0x4a008144,
	.cm_autoidle_dpll_per			= 0x4a008148,
	.cm_clksel_dpll_per			= 0x4a00814c,
	.cm_div_m2_dpll_per			= 0x4a008150,
	.cm_div_m3_dpll_per			= 0x4a008154,
	.cm_div_h11_dpll_per			= 0x4a008158,
	.cm_div_h12_dpll_per			= 0x4a00815c,
	.cm_div_h13_dpll_per			= 0x4a008160,
	.cm_div_h14_dpll_per			= 0x4a008164,
	.cm_ssc_deltamstep_dpll_per		= 0x4a008168,
	.cm_ssc_modfreqdiv_dpll_per		= 0x4a00816c,
	.cm_clkmode_dpll_usb			= 0x4a008180,
	.cm_idlest_dpll_usb			= 0x4a008184,
	.cm_autoidle_dpll_usb			= 0x4a008188,
	.cm_clksel_dpll_usb			= 0x4a00818c,
	.cm_div_m2_dpll_usb			= 0x4a008190,
	.cm_ssc_deltamstep_dpll_usb		= 0x4a0081a8,
	.cm_ssc_modfreqdiv_dpll_usb		= 0x4a0081ac,
	.cm_clkdcoldo_dpll_usb			= 0x4a0081b4,
	.cm_clkmode_dpll_pcie_ref		= 0x4a008200,
	.cm_clkmode_apll_pcie			= 0x4a00821c,
	.cm_idlest_apll_pcie			= 0x4a008220,
	.cm_div_m2_apll_pcie			= 0x4a008224,
	.cm_clkvcoldo_apll_pcie			= 0x4a008228,

	/* cm2.core */
	.cm_l3_1_clkstctrl			= 0x4a008700,
	.cm_l3_1_dynamicdep			= 0x4a008708,
	.cm_l3_1_l3_1_clkctrl			= 0x4a008720,
	.cm_l3_gpmc_clkctrl			= 0x4a008728,
	.cm_mpu_m3_clkstctrl			= 0x4a008900,
	.cm_mpu_m3_staticdep			= 0x4a008904,
	.cm_mpu_m3_dynamicdep			= 0x4a008908,
	.cm_mpu_m3_mpu_m3_clkctrl		= 0x4a008920,
	.cm_sdma_clkstctrl			= 0x4a008a00,
	.cm_sdma_staticdep			= 0x4a008a04,
	.cm_sdma_dynamicdep			= 0x4a008a08,
	.cm_sdma_sdma_clkctrl			= 0x4a008a20,
	.cm_memif_clkstctrl			= 0x4a008b00,
	.cm_memif_dmm_clkctrl			= 0x4a008b20,
	.cm_memif_emif_fw_clkctrl		= 0x4a008b28,
	.cm_memif_emif_1_clkctrl		= 0x4a008b30,
	.cm_memif_emif_2_clkctrl		= 0x4a008b38,
	.cm_memif_dll_clkctrl			= 0x4a008b40,
	.cm_l4cfg_clkstctrl			= 0x4a008d00,
	.cm_l4cfg_dynamicdep			= 0x4a008d08,
	.cm_l4cfg_l4_cfg_clkctrl		= 0x4a008d20,
	.cm_l4cfg_hw_sem_clkctrl		= 0x4a008d28,
	.cm_l4cfg_mailbox_clkctrl		= 0x4a008d30,
	.cm_l4cfg_sar_rom_clkctrl		= 0x4a008d38,
	.cm_l3instr_clkstctrl			= 0x4a008e00,
	.cm_l3instr_l3_3_clkctrl		= 0x4a008e20,
	.cm_l3instr_l3_instr_clkctrl		= 0x4a008e28,
	.cm_l3instr_intrconn_wp1_clkctrl	= 0x4a008e40,

	/* cm2.ivahd */
	.cm_ivahd_clkstctrl			= 0x4a008f00,
	.cm_ivahd_ivahd_clkctrl			= 0x4a008f20,
	.cm_ivahd_sl2_clkctrl			= 0x4a008f28,

	/* cm2.cam */
	.cm_cam_clkstctrl			= 0x4a009000,
	.cm_cam_vip1_clkctrl			= 0x4a009020,
	.cm_cam_vip2_clkctrl			= 0x4a009028,
	.cm_cam_vip3_clkctrl			= 0x4a009030,
	.cm_cam_lvdsrx_clkctrl			= 0x4a009038,
	.cm_cam_csi1_clkctrl			= 0x4a009040,
	.cm_cam_csi2_clkctrl			= 0x4a009048,

	/* cm2.dss */
	.cm_dss_clkstctrl			= 0x4a009100,
	.cm_dss_dss_clkctrl			= 0x4a009120,

	/* cm2.sgx */
	.cm_sgx_clkstctrl			= 0x4a009200,
	.cm_sgx_sgx_clkctrl			= 0x4a009220,

	/* cm2.l3init */
	.cm_l3init_clkstctrl			= 0x4a009300,

	/* cm2.l3init */
	.cm_l3init_hsmmc1_clkctrl		= 0x4a009328,
	.cm_l3init_hsmmc2_clkctrl		= 0x4a009330,
	.cm_l3init_hsusbhost_clkctrl		= 0x4a009340,
	.cm_l3init_hsusbotg_clkctrl		= 0x4a009348,
	.cm_l3init_hsusbtll_clkctrl		= 0x4a009350,
	.cm_l3init_ocp2scp1_clkctrl		= 0x4a0093e0,

	/* cm2.l4per */
	.cm_l4per_clkstctrl			= 0x4a009700,
	.cm_l4per_dynamicdep			= 0x4a009708,
	.cm_l4per_gptimer10_clkctrl		= 0x4a009728,
	.cm_l4per_gptimer11_clkctrl		= 0x4a009730,
	.cm_l4per_gptimer2_clkctrl		= 0x4a009738,
	.cm_l4per_gptimer3_clkctrl		= 0x4a009740,
	.cm_l4per_gptimer4_clkctrl		= 0x4a009748,
	.cm_l4per_gptimer9_clkctrl		= 0x4a009750,
	.cm_l4per_elm_clkctrl			= 0x4a009758,
	.cm_l4per_gpio2_clkctrl			= 0x4a009760,
	.cm_l4per_gpio3_clkctrl			= 0x4a009768,
	.cm_l4per_gpio4_clkctrl			= 0x4a009770,
	.cm_l4per_gpio5_clkctrl			= 0x4a009778,
	.cm_l4per_gpio6_clkctrl			= 0x4a009780,
	.cm_l4per_hdq1w_clkctrl			= 0x4a009788,
	.cm_l4per_i2c1_clkctrl			= 0x4a0097a0,
	.cm_l4per_i2c2_clkctrl			= 0x4a0097a8,
	.cm_l4per_i2c3_clkctrl			= 0x4a0097b0,
	.cm_l4per_i2c4_clkctrl			= 0x4a0097b8,
	.cm_l4per_l4per_clkctrl			= 0x4a0097c0,
	.cm_l4per_mcspi1_clkctrl		= 0x4a0097f0,
	.cm_l4per_mcspi2_clkctrl		= 0x4a0097f8,
	.cm_l4per_mcspi3_clkctrl		= 0x4a009800,
	.cm_l4per_mcspi4_clkctrl		= 0x4a009808,
	.cm_l4per_gpio7_clkctrl			= 0x4a009810,
	.cm_l4per_gpio8_clkctrl			= 0x4a009818,
	.cm_l4per_mmcsd3_clkctrl		= 0x4a009820,
	.cm_l4per_mmcsd4_clkctrl		= 0x4a009828,
	.cm_l4per_uart1_clkctrl			= 0x4a009840,
	.cm_l4per_uart2_clkctrl			= 0x4a009848,
	.cm_l4per_uart3_clkctrl			= 0x4a009850,
	.cm_l4per_uart4_clkctrl			= 0x4a009858,
	.cm_l4per_uart5_clkctrl			= 0x4a009870,
	.cm_l4sec_clkstctrl			= 0x4a009880,
	.cm_l4sec_staticdep			= 0x4a009884,
	.cm_l4sec_dynamicdep			= 0x4a009888,
	.cm_l4sec_aes1_clkctrl			= 0x4a0098a0,
	.cm_l4sec_aes2_clkctrl			= 0x4a0098a8,
	.cm_l4sec_des3des_clkctrl		= 0x4a0098b0,
	.cm_l4sec_rng_clkctrl			= 0x4a0098c0,
	.cm_l4sec_sha2md51_clkctrl		= 0x4a0098c8,
	.cm_l4sec_cryptodma_clkctrl		= 0x4a0098d8,

	/* l4 wkup regs */
	.cm_abe_pll_ref_clksel			= 0x4ae0610c,
	.cm_sys_clksel				= 0x4ae06110,
	.cm_wkup_clkstctrl			= 0x4ae07800,
	.cm_wkup_l4wkup_clkctrl			= 0x4ae07820,
	.cm_wkup_wdtimer1_clkctrl		= 0x4ae07828,
	.cm_wkup_wdtimer2_clkctrl		= 0x4ae07830,
	.cm_wkup_gpio1_clkctrl			= 0x4ae07838,
	.cm_wkup_gptimer1_clkctrl		= 0x4ae07840,
	.cm_wkup_gptimer12_clkctrl		= 0x4ae07848,
	.cm_wkup_sarram_clkctrl			= 0x4ae07860,
	.cm_wkup_keyboard_clkctrl		= 0x4ae07878,
	.cm_wkupaon_scrm_clkctrl		= 0x4ae07890,
	.prm_rstctrl				= 0x4ae07d00,
	.prm_rstst				= 0x4ae07d04,
	.prm_vc_val_bypass			= 0x4ae07da0,
	.prm_vc_cfg_i2c_mode			= 0x4ae07db4,
	.prm_vc_cfg_i2c_clk			= 0x4ae07db8,
};

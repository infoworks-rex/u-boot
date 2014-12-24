/*
 * Display driver for Allwinner SoCs.
 *
 * (C) Copyright 2013-2014 Luc Verhaegen <libv@skynet.be>
 * (C) Copyright 2014 Hans de Goede <hdegoede@redhat.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>

#include <asm/arch/clock.h>
#include <asm/arch/display.h>
#include <asm/arch/gpio.h>
#include <asm/global_data.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <errno.h>
#include <fdtdec.h>
#include <fdt_support.h>
#include <video_fb.h>
#include "videomodes.h"

DECLARE_GLOBAL_DATA_PTR;

enum sunxi_monitor {
	sunxi_monitor_none,
	sunxi_monitor_dvi,
	sunxi_monitor_hdmi,
	sunxi_monitor_lcd,
	sunxi_monitor_vga,
};
#define SUNXI_MONITOR_LAST sunxi_monitor_vga

struct sunxi_display {
	GraphicDevice graphic_device;
	enum sunxi_monitor monitor;
	unsigned int depth;
} sunxi_display;

#ifdef CONFIG_VIDEO_HDMI

/*
 * Wait up to 200ms for value to be set in given part of reg.
 */
static int await_completion(u32 *reg, u32 mask, u32 val)
{
	unsigned long tmo = timer_get_us() + 200000;

	while ((readl(reg) & mask) != val) {
		if (timer_get_us() > tmo) {
			printf("DDC: timeout reading EDID\n");
			return -ETIME;
		}
	}
	return 0;
}

static int sunxi_hdmi_hpd_detect(void)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	struct sunxi_hdmi_reg * const hdmi =
		(struct sunxi_hdmi_reg *)SUNXI_HDMI_BASE;
	unsigned long tmo = timer_get_us() + 300000;

	/* Set pll3 to 300MHz */
	clock_set_pll3(300000000);

	/* Set hdmi parent to pll3 */
	clrsetbits_le32(&ccm->hdmi_clk_cfg, CCM_HDMI_CTRL_PLL_MASK,
			CCM_HDMI_CTRL_PLL3);

	/* Set ahb gating to pass */
#ifdef CONFIG_MACH_SUN6I
	setbits_le32(&ccm->ahb_reset1_cfg, 1 << AHB_RESET_OFFSET_HDMI);
#endif
	setbits_le32(&ccm->ahb_gate1, 1 << AHB_GATE_OFFSET_HDMI);

	/* Clock on */
	setbits_le32(&ccm->hdmi_clk_cfg, CCM_HDMI_CTRL_GATE);

	writel(SUNXI_HDMI_CTRL_ENABLE, &hdmi->ctrl);
	writel(SUNXI_HDMI_PAD_CTRL0_HDP, &hdmi->pad_ctrl0);

	while (timer_get_us() < tmo) {
		if (readl(&hdmi->hpd) & SUNXI_HDMI_HPD_DETECT)
			return 1;
	}

	return 0;
}

static void sunxi_hdmi_shutdown(void)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	struct sunxi_hdmi_reg * const hdmi =
		(struct sunxi_hdmi_reg *)SUNXI_HDMI_BASE;

	clrbits_le32(&hdmi->ctrl, SUNXI_HDMI_CTRL_ENABLE);
	clrbits_le32(&ccm->hdmi_clk_cfg, CCM_HDMI_CTRL_GATE);
	clrbits_le32(&ccm->ahb_gate1, 1 << AHB_GATE_OFFSET_HDMI);
#ifdef CONFIG_MACH_SUN6I
	clrbits_le32(&ccm->ahb_reset1_cfg, 1 << AHB_RESET_OFFSET_HDMI);
#endif
	clock_set_pll3(0);
}

static int sunxi_hdmi_ddc_do_command(u32 cmnd, int offset, int n)
{
	struct sunxi_hdmi_reg * const hdmi =
		(struct sunxi_hdmi_reg *)SUNXI_HDMI_BASE;

	setbits_le32(&hdmi->ddc_fifo_ctrl, SUNXI_HDMI_DDC_FIFO_CTRL_CLEAR);
	writel(SUNXI_HMDI_DDC_ADDR_EDDC_SEGMENT(offset >> 8) |
	       SUNXI_HMDI_DDC_ADDR_EDDC_ADDR |
	       SUNXI_HMDI_DDC_ADDR_OFFSET(offset) |
	       SUNXI_HMDI_DDC_ADDR_SLAVE_ADDR, &hdmi->ddc_addr);
#ifndef CONFIG_MACH_SUN6I
	writel(n, &hdmi->ddc_byte_count);
	writel(cmnd, &hdmi->ddc_cmnd);
#else
	writel(n << 16 | cmnd, &hdmi->ddc_cmnd);
#endif
	setbits_le32(&hdmi->ddc_ctrl, SUNXI_HMDI_DDC_CTRL_START);

	return await_completion(&hdmi->ddc_ctrl, SUNXI_HMDI_DDC_CTRL_START, 0);
}

static int sunxi_hdmi_ddc_read(int offset, u8 *buf, int count)
{
	struct sunxi_hdmi_reg * const hdmi =
		(struct sunxi_hdmi_reg *)SUNXI_HDMI_BASE;
	int i, n;

	while (count > 0) {
		if (count > 16)
			n = 16;
		else
			n = count;

		if (sunxi_hdmi_ddc_do_command(
				SUNXI_HDMI_DDC_CMND_EXPLICIT_EDDC_READ,
				offset, n))
			return -ETIME;

		for (i = 0; i < n; i++)
			*buf++ = readb(&hdmi->ddc_fifo_data);

		offset += n;
		count -= n;
	}

	return 0;
}

static int sunxi_hdmi_edid_get_block(int block, u8 *buf)
{
	int r, retries = 2;

	do {
		r = sunxi_hdmi_ddc_read(block * 128, buf, 128);
		if (r)
			continue;
		r = edid_check_checksum(buf);
		if (r) {
			printf("EDID block %d: checksum error%s\n",
			       block, retries ? ", retrying" : "");
		}
	} while (r && retries--);

	return r;
}

static int sunxi_hdmi_edid_get_mode(struct ctfb_res_modes *mode)
{
	struct edid1_info edid1;
	struct edid_cea861_info cea681[4];
	struct edid_detailed_timing *t =
		(struct edid_detailed_timing *)edid1.monitor_details.timing;
	struct sunxi_hdmi_reg * const hdmi =
		(struct sunxi_hdmi_reg *)SUNXI_HDMI_BASE;
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	int i, r, ext_blocks = 0;

	/* SUNXI_HDMI_CTRL_ENABLE & PAD_CTRL0 are already set by hpd_detect */
	writel(SUNXI_HDMI_PAD_CTRL1 | SUNXI_HDMI_PAD_CTRL1_HALVE,
	       &hdmi->pad_ctrl1);
	writel(SUNXI_HDMI_PLL_CTRL | SUNXI_HDMI_PLL_CTRL_DIV(15),
	       &hdmi->pll_ctrl);
	writel(SUNXI_HDMI_PLL_DBG0_PLL3, &hdmi->pll_dbg0);

	/* Reset i2c controller */
	setbits_le32(&ccm->hdmi_clk_cfg, CCM_HDMI_CTRL_DDC_GATE);
	writel(SUNXI_HMDI_DDC_CTRL_ENABLE |
	       SUNXI_HMDI_DDC_CTRL_SDA_ENABLE |
	       SUNXI_HMDI_DDC_CTRL_SCL_ENABLE |
	       SUNXI_HMDI_DDC_CTRL_RESET, &hdmi->ddc_ctrl);
	if (await_completion(&hdmi->ddc_ctrl, SUNXI_HMDI_DDC_CTRL_RESET, 0))
		return -EIO;

	writel(SUNXI_HDMI_DDC_CLOCK, &hdmi->ddc_clock);
#ifndef CONFIG_MACH_SUN6I
	writel(SUNXI_HMDI_DDC_LINE_CTRL_SDA_ENABLE |
	       SUNXI_HMDI_DDC_LINE_CTRL_SCL_ENABLE, &hdmi->ddc_line_ctrl);
#endif

	r = sunxi_hdmi_edid_get_block(0, (u8 *)&edid1);
	if (r == 0) {
		r = edid_check_info(&edid1);
		if (r) {
			printf("EDID: invalid EDID data\n");
			r = -EINVAL;
		}
	}
	if (r == 0) {
		ext_blocks = edid1.extension_flag;
		if (ext_blocks > 4)
			ext_blocks = 4;
		for (i = 0; i < ext_blocks; i++) {
			if (sunxi_hdmi_edid_get_block(1 + i,
						(u8 *)&cea681[i]) != 0) {
				ext_blocks = i;
				break;
			}
		}
	}

	/* Disable DDC engine, no longer needed */
	clrbits_le32(&hdmi->ddc_ctrl, SUNXI_HMDI_DDC_CTRL_ENABLE);
	clrbits_le32(&ccm->hdmi_clk_cfg, CCM_HDMI_CTRL_DDC_GATE);

	if (r)
		return r;

	/* We want version 1.3 or 1.2 with detailed timing info */
	if (edid1.version != 1 || (edid1.revision < 3 &&
			!EDID1_INFO_FEATURE_PREFERRED_TIMING_MODE(edid1))) {
		printf("EDID: unsupported version %d.%d\n",
		       edid1.version, edid1.revision);
		return -EINVAL;
	}

	/* Take the first usable detailed timing */
	for (i = 0; i < 4; i++, t++) {
		r = video_edid_dtd_to_ctfb_res_modes(t, mode);
		if (r == 0)
			break;
	}
	if (i == 4) {
		printf("EDID: no usable detailed timing found\n");
		return -ENOENT;
	}

	/* Check for basic audio support, if found enable hdmi output */
	sunxi_display.monitor = sunxi_monitor_dvi;
	for (i = 0; i < ext_blocks; i++) {
		if (cea681[i].extension_tag != EDID_CEA861_EXTENSION_TAG ||
		    cea681[i].revision < 2)
			continue;

		if (EDID_CEA861_SUPPORTS_BASIC_AUDIO(cea681[i]))
			sunxi_display.monitor = sunxi_monitor_hdmi;
	}

	return 0;
}

#endif /* CONFIG_VIDEO_HDMI */

/*
 * This is the entity that mixes and matches the different layers and inputs.
 * Allwinner calls it the back-end, but i like composer better.
 */
static void sunxi_composer_init(void)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	struct sunxi_de_be_reg * const de_be =
		(struct sunxi_de_be_reg *)SUNXI_DE_BE0_BASE;
	int i;

#if defined CONFIG_MACH_SUN6I || defined CONFIG_MACH_SUN8I
	/* Reset off */
	setbits_le32(&ccm->ahb_reset1_cfg, 1 << AHB_RESET_OFFSET_DE_BE0);
#endif

	/* Clocks on */
	setbits_le32(&ccm->ahb_gate1, 1 << AHB_GATE_OFFSET_DE_BE0);
	setbits_le32(&ccm->dram_clk_gate, 1 << CCM_DRAM_GATE_OFFSET_DE_BE0);
	clock_set_de_mod_clock(&ccm->be0_clk_cfg, 300000000);

	/* Engine bug, clear registers after reset */
	for (i = 0x0800; i < 0x1000; i += 4)
		writel(0, SUNXI_DE_BE0_BASE + i);

	setbits_le32(&de_be->mode, SUNXI_DE_BE_MODE_ENABLE);
}

static void sunxi_composer_mode_set(const struct ctfb_res_modes *mode,
				    unsigned int address)
{
	struct sunxi_de_be_reg * const de_be =
		(struct sunxi_de_be_reg *)SUNXI_DE_BE0_BASE;

	writel(SUNXI_DE_BE_HEIGHT(mode->yres) | SUNXI_DE_BE_WIDTH(mode->xres),
	       &de_be->disp_size);
	writel(SUNXI_DE_BE_HEIGHT(mode->yres) | SUNXI_DE_BE_WIDTH(mode->xres),
	       &de_be->layer0_size);
	writel(SUNXI_DE_BE_LAYER_STRIDE(mode->xres), &de_be->layer0_stride);
	writel(address << 3, &de_be->layer0_addr_low32b);
	writel(address >> 29, &de_be->layer0_addr_high4b);
	writel(SUNXI_DE_BE_LAYER_ATTR1_FMT_XRGB8888, &de_be->layer0_attr1_ctrl);

	setbits_le32(&de_be->mode, SUNXI_DE_BE_MODE_LAYER0_ENABLE);
}

static void sunxi_composer_enable(void)
{
	struct sunxi_de_be_reg * const de_be =
		(struct sunxi_de_be_reg *)SUNXI_DE_BE0_BASE;

	setbits_le32(&de_be->reg_ctrl, SUNXI_DE_BE_REG_CTRL_LOAD_REGS);
	setbits_le32(&de_be->mode, SUNXI_DE_BE_MODE_START);
}

/*
 * LCDC, what allwinner calls a CRTC, so timing controller and serializer.
 */
static void sunxi_lcdc_pll_set(int tcon, int dotclock,
			       int *clk_div, int *clk_double)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	int value, n, m, min_m, max_m, diff;
	int best_n = 0, best_m = 0, best_diff = 0x0FFFFFFF;
	int best_double = 0;

	if (tcon == 0) {
		min_m = 6;
		max_m = 127;
	} else {
		min_m = 1;
		max_m = 15;
	}

	/*
	 * Find the lowest divider resulting in a matching clock, if there
	 * is no match, pick the closest lower clock, as monitors tend to
	 * not sync to higher frequencies.
	 */
	for (m = min_m; m <= max_m; m++) {
		n = (m * dotclock) / 3000;

		if ((n >= 9) && (n <= 127)) {
			value = (3000 * n) / m;
			diff = dotclock - value;
			if (diff < best_diff) {
				best_diff = diff;
				best_m = m;
				best_n = n;
				best_double = 0;
			}
		}

		/* These are just duplicates */
		if (!(m & 1))
			continue;

		n = (m * dotclock) / 6000;
		if ((n >= 9) && (n <= 127)) {
			value = (6000 * n) / m;
			diff = dotclock - value;
			if (diff < best_diff) {
				best_diff = diff;
				best_m = m;
				best_n = n;
				best_double = 1;
			}
		}
	}

	debug("dotclock: %dkHz = %dkHz: (%d * 3MHz * %d) / %d\n",
	      dotclock, (best_double + 1) * 3000 * best_n / best_m,
	      best_double + 1, best_n, best_m);

	clock_set_pll3(best_n * 3000000);

	if (tcon == 0) {
		writel(CCM_LCD_CH0_CTRL_GATE | CCM_LCD_CH0_CTRL_RST |
		       (best_double ? CCM_LCD_CH0_CTRL_PLL3_2X :
				      CCM_LCD_CH0_CTRL_PLL3),
		       &ccm->lcd0_ch0_clk_cfg);
	} else {
		writel(CCM_LCD_CH1_CTRL_GATE |
		       (best_double ? CCM_LCD_CH1_CTRL_PLL3_2X :
				      CCM_LCD_CH1_CTRL_PLL3) |
		       CCM_LCD_CH1_CTRL_M(best_m), &ccm->lcd0_ch1_clk_cfg);
	}

	*clk_div = best_m;
	*clk_double = best_double;
}

static void sunxi_lcdc_init(void)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	struct sunxi_lcdc_reg * const lcdc =
		(struct sunxi_lcdc_reg *)SUNXI_LCD0_BASE;

	/* Reset off */
#if defined CONFIG_MACH_SUN6I || defined CONFIG_MACH_SUN8I
	setbits_le32(&ccm->ahb_reset1_cfg, 1 << AHB_RESET_OFFSET_LCD0);
#else
	setbits_le32(&ccm->lcd0_ch0_clk_cfg, CCM_LCD_CH0_CTRL_RST);
#endif

	/* Clock on */
	setbits_le32(&ccm->ahb_gate1, 1 << AHB_GATE_OFFSET_LCD0);

	/* Init lcdc */
	writel(0, &lcdc->ctrl); /* Disable tcon */
	writel(0, &lcdc->int0); /* Disable all interrupts */

	/* Disable tcon0 dot clock */
	clrbits_le32(&lcdc->tcon0_dclk, SUNXI_LCDC_TCON0_DCLK_ENABLE);

	/* Set all io lines to tristate */
	writel(0xffffffff, &lcdc->tcon0_io_tristate);
	writel(0xffffffff, &lcdc->tcon1_io_tristate);
}

static void sunxi_lcdc_enable(void)
{
	struct sunxi_lcdc_reg * const lcdc =
		(struct sunxi_lcdc_reg *)SUNXI_LCD0_BASE;

	setbits_le32(&lcdc->ctrl, SUNXI_LCDC_CTRL_TCON_ENABLE);
}

static void sunxi_lcdc_panel_enable(void)
{
	int pin;

	/*
	 * Start with backlight disabled to avoid the screen flashing to
	 * white while the lcd inits.
	 */
	pin = sunxi_name_to_gpio(CONFIG_VIDEO_LCD_BL_EN);
	if (pin != -1) {
		gpio_request(pin, "lcd_backlight_enable");
		gpio_direction_output(pin, 0);
	}

	pin = sunxi_name_to_gpio(CONFIG_VIDEO_LCD_BL_PWM);
	if (pin != -1) {
		gpio_request(pin, "lcd_backlight_pwm");
		/* backlight pwm is inverted, set to 1 to disable backlight */
		gpio_direction_output(pin, 1);
	}

	/* Give the backlight some time to turn off and power up the panel. */
	mdelay(40);
	pin = sunxi_name_to_gpio(CONFIG_VIDEO_LCD_POWER);
	if (pin != -1) {
		gpio_request(pin, "lcd_power");
		gpio_direction_output(pin, 1);
	}
}

static void sunxi_lcdc_backlight_enable(void)
{
	int pin;

	/*
	 * We want to have scanned out at least one frame before enabling the
	 * backlight to avoid the screen flashing to white when we enable it.
	 */
	mdelay(40);

	pin = sunxi_name_to_gpio(CONFIG_VIDEO_LCD_BL_EN);
	if (pin != -1)
		gpio_direction_output(pin, 1);

	pin = sunxi_name_to_gpio(CONFIG_VIDEO_LCD_BL_PWM);
	if (pin != -1) {
		/* backlight pwm is inverted, set to 0 to enable backlight */
		gpio_direction_output(pin, 0);
	}
}

static int sunxi_lcdc_get_clk_delay(const struct ctfb_res_modes *mode)
{
	int delay;

	delay = mode->lower_margin + mode->vsync_len + mode->upper_margin - 2;
	return (delay > 30) ? 30 : delay;
}

static void sunxi_lcdc_tcon0_mode_set(const struct ctfb_res_modes *mode)
{
	struct sunxi_lcdc_reg * const lcdc =
		(struct sunxi_lcdc_reg *)SUNXI_LCD0_BASE;
	int bp, clk_delay, clk_div, clk_double, pin, total, val;

	for (pin = SUNXI_GPD(0); pin <= SUNXI_GPD(27); pin++)
		sunxi_gpio_set_cfgpin(pin, SUNXI_GPD0_LCD0);

	sunxi_lcdc_pll_set(0, mode->pixclock_khz, &clk_div, &clk_double);

	/* Use tcon0 */
	clrsetbits_le32(&lcdc->ctrl, SUNXI_LCDC_CTRL_IO_MAP_MASK,
			SUNXI_LCDC_CTRL_IO_MAP_TCON0);

	clk_delay = sunxi_lcdc_get_clk_delay(mode);
	writel(SUNXI_LCDC_TCON0_CTRL_ENABLE |
	       SUNXI_LCDC_TCON0_CTRL_CLK_DELAY(clk_delay), &lcdc->tcon0_ctrl);

	writel(SUNXI_LCDC_TCON0_DCLK_ENABLE |
	       SUNXI_LCDC_TCON0_DCLK_DIV(clk_div), &lcdc->tcon0_dclk);

	writel(SUNXI_LCDC_X(mode->xres) | SUNXI_LCDC_Y(mode->yres),
	       &lcdc->tcon0_timing_active);

	bp = mode->hsync_len + mode->left_margin;
	total = mode->xres + mode->right_margin + bp;
	writel(SUNXI_LCDC_TCON0_TIMING_H_TOTAL(total) |
	       SUNXI_LCDC_TCON0_TIMING_H_BP(bp), &lcdc->tcon0_timing_h);

	bp = mode->vsync_len + mode->upper_margin;
	total = mode->yres + mode->lower_margin + bp;
	writel(SUNXI_LCDC_TCON0_TIMING_V_TOTAL(total) |
	       SUNXI_LCDC_TCON0_TIMING_V_BP(bp), &lcdc->tcon0_timing_v);

	writel(SUNXI_LCDC_X(mode->hsync_len) | SUNXI_LCDC_Y(mode->vsync_len),
	       &lcdc->tcon0_timing_sync);

	/* We only support hv-sync parallel lcd-s for now */
	writel(0, &lcdc->tcon0_hv_intf);
	writel(0, &lcdc->tcon0_cpu_intf);

	if (sunxi_display.depth == 18 || sunxi_display.depth == 16) {
		writel(SUNXI_LCDC_TCON0_FRM_SEED, &lcdc->tcon0_frm_seed[0]);
		writel(SUNXI_LCDC_TCON0_FRM_SEED, &lcdc->tcon0_frm_seed[1]);
		writel(SUNXI_LCDC_TCON0_FRM_SEED, &lcdc->tcon0_frm_seed[2]);
		writel(SUNXI_LCDC_TCON0_FRM_SEED, &lcdc->tcon0_frm_seed[3]);
		writel(SUNXI_LCDC_TCON0_FRM_SEED, &lcdc->tcon0_frm_seed[4]);
		writel(SUNXI_LCDC_TCON0_FRM_SEED, &lcdc->tcon0_frm_seed[5]);
		writel(SUNXI_LCDC_TCON0_FRM_TAB0, &lcdc->tcon0_frm_table[0]);
		writel(SUNXI_LCDC_TCON0_FRM_TAB1, &lcdc->tcon0_frm_table[1]);
		writel(SUNXI_LCDC_TCON0_FRM_TAB2, &lcdc->tcon0_frm_table[2]);
		writel(SUNXI_LCDC_TCON0_FRM_TAB3, &lcdc->tcon0_frm_table[3]);
		writel(((sunxi_display.depth == 18) ?
			SUNXI_LCDC_TCON0_FRM_CTRL_RGB666 :
			SUNXI_LCDC_TCON0_FRM_CTRL_RGB565),
		       &lcdc->tcon0_frm_ctrl);
	}

	val = 0;
	if (!(mode->sync & FB_SYNC_HOR_HIGH_ACT))
		val |= SUNXI_LCDC_TCON_HSYNC_MASK;
	if (!(mode->sync & FB_SYNC_VERT_HIGH_ACT))
		val |= SUNXI_LCDC_TCON_VSYNC_MASK;
	writel(val, &lcdc->tcon0_io_polarity);

	writel(0, &lcdc->tcon0_io_tristate);
}

#ifdef CONFIG_VIDEO_HDMI

static void sunxi_lcdc_tcon1_mode_set(const struct ctfb_res_modes *mode,
				      int *clk_div, int *clk_double)
{
	struct sunxi_lcdc_reg * const lcdc =
		(struct sunxi_lcdc_reg *)SUNXI_LCD0_BASE;
	int bp, clk_delay, total;

	/* Use tcon1 */
	clrsetbits_le32(&lcdc->ctrl, SUNXI_LCDC_CTRL_IO_MAP_MASK,
			SUNXI_LCDC_CTRL_IO_MAP_TCON1);

	clk_delay = sunxi_lcdc_get_clk_delay(mode);
	writel(SUNXI_LCDC_TCON1_CTRL_ENABLE |
	       SUNXI_LCDC_TCON1_CTRL_CLK_DELAY(clk_delay), &lcdc->tcon1_ctrl);

	writel(SUNXI_LCDC_X(mode->xres) | SUNXI_LCDC_Y(mode->yres),
	       &lcdc->tcon1_timing_source);
	writel(SUNXI_LCDC_X(mode->xres) | SUNXI_LCDC_Y(mode->yres),
	       &lcdc->tcon1_timing_scale);
	writel(SUNXI_LCDC_X(mode->xres) | SUNXI_LCDC_Y(mode->yres),
	       &lcdc->tcon1_timing_out);

	bp = mode->hsync_len + mode->left_margin;
	total = mode->xres + mode->right_margin + bp;
	writel(SUNXI_LCDC_TCON1_TIMING_H_TOTAL(total) |
	       SUNXI_LCDC_TCON1_TIMING_H_BP(bp), &lcdc->tcon1_timing_h);

	bp = mode->vsync_len + mode->upper_margin;
	total = mode->yres + mode->lower_margin + bp;
	writel(SUNXI_LCDC_TCON1_TIMING_V_TOTAL(total) |
	       SUNXI_LCDC_TCON1_TIMING_V_BP(bp), &lcdc->tcon1_timing_v);

	writel(SUNXI_LCDC_X(mode->hsync_len) | SUNXI_LCDC_Y(mode->vsync_len),
	       &lcdc->tcon1_timing_sync);

	sunxi_lcdc_pll_set(1, mode->pixclock_khz, clk_div, clk_double);
}

static void sunxi_hdmi_setup_info_frames(const struct ctfb_res_modes *mode)
{
	struct sunxi_hdmi_reg * const hdmi =
		(struct sunxi_hdmi_reg *)SUNXI_HDMI_BASE;
	u8 checksum = 0;
	u8 avi_info_frame[17] = {
		0x82, 0x02, 0x0d, 0x00, 0x12, 0x00, 0x88, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00
	};
	u8 vendor_info_frame[19] = {
		0x81, 0x01, 0x06, 0x29, 0x03, 0x0c, 0x00, 0x40,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00
	};
	int i;

	if (mode->pixclock_khz <= 27000)
		avi_info_frame[5] = 0x40; /* SD-modes, ITU601 colorspace */
	else
		avi_info_frame[5] = 0x80; /* HD-modes, ITU709 colorspace */

	if (mode->xres * 100 / mode->yres < 156)
		avi_info_frame[5] |= 0x18; /* 4 : 3 */
	else
		avi_info_frame[5] |= 0x28; /* 16 : 9 */

	for (i = 0; i < ARRAY_SIZE(avi_info_frame); i++)
		checksum += avi_info_frame[i];

	avi_info_frame[3] = 0x100 - checksum;

	for (i = 0; i < ARRAY_SIZE(avi_info_frame); i++)
		writeb(avi_info_frame[i], &hdmi->avi_info_frame[i]);

	writel(SUNXI_HDMI_QCP_PACKET0, &hdmi->qcp_packet0);
	writel(SUNXI_HDMI_QCP_PACKET1, &hdmi->qcp_packet1);

	for (i = 0; i < ARRAY_SIZE(vendor_info_frame); i++)
		writeb(vendor_info_frame[i], &hdmi->vendor_info_frame[i]);

	writel(SUNXI_HDMI_PKT_CTRL0, &hdmi->pkt_ctrl0);
	writel(SUNXI_HDMI_PKT_CTRL1, &hdmi->pkt_ctrl1);

	setbits_le32(&hdmi->video_ctrl, SUNXI_HDMI_VIDEO_CTRL_HDMI);
}

static void sunxi_hdmi_mode_set(const struct ctfb_res_modes *mode,
				int clk_div, int clk_double)
{
	struct sunxi_hdmi_reg * const hdmi =
		(struct sunxi_hdmi_reg *)SUNXI_HDMI_BASE;
	int x, y;

	/* Write clear interrupt status bits */
	writel(SUNXI_HDMI_IRQ_STATUS_BITS, &hdmi->irq);

	if (sunxi_display.monitor == sunxi_monitor_hdmi)
		sunxi_hdmi_setup_info_frames(mode);

	/* Set input sync enable */
	writel(SUNXI_HDMI_UNKNOWN_INPUT_SYNC, &hdmi->unknown);

	/* Init various registers, select pll3 as clock source */
	writel(SUNXI_HDMI_VIDEO_POL_TX_CLK, &hdmi->video_polarity);
	writel(SUNXI_HDMI_PAD_CTRL0_RUN, &hdmi->pad_ctrl0);
	writel(SUNXI_HDMI_PAD_CTRL1, &hdmi->pad_ctrl1);
	writel(SUNXI_HDMI_PLL_CTRL, &hdmi->pll_ctrl);
	writel(SUNXI_HDMI_PLL_DBG0_PLL3, &hdmi->pll_dbg0);

	/* Setup clk div and doubler */
	clrsetbits_le32(&hdmi->pll_ctrl, SUNXI_HDMI_PLL_CTRL_DIV_MASK,
			SUNXI_HDMI_PLL_CTRL_DIV(clk_div));
	if (!clk_double)
		setbits_le32(&hdmi->pad_ctrl1, SUNXI_HDMI_PAD_CTRL1_HALVE);

	/* Setup timing registers */
	writel(SUNXI_HDMI_Y(mode->yres) | SUNXI_HDMI_X(mode->xres),
	       &hdmi->video_size);

	x = mode->hsync_len + mode->left_margin;
	y = mode->vsync_len + mode->upper_margin;
	writel(SUNXI_HDMI_Y(y) | SUNXI_HDMI_X(x), &hdmi->video_bp);

	x = mode->right_margin;
	y = mode->lower_margin;
	writel(SUNXI_HDMI_Y(y) | SUNXI_HDMI_X(x), &hdmi->video_fp);

	x = mode->hsync_len;
	y = mode->vsync_len;
	writel(SUNXI_HDMI_Y(y) | SUNXI_HDMI_X(x), &hdmi->video_spw);

	if (mode->sync & FB_SYNC_HOR_HIGH_ACT)
		setbits_le32(&hdmi->video_polarity, SUNXI_HDMI_VIDEO_POL_HOR);

	if (mode->sync & FB_SYNC_VERT_HIGH_ACT)
		setbits_le32(&hdmi->video_polarity, SUNXI_HDMI_VIDEO_POL_VER);
}

static void sunxi_hdmi_enable(void)
{
	struct sunxi_hdmi_reg * const hdmi =
		(struct sunxi_hdmi_reg *)SUNXI_HDMI_BASE;

	udelay(100);
	setbits_le32(&hdmi->video_ctrl, SUNXI_HDMI_VIDEO_CTRL_ENABLE);
}

#endif /* CONFIG_VIDEO_HDMI */

static void sunxi_drc_init(void)
{
#if defined CONFIG_MACH_SUN6I || defined CONFIG_MACH_SUN8I
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* On sun6i the drc must be clocked even when in pass-through mode */
	setbits_le32(&ccm->ahb_reset1_cfg, 1 << AHB_RESET_OFFSET_DRC0);
	clock_set_de_mod_clock(&ccm->iep_drc0_clk_cfg, 300000000);
#endif
}

static void sunxi_engines_init(void)
{
	sunxi_composer_init();
	sunxi_lcdc_init();
	sunxi_drc_init();
}

static void sunxi_mode_set(const struct ctfb_res_modes *mode,
			   unsigned int address)
{
	switch (sunxi_display.monitor) {
	case sunxi_monitor_none:
		break;
	case sunxi_monitor_dvi:
	case sunxi_monitor_hdmi: {
#ifdef CONFIG_VIDEO_HDMI
		int clk_div, clk_double;
		sunxi_composer_mode_set(mode, address);
		sunxi_lcdc_tcon1_mode_set(mode, &clk_div, &clk_double);
		sunxi_hdmi_mode_set(mode, clk_div, clk_double);
		sunxi_composer_enable();
		sunxi_lcdc_enable();
		sunxi_hdmi_enable();
#endif
		}
		break;
	case sunxi_monitor_lcd:
		sunxi_lcdc_panel_enable();
		sunxi_composer_mode_set(mode, address);
		sunxi_lcdc_tcon0_mode_set(mode);
		sunxi_composer_enable();
		sunxi_lcdc_enable();
		sunxi_lcdc_backlight_enable();
		break;
	case sunxi_monitor_vga:
#ifdef CONFIG_VIDEO_VGA_VIA_LCD
		sunxi_composer_mode_set(mode, address);
		sunxi_lcdc_tcon0_mode_set(mode);
		sunxi_composer_enable();
		sunxi_lcdc_enable();
#endif
		break;
	}
}

static const char *sunxi_get_mon_desc(enum sunxi_monitor monitor)
{
	switch (monitor) {
	case sunxi_monitor_none:	return "none";
	case sunxi_monitor_dvi:		return "dvi";
	case sunxi_monitor_hdmi:	return "hdmi";
	case sunxi_monitor_lcd:		return "lcd";
	case sunxi_monitor_vga:		return "vga";
	}
	return NULL; /* never reached */
}

void *video_hw_init(void)
{
	static GraphicDevice *graphic_device = &sunxi_display.graphic_device;
	const struct ctfb_res_modes *mode;
	struct ctfb_res_modes custom;
	const char *options;
#ifdef CONFIG_VIDEO_HDMI
	int ret, hpd, edid;
#endif
	char mon[16];
	char *lcd_mode = CONFIG_VIDEO_LCD_MODE;
	int i;

	memset(&sunxi_display, 0, sizeof(struct sunxi_display));

	printf("Reserved %dkB of RAM for Framebuffer.\n",
	       CONFIG_SUNXI_FB_SIZE >> 10);
	gd->fb_base = gd->ram_top;

	video_get_ctfb_res_modes(RES_MODE_1024x768, 24, &mode,
				 &sunxi_display.depth, &options);
#ifdef CONFIG_VIDEO_HDMI
	hpd = video_get_option_int(options, "hpd", 1);
	edid = video_get_option_int(options, "edid", 1);
	sunxi_display.monitor = sunxi_monitor_dvi;
#elif defined CONFIG_VIDEO_VGA_VIA_LCD
	sunxi_display.monitor = sunxi_monitor_vga;
#else
	sunxi_display.monitor = sunxi_monitor_lcd;
#endif
	video_get_option_string(options, "monitor", mon, sizeof(mon),
				sunxi_get_mon_desc(sunxi_display.monitor));
	for (i = 0; i <= SUNXI_MONITOR_LAST; i++) {
		if (strcmp(mon, sunxi_get_mon_desc(i)) == 0) {
			sunxi_display.monitor = i;
			break;
		}
	}
	if (i > SUNXI_MONITOR_LAST)
		printf("Unknown monitor: '%s', falling back to '%s'\n",
		       mon, sunxi_get_mon_desc(sunxi_display.monitor));

	switch (sunxi_display.monitor) {
	case sunxi_monitor_none:
		return NULL;
	case sunxi_monitor_dvi:
	case sunxi_monitor_hdmi:
#ifndef CONFIG_VIDEO_HDMI
		printf("HDMI/DVI not supported on this board\n");
		sunxi_display.monitor = sunxi_monitor_none;
		return NULL;
#else
		/* Always call hdp_detect, as it also enables clocks, etc. */
		ret = sunxi_hdmi_hpd_detect();
		if (ret) {
			printf("HDMI connected: ");
			if (edid && sunxi_hdmi_edid_get_mode(&custom) == 0)
				mode = &custom;
			break;
		}
		if (!hpd)
			break; /* User has requested to ignore hpd */

		sunxi_hdmi_shutdown();

		if (lcd_mode[0] == 0) {
			sunxi_display.monitor = sunxi_monitor_none;
			return NULL; /* No LCD, bail */
		}

		/* Fall back / through to LCD */
		sunxi_display.monitor = sunxi_monitor_lcd;
#endif
	case sunxi_monitor_lcd:
		if (lcd_mode[0]) {
			sunxi_display.depth = video_get_params(&custom, lcd_mode);
			mode = &custom;
			break;
		}
		printf("LCD not supported on this board\n");
		sunxi_display.monitor = sunxi_monitor_none;
		return NULL;
	case sunxi_monitor_vga:
#ifdef CONFIG_VIDEO_VGA_VIA_LCD
		sunxi_display.depth = 18;
		break;
#else
		printf("VGA not supported on this board\n");
		sunxi_display.monitor = sunxi_monitor_none;
		return NULL;
#endif
	}

	if (mode->vmode != FB_VMODE_NONINTERLACED) {
		printf("Only non-interlaced modes supported, falling back to 1024x768\n");
		mode = &res_mode_init[RES_MODE_1024x768];
	} else {
		printf("Setting up a %dx%d %s console\n", mode->xres,
		       mode->yres, sunxi_get_mon_desc(sunxi_display.monitor));
	}

	sunxi_engines_init();
	sunxi_mode_set(mode, gd->fb_base - CONFIG_SYS_SDRAM_BASE);

	/*
	 * These are the only members of this structure that are used. All the
	 * others are driver specific. There is nothing to decribe pitch or
	 * stride, but we are lucky with our hw.
	 */
	graphic_device->frameAdrs = gd->fb_base;
	graphic_device->gdfIndex = GDF_32BIT_X888RGB;
	graphic_device->gdfBytesPP = 4;
	graphic_device->winSizeX = mode->xres;
	graphic_device->winSizeY = mode->yres;

	return graphic_device;
}

/*
 * Simplefb support.
 */
#if defined(CONFIG_OF_BOARD_SETUP) && defined(CONFIG_VIDEO_DT_SIMPLEFB)
int sunxi_simplefb_setup(void *blob)
{
	static GraphicDevice *graphic_device = &sunxi_display.graphic_device;
	int offset, ret;
	const char *pipeline = NULL;

	switch (sunxi_display.monitor) {
	case sunxi_monitor_none:
		return 0;
	case sunxi_monitor_dvi:
	case sunxi_monitor_hdmi:
		pipeline = "de_be0-lcd0-hdmi";
		break;
	case sunxi_monitor_lcd:
		pipeline = "de_be0-lcd0";
		break;
	case sunxi_monitor_vga:
		pipeline = "de_be0-lcd0";
		break;
	}

	/* Find a prefilled simpefb node, matching out pipeline config */
	offset = fdt_node_offset_by_compatible(blob, -1,
					       "allwinner,simple-framebuffer");
	while (offset >= 0) {
		ret = fdt_find_string(blob, offset, "allwinner,pipeline",
				      pipeline);
		if (ret == 0)
			break;
		offset = fdt_node_offset_by_compatible(blob, offset,
					       "allwinner,simple-framebuffer");
	}
	if (offset < 0) {
		eprintf("Cannot setup simplefb: node not found\n");
		return 0; /* Keep older kernels working */
	}

	ret = fdt_setup_simplefb_node(blob, offset, gd->fb_base,
			graphic_device->winSizeX, graphic_device->winSizeY,
			graphic_device->winSizeX * graphic_device->gdfBytesPP,
			"x8r8g8b8");
	if (ret)
		eprintf("Cannot setup simplefb: Error setting properties\n");

	return ret;
}
#endif /* CONFIG_OF_BOARD_SETUP && CONFIG_VIDEO_DT_SIMPLEFB */

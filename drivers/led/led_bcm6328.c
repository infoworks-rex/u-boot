/*
 * Copyright (C) 2017 Álvaro Fernández Rojas <noltari@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <led.h>
#include <asm/io.h>
#include <dm/lists.h>

#define LEDS_MAX			24

/* LED Init register */
#define LED_INIT_REG			0x00
#define LED_INIT_FASTINTV_MS		20
#define LED_INIT_FASTINTV_SHIFT		6
#define LED_INIT_FASTINTV_MASK		(0x3f << LED_INIT_FASTINTV_SHIFT)
#define LED_INIT_SLEDEN_SHIFT		12
#define LED_INIT_SLEDEN_MASK		(1 << LED_INIT_SLEDEN_SHIFT)
#define LED_INIT_SLEDMUX_SHIFT		13
#define LED_INIT_SLEDMUX_MASK		(1 << LED_INIT_SLEDMUX_SHIFT)
#define LED_INIT_SLEDCLKNPOL_SHIFT	14
#define LED_INIT_SLEDCLKNPOL_MASK	(1 << LED_INIT_SLEDCLKNPOL_SHIFT)
#define LED_INIT_SLEDDATAPPOL_SHIFT	15
#define LED_INIT_SLEDDATANPOL_MASK	(1 << LED_INIT_SLEDDATAPPOL_SHIFT)
#define LED_INIT_SLEDSHIFTDIR_SHIFT	16
#define LED_INIT_SLEDSHIFTDIR_MASK	(1 << LED_INIT_SLEDSHIFTDIR_SHIFT)

/* LED Mode registers */
#define LED_MODE_REG_HI			0x04
#define LED_MODE_REG_LO			0x08
#define LED_MODE_ON			0
#define LED_MODE_FAST			1
#define LED_MODE_BLINK			2
#define LED_MODE_OFF			3
#define LED_MODE_MASK			0x3

DECLARE_GLOBAL_DATA_PTR;

struct bcm6328_led_priv {
	void __iomem *regs;
	void __iomem *mode;
	uint8_t shift;
	bool active_low;
};

static unsigned long bcm6328_led_get_mode(struct bcm6328_led_priv *priv)
{
	return ((readl_be(priv->mode) >> priv->shift) & LED_MODE_MASK);
}

static int bcm6328_led_set_mode(struct bcm6328_led_priv *priv, uint8_t mode)
{
	clrsetbits_be32(priv->mode, (LED_MODE_MASK << priv->shift),
			(mode << priv->shift));

	return 0;
}

static enum led_state_t bcm6328_led_get_state(struct udevice *dev)
{
	struct bcm6328_led_priv *priv = dev_get_priv(dev);
	enum led_state_t state = LEDST_OFF;

	switch (bcm6328_led_get_mode(priv)) {
#ifdef CONFIG_LED_BLINK
	case LED_MODE_BLINK:
	case LED_MODE_FAST:
		state = LEDST_BLINK;
		break;
#endif
	case LED_MODE_OFF:
		state = (priv->active_low ? LEDST_ON : LEDST_OFF);
		break;
	case LED_MODE_ON:
		state = (priv->active_low ? LEDST_OFF : LEDST_ON);
		break;
	}

	return state;
}

static int bcm6328_led_set_state(struct udevice *dev, enum led_state_t state)
{
	struct bcm6328_led_priv *priv = dev_get_priv(dev);
	unsigned long mode;

	switch (state) {
#ifdef CONFIG_LED_BLINK
	case LEDST_BLINK:
		mode = LED_MODE_BLINK;
		break;
#endif
	case LEDST_OFF:
		mode = (priv->active_low ? LED_MODE_ON : LED_MODE_OFF);
		break;
	case LEDST_ON:
		mode = (priv->active_low ? LED_MODE_OFF : LED_MODE_ON);
		break;
	case LEDST_TOGGLE:
		if (bcm6328_led_get_state(dev) == LEDST_OFF)
			return bcm6328_led_set_state(dev, LEDST_ON);
		else
			return bcm6328_led_set_state(dev, LEDST_OFF);
		break;
	default:
		return -ENOSYS;
	}

	return bcm6328_led_set_mode(priv, mode);
}

#ifdef CONFIG_LED_BLINK
static unsigned long bcm6328_blink_delay(int delay)
{
	unsigned long bcm6328_delay = delay;

	bcm6328_delay += (LED_INIT_FASTINTV_MS / 2);
	bcm6328_delay /= LED_INIT_FASTINTV_MS;
	bcm6328_delay <<= LED_INIT_FASTINTV_SHIFT;

	if (bcm6328_delay > LED_INIT_FASTINTV_MASK)
		return LED_INIT_FASTINTV_MASK;
	else
		return bcm6328_delay;
}

static int bcm6328_led_set_period(struct udevice *dev, int period_ms)
{
	struct bcm6328_led_priv *priv = dev_get_priv(dev);

	clrsetbits_be32(priv->regs + LED_INIT_REG, LED_INIT_FASTINTV_MASK,
			bcm6328_blink_delay(period_ms));

	return 0;
}
#endif

static const struct led_ops bcm6328_led_ops = {
	.get_state = bcm6328_led_get_state,
	.set_state = bcm6328_led_set_state,
#ifdef CONFIG_LED_BLINK
	.set_period = bcm6328_led_set_period,
#endif
};

static int bcm6328_led_probe(struct udevice *dev)
{
	struct led_uc_plat *uc_plat = dev_get_uclass_platdata(dev);
	fdt_addr_t addr;
	fdt_size_t size;

	/* Top-level LED node */
	if (!uc_plat->label) {
		void __iomem *regs;
		u32 set_bits = 0;

		addr = dev_get_addr_size_index(dev, 0, &size);
		if (addr == FDT_ADDR_T_NONE)
			return -EINVAL;

		regs = ioremap(addr, size);

		if (fdtdec_get_bool(gd->fdt_blob, dev_of_offset(dev),
				    "brcm,serial-leds"))
			set_bits |= LED_INIT_SLEDEN_MASK;
		if (fdtdec_get_bool(gd->fdt_blob, dev_of_offset(dev),
				    "brcm,serial-mux"))
			set_bits |= LED_INIT_SLEDMUX_MASK;
		if (fdtdec_get_bool(gd->fdt_blob, dev_of_offset(dev),
				    "brcm,serial-clk-low"))
			set_bits |= LED_INIT_SLEDCLKNPOL_MASK;
		if (!fdtdec_get_bool(gd->fdt_blob, dev_of_offset(dev),
				     "brcm,serial-dat-low"))
			set_bits |= LED_INIT_SLEDDATANPOL_MASK;
		if (!fdtdec_get_bool(gd->fdt_blob, dev_of_offset(dev),
				     "brcm,serial-shift-inv"))
			set_bits |= LED_INIT_SLEDSHIFTDIR_MASK;

		clrsetbits_be32(regs + LED_INIT_REG, ~0, set_bits);
	} else {
		struct bcm6328_led_priv *priv = dev_get_priv(dev);
		unsigned int pin;

		addr = dev_get_addr_size_index(dev_get_parent(dev), 0, &size);
		if (addr == FDT_ADDR_T_NONE)
			return -EINVAL;

		pin = fdtdec_get_uint(gd->fdt_blob, dev_of_offset(dev), "reg",
				      LEDS_MAX);
		if (pin >= LEDS_MAX)
			return -EINVAL;

		priv->regs = ioremap(addr, size);
		if (pin < 8) {
			/* LEDs 0-7 (bits 47:32) */
			priv->mode = priv->regs + LED_MODE_REG_HI;
			priv->shift = (pin << 1);
		} else {
			/* LEDs 8-23 (bits 31:0) */
			priv->mode = priv->regs + LED_MODE_REG_LO;
			priv->shift = ((pin - 8) << 1);
		}

		if (fdtdec_get_bool(gd->fdt_blob, dev_of_offset(dev),
				    "active-low"))
			priv->active_low = true;
	}

	return 0;
}

static int bcm6328_led_bind(struct udevice *parent)
{
	const void *blob = gd->fdt_blob;
	int node;

	for (node = fdt_first_subnode(blob, dev_of_offset(parent));
	     node > 0;
	     node = fdt_next_subnode(blob, node)) {
		struct led_uc_plat *uc_plat;
		struct udevice *dev;
		const char *label;
		int ret;

		label = fdt_getprop(blob, node, "label", NULL);
		if (!label) {
			debug("%s: node %s has no label\n", __func__,
			      fdt_get_name(blob, node, NULL));
			return -EINVAL;
		}

		ret = device_bind_driver_to_node(parent, "bcm6328-led",
						 fdt_get_name(blob, node, NULL),
						 node, &dev);
		if (ret)
			return ret;

		uc_plat = dev_get_uclass_platdata(dev);
		uc_plat->label = label;
	}

	return 0;
}

static const struct udevice_id bcm6328_led_ids[] = {
	{ .compatible = "brcm,bcm6328-leds" },
	{ /* sentinel */ }
};

U_BOOT_DRIVER(bcm6328_led) = {
	.name = "bcm6328-led",
	.id = UCLASS_LED,
	.of_match = bcm6328_led_ids,
	.ops = &bcm6328_led_ops,
	.bind = bcm6328_led_bind,
	.probe = bcm6328_led_probe,
	.priv_auto_alloc_size = sizeof(struct bcm6328_led_priv),
};

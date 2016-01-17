/*
 * Copyright (C) 2014 Google, Inc
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <dm.h>
#include <errno.h>
#include <fdtdec.h>
#include <malloc.h>
#include <pch.h>
#include <asm/lapic.h>
#include <asm/pci.h>
#include <asm/arch/bd82x6x.h>
#include <asm/arch/model_206ax.h>
#include <asm/arch/pch.h>
#include <asm/arch/sandybridge.h>

#define BIOS_CTRL	0xdc

void bd82x6x_pci_init(pci_dev_t dev)
{
	u16 reg16;
	u8 reg8;

	debug("bd82x6x PCI init.\n");
	/* Enable Bus Master */
	reg16 = x86_pci_read_config16(dev, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MASTER;
	x86_pci_write_config16(dev, PCI_COMMAND, reg16);

	/* This device has no interrupt */
	x86_pci_write_config8(dev, INTR, 0xff);

	/* disable parity error response and SERR */
	reg16 = x86_pci_read_config16(dev, BCTRL);
	reg16 &= ~(1 << 0);
	reg16 &= ~(1 << 1);
	x86_pci_write_config16(dev, BCTRL, reg16);

	/* Master Latency Count must be set to 0x04! */
	reg8 = x86_pci_read_config8(dev, SMLT);
	reg8 &= 0x07;
	reg8 |= (0x04 << 3);
	x86_pci_write_config8(dev, SMLT, reg8);

	/* Will this improve throughput of bus masters? */
	x86_pci_write_config8(dev, PCI_MIN_GNT, 0x06);

	/* Clear errors in status registers */
	reg16 = x86_pci_read_config16(dev, PSTS);
	/* reg16 |= 0xf900; */
	x86_pci_write_config16(dev, PSTS, reg16);

	reg16 = x86_pci_read_config16(dev, SECSTS);
	/* reg16 |= 0xf900; */
	x86_pci_write_config16(dev, SECSTS, reg16);
}

static int bd82x6x_probe(struct udevice *dev)
{
	const void *blob = gd->fdt_blob;
	struct pci_controller *hose;
	int sata_node, gma_node;
	int ret;

	if (!(gd->flags & GD_FLG_RELOC))
		return 0;

	hose = pci_bus_to_hose(0);
	lpc_enable(PCH_LPC_DEV);
	lpc_init_extra(hose, PCH_LPC_DEV);
	sata_node = fdtdec_next_compatible(blob, 0,
					   COMPAT_INTEL_PANTHERPOINT_AHCI);
	if (sata_node < 0) {
		debug("%s: Cannot find SATA node\n", __func__);
		return -EINVAL;
	}
	bd82x6x_sata_init(PCH_SATA_DEV, blob, sata_node);
	bd82x6x_usb_ehci_init(PCH_EHCI1_DEV);
	bd82x6x_usb_ehci_init(PCH_EHCI2_DEV);

	gma_node = fdtdec_next_compatible(blob, 0, COMPAT_INTEL_GMA);
	if (gma_node < 0) {
		debug("%s: Cannot find GMA node\n", __func__);
		return -EINVAL;
	}
	ret = dm_pci_bus_find_bdf(PCH_VIDEO_DEV, &dev);
	if (ret)
		return ret;
	ret = gma_func0_init(dev, blob, gma_node);
	if (ret)
		return ret;

	return 0;
}

/* TODO(sjg@chromium.org): Move this to the PCH init() method */
int bd82x6x_init_extra(void)
{
	const void *blob = gd->fdt_blob;
	int sata_node;

	sata_node = fdtdec_next_compatible(blob, 0,
					   COMPAT_INTEL_PANTHERPOINT_AHCI);
	if (sata_node < 0) {
		debug("%s: Cannot find SATA node\n", __func__);
		return -EINVAL;
	}

	bd82x6x_pci_init(PCH_DEV);
	bd82x6x_sata_enable(PCH_SATA_DEV, blob, sata_node);
	northbridge_enable(PCH_DEV);
	northbridge_init(PCH_DEV);

	return 0;
}

static int bd82x6x_pch_get_sbase(struct udevice *dev, ulong *sbasep)
{
	u32 rcba;

	dm_pci_read_config32(dev, PCH_RCBA, &rcba);
	/* Bits 31-14 are the base address, 13-1 are reserved, 0 is enable */
	rcba = rcba & 0xffffc000;
	*sbasep = rcba + 0x3800;

	return 0;
}

static enum pch_version bd82x6x_pch_get_version(struct udevice *dev)
{
	return PCHV_9;
}

static int bd82x6x_set_spi_protect(struct udevice *dev, bool protect)
{
	uint8_t bios_cntl;

	/* Adjust the BIOS write protect and SMM BIOS Write Protect Disable */
	dm_pci_read_config8(dev, BIOS_CTRL, &bios_cntl);
	if (protect) {
		bios_cntl &= ~BIOS_CTRL_BIOSWE;
		bios_cntl |= BIT(5);
	} else {
		bios_cntl |= BIOS_CTRL_BIOSWE;
		bios_cntl &= ~BIT(5);
	}
	dm_pci_write_config8(dev, BIOS_CTRL, bios_cntl);

	return 0;
}

static const struct pch_ops bd82x6x_pch_ops = {
	.get_sbase	= bd82x6x_pch_get_sbase,
	.get_version	= bd82x6x_pch_get_version,
	.set_spi_protect = bd82x6x_set_spi_protect,
};

static const struct udevice_id bd82x6x_ids[] = {
	{ .compatible = "intel,bd82x6x" },
	{ }
};

U_BOOT_DRIVER(bd82x6x_drv) = {
	.name		= "bd82x6x",
	.id		= UCLASS_PCH,
	.of_match	= bd82x6x_ids,
	.probe		= bd82x6x_probe,
	.ops		= &bd82x6x_pch_ops,
};

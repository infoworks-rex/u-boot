/*
 * Copyright (C) 2015, Bin Meng <bmeng.cn@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <pci.h>
#include <pci_rom.h>
#include <asm/pci.h>
#include <asm/arch/qemu.h>

DECLARE_GLOBAL_DATA_PTR;

void board_pci_setup_hose(struct pci_controller *hose)
{
	hose->first_busno = 0;
	hose->last_busno = 0;

	/* PCI memory space */
	pci_set_region(hose->regions + 0,
		       CONFIG_PCI_MEM_BUS,
		       CONFIG_PCI_MEM_PHYS,
		       CONFIG_PCI_MEM_SIZE,
		       PCI_REGION_MEM);

	/* PCI IO space */
	pci_set_region(hose->regions + 1,
		       CONFIG_PCI_IO_BUS,
		       CONFIG_PCI_IO_PHYS,
		       CONFIG_PCI_IO_SIZE,
		       PCI_REGION_IO);

	pci_set_region(hose->regions + 2,
		       CONFIG_PCI_PREF_BUS,
		       CONFIG_PCI_PREF_PHYS,
		       CONFIG_PCI_PREF_SIZE,
		       PCI_REGION_PREFETCH);

	pci_set_region(hose->regions + 3,
		       0,
		       0,
		       gd->ram_size,
		       PCI_REGION_MEM | PCI_REGION_SYS_MEMORY);

	hose->region_count = 4;
}

int board_pci_post_scan(struct pci_controller *hose)
{
	int ret = 0;
	ulong start;
	pci_dev_t bdf;
	struct pci_device_id graphic_card[] = { { 0x1234, 0x1111 } };
	u16 device;
	int pam, i;

	/*
	 * QEMU emulated graphic card shows in the PCI configuration space with
	 * PCI vendor id and device id as an artificial pair 0x1234:0x1111.
	 * It is on PCI bus 0, function 0, but device number is not consistent
	 * for the two x86 targets it supports. For i440FX and PIIX chipset
	 * board, it shows as device 2, while for Q35 and ICH9 chipset board,
	 * it shows as device 1. Here we locate its bdf at run-time based on
	 * its vendor id and device id pair so we can support both boards.
	 */
	bdf = pci_find_devices(graphic_card, 0);
	if (bdf != -1) {
		start = get_timer(0);
		ret = pci_run_vga_bios(bdf, NULL, PCI_ROM_USE_NATIVE);
		debug("BIOS ran in %lums\n", get_timer(start));
	}

	/*
	 * i440FX and Q35 chipset have different PAM register offset, but with
	 * the same bitfield layout. Here we determine the offset based on its
	 * PCI device ID.
	 */
	device = x86_pci_read_config16(PCI_BDF(0, 0, 0), PCI_DEVICE_ID);
	pam = (device == PCI_DEVICE_ID_INTEL_82441) ? I440FX_PAM : Q35_PAM;

	/*
	 * Initialize Programmable Attribute Map (PAM) Registers
	 *
	 * Configure legacy segments C/D/E/F to system RAM
	 */
	for (i = 0; i < PAM_NUM; i++)
		x86_pci_write_config8(PCI_BDF(0, 0, 0), pam + i, PAM_RW);

	return ret;
}

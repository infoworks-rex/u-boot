/*
 * (C) Copyright 2002-2010
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef	__ASM_GBL_DATA_H
#define __ASM_GBL_DATA_H

#ifndef __ASSEMBLY__

/* Architecture-specific global data */
struct arch_global_data {
	struct global_data *gd_addr;		/* Location of Global Data */
	uint8_t  x86;			/* CPU family */
	uint8_t  x86_vendor;		/* CPU vendor */
	uint8_t  x86_model;
	uint8_t  x86_mask;
	uint32_t x86_device;
	uint64_t tsc_base;		/* Initial value returned by rdtsc() */
	uint32_t tsc_base_kclocks;	/* Initial tsc as a kclocks value */
	uint32_t tsc_prev;		/* For show_boot_progress() */
	uint32_t tsc_mhz;		/* TSC frequency in MHz */
	void *new_fdt;			/* Relocated FDT */
	uint32_t bist;			/* Built-in self test value */
	struct pci_controller *hose;	/* PCI hose for early use */
};

#endif

#include <asm-generic/global_data.h>

#ifndef __ASSEMBLY__
static inline __attribute__((no_instrument_function)) gd_t *get_fs_gd_ptr(void)
{
	gd_t *gd_ptr;

	asm volatile("fs movl 0, %0\n" : "=r" (gd_ptr));

	return gd_ptr;
}

#define gd	get_fs_gd_ptr()

#endif

/*
 * Our private Global Data Flags
 */
#define GD_FLG_COLD_BOOT	0x00100	/* Cold Boot */
#define GD_FLG_WARM_BOOT	0x00200	/* Warm Boot */

#define DECLARE_GLOBAL_DATA_PTR

#endif /* __ASM_GBL_DATA_H */

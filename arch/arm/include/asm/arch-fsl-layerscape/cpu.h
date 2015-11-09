/*
 * Copyright 2014-2015, Freescale Semiconductor
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _FSL_LAYERSCAPE_CPU_H
#define _FSL_LAYERSCAPE_CPU_H

static struct cpu_type cpu_type_list[] = {
	CPU_TYPE_ENTRY(LS2080, LS2080, 8),
	CPU_TYPE_ENTRY(LS2085, LS2085, 8),
	CPU_TYPE_ENTRY(LS2045, LS2045, 4),
	CPU_TYPE_ENTRY(LS1043, LS1043, 4),
};

#ifndef CONFIG_SYS_DCACHE_OFF

#define SECTION_SHIFT_L0		39UL
#define SECTION_SHIFT_L1		30UL
#define SECTION_SHIFT_L2		21UL
#define BLOCK_SIZE_L0			0x8000000000
#define BLOCK_SIZE_L1			0x40000000
#define BLOCK_SIZE_L2			0x200000
#define NUM_OF_ENTRY			512
#define TCR_EL2_PS_40BIT		(2 << 16)

#define LAYERSCAPE_VA_BITS		(40)
#define LAYERSCAPE_TCR		(TCR_TG0_4K		| \
				TCR_EL2_PS_40BIT	| \
				TCR_SHARED_NON		| \
				TCR_ORGN_NC		| \
				TCR_IRGN_NC		| \
				TCR_T0SZ(LAYERSCAPE_VA_BITS))
#define LAYERSCAPE_TCR_FINAL	(TCR_TG0_4K		| \
				TCR_EL2_PS_40BIT	| \
				TCR_SHARED_OUTER	| \
				TCR_ORGN_WBWA		| \
				TCR_IRGN_WBWA		| \
				TCR_T0SZ(LAYERSCAPE_VA_BITS))

#ifdef CONFIG_FSL_LSCH3
#define CONFIG_SYS_FSL_CCSR_BASE	0x00000000
#define CONFIG_SYS_FSL_CCSR_SIZE	0x10000000
#define CONFIG_SYS_FSL_QSPI_BASE1	0x20000000
#define CONFIG_SYS_FSL_QSPI_SIZE1	0x10000000
#define CONFIG_SYS_FSL_IFC_BASE1	0x30000000
#define CONFIG_SYS_FSL_IFC_SIZE1	0x10000000
#define CONFIG_SYS_FSL_IFC_SIZE1_1	0x400000
#define CONFIG_SYS_FSL_DRAM_BASE1	0x80000000
#define CONFIG_SYS_FSL_DRAM_SIZE1	0x80000000
#define CONFIG_SYS_FSL_QSPI_BASE2	0x400000000
#define CONFIG_SYS_FSL_QSPI_SIZE2	0x100000000
#define CONFIG_SYS_FSL_IFC_BASE2	0x500000000
#define CONFIG_SYS_FSL_IFC_SIZE2	0x100000000
#define CONFIG_SYS_FSL_DCSR_BASE	0x700000000
#define CONFIG_SYS_FSL_DCSR_SIZE	0x40000000
#define CONFIG_SYS_FSL_MC_BASE		0x80c000000
#define CONFIG_SYS_FSL_MC_SIZE		0x4000000
#define CONFIG_SYS_FSL_NI_BASE		0x810000000
#define CONFIG_SYS_FSL_NI_SIZE		0x8000000
#define CONFIG_SYS_FSL_QBMAN_BASE	0x818000000
#define CONFIG_SYS_FSL_QBMAN_SIZE	0x8000000
#define CONFIG_SYS_FSL_QBMAN_SIZE_1	0x4000000
#define CONFIG_SYS_PCIE1_PHYS_SIZE	0x200000000
#define CONFIG_SYS_PCIE2_PHYS_SIZE	0x200000000
#define CONFIG_SYS_PCIE3_PHYS_SIZE	0x200000000
#define CONFIG_SYS_PCIE4_PHYS_SIZE	0x200000000
#define CONFIG_SYS_FSL_WRIOP1_BASE	0x4300000000
#define CONFIG_SYS_FSL_WRIOP1_SIZE	0x100000000
#define CONFIG_SYS_FSL_AIOP1_BASE	0x4b00000000
#define CONFIG_SYS_FSL_AIOP1_SIZE	0x100000000
#define CONFIG_SYS_FSL_PEBUF_BASE	0x4c00000000
#define CONFIG_SYS_FSL_PEBUF_SIZE	0x400000000
#define CONFIG_SYS_FSL_DRAM_BASE2	0x8080000000
#define CONFIG_SYS_FSL_DRAM_SIZE2	0x7F80000000
#elif defined(CONFIG_FSL_LSCH2)
#define CONFIG_SYS_FSL_BOOTROM_BASE	0x0
#define CONFIG_SYS_FSL_BOOTROM_SIZE	0x1000000
#define CONFIG_SYS_FSL_CCSR_BASE	0x1000000
#define CONFIG_SYS_FSL_CCSR_SIZE	0xf000000
#define CONFIG_SYS_FSL_DCSR_BASE	0x20000000
#define CONFIG_SYS_FSL_DCSR_SIZE	0x4000000
#define CONFIG_SYS_FSL_QSPI_BASE	0x40000000
#define CONFIG_SYS_FSL_QSPI_SIZE	0x20000000
#define CONFIG_SYS_FSL_IFC_BASE		0x60000000
#define CONFIG_SYS_FSL_IFC_SIZE		0x20000000
#define CONFIG_SYS_FSL_DRAM_BASE1	0x80000000
#define CONFIG_SYS_FSL_DRAM_SIZE1	0x80000000
#define CONFIG_SYS_FSL_QBMAN_BASE	0x500000000
#define CONFIG_SYS_FSL_QBMAN_SIZE	0x10000000
#define CONFIG_SYS_FSL_DRAM_BASE2	0x880000000
#define CONFIG_SYS_FSL_DRAM_SIZE2	0x780000000	/* 30GB */
#define CONFIG_SYS_PCIE1_PHYS_SIZE	0x800000000
#define CONFIG_SYS_PCIE2_PHYS_SIZE	0x800000000
#define CONFIG_SYS_PCIE3_PHYS_SIZE	0x800000000
#define CONFIG_SYS_FSL_DRAM_BASE3	0x8800000000
#define CONFIG_SYS_FSL_DRAM_SIZE3	0x7800000000	/* 480GB */
#endif

struct sys_mmu_table {
	u64 virt_addr;
	u64 phys_addr;
	u64 size;
	u64 memory_type;
	u64 share;
};

struct table_info {
	u64 *ptr;
	u64 table_base;
	u64 entry_size;
};

static const struct sys_mmu_table early_mmu_table[] = {
#ifdef CONFIG_FSL_LSCH3
	{ CONFIG_SYS_FSL_CCSR_BASE, CONFIG_SYS_FSL_CCSR_BASE,
	  CONFIG_SYS_FSL_CCSR_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_OCRAM_BASE, CONFIG_SYS_FSL_OCRAM_BASE,
	  CONFIG_SYS_FSL_OCRAM_SIZE, MT_NORMAL, PMD_SECT_NON_SHARE },
	/* For IFC Region #1, only the first 4MB is cache-enabled */
	{ CONFIG_SYS_FSL_IFC_BASE1, CONFIG_SYS_FSL_IFC_BASE1,
	  CONFIG_SYS_FSL_IFC_SIZE1_1, MT_NORMAL, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_IFC_BASE1 + CONFIG_SYS_FSL_IFC_SIZE1_1,
	  CONFIG_SYS_FSL_IFC_BASE1 + CONFIG_SYS_FSL_IFC_SIZE1_1,
	  CONFIG_SYS_FSL_IFC_SIZE1 - CONFIG_SYS_FSL_IFC_SIZE1_1,
	  MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FLASH_BASE, CONFIG_SYS_FSL_IFC_BASE1,
	  CONFIG_SYS_FSL_IFC_SIZE1, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_DRAM_BASE1, CONFIG_SYS_FSL_DRAM_BASE1,
	  CONFIG_SYS_FSL_DRAM_SIZE1, MT_NORMAL, PMD_SECT_OUTER_SHARE },
	{ CONFIG_SYS_FSL_DCSR_BASE, CONFIG_SYS_FSL_DCSR_BASE,
	  CONFIG_SYS_FSL_DCSR_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_DRAM_BASE2, CONFIG_SYS_FSL_DRAM_BASE2,
	  CONFIG_SYS_FSL_DRAM_SIZE2, MT_NORMAL, PMD_SECT_OUTER_SHARE },
#elif defined(CONFIG_FSL_LSCH2)
	{ CONFIG_SYS_FSL_CCSR_BASE, CONFIG_SYS_FSL_CCSR_BASE,
	  CONFIG_SYS_FSL_CCSR_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_OCRAM_BASE, CONFIG_SYS_FSL_OCRAM_BASE,
	  CONFIG_SYS_FSL_OCRAM_SIZE, MT_NORMAL, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_DCSR_BASE, CONFIG_SYS_FSL_DCSR_BASE,
	  CONFIG_SYS_FSL_DCSR_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_IFC_BASE, CONFIG_SYS_FSL_IFC_BASE,
	  CONFIG_SYS_FSL_IFC_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_DRAM_BASE1, CONFIG_SYS_FSL_DRAM_BASE1,
	  CONFIG_SYS_FSL_DRAM_SIZE1, MT_NORMAL, PMD_SECT_OUTER_SHARE },
	{ CONFIG_SYS_FSL_DRAM_BASE2, CONFIG_SYS_FSL_DRAM_BASE2,
	  CONFIG_SYS_FSL_DRAM_SIZE2, MT_NORMAL, PMD_SECT_OUTER_SHARE },
#endif
};

static const struct sys_mmu_table final_mmu_table[] = {
#ifdef CONFIG_FSL_LSCH3
	{ CONFIG_SYS_FSL_CCSR_BASE, CONFIG_SYS_FSL_CCSR_BASE,
	  CONFIG_SYS_FSL_CCSR_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_OCRAM_BASE, CONFIG_SYS_FSL_OCRAM_BASE,
	  CONFIG_SYS_FSL_OCRAM_SIZE, MT_NORMAL, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_DRAM_BASE1, CONFIG_SYS_FSL_DRAM_BASE1,
	  CONFIG_SYS_FSL_DRAM_SIZE1, MT_NORMAL, PMD_SECT_OUTER_SHARE },
	{ CONFIG_SYS_FSL_QSPI_BASE2, CONFIG_SYS_FSL_QSPI_BASE2,
	  CONFIG_SYS_FSL_QSPI_SIZE2, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_IFC_BASE2, CONFIG_SYS_FSL_IFC_BASE2,
	  CONFIG_SYS_FSL_IFC_SIZE2, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_DCSR_BASE, CONFIG_SYS_FSL_DCSR_BASE,
	  CONFIG_SYS_FSL_DCSR_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_MC_BASE, CONFIG_SYS_FSL_MC_BASE,
	  CONFIG_SYS_FSL_MC_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_NI_BASE, CONFIG_SYS_FSL_NI_BASE,
	  CONFIG_SYS_FSL_NI_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	/* For QBMAN portal, only the first 64MB is cache-enabled */
	{ CONFIG_SYS_FSL_QBMAN_BASE, CONFIG_SYS_FSL_QBMAN_BASE,
	  CONFIG_SYS_FSL_QBMAN_SIZE_1, MT_NORMAL, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_QBMAN_BASE + CONFIG_SYS_FSL_QBMAN_SIZE_1,
	  CONFIG_SYS_FSL_QBMAN_BASE + CONFIG_SYS_FSL_QBMAN_SIZE_1,
	  CONFIG_SYS_FSL_QBMAN_SIZE - CONFIG_SYS_FSL_QBMAN_SIZE_1,
	  MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_PCIE1_PHYS_ADDR, CONFIG_SYS_PCIE1_PHYS_ADDR,
	  CONFIG_SYS_PCIE1_PHYS_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_PCIE2_PHYS_ADDR, CONFIG_SYS_PCIE2_PHYS_ADDR,
	  CONFIG_SYS_PCIE2_PHYS_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_PCIE3_PHYS_ADDR, CONFIG_SYS_PCIE3_PHYS_ADDR,
	  CONFIG_SYS_PCIE3_PHYS_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
#ifdef CONFIG_LS2080A
	{ CONFIG_SYS_PCIE4_PHYS_ADDR, CONFIG_SYS_PCIE4_PHYS_ADDR,
	  CONFIG_SYS_PCIE4_PHYS_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
#endif
	{ CONFIG_SYS_FSL_WRIOP1_BASE, CONFIG_SYS_FSL_WRIOP1_BASE,
	  CONFIG_SYS_FSL_WRIOP1_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_AIOP1_BASE, CONFIG_SYS_FSL_AIOP1_BASE,
	  CONFIG_SYS_FSL_AIOP1_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_PEBUF_BASE, CONFIG_SYS_FSL_PEBUF_BASE,
	  CONFIG_SYS_FSL_PEBUF_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_DRAM_BASE2, CONFIG_SYS_FSL_DRAM_BASE2,
	  CONFIG_SYS_FSL_DRAM_SIZE2, MT_NORMAL, PMD_SECT_OUTER_SHARE },
#elif defined(CONFIG_FSL_LSCH2)
	{ CONFIG_SYS_FSL_BOOTROM_BASE, CONFIG_SYS_FSL_BOOTROM_BASE,
	  CONFIG_SYS_FSL_BOOTROM_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_CCSR_BASE, CONFIG_SYS_FSL_CCSR_BASE,
	  CONFIG_SYS_FSL_CCSR_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_OCRAM_BASE, CONFIG_SYS_FSL_OCRAM_BASE,
	  CONFIG_SYS_FSL_OCRAM_SIZE, MT_NORMAL, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_DCSR_BASE, CONFIG_SYS_FSL_DCSR_BASE,
	  CONFIG_SYS_FSL_DCSR_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_QSPI_BASE, CONFIG_SYS_FSL_QSPI_BASE,
	  CONFIG_SYS_FSL_QSPI_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_IFC_BASE, CONFIG_SYS_FSL_IFC_BASE,
	  CONFIG_SYS_FSL_IFC_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_DRAM_BASE1, CONFIG_SYS_FSL_DRAM_BASE1,
	  CONFIG_SYS_FSL_DRAM_SIZE1, MT_NORMAL,
	  PMD_SECT_OUTER_SHARE | PMD_SECT_NS },
	{ CONFIG_SYS_FSL_QBMAN_BASE, CONFIG_SYS_FSL_QBMAN_BASE,
	  CONFIG_SYS_FSL_QBMAN_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_DRAM_BASE2, CONFIG_SYS_FSL_DRAM_BASE2,
	  CONFIG_SYS_FSL_DRAM_SIZE2, MT_NORMAL, PMD_SECT_OUTER_SHARE },
	{ CONFIG_SYS_PCIE1_PHYS_ADDR, CONFIG_SYS_PCIE1_PHYS_ADDR,
	  CONFIG_SYS_PCIE1_PHYS_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_PCIE2_PHYS_ADDR, CONFIG_SYS_PCIE2_PHYS_ADDR,
	  CONFIG_SYS_PCIE2_PHYS_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_PCIE3_PHYS_ADDR, CONFIG_SYS_PCIE3_PHYS_ADDR,
	  CONFIG_SYS_PCIE3_PHYS_SIZE, MT_DEVICE_NGNRNE, PMD_SECT_NON_SHARE },
	{ CONFIG_SYS_FSL_DRAM_BASE3, CONFIG_SYS_FSL_DRAM_BASE3,
	  CONFIG_SYS_FSL_DRAM_SIZE3, MT_NORMAL, PMD_SECT_OUTER_SHARE },
#endif
};
#endif

int fsl_qoriq_core_to_cluster(unsigned int core);
u32 cpu_mask(void);
#endif /* _FSL_LAYERSCAPE_CPU_H */

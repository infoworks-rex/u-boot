/* SPDX-License-Identifier:     GPL-2.0+ */
/*
 * (C) Copyright 2018 Rockchip Electronics Co., Ltd
 *
 */

#ifndef __RK_ATAGS_H_
#define __RK_ATAGS_H_

/* Tag magic */
#define ATAG_CORE		0x54410001
#define ATAG_NONE		0x00000000

#define ATAG_SERIAL		0x54410050
#define ATAG_BOOTDEV		0x54410051
#define ATAG_DDR_MEM		0x54410052
#define ATAG_TOS_MEM		0x54410053
#define ATAG_RAM_PARTITION	0x54410054
#define ATAG_ATF_MEM		0x54410055
#define ATAG_MAX		0x544100ff

/* Tag size and offset */
#define ATAGS_SIZE		(0x2000)	/* 8K */
#define ATAGS_OFFSET		(0x200000 - ATAGS_SIZE)/* [2M-8K, 2M] */

/* Tag sdram position!! */
#define ATAGS_PHYS_BASE		(CONFIG_SYS_SDRAM_BASE + ATAGS_OFFSET)

#ifndef ATAGS_PHYS_BASE
"ERROR: ATAGS_PHYS_BASE is not defined!!"
#endif

/* tag_bootdev.devtype */
#define BOOT_TYPE_NAND		(1 << 0)
#define BOOT_TYPE_EMMC		(1 << 1)
#define BOOT_TYPE_SD0		(1 << 2)
#define BOOT_TYPE_SD1		(1 << 3)
#define BOOT_TYPE_SPI_NOR	(1 << 4)
#define BOOT_TYPE_SPI_NAND	(1 << 5)
#define BOOT_TYPE_RAM		(1 << 6)

/* tag_serial.m_mode */
#define SERIAL_M_MODE_M0	0x0
#define SERIAL_M_MODE_M1	0x1
#define SERIAL_M_MODE_M2	0x2

struct tag_serial {
	u32 version;
	u32 enable;
	u64 addr;
	u32 baudrate;
	u32 m_mode;
	u32 id;
	u32 reserved[2];
	u32 hash;
} __packed;

struct tag_bootdev {
	u32 version;
	u32 devtype;
	u32 devnum;
	u32 mode;
	u32 reserved[7];
	u32 hash;
} __packed;

struct tag_ddr_mem {
	u32 count;
	u32 version;
	u64 bank[20];
	u32 reserved[3];
	u32 hash;
} __packed;

struct tag_tos_mem {
	u32 version;
	struct {
		char name[8];
		u64 phy_addr;
		u32 size;
		u32 flags;
	} tee_mem;

	struct {
		char name[8];
		u64 phy_addr;
		u32 size;
		u32 flags;
	} drm_mem;

	u64 reserved[7];
	u32 reserved1;
	u32 hash;
} __packed;

struct tag_atf_mem {
	u32 version;
	u64 phy_addr;
	u32 size;
	u32 flags;
	u32 reserved[2];
	u32 hash;
} __packed;

struct tag_ram_partition {
	u32 version;
	u32 count;
	u32 reserved[4];

	struct {
		char name[16];
		u64 start;
		u64 size;
	} part[6];

	u32 reserved1[3];
	u32 hash;
} __packed;

struct tag_core {
	u32 flags;
	u32 pagesize;
	u32 rootdev;
} __packed;

struct tag_header {
	u32 size;	/* bytes = size * 4 */
	u32 magic;
} __packed;

/* Must be 4 bytes align */
struct tag {
	struct tag_header hdr;
	union {
		struct tag_core		core;
		struct tag_serial	serial;
		struct tag_bootdev	bootdev;
		struct tag_ddr_mem	ddr_mem;
		struct tag_tos_mem	tos_mem;
		struct tag_ram_partition ram_part;
		struct tag_atf_mem	atf_mem;
	} u;
} __aligned(4);

/*
 * Destroy atags
 *
 * first pre-loader who creates atags must call it before atags_set_tag(),
 * because atags_set_tag() may detect last valid and existence ATAG_CORE
 * tag in memory and lead a wrong setup, that is not what we expect.
 */
void atags_destroy(void);

/*
 * atags_set_tag - set tag data
 *
 * @magic: tag magic, i.e. ATAG_SERIAL, ATAG_BOOTDEV, ....
 * @tagdata: core data of struct, i.e. struct tag_serial/tag_bootdev ...
 *
 * return: 0 on success, others failed.
 */
int atags_set_tag(u32 magic, void *tagdata);

/*
 * atags_get_tag - get tag by tag magic
 *
 * @magic: tag magic, i.e. ATAG_SERIAL, ATAG_BOOTDEV, ...
 *
 * return: NULL on failed, otherwise return the tag that we want.
 */
struct tag *atags_get_tag(u32 magic);

/*
 * atags_is_available - check if atags is available, used for second or
 *			later pre-loaders.
 *
 * return: 0 is not available, otherwise available.
 */
int atags_is_available(void);

/* Print only one tag */
void atags_print_tag(struct tag *t);

/* Print all tags */
void atags_print_all_tags(void);

/* An atags example test */
void atags_test(void);

/* Atags stat */
void atags_stat(void);

#if CONFIG_IS_ENABLED(TINY_FRAMEWORK) &&		\
	!CONFIG_IS_ENABLED(LIBGENERIC_SUPPORT) &&	\
	defined(CONFIG_ARM64)
void *memset(void *s, int c, size_t count);
void *memcpy(void *dest, const void *src, size_t count);
#endif

#endif

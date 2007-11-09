/*
 * (C) Copyright 2005-2007
 * Samsung Electronics,
 * Kyungmin Park <kyungmin.park@samsung.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _APOLLON_OMAP24XX_MEM_H_
#define _APOLLON_OMAP24XX_MEM_H_

/* Slower full frequency range default timings for x32 operation*/
#define APOLLON_2420_SDRC_SHARING		0x00000100
#define APOLLON_2420_SDRC_MDCFG_0_DDR		0x00d04011
#define APOLLON_2420_SDRC_MR_0_DDR		0x00000032

/* optimized timings good for current shipping parts */
#define APOLLON_242X_SDRC_ACTIM_CTRLA_0_100MHz	0x4A59B485
#define APOLLON_242X_SDRC_ACTIM_CTRLB_0_100MHz	0x0000000C

#define APOLLON_242X_SDRC_ACTIM_CTRLA_0_166MHz	0x7BA35907
#define APOLLON_242X_SDRC_ACTIM_CTRLB_0_166MHz	0x00000013

#define APOLLON_242X_SDRC_RFR_CTRL_100MHz	0x00030001
#define APOLLON_242X_SDRC_RFR_CTRL_166MHz	0x00044C01

#define APOLLON_242x_SDRC_DLLAB_CTRL_100MHz	0x00007306
#define APOLLON_242x_SDRC_DLLAB_CTRL_166MHz	0x00000506

#ifdef PRCM_CONFIG_I
# define APOLLON_2420_SDRC_ACTIM_CTRLA_0	APOLLON_242X_SDRC_ACTIM_CTRLA_0_166MHz
# define APOLLON_2420_SDRC_ACTIM_CTRLB_0	APOLLON_242X_SDRC_ACTIM_CTRLB_0_166MHz
# define APOLLON_2420_SDRC_RFR_CTRL		APOLLON_242X_SDRC_RFR_CTRL_166MHz
# define APOLLON_2420_SDRC_DLLAB_CTRL		APOLLON_242x_SDRC_DLLAB_CTRL_166MHz
#elif PRCM_CONFIG_II
# define APOLLON_2420_SDRC_ACTIM_CTRLA_0	APOLLON_242X_SDRC_ACTIM_CTRLA_0_100MHz
# define APOLLON_2420_SDRC_ACTIM_CTRLB_0	APOLLON_242X_SDRC_ACTIM_CTRLB_0_100MHz
# define APOLLON_2420_SDRC_RFR_CTRL		APOLLON_242X_SDRC_RFR_CTRL_100MHz
# define APOLLON_2420_SDRC_DLLAB_CTRL		APOLLON_242x_SDRC_DLLAB_CTRL_100MHz
#endif

/* GPMC settings */
#ifdef PRCM_CONFIG_I		/* L3 at 165MHz */
/* CS0: OneNAND */
# define APOLLON_24XX_GPMC_CONFIG1_0	0x00000001
# define APOLLON_24XX_GPMC_CONFIG2_0	0x000c1000
# define APOLLON_24XX_GPMC_CONFIG3_0	0x00030400
# define APOLLON_24XX_GPMC_CONFIG4_0	0x0b841006
# define APOLLON_24XX_GPMC_CONFIG5_0	0x020f0c11
# define APOLLON_24XX_GPMC_CONFIG6_0	0x00000000
# define APOLLON_24XX_GPMC_CONFIG7_0	(0x00000e40|(APOLLON_CS0_BASE >> 24))

/* CS1: Ethernet */
# define APOLLON_24XX_GPMC_CONFIG1_1	0x00011200
# define APOLLON_24XX_GPMC_CONFIG2_1	0x001F1F01
# define APOLLON_24XX_GPMC_CONFIG3_1	0x00080803
# define APOLLON_24XX_GPMC_CONFIG4_1	0x1C0b1C0a
# define APOLLON_24XX_GPMC_CONFIG5_1	0x041F1F1F
# define APOLLON_24XX_GPMC_CONFIG6_1	0x000004C4
# define APOLLON_24XX_GPMC_CONFIG7_1	(0x00000F40|(APOLLON_CS1_BASE >> 24))

/* CS2: OneNAND */
/* It's same as CS0 */
# define APOLLON_24XX_GPMC_CONFIG7_2	(0x00000e40|(APOLLON_CS2_BASE >> 24))

/* CS3: NOR */
#ifdef ASYNC_NOR
# define APOLLON_24XX_GPMC_CONFIG1_3	0x00021201
# define APOLLON_24XX_GPMC_CONFIG2_3	0x00121601
# define APOLLON_24XX_GPMC_CONFIG3_3	0x00040401
# define APOLLON_24XX_GPMC_CONFIG4_3	0x12061605
# define APOLLON_24XX_GPMC_CONFIG5_3	0x01151317
#else
# define SYNC_NOR_VALUE			0x24aaa
# define APOLLON_24XX_GPMC_CONFIG1_3	0xe5011211
# define APOLLON_24XX_GPMC_CONFIG2_3	0x00090b01
# define APOLLON_24XX_GPMC_CONFIG3_3	0x00020201
# define APOLLON_24XX_GPMC_CONFIG4_3	0x09030b03
# define APOLLON_24XX_GPMC_CONFIG5_3	0x010a0a0c
#endif /* ASYNC_NOR */
# define APOLLON_24XX_GPMC_CONFIG6_3	0x00000000
# define APOLLON_24XX_GPMC_CONFIG7_3	(0x00000e40|(APOLLON_CS3_BASE >> 24))
#endif /* endif PRCM_CONFIG_I */

#ifdef PRCM_CONFIG_II		/* L3 at 100MHz */
/* CS0: OneNAND */
# define APOLLON_24XX_GPMC_CONFIG1_0	0x00000001
# define APOLLON_24XX_GPMC_CONFIG2_0	0x00081080
# define APOLLON_24XX_GPMC_CONFIG3_0	0x00030300
# define APOLLON_24XX_GPMC_CONFIG4_0	0x08041004
# define APOLLON_24XX_GPMC_CONFIG5_0	0x020b0910
# define APOLLON_24XX_GPMC_CONFIG6_0	0x00000000
# define APOLLON_24XX_GPMC_CONFIG7_0	(0x00000C40|(APOLLON_CS0_BASE >> 24))

/* CS1: ethernet */
# define APOLLON_24XX_GPMC_CONFIG1_1	0x00401203
# define APOLLON_24XX_GPMC_CONFIG2_1	0x001F1F01
# define APOLLON_24XX_GPMC_CONFIG3_1	0x00080803
# define APOLLON_24XX_GPMC_CONFIG4_1	0x1C091C09
# define APOLLON_24XX_GPMC_CONFIG5_1	0x041F1F1F
# define APOLLON_24XX_GPMC_CONFIG6_1	0x000004C4
# define APOLLON_24XX_GPMC_CONFIG7_1	(0x00000F40|(APOLLON_CS1_BASE >> 24))

/* CS2: OneNAND */
/* It's same as CS0 */
# define APOLLON_24XX_GPMC_CONFIG7_2	(0x00000e40|(APOLLON_CS2_BASE >> 24))

/* CS3: NOR */
#define ASYNC_NOR
#ifdef ASYNC_NOR
# define APOLLON_24XX_GPMC_CONFIG1_3	0x00021201
# define APOLLON_24XX_GPMC_CONFIG2_3	0x00121601
# define APOLLON_24XX_GPMC_CONFIG3_3	0x00040401
# define APOLLON_24XX_GPMC_CONFIG4_3	0x12061605
# define APOLLON_24XX_GPMC_CONFIG5_3	0x01151317
#else
# define SYNC_NOR_VALUE			0x24aaa
# define APOLLON_24XX_GPMC_CONFIG1_3	0xe1001202
# define APOLLON_24XX_GPMC_CONFIG2_3	0x00151501
# define APOLLON_24XX_GPMC_CONFIG3_3	0x00050501
# define APOLLON_24XX_GPMC_CONFIG4_3	0x0e070e07
# define APOLLON_24XX_GPMC_CONFIG5_3	0x01131F1F
#endif /* ASYNC_NOR */
# define APOLLON_24XX_GPMC_CONFIG6_3	0x00000000
# define APOLLON_24XX_GPMC_CONFIG7_3	(0x00000C40|(APOLLON_CS3_BASE >> 24))
#endif /* endif PRCM_CONFIG_II */

#ifdef PRCM_CONFIG_III		/* L3 at 133MHz */
# ifdef CFG_NAND_BOOT
#  define APOLLON_24XX_GPMC_CONFIG1_0   0x0
#  define APOLLON_24XX_GPMC_CONFIG2_0   0x00141400
#  define APOLLON_24XX_GPMC_CONFIG3_0   0x00141400
#  define APOLLON_24XX_GPMC_CONFIG4_0   0x0F010F01
#  define APOLLON_24XX_GPMC_CONFIG5_0   0x010C1414
#  define APOLLON_24XX_GPMC_CONFIG6_0   0x00000A80
# else /* NOR boot */
#  define APOLLON_24XX_GPMC_CONFIG1_0   0x3
#  define APOLLON_24XX_GPMC_CONFIG2_0   0x00151501
#  define APOLLON_24XX_GPMC_CONFIG3_0   0x00060602
#  define APOLLON_24XX_GPMC_CONFIG4_0   0x10081008
#  define APOLLON_24XX_GPMC_CONFIG5_0   0x01131F1F
#  define APOLLON_24XX_GPMC_CONFIG6_0   0x000004c4
# endif	/* endif CFG_NAND_BOOT */
# define APOLLON_24XX_GPMC_CONFIG7_0	(0x00000C40|(APOLLON_CS0_BASE >> 24))
# define APOLLON_24XX_GPMC_CONFIG1_1	0x00011000
# define APOLLON_24XX_GPMC_CONFIG2_1	0x001f1f01
# define APOLLON_24XX_GPMC_CONFIG3_1	0x00080803
# define APOLLON_24XX_GPMC_CONFIG4_1	0x1C091C09
# define APOLLON_24XX_GPMC_CONFIG5_1	0x041f1F1F
# define APOLLON_24XX_GPMC_CONFIG6_1	0x000004C4
# define APOLLON_24XX_GPMC_CONFIG7_1	(0x00000F40|(APOLLON_CS1_BASE >> 24))
#endif /* endif CFG_PRCM_III */

#endif /* endif _APOLLON_OMAP24XX_MEM_H_ */

/*
 * Copyright 2012 Freescale Semiconductor, Inc.
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
#include <asm/fsl_serdes.h>
#include <asm/processor.h>
#include <asm/io.h>
#include "fsl_corenet2_serdes.h"

struct serdes_config {
	u32 protocol;
	u8 lanes[SRDS_MAX_LANES];
};

static const struct serdes_config serdes1_cfg_tbl[] = {
	/* SerDes 1 */
	{1, {XAUI_FM1_MAC9, XAUI_FM1_MAC9,
		XAUI_FM1_MAC9, XAUI_FM1_MAC9,
		XAUI_FM1_MAC10, XAUI_FM1_MAC10,
		XAUI_FM1_MAC10, XAUI_FM1_MAC10}},
	{2, {HIGIG_FM1_MAC9, HIGIG_FM1_MAC9,
		HIGIG_FM1_MAC9, HIGIG_FM1_MAC9,
		HIGIG_FM1_MAC10, HIGIG_FM1_MAC10,
		HIGIG_FM1_MAC10, HIGIG_FM1_MAC10}},
	{4, {HIGIG_FM1_MAC9, HIGIG_FM1_MAC9,
		HIGIG_FM1_MAC9, HIGIG_FM1_MAC9,
		HIGIG_FM1_MAC10, HIGIG_FM1_MAC10,
		HIGIG_FM1_MAC10, HIGIG_FM1_MAC10}},
	{28, {SGMII_FM1_DTSEC5, SGMII_FM1_DTSEC6,
		SGMII_FM1_DTSEC10, SGMII_FM1_DTSEC9,
		SGMII_FM1_DTSEC1, SGMII_FM1_DTSEC2,
		SGMII_FM1_DTSEC3, SGMII_FM1_DTSEC4}},
	{36, {SGMII_FM1_DTSEC5, SGMII_FM1_DTSEC6,
		SGMII_FM1_DTSEC10, SGMII_FM1_DTSEC9,
		SGMII_FM1_DTSEC1, SGMII_FM1_DTSEC2,
		SGMII_FM1_DTSEC3, SGMII_FM1_DTSEC4}},
	{38, {NONE, NONE, QSGMII_FM1_B, NONE,
		NONE, NONE, QSGMII_FM1_A, NONE}},
	{40, {SGMII_FM1_DTSEC5, SGMII_FM1_DTSEC6,
		SGMII_FM1_DTSEC10, SGMII_FM1_DTSEC9,
		NONE, NONE, QSGMII_FM1_A, NONE}},
	{46, {SGMII_FM1_DTSEC5, SGMII_FM1_DTSEC6,
		SGMII_FM1_DTSEC10, SGMII_FM1_DTSEC9,
		NONE, NONE, QSGMII_FM1_A, NONE}},
	{48, {SGMII_FM1_DTSEC5, SGMII_FM1_DTSEC6,
		SGMII_FM1_DTSEC10, SGMII_FM1_DTSEC9,
		NONE, NONE, QSGMII_FM1_A, NONE}},
	{}
};
static const struct serdes_config serdes2_cfg_tbl[] = {
	/* SerDes 2 */
	{1, {XAUI_FM2_MAC9, XAUI_FM2_MAC9,
		XAUI_FM2_MAC9, XAUI_FM2_MAC9,
		XAUI_FM2_MAC10, XAUI_FM2_MAC10,
		XAUI_FM2_MAC10, XAUI_FM2_MAC10}},
	{2, {HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		HIGIG_FM2_MAC10, HIGIG_FM2_MAC10,
		HIGIG_FM2_MAC10, HIGIG_FM2_MAC10}},
	{4, {HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		HIGIG_FM2_MAC10, HIGIG_FM2_MAC10,
		HIGIG_FM2_MAC10, HIGIG_FM2_MAC10}},
	{7, {XAUI_FM2_MAC9, XAUI_FM2_MAC9,
		XAUI_FM2_MAC9, XAUI_FM2_MAC9,
		SGMII_FM2_DTSEC1, SGMII_FM2_DTSEC2,
		SGMII_FM2_DTSEC3, SGMII_FM2_DTSEC4}},
	{13, {XAUI_FM2_MAC9, XAUI_FM2_MAC9,
		XAUI_FM2_MAC9, XAUI_FM2_MAC9,
		SGMII_FM2_DTSEC1, SGMII_FM2_DTSEC2,
		SGMII_FM2_DTSEC3, SGMII_FM2_DTSEC4}},
	{14, {XAUI_FM2_MAC9, XAUI_FM2_MAC9,
		XAUI_FM2_MAC9, XAUI_FM2_MAC9,
		SGMII_FM2_DTSEC1, SGMII_FM2_DTSEC2,
		SGMII_FM2_DTSEC3, SGMII_FM2_DTSEC4}},
	{16, {HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		SGMII_FM2_DTSEC1, SGMII_FM2_DTSEC2,
		SGMII_FM2_DTSEC3, SGMII_FM2_DTSEC4}},
	{22, {HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		SGMII_FM2_DTSEC1, SGMII_FM2_DTSEC2,
		SGMII_FM2_DTSEC3, SGMII_FM2_DTSEC4}},
	{23, {HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		SGMII_FM2_DTSEC1, SGMII_FM2_DTSEC2,
		SGMII_FM2_DTSEC3, SGMII_FM2_DTSEC4}},
	{25, {HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		SGMII_FM2_DTSEC1, SGMII_FM2_DTSEC2,
		SGMII_FM2_DTSEC3, SGMII_FM2_DTSEC4}},
	{26, {HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		SGMII_FM2_DTSEC1, SGMII_FM2_DTSEC2,
		SGMII_FM2_DTSEC3, SGMII_FM2_DTSEC4}},
	{28, {SGMII_FM2_DTSEC5, SGMII_FM2_DTSEC6,
		SGMII_FM2_DTSEC10, SGMII_FM2_DTSEC9,
		SGMII_FM2_DTSEC1, SGMII_FM2_DTSEC2,
		SGMII_FM2_DTSEC3, SGMII_FM2_DTSEC4}},
	{36, {SGMII_FM2_DTSEC5, SGMII_FM2_DTSEC6,
		SGMII_FM2_DTSEC10, SGMII_FM2_DTSEC9,
		SGMII_FM2_DTSEC1, SGMII_FM2_DTSEC2,
		SGMII_FM2_DTSEC3, SGMII_FM2_DTSEC4}},
	{38, {NONE, NONE, QSGMII_FM2_B, NONE,
		NONE, NONE, QSGMII_FM1_A, NONE}},
	{40, {SGMII_FM2_DTSEC5, SGMII_FM2_DTSEC6,
		SGMII_FM2_DTSEC10, SGMII_FM2_DTSEC9,
		NONE, NONE, QSGMII_FM1_A, NONE}},
	{46, {SGMII_FM2_DTSEC5, SGMII_FM2_DTSEC6,
		SGMII_FM2_DTSEC10, SGMII_FM2_DTSEC9,
		NONE, NONE, QSGMII_FM1_A, NONE}},
	{48, {SGMII_FM2_DTSEC5, SGMII_FM2_DTSEC6,
		SGMII_FM2_DTSEC10, SGMII_FM2_DTSEC9,
		NONE, NONE, QSGMII_FM1_A, NONE}},
	{50, {XAUI_FM2_MAC9, XAUI_FM2_MAC9,
		XAUI_FM2_MAC9, XAUI_FM2_MAC9,
		NONE, NONE, QSGMII_FM1_A, NONE}},
	{52, {HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		NONE, NONE, QSGMII_FM1_A, NONE}},
	{54, {HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		HIGIG_FM2_MAC9, HIGIG_FM2_MAC9,
		NONE, NONE, QSGMII_FM1_A, NONE}},
	{56, {XFI_FM1_MAC9, XFI_FM1_MAC10,
		XFI_FM2_MAC10, XFI_FM2_MAC9,
		SGMII_FM2_DTSEC1, SGMII_FM2_DTSEC2,
		SGMII_FM2_DTSEC3, SGMII_FM2_DTSEC4}},
	{57, {XFI_FM1_MAC9, XFI_FM1_MAC10,
		XFI_FM2_MAC10, XFI_FM2_MAC9,
		SGMII_FM2_DTSEC1, SGMII_FM2_DTSEC2,
		SGMII_FM2_DTSEC3, SGMII_FM2_DTSEC4}},
	{}
};
static const struct serdes_config serdes3_cfg_tbl[] = {
	/* SerDes 3 */
	{2, {PCIE1, PCIE1, PCIE1, PCIE1, PCIE1, PCIE1, PCIE1, PCIE1}},
	{4, {PCIE1, PCIE1, PCIE1, PCIE1, PCIE2, PCIE2, PCIE2, PCIE2}},
	{6, {PCIE1, PCIE1, PCIE1, PCIE1, SRIO1, SRIO1, SRIO1, SRIO1}},
	{8, {PCIE1, PCIE1, PCIE1, PCIE1, SRIO1, NONE, NONE, NONE}},
	{9, {INTERLAKEN, INTERLAKEN, INTERLAKEN, INTERLAKEN,
		INTERLAKEN, INTERLAKEN, INTERLAKEN, INTERLAKEN}},
	{10, {INTERLAKEN, INTERLAKEN, INTERLAKEN, INTERLAKEN,
		INTERLAKEN, INTERLAKEN, INTERLAKEN, INTERLAKEN}},
	{12, {INTERLAKEN, INTERLAKEN, INTERLAKEN, INTERLAKEN,
		PCIE2, PCIE2, PCIE2, PCIE2}},
	{14, {INTERLAKEN, INTERLAKEN, INTERLAKEN, INTERLAKEN,
		PCIE2, PCIE2, PCIE2, PCIE2}},
	{16, {INTERLAKEN, INTERLAKEN, INTERLAKEN, INTERLAKEN,
		SRIO1, SRIO1, SRIO1, SRIO1}},
	{17, {INTERLAKEN, INTERLAKEN, INTERLAKEN, INTERLAKEN,
		SRIO1, SRIO1, SRIO1, SRIO1}},
	{19, {INTERLAKEN, INTERLAKEN, INTERLAKEN, INTERLAKEN,
		SRIO1, SRIO1, SRIO1, SRIO1}},
	{20, {INTERLAKEN, INTERLAKEN, INTERLAKEN, INTERLAKEN,
		SRIO1, SRIO1, SRIO1, SRIO1}},
	{}
};
static const struct serdes_config serdes4_cfg_tbl[] = {
	/* SerDes 4 */
	{2, {PCIE3, PCIE3, PCIE3, PCIE3, PCIE3, PCIE3, PCIE3, PCIE3}},
	{4, {PCIE3, PCIE3, PCIE3, PCIE3, PCIE4, PCIE4, PCIE4, PCIE4}},
	{6, {PCIE3, PCIE3, PCIE3, PCIE3, SRIO2, SRIO2, SRIO2, SRIO2}},
	{8, {PCIE3, PCIE3, PCIE3, PCIE3, SRIO2, SRIO2, SRIO2, SRIO2}},
	{10, {PCIE3, PCIE3, PCIE3, PCIE3, PCIE4, PCIE4, SATA1, SATA2} },
	{12, {PCIE3, PCIE3, PCIE3, PCIE3, AURORA, AURORA, SATA1, SATA2} },
	{14, {PCIE3, PCIE3, PCIE3, PCIE3, AURORA, AURORA, SRIO2, SRIO2}},
	{16, {PCIE3, PCIE3, PCIE3, PCIE3, AURORA, AURORA, SRIO2, SRIO2}},
	{18, {PCIE3, PCIE3, PCIE3, PCIE3, AURORA, AURORA, AURORA, AURORA}},
	{}
};
static const struct serdes_config *serdes_cfg_tbl[] = {
	serdes1_cfg_tbl,
	serdes2_cfg_tbl,
	serdes3_cfg_tbl,
	serdes4_cfg_tbl,
};

enum srds_prtcl serdes_get_prtcl(int serdes, int cfg, int lane)
{
	const struct serdes_config *ptr;

	if (serdes >= ARRAY_SIZE(serdes_cfg_tbl))
		return 0;

	ptr = serdes_cfg_tbl[serdes];
	while (ptr->protocol) {
		if (ptr->protocol == cfg)
			return ptr->lanes[lane];
		ptr++;
	}
	return 0;
}

int is_serdes_prtcl_valid(int serdes, u32 prtcl)
{
	int i;
	const struct serdes_config *ptr;

	if (serdes >= ARRAY_SIZE(serdes_cfg_tbl))
		return 0;

	ptr = serdes_cfg_tbl[serdes];
	while (ptr->protocol) {
		if (ptr->protocol == prtcl)
			break;
		ptr++;
	}

	if (!ptr->protocol)
		return 0;

	for (i = 0; i < SRDS_MAX_LANES; i++) {
		if (ptr->lanes[i] != NONE)
			return 1;
	}

	return 0;
}

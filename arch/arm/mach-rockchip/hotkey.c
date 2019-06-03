// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
 */

#include <common.h>
#include <bidram.h>
#include <console.h>
#include <sysmem.h>
#include <asm/arch/hotkey.h>

DECLARE_GLOBAL_DATA_PTR;

#define CTRL_B		0x02
#define CTRL_D		0x04
#define CTRL_F		0x06
#define CTRL_I		0x09
#define CTRL_M		0x0d
#define CTRL_P		0x10

bool is_hotkey(enum hotkey_t id)
{
	switch (id) {
	case HK_BROM_DNL:
		return gd->console_evt == CTRL_B;
	case HK_CMDLINE:
		return gd->console_evt == CTRL_P;
	case HK_FASTBOOT:
		return gd->console_evt == CTRL_F;
	case HK_INITCALL:
		return gd->console_evt == CTRL_I;
	case HK_ROCKUSB_DNL:
		return gd->console_evt == CTRL_D;
	case HK_SYSMEM:
		return gd->console_evt == CTRL_M;
	default:
		break;
	}

	return false;
}

void hotkey_run(enum hotkey_t id)
{
	switch ((id)) {
	case HK_SYSMEM:
		if (gd->console_evt == CTRL_M) {
			bidram_dump();
			sysmem_dump();
		}
		break;
	case HK_CMDLINE:
		if (gd->console_evt == CTRL_P)
			printf("cmdline: %s\n", env_get("bootargs"));
		break;
	case HK_INITCALL:
		if (gd->console_evt == CTRL_I)
			env_update("bootargs", "initcall_debug debug");
		break;
	default:
		break;
	}
}

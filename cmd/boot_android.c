/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <android_bootloader.h>
#include <common.h>
#include <command.h>

/**
 * part_get_info_by_dev_and_name - Parse a device number and partition name
 * string in the form of "device_num;partition_name", for example "0;misc".
 * If the partition is found, sets dev_desc and part_info accordingly with the
 * information of the partition with the given partition_name.
 *
 * @dev_iface:		Device interface. Only "mmc" is supported at the moment.
 * @dev_part_str:	Input string argument, like "0;misc".
 * @dev_desc:		Place to put the device description pointer.
 * @part_info:		Place to put the partition information.
 * @return 0 on success, or -1 on error
 */
static int part_get_info_by_dev_and_name(const char *dev_iface,
					 const char *dev_part_str,
					 struct blk_desc **dev_desc,
					 disk_partition_t *part_info)
{
#ifdef CONFIG_EFI_PARTITION
	char *ep;
	const char *part_str;
	int dev_num;

	/* We only support "mmc" for looking up partition names. */
	if (strcmp(dev_iface, "mmc") != 0)
		return -1;

	part_str = strchr(dev_part_str, ';');
	if (!part_str)
		return -1;

	dev_num = simple_strtoul(dev_part_str, &ep, 16);
	if (ep != part_str) {
		/* Not all the first part before the ; was parsed. */
		return -1;
	}
	part_str++;

	*dev_desc = blk_get_dev("mmc", dev_num);
	if (!*dev_desc) {
		printf("Could not find mmc %d\n", dev_num);
		return -1;
	}
	if (part_get_info_by_name(*dev_desc, part_str, part_info) < 0) {
		printf("Could not find \"%s\" partition\n", part_str);
		return -1;
	}
	return 0;
#else
	return -1;
#endif  /* CONFIG_EFI_PARTITION */
}

static int do_boot_android(cmd_tbl_t *cmdtp, int flag, int argc,
			   char * const argv[])
{
	unsigned long load_address;
	int ret = CMD_RET_SUCCESS;
	char *addr_arg_endp, *addr_str;
	struct blk_desc *dev_desc;
	disk_partition_t part_info;
	const char *misc_part_iface;
	const char *misc_part_desc;

	if (argc < 3)
		return CMD_RET_USAGE;
	if (argc > 4)
		return CMD_RET_USAGE;

	if (argc >= 4) {
		load_address = simple_strtoul(argv[3], &addr_arg_endp, 16);
		if (addr_arg_endp == argv[3] || *addr_arg_endp != '\0')
			return CMD_RET_USAGE;
	} else {
		addr_str = env_get("loadaddr");
		if (addr_str)
			load_address = simple_strtoul(addr_str, NULL, 16);
		else
			load_address = CONFIG_SYS_LOAD_ADDR;
	}

	/* Lookup the "misc" partition from argv[1] and argv[2] */
	misc_part_iface = argv[1];
	misc_part_desc = argv[2];
	/* Split the part_name if passed as "$dev_num;part_name". */
	if (part_get_info_by_dev_and_name(misc_part_iface, misc_part_desc,
					  &dev_desc, &part_info) < 0) {
		/* Couldn't lookup by name from mmc, try looking up the
		 * partition description directly.
		 */
		if (blk_get_device_part_str(misc_part_iface, misc_part_desc,
					    &dev_desc, &part_info, 1) < 0) {
			printf("Couldn't find partition %s %s\n",
			       misc_part_iface, misc_part_desc);
			return CMD_RET_FAILURE;
		}
	}

	ret = android_bootloader_boot_flow(dev_desc, &part_info, load_address);
	if (ret < 0) {
		printf("Android boot failed, error %d.\n", ret);
		return CMD_RET_FAILURE;
	}
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	boot_android, 4, 0, do_boot_android,
	"Execute the Android Bootloader flow.",
	"<interface> <dev[:part"
#if defined(CONFIG_EFI_PARTITION)
	/* When EFI is enabled we also support looking up a partition name. */
	"|;part_name"
#endif	/* CONFIG_EFI_PARTITION */
	"]> [<kernel_addr>]\n"
	"    - Load the Boot Control Block (BCB) from the partition 'part' on\n"
	"      device type 'interface' instance 'dev' to determine the boot\n"
	"      mode, and load and execute the appropriate kernel.\n"
	"      In normal and recovery mode, the kernel will be loaded from\n"
	"      the corresponding \"boot\" partition. In bootloader mode, the\n"
	"      command defined in the \"fastbootcmd\" variable will be\n"
	"      executed.\n"
#if defined(CONFIG_EFI_PARTITION)
	"    - If 'part_name' is passed, preceded with a ; instead of :, the\n"
	"      partition name whose label is 'part_name' will be looked up in\n"
	"      the partition table. This is commonly the \"misc\" partition.\n"
#endif	/* CONFIG_EFI_PARTITION */
);

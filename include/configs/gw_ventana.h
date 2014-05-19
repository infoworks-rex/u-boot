/*
 * Copyright (C) 2013 Gateworks Corporation
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "mx6_common.h"
#define CONFIG_MX6
#define CONFIG_DISPLAY_CPUINFO         /* display cpu info */
#define CONFIG_DISPLAY_BOARDINFO_LATE  /* display board info (after reloc) */

#define CONFIG_MACH_TYPE	4520   /* Gateworks Ventana Platform */

#include <asm/arch/imx-regs.h>
#include <asm/imx-common/gpio.h>

/* ATAGs */
#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_SERIAL_TAG
#define CONFIG_REVISION_TAG

#define CONFIG_SYS_GENERIC_BOARD

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(10 * 1024 * 1024)

/* Init Functions */
#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_MISC_INIT_R

/* GPIO */
#define CONFIG_MXC_GPIO

/* Serial */
#define CONFIG_MXC_UART
#define CONFIG_MXC_UART_BASE	       UART2_BASE

#ifdef CONFIG_SPI_FLASH

/* SPI */
#define CONFIG_CMD_SF
#ifdef CONFIG_CMD_SF
  #define CONFIG_MXC_SPI
  #define CONFIG_SPI_FLASH_MTD
  #define CONFIG_SPI_FLASH_BAR
  #define CONFIG_SPI_FLASH_WINBOND
  #define CONFIG_SF_DEFAULT_BUS              0
  #define CONFIG_SF_DEFAULT_CS               (0|(IMX_GPIO_NR(3, 19)<<8))
					     /* GPIO 3-19 (21248) */
  #define CONFIG_SF_DEFAULT_SPEED            30000000
  #define CONFIG_SF_DEFAULT_MODE             (SPI_MODE_0)
#endif

#else
/* Enable NAND support */
#define CONFIG_CMD_TIME
#define CONFIG_CMD_NAND
#define CONFIG_CMD_NAND_TRIMFFS
#ifdef CONFIG_CMD_NAND
  #define CONFIG_NAND_MXS
  #define CONFIG_SYS_MAX_NAND_DEVICE	1
  #define CONFIG_SYS_NAND_BASE		0x40000000
  #define CONFIG_SYS_NAND_5_ADDR_CYCLE
  #define CONFIG_SYS_NAND_ONFI_DETECTION

  /* DMA stuff, needed for GPMI/MXS NAND support */
  #define CONFIG_APBH_DMA
  #define CONFIG_APBH_DMA_BURST
  #define CONFIG_APBH_DMA_BURST8
#endif

#endif /* CONFIG_SPI_FLASH */

/* Flattened Image Tree Suport */
#define CONFIG_FIT
#define CONFIG_FIT_VERBOSE

/* I2C Configs */
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_SPEED		  100000

/* MMC Configs */
#define CONFIG_FSL_ESDHC
#define CONFIG_FSL_USDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR      0
#define CONFIG_SYS_FSL_USDHC_NUM       1
#define CONFIG_MMC
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_BOUNCE_BUFFER

/* Filesystem support */
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_CMD_UBIFS
#define CONFIG_DOS_PARTITION

/* Network config - Allow larger/faster download for TFTP/NFS */
#define CONFIG_IP_DEFRAG
#define CONFIG_TFTP_BLOCKSIZE 4096
#define CONFIG_NFS_READ_SIZE  4096

/*
 * SATA Configs
 */
#define CONFIG_CMD_SATA
#ifdef CONFIG_CMD_SATA
  #define CONFIG_DWC_AHSATA
  #define CONFIG_SYS_SATA_MAX_DEVICE	1
  #define CONFIG_DWC_AHSATA_PORT_ID	0
  #define CONFIG_DWC_AHSATA_BASE_ADDR	SATA_ARB_BASE_ADDR
  #define CONFIG_LBA48
  #define CONFIG_LIBATA
#endif

/*
 * PCI express
 */
#define CONFIG_CMD_PCI
#ifdef CONFIG_CMD_PCI
#define CONFIG_PCI
#define CONFIG_PCI_PNP
#define CONFIG_PCI_SCAN_SHOW
#define CONFIG_PCIE_IMX
#endif

/*
 * PMIC
 */
#define CONFIG_POWER
#define CONFIG_POWER_I2C
#define CONFIG_POWER_PFUZE100
#define CONFIG_POWER_PFUZE100_I2C_ADDR	0x08
#define CONFIG_POWER_LTC3676
#define CONFIG_POWER_LTC3676_I2C_ADDR  0x3c

/* Various command support */
#include <config_cmd_default.h>
#undef CONFIG_CMD_IMLS
#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_CMD_BMODE         /* set eFUSE shadow for a boot dev and reset */
#define CONFIG_CMD_HDMIDETECT    /* detect HDMI output device */
#define CONFIG_CMD_SETEXPR
#define CONFIG_CMD_BOOTZ
#define CONFIG_CMD_GSC
#define CONFIG_CMD_UBI
#define CONFIG_RBTREE
#define CONFIG_LZO
#define CONFIG_CMD_FUSE          /* eFUSE read/write support */
#ifdef CONFIG_CMD_FUSE
#define CONFIG_MXC_OCOTP
#endif


/* Ethernet support */
#define CONFIG_FEC_MXC
#define CONFIG_MII
#define IMX_FEC_BASE             ENET_BASE_ADDR
#define CONFIG_FEC_XCV_TYPE      RGMII
#define CONFIG_ETHPRIME          "FEC"
#define CONFIG_FEC_MXC_PHYADDR   0
#define CONFIG_PHYLIB
#define CONFIG_ARP_TIMEOUT       200UL

/* USB Configs */
#define CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_MX6
#define CONFIG_USB_STORAGE
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_ASIX
#define CONFIG_USB_ETHER_SMSC95XX
#define CONFIG_USB_MAX_CONTROLLER_COUNT 2
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET  /* For OTG port */
#define CONFIG_MXC_USB_PORTSC     (PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_MXC_USB_FLAGS      0
#define CONFIG_USB_KEYBOARD
#define CONFIG_CI_UDC
#define CONFIG_USBD_HS
#define CONFIG_USB_GADGET_DUALSPEED
#define CONFIG_USB_ETHER
#define CONFIG_USB_ETH_CDC
#define CONFIG_NETCONSOLE
#define CONFIG_SYS_USB_EVENT_POLL_VIA_CONTROL_EP

/* Framebuffer and LCD */
#define CONFIG_VIDEO
#define CONFIG_VIDEO_IPUV3
#define CONFIG_CFB_CONSOLE
#define CONFIG_VGA_AS_SINGLE_DEVICE
#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#define CONFIG_VIDEO_BMP_RLE8
#define CONFIG_SPLASH_SCREEN
#define CONFIG_BMP_16BPP
#define CONFIG_VIDEO_LOGO
#define CONFIG_IPUV3_CLK          260000000
#define CONFIG_CMD_HDMIDETECT
#define CONFIG_CONSOLE_MUX
#define CONFIG_IMX_HDMI
#define CONFIG_IMX_VIDEO_SKIP

/* serial console (ttymxc1,115200) */
#define CONFIG_CONS_INDEX              1
#define CONFIG_BAUDRATE                115200

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT	             "Ventana > "
#define CONFIG_SYS_CBSIZE	             1024
#define CONFIG_AUTO_COMPLETE
#define CONFIG_CMDLINE_EDITING
#define CONFIG_HWCONFIG

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS	           16
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE

/* Memory configuration */
#define CONFIG_SYS_MEMTEST_START       0x10000000
#define CONFIG_SYS_MEMTEST_END	       0x10010000
#define CONFIG_SYS_MEMTEST_SCRATCH     0x10800000
#define CONFIG_SYS_TEXT_BASE	         0x17800000
#define CONFIG_SYS_LOAD_ADDR           0x12000000

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS           1
#define PHYS_SDRAM                     MMDC0_ARB_BASE_ADDR
#define CONFIG_SYS_SDRAM_BASE          PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR       IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE       IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* FLASH and environment organization */
#define CONFIG_SYS_NO_FLASH  /* no NOR flash */

/*
 * MTD Command for mtdparts
 */
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#ifdef CONFIG_SPI_FLASH
#define MTDIDS_DEFAULT    "nor0=nor"
#define MTDPARTS_DEFAULT  \
	"mtdparts=nor:512k(uboot),64k(env),2m(kernel),-(rootfs)"
#else
#define MTDIDS_DEFAULT    "nand0=nand"
#define MTDPARTS_DEFAULT  "mtdparts=nand:16m(uboot),1m(env),-(rootfs)"
#endif

/* Persistent Environment Config */
#define CONFIG_ENV_OVERWRITE    /* allow to overwrite serial and ethaddr */
#ifdef CONFIG_SPI_FLASH
#define CONFIG_ENV_IS_IN_SPI_FLASH
#else
#define CONFIG_ENV_IS_IN_NAND
#endif
#if defined(CONFIG_ENV_IS_IN_MMC)
  #define CONFIG_ENV_OFFSET              (6 * 64 * 1024)
  #define CONFIG_ENV_SIZE                (8 * 1024)
  #define CONFIG_SYS_MMC_ENV_DEV         0
#elif defined(CONFIG_ENV_IS_IN_NAND)
  #define CONFIG_ENV_OFFSET              (16 << 20)
  #define CONFIG_ENV_SECT_SIZE           (128 << 10)
  #define CONFIG_ENV_SIZE                CONFIG_ENV_SECT_SIZE
  #define CONFIG_ENV_OFFSET_REDUND       (CONFIG_ENV_OFFSET + (512 << 10))
  #define CONFIG_ENV_SIZE_REDUND         CONFIG_ENV_SIZE
#elif defined(CONFIG_ENV_IS_IN_SPI_FLASH)
  #define CONFIG_ENV_OFFSET              (512 * 1024)
  #define CONFIG_ENV_SECT_SIZE           (64 * 1024)
  #define CONFIG_ENV_SIZE                (8 * 1024)
  #define CONFIG_ENV_SPI_BUS             CONFIG_SF_DEFAULT_BUS
  #define CONFIG_ENV_SPI_CS              CONFIG_SF_DEFAULT_CS
  #define CONFIG_ENV_SPI_MODE            CONFIG_SF_DEFAULT_MODE
  #define CONFIG_ENV_SPI_MAX_HZ          CONFIG_SF_DEFAULT_SPEED
#endif

/* Environment */
#define CONFIG_BOOTDELAY          3
#define CONFIG_LOADADDR           CONFIG_SYS_LOAD_ADDR
#define CONFIG_IPADDR             192.168.1.1
#define CONFIG_SERVERIP           192.168.1.146
#define HWCONFIG_DEFAULT \
	"hwconfig=rs232;" \
	"dio0:mode=gpio;dio1:mode=gpio;dio2:mode=gpio;dio3:mode=gpio\0" \

#define CONFIG_EXTRA_ENV_SETTINGS_COMMON \
	"console=ttymxc1\0" \
	"bootdevs=usb mmc sata flash\0" \
	HWCONFIG_DEFAULT \
	"video=\0" \
	\
	"mtdparts=" MTDPARTS_DEFAULT "\0" \
	"mtdids=" MTDIDS_DEFAULT "\0" \
	\
	"fdt_high=0xffffffff\0" \
	"fdt_addr=0x18000000\0" \
	"loadfdt=" \
		"if ${fsload} ${fdt_addr} boot/${fdt_file}; then " \
			"echo Loaded DTB from boot/${fdt_file}; " \
		"elif ${fsload} ${fdt_addr} boot/${fdt_file1}; then " \
			"echo Loaded DTB from boot/${fdt_file1}; " \
		"elif ${fsload} ${fdt_addr} boot/${fdt_file2}; then " \
				"echo Loaded DTB from boot/${fdt_file2}; " \
		"fi\0" \
	\
	"script=boot/6x_bootscript-ventana\0" \
	"loadscript=" \
		"if ${fsload} ${loadaddr} ${script}; then " \
			"source; " \
		"fi\0" \
	\
	"uimage=boot/uImage\0" \
	"mmc_root=/dev/mmcblk0p1 rootfstype=ext4 rootwait rw\0" \
	"mmc_boot=" \
		"setenv fsload 'ext2load mmc 0:1'; " \
		"mmc dev 0 && mmc rescan && " \
		"run loadscript; " \
		"if ${fsload} ${loadaddr} ${uimage}; then " \
			"setenv bootargs console=${console},${baudrate} " \
				"root=/dev/mmcblk0p1 rootfstype=ext4 " \
				"rootwait rw ${video} ${extra}; " \
			"if run loadfdt && fdt addr ${fdt_addr}; then " \
				"bootm ${loadaddr} - ${fdt_addr}; " \
			"else " \
				"bootm; " \
			"fi; " \
		"fi\0" \
	\
	"sata_boot=" \
		"setenv fsload 'ext2load sata 0:1'; sata init && " \
		"run loadscript; " \
		"if ${fsload} ${loadaddr} ${uimage}; then " \
			"setenv bootargs console=${console},${baudrate} " \
				"root=/dev/sda1 rootfstype=ext4 " \
				"rootwait rw ${video} ${extra}; " \
			"if run loadfdt && fdt addr ${fdt_addr}; then " \
				"bootm ${loadaddr} - ${fdt_addr}; " \
			"else " \
				"bootm; " \
			"fi; " \
		"fi\0" \
	"usb_boot=" \
		"setenv fsload 'ext2load usb 0:1'; usb start && usb dev 0 && " \
		"run loadscript; " \
		"if ${fsload} ${loadaddr} ${uimage}; then " \
			"setenv bootargs console=${console},${baudrate} " \
				"root=/dev/sda1 rootfstype=ext4 " \
				"rootwait rw ${video} ${extra}; " \
			"if run loadfdt && fdt addr ${fdt_addr}; then " \
				"bootm ${loadaddr} - ${fdt_addr}; " \
			"else " \
				"bootm; " \
			"fi; " \
		"fi\0"

#ifdef CONFIG_SPI_FLASH
	#define CONFIG_EXTRA_ENV_SETTINGS \
	CONFIG_EXTRA_ENV_SETTINGS_COMMON \
	"image_os=ventana/openwrt-imx6-imx6q-gw5400-a-squashfs.bin\0" \
	"image_uboot=ventana/u-boot_spi.imx\0" \
	\
	"spi_koffset=0x90000\0" \
	"spi_klen=0x200000\0" \
	\
	"spi_updateuboot=echo Updating uboot from " \
		"${serverip}:${image_uboot}...; " \
		"tftpboot ${loadaddr} ${image_uboot} && " \
		"sf probe && sf erase 0 80000 && " \
			"sf write ${loadaddr} 400 ${filesize}\0" \
	"spi_update=echo Updating OS from ${serverip}:${image_os} " \
		"to ${spi_koffset} ...; " \
		"tftp ${loadaddr} ${image_os} && " \
		"sf probe && " \
		"sf update ${loadaddr} ${spi_koffset} ${filesize}\0" \
	\
	"flash_boot=" \
		"if sf probe && " \
		"sf read ${loadaddr} ${spi_koffset} ${spi_klen}; then " \
			"setenv bootargs console=${console},${baudrate} " \
				"root=/dev/mtdblock3 " \
				"rootfstype=squashfs,jffs2 " \
				"${video} ${extra}; " \
			"bootm; " \
		"fi\0"
#else
	#define CONFIG_EXTRA_ENV_SETTINGS \
	CONFIG_EXTRA_ENV_SETTINGS_COMMON \
	"image_rootfs=openwrt-imx6-ventana-rootfs.ubi\0" \
	\
	"nand_update=echo Updating NAND from ${serverip}:${image_rootfs}...; " \
		"tftp ${loadaddr} ${image_rootfs} && " \
		"nand erase.part rootfs && " \
		"nand write ${loadaddr} rootfs ${filesize}\0" \
	\
	"flash_boot=" \
		"setenv fsload 'ubifsload'; " \
		"ubi part rootfs && ubifsmount ubi0:rootfs; " \
		"run loadscript; " \
		"if ${fsload} ${loadaddr} ${uimage}; then " \
			"setenv bootargs console=${console},${baudrate} " \
				"root=ubi0:rootfs ubi.mtd=2 " \
				"rootfstype=ubifs ${video} ${extra}; " \
			"if run loadfdt && fdt addr ${fdt_addr}; then " \
				"ubifsumount; " \
				"bootm ${loadaddr} - ${fdt_addr}; " \
			"else " \
				"ubifsumount; bootm; " \
			"fi; " \
		"fi\0"
#endif

#define CONFIG_BOOTCOMMAND \
	"for btype in ${bootdevs}; do " \
		"echo; echo Attempting ${btype} boot...; " \
		"if run ${btype}_boot; then; fi; " \
	"done"

/* Device Tree Support */
#define CONFIG_OF_BOARD_SETUP
#define CONFIG_OF_LIBFDT
#define CONFIG_FDT_FIXUP_PARTITIONS

#ifndef CONFIG_SYS_DCACHE_OFF
  #define CONFIG_CMD_CACHE
#endif

#endif			       /* __CONFIG_H */

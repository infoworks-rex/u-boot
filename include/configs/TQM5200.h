/*
 * (C) Copyright 2003-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2004
 * Martin Krause, TQ-Systems GmbH, martin.krause@tqs.de
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */

#define CONFIG_MPC5xxx		1	/* This is an MPC5xxx CPU */
#define CONFIG_MPC5200		1	/* (more precisely an MPC5200 CPU) */
#define CONFIG_TQM5200		1	/* ... on TQM5200 module */
#define CONFIG_STK52XX		1	/* ... on a STK52XX base board */

#define CFG_MPC5XXX_CLKIN	33000000 /* ... running at 33.000000MHz */

#define BOOTFLAG_COLD		0x01	/* Normal Power-On: Boot from FLASH  */
#define BOOTFLAG_WARM		0x02	/* Software reboot	     */

#define CFG_CACHELINE_SIZE	32	/* For MPC5xxx CPUs */
#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#  define CFG_CACHELINE_SHIFT	5	/* log base 2 of the above value */
#endif

/*
 * Serial console configuration
 */
#define CONFIG_PSC_CONSOLE	1	/* console is on PSC1 */
#define CONFIG_BAUDRATE		115200	/* ... at 115200 bps */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400 }

#ifdef CONFIG_STK52XX
#define CONFIG_PS2KBD			/* AT-PS/2 Keyboard		*/
#define CONFIG_PS2MULT			/* .. on PS/2 Multiplexer	*/
#define CONFIG_PS2SERIAL	6	/* .. on PSC6			*/
#define CONFIG_PS2MULT_DELAY	(CFG_HZ/2)	/* Initial delay	*/
#define CONFIG_BOARD_EARLY_INIT_R
#endif /* CONFIG_STK52XX */

#ifdef CONFIG_MPC5200	/* MPC5100 PCI is not supported yet. */
/*
 * PCI Mapping:
 * 0x40000000 - 0x4fffffff - PCI Memory
 * 0x50000000 - 0x50ffffff - PCI IO Space
 */
#ifdef CONFIG_STK52XX
#define CONFIG_PCI		1
#elif
#define CONFIG_PCI		0
#endif
#define CONFIG_PCI_PNP		1
/* #define CONFIG_PCI_SCAN_SHOW	1 */

#define CONFIG_PCI_MEM_BUS	0x40000000
#define CONFIG_PCI_MEM_PHYS	CONFIG_PCI_MEM_BUS
#define CONFIG_PCI_MEM_SIZE	0x10000000

#define CONFIG_PCI_IO_BUS	0x50000000
#define CONFIG_PCI_IO_PHYS	CONFIG_PCI_IO_BUS
#define CONFIG_PCI_IO_SIZE	0x01000000

#define CONFIG_NET_MULTI	1
#define CONFIG_EEPRO100		1
#define CFG_RX_ETH_BUFFER	8  /* use 8 rx buffer on eepro100  */
#define CONFIG_NS8382X		1

#ifdef CONFIG_STK52XX
#define ADD_PCI_CMD		CFG_CMD_PCI
#elif
#define ADD_PCI_CMD		0
#endif

#else	/* MPC5100 */

#define ADD_PCI_CMD		0  /* no CFG_CMD_PCI */

#endif

/* Partitions */
#undef CONFIG_MAC_PARTITION
#if defined (CONFIG_MINIFAP)
#define CONFIG_DOS_PARTITION
#endif

/* USB */
#ifdef CONFIG_STK52XX
#define CONFIG_USB_OHCI
#define ADD_USB_CMD		CFG_CMD_USB | CFG_CMD_FAT
#define CONFIG_DOS_PARTITION
#define CONFIG_USB_STORAGE
#else
#define ADD_USB_CMD		0
#endif

/* POST support */
#define CONFIG_POST		(CFG_POST_MEMORY   | \
				 CFG_POST_CPU	   | \
				 CFG_POST_I2C)

#ifdef CONFIG_POST
#define CFG_CMD_POST_DIAG CFG_CMD_DIAG
/* preserve space for the post_word at end of on-chip SRAM */
#define MPC5XXX_SRAM_POST_SIZE MPC5XXX_SRAM_SIZE-4
#else
#define CFG_CMD_POST_DIAG 0
#endif

/* IDE */
#if defined (CONFIG_MINIFAP) || defined (CONFIG_STK52XX)
#define ADD_IDE_CMD		CFG_CMD_IDE | CFG_CMD_FAT
#else
#define ADD_IDE_CMD		0
#endif

/*
 * Supported commands
 */
#define CONFIG_COMMANDS	       (CONFIG_CMD_DFL	| \
				CFG_CMD_EEPROM	| \
				CFG_CMD_I2C	| \
				ADD_PCI_CMD	| \
				ADD_USB_CMD	| \
				CFG_CMD_POST_DIAG | \
				CFG_CMD_DATE	| \
				CFG_CMD_REGINFO | \
				CFG_CMD_MII	| \
				CFG_CMD_PING	| \
				ADD_IDE_CMD)

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#if (TEXT_BASE == 0xFC000000)		/* Boot low */
#   define CFG_LOWBOOT		1
#endif

/*
 * Autobooting
 */
#define CONFIG_BOOTDELAY	5	/* autoboot after 5 seconds */

#define CONFIG_PREBOOT	"echo;" \
	"echo Type \"run flash_nfs\" to mount root filesystem over NFS;" \
	"echo"

#undef	CONFIG_BOOTARGS

#if defined (CONFIG_TQM5200_AA)
#define CONFIG_EXTRA_ENV_SETTINGS					\
	"netdev=eth0\0"							\
	"nfsargs=setenv bootargs root=/dev/nfs rw "			\
		"nfsroot=$(serverip):$(rootpath)\0"			\
	"ramargs=setenv bootargs root=/dev/ram rw\0"			\
	"addip=setenv bootargs $(bootargs) "				\
		"ip=$(ipaddr):$(serverip):$(gatewayip):$(netmask)"	\
		":$(hostname):$(netdev):off panic=1\0"			\
	"flash_nfs=run nfsargs addip;"					\
		"bootm $(kernel_addr)\0"				\
	"flash_self=run ramargs addip;"					\
		"bootm $(kernel_addr) $(ramdisk_addr)\0"		\
	"net_nfs=tftp 200000 $(bootfile);run nfsargs addip;bootm\0"	\
	"rootpath=/opt/eldk3.0_ppc/ppc_82xx\0"				\
	"bootfile=uImage_tqm5200_mkr\0"					\
	"load=tftp 200000 $(loadfile)\0"				\
	"load133=tftp 200000 $(loadfile133)\0"				\
	"loadfile=u-boot_tqm5200_aa_mkr.bin\0"				\
	"loadfile133=u-boot_tqm5200_aa_133_mkr.bin\0"			\
	"update=protect off 1:0-4; erase 1:0-4; cp.b 200000 0xfc000000 $(filesize); protect on 1:0-4\0"		\
	"serverip=172.20.5.13\0"					\
	""
#else
#if defined (CONFIG_TQM5200_AB)
#define CONFIG_EXTRA_ENV_SETTINGS					\
	"netdev=eth0\0"							\
	"nfsargs=setenv bootargs root=/dev/nfs rw "			\
		"nfsroot=$(serverip):$(rootpath)\0"			\
	"ramargs=setenv bootargs root=/dev/ram rw\0"			\
	"addip=setenv bootargs $(bootargs) "				\
		"ip=$(ipaddr):$(serverip):$(gatewayip):$(netmask)"	\
		":$(hostname):$(netdev):off panic=1\0"			\
	"flash_nfs=run nfsargs addip;"					\
		"bootm $(kernel_addr)\0"				\
	"flash_self=run ramargs addip;"					\
		"bootm $(kernel_addr) $(ramdisk_addr)\0"		\
	"net_nfs=tftp 200000 $(bootfile);run nfsargs addip;bootm\0"	\
	"rootpath=/opt/eldk3.0_ppc/ppc_82xx\0"				\
	"bootfile=uImage_tqm5200_mkr\0"					\
	"load=tftp 200000 $(loadfile)\0"				\
	"load133=tftp 200000 $(loadfile133)\0"				\
	"loadfile=u-boot_tqm5200_ab_mkr.bin\0"				\
	"loadfile133=u-boot_tqm5200_ab_133_mkr.bin\0"			\
	"update=protect off 1:0-1; erase 1:0-1; cp.b 200000 0xfc000000 $(filesize); protect on 1:0-1\0"		\
	"serverip=172.20.5.13\0"					\
	""
#else
#if defined (CONFIG_TQM5200_AC)
#define CONFIG_EXTRA_ENV_SETTINGS					\
	"netdev=eth0\0"							\
	"nfsargs=setenv bootargs root=/dev/nfs rw "			\
		"nfsroot=$(serverip):$(rootpath)\0"			\
	"ramargs=setenv bootargs root=/dev/ram rw\0"			\
	"addip=setenv bootargs $(bootargs) "				\
		"ip=$(ipaddr):$(serverip):$(gatewayip):$(netmask)"	\
		":$(hostname):$(netdev):off panic=1\0"			\
	"flash_nfs=run nfsargs addip;"					\
		"bootm $(kernel_addr)\0"				\
	"flash_self=run ramargs addip;"					\
		"bootm $(kernel_addr) $(ramdisk_addr)\0"		\
	"net_nfs=tftp 200000 $(bootfile);run nfsargs addip;bootm\0"	\
	"rootpath=/opt/eldk3.0_ppc/ppc_82xx\0"				\
	"bootfile=uImage_tqm5200_mkr\0"					\
	"load=tftp 200000 $(loadfile)\0"				\
	"load133=tftp 200000 $(loadfile133)\0"				\
	"loadfile=u-boot_tqm5200_ac_mkr.bin\0"				\
	"loadfile133=u-boot_tqm5200_ac_133_mkr.bin\0"			\
	"update=protect off 1:0-4; erase 1:0-4; cp.b 200000 0xfc000000 $(filesize); protect on 1:0-4\0"		\
	"serverip=172.20.5.13\0"					\
	""
#else
#define CONFIG_EXTRA_ENV_SETTINGS					\
	"netdev=eth0\0"							\
	"nfsargs=setenv bootargs root=/dev/nfs rw "			\
		"nfsroot=$(serverip):$(rootpath)\0"			\
	"ramargs=setenv bootargs root=/dev/ram rw\0"			\
	"addip=setenv bootargs $(bootargs) "				\
		"ip=$(ipaddr):$(serverip):$(gatewayip):$(netmask)"	\
		":$(hostname):$(netdev):off panic=1\0"			\
	"flash_nfs=run nfsargs addip;"					\
		"bootm $(kernel_addr)\0"				\
	"flash_self=run ramargs addip;"					\
		"bootm $(kernel_addr) $(ramdisk_addr)\0"		\
	"net_nfs=tftp 200000 $(bootfile);run nfsargs addip;bootm\0"	\
	"rootpath=/opt/eldk3.0_ppc/ppc_82xx\0"				\
	"bootfile=uImage_tqm5200_mkr\0"					\
	"load=tftp 200000 $(loadfile)\0"				\
	"load133=tftp 200000 $(loadfile133)\0"				\
	"loadfile=u-boot_tqm5200_mkr.bin\0"				\
	"loadfile133=u-boot_tqm5200_133_mkr.bin\0"			\
	"update=protect off fc000000 fc03ffff; erase fc000000 fc03ffff; cp.b 200000 0xfc000000 $(filesize); protect on fc000000 fc03ffff\0"		\
	"serverip=172.20.5.13\0"					\
	""
#endif
#endif
#endif

#define CONFIG_BOOTCOMMAND	"run net_nfs"

/*
 * IPB Bus clocking configuration.
 */
#define CFG_IPBSPEED_133		/* define for 133MHz speed */

#if defined(CFG_IPBSPEED_133)
/*
 * PCI Bus clocking configuration
 *
 * Actually a PCI Clock of 66 MHz is only set (in cpu_init.c) if
 * CFG_IPBSPEED_133 is defined. This is because a PCI Clock of 66 MHz yet hasn't
 * been tested with a IPB Bus Clock of 66 MHz.
 */
#define CFG_PCISPEED_66			/* define for 66MHz speed */
#endif

/*
 * I2C configuration
 */
#define CONFIG_HARD_I2C		1	/* I2C with hardware support */
#if defined (CONFIG_MINIFAP)
#define CFG_I2C_MODULE		2	/* Select I2C module #1 or #2 */
#else
#define CFG_I2C_MODULE		1	/* Select I2C module #1 or #2 */
#endif

/*
 * I2C clock frequency
 *
 * Please notice, that the resulting clock frequency could differ from the
 * configured value. This is because the I2C clock is derived from system
 * clock over a frequency divider with only a few divider values. U-boot
 * calculates the best approximation for CFG_I2C_SPEED. However the calculated
 * approximation allways lies below the configured value, never above.
 */
#define CFG_I2C_SPEED		100000 /* 100 kHz */
#define CFG_I2C_SLAVE		0x7F

/*
 * EEPROM configuration for onboard EEPROM M24C32 (M24C64 should work
 * also). For other EEPROMs configuration should be verified. On Mini-FAP the
 * EEPROM (24C64) is on the same I2C address (but on other I2C bus), so the
 * same configuration could be used.
 */
#define CFG_I2C_EEPROM_ADDR		0x50	/* 1010000x */
#define CFG_I2C_EEPROM_ADDR_LEN		2
#define CFG_EEPROM_PAGE_WRITE_BITS	5	/* =32 Bytes per write */
#define CFG_EEPROM_PAGE_WRITE_DELAY_MS	20

/*
 * HW-Monitor configuration on Mini-FAP
 */
#if defined (CONFIG_MINIFAP)
#define CFG_I2C_HWMON_ADDR		0x2C
#endif

/* List of I2C addresses to be verified by POST */
#if defined (CONFIG_TQM5200_AA) || defined (CONFIG_TQM5200_AB)
#define I2C_ADDR_LIST	{	CFG_I2C_EEPROM_ADDR,	\
				CFG_I2C_SLAVE }
#elif defined (CONFIG_TQM5200_AC)
#define I2C_ADDR_LIST	{	CFG_I2C_SLAVE }
#endif

#if defined (CONFIG_MINIFAP)
#undef I2C_ADDR_LIST
#define I2C_ADDR_LIST	{	CFG_I2C_EEPROM_ADDR,	\
				CFG_I2C_HWMON_ADDR,	\
				CFG_I2C_SLAVE }
#endif

/*
 * Flash configuration
 */
#define CFG_FLASH_BASE		TEXT_BASE /* 0xFC000000 */

/* use CFI flash driver if no module variant is spezified */
#define CFG_FLASH_CFI		1	/* Flash is CFI conformant */
#define CFG_FLASH_CFI_DRIVER	1	/* Use the common driver */
#define CFG_FLASH_BANKS_LIST	{ CFG_BOOTCS_START }
#define CFG_FLASH_EMPTY_INFO
#define CFG_FLASH_SIZE		0x02000000 /* 32 MByte */
#define CFG_MAX_FLASH_SECT	256	/* max num of sects on one chip */

#if !defined(CFG_LOWBOOT)
#define CFG_ENV_ADDR		(CFG_FLASH_BASE + 0x00740000 + 0x00800000)
#else	/* CFG_LOWBOOT */
#define CFG_ENV_ADDR		(CFG_FLASH_BASE + 0x00040000)
#endif	/* CFG_LOWBOOT */
#define CFG_MAX_FLASH_BANKS	1	/* max num of flash banks
					   (= chip selects) */
#define CFG_FLASH_ERASE_TOUT	240000	/* Flash Erase Timeout (in ms)	*/
#define CFG_FLASH_WRITE_TOUT	500	/* Flash Write Timeout (in ms)	*/


/*
 * Environment settings
 */
#define CFG_ENV_IS_IN_FLASH	1
#define CFG_ENV_SIZE		0x10000
#define CFG_ENV_SECT_SIZE	0x20000
#define CONFIG_ENV_OVERWRITE	1

/*
 * Memory map
 */
#define CFG_MBAR		0xF0000000
#define CFG_SDRAM_BASE		0x00000000
#define CFG_DEFAULT_MBAR	0x80000000

/* Use ON-Chip SRAM until RAM will be available */
#define CFG_INIT_RAM_ADDR	MPC5XXX_SRAM
#ifdef CONFIG_POST
/* preserve space for the post_word at end of on-chip SRAM */
#define CFG_INIT_RAM_END	MPC5XXX_SRAM_POST_SIZE
#else
#define CFG_INIT_RAM_END	MPC5XXX_SRAM_SIZE
#endif


#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */
#define CFG_GBL_DATA_OFFSET	(CFG_INIT_RAM_END - CFG_GBL_DATA_SIZE)
#define CFG_INIT_SP_OFFSET	CFG_GBL_DATA_OFFSET

#define CFG_MONITOR_BASE    TEXT_BASE
#if (CFG_MONITOR_BASE < CFG_FLASH_BASE)
#   define CFG_RAMBOOT		1
#endif

#define CFG_MONITOR_LEN		(192 << 10)	/* Reserve 192 kB for Monitor	*/
#define CFG_MALLOC_LEN		(128 << 10)	/* Reserve 128 kB for malloc()	*/
#define CFG_BOOTMAPSZ		(8 << 20)	/* Initial Memory map for Linux */

/*
 * Ethernet configuration
 */
#define CONFIG_MPC5xxx_FEC	1
/*
 * Define CONFIG_FEC_10MBIT to force FEC at 10Mb
 */
/* #define CONFIG_FEC_10MBIT 1 */
#define CONFIG_PHY_ADDR		0x00

/*
 * GPIO configuration
 *
 * use pin gpio_wkup_6 as second SDRAM chip select (mem_cs1):
 *	Bit 0 (mask: 0x80000000): 1
 * use ALT CAN position: Bits 2-3 (mask: 0x30000000):
 *	00 -> No Alternatives, I2C1 is used for onboard EEPROM
 *	01 -> CAN1 on I2C1, CAN2 on Tmr0/1 do not use on TQM5200 with onboard
 *	      EEPROM
 * use PSC1 as UART: Bits 28-31 (mask: 0x00000007): 0100
 * use PSC6:
 *   on STK52xx:
 *      use as UART. Pins PSC6_0 to PSC6_3 are used.
        Bits 9:11 (mask: 0x00700000):
 *	   101 -> PSC6 : Extended POST test is not available
 *   on MINI-FAP and TQM5200_IB:
 *      use PSC6_1 and PSC6_3 as GPIO: Bits 9:11 (mask: 0x00700000):
 *	   011 -> PSC6 could not be used as UART or CODEC. IrDA still possible.
 * GPIO on PSC6_3 is used in post_hotkeys_pressed() to enable extended POST
 * tests.
 */
#if defined (CONFIG_MINIFAP)
#define CFG_GPS_PORT_CONFIG	0x91300004
#elif defined (CONFIG_STK52XX)
#define CFG_GPS_PORT_CONFIG	0x81500004
#else
#define CFG_GPS_PORT_CONFIG	0x81300004
#endif

/*
 * RTC configuration
 */
#define CONFIG_RTC_MPC5200	1	/* use internal MPC5200 RTC */

/*
 * Miscellaneous configurable options
 */
#define CFG_LONGHELP			/* undef to save memory	    */
#define CFG_PROMPT		"=> "	/* Monitor Command Prompt   */
#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CFG_CBSIZE		1024	/* Console I/O Buffer Size  */
#else
#define CFG_CBSIZE		256	/* Console I/O Buffer Size  */
#endif
#define CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define CFG_MAXARGS		16	/* max number of command args	*/
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size	*/

/* Enable an alternate, more extensive memory test */
#define CFG_ALT_MEMTEST

#define CFG_MEMTEST_START	0x00100000	/* memtest works on */
#define CFG_MEMTEST_END		0x00f00000	/* 1 ... 15 MB in DRAM	*/

#define CFG_LOAD_ADDR		0x100000	/* default load address */

#define CFG_HZ			1000	/* decrementer freq: 1 ms ticks */

/*
 * Enable loopw commando. This has only affect, if CFG_CMD_MEM is defined,
 * which is normally part of the default commands (CFV_CMD_DFL)
 */
#define CONFIG_LOOPW

/*
 * Various low-level settings
 */
#if defined(CONFIG_MPC5200)
#define CFG_HID0_INIT		HID0_ICE | HID0_ICFI
#define CFG_HID0_FINAL		HID0_ICE
#else
#define CFG_HID0_INIT		0
#define CFG_HID0_FINAL		0
#endif

#define CFG_BOOTCS_START	CFG_FLASH_BASE
#define CFG_BOOTCS_SIZE		CFG_FLASH_SIZE
#ifdef CFG_PCISPEED_66
#define CFG_BOOTCS_CFG		0x0008DF30 /* for pci_clk  = 66 MHz */
#else
#define CFG_BOOTCS_CFG		0x0004DF30 /* for pci_clk = 33 MHz */
#endif
#define CFG_CS0_START		CFG_FLASH_BASE
#define CFG_CS0_SIZE		CFG_FLASH_SIZE

/* automatic configuration of chip selects */
#ifdef CONFIG_CS_AUTOCONF
#define CONFIG_LAST_STAGE_INIT
#endif

/*
 * SRAM - Do not map below 2 GB in address space, because this area is used
 * for SDRAM autosizing.
 */
#if defined CONFIG_TQM5200_AB || defined (CONFIG_CS_AUTOCONF)
#define CFG_CS2_START		0xE5000000
#ifdef CONFIG_TQM5200_AB
#define CFG_CS2_SIZE		0x80000		/* 512 kByte */
#else  /* CONFIG_CS_AUTOCONF */
#define CFG_CS2_SIZE		0x100000	/* 1 MByte */
#endif
#define CFG_CS2_CFG		0x0004D930
#endif

/*
 * Grafic controller - Do not map below 2 GB in address space, because this
 * area is used for SDRAM autosizing.
 */
#if defined (CONFIG_TQM5200_AB) || defined (CONFIG_TQM5200_AC) || \
    defined (CONFIG_CS_AUTOCONF)
#define CFG_CS1_START		0xE0000000
#define CFG_CS1_SIZE		0x4000000	/* 64 MByte */
#define CFG_CS1_CFG		0x8F48FF70
#define SM501_MMIO_BASE		CFG_CS1_START + 0x03E00000
#endif

#define CFG_CS_BURST		0x00000000
#define CFG_CS_DEADCYCLE	0x33333333

#define CFG_RESET_ADDRESS	0xff000000

/*-----------------------------------------------------------------------
 * USB stuff
 *-----------------------------------------------------------------------
 */
#define CONFIG_USB_CLOCK	0x0001BBBB
#define CONFIG_USB_CONFIG	0x00001000

/*-----------------------------------------------------------------------
 * IDE/ATA stuff Supports IDE harddisk
 *-----------------------------------------------------------------------
 */

#undef	CONFIG_IDE_8xx_PCCARD		/* Use IDE with PC Card Adapter */

#undef	CONFIG_IDE_8xx_DIRECT		/* Direct IDE	 not supported	*/
#undef	CONFIG_IDE_LED			/* LED	 for ide not supported	*/

#define CONFIG_IDE_RESET		/* reset for ide supported	*/
#define CONFIG_IDE_PREINIT

#define CFG_IDE_MAXBUS		1	/* max. 1 IDE bus		*/
#define CFG_IDE_MAXDEVICE	1	/* max. 1 drive per IDE bus	*/

#define CFG_ATA_IDE0_OFFSET	0x0000

#define CFG_ATA_BASE_ADDR	MPC5XXX_ATA

/* Offset for data I/O			*/
#define CFG_ATA_DATA_OFFSET	(0x0060)

/* Offset for normal register accesses	*/
#define CFG_ATA_REG_OFFSET	(CFG_ATA_DATA_OFFSET)

/* Offset for alternate registers	*/
#define CFG_ATA_ALT_OFFSET	(0x005C)

/* Interval between registers						     */
#define CFG_ATA_STRIDE		4

#endif /* __CONFIG_H */

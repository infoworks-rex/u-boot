/*
 * (C) Copyright 2008
 * Heiko Schocher, DENX Software Engineering, hs@denx.de.
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

#define CONFIG_8260		1
#define CONFIG_MPC8260		1
#define CONFIG_MUAS3001		1

#define CONFIG_CPM2		1	/* Has a CPM2 */

/* Do boardspecific init */
#define CONFIG_BOARD_EARLY_INIT_R       1

/* enable Watchdog */
#define CONFIG_WATCHDOG		1

/*
 * Select serial console configuration
 *
 * If either CONFIG_CONS_ON_SMC or CONFIG_CONS_ON_SCC is selected, then
 * CONFIG_CONS_INDEX must be set to the channel number (1-2 for SMC, 1-4
 * for SCC).
 */
#define	CONFIG_CONS_ON_SMC		/* Console is on SMC         */
#undef  CONFIG_CONS_ON_SCC		/* It's not on SCC           */
#undef	CONFIG_CONS_NONE		/* It's not on external UART */
#if defined(CONFIG_MUAS_DEV_BOARD)
#define CONFIG_CONS_INDEX	2	/* SMC2 is used for console  */
#else
#define CONFIG_CONS_INDEX	1	/* SMC1 is used for console  */
#endif

/*
 * Select ethernet configuration
 *
 * If either CONFIG_ETHER_ON_SCC or CONFIG_ETHER_ON_FCC is selected,
 * then CONFIG_ETHER_INDEX must be set to the channel number (1-4 for
 * SCC, 1-3 for FCC)
 *
 * If CONFIG_ETHER_NONE is defined, then either the ethernet routines
 * must be defined elsewhere (as for the console), or CONFIG_CMD_NET
 * must be unset.
 */
#undef	CONFIG_ETHER_ON_SCC		/* Ethernet is not on SCC */
#define	CONFIG_ETHER_ON_FCC		/* Ethernet is on FCC     */
#undef	CONFIG_ETHER_NONE		/* No external Ethernet   */

#define CONFIG_ETHER_INDEX	1
#define CONFIG_ETHER_ON_FCC1
#define FCC_ENET

/*
 * - Rx-CLK is CLK11
 * - Tx-CLK is CLK12
 */
# define CFG_CMXFCR_VALUE	(CMXFCR_RF1CS_CLK11 | CMXFCR_TF1CS_CLK12)
# define CFG_CMXFCR_MASK	(CMXFCR_FC1 | CMXFCR_RF1CS_MSK | CMXFCR_TF1CS_MSK)
/*
 * - RAM for BD/Buffers is on the 60x Bus (see 28-13)
 */
# define CFG_CPMFCR_RAMTYPE	(0)
/* know on local Bus */
/* define CFG_CPMFCR_RAMTYPE	(CPMFCR_DTB | CPMFCR_BDB) */
/*
 * - Enable Full Duplex in FSMR
 */
# define CFG_FCC_PSMR		(FCC_PSMR_FDE|FCC_PSMR_LPB)

#define CONFIG_MII			/* MII PHY management		*/
#define CONFIG_BITBANGMII		/* bit-bang MII PHY management	*/
# define CFG_PHY_ADDR		1
/*
 * GPIO pins used for bit-banged MII communications
 */
#define MDIO_PORT	0		/* Port A */

#define CFG_MDIO_PIN	0x00200000	/* PA10 */
#define CFG_MDC_PIN	0x00400000	/* PA9  */

#define MDIO_ACTIVE	(iop->pdir |=  CFG_MDIO_PIN)
#define MDIO_TRISTATE	(iop->pdir &= ~CFG_MDIO_PIN)
#define MDIO_READ	((iop->pdat &  CFG_MDIO_PIN) != 0)

#define MDIO(bit)	if(bit) iop->pdat |=  CFG_MDIO_PIN; \
			else	iop->pdat &= ~CFG_MDIO_PIN

#define MDC(bit)	if(bit) iop->pdat |=  CFG_MDC_PIN; \
			else	iop->pdat &= ~CFG_MDC_PIN

#define MIIDELAY	udelay(1)

#ifndef CONFIG_8260_CLKIN
#define CONFIG_8260_CLKIN	66000000	/* in Hz */
#endif

#define CONFIG_BAUDRATE		115200

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_DTT
#define CONFIG_CMD_ECHO
#define CONFIG_CMD_IMMAP
#define CONFIG_CMD_MII
#define CONFIG_CMD_PING
#define CONFIG_CMD_I2C

/*
 * Default environment settings
 */
#define CONFIG_EXTRA_ENV_SETTINGS						\
	"netdev=eth0\0"								\
	"u-boot_addr_r=100000\0"						\
	"kernel_addr_r=200000\0"						\
	"fdt_addr_r=400000\0"							\
	"rootpath=/opt/eldk/ppc_6xx\0"						\
	"u-boot=muas3001/u-boot.bin\0"						\
	"bootfile=muas3001/uImage\0"						\
	"fdt_file=muas3001/muas3001.dtb\0"					\
	"ramdisk_file=uRamdisk\0"						\
	"load=tftp ${u-boot_addr_r} ${u-boot}\0"				\
	"update=prot off ff000000 ff03ffff; era ff000000 ff03ffff; "		\
		"cp.b ${u-boot_addr_r} ff000000 ${filesize};"			\
		"prot on ff000000 ff03ffff\0"					\
	"ramargs=setenv bootargs root=/dev/ram rw\0"				\
	"nfsargs=setenv bootargs root=/dev/nfs rw "				\
		"nfsroot=${serverip}:${rootpath}\0"				\
	"addcons=setenv bootargs ${bootargs} console=ttyCPM0,${baudrate}\0"	\
	"addmtd=setenv bootargs ${bootargs} ${mtdparts}\0"			\
	"addip=setenv bootargs ${bootargs} "					\
		"ip=${ipaddr}:${serverip}:${gatewayip}:"			\
		"${netmask}:${hostname}:${netdev}:off panic=1\0"		\
	"net_nfs=tftp ${kernel_addr_r} ${bootfile}; "				\
		"tftp ${fdt_addr_r} ${fdt_file}; run nfsargs addip addcons;"	\
		"bootm ${kernel_addr_r} - ${fdt_addr_r}\0"			\
	"net_self=tftp ${kernel_addr_r} ${bootfile}; "				\
		"tftp ${fdt_addr_r} ${fdt_file}; "				\
		"tftp ${ramdisk_addr} ${ramdisk_file}; "			\
		"run ramargs addip; "						\
		"bootm ${kernel_addr_r} ${ramdisk_addr} ${fdt_addr_r}\0"	\
	"ramdisk_addr=ff210000\0"						\
	"kernel_addr=ff050000\0"						\
	"fdt_addr=ff200000\0"							\
	"flash_self=run ramargs addip addcons;bootm ${kernel_addr}"		\
	" ${ramdisk_addr} ${fdt_addr}\0"					\
	"updateramdisk=era ${ramdisk_addr} +1f0000;tftpb ${kernel_addr_r}"	\
	" ${ramdisk_file};"							\
	"cp.b ${kernel_addr_r} ${ramdisk_addr} ${filesize}\0"			\
	"updatekernel=era ${kernel_addr} +1b0000;tftpb ${kernel_addr_r}"	\
	" ${bootfile};"								\
	"cp.b ${kernel_addr_r} ${kernel_addr} ${filesize}\0"			\
	"updatefdt=era ${fdt_addr} +10000;tftpb ${fdt_addr_r} ${fdt_file};"	\
	"cp.b ${fdt_addr_r} ${fdt_addr} ${filesize}\0"				\
	""

#define CONFIG_BOOTCOMMAND	"run net_nfs"
#define CONFIG_BOOTDELAY	5	/* autoboot after 5 seconds */

/*
 * Miscellaneous configurable options
 */
#define CFG_HUSH_PARSER
#define CFG_PROMPT_HUSH_PS2	"> "
#define CFG_LONGHELP			/* undef to save memory	    */
#define CFG_PROMPT		"=> "	/* Monitor Command Prompt   */
#if defined(CONFIG_CMD_KGDB)
#define CFG_CBSIZE		1024	/* Console I/O Buffer Size  */
#else
#define CFG_CBSIZE		256	/* Console I/O Buffer Size  */
#endif
#define CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16)	/* Print Buffer Size  */
#define CFG_MAXARGS		16		/* max number of command args */
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size  */

#define CFG_MEMTEST_START	0x00100000	/* memtest works on */
#define CFG_MEMTEST_END		0x00f00000	/* 1 ... 15 MB in DRAM	*/

#define CFG_LOAD_ADDR		0x100000	/* default load address */

#define CFG_HZ			1000	/* decrementer freq: 1 ms ticks */

#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400 }

#define CFG_SDRAM_BASE		0x00000000
#define CFG_FLASH_BASE		0xFF000000
#define CFG_FLASH_SIZE		32
#define CFG_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER
#define CFG_MAX_FLASH_BANKS	1	/* max num of flash banks	*/
#define CFG_MAX_FLASH_SECT	512	/* max num of sects on one chip */

#define CFG_FLASH_BANKS_LIST { CFG_FLASH_BASE }

#define CFG_MONITOR_BASE	TEXT_BASE
#if (CFG_MONITOR_BASE < CFG_FLASH_BASE)
#define CFG_RAMBOOT
#endif

#define CFG_MONITOR_LEN		(256 << 10)	/* Reserve 256KB for Monitor */

#define CONFIG_ENV_IS_IN_FLASH

#ifdef CONFIG_ENV_IS_IN_FLASH
#define CFG_ENV_SECT_SIZE	0x10000
#define CFG_ENV_ADDR		(CFG_MONITOR_BASE + CFG_MONITOR_LEN)
#endif /* CONFIG_ENV_IS_IN_FLASH */

/*
 * I2C Bus
 */
#define CONFIG_HARD_I2C		1	/* To enable I2C support	*/
#define CFG_I2C_SPEED		100000	/* I2C speed and slave address	*/
#define CFG_I2C_SLAVE		0x7F

#define	CFG_EEPROM_PAGE_WRITE_ENABLE
#define	CFG_EEPROM_PAGE_WRITE_BITS 3
/* I2C SYSMON (LM75, AD7414 is almost compatible)                       */
#define	CONFIG_DTT_LM75		1	/* ON Semi's LM75               */
#define	CONFIG_DTT_SENSORS	{0}	/* Sensor addresses             */
#define	CFG_DTT_MAX_TEMP	70
#define	CFG_DTT_LOW_TEMP	-30
#define	CFG_DTT_HYSTERESIS	3

#define CFG_IMMR		0xF0000000
#define CFG_DEFAULT_IMMR	0x0F010000

#define CFG_INIT_RAM_ADDR	CFG_IMMR
#define CFG_INIT_RAM_END	0x2000	/* End of used area in DPRAM	*/
#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */
#define CFG_GBL_DATA_OFFSET	(CFG_INIT_RAM_END - CFG_GBL_DATA_SIZE)
#define CFG_INIT_SP_OFFSET	CFG_GBL_DATA_OFFSET

/* Hard reset configuration word */
#define CFG_HRCW_MASTER		0x0E028200	/* BPS=11 CIP=1 ISB=010 BMS=1 */

/* No slaves */
#define CFG_HRCW_SLAVE1 	0
#define CFG_HRCW_SLAVE2 	0
#define CFG_HRCW_SLAVE3 	0
#define CFG_HRCW_SLAVE4 	0
#define CFG_HRCW_SLAVE5 	0
#define CFG_HRCW_SLAVE6 	0
#define CFG_HRCW_SLAVE7 	0

#define BOOTFLAG_COLD		0x01	/* Normal Power-On: Boot from FLASH */
#define BOOTFLAG_WARM		0x02	/* Software reboot                  */

#define CFG_MALLOC_LEN		(4096 << 10)	/* Reserve 4 MB for malloc()	*/
#define CFG_BOOTMAPSZ		(8 << 20)	/* Initial Memory map for Linux */

#define CFG_CACHELINE_SIZE	32	/* For MPC8260 CPUs */
#if defined(CONFIG_CMD_KGDB)
#  define CFG_CACHELINE_SHIFT	5	/* log base 2 of the above value */
#endif

#define CFG_HID0_INIT		0
#define CFG_HID0_FINAL		(HID0_ICE | HID0_IFEM | HID0_ABE)

#define CFG_HID2		0

#define CFG_SIUMCR		0x00200000
#define CFG_BCR			0x004c0000
#define CFG_SCCR		0x0

/*-----------------------------------------------------------------------
 * SYPCR - System Protection Control                             4-35
 * SYPCR can only be written once after reset!
 *-----------------------------------------------------------------------
 * Watchdog & Bus Monitor Timer max, 60x Bus Monitor enable
 */
#if defined(CONFIG_WATCHDOG)
#define CFG_SYPCR       (SYPCR_SWTC|SYPCR_BMT|SYPCR_PBME|SYPCR_LBME|\
			 SYPCR_SWRI|SYPCR_SWP|SYPCR_SWE)
#else
#define CFG_SYPCR       (SYPCR_SWTC|SYPCR_BMT|SYPCR_PBME|SYPCR_LBME|\
			 SYPCR_SWRI|SYPCR_SWP)
#endif /* CONFIG_WATCHDOG */

/*-----------------------------------------------------------------------
 * RMR - Reset Mode Register                                     5-5
 *-----------------------------------------------------------------------
 * turn on Checkstop Reset Enable
 */
#define CFG_RMR         0

/*-----------------------------------------------------------------------
 * TMCNTSC - Time Counter Status and Control                     4-40
 *-----------------------------------------------------------------------
 * Clear once per Second and Alarm Interrupt Status, Set 32KHz timersclk,
 * and enable Time Counter
 */
#define CFG_TMCNTSC     (TMCNTSC_SEC|TMCNTSC_ALR|TMCNTSC_TCF|TMCNTSC_TCE)

/*-----------------------------------------------------------------------
 * PISCR - Periodic Interrupt Status and Control                 4-42
 *-----------------------------------------------------------------------
 * Clear Periodic Interrupt Status, Set 32KHz timersclk, and enable
 * Periodic timer
 */
#define CFG_PISCR       (PISCR_PS|PISCR_PTF|PISCR_PTE)

/*-----------------------------------------------------------------------
 * RCCR - RISC Controller Configuration                         13-7
 *-----------------------------------------------------------------------
 */
#define CFG_RCCR        0

/*
 * Init Memory Controller:
 *
 * Bank Bus     Machine PortSz  Device
 * ---- ---     ------- ------  ------
 *  0   60x     GPCM    32 bit  FLASH
 *  1   60x     SDRAM   64 bit  SDRAM
 *  4   60x     GPCM    16 bit  I/O Ctrl
 *
 */
/* Bank 0 - FLASH
 */
#define CFG_BR0_PRELIM  ((CFG_FLASH_BASE & BRx_BA_MSK)	|\
			 BRx_PS_32			|\
			 BRx_MS_GPCM_P			|\
			 BRx_V)

#define CFG_OR0_PRELIM (0xff000020)

/* Bank 1 - 60x bus SDRAM
 */
#define CFG_GLOBAL_SDRAM_LIMIT	(256 << 20)	/* less than 256 MB */

#define CFG_MPTPR       0x2800

/*-----------------------------------------------------------------------------
 * Address for Mode Register Set (MRS) command
 *-----------------------------------------------------------------------------
 */
#define CFG_MRS_OFFS	0x00000110
#define CFG_PSRT        0x13

#define CFG_BR1_PRELIM  ((CFG_SDRAM_BASE & BRx_BA_MSK)  |\
			 BRx_PS_64                      |\
			 BRx_MS_SDRAM_P                 |\
			 BRx_V)

#define CFG_OR1_PRELIM	CFG_OR1_LITTLE

/* SDRAM initialization values
*/
#define CFG_OR1_LITTLE	((~(CFG_GLOBAL_SDRAM_LIMIT-1) & ORxS_SDAM_MSK) |\
			 ORxS_BPD_4                     |\
			 ORxS_ROWST_PBI1_A7		|\
			 ORxS_NUMR_12)

#define CFG_PSDMR_LITTLE	0x004b36a3

#define CFG_OR1_BIG	((~(CFG_GLOBAL_SDRAM_LIMIT-1) & ORxS_SDAM_MSK) |\
			 ORxS_BPD_4                     |\
			 ORxS_ROWST_PBI1_A4		|\
			 ORxS_NUMR_12)

#define CFG_PSDMR_BIG		0x014f36a3

/* IO on CS4 initialization values
*/
#define CFG_IO_BASE	0xc0000000
#define CFG_IO_SIZE	1

#define CFG_BR4_PRELIM	((CFG_IO_BASE & BRx_BA_MSK) |\
			 BRx_PS_16 | BRx_MS_GPCM_L | BRx_V)

#define CFG_OR4_PRELIM	(0xfff80020)

#define	CFG_RESET_ADDRESS 0xFDFFFFFC	/* "bad" address		*/

/* pass open firmware flat tree */
#define CONFIG_OF_LIBFDT	1
#define CONFIG_OF_BOARD_SETUP	1

#define OF_CPU			"PowerPC,8270@0"
#define OF_SOC			"soc@f0000000"
#define OF_TBCLK		(bd->bi_busfreq / 4)
#if defined(CONFIG_MUAS_DEV_BOARD)
#define OF_STDOUT_PATH		"/soc/cpm/serial@11a90"
#else
#define OF_STDOUT_PATH		"/soc/cpm/serial@11a80"
#endif

#endif /* __CONFIG_H */

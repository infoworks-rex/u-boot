/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 * Keith Outwater, keith_outwater@mvis.com
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

/*
 * board/config_GEN860T.h - board specific configuration options
 */

#ifndef __CONFIG_GEN860T_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 */
#define CONFIG_MPC860
#define CONFIG_GEN860T

/*
 * Identify the board
 */
#if !defined(CONFIG_SC)
#define CONFIG_IDENT_STRING		" B2"
#else
#define CONFIG_IDENT_STRING		" SC"
#endif

/*
 * Don't depend on the RTC clock to determine clock frequency -
 * the 860's internal rtc uses a 32.768 KHz clock which is
 * generated by the DS1337 - and the DS1337 clock can be turned off.
 */
#if !defined(CONFIG_SC)
#define	CONFIG_8xx_GCLK_FREQ		66600000
#else
#define	CONFIG_8xx_GCLK_FREQ		48000000
#endif

/*
 * The RS-232 console port is on SMC1
 */
#define	CONFIG_8xx_CONS_SMC1
#define CONFIG_BAUDRATE			38400

/*
 * Set allowable console baud rates
 */
#define CFG_BAUDRATE_TABLE		{ 9600,		\
					  19200,	\
					  38400,	\
					  57600,	\
					  115200,	\
					}

/*
 * Print console information
 */
#undef	 CFG_CONSOLE_INFO_QUIET

/*
 * Set the autoboot delay in seconds.  A delay of -1 disables autoboot
 */
#define CONFIG_BOOTDELAY				5

/*
 * Pass the clock frequency to the Linux kernel in units of MHz
 */
#define	CONFIG_CLOCKS_IN_MHZ

#define CONFIG_PREBOOT		\
	"echo;echo"

#undef	CONFIG_BOOTARGS
#define CONFIG_BOOTCOMMAND	\
	"bootp;" \
	"setenv bootargs root=/dev/nfs rw nfsroot=${serverip}:${rootpath} " \
	"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}::off; " \
	"bootm"

/*
 * Turn off echo for serial download by default.  Allow baud rate to be changed
 * for downloads
 */
#undef	CONFIG_LOADS_ECHO
#define	CFG_LOADS_BAUD_CHANGE

/*
 * Set default load address for tftp network downloads
 */
#define	CFG_TFTP_LOADADDR				0x01000000

/*
 * Turn off the watchdog timer
 */
#undef	CONFIG_WATCHDOG

/*
 * Do not reboot if a panic occurs
 */
#define CONFIG_PANIC_HANG

/*
 * Enable the status LED
 */
#define	CONFIG_STATUS_LED

/*
 * Reset address. We pick an address such that when an instruction
 * is executed at that address, a machine check exception occurs
 */
#define CFG_RESET_ADDRESS				((ulong) -1)

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_BOOTFILESIZE


/*
 * The GEN860T network interface uses the on-chip 10/100 FEC with
 * an Intel LXT971A PHY connected to the 860T's MII. The PHY's
 * MII address is hardwired on the board to zero.
 */
#define CONFIG_FEC_ENET
#define CFG_DISCOVER_PHY
#define CONFIG_MII
#define CONFIG_MII_INIT			1
#define CONFIG_PHY_ADDR			0

/*
 * Set default IP stuff just to get bootstrap entries into the
 * environment so that we can autoscript the full default environment.
 */
#define CONFIG_ETHADDR					9a:52:63:15:85:25
#define CONFIG_SERVERIP					10.0.4.201
#define CONFIG_IPADDR					10.0.4.111

/*
 * This board has a 32 kibibyte EEPROM (Atmel AT24C256) connected to
 * the MPC860T I2C interface.
 */
#define CFG_I2C_EEPROM_ADDR				0x50
#define CFG_EEPROM_PAGE_WRITE_BITS		6		/* 64 byte pages		*/
#define CFG_EEPROM_PAGE_WRITE_DELAY_MS	12		/* 10 mS w/ 20% margin	*/
#define	CFG_I2C_EEPROM_ADDR_LEN			2		/* need 16 bit address	*/
#define CFG_ENV_EEPROM_SIZE				(32 * 1024)

/*
 * Enable I2C and select the hardware/software driver
 */
#define CONFIG_HARD_I2C		1				/* CPM based I2C			*/
#undef	CONFIG_SOFT_I2C						/* Bit-banged I2C			*/

#ifdef CONFIG_HARD_I2C
#define	CFG_I2C_SPEED		100000			/* clock speed in Hz		*/
#define CFG_I2C_SLAVE		0xFE			/* I2C slave address		*/
#endif

#ifdef CONFIG_SOFT_I2C
#define PB_SCL				0x00000020		/* PB 26					*/
#define PB_SDA				0x00000010		/* PB 27					*/
#define I2C_INIT			(immr->im_cpm.cp_pbdir |=  PB_SCL)
#define I2C_ACTIVE			(immr->im_cpm.cp_pbdir |=  PB_SDA)
#define I2C_TRISTATE		(immr->im_cpm.cp_pbdir &= ~PB_SDA)
#define I2C_READ			((immr->im_cpm.cp_pbdat & PB_SDA) != 0)
#define I2C_SDA(bit)		if(bit) immr->im_cpm.cp_pbdat |=  PB_SDA; \
								else    immr->im_cpm.cp_pbdat &= ~PB_SDA
#define I2C_SCL(bit)		if(bit) immr->im_cpm.cp_pbdat |=  PB_SCL; \
								else    immr->im_cpm.cp_pbdat &= ~PB_SCL
#define I2C_DELAY			udelay(5)		/* 1/4 I2C clock duration	*/
#endif

/*
 * Allow environment overwrites by anyone
 */
#define CONFIG_ENV_OVERWRITE

#if !defined(CONFIG_SC)
/*
 * The MPC860's internal RTC is horribly broken in rev D masks. Three
 * internal MPC860T circuit nodes were inadvertently left floating; this
 * causes KAPWR current in power down mode to be three orders of magnitude
 * higher than specified in the datasheet (from 10 uA to 10 mA).  No
 * reasonable battery can keep that kind RTC running during powerdown for any
 * length of time, so we use an external RTC on the I2C bus instead.
 */
#define CONFIG_RTC_DS1337
#define CFG_I2C_RTC_ADDR				0x68

#else
/*
 * No external RTC on SC variant, so we're stuck with the internal one.
 */
#define	CONFIG_RTC_MPC8xx
#endif

/*
 * Power On Self Test support
 */
#define CONFIG_POST			  ( CFG_POST_CACHE		| \
								CFG_POST_MEMORY		| \
								CFG_POST_CPU		| \
								CFG_POST_UART		| \
								CFG_POST_SPR )


/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_I2C
#define CONFIG_CMD_EEPROM
#define CONFIG_CMD_REGINFO
#define CONFIG_CMD_IMMAP
#define CONFIG_CMD_ELF
#define CONFIG_CMD_DATE
#define CONFIG_CMD_FPGA
#define CONFIG_CMD_MII
#define CONFIG_CMD_BEDBUG

#if !defined(CONFIG_SC)
    #define CONFIG_CMD_DOC
#endif

#ifdef CONFIG_POST
#define CONFIG_CMD_DIAG
#endif

/*
 * There is no IDE/PCMCIA hardware support on the board.
 */
#undef  CONFIG_IDE_PCMCIA
#undef  CONFIG_IDE_LED
#undef  CONFIG_IDE_RESET

/*
 * Enable the call to misc_init_r() for miscellaneous platform
 * dependent initialization.
 */
#define CONFIG_MISC_INIT_R

/*
 * Enable call to last_stage_init() so we can twiddle some LEDS :)
 */
#define CONFIG_LAST_STAGE_INIT

/*
 * Virtex2 FPGA configuration support
 */
#define CONFIG_FPGA_COUNT		1
#define CONFIG_FPGA
#define CONFIG_FPGA_XILINX
#define CONFIG_FPGA_VIRTEX2
#define CFG_FPGA_PROG_FEEDBACK


#define CONFIG_NAND_LEGACY

/*
 * Verbose help from command monitor.
 */
#define	CFG_LONGHELP
#if !defined(CONFIG_SC)
#define	CFG_PROMPT			"B2> "
#else
#define	CFG_PROMPT			"SC> "
#endif


/*
 * Use the "hush" command parser
 */
#define	CFG_HUSH_PARSER
#define	CFG_PROMPT_HUSH_PS2	"> "

/*
 * Set buffer size for console I/O
 */
#if defined(CONFIG_CMD_KGDB)
#define	CFG_CBSIZE			1024
#else
#define	CFG_CBSIZE			256
#endif

/*
 * Print buffer size
 */
#define	CFG_PBSIZE (CFG_CBSIZE + sizeof(CFG_PROMPT) + 16)

/*
 * Maximum number of arguments that a command can accept
 */
#define	CFG_MAXARGS			16

/*
 * Boot argument buffer size
 */
#define CFG_BARGSIZE		CFG_CBSIZE

/*
 * Default memory test range
 */
#define CFG_MEMTEST_START	0x0100000
#define CFG_MEMTEST_END		(CFG_MEMTEST_START  + (128 * 1024))

/*
 * Select the more full-featured memory test
 */
#define	CFG_ALT_MEMTEST

/*
 * Default load address
 */
#define	CFG_LOAD_ADDR		0x01000000

/*
 * Set decrementer frequency (1 ms ticks)
 */
#define	CFG_HZ				1000

/*
 * Device memory map (after SDRAM remap to 0x0):
 *
 * CS		Device				Base Addr	Size
 * ----------------------------------------------------
 * CS0*		Flash				0x40000000	64 M
 * CS1*		SDRAM				0x00000000	16 M
 * CS2*		Disk-On-Chip		0x50000000	32 K
 * CS3*		FPGA				0x60000000	64 M
 * CS4*		SelectMap			0x70000000	32 K
 * CS5*		Mil-Std 1553 I/F	0x80000000	32 K
 * CS6*		Unused
 * CS7*		Unused
 * IMMR		860T Registers		0xfff00000
 */

/*
 * Base addresses and block sizes
 */
#define CFG_IMMR			0xFF000000

#define SDRAM_BASE			0x00000000
#define SDRAM_SIZE			(64 * 1024 * 1024)

#define FLASH_BASE			0x40000000
#define FLASH_SIZE			(16 * 1024 * 1024)

#define DOC_BASE			0x50000000
#define DOC_SIZE			(32 * 1024)

#define FPGA_BASE			0x60000000
#define FPGA_SIZE			(64 * 1024 * 1024)

#define SELECTMAP_BASE		0x70000000
#define SELECTMAP_SIZE		(32 * 1024)

#define M1553_BASE			0x80000000
#define M1553_SIZE			(64 * 1024)

/*
 * Definitions for initial stack pointer and data area (in DPRAM)
 */
#define CFG_INIT_RAM_ADDR		CFG_IMMR
#define	CFG_INIT_RAM_END		0x2F00	/* End of used area in DPRAM		*/
#define	CFG_INIT_DATA_SIZE		64	/* # bytes reserved for initial data*/
#define CFG_GBL_DATA_OFFSET		(CFG_INIT_RAM_END - CFG_INIT_DATA_SIZE)
#define	CFG_INIT_SP_OFFSET		CFG_GBL_DATA_OFFSET

/*
 * Start addresses for the final memory configuration
 * (Set up by the startup code)
 * Please note that CFG_SDRAM_BASE _must_ start at 0
 */
#define	CFG_SDRAM_BASE			SDRAM_BASE

/*
 * FLASH organization
 */
#define CFG_FLASH_BASE			FLASH_BASE
#define CFG_FLASH_SIZE			FLASH_SIZE
#define CFG_FLASH_SECT_SIZE		(128 * 1024)
#define CFG_MAX_FLASH_BANKS		1
#define CFG_MAX_FLASH_SECT		128

/*
 * The timeout values are for an entire chip and are in milliseconds.
 * Yes I know that the write timeout is huge.  Accroding to the
 * datasheet a single byte takes 630 uS (round to 1 ms) max at worst
 * case VCC and temp after 100K programming cycles.  It works out
 * to 280 minutes (might as well be forever).
 */
#define CFG_FLASH_ERASE_TOUT	(CFG_MAX_FLASH_SECT * 5000)
#define CFG_FLASH_WRITE_TOUT	(CFG_MAX_FLASH_SECT * 128 * 1024 * 1)

/*
 * Allow direct writes to FLASH from tftp transfers (** dangerous **)
 */
#define	CFG_DIRECT_FLASH_TFTP

/*
 * Reserve memory for U-Boot.
 */
#define CFG_MAX_UBOOT_SECTS		4
#define	CFG_MONITOR_LEN			(CFG_MAX_UBOOT_SECTS * CFG_FLASH_SECT_SIZE)
#define CFG_MONITOR_BASE		CFG_FLASH_BASE

/*
 * Select environment placement.  NOTE that u-boot.lds must
 * be edited if this is changed!
 */
#undef	CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_IS_IN_EEPROM

#if defined(CONFIG_ENV_IS_IN_EEPROM)
#define CFG_ENV_SIZE			(2 * 1024)
#define CFG_ENV_OFFSET			(CFG_ENV_EEPROM_SIZE - (8 * 1024))
#else
#define CFG_ENV_SIZE			0x1000
#define CFG_ENV_SECT_SIZE		CFG_FLASH_SECT_SIZE

/*
 * This ultimately gets passed right into the linker script, so we have to
 * use a number :(
 */
#define CFG_ENV_OFFSET			0x060000
#endif

/*
 * Reserve memory for malloc()
 */
#define	CFG_MALLOC_LEN		(128 * 1024)

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define	CFG_BOOTMAPSZ		(8 * 1024 * 1024)

/*
 * Cache Configuration
 */
#define CFG_CACHELINE_SIZE		16	/* For all MPC8xx CPUs				*/
#if defined(CONFIG_CMD_KGDB)
#define CFG_CACHELINE_SHIFT		4	/* log base 2 of above value		*/
#endif

/*------------------------------------------------------------------------
 * SYPCR - System Protection Control							UM 11-9
 * -----------------------------------------------------------------------
 * SYPCR can only be written once after reset!
 *
 * Software & Bus Monitor Timer max, Bus Monitor enable, SW Watchdog freeze
 */
#if defined(CONFIG_WATCHDOG)
#define CFG_SYPCR	( SYPCR_SWTC	| \
					  SYPCR_BMT	| \
					  SYPCR_BME	| \
					  SYPCR_SWF	| \
					  SYPCR_SWE	| \
					  SYPCR_SWRI	| \
					  SYPCR_SWP		  \
					)
#else
#define CFG_SYPCR	( SYPCR_SWTC	| \
					  SYPCR_BMT	| \
					  SYPCR_BME	| \
					  SYPCR_SWF	| \
					  SYPCR_SWP		  \
					)
#endif

/*-----------------------------------------------------------------------
 * SIUMCR - SIU Module Configuration							UM 11-6
 *-----------------------------------------------------------------------
 * Set debug pin mux, enable SPKROUT and GPLB5*.
 */
#define CFG_SIUMCR	( SIUMCR_DBGC11 | \
					  SIUMCR_DBPC11 | \
					  SIUMCR_MLRC11	| \
					  SIUMCR_GB5E	  \
					)

/*-----------------------------------------------------------------------
 * TBSCR - Time Base Status and Control							UM 11-26
 *-----------------------------------------------------------------------
 * Clear Reference Interrupt Status, Timebase freeze enabled
 */
#define CFG_TBSCR	( TBSCR_REFA | \
					  TBSCR_REFB | \
					  TBSCR_TBF	   \
					)

/*-----------------------------------------------------------------------
 * RTCSC - Real-Time Clock Status and Control Register			UM 11-27
 *-----------------------------------------------------------------------
 */
#define CFG_RTCSC	( RTCSC_SEC	| \
					  RTCSC_ALR | \
					  RTCSC_RTF | \
					  RTCSC_RTE	  \
					)

/*-----------------------------------------------------------------------
 * PISCR - Periodic Interrupt Status and Control				UM 11-31
 *-----------------------------------------------------------------------
 * Clear Periodic Interrupt Status, Interrupt Timer freezing enabled
 */
#define CFG_PISCR	( PISCR_PS		| \
					  PISCR_PITF	  \
					)

/*-----------------------------------------------------------------------
 * PLPRCR - PLL, Low-Power, and Reset Control Register			UM 15-30
 *-----------------------------------------------------------------------
 * Reset PLL lock status sticky bit, timer expired status bit and timer
 * interrupt status bit. Set MF for 1:2:1 mode.
 */
#define CFG_PLPRCR	( ((0x1 << PLPRCR_MF_SHIFT) & PLPRCR_MF_MSK)	| \
					  PLPRCR_SPLSS	| \
					  PLPRCR_TEXPS	| \
					  PLPRCR_TMIST	  \
					)

/*-----------------------------------------------------------------------
 * SCCR - System Clock and reset Control Register				UM 15-27
 *-----------------------------------------------------------------------
 * Set clock output, timebase and RTC source and divider,
 * power management and some other internal clocks
 */
#define SCCR_MASK   SCCR_EBDF11

#if !defined(CONFIG_SC)
#define CFG_SCCR	( SCCR_TBS			|	/* timebase = GCLK/2	*/ \
					  SCCR_COM00		|	/* full strength CLKOUT	*/ \
					  SCCR_DFSYNC00	|	/* SYNCLK / 1 (normal)	*/ \
					  SCCR_DFBRG00		|	/* BRGCLK / 1 (normal)	*/ \
					  SCCR_DFNL000		| \
					  SCCR_DFNH000		  \
					)
#else
#define CFG_SCCR	( SCCR_TBS			|	/* timebase = GCLK/2	*/ \
					  SCCR_COM00		|	/* full strength CLKOUT	*/ \
					  SCCR_DFSYNC00	|	/* SYNCLK / 1 (normal)	*/ \
					  SCCR_DFBRG00		|	/* BRGCLK / 1 (normal)	*/ \
					  SCCR_DFNL000		| \
					  SCCR_DFNH000		| \
					  SCCR_RTDIV		| \
					  SCCR_RTSEL		  \
					)
#endif

/*-----------------------------------------------------------------------
 * DER - Debug Enable Register									UM 37-46
 *-----------------------------------------------------------------------
 * Mask all events that can cause entry into debug mode
 */
#define CFG_DER				0

/*
 * Initialize Memory Controller:
 *
 * BR0 and OR0 (FLASH memory)
 */
#define FLASH_BASE0_PRELIM	FLASH_BASE

/*
 * Flash address mask
 */
#define CFG_PRELIM_OR_AM	0xfe000000

/*
 * FLASH timing:
 * 33 Mhz bus with ACS = 11, TRLX = 1, CSNT = 1, SCY = 3, EHTR = 1
 */
#define CFG_OR_TIMING_FLASH	( OR_CSNT_SAM	| \
							  OR_ACS_DIV2	| \
							  OR_BI			| \
							  OR_SCY_2_CLK	| \
							  OR_TRLX		| \
							  OR_EHTR		  \
							)

#define CFG_OR0_PRELIM	( CFG_PRELIM_OR_AM		| \
						  CFG_OR_TIMING_FLASH	  \
						)

#define CFG_BR0_PRELIM	( (FLASH_BASE0_PRELIM & BR_BA_MSK)	| \
						  BR_MS_GPCM						| \
						  BR_PS_8							| \
						  BR_V								  \
						)

/*
 * SDRAM configuration
 */
#define CFG_OR1_AM	0xfc000000
#define CFG_OR1		( (CFG_OR1_AM & OR_AM_MSK)	| \
					  OR_CSNT_SAM				  \
					)

#define CFG_BR1		( (SDRAM_BASE & BR_BA_MSK)	| \
					  BR_MS_UPMA				| \
					  BR_PS_32					| \
					  BR_V						  \
					)

/*
 * Refresh rate 7.8 us (= 64 ms / 8K = 31.2 uS quad bursts) for one bank
 * of 256 MBit SDRAM
 */
#define CFG_MPTPR_1BK_8K	MPTPR_PTP_DIV16

/*
 * Periodic timer for refresh @ 33 MHz system clock
 */
#define CFG_MAMR_PTA	64

/*
 * MAMR settings for SDRAM
 */
#define CFG_MAMR_8COL	( (CFG_MAMR_PTA << MAMR_PTA_SHIFT)	| \
						  MAMR_PTAE				| \
						  MAMR_AMA_TYPE_1			| \
						  MAMR_DSA_1_CYCL			| \
						  MAMR_G0CLA_A10			| \
						  MAMR_RLFA_1X				| \
						  MAMR_WLFA_1X				| \
						  MAMR_TLFA_4X				  \
						)

/*
 * CS2* configuration for Disk On Chip:
 * 33 MHz bus with TRLX=1, ACS=11, CSNT=1, EBDF=1, SCY=2, EHTR=1,
 * no burst.
 */
#define CFG_OR2_PRELIM	( (0xffff0000 & OR_AM_MSK)	| \
						  OR_CSNT_SAM				| \
						  OR_ACS_DIV2				| \
						  OR_BI						| \
						  OR_SCY_2_CLK				| \
						  OR_TRLX					| \
						  OR_EHTR					  \
						)

#define CFG_BR2_PRELIM	( (DOC_BASE & BR_BA_MSK)	| \
						  BR_PS_8					| \
						  BR_MS_GPCM				| \
						  BR_V						  \
						)

/*
 * CS3* configuration for FPGA:
 * 33 MHz bus with SCY=15, no burst.
 * The FPGA uses TA and TEA to terminate bus cycles, but we
 * clear SETA and set the cycle length to a large number so that
 * the cycle will still complete even if there is a configuration
 * error that prevents TA from asserting on FPGA accesss.
 */
#define CFG_OR3_PRELIM	( (0xfc000000 & OR_AM_MSK)  | \
						  OR_SCY_15_CLK				| \
						  OR_BI					  \
						)

#define CFG_BR3_PRELIM	( (FPGA_BASE & BR_BA_MSK)	| \
						  BR_PS_32					| \
						  BR_MS_GPCM				| \
						  BR_V						  \
						)
/*
 * CS4* configuration for FPGA SelectMap configuration interface.
 * 33 MHz bus, UPMB, no burst. Do not assert GPLB5 on falling edge
 * of GCLK1_50
 */
#define CFG_OR4_PRELIM	( (0xffff0000 & OR_AM_MSK)	| \
						  OR_G5LS						| \
						  OR_BI							  \
						)

#define CFG_BR4_PRELIM	( (SELECTMAP_BASE & BR_BA_MSK)	| \
						  BR_PS_8						| \
						  BR_MS_UPMB					| \
						  BR_V							  \
						)

/*
 * CS5* configuration for Mil-Std 1553 databus interface.
 * 33 MHz bus, GPCM, no burst.
 * The 1553 interface  uses TA and TEA to terminate bus cycles,
 * but we clear SETA and set the cycle length to a large number so that
 * the cycle will still complete even if there is a configuration
 * error that prevents TA from asserting on FPGA accesss.
 */
#define CFG_OR5_PRELIM	( (0xffff0000 & OR_AM_MSK)  | \
						  OR_SCY_15_CLK				| \
						  OR_EHTR					| \
						  OR_TRLX					| \
						  OR_CSNT_SAM				| \
						  OR_BI						  \
						)

#define CFG_BR5_PRELIM	( (M1553_BASE & BR_BA_MSK)	| \
						  BR_PS_16					| \
						  BR_MS_GPCM				| \
						  BR_V						  \
						)

/*
 * Boot Flags
 */
#define	BOOTFLAG_COLD	0x01	/* Normal Power-On: Boot from FLASH	*/
#define BOOTFLAG_WARM	0x02	/* Software reboot					*/

/*
 * Disk On Chip (millenium) configuration
 */
#if !defined(CONFIG_SC)
#define CFG_MAX_DOC_DEVICE	1
#undef	CFG_DOC_SUPPORT_2000
#define CFG_DOC_SUPPORT_MILLENNIUM
#undef	CFG_DOC_PASSIVE_PROBE
#endif

/*
 * FEC interrupt assignment
 */
#define FEC_INTERRUPT   SIU_LEVEL1

/*
 * Sanity checks
 */
#if defined(CONFIG_SCC1_ENET) && defined(CONFIG_FEC_ENET)
#error Both CONFIG_SCC1_ENET and CONFIG_FEC_ENET configured
#endif

#endif	/* __CONFIG_GEN860T_H */

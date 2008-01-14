/*
 * MCF5329 Internal Memory Map
 *
 * Copyright (C) 2004-2007 Freescale Semiconductor, Inc.
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com)
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

#ifndef __IMMAP_5329__
#define __IMMAP_5329__

#define MMAP_SCM1	0xEC000000
#define MMAP_MDHA	0xEC080000
#define MMAP_SKHA	0xEC084000
#define MMAP_RNG	0xEC088000
#define MMAP_SCM2	0xFC000000
#define MMAP_XBS	0xFC004000
#define MMAP_FBCS	0xFC008000
#define MMAP_CAN	0xFC020000
#define MMAP_FEC	0xFC030000
#define MMAP_SCM3	0xFC040000
#define MMAP_EDMA	0xFC044000
#define MMAP_TCD	0xFC045000
#define MMAP_INTC0	0xFC048000
#define MMAP_INTC1	0xFC04C000
#define MMAP_INTCACK	0xFC054000
#define MMAP_I2C	0xFC058000
#define MMAP_QSPI	0xFC05C000
#define MMAP_UART0	0xFC060000
#define MMAP_UART1	0xFC064000
#define MMAP_UART2	0xFC068000
#define MMAP_DTMR0	0xFC070000
#define MMAP_DTMR1	0xFC074000
#define MMAP_DTMR2	0xFC078000
#define MMAP_DTMR3	0xFC07C000
#define MMAP_PIT0	0xFC080000
#define MMAP_PIT1	0xFC084000
#define MMAP_PIT2	0xFC088000
#define MMAP_PIT3	0xFC08C000
#define MMAP_PWM	0xFC090000
#define MMAP_EPORT	0xFC094000
#define MMAP_WDOG	0xFC098000
#define MMAP_RCM	0xFC0A0000
#define MMAP_CCM	0xFC0A0004
#define MMAP_GPIO	0xFC0A4000
#define MMAP_RTC	0xFC0A8000
#define MMAP_LCDC	0xFC0AC000
#define MMAP_USBOTG	0xFC0B0000
#define MMAP_USBH	0xFC0B4000
#define MMAP_SDRAM	0xFC0B8000
#define MMAP_SSI	0xFC0BC000
#define MMAP_PLL	0xFC0C0000

#include <asm/coldfire/crossbar.h>
#include <asm/coldfire/edma.h>
#include <asm/coldfire/flexbus.h>
#include <asm/coldfire/lcd.h>
#include <asm/coldfire/ssi.h>

/* System control module registers */
typedef struct scm1_ctrl {
	u32 mpr0;		/* 0x00 Master Privilege Register 0 */
	u32 res1[15];		/* 0x04 - 0x3F */
	u32 pacrh;		/* 0x40 Peripheral Access Control Register H */
	u32 res2[3];		/* 0x44 - 0x53 */
	u32 bmt0;		/*0x54 Bus Monitor Timeout 0 */
} scm1_t;

/* Message Digest Hardware Accelerator */
typedef struct mdha_ctrl {
	u32 mdmr;		/* 0x00 MDHA Mode Register */
	u32 mdcr;		/* 0x04 Control register */
	u32 mdcmr;		/* 0x08 Command Register */
	u32 mdsr;		/* 0x0C Status Register */
	u32 mdisr;		/* 0x10 Interrupt Status Register */
	u32 mdimr;		/* 0x14 Interrupt Mask Register */
	u32 mddsr;		/* 0x1C Data Size Register */
	u32 mdin;		/* 0x20 Input FIFO */
	u32 res1[3];		/* 0x24 - 0x2F */
	u32 mdao;		/* 0x30 Message Digest AO Register */
	u32 mdbo;		/* 0x34 Message Digest BO Register */
	u32 mdco;		/* 0x38 Message Digest CO Register */
	u32 mddo;		/* 0x3C Message Digest DO Register */
	u32 mdeo;		/* 0x40 Message Digest EO Register */
	u32 mdmds;		/* 0x44 Message Data Size Register */
	u32 res[10];		/* 0x48 - 0x6F */
	u32 mda1;		/* 0x70 Message Digest A1 Register */
	u32 mdb1;		/* 0x74 Message Digest B1 Register */
	u32 mdc1;		/* 0x78 Message Digest C1 Register */
	u32 mdd1;		/* 0x7C Message Digest D1 Register */
	u32 mde1;		/* 0x80 Message Digest E1 Register */
} mdha_t;

/* Symmetric Key Hardware Accelerator */
typedef struct skha_ctrl {
	u32 mr;			/* 0x00 Mode Register */
	u32 cr;			/* 0x04 Control Register */
	u32 cmr;		/* 0x08 Command Register */
	u32 sr;			/* 0x0C Status Register */
	u32 esr;		/* 0x10 Error Status Register */
	u32 emr;		/* 0x14 Error Status Mask Register) */
	u32 ksr;		/* 0x18 Key Size Register */
	u32 dsr;		/* 0x1C Data Size Register */
	u32 in;			/* 0x20 Input FIFO */
	u32 out;		/* 0x24 Output FIFO */
	u32 res1[2];		/* 0x28 - 0x2F */
	u32 kdr1;		/* 0x30 Key Data Register 1  */
	u32 kdr2;		/* 0x34 Key Data Register 2 */
	u32 kdr3;		/* 0x38 Key Data Register 3 */
	u32 kdr4;		/* 0x3C Key Data Register 4 */
	u32 kdr5;		/* 0x40 Key Data Register 5 */
	u32 kdr6;		/* 0x44 Key Data Register 6 */
	u32 res2[10];		/* 0x48 - 0x6F */
	u32 c1;			/* 0x70 Context 1 */
	u32 c2;			/* 0x74 Context 2 */
	u32 c3;			/* 0x78 Context 3 */
	u32 c4;			/* 0x7C Context 4 */
	u32 c5;			/* 0x80 Context 5 */
	u32 c6;			/* 0x84 Context 6 */
	u32 c7;			/* 0x88 Context 7 */
	u32 c8;			/* 0x8C Context 8 */
	u32 c9;			/* 0x90 Context 9 */
	u32 c10;		/* 0x94 Context 10 */
	u32 c11;		/* 0x98 Context 11 */
} skha_t;

/* Random Number Generator */
typedef struct rng_ctrl {
	u32 rngcr;		/* 0x00 RNG Control Register */
	u32 rngsr;		/* 0x04 RNG Status Register */
	u32 rnger;		/* 0x08 RNG Entropy Register */
	u32 rngout;		/* 0x0C RNG Output FIFO */
} rng_t;

/* System control module registers 2 */
typedef struct scm2_ctrl {
	u32 mpr1;		/* 0x00 Master Privilege Register */
	u32 res1[7];		/* 0x04 - 0x1F */
	u32 pacra;		/* 0x20 Peripheral Access Control Register A */
	u32 pacrb;		/* 0x24 Peripheral Access Control Register B */
	u32 pacrc;		/* 0x28 Peripheral Access Control Register C */
	u32 pacrd;		/* 0x2C Peripheral Access Control Register D */
	u32 res2[4];		/* 0x30 - 0x3F */
	u32 pacre;		/* 0x40 Peripheral Access Control Register E */
	u32 pacrf;		/* 0x44 Peripheral Access Control Register F */
	u32 pacrg;		/* 0x48 Peripheral Access Control Register G */
	u32 res3[2];		/* 0x4C - 0x53 */
	u32 bmt1;		/* 0x54 Bus Monitor Timeout 1 */
} scm2_t;

/* FlexCan module registers */
typedef struct can_ctrl {
	u32 mcr;		/* 0x00 Module Configuration register */
	u32 ctrl;		/* 0x04 Control register */
	u32 timer;		/* 0x08 Free Running Timer */
	u32 res1;		/* 0x0C */
	u32 rxgmask;		/* 0x10 Rx Global Mask */
	u32 rx14mask;		/* 0x14 RxBuffer 14 Mask */
	u32 rx15mask;		/* 0x18 RxBuffer 15 Mask */
	u32 errcnt;		/* 0x1C Error Counter Register */
	u32 errstat;		/* 0x20 Error and status Register */
	u32 res2;		/* 0x24 */
	u32 imask;		/* 0x28 Interrupt Mask Register */
	u32 res3;		/* 0x2C */
	u32 iflag;		/* 0x30 Interrupt Flag Register */
	u32 res4[19];		/* 0x34 - 0x7F */
	u32 MB0_15[2048];	/* 0x80 Message Buffer 0-15 */
} can_t;

/* System Control Module register 3 */
typedef struct scm3_ctrl {
	u8 res1[19];		/* 0x00 - 0x12 */
	u8 wcr;			/* 0x13 wakeup control register */
	u16 res2;		/* 0x14 - 0x15 */
	u16 cwcr;		/* 0x16 Core Watchdog Control Register */
	u8 res3[3];		/* 0x18 - 0x1A */
	u8 cwsr;		/* 0x1B Core Watchdog Service Register */
	u8 res4[2];		/* 0x1C - 0x1D */
	u8 scmisr;		/* 0x1F Interrupt Status Register */
	u32 res5;		/* 0x20 */
	u32 bcr;		/* 0x24 Burst Configuration Register */
	u32 res6[18];		/* 0x28 - 0x6F */
	u32 cfadr;		/* 0x70 Core Fault Address Register */
	u8 res7[4];		/* 0x71 - 0x74 */
	u8 cfier;		/* 0x75 Core Fault Interrupt Enable Register */
	u8 cfloc;		/* 0x76 Core Fault Location Register */
	u8 cfatr;		/* 0x77 Core Fault Attributes Register */
	u32 res8;		/* 0x78 */
	u32 cfdtr;		/* 0x7C Core Fault Data Register */
} scm3_t;

/* Interrupt module registers */
typedef struct int0_ctrl {
	/* Interrupt Controller 0 */
	u32 iprh0;		/* 0x00 Pending Register High */
	u32 iprl0;		/* 0x04 Pending Register Low */
	u32 imrh0;		/* 0x08 Mask Register High */
	u32 imrl0;		/* 0x0C Mask Register Low */
	u32 frch0;		/* 0x10 Force Register High */
	u32 frcl0;		/* 0x14 Force Register Low */
	u16 res1;		/* 0x18 - 0x19 */
	u16 icfg0;		/* 0x1A Configuration Register */
	u8 simr0;		/* 0x1C Set Interrupt Mask */
	u8 cimr0;		/* 0x1D Clear Interrupt Mask */
	u8 clmask0;		/* 0x1E Current Level Mask */
	u8 slmask;		/* 0x1F Saved Level Mask */
	u32 res2[8];		/* 0x20 - 0x3F */
	u8 icr0[64];		/* 0x40 - 0x7F Control registers */
	u32 res3[24];		/* 0x80 - 0xDF */
	u8 swiack0;		/* 0xE0 Software Interrupt Acknowledge */
	u8 res4[3];		/* 0xE1 - 0xE3 */
	u8 Lniack0_1;		/* 0xE4 Level n interrupt acknowledge resister */
	u8 res5[3];		/* 0xE5 - 0xE7 */
	u8 Lniack0_2;		/* 0xE8 Level n interrupt acknowledge resister */
	u8 res6[3];		/* 0xE9 - 0xEB */
	u8 Lniack0_3;		/* 0xEC Level n interrupt acknowledge resister */
	u8 res7[3];		/* 0xED - 0xEF */
	u8 Lniack0_4;		/* 0xF0 Level n interrupt acknowledge resister */
	u8 res8[3];		/* 0xF1 - 0xF3 */
	u8 Lniack0_5;		/* 0xF4 Level n interrupt acknowledge resister */
	u8 res9[3];		/* 0xF5 - 0xF7 */
	u8 Lniack0_6;		/* 0xF8 Level n interrupt acknowledge resister */
	u8 resa[3];		/* 0xF9 - 0xFB */
	u8 Lniack0_7;		/* 0xFC Level n interrupt acknowledge resister */
	u8 resb[3];		/* 0xFD - 0xFF */
} int0_t;

typedef struct int1_ctrl {
	/* Interrupt Controller 1 */
	u32 iprh1;		/* 0x00 Pending Register High */
	u32 iprl1;		/* 0x04 Pending Register Low */
	u32 imrh1;		/* 0x08 Mask Register High */
	u32 imrl1;		/* 0x0C Mask Register Low */
	u32 frch1;		/* 0x10 Force Register High */
	u32 frcl1;		/* 0x14 Force Register Low */
	u16 res1;		/* 0x18 */
	u16 icfg1;		/* 0x1A Configuration Register */
	u8 simr1;		/* 0x1C Set Interrupt Mask */
	u8 cimr1;		/* 0x1D Clear Interrupt Mask */
	u16 res2;		/* 0x1E - 0x1F */
	u32 res3[8];		/* 0x20 - 0x3F */
	u8 icr1[64];		/* 0x40 - 0x7F */
	u32 res4[24];		/* 0x80 - 0xDF */
	u8 swiack1;		/* 0xE0 Software Interrupt Acknowledge */
	u8 res5[3];		/* 0xE1 - 0xE3 */
	u8 Lniack1_1;		/* 0xE4 Level n interrupt acknowledge resister */
	u8 res6[3];		/* 0xE5 - 0xE7 */
	u8 Lniack1_2;		/* 0xE8 Level n interrupt acknowledge resister */
	u8 res7[3];		/* 0xE9 - 0xEB */
	u8 Lniack1_3;		/* 0xEC Level n interrupt acknowledge resister */
	u8 res8[3];		/* 0xED - 0xEF */
	u8 Lniack1_4;		/* 0xF0 Level n interrupt acknowledge resister */
	u8 res9[3];		/* 0xF1 - 0xF3 */
	u8 Lniack1_5;		/* 0xF4 Level n interrupt acknowledge resister */
	u8 resa[3];		/* 0xF5 - 0xF7 */
	u8 Lniack1_6;		/* 0xF8 Level n interrupt acknowledge resister */
	u8 resb[3];		/* 0xF9 - 0xFB */
	u8 Lniack1_7;		/* 0xFC Level n interrupt acknowledge resister */
	u8 resc[3];		/* 0xFD - 0xFF */
} int1_t;

typedef struct intgack_ctrl1 {
	/* Global IACK Registers */
	u8 swiack;		/* 0xE0 Global Software Interrupt Acknowledge */
	u8 Lniack[7];		/* 0xE1 - 0xE7 Global Level 0 Interrupt Acknowledge */
} intgack_t;

/* QSPI module registers */
typedef struct qspi_ctrl {
	u16 qmr;		/* Mode register */
	u16 res1;
	u16 qdlyr;		/* Delay register */
	u16 res2;
	u16 qwr;		/* Wrap register */
	u16 res3;
	u16 qir;		/* Interrupt register */
	u16 res4;
	u16 qar;		/* Address register */
	u16 res5;
	u16 qdr;		/* Data register */
	u16 res6;
} qspi_t;

/* PWM module registers */
typedef struct pwm_ctrl {
	u8 en;			/* 0x00 PWM Enable Register */
	u8 pol;			/* 0x01 Polarity Register */
	u8 clk;			/* 0x02 Clock Select Register */
	u8 prclk;		/* 0x03 Prescale Clock Select Register */
	u8 cae;			/* 0x04 Center Align Enable Register */
	u8 ctl;			/* 0x05 Control Register */
	u8 res1[2];		/* 0x06 - 0x07 */
	u8 scla;		/* 0x08 Scale A register */
	u8 sclb;		/* 0x09 Scale B register */
	u8 res2[2];		/* 0x0A - 0x0B */
	u8 cnt0;		/* 0x0C Channel 0 Counter register */
	u8 cnt1;		/* 0x0D Channel 1 Counter register */
	u8 cnt2;		/* 0x0E Channel 2 Counter register */
	u8 cnt3;		/* 0x0F Channel 3 Counter register */
	u8 cnt4;		/* 0x10 Channel 4 Counter register */
	u8 cnt5;		/* 0x11 Channel 5 Counter register */
	u8 cnt6;		/* 0x12 Channel 6 Counter register */
	u8 cnt7;		/* 0x13 Channel 7 Counter register */
	u8 per0;		/* 0x14 Channel 0 Period register */
	u8 per1;		/* 0x15 Channel 1 Period register */
	u8 per2;		/* 0x16 Channel 2 Period register */
	u8 per3;		/* 0x17 Channel 3 Period register */
	u8 per4;		/* 0x18 Channel 4 Period register */
	u8 per5;		/* 0x19 Channel 5 Period register */
	u8 per6;		/* 0x1A Channel 6 Period register */
	u8 per7;		/* 0x1B Channel 7 Period register */
	u8 dty0;		/* 0x1C Channel 0 Duty register */
	u8 dty1;		/* 0x1D Channel 1 Duty register */
	u8 dty2;		/* 0x1E Channel 2 Duty register */
	u8 dty3;		/* 0x1F Channel 3 Duty register */
	u8 dty4;		/* 0x20 Channel 4 Duty register */
	u8 dty5;		/* 0x21 Channel 5 Duty register */
	u8 dty6;		/* 0x22 Channel 6 Duty register */
	u8 dty7;		/* 0x23 Channel 7 Duty register */
	u8 sdn;			/* 0x24 Shutdown register */
	u8 res3[3];		/* 0x25 - 0x27 */
} pwm_t;

/* Edge Port module registers */
typedef struct eport_ctrl {
	u16 par;		/* 0x00 Pin Assignment Register */
	u8 ddar;		/* 0x02 Data Direction Register */
	u8 ier;			/* 0x03 Interrupt Enable Register */
	u8 dr;			/* 0x04 Data Register */
	u8 pdr;			/* 0x05 Pin Data  Register */
	u8 fr;			/* 0x06 Flag_Register */
	u8 res1;
} eport_t;

/* Watchdog registers */
typedef struct wdog_ctrl {
	u16 cr;			/* 0x00 Control register */
	u16 mr;			/* 0x02 Modulus register */
	u16 cntr;		/* 0x04 Count register */
	u16 sr;			/* 0x06 Service register */
} wdog_t;

/*Chip configuration module registers */
typedef struct ccm_ctrl {
	u16 ccr;		/* 0x00 Chip configuration register */
	u16 res2;		/* 0x02 */
	u16 rcon;		/* 0x04 Rreset configuration register */
	u16 cir;		/* 0x06 Chip identification register */
	u32 res3;		/* 0x08 */
	u16 misccr;		/* 0x0A Miscellaneous control register */
	u16 cdr;		/* 0x0C Clock divider register */
	u16 uhcsr;		/* 0x10 USB Host controller status register */
	u16 uocsr;		/* 0x12 USB On-the-Go Controller Status Reg */
} ccm_t;

typedef struct rcm {
	u8 rcr;
	u8 rsr;
} rcm_t;

/* GPIO port registers */
typedef struct gpio_ctrl {
	/* Port Output Data Registers */
	u8 podr_fech;		/* 0x00 */
	u8 podr_fecl;		/* 0x01 */
	u8 podr_ssi;		/* 0x02 */
	u8 podr_busctl;		/* 0x03 */
	u8 podr_be;		/* 0x04 */
	u8 podr_cs;		/* 0x05 */
	u8 podr_pwm;		/* 0x06 */
	u8 podr_feci2c;		/* 0x07 */
	u8 res1;		/* 0x08 */
	u8 podr_uart;		/* 0x09 */
	u8 podr_qspi;		/* 0x0A */
	u8 podr_timer;		/* 0x0B */
	u8 res2;		/* 0x0C */
	u8 podr_lcddatah;	/* 0x0D */
	u8 podr_lcddatam;	/* 0x0E */
	u8 podr_lcddatal;	/* 0x0F */
	u8 podr_lcdctlh;	/* 0x10 */
	u8 podr_lcdctll;	/* 0x11 */

	/* Port Data Direction Registers */
	u16 res3;		/* 0x12 - 0x13 */
	u8 pddr_fech;		/* 0x14 */
	u8 pddr_fecl;		/* 0x15 */
	u8 pddr_ssi;		/* 0x16 */
	u8 pddr_busctl;		/* 0x17 */
	u8 pddr_be;		/* 0x18 */
	u8 pddr_cs;		/* 0x19 */
	u8 pddr_pwm;		/* 0x1A */
	u8 pddr_feci2c;		/* 0x1B */
	u8 res4;		/* 0x1C */
	u8 pddr_uart;		/* 0x1D */
	u8 pddr_qspi;		/* 0x1E */
	u8 pddr_timer;		/* 0x1F */
	u8 res5;		/* 0x20 */
	u8 pddr_lcddatah;	/* 0x21 */
	u8 pddr_lcddatam;	/* 0x22 */
	u8 pddr_lcddatal;	/* 0x23 */
	u8 pddr_lcdctlh;	/* 0x24 */
	u8 pddr_lcdctll;	/* 0x25 */
	u16 res6;		/* 0x26 - 0x27 */

	/* Port Data Direction Registers */
	u8 ppd_fech;		/* 0x28 */
	u8 ppd_fecl;		/* 0x29 */
	u8 ppd_ssi;		/* 0x2A */
	u8 ppd_busctl;		/* 0x2B */
	u8 ppd_be;		/* 0x2C */
	u8 ppd_cs;		/* 0x2D */
	u8 ppd_pwm;		/* 0x2E */
	u8 ppd_feci2c;		/* 0x2F */
	u8 res7;		/* 0x30 */
	u8 ppd_uart;		/* 0x31 */
	u8 ppd_qspi;		/* 0x32 */
	u8 ppd_timer;		/* 0x33 */
	u8 res8;		/* 0x34 */
	u8 ppd_lcddatah;	/* 0x35 */
	u8 ppd_lcddatam;	/* 0x36 */
	u8 ppd_lcddatal;	/* 0x37 */
	u8 ppd_lcdctlh;		/* 0x38 */
	u8 ppd_lcdctll;		/* 0x39 */
	u16 res9;		/* 0x3A - 0x3B */

	/* Port Clear Output Data Registers */
	u8 pclrr_fech;		/* 0x3C */
	u8 pclrr_fecl;		/* 0x3D */
	u8 pclrr_ssi;		/* 0x3E */
	u8 pclrr_busctl;	/* 0x3F */
	u8 pclrr_be;		/* 0x40 */
	u8 pclrr_cs;		/* 0x41 */
	u8 pclrr_pwm;		/* 0x42 */
	u8 pclrr_feci2c;	/* 0x43 */
	u8 res10;		/* 0x44 */
	u8 pclrr_uart;		/* 0x45 */
	u8 pclrr_qspi;		/* 0x46 */
	u8 pclrr_timer;		/* 0x47 */
	u8 res11;		/* 0x48 */
	u8 pclrr_lcddatah;	/* 0x49 */
	u8 pclrr_lcddatam;	/* 0x4A */
	u8 pclrr_lcddatal;	/* 0x4B */
	u8 pclrr_lcdctlh;	/* 0x4C */
	u8 pclrr_lcdctll;	/* 0x4D */
	u16 res12;		/* 0x4E - 0x4F */

	/* Pin Assignment Registers */
	u8 par_fec;		/* 0x50 */
	u8 par_pwm;		/* 0x51 */
	u8 par_busctl;		/* 0x52 */
	u8 par_feci2c;		/* 0x53 */
	u8 par_be;		/* 0x54 */
	u8 par_cs;		/* 0x55 */
	u16 par_ssi;		/* 0x56 */
	u16 par_uart;		/* 0x58 */
	u16 par_qspi;		/* 0x5A */
	u8 par_timer;		/* 0x5C */
	u8 par_lcddata;		/* 0x5D */
	u16 par_lcdctl;		/* 0x5E */
	u16 par_irq;		/* 0x60 */
	u16 res16;		/* 0x62 - 0x63 */

	/* Mode Select Control Registers */
	u8 mscr_flexbus;	/* 0x64 */
	u8 mscr_sdram;		/* 0x65 */
	u16 res17;		/* 0x66 - 0x67 */

	/* Drive Strength Control Registers */
	u8 dscr_i2c;		/* 0x68 */
	u8 dscr_pwm;		/* 0x69 */
	u8 dscr_fec;		/* 0x6A */
	u8 dscr_uart;		/* 0x6B */
	u8 dscr_qspi;		/* 0x6C */
	u8 dscr_timer;		/* 0x6D */
	u8 dscr_ssi;		/* 0x6E */
	u8 dscr_lcd;		/* 0x6F */
	u8 dscr_debug;		/* 0x70 */
	u8 dscr_clkrst;		/* 0x71 */
	u8 dscr_irq;		/* 0x72 */
} gpio_t;

/* USB OTG module registers */
typedef struct usb_otg {
	u32 id;			/* 0x000 Identification Register */
	u32 hwgeneral;		/* 0x004 General HW Parameters */
	u32 hwhost;		/* 0x008 Host HW Parameters */
	u32 hwdev;		/* 0x00C Device HW parameters */
	u32 hwtxbuf;		/* 0x010 TX Buffer HW Parameters */
	u32 hwrxbuf;		/* 0x014 RX Buffer HW Parameters */
	u32 res1[58];		/* 0x18 - 0xFF */
	u8 caplength;		/* 0x100 Capability Register Length */
	u8 res2;		/* 0x101 */
	u16 hciver;		/* 0x102 Host Interface Version Number */
	u32 hcsparams;		/* 0x104 Host Structural Parameters */
	u32 hccparams;		/* 0x108 Host Capability Parameters */
	u32 res3[5];		/* 0x10C - 0x11F */
	u16 dciver;		/* 0x120 Device Interface Version Number */
	u16 res4;		/* 0x122 */
	u32 dccparams;		/* 0x124 Device Capability Parameters */
	u32 res5[6];		/* 0x128 - 0x13F */
	u32 cmd;		/* 0x140 USB Command */
	u32 sts;		/* 0x144 USB Status */
	u32 intr;		/* 0x148 USB Interrupt Enable */
	u32 frindex;		/* 0x14C USB Frame Index */
	u32 res6;		/* 0x150 */
	u32 prd_dev;		/* 0x154 Periodic Frame List Base or Device Address */
	u32 aync_ep;		/* 0x158 Current Asynchronous List or Address at Endpoint List Address */
	u32 ttctrl;		/* 0x15C Host TT Asynchronous Buffer Control */
	u32 burstsize;		/* 0x160 Master Interface Data Burst Size */
	u32 txfill;		/* 0x164 Host Transmit FIFO Tuning Control */
	u32 res7[6];		/* 0x168 - 0x17F */
	u32 cfgflag;		/* 0x180 Configure Flag Register */
	u32 portsc1;		/* 0x184 Port Status/Control */
	u32 res8[7];		/* 0x188 - 0x1A3 */
	u32 otgsc;		/* 0x1A4 On The Go Status and Control */
	u32 mode;		/* 0x1A8 USB mode register */
	u32 eptsetstat;		/* 0x1AC Endpoint Setup status */
	u32 eptprime;		/* 0x1B0 Endpoint initialization */
	u32 eptflush;		/* 0x1B4 Endpoint de-initialize */
	u32 eptstat;		/* 0x1B8 Endpoint status */
	u32 eptcomplete;	/* 0x1BC Endpoint Complete */
	u32 eptctrl0;		/* 0x1C0 Endpoint control 0 */
	u32 eptctrl1;		/* 0x1C4 Endpoint control 1 */
	u32 eptctrl2;		/* 0x1C8 Endpoint control 2 */
	u32 eptctrl3;		/* 0x1CC Endpoint control 3 */
} usbotg_t;

/* USB Host module registers */
typedef struct usb_host {
	u32 id;			/* 0x000 Identification Register */
	u32 hwgeneral;		/* 0x004 General HW Parameters */
	u32 hwhost;		/* 0x008 Host HW Parameters */
	u32 res1;		/* 0x0C */
	u32 hwtxbuf;		/* 0x010 TX Buffer HW Parameters */
	u32 hwrxbuf;		/* 0x014 RX Buffer HW Parameters */
	u32 res2[58];		/* 0x18 - 0xFF */

	/* Host Controller Capability Register */
	u8 caplength;		/* 0x100 Capability Register Length */
	u8 res3;		/* 0x101 */
	u16 hciver;		/* 0x102 Host Interface Version Number */
	u32 hcsparams;		/* 0x104 Host Structural Parameters */
	u32 hccparams;		/* 0x108 Host Capability Parameters */
	u32 res4[13];		/* 0x10C - 0x13F */

	/* Host Controller Operational Register */
	u32 cmd;		/* 0x140 USB Command */
	u32 sts;		/* 0x144 USB Status */
	u32 intr;		/* 0x148 USB Interrupt Enable */
	u32 frindex;		/* 0x14C USB Frame Index */
	u32 res5;		/* 0x150 (ctrl segment register in EHCI spec) */
	u32 prdlst;		/* 0x154 Periodic Frame List Base Address */
	u32 aynclst;		/* 0x158 Current Asynchronous List Address */
	u32 ttctrl;		/* 0x15C Host TT Asynchronous Buffer Control (non-ehci) */
	u32 burstsize;		/* 0x160 Master Interface Data Burst Size (non-ehci) */
	u32 txfill;		/* 0x164 Host Transmit FIFO Tuning Control  (non-ehci) */
	u32 res6[6];		/* 0x168 - 0x17F */
	u32 cfgflag;		/* 0x180 Configure Flag Register */
	u32 portsc1;		/* 0x184 Port Status/Control */
	u32 res7[8];		/* 0x188 - 0x1A7 */

	/* non-ehci registers */
	u32 mode;		/* 0x1A8 USB mode register */
	u32 eptsetstat;		/* 0x1AC Endpoint Setup status */
	u32 eptprime;		/* 0x1B0 Endpoint initialization */
	u32 eptflush;		/* 0x1B4 Endpoint de-initialize */
	u32 eptstat;		/* 0x1B8 Endpoint status */
	u32 eptcomplete;	/* 0x1BC Endpoint Complete */
	u32 eptctrl0;		/* 0x1C0 Endpoint control 0 */
	u32 eptctrl1;		/* 0x1C4 Endpoint control 1 */
	u32 eptctrl2;		/* 0x1C8 Endpoint control 2 */
	u32 eptctrl3;		/* 0x1CC Endpoint control 3 */
} usbhost_t;

/* SDRAM controller registers */
typedef struct sdram_ctrl {
	u32 mode;		/* 0x00 Mode/Extended Mode register */
	u32 ctrl;		/* 0x04 Control register */
	u32 cfg1;		/* 0x08 Configuration register 1 */
	u32 cfg2;		/* 0x0C Configuration register 2 */
	u32 res1[64];		/* 0x10 - 0x10F */
	u32 cs0;		/* 0x110 Chip Select 0 Configuration */
	u32 cs1;		/* 0x114 Chip Select 1 Configuration */
} sdram_t;

/* Clock Module registers */
typedef struct pll_ctrl {
	u8 podr;		/* 0x00 Output Divider Register */
	u8 res1[3];
	u8 pcr;			/* 0x04 Control Register */
	u8 res2[3];
	u8 pmdr;		/* 0x08 Modulation Divider Register */
	u8 res3[3];
	u8 pfdr;		/* 0x0C Feedback Divider Register */
	u8 res4[3];
} pll_t;

#endif				/* __IMMAP_5329__ */

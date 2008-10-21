/*
 * (C) Copyright 2003
 * Josef Baumgartner <josef.baumgartner@telex.de>
 *
 * MCF5282 additionals
 * (C) Copyright 2005
 * BuS Elektronik GmbH & Co. KG <esw@bus-elektronik.de>
 *
 * Copyright (C) 2004-2007 Freescale Semiconductor, Inc.
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com)
 * Hayden Fraser (Hayden.Fraser@freescale.com)
 *
 * MCF5275 additions
 * Copyright (C) 2008 Arthur Shipkowski (art@videon-central.com)
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

#include <common.h>
#include <watchdog.h>
#include <asm/immap.h>

#if defined(CONFIG_M5253)
/*
 * Breath some life into the CPU...
 *
 * Set up the memory map,
 * initialize a bunch of registers,
 * initialize the UPM's
 */
void cpu_init_f(void)
{
	mbar_writeByte(MCFSIM_MPARK, 0x40);	/* 5249 Internal Core takes priority over DMA */
	mbar_writeByte(MCFSIM_SYPCR, 0x00);
	mbar_writeByte(MCFSIM_SWIVR, 0x0f);
	mbar_writeByte(MCFSIM_SWSR, 0x00);
	mbar_writeByte(MCFSIM_SWDICR, 0x00);
	mbar_writeByte(MCFSIM_TIMER1ICR, 0x00);
	mbar_writeByte(MCFSIM_TIMER2ICR, 0x88);
	mbar_writeByte(MCFSIM_I2CICR, 0x00);
	mbar_writeByte(MCFSIM_UART1ICR, 0x00);
	mbar_writeByte(MCFSIM_UART2ICR, 0x00);
	mbar_writeByte(MCFSIM_ICR6, 0x00);
	mbar_writeByte(MCFSIM_ICR7, 0x00);
	mbar_writeByte(MCFSIM_ICR8, 0x00);
	mbar_writeByte(MCFSIM_ICR9, 0x00);
	mbar_writeByte(MCFSIM_QSPIICR, 0x00);

	mbar2_writeLong(MCFSIM_GPIO_INT_EN, 0x00000080);
	mbar2_writeByte(MCFSIM_INTBASE, 0x40);	/* Base interrupts at 64 */
	mbar2_writeByte(MCFSIM_SPURVEC, 0x00);

	/*mbar2_writeLong(MCFSIM_IDECONFIG1, 0x00000020); */ /* Enable a 1 cycle pre-drive cycle on CS1 */

	/*
	 *  Setup chip selects...
	 */

	mbar_writeShort(MCFSIM_CSAR1, CONFIG_SYS_CSAR1);
	mbar_writeShort(MCFSIM_CSCR1, CONFIG_SYS_CSCR1);
	mbar_writeLong(MCFSIM_CSMR1, CONFIG_SYS_CSMR1);

	mbar_writeShort(MCFSIM_CSAR0, CONFIG_SYS_CSAR0);
	mbar_writeShort(MCFSIM_CSCR0, CONFIG_SYS_CSCR0);
	mbar_writeLong(MCFSIM_CSMR0, CONFIG_SYS_CSMR0);

#ifdef CONFIG_FSL_I2C
	CONFIG_SYS_I2C_PINMUX_REG = CONFIG_SYS_I2C_PINMUX_REG & CONFIG_SYS_I2C_PINMUX_CLR;
	CONFIG_SYS_I2C_PINMUX_REG |= CONFIG_SYS_I2C_PINMUX_SET;
#ifdef CONFIG_SYS_I2C2_OFFSET
	CONFIG_SYS_I2C2_PINMUX_REG &= CONFIG_SYS_I2C2_PINMUX_CLR;
	CONFIG_SYS_I2C2_PINMUX_REG |= CONFIG_SYS_I2C2_PINMUX_SET;
#endif
#endif

	/* enable instruction cache now */
	icache_enable();
}

/*initialize higher level parts of CPU like timers */
int cpu_init_r(void)
{
	return (0);
}

void uart_port_conf(void)
{
	/* Setup Ports: */
	switch (CONFIG_SYS_UART_PORT) {
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	}
}
#endif				/* #if defined(CONFIG_M5253) */

#if defined(CONFIG_M5271)
void cpu_init_f(void)
{
#ifndef CONFIG_WATCHDOG
	/* Disable the watchdog if we aren't using it */
	mbar_writeShort(MCF_WTM_WCR, 0);
#endif

	/* Set clockspeed to 100MHz */
	mbar_writeShort(MCF_FMPLL_SYNCR,
			MCF_FMPLL_SYNCR_MFD(0) | MCF_FMPLL_SYNCR_RFD(0));
	while (!mbar_readByte(MCF_FMPLL_SYNSR) & MCF_FMPLL_SYNSR_LOCK) ;
}

/*
 * initialize higher level parts of CPU like timers
 */
int cpu_init_r(void)
{
	return (0);
}

void uart_port_conf(void)
{
	/* Setup Ports: */
	switch (CONFIG_SYS_UART_PORT) {
	case 0:
		mbar_writeShort(MCF_GPIO_PAR_UART, MCF_GPIO_PAR_UART_U0TXD |
				MCF_GPIO_PAR_UART_U0RXD);
		break;
	case 1:
		mbar_writeShort(MCF_GPIO_PAR_UART,
				MCF_GPIO_PAR_UART_U1RXD_UART1 |
				MCF_GPIO_PAR_UART_U1TXD_UART1);
		break;
	case 2:
		mbar_writeShort(MCF_GPIO_PAR_UART, 0x3000);
		break;
	}
}
#endif

#if defined(CONFIG_M5272)
/*
 * Breath some life into the CPU...
 *
 * Set up the memory map,
 * initialize a bunch of registers,
 * initialize the UPM's
 */
void cpu_init_f(void)
{
	/* if we come from RAM we assume the CPU is
	 * already initialized.
	 */
#ifndef CONFIG_MONITOR_IS_IN_RAM
	volatile sysctrl_t *sysctrl = (sysctrl_t *) (CONFIG_SYS_MBAR);
	volatile gpio_t *gpio = (gpio_t *) (MMAP_GPIO);
	volatile csctrl_t *csctrl = (csctrl_t *) (MMAP_FBCS);

	sysctrl->sc_scr = CONFIG_SYS_SCR;
	sysctrl->sc_spr = CONFIG_SYS_SPR;

	/* Setup Ports: */
	gpio->gpio_pacnt = CONFIG_SYS_PACNT;
	gpio->gpio_paddr = CONFIG_SYS_PADDR;
	gpio->gpio_padat = CONFIG_SYS_PADAT;
	gpio->gpio_pbcnt = CONFIG_SYS_PBCNT;
	gpio->gpio_pbddr = CONFIG_SYS_PBDDR;
	gpio->gpio_pbdat = CONFIG_SYS_PBDAT;
	gpio->gpio_pdcnt = CONFIG_SYS_PDCNT;

	/* Memory Controller: */
	csctrl->cs_br0 = CONFIG_SYS_BR0_PRELIM;
	csctrl->cs_or0 = CONFIG_SYS_OR0_PRELIM;

#if (defined(CONFIG_SYS_OR1_PRELIM) && defined(CONFIG_SYS_BR1_PRELIM))
	csctrl->cs_br1 = CONFIG_SYS_BR1_PRELIM;
	csctrl->cs_or1 = CONFIG_SYS_OR1_PRELIM;
#endif

#if defined(CONFIG_SYS_OR2_PRELIM) && defined(CONFIG_SYS_BR2_PRELIM)
	csctrl->cs_br2 = CONFIG_SYS_BR2_PRELIM;
	csctrl->cs_or2 = CONFIG_SYS_OR2_PRELIM;
#endif

#if defined(CONFIG_SYS_OR3_PRELIM) && defined(CONFIG_SYS_BR3_PRELIM)
	csctrl->cs_br3 = CONFIG_SYS_BR3_PRELIM;
	csctrl->cs_or3 = CONFIG_SYS_OR3_PRELIM;
#endif

#if defined(CONFIG_SYS_OR4_PRELIM) && defined(CONFIG_SYS_BR4_PRELIM)
	csctrl->cs_br4 = CONFIG_SYS_BR4_PRELIM;
	csctrl->cs_or4 = CONFIG_SYS_OR4_PRELIM;
#endif

#if defined(CONFIG_SYS_OR5_PRELIM) && defined(CONFIG_SYS_BR5_PRELIM)
	csctrl->cs_br5 = CONFIG_SYS_BR5_PRELIM;
	csctrl->cs_or5 = CONFIG_SYS_OR5_PRELIM;
#endif

#if defined(CONFIG_SYS_OR6_PRELIM) && defined(CONFIG_SYS_BR6_PRELIM)
	csctrl->cs_br6 = CONFIG_SYS_BR6_PRELIM;
	csctrl->cs_or6 = CONFIG_SYS_OR6_PRELIM;
#endif

#if defined(CONFIG_SYS_OR7_PRELIM) && defined(CONFIG_SYS_BR7_PRELIM)
	csctrl->cs_br7 = CONFIG_SYS_BR7_PRELIM;
	csctrl->cs_or7 = CONFIG_SYS_OR7_PRELIM;
#endif

#endif				/* #ifndef CONFIG_MONITOR_IS_IN_RAM */

	/* enable instruction cache now */
	icache_enable();

}

/*
 * initialize higher level parts of CPU like timers
 */
int cpu_init_r(void)
{
	return (0);
}

void uart_port_conf(void)
{
	volatile gpio_t *gpio = (gpio_t *) MMAP_GPIO;

	/* Setup Ports: */
	switch (CONFIG_SYS_UART_PORT) {
	case 0:
		gpio->gpio_pbcnt &= ~(GPIO_PBCNT_PB0MSK | GPIO_PBCNT_PB1MSK);
		gpio->gpio_pbcnt |= (GPIO_PBCNT_URT0_TXD | GPIO_PBCNT_URT0_RXD);
		break;
	case 1:
		gpio->gpio_pdcnt &= ~(GPIO_PDCNT_PD1MSK | GPIO_PDCNT_PD4MSK);
		gpio->gpio_pdcnt |= (GPIO_PDCNT_URT1_RXD | GPIO_PDCNT_URT1_TXD);
		break;
	}
}
#endif				/* #if defined(CONFIG_M5272) */

#if defined(CONFIG_M5275)

/*
 * Breathe some life into the CPU...
 *
 * Set up the memory map,
 * initialize a bunch of registers,
 * initialize the UPM's
 */
void cpu_init_f(void)
{
	/* if we come from RAM we assume the CPU is
	 * already initialized.
	 */

#ifndef CONFIG_MONITOR_IS_IN_RAM
	volatile wdog_t *wdog_reg = (wdog_t *)(MMAP_WDOG);
	volatile gpio_t *gpio_reg = (gpio_t *)(MMAP_GPIO);
	volatile csctrl_t *csctrl_reg = (csctrl_t *)(MMAP_FBCS);

	/* Kill watchdog so we can initialize the PLL */
	wdog_reg->wcr = 0;

	/* Memory Controller: */
	/* Flash */
	csctrl_reg->ar0 = CONFIG_SYS_AR0_PRELIM;
	csctrl_reg->cr0 = CONFIG_SYS_CR0_PRELIM;
	csctrl_reg->mr0 = CONFIG_SYS_MR0_PRELIM;

#if (defined(CONFIG_SYS_AR1_PRELIM) && defined(CONFIG_SYS_CR1_PRELIM) && defined(CONFIG_SYS_MR1_PRELIM))
	csctrl_reg->ar1 = CONFIG_SYS_AR1_PRELIM;
	csctrl_reg->cr1 = CONFIG_SYS_CR1_PRELIM;
	csctrl_reg->mr1 = CONFIG_SYS_MR1_PRELIM;
#endif

#if (defined(CONFIG_SYS_AR2_PRELIM) && defined(CONFIG_SYS_CR2_PRELIM) && defined(CONFIG_SYS_MR2_PRELIM))
	csctrl_reg->ar2 = CONFIG_SYS_AR2_PRELIM;
	csctrl_reg->cr2 = CONFIG_SYS_CR2_PRELIM;
	csctrl_reg->mr2 = CONFIG_SYS_MR2_PRELIM;
#endif

#if (defined(CONFIG_SYS_AR3_PRELIM) && defined(CONFIG_SYS_CR3_PRELIM) && defined(CONFIG_SYS_MR3_PRELIM))
	csctrl_reg->ar3 = CONFIG_SYS_AR3_PRELIM;
	csctrl_reg->cr3 = CONFIG_SYS_CR3_PRELIM;
	csctrl_reg->mr3 = CONFIG_SYS_MR3_PRELIM;
#endif

#if (defined(CONFIG_SYS_AR4_PRELIM) && defined(CONFIG_SYS_CR4_PRELIM) && defined(CONFIG_SYS_MR4_PRELIM))
	csctrl_reg->ar4 = CONFIG_SYS_AR4_PRELIM;
	csctrl_reg->cr4 = CONFIG_SYS_CR4_PRELIM;
	csctrl_reg->mr4 = CONFIG_SYS_MR4_PRELIM;
#endif

#if (defined(CONFIG_SYS_AR5_PRELIM) && defined(CONFIG_SYS_CR5_PRELIM) && defined(CONFIG_SYS_MR5_PRELIM))
	csctrl_reg->ar5 = CONFIG_SYS_AR5_PRELIM;
	csctrl_reg->cr5 = CONFIG_SYS_CR5_PRELIM;
	csctrl_reg->mr5 = CONFIG_SYS_MR5_PRELIM;
#endif

#if (defined(CONFIG_SYS_AR6_PRELIM) && defined(CONFIG_SYS_CR6_PRELIM) && defined(CONFIG_SYS_MR6_PRELIM))
	csctrl_reg->ar6 = CONFIG_SYS_AR6_PRELIM;
	csctrl_reg->cr6 = CONFIG_SYS_CR6_PRELIM;
	csctrl_reg->mr6 = CONFIG_SYS_MR6_PRELIM;
#endif

#if (defined(CONFIG_SYS_AR7_PRELIM) && defined(CONFIG_SYS_CR7_PRELIM) && defined(CONFIG_SYS_MR7_PRELIM))
	csctrl_reg->ar7 = CONFIG_SYS_AR7_PRELIM;
	csctrl_reg->cr7 = CONFIG_SYS_CR7_PRELIM;
	csctrl_reg->mr7 = CONFIG_SYS_MR7_PRELIM;
#endif

#endif				/* #ifndef CONFIG_MONITOR_IS_IN_RAM */

#ifdef CONFIG_FSL_I2C
	CONFIG_SYS_I2C_PINMUX_REG &= CONFIG_SYS_I2C_PINMUX_CLR;
	CONFIG_SYS_I2C_PINMUX_REG |= CONFIG_SYS_I2C_PINMUX_SET;
#endif

	/* enable instruction cache now */
	icache_enable();
}

/*
 * initialize higher level parts of CPU like timers
 */
int cpu_init_r(void)
{
	return (0);
}

void uart_port_conf(void)
{
	volatile gpio_t *gpio = (gpio_t *)MMAP_GPIO;

	/* Setup Ports: */
	switch (CONFIG_SYS_UART_PORT) {
	case 0:
		gpio->par_uart |= UART0_ENABLE_MASK;
		break;
	case 1:
		gpio->par_uart |= UART1_ENABLE_MASK;
		break;
	case 2:
		gpio->par_uart |= UART2_ENABLE_MASK;
		break;
	}
}
#endif				/* #if defined(CONFIG_M5275) */

#if defined(CONFIG_M5282)
/*
 * Breath some life into the CPU...
 *
 * Set up the memory map,
 * initialize a bunch of registers,
 * initialize the UPM's
 */
void cpu_init_f(void)
{
#ifndef CONFIG_WATCHDOG
	/* disable watchdog if we aren't using it */
	MCFWTM_WCR = 0;
#endif

#ifndef CONFIG_MONITOR_IS_IN_RAM
	/* Set speed /PLL */
	MCFCLOCK_SYNCR =
	    MCFCLOCK_SYNCR_MFD(CONFIG_SYS_MFD) | MCFCLOCK_SYNCR_RFD(CONFIG_SYS_RFD);
	while (!(MCFCLOCK_SYNSR & MCFCLOCK_SYNSR_LOCK)) ;

	MCFGPIO_PBCDPAR = 0xc0;

	/* Set up the GPIO ports */
#ifdef CONFIG_SYS_PEPAR
	MCFGPIO_PEPAR = CONFIG_SYS_PEPAR;
#endif
#ifdef	CONFIG_SYS_PFPAR
	MCFGPIO_PFPAR = CONFIG_SYS_PFPAR;
#endif
#ifdef CONFIG_SYS_PJPAR
	MCFGPIO_PJPAR = CONFIG_SYS_PJPAR;
#endif
#ifdef CONFIG_SYS_PSDPAR
	MCFGPIO_PSDPAR = CONFIG_SYS_PSDPAR;
#endif
#ifdef CONFIG_SYS_PASPAR
	MCFGPIO_PASPAR = CONFIG_SYS_PASPAR;
#endif
#ifdef CONFIG_SYS_PEHLPAR
	MCFGPIO_PEHLPAR = CONFIG_SYS_PEHLPAR;
#endif
#ifdef CONFIG_SYS_PQSPAR
	MCFGPIO_PQSPAR = CONFIG_SYS_PQSPAR;
#endif
#ifdef CONFIG_SYS_PTCPAR
	MCFGPIO_PTCPAR = CONFIG_SYS_PTCPAR;
#endif
#ifdef CONFIG_SYS_PTDPAR
	MCFGPIO_PTDPAR = CONFIG_SYS_PTDPAR;
#endif
#ifdef CONFIG_SYS_PUAPAR
	MCFGPIO_PUAPAR = CONFIG_SYS_PUAPAR;
#endif

#ifdef CONFIG_SYS_DDRUA
	MCFGPIO_DDRUA = CONFIG_SYS_DDRUA;
#endif

	/* This is probably a bad place to setup chip selects, but everyone
	   else is doing it! */

#if defined(CONFIG_SYS_CS0_BASE) & defined(CONFIG_SYS_CS0_SIZE) & \
    defined(CONFIG_SYS_CS0_WIDTH) & defined(CONFIG_SYS_CS0_WS)

	MCFCSM_CSAR0 = (CONFIG_SYS_CS0_BASE >> 16) & 0xFFFF;

#if (CONFIG_SYS_CS0_WIDTH == 8)
#define	 CONFIG_SYS_CS0_PS  MCFCSM_CSCR_PS_8
#elif (CONFIG_SYS_CS0_WIDTH == 16)
#define	 CONFIG_SYS_CS0_PS  MCFCSM_CSCR_PS_16
#elif (CONFIG_SYS_CS0_WIDTH == 32)
#define	 CONFIG_SYS_CS0_PS  MCFCSM_CSCR_PS_32
#else
#error	"CONFIG_SYS_CS0_WIDTH: Fault - wrong bus with for CS0"
#endif
	MCFCSM_CSCR0 = MCFCSM_CSCR_WS(CONFIG_SYS_CS0_WS)
	    | CONFIG_SYS_CS0_PS | MCFCSM_CSCR_AA;

#if (CONFIG_SYS_CS0_RO != 0)
	MCFCSM_CSMR0 = MCFCSM_CSMR_BAM(CONFIG_SYS_CS0_SIZE - 1)
	    | MCFCSM_CSMR_WP | MCFCSM_CSMR_V;
#else
	MCFCSM_CSMR0 = MCFCSM_CSMR_BAM(CONFIG_SYS_CS0_SIZE - 1) | MCFCSM_CSMR_V;
#endif
#else
#warning "Chip Select 0 are not initialized/used"
#endif

#if defined(CONFIG_SYS_CS1_BASE) & defined(CONFIG_SYS_CS1_SIZE) & \
    defined(CONFIG_SYS_CS1_WIDTH) & defined(CONFIG_SYS_CS1_WS)

	MCFCSM_CSAR1 = (CONFIG_SYS_CS1_BASE >> 16) & 0xFFFF;

#if (CONFIG_SYS_CS1_WIDTH == 8)
#define	 CONFIG_SYS_CS1_PS  MCFCSM_CSCR_PS_8
#elif (CONFIG_SYS_CS1_WIDTH == 16)
#define	 CONFIG_SYS_CS1_PS  MCFCSM_CSCR_PS_16
#elif (CONFIG_SYS_CS1_WIDTH == 32)
#define	 CONFIG_SYS_CS1_PS  MCFCSM_CSCR_PS_32
#else
#error	"CONFIG_SYS_CS1_WIDTH: Fault - wrong bus with for CS1"
#endif
	MCFCSM_CSCR1 = MCFCSM_CSCR_WS(CONFIG_SYS_CS1_WS)
	    | CONFIG_SYS_CS1_PS | MCFCSM_CSCR_AA;

#if (CONFIG_SYS_CS1_RO != 0)
	MCFCSM_CSMR1 = MCFCSM_CSMR_BAM(CONFIG_SYS_CS1_SIZE - 1)
	    | MCFCSM_CSMR_WP | MCFCSM_CSMR_V;
#else
	MCFCSM_CSMR1 = MCFCSM_CSMR_BAM(CONFIG_SYS_CS1_SIZE - 1)
	    | MCFCSM_CSMR_V;
#endif
#else
#warning "Chip Select 1 are not initialized/used"
#endif

#if defined(CONFIG_SYS_CS2_BASE) & defined(CONFIG_SYS_CS2_SIZE) & \
    defined(CONFIG_SYS_CS2_WIDTH) & defined(CONFIG_SYS_CS2_WS)

	MCFCSM_CSAR2 = (CONFIG_SYS_CS2_BASE >> 16) & 0xFFFF;

#if (CONFIG_SYS_CS2_WIDTH == 8)
#define	 CONFIG_SYS_CS2_PS  MCFCSM_CSCR_PS_8
#elif (CONFIG_SYS_CS2_WIDTH == 16)
#define	 CONFIG_SYS_CS2_PS  MCFCSM_CSCR_PS_16
#elif (CONFIG_SYS_CS2_WIDTH == 32)
#define	 CONFIG_SYS_CS2_PS  MCFCSM_CSCR_PS_32
#else
#error	"CONFIG_SYS_CS2_WIDTH: Fault - wrong bus with for CS2"
#endif
	MCFCSM_CSCR2 = MCFCSM_CSCR_WS(CONFIG_SYS_CS2_WS)
	    | CONFIG_SYS_CS2_PS | MCFCSM_CSCR_AA;

#if (CONFIG_SYS_CS2_RO != 0)
	MCFCSM_CSMR2 = MCFCSM_CSMR_BAM(CONFIG_SYS_CS2_SIZE - 1)
	    | MCFCSM_CSMR_WP | MCFCSM_CSMR_V;
#else
	MCFCSM_CSMR2 = MCFCSM_CSMR_BAM(CONFIG_SYS_CS2_SIZE - 1)
	    | MCFCSM_CSMR_V;
#endif
#else
#warning "Chip Select 2 are not initialized/used"
#endif

#if defined(CONFIG_SYS_CS3_BASE) & defined(CONFIG_SYS_CS3_SIZE) & \
    defined(CONFIG_SYS_CS3_WIDTH) & defined(CONFIG_SYS_CS3_WS)

	MCFCSM_CSAR3 = (CONFIG_SYS_CS3_BASE >> 16) & 0xFFFF;

#if (CONFIG_SYS_CS3_WIDTH == 8)
#define	 CONFIG_SYS_CS3_PS  MCFCSM_CSCR_PS_8
#elif (CONFIG_SYS_CS3_WIDTH == 16)
#define	 CONFIG_SYS_CS3_PS  MCFCSM_CSCR_PS_16
#elif (CONFIG_SYS_CS3_WIDTH == 32)
#define	 CONFIG_SYS_CS3_PS  MCFCSM_CSCR_PS_32
#else
#error	"CONFIG_SYS_CS3_WIDTH: Fault - wrong bus with for CS1"
#endif
	MCFCSM_CSCR3 = MCFCSM_CSCR_WS(CONFIG_SYS_CS3_WS)
	    | CONFIG_SYS_CS3_PS | MCFCSM_CSCR_AA;

#if (CONFIG_SYS_CS3_RO != 0)
	MCFCSM_CSMR3 = MCFCSM_CSMR_BAM(CONFIG_SYS_CS3_SIZE - 1)
	    | MCFCSM_CSMR_WP | MCFCSM_CSMR_V;
#else
	MCFCSM_CSMR3 = MCFCSM_CSMR_BAM(CONFIG_SYS_CS3_SIZE - 1)
	    | MCFCSM_CSMR_V;
#endif
#else
#warning "Chip Select 3 are not initialized/used"
#endif

#endif				/* CONFIG_MONITOR_IS_IN_RAM */

	/* defer enabling cache until boot (see do_go) */
	/* icache_enable(); */
}

/*
 * initialize higher level parts of CPU like timers
 */
int cpu_init_r(void)
{
	return (0);
}

void uart_port_conf(void)
{
	/* Setup Ports: */
	switch (CONFIG_SYS_UART_PORT) {
	case 0:
		MCFGPIO_PUAPAR &= 0xFc;
		MCFGPIO_PUAPAR |= 0x03;
		break;
	case 1:
		MCFGPIO_PUAPAR &= 0xF3;
		MCFGPIO_PUAPAR |= 0x0C;
		break;
	case 2:
		MCFGPIO_PASPAR &= 0xFF0F;
		MCFGPIO_PASPAR |= 0x00A0;
		break;
	}
}
#endif

#if defined(CONFIG_M5249)
/*
 * Breath some life into the CPU...
 *
 * Set up the memory map,
 * initialize a bunch of registers,
 * initialize the UPM's
 */
void cpu_init_f(void)
{
	/*
	 *  NOTE: by setting the GPIO_FUNCTION registers, we ensure that the UART pins
	 *        (UART0: gpio 30,27, UART1: gpio 31, 28) will be used as UART pins
	 *        which is their primary function.
	 *        ~Jeremy
	 */
	mbar2_writeLong(MCFSIM_GPIO_FUNC, CONFIG_SYS_GPIO_FUNC);
	mbar2_writeLong(MCFSIM_GPIO1_FUNC, CONFIG_SYS_GPIO1_FUNC);
	mbar2_writeLong(MCFSIM_GPIO_EN, CONFIG_SYS_GPIO_EN);
	mbar2_writeLong(MCFSIM_GPIO1_EN, CONFIG_SYS_GPIO1_EN);
	mbar2_writeLong(MCFSIM_GPIO_OUT, CONFIG_SYS_GPIO_OUT);
	mbar2_writeLong(MCFSIM_GPIO1_OUT, CONFIG_SYS_GPIO1_OUT);

	/*
	 *  dBug Compliance:
	 *    You can verify these values by using dBug's 'ird'
	 *    (Internal Register Display) command
	 *    ~Jeremy
	 *
	 */
	mbar_writeByte(MCFSIM_MPARK, 0x30);	/* 5249 Internal Core takes priority over DMA */
	mbar_writeByte(MCFSIM_SYPCR, 0x00);
	mbar_writeByte(MCFSIM_SWIVR, 0x0f);
	mbar_writeByte(MCFSIM_SWSR, 0x00);
	mbar_writeLong(MCFSIM_IMR, 0xfffffbff);
	mbar_writeByte(MCFSIM_SWDICR, 0x00);
	mbar_writeByte(MCFSIM_TIMER1ICR, 0x00);
	mbar_writeByte(MCFSIM_TIMER2ICR, 0x88);
	mbar_writeByte(MCFSIM_I2CICR, 0x00);
	mbar_writeByte(MCFSIM_UART1ICR, 0x00);
	mbar_writeByte(MCFSIM_UART2ICR, 0x00);
	mbar_writeByte(MCFSIM_ICR6, 0x00);
	mbar_writeByte(MCFSIM_ICR7, 0x00);
	mbar_writeByte(MCFSIM_ICR8, 0x00);
	mbar_writeByte(MCFSIM_ICR9, 0x00);
	mbar_writeByte(MCFSIM_QSPIICR, 0x00);

	mbar2_writeLong(MCFSIM_GPIO_INT_EN, 0x00000080);
	mbar2_writeByte(MCFSIM_INTBASE, 0x40);	/* Base interrupts at 64 */
	mbar2_writeByte(MCFSIM_SPURVEC, 0x00);
	mbar2_writeLong(MCFSIM_IDECONFIG1, 0x00000020);	/* Enable a 1 cycle pre-drive cycle on CS1 */

	/* Setup interrupt priorities for gpio7 */
	/* mbar2_writeLong(MCFSIM_INTLEV5, 0x70000000); */

	/* IDE Config registers */
	mbar2_writeLong(MCFSIM_IDECONFIG1, 0x00000020);
	mbar2_writeLong(MCFSIM_IDECONFIG2, 0x00000000);

	/*
	 *  Setup chip selects...
	 */

	mbar_writeShort(MCFSIM_CSAR1, CONFIG_SYS_CSAR1);
	mbar_writeShort(MCFSIM_CSCR1, CONFIG_SYS_CSCR1);
	mbar_writeLong(MCFSIM_CSMR1, CONFIG_SYS_CSMR1);

	mbar_writeShort(MCFSIM_CSAR0, CONFIG_SYS_CSAR0);
	mbar_writeShort(MCFSIM_CSCR0, CONFIG_SYS_CSCR0);
	mbar_writeLong(MCFSIM_CSMR0, CONFIG_SYS_CSMR0);

	/* enable instruction cache now */
	icache_enable();
}

/*
 * initialize higher level parts of CPU like timers
 */
int cpu_init_r(void)
{
	return (0);
}

void uart_port_conf(void)
{
	/* Setup Ports: */
	switch (CONFIG_SYS_UART_PORT) {
	case 0:
		break;
	case 1:
		break;
	}
}
#endif				/* #if defined(CONFIG_M5249) */

/*
 * (C) Copyright 2003 Josef Baumgartner <josef.baumgartner@telex.de>
 *
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

#include <asm/mcftimer.h>

#ifdef	CONFIG_M5272
#include <asm/m5272.h>
#include <asm/immap_5272.h>
#endif

#ifdef	CONFIG_M5282
#include <asm/m5282.h>
#endif


static ulong timestamp;
#ifdef	CONFIG_M5282
static unsigned short lastinc;
#endif


#if defined(CONFIG_M5272)
/*
 * We use timer 3 which is running with a period of 1 us
 */
void udelay(unsigned long usec)
{
	volatile timer_t *timerp = (timer_t *) (CFG_MBAR + MCFTIMER_BASE3);
	uint start, now, tmp;

	while (usec > 0) {
		if (usec > 65000)
			tmp = 65000;
		else
			tmp = usec;
		usec = usec - tmp;

		/* Set up TIMER 3 as timebase clock */
		timerp->timer_tmr = MCFTIMER_TMR_DISABLE;
		timerp->timer_tcn = 0;
		/* set period to 1 us */
		timerp->timer_tmr = (((CFG_CLK / 1000000) - 1)	<< 8) | MCFTIMER_TMR_CLK1 |
				     MCFTIMER_TMR_FREERUN | MCFTIMER_TMR_ENABLE;

		start = now = timerp->timer_tcn;
		while (now < start + tmp)
			now = timerp->timer_tcn;
	}
}

void mcf_timer_interrupt (void * not_used){
	volatile timer_t *timerp = (timer_t *) (CFG_MBAR + MCFTIMER_BASE4);
	volatile intctrl_t *intp = (intctrl_t *) (CFG_MBAR + MCFSIM_ICR1);

	/* check for timer 4 interrupts */
	if ((intp->int_isr & 0x01000000) != 0) {
		return;
	}

	/* reset timer */
	timerp->timer_ter = MCFTIMER_TER_CAP | MCFTIMER_TER_REF;
	timestamp ++;
}

void timer_init (void) {
	volatile timer_t *timerp = (timer_t *) (CFG_MBAR + MCFTIMER_BASE4);
	volatile intctrl_t *intp = (intctrl_t *) (CFG_MBAR + MCFSIM_ICR1);

	timestamp = 0;

	/* Set up TIMER 4 as clock */
	timerp->timer_tmr = MCFTIMER_TMR_DISABLE;

	/* initialize and enable timer 4 interrupt */
	irq_install_handler (72, mcf_timer_interrupt, 0);
	intp->int_icr1 |= 0x0000000d;

	timerp->timer_tcn = 0;
	timerp->timer_trr = 1000;	/* Interrupt every ms */
	/* set a period of 1us, set timer mode to restart and enable timer and interrupt */
	timerp->timer_tmr = (((CFG_CLK / 1000000) - 1)	<< 8) | MCFTIMER_TMR_CLK1 |
		MCFTIMER_TMR_RESTART | MCFTIMER_TMR_ENORI | MCFTIMER_TMR_ENABLE;
}

void reset_timer (void)
{
	timestamp = 0;
}

ulong get_timer (ulong base)
{
	return (timestamp - base);
}

void set_timer (ulong t)
{
	timestamp = t;
}
#endif

#if defined(CONFIG_M5282)

void udelay(unsigned long usec)
{
}

void timer_init (void)
{
	volatile unsigned short *timerp;

	timerp = (volatile unsigned short *) (CFG_MBAR + MCFTIMER_BASE4);
	timestamp = 0;

	/* Set up TIMER 4 as poll clock */
	timerp[MCFTIMER_PCSR] = MCFTIMER_PCSR_OVW;
	timerp[MCFTIMER_PMR] = lastinc = 0;
	timerp[MCFTIMER_PCSR] =
		(5 << 8) | MCFTIMER_PCSR_EN | MCFTIMER_PCSR_OVW;
}

void set_timer (ulong t)
{
	volatile unsigned short *timerp;

	timerp = (volatile unsigned short *) (CFG_MBAR + MCFTIMER_BASE4);
	timestamp = 0;
	timerp[MCFTIMER_PMR] = lastinc = 0;
}

ulong get_timer (ulong base)
{
	unsigned short now, diff;
	volatile unsigned short *timerp;

	timerp = (volatile unsigned short *) (CFG_MBAR + MCFTIMER_BASE4);
	now = timerp[MCFTIMER_PCNTR];
	diff = -(now - lastinc);

	timestamp += diff;
	lastinc = now;
	return timestamp - base;
}

void wait_ticks (unsigned long ticks)
{
	set_timer (0);
	while (get_timer (0) < ticks);
}
#endif


/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On M68K it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On M68K it returns the number of timer ticks per second.
 */
ulong get_tbclk (void)
{
	ulong tbclk;
	tbclk = CFG_HZ;
	return tbclk;
}

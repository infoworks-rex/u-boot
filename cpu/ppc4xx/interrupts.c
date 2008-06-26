/*
 * (C) Copyright 2000-2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002 (440 port)
 * Scott McNutt, Artesyn Communication Producs, smcnutt@artsyncp.com
 *
 * (C) Copyright 2003 (440GX port)
 * Travis B. Sawyer, Sandburst Corporation, tsawyer@sandburst.com
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
#include <watchdog.h>
#include <command.h>
#include <asm/processor.h>
#include <ppc4xx.h>
#include <ppc_asm.tmpl>
#include <commproc.h>

#if (UIC_MAX > 3)
#define UICB0_ALL	(UIC_MASK(VECNUM_UIC1CI) | UIC_MASK(VECNUM_UIC1NCI) | \
			 UIC_MASK(VECNUM_UIC2CI) | UIC_MASK(VECNUM_UIC2NCI) | \
			 UIC_MASK(VECNUM_UIC3CI) | UIC_MASK(VECNUM_UIC3NCI))
#elif (UIC_MAX > 2)
#define UICB0_ALL	(UIC_MASK(VECNUM_UIC1CI) | UIC_MASK(VECNUM_UIC1NCI) | \
			 UIC_MASK(VECNUM_UIC2CI) | UIC_MASK(VECNUM_UIC2NCI))
#elif (UIC_MAX > 1)
#define UICB0_ALL	(UIC_MASK(VECNUM_UIC1CI) | UIC_MASK(VECNUM_UIC1NCI))
#else
#define UICB0_ALL	0
#endif

DECLARE_GLOBAL_DATA_PTR;

/*
 * CPM interrupt vector functions.
 */
struct	irq_action {
	interrupt_handler_t *handler;
	void *arg;
	int count;
};

static struct irq_action irq_vecs[UIC_MAX * 32];

u32 get_dcr(u16);
void set_dcr(u16, u32);

#if defined(CONFIG_440)

/* SPRN changed in 440 */
static __inline__ void set_evpr(unsigned long val)
{
	asm volatile("mtspr 0x03f,%0" : : "r" (val));
}

#else /* !defined(CONFIG_440) */

static __inline__ void set_pit(unsigned long val)
{
	asm volatile("mtpit %0" : : "r" (val));
}


static __inline__ void set_tcr(unsigned long val)
{
	asm volatile("mttcr %0" : : "r" (val));
}


static __inline__ void set_evpr(unsigned long val)
{
	asm volatile("mtevpr %0" : : "r" (val));
}
#endif /* defined(CONFIG_440 */

int interrupt_init_cpu (unsigned *decrementer_count)
{
	int vec;
	unsigned long val;

	/* decrementer is automatically reloaded */
	*decrementer_count = 0;

	/*
	 * Mark all irqs as free
	 */
	for (vec = 0; vec < (UIC_MAX * 32); vec++) {
		irq_vecs[vec].handler = NULL;
		irq_vecs[vec].arg = NULL;
		irq_vecs[vec].count = 0;
	}

#ifdef CONFIG_4xx
	/*
	 * Init PIT
	 */
#if defined(CONFIG_440)
	val = mfspr( tcr );
	val &= (~0x04400000);		/* clear DIS & ARE */
	mtspr( tcr, val );
	mtspr( dec, 0 );		/* Prevent exception after TSR clear*/
	mtspr( decar, 0 );		/* clear reload */
	mtspr( tsr, 0x08000000 );	/* clear DEC status */
	val = gd->bd->bi_intfreq/1000;	/* 1 msec */
	mtspr( decar, val );		/* Set auto-reload value */
	mtspr( dec, val );		/* Set inital val */
#else
	set_pit(gd->bd->bi_intfreq / 1000);
#endif
#endif  /* CONFIG_4xx */

#ifdef CONFIG_ADCIOP
	/*
	 * Init PIT
	 */
	set_pit(66000);
#endif

	/*
	 * Enable PIT
	 */
	val = mfspr(tcr);
	val |= 0x04400000;
	mtspr(tcr, val);

	/*
	 * Set EVPR to 0
	 */
	set_evpr(0x00000000);

#if (UIC_MAX > 1)
	/* Install the UIC1 handlers */
	irq_install_handler(VECNUM_UIC1NCI, (void *)(void *)external_interrupt, 0);
	irq_install_handler(VECNUM_UIC1CI, (void *)(void *)external_interrupt, 0);
#endif
#if (UIC_MAX > 2)
	irq_install_handler(VECNUM_UIC2NCI, (void *)(void *)external_interrupt, 0);
	irq_install_handler(VECNUM_UIC2CI, (void *)(void *)external_interrupt, 0);
#endif
#if (UIC_MAX > 3)
	irq_install_handler(VECNUM_UIC3NCI, (void *)(void *)external_interrupt, 0);
	irq_install_handler(VECNUM_UIC3CI, (void *)(void *)external_interrupt, 0);
#endif

	return (0);
}

/* Handler for UIC interrupt */
static void uic_interrupt(u32 uic_base, int vec_base)
{
	u32 uic_msr;
	u32 msr_shift;
	int vec;

	/*
	 * Read masked interrupt status register to determine interrupt source
	 */
	uic_msr = get_dcr(uic_base + UIC_MSR);
	msr_shift = uic_msr;
	vec = vec_base;

	while (msr_shift != 0) {
		if (msr_shift & 0x80000000) {
			/*
			 * Increment irq counter (for debug purpose only)
			 */
			irq_vecs[vec].count++;

			if (irq_vecs[vec].handler != NULL) {
				/* call isr */
				(*irq_vecs[vec].handler)(irq_vecs[vec].arg);
			} else {
				set_dcr(uic_base + UIC_ER,
					get_dcr(uic_base + UIC_ER) & ~UIC_MASK(vec));
				printf("Masking bogus interrupt vector %d"
				       " (UIC_BASE=0x%x)\n", vec, uic_base);
			}

			/*
			 * After servicing the interrupt, we have to remove the
			 * status indicator
			 */
			set_dcr(uic_base + UIC_SR, UIC_MASK(vec));
		}

		/*
		 * Shift msr to next position and increment vector
		 */
		msr_shift <<= 1;
		vec++;
	}
}

/*
 * Handle external interrupts
 */
void external_interrupt(struct pt_regs *regs)
{
	u32 uic_msr;

	/*
	 * Read masked interrupt status register to determine interrupt source
	 */
	uic_msr = mfdcr(uic0msr);

#if (UIC_MAX > 1)
	if ((UIC_MASK(VECNUM_UIC1CI) & uic_msr) ||
	    (UIC_MASK(VECNUM_UIC1NCI) & uic_msr))
		uic_interrupt(UIC1_DCR_BASE, 32);
#endif

#if (UIC_MAX > 2)
	if ((UIC_MASK(VECNUM_UIC2CI) & uic_msr) ||
	    (UIC_MASK(VECNUM_UIC2NCI) & uic_msr))
		uic_interrupt(UIC2_DCR_BASE, 64);
#endif

#if (UIC_MAX > 3)
	if ((UIC_MASK(VECNUM_UIC3CI) & uic_msr) ||
	    (UIC_MASK(VECNUM_UIC3NCI) & uic_msr))
		uic_interrupt(UIC3_DCR_BASE, 96);
#endif

	if (uic_msr & ~(UICB0_ALL))
		uic_interrupt(UIC0_DCR_BASE, 0);

	mtdcr(uic0sr, uic_msr);

	return;
}

/*
 * Install and free a interrupt handler.
 */
void irq_install_handler(int vec, interrupt_handler_t * handler, void *arg)
{
	/*
	 * Print warning when replacing with a different irq vector
	 */
	if ((irq_vecs[vec].handler != NULL) && (irq_vecs[vec].handler != handler)) {
		printf("Interrupt vector %d: handler 0x%x replacing 0x%x\n",
		       vec, (uint) handler, (uint) irq_vecs[vec].handler);
	}
	irq_vecs[vec].handler = handler;
	irq_vecs[vec].arg = arg;

	if ((vec >= 0) && (vec < 32))
		mtdcr(uicer, mfdcr(uicer) | UIC_MASK(vec));
#if (UIC_MAX > 1)
	else if ((vec >= 32) && (vec < 64))
		mtdcr(uic1er, mfdcr(uic1er) | UIC_MASK(vec));
#endif
#if (UIC_MAX > 2)
	else if ((vec >= 64) && (vec < 96))
		mtdcr(uic2er, mfdcr(uic2er) | UIC_MASK(vec));
#endif
#if (UIC_MAX > 3)
	else if (vec >= 96)
		mtdcr(uic3er, mfdcr(uic3er) | UIC_MASK(vec));
#endif

	debug("Install interrupt for vector %d ==> %p\n", vec, handler);
}

void irq_free_handler (int vec)
{
	debug("Free interrupt for vector %d ==> %p\n",
	      vec, irq_vecs[vec].handler);

	if ((vec >= 0) && (vec < 32))
		mtdcr(uicer, mfdcr(uicer) & ~UIC_MASK(vec));
#if (UIC_MAX > 1)
	else if ((vec >= 32) && (vec < 64))
		mtdcr(uic1er, mfdcr(uic1er) & ~UIC_MASK(vec));
#endif
#if (UIC_MAX > 2)
	else if ((vec >= 64) && (vec < 96))
		mtdcr(uic2er, mfdcr(uic2er) & ~UIC_MASK(vec));
#endif
#if (UIC_MAX > 3)
	else if (vec >= 96)
		mtdcr(uic3er, mfdcr(uic3er) & ~UIC_MASK(vec));
#endif

	irq_vecs[vec].handler = NULL;
	irq_vecs[vec].arg = NULL;
}

void timer_interrupt_cpu (struct pt_regs *regs)
{
	/* nothing to do here */
	return;
}

#if defined(CONFIG_CMD_IRQ)
int do_irqinfo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int vec;

	printf ("Interrupt-Information:\n");
	printf ("Nr  Routine   Arg       Count\n");

	for (vec = 0; vec < (UIC_MAX * 32); vec++) {
		if (irq_vecs[vec].handler != NULL) {
			printf ("%02d  %08lx  %08lx  %d\n",
				vec,
				(ulong)irq_vecs[vec].handler,
				(ulong)irq_vecs[vec].arg,
				irq_vecs[vec].count);
		}
	}

	return 0;
}
#endif

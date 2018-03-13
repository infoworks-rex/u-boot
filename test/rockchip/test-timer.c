/*
 * (C) Copyright 2017 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <asm/io.h>
#include <common.h>
#include <irq-generic.h>
#include <irq-platform.h>
#include "test-rockchip.h"

#define TIMER_LOAD_COUNT0	0x00
#define TIMER_LOAD_COUNT1	0x04
#define TIMER_CTRL		0x10
#define TIMER_INTSTATUS		0x18

#define SYS_COUNTER_FREQ0	24000000
#define SYS_COUNTER_FREQ1	0

#define TIMER_EN		BIT(0)
#define TIMER_INT_EN		BIT(2)
#define TIMER_CLR_INT		BIT(0)

#if defined(CONFIG_ROCKCHIP_RK3128)
#define TIMER_BASE		(0x20044000 + 0x20)	/* TIMER 1 */
#define TIMER_IRQ		IRQ_TIMER1
#elif defined(CONFIG_ROCKCHIP_RK322X)
#define TIMER_BASE		(0x110C0000 + 0x20)	/* TIMER 1 */
#define TIMER_IRQ		IRQ_TIMER1
#elif defined(CONFIG_ROCKCHIP_RK3288)
#define TIMER_BASE		(0xFF6B0000 + 0x20)	/* TIMER 1 */
#define TIMER_IRQ		IRQ_TIMER1
#elif defined(CONFIG_ROCKCHIP_RK3328)
#define TIMER_BASE		(0xFF1C0000 + 0x20)	/* TIMER 1 */
#define TIMER_IRQ		IRQ_TIMER1
#elif defined(CONFIG_ROCKCHIP_RK3368)
#define TIMER_BASE		(0xFF810000 + 0x20)	/* TIMER 1 */
#define TIMER_IRQ		IRQ_TIMER1
#elif defined(CONFIG_ROCKCHIP_RK3399)
#define TIMER_BASE		(0xFF850000 + 0x20)	/* TIMER 1 */
#define TIMER_IRQ		IRQ_TIMER1
#elif defined(CONFIG_ROCKCHIP_PX30)
#define TIMER_BASE		(0xFF210000 + 0x20)	/* TIMER 1 */
#define TIMER_IRQ		IRQ_TIMER1
#else
"Missing definitions of timer module test"
#endif

/*************************** timer irq test ***********************************/
static ulong seconds;

/* must use volatile to avoid being optimized by complier */
static int volatile exit;

static void timer_irq_handler(int irq, void *data)
{
	static int count;
	int period;

	writel(TIMER_CLR_INT, TIMER_BASE + TIMER_INTSTATUS);

	period = get_timer(seconds);
	printf("timer_irq_handler: round-%d, irq=%d, period=%dms\n",
	       count++, irq, period);
	seconds = get_timer(0);

	if (count >= 5) {
		exit = 1;
		irq_free_handler(TIMER_IRQ);
		printf("timer_irq_handler: irq test finish.\n");
	}
}

static int soc_timer_irq_test_init(void)
{
	/* Disable before config */
	writel(0, TIMER_BASE + TIMER_CTRL);

	/* Config */
	writel(SYS_COUNTER_FREQ0, TIMER_BASE + TIMER_LOAD_COUNT0);
	writel(SYS_COUNTER_FREQ1, TIMER_BASE + TIMER_LOAD_COUNT1);
	writel(TIMER_CLR_INT, TIMER_BASE + TIMER_INTSTATUS);
	writel(TIMER_EN | TIMER_INT_EN, TIMER_BASE + TIMER_CTRL);

	/* Request irq */
	irq_install_handler(TIMER_IRQ, timer_irq_handler, NULL);
	irq_handler_enable(TIMER_IRQ);

	seconds = get_timer(0);

	while (!exit)
		;

	return 0;
}

/*************************** timer delay test *********************************/
static inline uint64_t arch_counter_get_cntpct(void)
{
	uint64_t cval;

	isb();
#ifdef CONFIG_ARM64
	asm volatile("mrs %0, cntpct_el0" : "=r" (cval));
#else
	asm volatile("mrrc p15, 0, %Q0, %R0, c14" : "=r" (cval));
#endif
	return cval;
}

static void sys_timer_delay_test_init(void)
{
	ulong delay_t = 100, cost_t;
	u64 tick_start;
	int i;

	for (i = 0; i < 5; i++) {
		printf("sys timer delay test, round-%d\n", i);

		/* us delay */
		tick_start = arch_counter_get_cntpct();
		udelay(delay_t);
		cost_t = (arch_counter_get_cntpct() - tick_start) / 24;
		printf("\tdesire delay %luus, actually delay %luus\n",
		       delay_t, cost_t);

		/* ms delay */
		tick_start = arch_counter_get_cntpct();
		mdelay(delay_t);
		cost_t = (arch_counter_get_cntpct() - tick_start) / 24000;

		printf("\tdesire delay %lums, actually delay: %lums\n",
		       delay_t, cost_t);

		/* ms delay */
		tick_start = arch_counter_get_cntpct();
		mdelay(delay_t * 10);
		cost_t = (arch_counter_get_cntpct() - tick_start) / 24000;

		printf("\tdesire delay %lums, actually delay: %lums\n",
		       delay_t * 10, cost_t);
	}
}

int board_timer_test(int argc, char * const argv[])
{
	sys_timer_delay_test_init();

	soc_timer_irq_test_init();

	return 0;
}

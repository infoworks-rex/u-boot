/*
 * Copyright (c) 2009 Wind River Systems, Inc.
 * Tom Rix <Tom.Rix@windriver.com>
 *
 * SPDX-License-Identifier:	GPL-2.0
 *
 * This work is derived from the linux 2.6.27 kernel source
 * To fetch, use the kernel repository
 * git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux-2.6.git
 * Use the v2.6.27 tag.
 *
 * Below is the original's header including its copyright
 *
 *  linux/arch/arm/plat-omap/gpio.c
 *
 * Support functions for OMAP GPIO
 *
 * Copyright (C) 2003-2005 Nokia Corporation
 * Written by Juha Yrjölä <juha.yrjola@nokia.com>
 */
#include <common.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/errno.h>

#define OMAP_GPIO_DIR_OUT	0
#define OMAP_GPIO_DIR_IN	1

static inline int get_gpio_index(int gpio)
{
	return gpio & 0x1f;
}

int gpio_is_valid(int gpio)
{
	return (gpio >= 0) && (gpio < OMAP_MAX_GPIO);
}

static void _set_gpio_direction(const struct gpio_bank *bank, int gpio,
				int is_input)
{
	void *reg = bank->base;
	u32 l;

	switch (bank->method) {
	case METHOD_GPIO_24XX:
		reg += OMAP_GPIO_OE;
		break;
	default:
		return;
	}
	l = __raw_readl(reg);
	if (is_input)
		l |= 1 << gpio;
	else
		l &= ~(1 << gpio);
	__raw_writel(l, reg);
}

/**
 * Get the direction of the GPIO by reading the GPIO_OE register
 * corresponding to the specified bank.
 */
static int _get_gpio_direction(const struct gpio_bank *bank, int gpio)
{
	void *reg = bank->base;
	u32 v;

	switch (bank->method) {
	case METHOD_GPIO_24XX:
		reg += OMAP_GPIO_OE;
		break;
	default:
		return -1;
	}

	v = __raw_readl(reg);

	if (v & (1 << gpio))
		return OMAP_GPIO_DIR_IN;
	else
		return OMAP_GPIO_DIR_OUT;
}

static void _set_gpio_dataout(const struct gpio_bank *bank, int gpio,
				int enable)
{
	void *reg = bank->base;
	u32 l = 0;

	switch (bank->method) {
	case METHOD_GPIO_24XX:
		if (enable)
			reg += OMAP_GPIO_SETDATAOUT;
		else
			reg += OMAP_GPIO_CLEARDATAOUT;
		l = 1 << gpio;
		break;
	default:
		printf("omap3-gpio unknown bank method %s %d\n",
		       __FILE__, __LINE__);
		return;
	}
	__raw_writel(l, reg);
}

static int _get_gpio_value(const struct gpio_bank *bank, int gpio)
{
	void *reg = bank->base;
	int input;

	switch (bank->method) {
	case METHOD_GPIO_24XX:
		input = _get_gpio_direction(bank, gpio);
		switch (input) {
		case OMAP_GPIO_DIR_IN:
			reg += OMAP_GPIO_DATAIN;
			break;
		case OMAP_GPIO_DIR_OUT:
			reg += OMAP_GPIO_DATAOUT;
			break;
		default:
			return -1;
		}
		break;
	default:
		return -1;
	}

	return (__raw_readl(reg) & (1 << gpio)) != 0;
}

static inline const struct gpio_bank *get_gpio_bank(int gpio)
{
	return &omap_gpio_bank[gpio >> 5];
}

static int check_gpio(int gpio)
{
	if (!gpio_is_valid(gpio)) {
		printf("ERROR : check_gpio: invalid GPIO %d\n", gpio);
		return -1;
	}
	return 0;
}

/**
 * Set value of the specified gpio
 */
int gpio_set_value(unsigned gpio, int value)
{
	const struct gpio_bank *bank;

	if (check_gpio(gpio) < 0)
		return -1;
	bank = get_gpio_bank(gpio);
	_set_gpio_dataout(bank, get_gpio_index(gpio), value);

	return 0;
}

/**
 * Get value of the specified gpio
 */
int gpio_get_value(unsigned gpio)
{
	const struct gpio_bank *bank;

	if (check_gpio(gpio) < 0)
		return -1;
	bank = get_gpio_bank(gpio);

	return _get_gpio_value(bank, get_gpio_index(gpio));
}

/**
 * Set gpio direction as input
 */
int gpio_direction_input(unsigned gpio)
{
	const struct gpio_bank *bank;

	if (check_gpio(gpio) < 0)
		return -1;

	bank = get_gpio_bank(gpio);
	_set_gpio_direction(bank, get_gpio_index(gpio), 1);

	return 0;
}

/**
 * Set gpio direction as output
 */
int gpio_direction_output(unsigned gpio, int value)
{
	const struct gpio_bank *bank;

	if (check_gpio(gpio) < 0)
		return -1;

	bank = get_gpio_bank(gpio);
	_set_gpio_dataout(bank, get_gpio_index(gpio), value);
	_set_gpio_direction(bank, get_gpio_index(gpio), 0);

	return 0;
}

/**
 * Request a gpio before using it.
 *
 * NOTE: Argument 'label' is unused.
 */
int gpio_request(unsigned gpio, const char *label)
{
	if (check_gpio(gpio) < 0)
		return -1;

	return 0;
}

/**
 * Reset and free the gpio after using it.
 */
int gpio_free(unsigned gpio)
{
	return 0;
}

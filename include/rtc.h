/*
 * (C) Copyright 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Generic RTC interface.
 */
#ifndef _RTC_H_
#define _RTC_H_

/* bcd<->bin functions are needed by almost all the RTC drivers, let's include
 * it there instead of in evey single driver */

#include <bcd.h>

/*
 * The struct used to pass data from the generic interface code to
 * the hardware dependend low-level code ande vice versa. Identical
 * to struct rtc_time used by the Linux kernel.
 *
 * Note that there are small but significant differences to the
 * common "struct time":
 *
 *		struct time:		struct rtc_time:
 * tm_mon	0 ... 11		1 ... 12
 * tm_year	years since 1900	years since 0
 */

struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

int rtc_get (struct rtc_time *);
int rtc_set (struct rtc_time *);
void rtc_reset (void);

unsigned long mktime (unsigned int, unsigned int, unsigned int,
		      unsigned int, unsigned int, unsigned int);

/**
 * rtc_read8() - Read an 8-bit register
 *
 * @reg:	Register to read
 * @return value read
 */
int rtc_read8(int reg);

/**
 * rtc_write8() - Write an 8-bit register
 *
 * @reg:	Register to write
 * @value:	Value to write
 */
void rtc_write8(int reg, uchar val);

/**
 * rtc_read32() - Read a 32-bit value from the RTC
 *
 * @reg:	Offset to start reading from
 * @return value read
 */
u32 rtc_read32(int reg);

/**
 * rtc_write32() - Write a 32-bit value to the RTC
 *
 * @reg:	Register to start writing to
 * @value:	Value to write
 */
void rtc_write32(int reg, u32 value);

/**
 * rtc_init() - Set up the real time clock ready for use
 */
void rtc_init(void);

/**
 * rtc_calc_weekday() - Work out the weekday from a time
 *
 * This only works for the Gregorian calendar - i.e. after 1752 (in the UK).
 * It sets time->tm_wdaay to the correct day of the week.
 *
 * @time:	Time to inspect. tm_wday is updated
 * @return 0 if OK, -EINVAL if the weekday could not be determined
 */
int rtc_calc_weekday(struct rtc_time *time);

/**
 * rtc_to_tm() - Convert a time_t value into a broken-out time
 *
 * The following fields are set up by this function:
 *	tm_sec, tm_min, tm_hour, tm_mday, tm_mon, tm_year, tm_wday
 *
 * Note that tm_yday and tm_isdst are set to 0.
 *
 * @time_t:	Number of seconds since 1970-01-01 00:00:00
 * @time:	Place to put the broken-out time
 * @return 0 if OK, -EINVAL if the weekday could not be determined
 */
int rtc_to_tm(int time_t, struct rtc_time *time);

#endif	/* _RTC_H_ */

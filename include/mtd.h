/*
 * Copyright (C) 2015 Thomas Chou <thomas@wytron.com.tw>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _MTD_H_
#define _MTD_H_

#include <linux/mtd/mtd.h>

int mtd_probe(struct udevice *dev);

#endif	/* _MTD_H_ */

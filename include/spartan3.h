/*
 * (C) Copyright 2002
 * Rich Ireland, Enterasys Networks, rireland@enterasys.com.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _SPARTAN3_H_
#define _SPARTAN3_H_

#include <xilinx.h>

int spartan3_load(Xilinx_desc *desc, const void *image, size_t size);
int spartan3_dump(Xilinx_desc *desc, const void *buf, size_t bsize);
int spartan3_info(Xilinx_desc *desc);

/* Slave Parallel Implementation function table */
typedef struct {
	Xilinx_pre_fn	pre;
	Xilinx_pgm_fn	pgm;
	Xilinx_init_fn	init;
	Xilinx_err_fn	err;
	Xilinx_done_fn	done;
	Xilinx_clk_fn	clk;
	Xilinx_cs_fn	cs;
	Xilinx_wr_fn	wr;
	Xilinx_rdata_fn	rdata;
	Xilinx_wdata_fn	wdata;
	Xilinx_busy_fn	busy;
	Xilinx_abort_fn	abort;
	Xilinx_post_fn	post;
} xilinx_spartan3_slave_parallel_fns;

/* Slave Serial Implementation function table */
typedef struct {
	Xilinx_pre_fn	pre;
	Xilinx_pgm_fn	pgm;
	Xilinx_clk_fn	clk;
	Xilinx_init_fn	init;
	Xilinx_done_fn	done;
	Xilinx_wr_fn	wr;
	Xilinx_post_fn	post;
	Xilinx_bwr_fn	bwr; /* block write function */
	Xilinx_abort_fn abort;
} xilinx_spartan3_slave_serial_fns;

/* Device Image Sizes
 *********************************************************************/
/* Spartan-III (1.2V) */
#define XILINX_XC3S50_SIZE	439264/8
#define XILINX_XC3S200_SIZE	1047616/8
#define XILINX_XC3S400_SIZE	1699136/8
#define XILINX_XC3S1000_SIZE	3223488/8
#define XILINX_XC3S1500_SIZE	5214784/8
#define XILINX_XC3S2000_SIZE	7673024/8
#define XILINX_XC3S4000_SIZE	11316864/8
#define XILINX_XC3S5000_SIZE	13271936/8

/* Spartan-3E (v3.4) */
#define	XILINX_XC3S100E_SIZE	581344/8
#define	XILINX_XC3S250E_SIZE	1353728/8
#define	XILINX_XC3S500E_SIZE	2270208/8
#define	XILINX_XC3S1200E_SIZE	3841184/8
#define	XILINX_XC3S1600E_SIZE	5969696/8

/*
 * Spartan-6 : the Spartan-6 family can be programmed
 * exactly as the Spartan-3
 */
#define XILINK_XC6SLX4_SIZE	(3713568/8)

/* Descriptor Macros
 *********************************************************************/
/* Spartan-III devices */
#define XILINX_XC3S50_DESC(iface, fn_table, cookie) \
{ xilinx_spartan3, iface, XILINX_XC3S50_SIZE, fn_table, cookie }

#define XILINX_XC3S200_DESC(iface, fn_table, cookie) \
{ xilinx_spartan3, iface, XILINX_XC3S200_SIZE, fn_table, cookie }

#define XILINX_XC3S400_DESC(iface, fn_table, cookie) \
{ xilinx_spartan3, iface, XILINX_XC3S400_SIZE, fn_table, cookie }

#define XILINX_XC3S1000_DESC(iface, fn_table, cookie) \
{ xilinx_spartan3, iface, XILINX_XC3S1000_SIZE, fn_table, cookie }

#define XILINX_XC3S1500_DESC(iface, fn_table, cookie) \
{ xilinx_spartan3, iface, XILINX_XC3S1500_SIZE, fn_table, cookie }

#define XILINX_XC3S2000_DESC(iface, fn_table, cookie) \
{ xilinx_spartan3, iface, XILINX_XC3S2000_SIZE, fn_table, cookie }

#define XILINX_XC3S4000_DESC(iface, fn_table, cookie) \
{ xilinx_spartan3, iface, XILINX_XC3S4000_SIZE, fn_table, cookie }

#define XILINX_XC3S5000_DESC(iface, fn_table, cookie) \
{ xilinx_spartan3, iface, XILINX_XC3S5000_SIZE, fn_table, cookie }

/* Spartan-3E devices */
#define XILINX_XC3S100E_DESC(iface, fn_table, cookie) \
{ xilinx_spartan3, iface, XILINX_XC3S100E_SIZE, fn_table, cookie }

#define XILINX_XC3S250E_DESC(iface, fn_table, cookie) \
{ xilinx_spartan3, iface, XILINX_XC3S250E_SIZE, fn_table, cookie }

#define XILINX_XC3S500E_DESC(iface, fn_table, cookie) \
{ xilinx_spartan3, iface, XILINX_XC3S500E_SIZE, fn_table, cookie }

#define XILINX_XC3S1200E_DESC(iface, fn_table, cookie) \
{ xilinx_spartan3, iface, XILINX_XC3S1200E_SIZE, fn_table, cookie }

#define XILINX_XC3S1600E_DESC(iface, fn_table, cookie) \
{ xilinx_spartan3, iface, XILINX_XC3S1600E_SIZE, fn_table, cookie }

#define XILINX_XC6SLX4_DESC(iface, fn_table, cookie) \
{ xilinx_spartan3, iface, XILINK_XC6SLX4_SIZE, fn_table, cookie }

#endif /* _SPARTAN3_H_ */

/*
 * Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Define _STDBOOL_H here to avoid macro expansion of true and false.
 * If the future code requires macro true or false, remove this define
 * and undef true and false before U_BOOT_CMD. This define and comment
 * shall be removed if change to U_BOOT_CMD is made to take string
 * instead of stringifying it.
 */
#define _STDBOOL_H

#include <common.h>
#include <command.h>

#define OP_INVALID	0
#define OP_OR		2
#define OP_AND		3
#define OP_STR_EMPTY	4
#define OP_STR_NEMPTY	5
#define OP_STR_EQ	6
#define OP_STR_NEQ	7
#define OP_STR_LT	8
#define OP_STR_GT	9
#define OP_INT_EQ	10
#define OP_INT_NEQ	11
#define OP_INT_LT	12
#define OP_INT_LE	13
#define OP_INT_GT	14
#define OP_INT_GE	15

const struct {
	int arg;
	const char *str;
	int op;
	int adv;
} op_adv[] = {
	{0, "-o", OP_OR, 1},
	{0, "-a", OP_AND, 1},
	{0, "-z", OP_STR_EMPTY, 2},
	{0, "-n", OP_STR_NEMPTY, 2},
	{1, "=", OP_STR_EQ, 3},
	{1, "!=", OP_STR_NEQ, 3},
	{1, "<", OP_STR_LT, 3},
	{1, ">", OP_STR_GT, 3},
	{1, "-eq", OP_INT_EQ, 3},
	{1, "-ne", OP_INT_NEQ, 3},
	{1, "-lt", OP_INT_LT, 3},
	{1, "-le", OP_INT_LE, 3},
	{1, "-gt", OP_INT_GT, 3},
	{1, "-ge", OP_INT_GE, 3},
};

static int do_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char * const *ap;
	int i, op, left, adv, expr, last_expr, neg, last_cmp;

	/* args? */
	if (argc < 3)
		return 1;

#ifdef DEBUG
	{
		debug("test(%d):", argc);
		left = 1;
		while (argv[left])
			debug(" '%s'", argv[left++]);
	}
#endif

	last_expr = 0;
	left = argc - 1; ap = argv + 1;
	if (left > 0 && strcmp(ap[0], "!") == 0) {
		neg = 1;
		ap++;
		left--;
	} else
		neg = 0;

	expr = -1;
	last_cmp = OP_INVALID;
	last_expr = -1;
	while (left > 0) {
		for (i = 0; i < ARRAY_SIZE(op_adv); i++) {
			if (left <= op_adv[i].arg)
				continue;
			if (!strcmp(ap[op_adv[i].arg], op_adv[i].str)) {
				op = op_adv[i].op;
				adv = op_adv[i].adv;
				break;
			}
		}
		if (i == ARRAY_SIZE(op_adv)) {
			expr = 1;
			break;
		}
		if (left < adv) {
			expr = 1;
			break;
		}

		switch (op) {
		case OP_STR_EMPTY:
			expr = strlen(ap[1]) == 0 ? 1 : 0;
			break;
		case OP_STR_NEMPTY:
			expr = strlen(ap[1]) == 0 ? 0 : 1;
			break;
		case OP_STR_EQ:
			expr = strcmp(ap[0], ap[2]) == 0;
			break;
		case OP_STR_NEQ:
			expr = strcmp(ap[0], ap[2]) != 0;
			break;
		case OP_STR_LT:
			expr = strcmp(ap[0], ap[2]) < 0;
			break;
		case OP_STR_GT:
			expr = strcmp(ap[0], ap[2]) > 0;
			break;
		case OP_INT_EQ:
			expr = simple_strtol(ap[0], NULL, 10) ==
					simple_strtol(ap[2], NULL, 10);
			break;
		case OP_INT_NEQ:
			expr = simple_strtol(ap[0], NULL, 10) !=
					simple_strtol(ap[2], NULL, 10);
			break;
		case OP_INT_LT:
			expr = simple_strtol(ap[0], NULL, 10) <
					simple_strtol(ap[2], NULL, 10);
			break;
		case OP_INT_LE:
			expr = simple_strtol(ap[0], NULL, 10) <=
					simple_strtol(ap[2], NULL, 10);
			break;
		case OP_INT_GT:
			expr = simple_strtol(ap[0], NULL, 10) >
					simple_strtol(ap[2], NULL, 10);
			break;
		case OP_INT_GE:
			expr = simple_strtol(ap[0], NULL, 10) >=
					simple_strtol(ap[2], NULL, 10);
			break;
		}

		switch (op) {
		case OP_OR:
			last_expr = expr;
			last_cmp = OP_OR;
			break;
		case OP_AND:
			last_expr = expr;
			last_cmp = OP_AND;
			break;
		default:
			if (last_cmp == OP_OR)
				expr = last_expr || expr;
			else if (last_cmp == OP_AND)
				expr = last_expr && expr;
			last_cmp = OP_INVALID;
			break;
		}

		ap += adv; left -= adv;
	}

	if (neg)
		expr = !expr;

	expr = !expr;

	debug (": returns %d\n", expr);

	return expr;
}

U_BOOT_CMD(
	test,	CONFIG_SYS_MAXARGS,	1,	do_test,
	"minimal test like /bin/sh",
	"[args..]"
);

static int do_false(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	return 1;
}

U_BOOT_CMD(
	false,	CONFIG_SYS_MAXARGS,	1,	do_false,
	"do nothing, unsuccessfully",
	NULL
);

static int do_true(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	return 0;
}

U_BOOT_CMD(
	true,	CONFIG_SYS_MAXARGS,	1,	do_true,
	"do nothing, successfully",
	NULL
);

/*
 * (C) Copyright 2004-2008 Texas Instruments, <www.ti.com>
 * Rohit Choraria <rohitkc@ti.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <asm/arch/mem.h>
#include <asm/arch/cpu.h>
#include <asm/omap_gpmc.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/bch.h>
#include <linux/compiler.h>
#include <nand.h>
#include <asm/omap_elm.h>

#define BADBLOCK_MARKER_LENGTH	2
#define SECTOR_BYTES		512
#define ECCCLEAR		(0x1 << 8)
#define ECCRESULTREG1		(0x1 << 0)
#ifdef CONFIG_BCH
static u8  bch8_polynomial[] = {0xef, 0x51, 0x2e, 0x09, 0xed, 0x93, 0x9a, 0xc2,
				0x97, 0x79, 0xe5, 0x24, 0xb5};
#endif
static uint8_t cs;
static __maybe_unused struct nand_ecclayout omap_ecclayout;

/*
 * omap_nand_hwcontrol - Set the address pointers corretly for the
 *			following address/data/command operation
 */
static void omap_nand_hwcontrol(struct mtd_info *mtd, int32_t cmd,
				uint32_t ctrl)
{
	register struct nand_chip *this = mtd->priv;

	/*
	 * Point the IO_ADDR to DATA and ADDRESS registers instead
	 * of chip address
	 */
	switch (ctrl) {
	case NAND_CTRL_CHANGE | NAND_CTRL_CLE:
		this->IO_ADDR_W = (void __iomem *)&gpmc_cfg->cs[cs].nand_cmd;
		break;
	case NAND_CTRL_CHANGE | NAND_CTRL_ALE:
		this->IO_ADDR_W = (void __iomem *)&gpmc_cfg->cs[cs].nand_adr;
		break;
	case NAND_CTRL_CHANGE | NAND_NCE:
		this->IO_ADDR_W = (void __iomem *)&gpmc_cfg->cs[cs].nand_dat;
		break;
	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, this->IO_ADDR_W);
}

#ifdef CONFIG_SPL_BUILD
/* Check wait pin as dev ready indicator */
int omap_spl_dev_ready(struct mtd_info *mtd)
{
	return gpmc_cfg->status & (1 << 8);
}
#endif


/*
 * gen_true_ecc - This function will generate true ECC value, which
 * can be used when correcting data read from NAND flash memory core
 *
 * @ecc_buf:	buffer to store ecc code
 *
 * @return:	re-formatted ECC value
 */
static uint32_t gen_true_ecc(uint8_t *ecc_buf)
{
	return ecc_buf[0] | (ecc_buf[1] << 16) | ((ecc_buf[2] & 0xF0) << 20) |
		((ecc_buf[2] & 0x0F) << 8);
}

/*
 * omap_correct_data - Compares the ecc read from nand spare area with ECC
 * registers values and corrects one bit error if it has occured
 * Further details can be had from OMAP TRM and the following selected links:
 * http://en.wikipedia.org/wiki/Hamming_code
 * http://www.cs.utexas.edu/users/plaxton/c/337/05f/slides/ErrorCorrection-4.pdf
 *
 * @mtd:		 MTD device structure
 * @dat:		 page data
 * @read_ecc:		 ecc read from nand flash
 * @calc_ecc:		 ecc read from ECC registers
 *
 * @return 0 if data is OK or corrected, else returns -1
 */
static int __maybe_unused omap_correct_data(struct mtd_info *mtd, uint8_t *dat,
				uint8_t *read_ecc, uint8_t *calc_ecc)
{
	uint32_t orig_ecc, new_ecc, res, hm;
	uint16_t parity_bits, byte;
	uint8_t bit;

	/* Regenerate the orginal ECC */
	orig_ecc = gen_true_ecc(read_ecc);
	new_ecc = gen_true_ecc(calc_ecc);
	/* Get the XOR of real ecc */
	res = orig_ecc ^ new_ecc;
	if (res) {
		/* Get the hamming width */
		hm = hweight32(res);
		/* Single bit errors can be corrected! */
		if (hm == 12) {
			/* Correctable data! */
			parity_bits = res >> 16;
			bit = (parity_bits & 0x7);
			byte = (parity_bits >> 3) & 0x1FF;
			/* Flip the bit to correct */
			dat[byte] ^= (0x1 << bit);
		} else if (hm == 1) {
			printf("Error: Ecc is wrong\n");
			/* ECC itself is corrupted */
			return 2;
		} else {
			/*
			 * hm distance != parity pairs OR one, could mean 2 bit
			 * error OR potentially be on a blank page..
			 * orig_ecc: contains spare area data from nand flash.
			 * new_ecc: generated ecc while reading data area.
			 * Note: if the ecc = 0, all data bits from which it was
			 * generated are 0xFF.
			 * The 3 byte(24 bits) ecc is generated per 512byte
			 * chunk of a page. If orig_ecc(from spare area)
			 * is 0xFF && new_ecc(computed now from data area)=0x0,
			 * this means that data area is 0xFF and spare area is
			 * 0xFF. A sure sign of a erased page!
			 */
			if ((orig_ecc == 0x0FFF0FFF) && (new_ecc == 0x00000000))
				return 0;
			printf("Error: Bad compare! failed\n");
			/* detected 2 bit error */
			return -1;
		}
	}
	return 0;
}

/*
 * Generic BCH interface
 */
struct nand_bch_priv {
	uint8_t mode;
	uint8_t type;
	uint8_t nibbles;
	struct bch_control *control;
	enum omap_ecc ecc_scheme;
};

/* bch types */
#define ECC_BCH4	0
#define ECC_BCH8	1
#define ECC_BCH16	2

/* BCH nibbles for diff bch levels */
#define ECC_BCH4_NIBBLES	13
#define ECC_BCH8_NIBBLES	26
#define ECC_BCH16_NIBBLES	52

/*
 * This can be a single instance cause all current users have only one NAND
 * with nearly the same setup (BCH8, some with ELM and others with sw BCH
 * library).
 * When some users with other BCH strength will exists this have to change!
 */
static __maybe_unused struct nand_bch_priv bch_priv = {
	.type = ECC_BCH8,
	.nibbles = ECC_BCH8_NIBBLES,
	.control = NULL
};

/*
 * omap_enable_hwecc - configures GPMC as per ECC scheme before read/write
 * @mtd:	MTD device structure
 * @mode:	Read/Write mode
 */
__maybe_unused
static void omap_enable_hwecc(struct mtd_info *mtd, int32_t mode)
{
	struct nand_chip	*nand	= mtd->priv;
	struct nand_bch_priv	*bch	= nand->priv;
	unsigned int dev_width = (nand->options & NAND_BUSWIDTH_16) ? 1 : 0;
	unsigned int ecc_algo = 0;
	unsigned int bch_type = 0;
	unsigned int eccsize1 = 0x00, eccsize0 = 0x00, bch_wrapmode = 0x00;
	u32 ecc_size_config_val = 0;
	u32 ecc_config_val = 0;

	/* configure GPMC for specific ecc-scheme */
	switch (bch->ecc_scheme) {
	case OMAP_ECC_HAM1_CODE_SW:
		return;
	case OMAP_ECC_HAM1_CODE_HW:
		ecc_algo = 0x0;
		bch_type = 0x0;
		bch_wrapmode = 0x00;
		eccsize0 = 0xFF;
		eccsize1 = 0xFF;
		break;
	case OMAP_ECC_BCH8_CODE_HW_DETECTION_SW:
	case OMAP_ECC_BCH8_CODE_HW:
		ecc_algo = 0x1;
		bch_type = 0x1;
		if (mode == NAND_ECC_WRITE) {
			bch_wrapmode = 0x01;
			eccsize0 = 0;  /* extra bits in nibbles per sector */
			eccsize1 = 28; /* OOB bits in nibbles per sector */
		} else {
			bch_wrapmode = 0x01;
			eccsize0 = 26; /* ECC bits in nibbles per sector */
			eccsize1 = 2;  /* non-ECC bits in nibbles per sector */
		}
		break;
	default:
		return;
	}
	/* Clear ecc and enable bits */
	writel(ECCCLEAR | ECCRESULTREG1, &gpmc_cfg->ecc_control);
	/* Configure ecc size for BCH */
	ecc_size_config_val = (eccsize1 << 22) | (eccsize0 << 12);
	writel(ecc_size_config_val, &gpmc_cfg->ecc_size_config);

	/* Configure device details for BCH engine */
	ecc_config_val = ((ecc_algo << 16)	| /* HAM1 | BCHx */
			(bch_type << 12)	| /* BCH4/BCH8/BCH16 */
			(bch_wrapmode << 8)	| /* wrap mode */
			(dev_width << 7)	| /* bus width */
			(0x0 << 4)		| /* number of sectors */
			(cs <<  1)		| /* ECC CS */
			(0x1));			  /* enable ECC */
	writel(ecc_config_val, &gpmc_cfg->ecc_config);
}

/*
 *  omap_calculate_ecc - Read ECC result
 *  @mtd:	MTD structure
 *  @dat:	unused
 *  @ecc_code:	ecc_code buffer
 *  Using noninverted ECC can be considered ugly since writing a blank
 *  page ie. padding will clear the ECC bytes. This is no problem as
 *  long nobody is trying to write data on the seemingly unused page.
 *  Reading an erased page will produce an ECC mismatch between
 *  generated and read ECC bytes that has to be dealt with separately.
 *  E.g. if page is 0xFF (fresh erased), and if HW ECC engine within GPMC
 *  is used, the result of read will be 0x0 while the ECC offsets of the
 *  spare area will be 0xFF which will result in an ECC mismatch.
 */
static int omap_calculate_ecc(struct mtd_info *mtd, const uint8_t *dat,
				uint8_t *ecc_code)
{
	struct nand_chip *chip = mtd->priv;
	struct nand_bch_priv *bch = chip->priv;
	uint32_t *ptr, val = 0;
	int8_t i = 0, j;

	switch (bch->ecc_scheme) {
	case OMAP_ECC_HAM1_CODE_HW:
		val = readl(&gpmc_cfg->ecc1_result);
		ecc_code[0] = val & 0xFF;
		ecc_code[1] = (val >> 16) & 0xFF;
		ecc_code[2] = ((val >> 8) & 0x0F) | ((val >> 20) & 0xF0);
		break;
#ifdef CONFIG_BCH
	case OMAP_ECC_BCH8_CODE_HW_DETECTION_SW:
#endif
	case OMAP_ECC_BCH8_CODE_HW:
		ptr = &gpmc_cfg->bch_result_0_3[0].bch_result_x[3];
		val = readl(ptr);
		ecc_code[i++] = (val >>  0) & 0xFF;
		ptr--;
		for (j = 0; j < 3; j++) {
			val = readl(ptr);
			ecc_code[i++] = (val >> 24) & 0xFF;
			ecc_code[i++] = (val >> 16) & 0xFF;
			ecc_code[i++] = (val >>  8) & 0xFF;
			ecc_code[i++] = (val >>  0) & 0xFF;
			ptr--;
		}
		break;
	default:
		return -EINVAL;
	}
	/* ECC scheme specific syndrome customizations */
	switch (bch->ecc_scheme) {
	case OMAP_ECC_HAM1_CODE_HW:
		break;
#ifdef CONFIG_BCH
	case OMAP_ECC_BCH8_CODE_HW_DETECTION_SW:

		for (i = 0; i < chip->ecc.bytes; i++)
			*(ecc_code + i) = *(ecc_code + i) ^
						bch8_polynomial[i];
		break;
#endif
	case OMAP_ECC_BCH8_CODE_HW:
		ecc_code[chip->ecc.bytes - 1] = 0x00;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

#ifdef CONFIG_NAND_OMAP_ELM
/*
 * omap_rotate_ecc_bch - Rotate the syndrome bytes
 *
 * @mtd:	MTD device structure
 * @calc_ecc:	ECC read from ECC registers
 * @syndrome:	Rotated syndrome will be retuned in this array
 *
 */
static void omap_rotate_ecc_bch(struct mtd_info *mtd, uint8_t *calc_ecc,
		uint8_t *syndrome)
{
	struct nand_chip *chip = mtd->priv;
	struct nand_bch_priv *bch = chip->priv;
	uint8_t n_bytes = 0;
	int8_t i, j;

	switch (bch->type) {
	case ECC_BCH4:
		n_bytes = 8;
		break;

	case ECC_BCH16:
		n_bytes = 28;
		break;

	case ECC_BCH8:
	default:
		n_bytes = 13;
		break;
	}

	for (i = 0, j = (n_bytes-1); i < n_bytes; i++, j--)
		syndrome[i] =  calc_ecc[j];
}

/*
 * omap_fix_errors_bch - Correct bch error in the data
 *
 * @mtd:	MTD device structure
 * @data:	Data read from flash
 * @error_count:Number of errors in data
 * @error_loc:	Locations of errors in the data
 *
 */
static void omap_fix_errors_bch(struct mtd_info *mtd, uint8_t *data,
		uint32_t error_count, uint32_t *error_loc)
{
	struct nand_chip *chip = mtd->priv;
	struct nand_bch_priv *bch = chip->priv;
	uint8_t count = 0;
	uint32_t error_byte_pos;
	uint32_t error_bit_mask;
	uint32_t last_bit = (bch->nibbles * 4) - 1;

	/* Flip all bits as specified by the error location array. */
	/* FOR( each found error location flip the bit ) */
	for (count = 0; count < error_count; count++) {
		if (error_loc[count] > last_bit) {
			/* Remove the ECC spare bits from correction. */
			error_loc[count] -= (last_bit + 1);
			/* Offset bit in data region */
			error_byte_pos = ((512 * 8) -
					(error_loc[count]) - 1) / 8;
			/* Error Bit mask */
			error_bit_mask = 0x1 << (error_loc[count] % 8);
			/* Toggle the error bit to make the correction. */
			data[error_byte_pos] ^= error_bit_mask;
		}
	}
}

/*
 * omap_correct_data_bch - Compares the ecc read from nand spare area
 * with ECC registers values and corrects one bit error if it has occured
 *
 * @mtd:	MTD device structure
 * @dat:	page data
 * @read_ecc:	ecc read from nand flash (ignored)
 * @calc_ecc:	ecc read from ECC registers
 *
 * @return 0 if data is OK or corrected, else returns -1
 */
static int omap_correct_data_bch(struct mtd_info *mtd, uint8_t *dat,
				uint8_t *read_ecc, uint8_t *calc_ecc)
{
	struct nand_chip *chip = mtd->priv;
	struct nand_bch_priv *bch = chip->priv;
	uint8_t syndrome[28];
	uint32_t error_count = 0;
	uint32_t error_loc[8];
	uint32_t i, ecc_flag;

	ecc_flag = 0;
	for (i = 0; i < chip->ecc.bytes; i++)
		if (read_ecc[i] != 0xff)
			ecc_flag = 1;

	if (!ecc_flag)
		return 0;

	elm_reset();
	elm_config((enum bch_level)(bch->type));

	/*
	 * while reading ECC result we read it in big endian.
	 * Hence while loading to ELM we have rotate to get the right endian.
	 */
	omap_rotate_ecc_bch(mtd, calc_ecc, syndrome);

	/* use elm module to check for errors */
	if (elm_check_error(syndrome, bch->nibbles, &error_count,
				error_loc) != 0) {
		printf("ECC: uncorrectable.\n");
		return -1;
	}

	/* correct bch error */
	if (error_count > 0)
		omap_fix_errors_bch(mtd, dat, error_count, error_loc);

	return 0;
}

/**
 * omap_read_page_bch - hardware ecc based page read function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	buffer to store read data
 * @oob_required: caller expects OOB data read to chip->oob_poi
 * @page:	page number to read
 *
 */
static int omap_read_page_bch(struct mtd_info *mtd, struct nand_chip *chip,
				uint8_t *buf, int oob_required, int page)
{
	int i, eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	uint8_t *p = buf;
	uint8_t *ecc_calc = chip->buffers->ecccalc;
	uint8_t *ecc_code = chip->buffers->ecccode;
	uint32_t *eccpos = chip->ecc.layout->eccpos;
	uint8_t *oob = chip->oob_poi;
	uint32_t data_pos;
	uint32_t oob_pos;

	data_pos = 0;
	/* oob area start */
	oob_pos = (eccsize * eccsteps) + chip->ecc.layout->eccpos[0];
	oob += chip->ecc.layout->eccpos[0];

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize,
				oob += eccbytes) {
		chip->ecc.hwctl(mtd, NAND_ECC_READ);
		/* read data */
		chip->cmdfunc(mtd, NAND_CMD_RNDOUT, data_pos, page);
		chip->read_buf(mtd, p, eccsize);

		/* read respective ecc from oob area */
		chip->cmdfunc(mtd, NAND_CMD_RNDOUT, oob_pos, page);
		chip->read_buf(mtd, oob, eccbytes);
		/* read syndrome */
		chip->ecc.calculate(mtd, p, &ecc_calc[i]);

		data_pos += eccsize;
		oob_pos += eccbytes;
	}

	for (i = 0; i < chip->ecc.total; i++)
		ecc_code[i] = chip->oob_poi[eccpos[i]];

	eccsteps = chip->ecc.steps;
	p = buf;

	for (i = 0 ; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		int stat;

		stat = chip->ecc.correct(mtd, p, &ecc_code[i], &ecc_calc[i]);
		if (stat < 0)
			mtd->ecc_stats.failed++;
		else
			mtd->ecc_stats.corrected += stat;
	}
	return 0;
}
#endif /* CONFIG_NAND_OMAP_ELM */

/*
 * OMAP3 BCH8 support (with BCH library)
 */
#ifdef CONFIG_BCH
/**
 * omap_correct_data_bch_sw - Decode received data and correct errors
 * @mtd: MTD device structure
 * @data: page data
 * @read_ecc: ecc read from nand flash
 * @calc_ecc: ecc read from HW ECC registers
 */
static int omap_correct_data_bch_sw(struct mtd_info *mtd, u_char *data,
				 u_char *read_ecc, u_char *calc_ecc)
{
	int i, count;
	/* cannot correct more than 8 errors */
	unsigned int errloc[8];
	struct nand_chip *chip = mtd->priv;
	struct nand_bch_priv *chip_priv = chip->priv;
	struct bch_control *bch = chip_priv->control;

	count = decode_bch(bch, NULL, 512, read_ecc, calc_ecc, NULL, errloc);
	if (count > 0) {
		/* correct errors */
		for (i = 0; i < count; i++) {
			/* correct data only, not ecc bytes */
			if (errloc[i] < 8*512)
				data[errloc[i]/8] ^= 1 << (errloc[i] & 7);
			printf("corrected bitflip %u\n", errloc[i]);
#ifdef DEBUG
			puts("read_ecc: ");
			/*
			 * BCH8 have 13 bytes of ECC; BCH4 needs adoption
			 * here!
			 */
			for (i = 0; i < 13; i++)
				printf("%02x ", read_ecc[i]);
			puts("\n");
			puts("calc_ecc: ");
			for (i = 0; i < 13; i++)
				printf("%02x ", calc_ecc[i]);
			puts("\n");
#endif
		}
	} else if (count < 0) {
		puts("ecc unrecoverable error\n");
	}
	return count;
}

/**
 * omap_free_bch - Release BCH ecc resources
 * @mtd: MTD device structure
 */
static void __maybe_unused omap_free_bch(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct nand_bch_priv *chip_priv = chip->priv;
	struct bch_control *bch = NULL;

	if (chip_priv)
		bch = chip_priv->control;

	if (bch) {
		free_bch(bch);
		chip_priv->control = NULL;
	}
}
#endif /* CONFIG_BCH */

/**
 * omap_select_ecc_scheme - configures driver for particular ecc-scheme
 * @nand: NAND chip device structure
 * @ecc_scheme: ecc scheme to configure
 * @pagesize: number of main-area bytes per page of NAND device
 * @oobsize: number of OOB/spare bytes per page of NAND device
 */
static int omap_select_ecc_scheme(struct nand_chip *nand,
	enum omap_ecc ecc_scheme, unsigned int pagesize, unsigned int oobsize) {
	struct nand_bch_priv	*bch		= nand->priv;
	struct nand_ecclayout	*ecclayout	= &omap_ecclayout;
	int eccsteps = pagesize / SECTOR_BYTES;
	int i;

	switch (ecc_scheme) {
	case OMAP_ECC_HAM1_CODE_SW:
		debug("nand: selected OMAP_ECC_HAM1_CODE_SW\n");
		/* For this ecc-scheme, ecc.bytes, ecc.layout, ... are
		 * initialized in nand_scan_tail(), so just set ecc.mode */
		bch_priv.control	= NULL;
		bch_priv.type		= 0;
		nand->ecc.mode		= NAND_ECC_SOFT;
		nand->ecc.layout	= NULL;
		nand->ecc.size		= 0;
		bch->ecc_scheme		= OMAP_ECC_HAM1_CODE_SW;
		break;

	case OMAP_ECC_HAM1_CODE_HW:
		debug("nand: selected OMAP_ECC_HAM1_CODE_HW\n");
		/* check ecc-scheme requirements before updating ecc info */
		if ((3 * eccsteps) + BADBLOCK_MARKER_LENGTH > oobsize) {
			printf("nand: error: insufficient OOB: require=%d\n", (
				(3 * eccsteps) + BADBLOCK_MARKER_LENGTH));
			return -EINVAL;
		}
		bch_priv.control	= NULL;
		bch_priv.type		= 0;
		/* populate ecc specific fields */
		memset(&nand->ecc, 0, sizeof(struct nand_ecc_ctrl));
		nand->ecc.mode		= NAND_ECC_HW;
		nand->ecc.strength	= 1;
		nand->ecc.size		= SECTOR_BYTES;
		nand->ecc.bytes		= 3;
		nand->ecc.hwctl		= omap_enable_hwecc;
		nand->ecc.correct	= omap_correct_data;
		nand->ecc.calculate	= omap_calculate_ecc;
		/* define ecc-layout */
		ecclayout->eccbytes	= nand->ecc.bytes * eccsteps;
		for (i = 0; i < ecclayout->eccbytes; i++) {
			if (nand->options & NAND_BUSWIDTH_16)
				ecclayout->eccpos[i] = i + 2;
			else
				ecclayout->eccpos[i] = i + 1;
		}
		ecclayout->oobfree[0].offset = i + BADBLOCK_MARKER_LENGTH;
		ecclayout->oobfree[0].length = oobsize - ecclayout->eccbytes -
						BADBLOCK_MARKER_LENGTH;
		bch->ecc_scheme		= OMAP_ECC_HAM1_CODE_HW;
		break;

	case OMAP_ECC_BCH8_CODE_HW_DETECTION_SW:
#ifdef CONFIG_BCH
		debug("nand: selected OMAP_ECC_BCH8_CODE_HW_DETECTION_SW\n");
		/* check ecc-scheme requirements before updating ecc info */
		if ((13 * eccsteps) + BADBLOCK_MARKER_LENGTH > oobsize) {
			printf("nand: error: insufficient OOB: require=%d\n", (
				(13 * eccsteps) + BADBLOCK_MARKER_LENGTH));
			return -EINVAL;
		}
		/* check if BCH S/W library can be used for error detection */
		bch_priv.control = init_bch(13, 8, 0x201b);
		if (!bch_priv.control) {
			printf("nand: error: could not init_bch()\n");
			return -ENODEV;
		}
		bch_priv.type = ECC_BCH8;
		/* populate ecc specific fields */
		memset(&nand->ecc, 0, sizeof(struct nand_ecc_ctrl));
		nand->ecc.mode		= NAND_ECC_HW;
		nand->ecc.strength	= 8;
		nand->ecc.size		= SECTOR_BYTES;
		nand->ecc.bytes		= 13;
		nand->ecc.hwctl		= omap_enable_hwecc;
		nand->ecc.correct	= omap_correct_data_bch_sw;
		nand->ecc.calculate	= omap_calculate_ecc;
		/* define ecc-layout */
		ecclayout->eccbytes	= nand->ecc.bytes * eccsteps;
		ecclayout->eccpos[0]	= BADBLOCK_MARKER_LENGTH;
		for (i = 1; i < ecclayout->eccbytes; i++) {
			if (i % nand->ecc.bytes)
				ecclayout->eccpos[i] =
						ecclayout->eccpos[i - 1] + 1;
			else
				ecclayout->eccpos[i] =
						ecclayout->eccpos[i - 1] + 2;
		}
		ecclayout->oobfree[0].offset = i + BADBLOCK_MARKER_LENGTH;
		ecclayout->oobfree[0].length = oobsize - ecclayout->eccbytes -
						BADBLOCK_MARKER_LENGTH;
		bch->ecc_scheme		= OMAP_ECC_BCH8_CODE_HW_DETECTION_SW;
		break;
#else
		printf("nand: error: CONFIG_BCH required for ECC\n");
		return -EINVAL;
#endif

	case OMAP_ECC_BCH8_CODE_HW:
#ifdef CONFIG_NAND_OMAP_ELM
		debug("nand: selected OMAP_ECC_BCH8_CODE_HW\n");
		/* check ecc-scheme requirements before updating ecc info */
		if ((14 * eccsteps) + BADBLOCK_MARKER_LENGTH > oobsize) {
			printf("nand: error: insufficient OOB: require=%d\n", (
				(14 * eccsteps) + BADBLOCK_MARKER_LENGTH));
			return -EINVAL;
		}
		/* intialize ELM for ECC error detection */
		elm_init();
		bch_priv.type		= ECC_BCH8;
		/* populate ecc specific fields */
		memset(&nand->ecc, 0, sizeof(struct nand_ecc_ctrl));
		nand->ecc.mode		= NAND_ECC_HW;
		nand->ecc.strength	= 8;
		nand->ecc.size		= SECTOR_BYTES;
		nand->ecc.bytes		= 14;
		nand->ecc.hwctl		= omap_enable_hwecc;
		nand->ecc.correct	= omap_correct_data_bch;
		nand->ecc.calculate	= omap_calculate_ecc;
		nand->ecc.read_page	= omap_read_page_bch;
		/* define ecc-layout */
		ecclayout->eccbytes	= nand->ecc.bytes * eccsteps;
		for (i = 0; i < ecclayout->eccbytes; i++)
			ecclayout->eccpos[i] = i + BADBLOCK_MARKER_LENGTH;
		ecclayout->oobfree[0].offset = i + BADBLOCK_MARKER_LENGTH;
		ecclayout->oobfree[0].length = oobsize - ecclayout->eccbytes -
						BADBLOCK_MARKER_LENGTH;
		bch->ecc_scheme		= OMAP_ECC_BCH8_CODE_HW;
		break;
#else
		printf("nand: error: CONFIG_NAND_OMAP_ELM required for ECC\n");
		return -EINVAL;
#endif

	default:
		debug("nand: error: ecc scheme not enabled or supported\n");
		return -EINVAL;
	}

	/* nand_scan_tail() sets ham1 sw ecc; hw ecc layout is set by driver */
	if (ecc_scheme != OMAP_ECC_HAM1_CODE_SW)
		nand->ecc.layout = ecclayout;

	return 0;
}

#ifndef CONFIG_SPL_BUILD
/*
 * omap_nand_switch_ecc - switch the ECC operation between different engines
 * (h/w and s/w) and different algorithms (hamming and BCHx)
 *
 * @hardware		- true if one of the HW engines should be used
 * @eccstrength		- the number of bits that could be corrected
 *			  (1 - hamming, 4 - BCH4, 8 - BCH8, 16 - BCH16)
 */
int __maybe_unused omap_nand_switch_ecc(uint32_t hardware, uint32_t eccstrength)
{
	struct nand_chip *nand;
	struct mtd_info *mtd;
	int err = 0;

	if (nand_curr_device < 0 ||
	    nand_curr_device >= CONFIG_SYS_MAX_NAND_DEVICE ||
	    !nand_info[nand_curr_device].name) {
		printf("nand: error: no NAND devices found\n");
		return -ENODEV;
	}

	mtd = &nand_info[nand_curr_device];
	nand = mtd->priv;
	nand->options |= NAND_OWN_BUFFERS;
	nand->options &= ~NAND_SUBPAGE_READ;
	/* Setup the ecc configurations again */
	if (hardware) {
		if (eccstrength == 1) {
			err = omap_select_ecc_scheme(nand,
					OMAP_ECC_HAM1_CODE_HW,
					mtd->writesize, mtd->oobsize);
		} else if (eccstrength == 8) {
			err = omap_select_ecc_scheme(nand,
					OMAP_ECC_BCH8_CODE_HW,
					mtd->writesize, mtd->oobsize);
		} else {
			printf("nand: error: unsupported ECC scheme\n");
			return -EINVAL;
		}
	} else {
		err = omap_select_ecc_scheme(nand, OMAP_ECC_HAM1_CODE_SW,
					mtd->writesize, mtd->oobsize);
	}

	/* Update NAND handling after ECC mode switch */
	if (!err)
		err = nand_scan_tail(mtd);
	return err;
}
#endif /* CONFIG_SPL_BUILD */

/*
 * Board-specific NAND initialization. The following members of the
 * argument are board-specific:
 * - IO_ADDR_R: address to read the 8 I/O lines of the flash device
 * - IO_ADDR_W: address to write the 8 I/O lines of the flash device
 * - cmd_ctrl: hardwarespecific function for accesing control-lines
 * - waitfunc: hardwarespecific function for accesing device ready/busy line
 * - ecc.hwctl: function to enable (reset) hardware ecc generator
 * - ecc.mode: mode of ecc, see defines
 * - chip_delay: chip dependent delay for transfering data from array to
 *   read regs (tR)
 * - options: various chip options. They can partly be set to inform
 *   nand_scan about special functionality. See the defines for further
 *   explanation
 */
int board_nand_init(struct nand_chip *nand)
{
	int32_t gpmc_config = 0;
	cs = 0;
	int err = 0;
	/*
	 * xloader/Uboot's gpmc configuration would have configured GPMC for
	 * nand type of memory. The following logic scans and latches on to the
	 * first CS with NAND type memory.
	 * TBD: need to make this logic generic to handle multiple CS NAND
	 * devices.
	 */
	while (cs < GPMC_MAX_CS) {
		/* Check if NAND type is set */
		if ((readl(&gpmc_cfg->cs[cs].config1) & 0xC00) == 0x800) {
			/* Found it!! */
			break;
		}
		cs++;
	}
	if (cs >= GPMC_MAX_CS) {
		printf("nand: error: Unable to find NAND settings in "
			"GPMC Configuration - quitting\n");
		return -ENODEV;
	}

	gpmc_config = readl(&gpmc_cfg->config);
	/* Disable Write protect */
	gpmc_config |= 0x10;
	writel(gpmc_config, &gpmc_cfg->config);

	nand->IO_ADDR_R = (void __iomem *)&gpmc_cfg->cs[cs].nand_dat;
	nand->IO_ADDR_W = (void __iomem *)&gpmc_cfg->cs[cs].nand_cmd;
	nand->priv	= &bch_priv;
	nand->cmd_ctrl	= omap_nand_hwcontrol;
	nand->options	|= NAND_NO_PADDING | NAND_CACHEPRG;
	/* If we are 16 bit dev, our gpmc config tells us that */
	if ((readl(&gpmc_cfg->cs[cs].config1) & 0x3000) == 0x1000)
		nand->options |= NAND_BUSWIDTH_16;

	nand->chip_delay = 100;
	nand->ecc.layout = &omap_ecclayout;

	/* select ECC scheme */
#if defined(CONFIG_NAND_OMAP_ECCSCHEME)
	err = omap_select_ecc_scheme(nand, CONFIG_NAND_OMAP_ECCSCHEME,
			CONFIG_SYS_NAND_PAGE_SIZE, CONFIG_SYS_NAND_OOBSIZE);
#else
	/* pagesize and oobsize are not required to configure sw ecc-scheme */
	err = omap_select_ecc_scheme(nand, OMAP_ECC_HAM1_CODE_SW,
			0, 0);
#endif
	if (err)
		return err;

#ifdef CONFIG_SPL_BUILD
	if (nand->options & NAND_BUSWIDTH_16)
		nand->read_buf = nand_read_buf16;
	else
		nand->read_buf = nand_read_buf;
	nand->dev_ready = omap_spl_dev_ready;
#endif

	return 0;
}

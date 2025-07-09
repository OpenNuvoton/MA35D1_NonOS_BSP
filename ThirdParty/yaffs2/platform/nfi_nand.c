/**************************************************************************//**
 * @file     nfi_nand.c
 *
 * @brief    NFI driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdlib.h>
#include "ma35d1.h"
#include "stdio.h"
#include "nand.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup NAND_Driver NAND Driver
  @{
*/

/** @addtogroup NAND_EXPORTED_CONSTANTS NAND Exported Constants
  @{
*/
/// @cond HIDDEN_SYMBOLS


#define NAND_EN     0x08
#define READYBUSY   (0x01 << 18)
#define ENDADDR     (0x80000000)

/*-----------------------------------------------------------------------------
 * Define some constants for BCH
 *---------------------------------------------------------------------------*/
// define the total padding bytes for 512/1024 data segment
#define BCH_PADDING_LEN_512     32
#define BCH_PADDING_LEN_1024    64
// define the BCH parity code length for 512 bytes data pattern
#define BCH_PARITY_LEN_T8  15
#define BCH_PARITY_LEN_T12 23
// define the BCH parity code length for 1024 bytes data pattern
#define BCH_PARITY_LEN_T24 45

#define BCH_T8    0x00100000
#define BCH_T12   0x00200000
#define BCH_T24   0x00040000

struct nuvoton_nand_info {
    struct nand_hw_control  controller;
    struct mtd_info         mtd;
    struct nand_chip        chip;
    int                     eBCHAlgo;
    int                     m_i32SMRASize;
};
struct nuvoton_nand_info g_nuvoton_nand;
struct nuvoton_nand_info *nuvoton_nand;

static struct nand_ecclayout nuvoton_nand_oob;

static const int g_i32BCHAlgoIdx[4] = {BCH_T8, BCH_T8, BCH_T12, BCH_T24};
static const int g_i32ParityNum[3][4] = {
    { 0,  60,   92,   90  },  // For 2K
    { 0,  120,  184,  180 },  // For 4K
    { 0,  240,  368,  360 },  // For 8K
};

extern uint32_t volatile msTicks0;
void udelay(unsigned int tick)
{
    int volatile start;
    start = msTicks0;
    while(1)
        if ((msTicks0 - start) > tick)
            break;
}

static void nuvoton_layout_oob_table(struct nand_ecclayout* pNandOOBTbl, int oobsize, int eccbytes)
{
    pNandOOBTbl->eccbytes = eccbytes;

    pNandOOBTbl->oobavail = oobsize - 4 - eccbytes;

    pNandOOBTbl->oobfree[0].offset = 4;  // Bad block marker size

    pNandOOBTbl->oobfree[0].length = oobsize - eccbytes - pNandOOBTbl->oobfree[0].offset;
}


static void nuvoton_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
    struct nand_chip *chip = mtd->priv;

    if (ctrl & NAND_CTRL_CHANGE) {
        ulong IO_ADDR_W = (ulong)(NAND_BASE+0x8B8);

        if ((ctrl & NAND_CLE))
            IO_ADDR_W = (NAND_BASE+0x8B0);
        if ((ctrl & NAND_ALE))
            IO_ADDR_W = (NAND_BASE+0x8B4);

        chip->IO_ADDR_W = (void *)IO_ADDR_W;
    }

    if (cmd != NAND_CMD_NONE)
        outpb(chip->IO_ADDR_W, cmd);
}

/* select chip */
static void nuvoton_nand_select_chip(struct mtd_info *mtd, int chip)
{
	if (chip == 0)
		NFI->NANDCTL &= (~0x02000000);
	else
		NFI->NANDCTL |= 0x02000000;
}

static int nuvoton_dev_ready(struct mtd_info *mtd)
{
    return ((NFI->NANDINTSTS & READYBUSY) ? 1 : 0);
}


static int nuvoton_waitfunc(struct mtd_info *mtd, struct nand_chip *chip)
{
	struct ma35d1_nand_info *nand = nand_get_controller_data(chip);
	unsigned long time;
	int status = -1;

	time = msTicks0;
	while (1) {
		if (NFI->NANDINTSTS & 0x400) {	/* check r/b# flag */
			NFI->NANDINTSTS = 0x400;
			status = 0;
			break;
		}
		if ((msTicks0 - time) > 3000) {
			break;
		}
	}
	return status;
}

static void nuvoton_nand_command(struct mtd_info *mtd, unsigned int command, int column, int page_addr)
{
    register struct nand_chip *chip = mtd->priv;

	NFI->NANDINTSTS = 0x400;

	if (command == NAND_CMD_READOOB) {
		column += mtd->writesize;
		command = NAND_CMD_READ0;
	}

	switch (command) {

	case NAND_CMD_RESET:
		NFI->NANDCMD = command;
		nuvoton_waitfunc(mtd, chip);
		break;

	case NAND_CMD_READID:
		NFI->NANDCMD = command;
		NFI->NANDADDR = ENDADDR|column;
		break;

	case NAND_CMD_PARAM:
		NFI->NANDCMD = command;
		NFI->NANDADDR = ENDADDR|column;
		nuvoton_waitfunc(mtd, chip);
		break;

	case NAND_CMD_READ0:
		NFI->NANDCMD = command;
		if (column != -1) {
			NFI->NANDADDR = column & 0xff;
			NFI->NANDADDR = (column >> 8) & 0xff;
		}
		if (page_addr != -1) {
			NFI->NANDADDR = page_addr & 0xff;
			if (chip->options & NAND_ROW_ADDR_3) {
				NFI->NANDADDR = (page_addr >> 8) & 0xff;
				NFI->NANDADDR = ((page_addr >> 16) & 0xff)|ENDADDR;
			} else
				NFI->NANDADDR = ((page_addr >> 8) & 0xff)|ENDADDR;
		}
		NFI->NANDCMD = NAND_CMD_READSTART;
		nuvoton_waitfunc(mtd, chip);
		break;


	case NAND_CMD_ERASE1:
		NFI->NANDCMD = command;
		NFI->NANDADDR = page_addr & 0xff;
		if (chip->options & NAND_ROW_ADDR_3) {
			NFI->NANDADDR = (page_addr >> 8) & 0xff;
			NFI->NANDADDR = ((page_addr >> 16) & 0xff)|ENDADDR;
		} else
			NFI->NANDADDR = ((page_addr >> 8) & 0xff)|ENDADDR;
		break;


	case NAND_CMD_SEQIN:
		NFI->NANDCMD = command;
		NFI->NANDADDR = column & 0xff;
		NFI->NANDADDR = (column >> 8) & 0xff;
		NFI->NANDADDR = page_addr & 0xff;
		if (chip->options & NAND_ROW_ADDR_3) {
			NFI->NANDADDR = (page_addr >> 8) & 0xff;
			NFI->NANDADDR = ((page_addr >> 16) & 0xff)|ENDADDR;
		} else
			NFI->NANDADDR = ((page_addr >> 8) & 0xff)|ENDADDR;
		break;

	default:
		NFI->NANDCMD = command;
	}
}

/*
 * nuvoton_nand_read_byte - read a byte from NAND controller into buffer
 * @mtd: MTD device structure
 */
static unsigned char nuvoton_nand_read_byte(struct mtd_info *mtd)
{
    return ((unsigned char)NFI->NANDDATA);
}

/*
 * nuvoton_nand_write_buf - write data from buffer into NAND controller
 * @mtd: MTD device structure
 * @buf: virtual address in RAM of source
 * @len: number of data bytes to be transferred
 */

static void nuvoton_nand_write_buf(struct mtd_info *mtd, const unsigned char *buf, int len)
{
    int i;

    for (i = 0; i < len; i++)
    	NFI->NANDDATA = buf[i];
}

/*
 * nuvoton_nand_read_buf - read data from NAND controller into buffer
 * @mtd: MTD device structure
 * @buf: virtual address in RAM of source
 * @len: number of data bytes to be transferred
 */
static void nuvoton_nand_read_buf(struct mtd_info *mtd, unsigned char *buf, int len)
{
    int i;

    for (i = 0; i < len; i++)
        buf[i] = (unsigned char)NFI->NANDDATA;
}


/*
 * Enable HW ECC : unused on most chips
 */
void nuvoton_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
}

/*
 * Calculate HW ECC
 * function called after a write
 * mtd:        MTD block structure
 * dat:        raw data (unused)
 * ecc_code:   buffer for ECC
 */
static int nuvoton_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat, u_char *ecc_code)
{
    return 0;
}

/*
 * HW ECC Correction
 * function called after a read
 * mtd:        MTD block structure
 * dat:        raw data read from the chip
 * read_ecc:   ECC from the chip (unused)
 * isnull:     unused
 */
static int nuvoton_nand_correct_data(struct mtd_info *mtd, u_char *dat,
                     u_char *read_ecc, u_char *calc_ecc)
{
    return 0;
}


/*-----------------------------------------------------------------------------
 * Correct data by BCH algorithm.
 *      Support 8K page size NAND and BCH T8/12/24.
 *---------------------------------------------------------------------------*/
void nuvoton_correctdata_BCH(u8 ucFieidIndex, u8 ucErrorCnt, u8* pDAddr)
{
    u32 uaData[24], uaAddr[24];
    u32 uaErrorData[6];
    u8  ii, jj;
    u32 uPageSize;
    u32 field_len, padding_len, parity_len;
    u32 total_field_num;
    u8  *smra_index;

    //--- assign some parameters for different BCH and page size
    switch (NFI->NANDCTL & 0x007C0000)
    {
        case BCH_T24:
            field_len   = 1024;
            padding_len = BCH_PADDING_LEN_1024;
            parity_len  = BCH_PARITY_LEN_T24;
            break;
        case BCH_T12:
            field_len   = 512;
            padding_len = BCH_PADDING_LEN_512;
            parity_len  = BCH_PARITY_LEN_T12;
            break;
        case BCH_T8:
            field_len   = 512;
            padding_len = BCH_PADDING_LEN_512;
            parity_len  = BCH_PARITY_LEN_T8;
            break;
        default:
            return;
    }

    uPageSize = NFI->NANDCTL & 0x00030000;
    switch (uPageSize)
    {
        case 0x30000: total_field_num = 8192 / field_len; break;
        case 0x20000: total_field_num = 4096 / field_len; break;
        case 0x10000: total_field_num = 2048 / field_len; break;
        case 0x00000: total_field_num =  512 / field_len; break;
        default:
            return;
    }

    //--- got valid BCH_ECC_DATAx and parse them to uaData[]
    // got the valid register number of BCH_ECC_DATAx since one register include 4 error bytes
    jj = ucErrorCnt/4;
    jj ++;
    if (jj > 6)
        jj = 6;     // there are 6 BCH_ECC_DATAx registers to support BCH T24

    for(ii=0; ii<jj; ii++)
    {
        uaErrorData[ii] = inpw(NAND_BASE + 0x960 + ii*4);
    }

    for(ii=0; ii<jj; ii++)
    {
        uaData[ii*4+0] = uaErrorData[ii] & 0xff;
        uaData[ii*4+1] = (uaErrorData[ii]>>8) & 0xff;
        uaData[ii*4+2] = (uaErrorData[ii]>>16) & 0xff;
        uaData[ii*4+3] = (uaErrorData[ii]>>24) & 0xff;
    }

    //--- got valid REG_BCH_ECC_ADDRx and parse them to uaAddr[]
    // got the valid register number of REG_BCH_ECC_ADDRx since one register include 2 error addresses
    jj = ucErrorCnt/2;
    jj ++;
    if (jj > 12)
        jj = 12;    // there are 12 REG_BCH_ECC_ADDRx registers to support BCH T24

    for(ii=0; ii<jj; ii++)
    {
        uaAddr[ii*2+0] = inpw(NAND_BASE + 0x900 + ii*4) & 0x07ff;   // 11 bits for error address
        uaAddr[ii*2+1] = (inpw(NAND_BASE + 0x900 + ii*4)>>16) & 0x07ff;
    }

    //--- pointer to begin address of field that with data error
    pDAddr += (ucFieidIndex-1) * field_len;

    //--- correct each error bytes
    for(ii=0; ii<ucErrorCnt; ii++)
    {
        // for wrong data in field
        if (uaAddr[ii] < field_len)
        {
            *(pDAddr+uaAddr[ii]) ^= uaData[ii];
        }
        // for wrong first-3-bytes in redundancy area
        else if (uaAddr[ii] < (field_len+3))
        {
            uaAddr[ii] -= field_len;
            uaAddr[ii] += (parity_len*(ucFieidIndex-1));    // field offset
            *((u8 *)(NAND_BASE + 0xA00) + uaAddr[ii]) ^= uaData[ii];
        }
        // for wrong parity code in redundancy area
        else
        {
            // BCH_ERR_ADDRx = [data in field] + [3 bytes] + [xx] + [parity code]
            //                                   |<--     padding bytes      -->|
            // The BCH_ERR_ADDRx for last parity code always = field size + padding size.
            // So, the first parity code = field size + padding size - parity code length.
            // For example, for BCH T12, the first parity code = 512 + 32 - 23 = 521.
            // That is, error byte address offset within field is
            uaAddr[ii] = uaAddr[ii] - (field_len + padding_len - parity_len);

            // smra_index point to the first parity code of first field in register SMRA0~n
            smra_index = (u8 *)
                         (NAND_BASE + 0xA00 + (NFI->NANDRACTL & 0x1ff) - // bottom of all parity code -
                          (parity_len * total_field_num)                             // byte count of all parity code
                         );

            // final address = first parity code of first field +
            //                 offset of fields +
            //                 offset within field
            *((u8 *)smra_index + (parity_len * (ucFieidIndex-1)) + uaAddr[ii]) ^= uaData[ii];
        }
    }   // end of for (ii<ucErrorCnt)
}

int nuvoton_CorrectData(struct mtd_info *mtd, unsigned long uDAddr)
{
    int uStatus, ii, jj, i32FieldNum=0;
    volatile int uErrorCnt = 0, maxbitflips = 0;

    if (NFI->NANDINTSTS & 0x4)
    {
        if ((NFI->NANDCTL & 0x7C0000) == BCH_T24)
            i32FieldNum = mtd->writesize / 1024;    // Block=1024 for BCH
        else
            i32FieldNum = mtd->writesize / 512;

        if (i32FieldNum < 4)
            i32FieldNum  = 1;
        else
            i32FieldNum /= 4;

        for (jj=0; jj<i32FieldNum; jj++)
        {
            uStatus = inpw (NAND_BASE + 0x8D0 + jj*4);
            if (!uStatus)
                continue;

            for (ii=1; ii<5; ii++)
            {
                if (!(uStatus & 0x03)) { // No error

                    uStatus >>= 8;
                    continue;

                } else if ((uStatus & 0x03) == 0x01) { // Correctable error

                    uErrorCnt = (uStatus >> 2) & 0x1F;
                    nuvoton_correctdata_BCH(jj*4+ii, uErrorCnt, (u8 *)uDAddr);
                    maxbitflips = max_t(u32, maxbitflips, uErrorCnt);
                    mtd->ecc_stats.corrected += uErrorCnt;
                } else { // uncorrectable error or ECC error
                	mtd->ecc_stats.failed++;
                	return -EBADMSG;
                }
                uStatus >>= 8;
            }
        }
    }
    return maxbitflips;
}


static inline int nuvoton_nand_dma_transfer(struct mtd_info *mtd, const u_char *addr, unsigned int len, int is_write)
{
    struct nuvoton_nand_info *nand = nuvoton_nand;
    int cnt = 0;

    // For save, wait DMAC to ready
    while (NFI->DMACTL & 0x200);

    // DMAC enable
    NFI->DMACTL |= 0x3;
    while (NFI->DMACTL & 0x2);

    // Fill dma_addr
    dcache_clean_invalidate_by_mva(addr, len);
    NFI->DMASA = (unsigned long)addr;

    NFI->NANDRACTL = nand->m_i32SMRASize;

    // Enable SM_CS0
    NFI->NANDCTL = (NFI->NANDCTL & (~0x06000000)) | 0x04000000;
    /* setup and start DMA using dma_addr */

    if (is_write) {
        register char *ptr = (char *)(NAND_BASE+0xA00);
        // To mark this page as dirty.
        if ((*(ptr+3) & 0xff) == 0xff)
        	*(ptr+3) = 0;
        if ((*(ptr+2) & 0xff) == 0xff)
        	*(ptr+2) = 0;

        NFI->NANDCTL |= 0x4;
        while (!(NFI->NANDINTSTS & 0x1));

    } else {
        // Blocking for reading
        // Enable DMA Read
    	NFI->NANDCTL |= 0x2;

        if (NFI->NANDCTL & 0x80) {
            do {
                int stat=0;

                cnt = nuvoton_CorrectData(mtd, (unsigned long)addr);
                if (cnt < 0) {
                    NFI->NANDINTSTS = 0x4;
                    NFI->DMACTL = 0x3;          // reset DMAC
                    NFI->NANDCTL |= 0x1;
                    break;
                }
                else {
                	NFI->NANDINTSTS = 0x4;
                }

            } while (!(NFI->NANDINTSTS & 0x1) || (NFI->NANDINTSTS & 0x4));
        } else
            while (!(NFI->NANDINTSTS & 0x1));
    }
    dcache_clean_invalidate_by_mva(addr, len);

    // Clear DMA finished flag
    NFI->NANDINTSTS |= 0x1;

    return cnt;
}

/**
 * Raw page write function
 *
 * @param mtd	mtd info structure
 * @param chip	nand chip info structure
 * @param buf	data buffer
 */
static int nuvoton_nand_write_page_raw(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf, int oob_required, int page)
{
	nuvoton_nand_command(mtd, NAND_CMD_SEQIN, 0, page);
	nuvoton_nand_write_buf(mtd, buf, mtd->writesize);
	nuvoton_nand_command(mtd, NAND_CMD_PAGEPROG, -1, -1);
	nuvoton_waitfunc(mtd, chip);

	return 0;
}


/**
 * nand_write_page_hwecc - [REPLACABLE] hardware ecc based page write function
 * @mtd:        mtd info structure
 * @chip:       nand chip info structure
 * @buf:        data buffer
 */
static int nuvoton_nand_write_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf, int oob_required, int page)
{
    uint8_t *ecc_calc = chip->buffers->ecccalc;
    register char * ptr = (char *)(NAND_BASE+0xA00);
    int volatile i;

    memset((void*)ptr, 0xFF, mtd->oobsize);
    memcpy((void*)ptr, (void *)chip->oob_poi,  mtd->oobsize - chip->ecc.total);

	nuvoton_nand_command(mtd, NAND_CMD_SEQIN, 0, page);
	nuvoton_nand_dma_transfer(mtd, buf, mtd->writesize, 0x1);
	nuvoton_nand_command(mtd, NAND_CMD_PAGEPROG, -1, -1);
	nuvoton_waitfunc(mtd, chip);

	// Copy parity code in SMRA to calc
	//memcpy((void *)ecc_calc, (void *)((long)ptr + (mtd->oobsize - chip->ecc.total)), chip->ecc.total);
	for (i=0; i<chip->ecc.total; i++)
		*(ecc_calc+i) = *(uint8_t *)((long)ptr + (mtd->oobsize - chip->ecc.total));

	// Copy parity code in calc to oob_poi
	memcpy((void *)(chip->oob_poi + (mtd->oobsize-chip->ecc.total)), (void *)ecc_calc, chip->ecc.total);

    return 0;
}

/**
 * Read raw page data without ecc
 *
 * @param mtd	mtd info structure
 * @param chip	nand chip info structure
 * @param buf	buffer to store read data
 * @param page	page number to read
 * @return	0 when successfully completed
 *		-EINVAL when chip->oob_poi is not double-word aligned
 *		-EIO when command timeout
 */
static int nuvoton_nand_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf, int oob_required, int page)
{
	// read data from nand
	nuvoton_nand_command(mtd, NAND_CMD_READ0, 0, page);
	nuvoton_nand_read_buf(mtd, buf, mtd->writesize);

	return 0;
}

/**
 * nuvoton_nand_read_page_hwecc_oob_first - hardware ecc based page write function
 * @mtd:        mtd info structure
 * @chip:       nand chip info structure
 * @buf:        buffer to store read data
 * @page:       page number to read
 */
static int nuvoton_nand_read_page_hwecc_oob_first(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf, int oob_required, int page)
{
    uint8_t *p = buf;
    char * ptr= (char *)(NAND_BASE+0xA00);
    int bitflips = 0;

    /* At first, read the OOB area  */
    nuvoton_nand_command(mtd, NAND_CMD_READOOB, 0, page);
    nuvoton_nand_read_buf(mtd, chip->oob_poi, mtd->oobsize);

    // Second, copy OOB data to SMRA for page read
    memcpy((void*)ptr, (void*)chip->oob_poi, mtd->oobsize);

	if ((*(ptr+2) != 0) && (*(ptr+3) != 0))
		memset((void *)p, 0xff, mtd->writesize);
	else {
		// Third, read data from nand
		nuvoton_nand_command(mtd, NAND_CMD_READ0, 0, page);
		bitflips = nuvoton_nand_dma_transfer(mtd, p, mtd->writesize, 0x0);

		// Fourth, restore OOB data from SMRA
		memcpy((void *)chip->oob_poi, (void *)ptr, mtd->oobsize);
	}

    return bitflips;
}

/**
 * nuvoton_nand_read_oob_hwecc - [REPLACABLE] the most common OOB data read function
 * @mtd:        mtd info structure
 * @chip:       nand chip info structure
 * @page:       page number to read
 * @sndcmd:     flag whether to issue read command or not
 */
static int nuvoton_nand_read_oob_hwecc(struct mtd_info *mtd, struct nand_chip *chip, int page)
{
    char * ptr = (char *)(NAND_BASE+0xA00);

    /* At first, read the OOB area  */
    nuvoton_nand_command(mtd, NAND_CMD_READOOB, 0, page);
    nuvoton_nand_read_buf(mtd, chip->oob_poi, mtd->oobsize);

    // Second, copy OOB data to SMRA for page read
    memcpy((void*)ptr, (void*)chip->oob_poi, mtd->oobsize);

	//if ((*(ptr+2) != 0) && (*(ptr+3) != 0))
	//	memset((void *)chip->oob_poi, 0xff, mtd->oobsize);

	return 0;
}

static int nuvoton_nand_write_oob(struct mtd_info *mtd, struct nand_chip *chip, int page)
{
	nuvoton_nand_command(mtd, NAND_CMD_SEQIN, mtd->writesize, page);
    nuvoton_nand_write_buf(mtd, chip->oob_poi, mtd->oobsize);
	nuvoton_nand_command(mtd, NAND_CMD_PAGEPROG, -1, -1);
	nuvoton_waitfunc(mtd, chip);

	return 0;
}

int board_nand_init(struct nand_chip *nand)
{
    struct mtd_info *mtd;

    nuvoton_nand = &g_nuvoton_nand;
    memset((void*)nuvoton_nand, 0, sizeof(struct nuvoton_nand_info));

    if (!nuvoton_nand)
        return -1;

    mtd = &nuvoton_nand->mtd;
    nuvoton_nand->chip.controller = &nuvoton_nand->controller;

    /* initialize nand_chip data structure */
    nand->IO_ADDR_R = (void *)(NAND_BASE+0x8B8);
    nand->IO_ADDR_W = (void *)(NAND_BASE+0x8B8);

    /* hwcontrol always must be implemented */
	nand->cmd_ctrl = nuvoton_hwcontrol;
	nand->cmdfunc = nuvoton_nand_command;
	nand->waitfunc = nuvoton_waitfunc;
	nand->dev_ready = nuvoton_dev_ready;
	nand->select_chip = nuvoton_nand_select_chip;

    nand->read_byte = nuvoton_nand_read_byte;
    nand->write_buf = nuvoton_nand_write_buf;
    nand->read_buf = nuvoton_nand_read_buf;
    nand->chip_delay = 50;

    nand->controller = &nuvoton_nand->controller;

    nand->ecc.options    |= NAND_ECC_CUSTOM_PAGE_ACCESS;
    nand->ecc.mode       = NAND_ECC_HW_OOB_FIRST;
    nand->ecc.hwctl      = nuvoton_nand_enable_hwecc;
    nand->ecc.calculate  = nuvoton_nand_calculate_ecc;
    nand->ecc.correct    = nuvoton_nand_correct_data;
    nand->ecc.write_page = nuvoton_nand_write_page_hwecc;
    nand->ecc.read_page  = nuvoton_nand_read_page_hwecc_oob_first;
    nand->ecc.read_oob   = nuvoton_nand_read_oob_hwecc;
    nand->ecc.write_oob  = nuvoton_nand_write_oob;
    nand->ecc.layout     = &nuvoton_nand_oob;
	nand->ecc.write_page_raw = nuvoton_nand_write_page_raw;
	nand->ecc.read_page_raw  = nuvoton_nand_read_page_raw;
	nand->options |= NAND_NO_SUBPAGE_WRITE;

	/* setup by user */
	nand->ecc.strength   = 8;
	nand->ecc.size = 512;

	mtd = nand_to_mtd(nand);

    mtd->priv = nand;

    // Enable SM_EN
    NFI->GCTL = NAND_EN;

    // Enable SM_CS0
    NFI->NANDCTL = (NFI->NANDCTL & (~0x06000000)) | 0x04000000;
    NFI->NANDECTL = 0x1; /* un-lock write protect */

    // NAND Reset
    NFI->NANDCTL |= 0x1;    // software reset
    while (NFI->NANDCTL & 0x1);

    /* Detect NAND chips */
	if (nand_scan(mtd, 1)) {
        sysprintf("NAND Flash not found !\n");
        return -1;
    }

    //Set PSize bits of SMCSR register to select NAND card page size
    if (mtd->writesize == 2048)
        	NFI->NANDCTL = (NFI->NANDCTL & (~0x30000)) | 0x10000;
    else if (mtd->writesize == 4096)
        	NFI->NANDCTL = (NFI->NANDCTL & (~0x30000)) | 0x20000;
    else if (mtd->writesize == 8192)
        	NFI->NANDCTL = (NFI->NANDCTL & (~0x30000)) | 0x30000;

	if (nand->ecc.strength == 0) {
		nuvoton_nand->eBCHAlgo = 0; /* No ECC */
		nuvoton_layout_oob_table(&nuvoton_nand_oob, mtd->oobsize,
			g_i32ParityNum[mtd->writesize>>12][nuvoton_nand->eBCHAlgo]);

	} else if (nand->ecc.strength <= 8) {
		nuvoton_nand->eBCHAlgo = 1; /* T8 */
		nuvoton_layout_oob_table(&nuvoton_nand_oob, mtd->oobsize,
			g_i32ParityNum[mtd->writesize>>12][nuvoton_nand->eBCHAlgo]);

	} else if (nand->ecc.strength <= 12) {
		nuvoton_nand->eBCHAlgo = 2; /* T12 */
		nuvoton_layout_oob_table(&nuvoton_nand_oob, mtd->oobsize,
			g_i32ParityNum[mtd->writesize>>12][nuvoton_nand->eBCHAlgo]);

	} else if (nand->ecc.strength <= 24) {
		nuvoton_nand->eBCHAlgo = 3; /* T24 */
		nuvoton_layout_oob_table(&nuvoton_nand_oob, mtd->oobsize,
			g_i32ParityNum[mtd->writesize>>12][nuvoton_nand->eBCHAlgo]);

	} else {
		sysprintf("NAND Controller is not support this flash. (%d, %d)\n", mtd->writesize, mtd->oobsize);
	}

	nuvoton_nand->m_i32SMRASize  = mtd->oobsize;
	nand->ecc.steps = mtd->writesize / nand->ecc.size;
	nand->ecc.bytes = nuvoton_nand_oob.eccbytes / nand->ecc.steps;
	nand->ecc.total = nuvoton_nand_oob.eccbytes;

    nand->options = 0;
    nand->bbt_options = (NAND_BBT_USE_FLASH | NAND_BBT_NO_OOB);

    // Redundant area size
    NFI->NANDRACTL = nuvoton_nand->m_i32SMRASize;

    // Protect redundant 3 bytes
    // because we need to implement write_oob function to partial data to oob available area.
    // Please note we skip 4 bytes
    NFI->NANDCTL |= 0x100;

    // To read/write the ECC parity codes automatically from/to NAND Flash after data area field written.
    NFI->NANDCTL |= 0x10;
    // Set BCH algorithm
    NFI->NANDCTL = (NFI->NANDCTL & (~0x007C0000)) | g_i32BCHAlgoIdx[nuvoton_nand->eBCHAlgo];
    // Enable H/W ECC, ECC parity check enable bit during read page
    NFI->NANDCTL |= 0x00800080;

    return 0;
}

/// @endcond HIDDEN_SYMBOLS


/*@}*/ /* end of group NAND_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group NAND_Driver */

/*@}*/ /* end of group Standard_Driver */

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/




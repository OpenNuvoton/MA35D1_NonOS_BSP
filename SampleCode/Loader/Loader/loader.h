/*************************************************************************//**
 * @file     loader.h
 * @version  V1.00
 * @brief    baremetal loader header for MA35D1 MPU.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#ifndef _MA35D1_LOADER_H
#define _MA35D1_LOADER_H
/*--------------------------------------------------------------------------*/
/* the following definition should based on application                     */
/* modify by user                                                           */
/*--------------------------------------------------------------------------*/
/* application execute address */
#define APP_EXE_ADDR		(0x80400000UL)
/* storage offset */
#define APP_OFFSET_NAND		(0xC0000)	/* block 6, page size = 2048, page/block = 64 */
#define APP_OFFSET_SPINAND	(0xC0000)	/* block 6, page size = 2048, page/block = 64 */
#define APP_OFFSET_SPINOR	(0x40000)	/* 256K */
#define APP_OFFSET_EMMC		(0x40000)	/* 256K */

#define APP_SIZE			(0x10000)	/* application size */

#define SPINAND_PAGE_SIZE		2048
#define SPINAND_PAGE_PER_BLOCK	64

/*****************************************************************************/
/* SPI */
/* Write commands */
#define CMD_WRITE_STATUS        0x01
#define CMD_WRITE_DISABLE       0x04
#define CMD_WRITE_ENABLE        0x06
#define CMD_WRITE_EVCR          0x61

/* Read commands */
#define CMD_READ_ARRAY_SLOW         0x03
#define CMD_READ_QUAD_OUTPUT_FAST   0x6b
#define CMD_READ_QUAD_IO_FAST       0xeb
#define CMD_READ_ID                 0x9f
#define CMD_READ_STATUS             0x05
#define CMD_READ_STATUS1            0x35
#define CMD_READ_CONFIG             0x35
#define CMD_READ_EVCR               0x65

/*****************************************************************************/
/* SDH */
#define  SDH_POWER_ON            0x01
#define  SDH_POWER_180           0x0A
#define  SDH_POWER_300           0x0C
#define  SDH_POWER_330           0x0E

#define  SDH_RESET_ALL           0x01
#define  SDH_RESET_CMD           0x02
#define  SDH_RESET_DATA          0x04

#define  SDH_CMD_RESP_MASK       0x03
#define  SDH_CMD_CRC             0x08
#define  SDH_CMD_INDEX           0x10
#define  SDH_CMD_DATA            0x20
#define  SDH_CMD_ABORTCMD        0xC0

#define  SDH_CMD_RESP_NONE       0x00
#define  SDH_CMD_RESP_LONG       0x01
#define  SDH_CMD_RESP_SHORT      0x02
#define  SDH_CMD_RESP_SHORT_BUSY 0x03


/* MMC command */
#define MMC_CMD_GO_IDLE_STATE           0
#define MMC_CMD_SEND_OP_COND            1
#define MMC_CMD_ALL_SEND_CID            2
#define MMC_CMD_SET_RELATIVE_ADDR       3
#define MMC_CMD_SET_DSR                 4
#define MMC_CMD_SWITCH                  6
#define MMC_CMD_SELECT_CARD             7
#define MMC_CMD_SEND_EXT_CSD            8
#define MMC_CMD_SEND_CSD                9
#define MMC_CMD_SEND_CID                10
#define MMC_CMD_STOP_TRANSMISSION       12
#define MMC_CMD_SEND_STATUS             13
#define MMC_CMD_SET_BLOCKLEN            16
#define MMC_CMD_READ_SINGLE_BLOCK       17
#define MMC_CMD_READ_MULTIPLE_BLOCK     18
#define MMC_CMD_SEND_TUNING_BLOCK       19
#define MMC_CMD_SEND_TUNING_BLOCK_HS200 21
#define MMC_CMD_SET_BLOCK_COUNT         23
#define MMC_CMD_WRITE_SINGLE_BLOCK      24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK    25
#define MMC_CMD_ERASE_GROUP_START       35
#define MMC_CMD_ERASE_GROUP_END         36
#define MMC_CMD_ERASE                   38
#define MMC_CMD_APP_CMD                 55
#define MMC_CMD_SPI_READ_OCR            58
#define MMC_CMD_SPI_CRC_ON_OFF          59
#define MMC_CMD_RES_MAN                 62

#define SD_CMD_SEND_RELATIVE_ADDR       3
#define SD_CMD_SWITCH_FUNC              6
#define SD_CMD_SEND_IF_COND             8
#define SD_CMD_SWITCH_UHS18V            11

#define SD_CMD_APP_SET_BUS_WIDTH        6
#define SD_CMD_APP_SD_STATUS            13
#define SD_CMD_ERASE_WR_BLK_START       32
#define SD_CMD_ERASE_WR_BLK_END         33
#define SD_CMD_APP_SEND_OP_COND         41
#define SD_CMD_APP_SEND_SCR             51

/* MMC response */
#define MMC_RSP_PRESENT (1 << 0)
#define MMC_RSP_136     (1 << 1)        /* 136 bit response */
#define MMC_RSP_CRC     (1 << 2)        /* expect valid crc */
#define MMC_RSP_BUSY    (1 << 3)        /* card may send busy */
#define MMC_RSP_OPCODE  (1 << 4)        /* response contains opcode */

#define MMC_RSP_NONE    (0)
#define MMC_RSP_R1  (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1b (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_BUSY)
#define MMC_RSP_R2  (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC)
#define MMC_RSP_R3  (MMC_RSP_PRESENT)
#define MMC_RSP_R4  (MMC_RSP_PRESENT)
#define MMC_RSP_R5  (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R6  (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R7  (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)

#define DEVICE_UNKNOWN      0
#define SD_VERSION_2        1
#define SD_VERSION_1_0      2
#define MMC_VERSION         3

#define MMC_DATA_READ       1
#define MMC_DATA_WRITE      2

/*******************************************************************************************/
extern uint64_t volatile gStartTime;
extern NAND_INFO_T tNAND;

/*******************************************************************************************/
void DelayMicrosecond(unsigned int count);

void spiNandReset(void);
int  spiNandRead(unsigned int addr, unsigned int len, unsigned int *buf);
int  spiNandGetStatus(unsigned char offset);
void spiNandSetStatus(unsigned char offset, unsigned char value);
int  spiNandIsBlockValid(unsigned int block);
int  spiNorReset(void);
int  spiRead(unsigned int addr, unsigned int len, unsigned int *buf);

int  sdhInit(struct mmc *mmc);
int  sdhReadBlocks(struct mmc *mmc, void *dst, unsigned int start, unsigned int blkcnt);

#endif /* _MA35D1_LOADER_H */

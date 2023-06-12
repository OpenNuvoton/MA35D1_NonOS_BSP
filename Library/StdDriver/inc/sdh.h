/**************************************************************************//**
 * @file     sdh.h
 * @version  V1.00
 * @brief    M480 SDH driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2016-2020 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>

#ifndef __SDH_H__
#define __SDH_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define SDH	SDH1

#define SDH0_FREQ         50000000ul   /*!< output 50MHz to SD  \hideinitializer */

#define SDH1_MMC_ENABLE_8BIT /* by SDH1 eMMC only */
#define SDH1_ENABLE_1_8_V	/* by SDH1 SD only */
#define SDH1_FREQ         200000000ul   /*!< output 200MHz to SD  \hideinitializer */

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup SDH_Driver SDH Driver
  @{
*/

/** @addtogroup SDH_EXPORTED_CONSTANTS SDH Exported Constants
  @{
*/
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

#define SDH_INT_RESPONSE        BIT0
#define SDH_INT_DATA_END        BIT1
#define SDH_INT_DMA_END         BIT3
#define SDH_INT_SPACE_AVAIL     BIT4
#define SDH_INT_DATA_AVAIL      BIT5
#define SDH_INT_CARD_INSERT     BIT6
#define SDH_INT_CARD_REMOVE     BIT7
#define SDH_INT_CARD_INT        BIT8
#define SDH_INT_ERROR           BIT15
#define SDH_INT_TIMEOUT         BIT16
#define SDH_INT_CRC             BIT17
#define SDH_INT_END_BIT         BIT18
#define SDH_INT_INDEX           BIT19
#define SDH_INT_DATA_TIMEOUT    BIT20
#define SDH_INT_DATA_CRC        BIT21
#define SDH_INT_DATA_END_BIT    BIT22
#define SDH_INT_BUS_POWER       BIT23
#define SDH_INT_ACMD12ERR       BIT24
#define SDH_INT_ADMA_ERROR      BIT25

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

#define  SDH_CHCEK_FREQ         100000000ul

/* Maximum block size for MMC */
#define MMC_MAX_BLOCK_LEN   512
//---------------------------------------------------------------------------------------
#define SDH_ERR_ID       0xFFFF0100ul /*!< SDH error ID  \hideinitializer */

#define SDH_TIMEOUT      (SDH_ERR_ID|0x01ul) /*!< Timeout  \hideinitializer */
#define SDH_NO_MEMORY    (SDH_ERR_ID|0x02ul) /*!< OOM  \hideinitializer */

/*-- function return value */
#define    Successful  0ul   /*!< Success  \hideinitializer */
#define    Fail        1ul   /*!< Failed  \hideinitializer */

/*--- define type of SD card or MMC */
#define SDH_TYPE_UNKNOWN     0ul /*!< Unknown card type  \hideinitializer */
#define SDH_TYPE_SD_HIGH     1ul /*!< SDHC card  \hideinitializer */
#define SDH_TYPE_SD_LOW      2ul /*!< SD card  \hideinitializer */
#define SDH_TYPE_MMC         3ul /*!< MMC card  \hideinitializer */
#define SDH_TYPE_EMMC        4ul /*!< eMMC card  \hideinitializer */

#define INIT_FREQ         200000ul   /*!< output 50MHz to SDH \hideinitializer */

#define  SDH_CTRL_UHS_MASK		0x0007
#define  SDH_CTRL_UHS_SDR12		0x0000
#define  SDH_CTRL_UHS_SDR25		0x0001
#define  SDH_CTRL_UHS_SDR50		0x0002
#define  SDH_CTRL_UHS_SDR104	0x0003
#define  SDH_CTRL_UHS_DDR50		0x0004
#define  SDH_CTRL_1_8_V			0x0008

#define VENDOR0_BA	(SDH0_BASE+0x500)
#define VENDOR1_BA	(SDH1_BASE+0x500)
#define VENDOR0		((VENDOR_SPECIFIC_AREA_T *)  VENDOR0_BA)
#define VENDOR1		((VENDOR_SPECIFIC_AREA_T *)  VENDOR1_BA)

enum mmc_voltage {
	MMC_SIGNAL_VOLTAGE_000 = 0,
	MMC_SIGNAL_VOLTAGE_120 = 1,
	MMC_SIGNAL_VOLTAGE_180 = 2,
	MMC_SIGNAL_VOLTAGE_330 = 4,
};

enum bus_mode {
	MMC_LEGACY,
	SD_LEGACY,
	MMC_HS,
	SD_HS,
	MMC_HS_52,
	MMC_DDR_52,
	UHS_SDR12,
	UHS_SDR25,
	UHS_SDR50,
	UHS_DDR50,
	UHS_SDR104,
	MMC_HS_200,
	MMC_HS_400,
	MMC_HS_400_ES,
	MMC_MODES_END
};
#define EXT_CSD_TIMING_LEGACY	0	/* no high speed */
#define EXT_CSD_TIMING_HS	1	/* HS */
#define EXT_CSD_TIMING_HS200	2	/* HS200 */
#define EXT_CSD_HS_TIMING		185	/* R/W */


#define MMC_SWITCH_MODE_WRITE_BYTE	0x03 /* Set target byte to value */

#define UHS_SDR12_BUS_SPEED		0
#define HIGH_SPEED_BUS_SPEED	1
#define UHS_SDR25_BUS_SPEED		1
#define UHS_SDR50_BUS_SPEED		2
#define UHS_SDR104_BUS_SPEED	3
#define UHS_DDR50_BUS_SPEED		4

/*@}*/ /* end of group SDH_EXPORTED_CONSTANTS */

/** @addtogroup SDH_EXPORTED_TYPEDEF SDH Exported Type Defines
  @{
*/

struct mmc_cmd {
    unsigned short cmdidx;
    unsigned int   resp_type;
    unsigned int   cmdarg;
    unsigned int   response[4];
};

struct mmc_data {
    union {
        char *dest;
        const char *src; /* src buffers don't get written to */
    };
    unsigned int flags;
    unsigned int blocks;
    unsigned int blocksize;
};

struct mmc {
    unsigned int base;
    unsigned int rca;
    unsigned int version;
    int high_capacity;
    uint8_t bus_width;	/* 0: 4-bit, 1: 8-bit */
    unsigned int csd[4];
};

typedef struct SDH_info_t
{
    unsigned int    CardType;       /*!< SDHC, SD, or MMC */
    unsigned int    RCA;            /*!< Relative card address */
    unsigned int    totalSectorN;   /*!< Total sector number */
    unsigned int    diskSize;       /*!< Disk size in K bytes */
    int             sectorSize;     /*!< Sector size in bytes */
    int 			busWidth;		/*!< bus width */
    int 			signalVoltage;		/*!< signal voltage */
    unsigned char   *dmabuf;
} SDH_INFO_T;                       /*!< Structure holds SD card info */

/*@}*/ /* end of group SDH_EXPORTED_TYPEDEF */

/** @cond HIDDEN_SYMBOLS */
extern SDH_INFO_T SD0, SD1;
/** @endcond HIDDEN_SYMBOLS */

/** @addtogroup SDH_EXPORTED_FUNCTIONS SDH Exported Functions
  @{
*/

/**
 *  @brief    Check SD Card inserted or removed.
 *
 *  @param[in]    sdh    Select SDH0 or SDH1.
 *
 *  @return   1: Card inserted.
 *            0: Card removed.
 * \hideinitializer
 */
#define SDH_IS_CARD_PRESENT(sdh) (((sdh) == SDH0)? SD0.IsCardInsert : SD1.IsCardInsert)

/**
 *  @brief    Get SD Card capacity.
 *
 *  @param[in]    sdh    Select SDH0 or SDH1.
 *
 *  @return   SD Card capacity. (unit: KByte)
 * \hideinitializer
 */
#define SDH_GET_CARD_CAPACITY(sdh)  (((sdh) == SDH0)? SD0.diskSize : SD1.diskSize)

void SDH_Reset(SDH_T *sdh);
void SDH_Open(SDH_T *sdh);
uint32_t SDH_Probe(SDH_T *sdh);
int SDH_Read(SDH_T *sdh, uint8_t *pu8BufAddr, uint32_t u32StartSec, uint32_t u32SecCount);
uint32_t SDH_Write(SDH_T *sdh, uint8_t *pu8BufAddr, uint32_t u32StartSec, uint32_t u32SecCount);
uint32_t SDH_CardDetection(SDH_T *sdh);
void SDH_Open_Disk(SDH_T *sdh);
void SDH_Close_Disk(SDH_T *sdh);


/*@}*/ /* end of group SDH_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group SDH_Driver */

/*@}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif  /* __SDH_H__ */
/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/

/*************************************************************************//**
 * @file     sd.h
 * @version  V1.00
 * @brief    baremetal loader SDH header for MA35D1 MPU.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#ifndef _SD_H
#define _SD_H

/*---------------------- SD/SDIO Host Controller -------------------------*/
/**
	@addtogroup SDH SD/SDIO Host Controller(SDH)
	Memory Mapped Structure for SDH Controller
@{ */

/* only define offset */
#define SDH_DMA_ADDRESS         (0x00)
#define SDH_BLOCK_SIZE          (0x04)
#define SDH_BLOCK_COUNT         (0x06)
#define SDH_ARGUMENT            (0x08)
#define SDH_XFER_MODE           (0x0C)
#define SDH_COMMAND             (0x0E)
	#define SDH_MAKE_CMD(c, f) (((c & 0xff) << 8) | (f & 0xff))
	#define SDH_GET_CMD(c) ((c>>8) & 0x3f)
#define SDH_RESPONSE            (0x10)  /* 0x10 ~ 0x1C */
#define SDH_BUFFER              (0x20)
#define SDH_PRESENT_STATE       (0x24)
#define SDH_HOST_CONTROL        (0x28)
#define SDH_POWER_CONTROL       (0x29)
#define SDH_BGAP_CONTROL        (0x2A)
#define SDH_WUP_CONTROL         (0x2B)
#define SDH_CLOCK_CONTROL       (0x2C)
#define SDH_TOUT_CONTROL        (0x2E)
#define SDH_SW_RESET            (0x2F)
#define SDH_INT_STATUS          (0x30)
#define SDH_INT_ENABLE          (0x34)
#define SDH_SIGNAL_ENABLE       (0x38)
#define SDH_ACMD12_ERR          (0x3C)
#define SDH_HOST_CONTROL2       (0x3E)
#define SDH_CAPABILITIES        (0x40)
#define SDH_CAPABILITIES_1      (0x44)
#define SDH_MAX_CURRENT         (0x48)  /* 0x48 ~ 0x4C */
#define SDH_SET_ACMD12_ERROR    (0x50)
#define SDH_SET_INT_ERROR       (0x52)
#define SDH_ADMA_ERROR          (0x54)
#define SDH_ADMA_ADDRESS        (0x58)
#define SDH_ADMA_ADDRESS_HI     (0x5c)
#define SDH_SLOT_INT_STATUS     (0xFC)
#define SDH_HOST_VERSION        (0xFE)

/**@}*/ /* end of SDH register group */


#endif /* _SD_H */

/**************************************************************************//**
 * @file     nand.h
 *
 * @brief    NFI driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __NAND_H__
#define __NAND_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup NAND_Driver NAND Driver
  @{
*/

/** @addtogroup NAND_EXPORTED_CONSTANTS NAND Exported Constants
  @{
*/
/*---------------------------------------------------------------------------------------------------------*/
/*  NAND BCH Constant Definitions                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define NAND_BCH_T8		0x100000
#define NAND_BCH_T12	0x200000
#define NAND_BCH_T24	0x040000

#define NAND_TYPE_SLC		0x01
#define NAND_TYPE_MLC		0x00

#define NAND_PAGE_2KB		2048
#define NAND_PAGE_4KB		4096
#define NAND_PAGE_8KB		8192


/*@}*/ /* end of group NAND_EXPORTED_CONSTANTS */


/** @addtogroup NAND_EXPORTED_FUNCTIONS NAND Exported Functions
  @{
*/


/*@}*/ /* end of group NAND_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group NAND_Driver */

/*@}*/ /* end of group Standard_Driver */


typedef struct nand_info_t
{
	uint32_t uBlockPerFlash;
	uint32_t uPagePerBlock;
	uint32_t uPageSize;
	uint32_t uSpareSize;
	uint32_t NandECC;
	uint8_t  bIsMulticycle;
	uint8_t  bIsMLCNand;
	uint8_t  bIsCheckECC;
} NAND_INFO_T;

extern NAND_INFO_T tNAND;

int nfiOpen(void);
int nfiPageRead(int page, uint8_t *buff);
int nfiIsBlockValid(NAND_INFO_T *pNAND, uint32_t pba);

#ifdef __cplusplus
}
#endif

#endif /* __NAND_H__ */

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/

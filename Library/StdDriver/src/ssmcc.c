/**************************************************************************//**
 * @file     ssmcc.c
 * @brief    SSMCC driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NuMicro.h"


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup SSMCC_Driver SSMCC Driver
  @{
*/

/** @addtogroup SSMCC_EXPORTED_FUNCTIONS SSMCC Exported Functions
  @{
*/

/**
  * @brief  Set TZC Region0.
  * @param[in] u32Attr is security attribute for all region 0.
  * @return none
  * @details
  * SSMCC::TZC0:Ch0   CA35, AXI-AP
  * SSMCC::TZC0:Ch1   GFX, VC8000
  * SSMCC::TZC0:Ch2   DCUltra
  * SSMCC::TZC0:Ch3   GMAC0, GMAC1
  * SSMCC::TZC2:Ch0   CCAP0, CCAP1
  * SSMCC::TZC2:Ch1   CM4
  *                   PDMA0, PDMA1, PDMA2, PDMA3
  *                   SDH0, SDH1
  *                   HSUSBH0, HSUSBH1, USBH0, USBH1, USBH2
  *                   HSUSBD, NFI
  * SSMCC::TZC2:Ch2   CRYPTO
  * \hideinitializer
  */
#define TZC0_CH_NUM 4
#define TZC2_CH_NUM 3

void SSMCC_SetRegion0(uint32_t attrib)
{
        CLK->APBCLK2 |= (1 << 2);      /* enable ssmcc */
        /* set region 0 secure attribute */
        TZC0->REGION_ATTRIBUTES_0 = attrib & 0xC0000000;
        TZC0->GATE_KEEPER = 0x7;
        TZC2->REGION_ATTRIBUTES_0 = attrib & 0xC0000000;
        TZC2->GATE_KEEPER = 0x7;

        /* set region 0 non-secure attribute */
        TZC0->REGION_ID_ACCESS_0 = attrib & 0x00010001;
        TZC2->REGION_ID_ACCESS_0 = attrib & 0x00030003;
}

/**
  * @brief  Set TZC Regions.
  * @param[in] psParam is structure pointer of SSMCC parameter.
  * @return none
  * @details
  * \hideinitializer
  */
void SSMCC_SetRegion(SSMCC_PARAM_T param)
{
        TZC0->REGION[param.region_no].REGION_BASE_LOW = param.base_address;
        TZC0->REGION[param.region_no].REGION_TOP_LOW = param.base_address + param.size - 1;
        TZC0->REGION[param.region_no].REGION_ATTRIBUTES = 0x7 | (param.attribute & 0xC0000000);
        TZC0->REGION[param.region_no].REGION_ID_ACCESS = param.attribute & 0x00010001;         /* bit0: Non-secure, bit1: M4 */

        TZC2->REGION[param.region_no].REGION_BASE_LOW = param.base_address;
        TZC2->REGION[param.region_no].REGION_TOP_LOW = param.base_address + param.size - 1;
        TZC2->REGION[param.region_no].REGION_ATTRIBUTES = 0x7 | (param.attribute & 0xC0000000);
        TZC2->REGION[param.region_no].REGION_ID_ACCESS = param.attribute & 0x00030003;         /* bit0: Non-secure, bit1: M4 */
}

/*@}*/ /* end of group SSMCC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group SSMCC_Driver */

/*@}*/ /* end of group Standard_Driver */


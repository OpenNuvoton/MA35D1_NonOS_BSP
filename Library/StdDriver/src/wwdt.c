/**************************************************************************//**
 * @file     wwdt.c
 * @brief    WWDT driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NuMicro.h"


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup WWDT_Driver WWDT Driver
  @{
*/

/** @addtogroup WWDT_EXPORTED_FUNCTIONS WWDT Exported Functions
  @{
*/

/**
  * @brief      Initialize WWDT and start counting
  *
  * @param[in]  wwdt            The pointer of the specified WWDT module.
  * @param[in]  u32PreScale     Pre-scale setting of WWDT counter. Valid values are:
  *                             - \ref WWDT_PRESCALER_1
  *                             - \ref WWDT_PRESCALER_2
  *                             - \ref WWDT_PRESCALER_4
  *                             - \ref WWDT_PRESCALER_8
  *                             - \ref WWDT_PRESCALER_16
  *                             - \ref WWDT_PRESCALER_32
  *                             - \ref WWDT_PRESCALER_64
  *                             - \ref WWDT_PRESCALER_128
  *                             - \ref WWDT_PRESCALER_192
  *                             - \ref WWDT_PRESCALER_256
  *                             - \ref WWDT_PRESCALER_384
  *                             - \ref WWDT_PRESCALER_512
  *                             - \ref WWDT_PRESCALER_768
  *                             - \ref WWDT_PRESCALER_1024
  *                             - \ref WWDT_PRESCALER_1536
  *                             - \ref WWDT_PRESCALER_2048
  * @param[in]  u32CmpValue     Setting the window compared value. Valid values are between 0x0 to 0x3F.
  * @param[in]  u32EnableInt    Enable WWDT counter compare match interrupt function. Valid values are TRUE and FALSE.
  *
  * @details    This function makes WWDT module start counting with different counter period by pre-scale setting and compared window value.
  * @note       This WWDT_CTL register can be written only one time after chip is powered on or reset.
  */
void WWDT_Open(WWDT_T *wwdt,
		       uint32_t u32PreScale,
               uint32_t u32CmpValue,
               uint32_t u32EnableInt)
{
    wwdt->CTL = u32PreScale |
                (u32CmpValue << WWDT_CTL_CMPDAT_Pos) |
                ((u32EnableInt == TRUE) ? WWDT_CTL_INTEN_Msk : 0U) |
                WWDT_CTL_WWDTEN_Msk;
    return;
}

/*! @}*/ /* end of group WWDT_EXPORTED_FUNCTIONS */

/*! @}*/ /* end of group WWDT_Driver */

/*! @}*/ /* end of group Standard_Driver */


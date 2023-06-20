/**************************************************************************//**
 * @file     disp.h
 * @brief    Display driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright(C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __DISP_H__
#define __DISP_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup DISP_Driver Display Driver
  @{
*/

/** @addtogroup DISP_EXPORTED_CONSTANTS Display Exported Constants
  @{
*/


/*! @}*/ /* end of group DISP_EXPORTED_CONSTANTS */

/** @addtogroup DISP_EXPORTED_FUNCTIONS Display Exported Functions
  @{
*/

/**
  * @brief This macro enable display interrupt
  * \hideinitializer
  */
#define DISP_EnableInt() (DISP->DisplayIntrEnable |= DISP_DisplayIntrEnable_DISP0_Msk)

/**
  * @brief This macro disable display interrupt
  * \hideinitializer
  */
#define DISP_DisableInt() (DISP->DisplayIntrEnable &= ~DISP_DisplayIntrEnable_DISP0_Msk)


void DISP_EnableDCUClk(void);
uint64_t DISP_GeneratePixelClk(uint32_t u32PixClkInHz);
void DISP_SuspendPixelClk(void);
uint32_t DISP_GetIntFlag(void);
void DISP_SetInputPixelSwizzle(uint8_t u8PixelOrder);

/*! @}*/ /* end of group DISP_EXPORTED_FUNCTIONS */

/*! @}*/ /* end of group DISP_Driver */

/*! @}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __DISP_H__ */

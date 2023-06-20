/**************************************************************************//**
 * @file     disp.c
 * @brief    Display driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup DISP_Driver DISP Driver
  @{
*/

/** @addtogroup DISP_EXPORTED_FUNCTIONS DISP Exported Functions
  @{
*/

/**
  * @brief DISP core clock use EPLL with 250MHz.
  */
void DISP_EnableDCUClk(void)
{
    /* DCU core clock use EPLL with 250MHz */
    if((CLK->CLKSEL0 & CLK_CLKSEL0_DCUSEL_Msk) == CLK_CLKSEL0_SYSCK0SEL_EPLL_DIV2)
        CLK->PLL[EPLL].CTL1 &= ~CLK_PLLnCTL1_PD_Msk;
}

/**
  * @brief      Generate VPLL frequency as DISP pixel clock.
  * @param[in]  u32PixClkInHz is the DISP pixel clock input frequency
  * @return     DISP pixel clock output frequency.
  */
uint64_t DISP_GeneratePixelClk(uint32_t u32PixClkInHz)
{
    uint64_t u64PixClkOut;

    /* Set new VPLL clock frequency. */
    u32PixClkInHz <<= 1U;
    u64PixClkOut = CLK_SetPLLClockFreq(VPLL, PLL_OPMODE_INTEGER, __HXT, (uint64_t)u32PixClkInHz);

    /* Waiting clock ready */
    CLK_WaitClockReady(CLK_STATUS_STABLE_VPLL);

    return u64PixClkOut;
}

/**
  * @brief Suspend DISP pixel clock.
  */
void DISP_SuspendPixelClk(void)
{
    /* Stop VPLL forcely. */
    CLK_DisableAdvPLL(VPLL);
}

/**
  * @brief    Get Display Interrupt Flag
  * @return   0   Display interrupt did not occur
  * @return   1   Display interrupt occurred
  */
uint32_t DISP_GetIntFlag(void)
{
    return (DISP->DisplayIntr & DISP_DisplayIntr_DISP0_Msk);
}

/**
  * @brief      Set Display Input Pixel Format. Swizzle is used to switch the position of components
  *             in pixel data.
  * @param[in]  u8PixelOrder is the DISP pixel format orders
  */
void DISP_SetInputPixelSwizzle(uint8_t u8PixelOrder)
{
    DISP->FrameBufferConfig0 &= ~DISP_FrameBufferConfig0_SWIZZLE_Msk;
    DISP->FrameBufferConfig0 |= (u8PixelOrder << DISP_FrameBufferConfig0_SWIZZLE_Pos);
}

/*! @}*/ /* end of group DISP_EXPORTED_FUNCTIONS */

/*! @}*/ /* end of group DISP_Driver */

/*! @}*/ /* end of group Standard_Driver */

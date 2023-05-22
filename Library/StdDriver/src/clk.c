/**************************************************************************//**
 * @file     clk.c
 * @brief    CLK driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright(C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup CLK_Driver CLK Driver
  @{
*/

/** @addtogroup CLK_EXPORTED_FUNCTIONS CLK Exported Functions
  @{
*/

/**
  * @brief      Disable clock divider output function
  * @details    This function disable clock divider output function.
  */
void CLK_DisableCKO(void)
{
    /* Disable CKO clock source */
    CLK_DisableModuleClock(CLKO_MODULE);
}

/**
  * @brief      This function enable clock divider output module clock,
  *             enable clock divider output function and set frequency selection.
  * @param[in]  u32ClkSrc is frequency divider function clock source. Including :
  *             - \ref CLK_CLKSEL4_CKOSEL_HXT
  *             - \ref CLK_CLKSEL4_CKOSEL_LXT
  *             - \ref CLK_CLKSEL4_CKOSEL_LIRC
  *             - \ref CLK_CLKSEL4_CKOSEL_HIRC
  *             - \ref CLK_CLKSEL4_CKOSEL_CAPLL_DIV4
  *             - \ref CLK_CLKSEL4_CKOSEL_SYSPLL
  *             - \ref CLK_CLKSEL4_CKOSEL_APLL
  *             - \ref CLK_CLKSEL4_CKOSEL_EPLL_DIV2
  *             - \ref CLK_CLKSEL4_CKOSEL_VPLL
  * @param[in]  u32ClkDiv is divider output frequency selection. It could be 0~15.
  * @param[in]  u32ClkDivBy1En is clock divided by one enabled.
  * @details    Output selected clock to CKO. The output clock frequency is divided by u32ClkDiv. \n
  *             The formula is: \n
  *                 CKO frequency = (Clock source frequency) / 2^(u32ClkDiv + 1) \n
  *             This function is just used to set CKO clock.
  *             User must enable I/O for CKO clock output pin by themselves. \n
  */
void CLK_EnableCKO(uint32_t u32ClkSrc, uint32_t u32ClkDiv, uint32_t u32ClkDivBy1En)
{
    /* CKO = clock source / 2^(u32ClkDiv + 1) */
    CLK->CLKOCTL = CLK_CLKOCTL_CLKOEN_Msk | (u32ClkDiv) | (u32ClkDivBy1En << CLK_CLKOCTL_DIV1EN_Pos);

    /* Enable CKO clock source */
    CLK_EnableModuleClock(CLKO_MODULE);

    /* Select CKO clock source */
    CLK_SetModuleClock(CLKO_MODULE, u32ClkSrc, 0UL);
}

/**
  * @brief      Get external high speed crystal clock frequency
  * @return     External high frequency crystal frequency
  * @details    This function get external high frequency crystal frequency. The frequency unit is Hz.
  */
uint32_t CLK_GetHXTFreq(void)
{
    uint32_t u32Freq;

    if ((CLK->PWRCTL & CLK_PWRCTL_HXTEN_Msk) == CLK_PWRCTL_HXTEN_Msk)
    {
        u32Freq = __HXT;
    }
    else
    {
        u32Freq = 0UL;
    }

    return u32Freq;
}

/**
  * @brief      Get external low speed crystal clock frequency
  * @return     External low speed crystal clock frequency
  * @details    This function get external low frequency crystal frequency. The frequency unit is Hz.
  */
uint32_t CLK_GetLXTFreq(void)
{
    uint32_t u32Freq;

    if ((CLK->PWRCTL & CLK_PWRCTL_LXTEN_Msk) == CLK_PWRCTL_LXTEN_Msk)
    {
        u32Freq = __LXT;
    }
    else
    {
        u32Freq = 0UL;
    }

    return u32Freq;
}

/**
  * @brief      Get SYSCLK0 frequency
  * @return     SYSCLK0 frequency
  * @details    This function get SYSCLK0 frequency. The frequency unit is Hz.
  */
uint32_t CLK_GetSYSCLK0Freq(void)
{
    uint32_t u32Freq;

    if ((CLK->CLKSEL0 & CLK_CLKSEL0_SYSCK0SEL_Msk) == CLK_CLKSEL0_SYSCK0SEL_EPLL_DIV2)
    {
        u32Freq = CLK_GetPLLClockFreq(EPLL) / 2;
    }
    else
    {
        u32Freq = CLK_GetPLLClockFreq(SYSPLL);
    }

    return u32Freq;
}

/**
  * @brief      Get SYSCLK1 frequency
  * @return     SYSCLK1 frequency
  * @details    This function get SYSCLK1 frequency. The frequency unit is Hz.
  */
uint32_t CLK_GetSYSCLK1Freq(void)
{
    uint32_t u32Freq;

    if ((CLK->CLKSEL0 & CLK_CLKSEL0_SYSCK1SEL_Msk) == CLK_CLKSEL0_SYSCK1SEL_HXT)
    {
        u32Freq = __HXT;
    }
    else
    {
        u32Freq = CLK_GetPLLClockFreq(SYSPLL);
    }

    return u32Freq;
}

/**
  * @brief      Get HCLK0~2 and PCLK0~2 frequency
  * @return     HCLK0~2 and PCLK0~2 frequency
  * @details    This function get HCLK0~2 and PCLK0~2 frequency. The frequency unit is Hz.
  */
uint32_t CLK_GetPCLK0Freq(void)
{
    uint32_t u32Freq;

    u32Freq = CLK_GetSYSCLK1Freq();

    return u32Freq;
}

/**
  * @brief      Get HCLK3 and PCLK3~4 frequency
  * @return     HCLK3 and PCLK3~4 frequency
  * @details    This function get HCLK3 and PCLK3~4 frequency. The frequency unit is Hz.
  */
uint32_t CLK_GetPCLK3Freq(void)
{
    uint32_t u32Freq;

    u32Freq = CLK_GetSYSCLK1Freq() / 2;

    return u32Freq;
}

/**
  * @brief      This function set selected module clock source and module clock divider
  * @param[in]  u32ModuleIdx is module index.
  * @param[in]  u32ClkSrc is module clock source.
  * @param[in]  u32ClkDiv is module clock divider.
  * @details    Valid parameter combinations listed in following table:
  *
  * |Module index          |Clock source                            |Divider                    |
  * | :----------------    | :------------------------------------  | :----------------------   |
  * |\ref CA35_MODULE      |\ref CLK_CLKSEL0_CA35CKSEL_HXT          | x                         |
  * |\ref CA35_MODULE      |\ref CLK_CLKSEL0_CA35CKSEL_CAPLL        | x                         |
  * |\ref CA35_MODULE      |\ref CLK_CLKSEL0_CA35CKSEL_DDRPLL       | x                         |
  * |\ref SYSCK0_MODULE    |\ref CLK_CLKSEL0_SYSCK0SEL_SYSPLL       | x                         |
  * |\ref SYSCK0_MODULE    |\ref CLK_CLKSEL0_SYSCK0SEL_EPLL_DIV2    | x                         |
  * |\ref LVRDB_MODULE     |\ref CLK_CLKSEL0_LVRDBSEL_LIRC          | x                         |
  * |\ref LVRDB_MODULE     |\ref CLK_CLKSEL0_LVRDBSEL_HIRC          | x                         |
  * |\ref SYSCK1_MODULE    |\ref CLK_CLKSEL0_SYSCK1SEL_HXT          | x                         |
  * |\ref SYSCK1_MODULE    |\ref CLK_CLKSEL0_SYSCK1SEL_SYSPLL       | x                         |
  * |\ref RTPST_MODULE     |\ref CLK_CLKSEL0_RTPSTSEL_HXT           | x                         |
  * |\ref RTPST_MODULE     |\ref CLK_CLKSEL0_RTPSTSEL_LXT           | x                         |
  * |\ref RTPST_MODULE     |\ref CLK_CLKSEL0_RTPSTSEL_HXT_DIV2      | x                         |
  * |\ref RTPST_MODULE     |\ref CLK_CLKSEL0_RTPSTSEL_SYSCLK1_DIV2  | x                         |
  * |\ref RTPST_MODULE     |\ref CLK_CLKSEL0_RTPSTSEL_HIRC          | x                         |
  * |\ref CCAP0_MODULE     |\ref CLK_CLKSEL0_CCAP0SEL_HXT           |\ref CLK_CLKDIV1_CCAP0(x)  |
  * |\ref CCAP0_MODULE     |\ref CLK_CLKSEL0_CCAP0SEL_SYSPLL        |\ref CLK_CLKDIV1_CCAP0(x)  |
  * |\ref CCAP0_MODULE     |\ref CLK_CLKSEL0_CCAP0SEL_APLL          |\ref CLK_CLKDIV1_CCAP0(x)  |
  * |\ref CCAP0_MODULE     |\ref CLK_CLKSEL0_CCAP0SEL_VPLL          |\ref CLK_CLKDIV1_CCAP0(x)  |
  * |\ref CCAP1_MODULE     |\ref CLK_CLKSEL0_CCAP1SEL_HXT           |\ref CLK_CLKDIV1_CCAP1(x)  |
  * |\ref CCAP1_MODULE     |\ref CLK_CLKSEL0_CCAP1SEL_SYSPLL        |\ref CLK_CLKDIV1_CCAP1(x)  |
  * |\ref CCAP1_MODULE     |\ref CLK_CLKSEL0_CCAP1SEL_APLL          |\ref CLK_CLKDIV1_CCAP1(x)  |
  * |\ref CCAP1_MODULE     |\ref CLK_CLKSEL0_CCAP1SEL_VPLL          |\ref CLK_CLKDIV1_CCAP1(x)  |
  * |\ref SD0_MODULE       |\ref CLK_CLKSEL0_SD0SEL_SYSPLL          | x                         |
  * |\ref SD0_MODULE       |\ref CLK_CLKSEL0_SD0SEL_APLL            | x                         |
  * |\ref SD1_MODULE       |\ref CLK_CLKSEL0_SD1SEL_SYSPLL          | x                         |
  * |\ref SD1_MODULE       |\ref CLK_CLKSEL0_SD1SEL_APLL            | x                         |
  * |\ref DCU_MODULE       |\ref CLK_CLKSEL0_DCUSEL_EPLL_DIV2       | x                         |
  * |\ref DCU_MODULE       |\ref CLK_CLKSEL0_DCUSEL_SYSPLL          | x                         |
  * |\ref DCUP_MODULE      | x                                      |\ref CLK_CLKDIV0_DCUP(x)   |
  * |\ref DCUP_MODULE      | x                                      |\ref CLK_CLKDIV0_DCUP(x)   |
  * |\ref GFX_MODULE       |\ref CLK_CLKSEL0_GFXSEL_SYSPLL          | x                         |
  * |\ref GFX_MODULE       |\ref CLK_CLKSEL0_GFXSEL_EPLL            | x                         |
  * |\ref TMR0_MODULE      |\ref CLK_CLKSEL1_TMR0SEL_HXT            | x                         |
  * |\ref TMR0_MODULE      |\ref CLK_CLKSEL1_TMR0SEL_LXT            | x                         |
  * |\ref TMR0_MODULE      |\ref CLK_CLKSEL1_TMR0SEL_LIRC           | x                         |
  * |\ref TMR0_MODULE      |\ref CLK_CLKSEL1_TMR0SEL_HIRC           | x                         |
  * |\ref TMR0_MODULE      |\ref CLK_CLKSEL1_TMR0SEL_PCLK0          | x                         |
  * |\ref TMR0_MODULE      |\ref CLK_CLKSEL1_TMR0SEL_EXT            | x                         |
  * |\ref TMR1_MODULE      |\ref CLK_CLKSEL1_TMR1SEL_HXT            | x                         |
  * |\ref TMR1_MODULE      |\ref CLK_CLKSEL1_TMR1SEL_LXT            | x                         |
  * |\ref TMR1_MODULE      |\ref CLK_CLKSEL1_TMR1SEL_LIRC           | x                         |
  * |\ref TMR1_MODULE      |\ref CLK_CLKSEL1_TMR1SEL_HIRC           | x                         |
  * |\ref TMR1_MODULE      |\ref CLK_CLKSEL1_TMR1SEL_PCLK0          | x                         |
  * |\ref TMR1_MODULE      |\ref CLK_CLKSEL1_TMR1SEL_EXT            | x                         |
  * |\ref TMR2_MODULE      |\ref CLK_CLKSEL1_TMR2SEL_HXT            | x                         |
  * |\ref TMR2_MODULE      |\ref CLK_CLKSEL1_TMR2SEL_LXT            | x                         |
  * |\ref TMR2_MODULE      |\ref CLK_CLKSEL1_TMR2SEL_LIRC           | x                         |
  * |\ref TMR2_MODULE      |\ref CLK_CLKSEL1_TMR2SEL_HIRC           | x                         |
  * |\ref TMR2_MODULE      |\ref CLK_CLKSEL1_TMR2SEL_PCLK1          | x                         |
  * |\ref TMR2_MODULE      |\ref CLK_CLKSEL1_TMR2SEL_EXT            | x                         |
  * |\ref TMR3_MODULE      |\ref CLK_CLKSEL1_TMR3SEL_HXT            | x                         |
  * |\ref TMR3_MODULE      |\ref CLK_CLKSEL1_TMR3SEL_LXT            | x                         |
  * |\ref TMR3_MODULE      |\ref CLK_CLKSEL1_TMR3SEL_LIRC           | x                         |
  * |\ref TMR3_MODULE      |\ref CLK_CLKSEL1_TMR3SEL_HIRC           | x                         |
  * |\ref TMR3_MODULE      |\ref CLK_CLKSEL1_TMR3SEL_PCLK1          | x                         |
  * |\ref TMR3_MODULE      |\ref CLK_CLKSEL1_TMR3SEL_EXT            | x                         |
  * |\ref TMR4_MODULE      |\ref CLK_CLKSEL1_TMR4SEL_HXT            | x                         |
  * |\ref TMR4_MODULE      |\ref CLK_CLKSEL1_TMR4SEL_LXT            | x                         |
  * |\ref TMR4_MODULE      |\ref CLK_CLKSEL1_TMR4SEL_LIRC           | x                         |
  * |\ref TMR4_MODULE      |\ref CLK_CLKSEL1_TMR4SEL_HIRC           | x                         |
  * |\ref TMR4_MODULE      |\ref CLK_CLKSEL1_TMR4SEL_EXT            | x                         |
  * |\ref TMR4_MODULE      |\ref CLK_CLKSEL1_TMR4SEL_PCLK2          | x                         |
  * |\ref TMR5_MODULE      |\ref CLK_CLKSEL1_TMR5SEL_HXT            | x                         |
  * |\ref TMR5_MODULE      |\ref CLK_CLKSEL1_TMR5SEL_LXT            | x                         |
  * |\ref TMR5_MODULE      |\ref CLK_CLKSEL1_TMR5SEL_LIRC           | x                         |
  * |\ref TMR5_MODULE      |\ref CLK_CLKSEL1_TMR5SEL_HIRC           | x                         |
  * |\ref TMR5_MODULE      |\ref CLK_CLKSEL1_TMR5SEL_EXT            | x                         |
  * |\ref TMR5_MODULE      |\ref CLK_CLKSEL1_TMR5SEL_PCLK2          | x                         |
  * |\ref TMR6_MODULE      |\ref CLK_CLKSEL1_TMR6SEL_HXT            | x                         |
  * |\ref TMR6_MODULE      |\ref CLK_CLKSEL1_TMR6SEL_LXT            | x                         |
  * |\ref TMR6_MODULE      |\ref CLK_CLKSEL1_TMR6SEL_LIRC           | x                         |
  * |\ref TMR6_MODULE      |\ref CLK_CLKSEL1_TMR6SEL_HIRC           | x                         |
  * |\ref TMR6_MODULE      |\ref CLK_CLKSEL1_TMR6SEL_PCLK0          | x                         |
  * |\ref TMR6_MODULE      |\ref CLK_CLKSEL1_TMR6SEL_EXT            | x                         |
  * |\ref TMR7_MODULE      |\ref CLK_CLKSEL1_TMR7SEL_HXT            | x                         |
  * |\ref TMR7_MODULE      |\ref CLK_CLKSEL1_TMR7SEL_LXT            | x                         |
  * |\ref TMR7_MODULE      |\ref CLK_CLKSEL1_TMR7SEL_LIRC           | x                         |
  * |\ref TMR7_MODULE      |\ref CLK_CLKSEL1_TMR7SEL_HIRC           | x                         |
  * |\ref TMR7_MODULE      |\ref CLK_CLKSEL1_TMR7SEL_PCLK0          | x                         |
  * |\ref TMR7_MODULE      |\ref CLK_CLKSEL1_TMR7SEL_EXT            | x                         |
  * |\ref TMR8_MODULE      |\ref CLK_CLKSEL2_TMR8SEL_HXT            | x                         |
  * |\ref TMR8_MODULE      |\ref CLK_CLKSEL2_TMR8SEL_LXT            | x                         |
  * |\ref TMR8_MODULE      |\ref CLK_CLKSEL2_TMR8SEL_LIRC           | x                         |
  * |\ref TMR8_MODULE      |\ref CLK_CLKSEL2_TMR8SEL_HIRC           | x                         |
  * |\ref TMR8_MODULE      |\ref CLK_CLKSEL2_TMR8SEL_PCLK1          | x                         |
  * |\ref TMR8_MODULE      |\ref CLK_CLKSEL2_TMR8SEL_EXT            | x                         |
  * |\ref TMR9_MODULE      |\ref CLK_CLKSEL2_TMR9SEL_HXT            | x                         |
  * |\ref TMR9_MODULE      |\ref CLK_CLKSEL2_TMR9SEL_LXT            | x                         |
  * |\ref TMR9_MODULE      |\ref CLK_CLKSEL2_TMR9SEL_LIRC           | x                         |
  * |\ref TMR9_MODULE      |\ref CLK_CLKSEL2_TMR9SEL_HIRC           | x                         |
  * |\ref TMR9_MODULE      |\ref CLK_CLKSEL2_TMR9SEL_PCLK1          | x                         |
  * |\ref TMR9_MODULE      |\ref CLK_CLKSEL2_TMR9SEL_EXT            | x                         |
  * |\ref TMR10_MODULE     |\ref CLK_CLKSEL2_TMR10SEL_HXT           | x                         |
  * |\ref TMR10_MODULE     |\ref CLK_CLKSEL2_TMR10SEL_LXT           | x                         |
  * |\ref TMR10_MODULE     |\ref CLK_CLKSEL2_TMR10SEL_LIRC          | x                         |
  * |\ref TMR10_MODULE     |\ref CLK_CLKSEL2_TMR10SEL_HIRC          | x                         |
  * |\ref TMR10_MODULE     |\ref CLK_CLKSEL2_TMR10SEL_EXT           | x                         |
  * |\ref TMR10_MODULE     |\ref CLK_CLKSEL2_TMR10SEL_PCLK2         | x                         |
  * |\ref TMR11_MODULE     |\ref CLK_CLKSEL2_TMR11SEL_HXT           | x                         |
  * |\ref TMR11_MODULE     |\ref CLK_CLKSEL2_TMR11SEL_LXT           | x                         |
  * |\ref TMR11_MODULE     |\ref CLK_CLKSEL2_TMR11SEL_LIRC          | x                         |
  * |\ref TMR11_MODULE     |\ref CLK_CLKSEL2_TMR11SEL_HIRC          | x                         |
  * |\ref TMR11_MODULE     |\ref CLK_CLKSEL2_TMR11SEL_EXT           | x                         |
  * |\ref TMR11_MODULE     |\ref CLK_CLKSEL2_TMR11SEL_PCLK2         | x                         |
  * |\ref UART0_MODULE     |\ref CLK_CLKSEL2_UART0SEL_HXT           |\ref CLK_CLKDIV1_UART0(x)  |
  * |\ref UART0_MODULE     |\ref CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2  |\ref CLK_CLKDIV1_UART0(x)  |
  * |\ref UART1_MODULE     |\ref CLK_CLKSEL2_UART1SEL_HXT           |\ref CLK_CLKDIV1_UART1(x)  |
  * |\ref UART1_MODULE     |\ref CLK_CLKSEL2_UART1SEL_SYSCLK1_DIV2  |\ref CLK_CLKDIV1_UART1(x)  |
  * |\ref UART2_MODULE     |\ref CLK_CLKSEL2_UART2SEL_HXT           |\ref CLK_CLKDIV1_UART2(x)  |
  * |\ref UART2_MODULE     |\ref CLK_CLKSEL2_UART2SEL_SYSCLK1_DIV2  |\ref CLK_CLKDIV1_UART2(x)  |
  * |\ref UART3_MODULE     |\ref CLK_CLKSEL2_UART3SEL_HXT           |\ref CLK_CLKDIV1_UART3(x)  |
  * |\ref UART3_MODULE     |\ref CLK_CLKSEL2_UART3SEL_SYSCLK1_DIV2  |\ref CLK_CLKDIV1_UART3(x)  |
  * |\ref UART4_MODULE     |\ref CLK_CLKSEL2_UART4SEL_HXT           |\ref CLK_CLKDIV2_UART4(x)  |
  * |\ref UART4_MODULE     |\ref CLK_CLKSEL2_UART4SEL_SYSCLK1_DIV2  |\ref CLK_CLKDIV2_UART4(x)  |
  * |\ref UART5_MODULE     |\ref CLK_CLKSEL2_UART5SEL_HXT           |\ref CLK_CLKDIV2_UART5(x)  |
  * |\ref UART5_MODULE     |\ref CLK_CLKSEL2_UART5SEL_SYSCLK1_DIV2  |\ref CLK_CLKDIV2_UART5(x)  |
  * |\ref UART6_MODULE     |\ref CLK_CLKSEL2_UART6SEL_HXT           |\ref CLK_CLKDIV2_UART6(x)  |
  * |\ref UART6_MODULE     |\ref CLK_CLKSEL2_UART6SEL_SYSCLK1_DIV2  |\ref CLK_CLKDIV2_UART6(x)  |
  * |\ref UART7_MODULE     |\ref CLK_CLKSEL2_UART7SEL_HXT           |\ref CLK_CLKDIV2_UART7(x)  |
  * |\ref UART7_MODULE     |\ref CLK_CLKSEL2_UART7SEL_SYSCLK1_DIV2  |\ref CLK_CLKDIV2_UART7(x)  |
  * |\ref UART8_MODULE     |\ref CLK_CLKSEL3_UART8SEL_HXT           |\ref CLK_CLKDIV2_UART8(x)  |
  * |\ref UART8_MODULE     |\ref CLK_CLKSEL3_UART8SEL_SYSCLK1_DIV2  |\ref CLK_CLKDIV2_UART8(x)  |
  * |\ref UART9_MODULE     |\ref CLK_CLKSEL3_UART9SEL_HXT           |\ref CLK_CLKDIV2_UART9(x)  |
  * |\ref UART9_MODULE     |\ref CLK_CLKSEL3_UART9SEL_SYSCLK1_DIV2  |\ref CLK_CLKDIV2_UART9(x)  |
  * |\ref UART10_MODULE    |\ref CLK_CLKSEL3_UART10SEL_HXT          |\ref CLK_CLKDIV2_UART10(x) |
  * |\ref UART10_MODULE    |\ref CLK_CLKSEL3_UART10SEL_SYSCLK1_DIV2 |\ref CLK_CLKDIV2_UART10(x) |
  * |\ref UART11_MODULE    |\ref CLK_CLKSEL3_UART11SEL_HXT          |\ref CLK_CLKDIV2_UART11(x) |
  * |\ref UART11_MODULE    |\ref CLK_CLKSEL3_UART11SEL_SYSCLK1_DIV2 |\ref CLK_CLKDIV2_UART11(x) |
  * |\ref UART12_MODULE    |\ref CLK_CLKSEL3_UART12SEL_HXT          |\ref CLK_CLKDIV3_UART12(x) |
  * |\ref UART12_MODULE    |\ref CLK_CLKSEL3_UART12SEL_SYSCLK1_DIV2 |\ref CLK_CLKDIV3_UART12(x) |
  * |\ref UART13_MODULE    |\ref CLK_CLKSEL3_UART13SEL_HXT          |\ref CLK_CLKDIV3_UART13(x) |
  * |\ref UART13_MODULE    |\ref CLK_CLKSEL3_UART13SEL_SYSCLK1_DIV2 |\ref CLK_CLKDIV3_UART13(x) |
  * |\ref UART14_MODULE    |\ref CLK_CLKSEL3_UART14SEL_HXT          |\ref CLK_CLKDIV3_UART14(x) |
  * |\ref UART14_MODULE    |\ref CLK_CLKSEL3_UART14SEL_SYSCLK1_DIV2 |\ref CLK_CLKDIV3_UART14(x) |
  * |\ref UART15_MODULE    |\ref CLK_CLKSEL3_UART15SEL_HXT          |\ref CLK_CLKDIV3_UART15(x) |
  * |\ref UART15_MODULE    |\ref CLK_CLKSEL3_UART15SEL_SYSCLK1_DIV2 |\ref CLK_CLKDIV3_UART15(x) |
  * |\ref UART16_MODULE    |\ref CLK_CLKSEL3_UART16SEL_HXT          |\ref CLK_CLKDIV3_UART16(x) |
  * |\ref UART16_MODULE    |\ref CLK_CLKSEL3_UART16SEL_SYSCLK1_DIV2 |\ref CLK_CLKDIV3_UART16(x) |
  * |\ref WDT0_MODULE      |\ref CLK_CLKSEL3_WDT0SEL_LXT            | x                         |
  * |\ref WDT0_MODULE      |\ref CLK_CLKSEL3_WDT0SEL_LIRC           | x                         |
  * |\ref WDT0_MODULE      |\ref CLK_CLKSEL3_WDT0SEL_PCLK3_DIV4096  | x                         |
  * |\ref WWDT0_MODULE     |\ref CLK_CLKSEL3_WWDT0SEL_LIRC          | x                         |
  * |\ref WWDT0_MODULE     |\ref CLK_CLKSEL3_WWDT0SEL_PCLK3_DIV4096 | x                         |
  * |\ref WDT1_MODULE      |\ref CLK_CLKSEL3_WDT1SEL_LXT            | x                         |
  * |\ref WDT1_MODULE      |\ref CLK_CLKSEL3_WDT1SEL_LIRC           | x                         |
  * |\ref WDT1_MODULE      |\ref CLK_CLKSEL3_WDT1SEL_PCLK3_DIV4096  | x                         |
  * |\ref WWDT1_MODULE     |\ref CLK_CLKSEL3_WWDT1SEL_LIRC          | x                         |
  * |\ref WWDT1_MODULE     |\ref CLK_CLKSEL3_WWDT1SEL_PCLK3_DIV4096 | x                         |
  * |\ref WDT2_MODULE      |\ref CLK_CLKSEL3_WDT2SEL_LXT            | x                         |
  * |\ref WDT2_MODULE      |\ref CLK_CLKSEL3_WDT2SEL_LIRC           | x                         |
  * |\ref WDT2_MODULE      |\ref CLK_CLKSEL3_WDT2SEL_PCLK4_DIV4096  | x                         |
  * |\ref WWDT2_MODULE     |\ref CLK_CLKSEL3_WWDT2SEL_LIRC          | x                         |
  * |\ref WWDT2_MODULE     |\ref CLK_CLKSEL3_WWDT2SEL_PCLK4_DIV4096 | x                         |
  * |\ref SPI0_MODULE      |\ref CLK_CLKSEL4_SPI0SEL_PCLK1          | x                         |
  * |\ref SPI0_MODULE      |\ref CLK_CLKSEL4_SPI0SEL_APLL           | x                         |
  * |\ref SPI1_MODULE      |\ref CLK_CLKSEL4_SPI1SEL_PCLK2          | x                         |
  * |\ref SPI1_MODULE      |\ref CLK_CLKSEL4_SPI1SEL_APLL           | x                         |
  * |\ref SPI2_MODULE      |\ref CLK_CLKSEL4_SPI2SEL_PCLK1          | x                         |
  * |\ref SPI2_MODULE      |\ref CLK_CLKSEL4_SPI2SEL_APLL           | x                         |
  * |\ref SPI3_MODULE      |\ref CLK_CLKSEL4_SPI3SEL_PCLK2          | x                         |
  * |\ref SPI3_MODULE      |\ref CLK_CLKSEL4_SPI3SEL_APLL           | x                         |
  * |\ref QSPI0_MODULE     |\ref CLK_CLKSEL4_QSPI0SEL_PCLK0         | x                         |
  * |\ref QSPI0_MODULE     |\ref CLK_CLKSEL4_QSPI0SEL_APLL          | x                         |
  * |\ref QSPI1_MODULE     |\ref CLK_CLKSEL4_QSPI1SEL_PCLK0         | x                         |
  * |\ref QSPI1_MODULE     |\ref CLK_CLKSEL4_QSPI1SEL_APLL          | x                         |
  * |\ref I2S0_MODULE      |\ref CLK_CLKSEL4_I2S0SEL_APLL           | x                         |
  * |\ref I2S0_MODULE      |\ref CLK_CLKSEL4_I2S0SEL_SYSCLK1_DIV2   | x                         |
  * |\ref I2S1_MODULE      |\ref CLK_CLKSEL4_I2S1SEL_APLL           | x                         |
  * |\ref I2S1_MODULE      |\ref CLK_CLKSEL4_I2S1SEL_SYSCLK1_DIV2   | x                         |
  * |\ref CANFD0_MODULE    |\ref CLK_CLKSEL4_CANFD0SEL_APLL         | x                         |
  * |\ref CANFD0_MODULE    |\ref CLK_CLKSEL4_CANFD0SEL_VPLL         | x                         |
  * |\ref CANFD1_MODULE    |\ref CLK_CLKSEL4_CANFD1SEL_APLL         | x                         |
  * |\ref CANFD1_MODULE    |\ref CLK_CLKSEL4_CANFD1SEL_VPLL         | x                         |
  * |\ref CANFD2_MODULE    |\ref CLK_CLKSEL4_CANFD2SEL_APLL         | x                         |
  * |\ref CANFD2_MODULE    |\ref CLK_CLKSEL4_CANFD2SEL_VPLL         | x                         |
  * |\ref CANFD3_MODULE    |\ref CLK_CLKSEL4_CANFD3SEL_APLL         | x                         |
  * |\ref CANFD3_MODULE    |\ref CLK_CLKSEL4_CANFD3SEL_VPLL         | x                         |
  * |\ref CLKO_MODULE      |\ref CLK_CLKSEL4_CKOSEL_HXT             | x                         |
  * |\ref CLKO_MODULE      |\ref CLK_CLKSEL4_CKOSEL_LXT             | x                         |
  * |\ref CLKO_MODULE      |\ref CLK_CLKSEL4_CKOSEL_LIRC            | x                         |
  * |\ref CLKO_MODULE      |\ref CLK_CLKSEL4_CKOSEL_HIRC            | x                         |
  * |\ref CLKO_MODULE      |\ref CLK_CLKSEL4_CKOSEL_CAPLL_DIV4      | x                         |
  * |\ref CLKO_MODULE      |\ref CLK_CLKSEL4_CKOSEL_SYSPLL          | x                         |
  * |\ref CLKO_MODULE      |\ref CLK_CLKSEL4_CKOSEL_DDRPLL          | x                         |
  * |\ref CLKO_MODULE      |\ref CLK_CLKSEL4_CKOSEL_EPLL_DIV2       | x                         |
  * |\ref CLKO_MODULE      |\ref CLK_CLKSEL4_CKOSEL_APLL            | x                         |
  * |\ref CLKO_MODULE      |\ref CLK_CLKSEL4_CKOSEL_VPLL            | x                         |
  * |\ref SC0_MODULE       |\ref CLK_CLKSEL4_SC0SEL_HXT             |\ref CLK_CLKDIV1_SC0(x)    |
  * |\ref SC0_MODULE       |\ref CLK_CLKSEL4_SC0SEL_PCLK4           |\ref CLK_CLKDIV1_SC0(x)    |
  * |\ref SC1_MODULE       |\ref CLK_CLKSEL4_SC1SEL_HXT             |\ref CLK_CLKDIV1_SC1(x)    |
  * |\ref SC1_MODULE       |\ref CLK_CLKSEL4_SC1SEL_PCLK4           |\ref CLK_CLKDIV1_SC1(x)    |
  * |\ref KPI_MODULE       |\ref CLK_CLKSEL4_KPISEL_HXT             |\ref CLK_CLKDIV4_KPI(x)    |
  * |\ref KPI_MODULE       |\ref CLK_CLKSEL4_KPISEL_LXT             |\ref CLK_CLKDIV4_KPI(x)    |
  * |\ref ADC_MODULE       | x                                      |\ref CLK_CLKDIV4_ADC(x)    |
  * |\ref EADC_MODULE      | x                                      |\ref CLK_CLKDIV4_EADC(x)   |
  *
  */
void CLK_SetModuleClock(uint32_t u32ModuleIdx, uint32_t u32ClkSrc, uint32_t u32ClkDiv)
{
    uint32_t u32sel = 0U, u32div = 0U;

    if (u32ModuleIdx == KPI_MODULE)
    {
        CLK->CLKDIV4 = (CLK->CLKDIV4 & ~(CLK_CLKDIV4_KPIDIV_Msk)) | u32ClkDiv;
        CLK->CLKSEL4 = (CLK->CLKSEL4 & ~(CLK_CLKSEL4_KPISEL_Msk)) | u32ClkSrc;
        CLK->APBCLK0 = (CLK->APBCLK0 & ~(CLK_APBCLK0_KPICKEN_Msk)) | CLK_APBCLK0_KPICKEN_Msk;
    }
    else if (u32ModuleIdx == ADC_MODULE)
    {
        CLK->CLKDIV4 = (CLK->CLKDIV4 & ~(CLK_CLKDIV4_ADCDIV_Msk)) | u32ClkDiv;
        CLK->APBCLK2 = (CLK->APBCLK2 & ~(CLK_APBCLK2_ADCCKEN_Msk)) | CLK_APBCLK2_ADCCKEN_Msk;
    }
    else
    {
        if (MODULE_CLKDIV_Msk(u32ModuleIdx) != MODULE_NoMsk)
        {
            /* Get clock divider control register address */
            u32div = ptr_to_u32(&CLK->CLKDIV0) + ((MODULE_CLKDIV(u32ModuleIdx)) * 4U);
            /* Apply new divider */
            M32(u32div) = (M32(u32div) & (~(MODULE_CLKDIV_Msk(u32ModuleIdx) << MODULE_CLKDIV_Pos(u32ModuleIdx)))) | u32ClkDiv;
        }

        if (MODULE_CLKSEL_Msk(u32ModuleIdx) != MODULE_NoMsk)
        {
            /* Get clock select control register address */
            u32sel = ptr_to_u32(&CLK->CLKSEL0) + ((MODULE_CLKSEL(u32ModuleIdx)) * 4U);
            /* Set new clock selection setting */
            M32(u32sel) = (M32(u32sel) & (~(MODULE_CLKSEL_Msk(u32ModuleIdx) << MODULE_CLKSEL_Pos(u32ModuleIdx)))) | u32ClkSrc;
        }
    }
}

/**
  * @brief      Set SysTick clock source
  * @param[in]  u32ClkSrc is module clock source. Including:
  *             - \ref CLK_CLKSEL0_RTPSTSEL_HXT
  *             - \ref CLK_CLKSEL0_RTPSTSEL_LXT
  *             - \ref CLK_CLKSEL0_RTPSTSEL_HXT_DIV2
  *             - \ref CLK_CLKSEL0_RTPSTSEL_SYSCLK1_DIV2
  *             - \ref CLK_CLKSEL0_RTPSTSEL_HIRC
  * @details    This function set SysTick clock source. \n
  *             The register write-protection function should be disabled before using this function.
  */
void CLK_SetSysTickClockSrc(uint32_t u32ClkSrc)
{
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_RTPSTSEL_Msk) | u32ClkSrc;
}

/**
  * @brief      Enable clock source
  * @param[in]  u32ClkMask is clock source mask. Including :
  *             - \ref CLK_PWRCTL_ENABLE_HXT
  *             - \ref CLK_PWRCTL_ENABLE_LXT
  *             - \ref CLK_PWRCTL_ENABLE_HIRC
  *             - \ref CLK_PWRCTL_ENABLE_LIRC
  * @details    This function enable clock source. \n
  *             The register write-protection function should be disabled before using this function.
  */
void CLK_EnableXtalRC(uint32_t u32ClkMask)
{
    CLK->PWRCTL |= u32ClkMask;
}

/**
  * @brief      Disable clock source
  * @param[in]  u32ClkMask is clock source mask. Including :
  *             - \ref CLK_PWRCTL_ENABLE_HXT
  *             - \ref CLK_PWRCTL_ENABLE_LXT
  *             - \ref CLK_PWRCTL_ENABLE_HIRC
  *             - \ref CLK_PWRCTL_ENABLE_LIRC
  * @details    This function disable clock source. \n
  *             The register write-protection function should be disabled before using this function.
  */
void CLK_DisableXtalRC(uint32_t u32ClkMask)
{
    CLK->PWRCTL &= ~u32ClkMask;
}

/**
  * @brief      Enable module clock
  * @param[in]  u32ModuleIdx is module index. Including :
  *             - \ref PDMA0_MODULE
  *             - \ref PDMA1_MODULE
  *             - \ref PDMA2_MODULE
  *             - \ref PDMA3_MODULE
  *             - \ref WH0_MODULE
  *             - \ref WH1_MODULE
  *             - \ref HWS_MODULE
  *             - \ref EBI_MODULE
  *             - \ref SRAM0_MODULE
  *             - \ref SRAM1_MODULE
  *             - \ref ROM_MODULE
  *             - \ref TRA_MODULE
  *             - \ref DBG_MODULE
  *             - \ref CLKO_MODULE
  *             - \ref GTMR_MODULE
  *             - \ref GPA_MODULE
  *             - \ref GPB_MODULE
  *             - \ref GPC_MODULE
  *             - \ref GPD_MODULE
  *             - \ref GPE_MODULE
  *             - \ref GPF_MODULE
  *             - \ref GPG_MODULE
  *             - \ref GPH_MODULE
  *             - \ref GPI_MODULE
  *             - \ref GPJ_MODULE
  *             - \ref GPK_MODULE
  *             - \ref GPL_MODULE
  *             - \ref GPM_MODULE
  *             - \ref GPN_MODULE
  *             - \ref CA35_MODULE
  *             - \ref SYSCK0_MODULE
  *             - \ref SYSCK1_MODULE
  *             - \ref RTPST_MODULE
  *             - \ref LVRDB_MODULE
  *             - \ref DDR0_MODULE
  *             - \ref DDR6_MODULE
  *             - \ref CANFD0_MODULE
  *             - \ref CANFD1_MODULE
  *             - \ref CANFD2_MODULE
  *             - \ref CANFD3_MODULE
  *             - \ref SD0_MODULE
  *             - \ref SD1_MODULE
  *             - \ref NAND_MODULE
  *             - \ref USBD_MODULE
  *             - \ref USBH_MODULE
  *             - \ref HUSBH0_MODULE
  *             - \ref HUSBH1_MODULE
  *             - \ref GFX_MODULE
  *             - \ref VDEC_MODULE
  *             - \ref DCU_MODULE
  *             - \ref DCUP_MODULE
  *             - \ref GMAC0_MODULE
  *             - \ref GMAC1_MODULE
  *             - \ref CCAP0_MODULE
  *             - \ref CCAP1_MODULE
  *             - \ref TMR0_MODULE
  *             - \ref TMR1_MODULE
  *             - \ref TMR2_MODULE
  *             - \ref TMR3_MODULE
  *             - \ref TMR4_MODULE
  *             - \ref TMR5_MODULE
  *             - \ref TMR6_MODULE
  *             - \ref TMR7_MODULE
  *             - \ref TMR8_MODULE
  *             - \ref TMR9_MODULE
  *             - \ref TMR10_MODULE
  *             - \ref TMR11_MODULE
  *             - \ref UART0_MODULE
  *             - \ref UART1_MODULE
  *             - \ref UART2_MODULE
  *             - \ref UART3_MODULE
  *             - \ref UART4_MODULE
  *             - \ref UART5_MODULE
  *             - \ref UART6_MODULE
  *             - \ref UART7_MODULE
  *             - \ref UART8_MODULE
  *             - \ref UART9_MODULE
  *             - \ref UART10_MODULE
  *             - \ref UART11_MODULE
  *             - \ref UART12_MODULE
  *             - \ref UART13_MODULE
  *             - \ref UART14_MODULE
  *             - \ref UART15_MODULE
  *             - \ref UART16_MODULE
  *             - \ref RTC_MODULE
  *             - \ref DDRP_MODULE
  *             - \ref KPI_MODULE
  *             - \ref I2C0_MODULE
  *             - \ref I2C1_MODULE
  *             - \ref I2C2_MODULE
  *             - \ref I2C3_MODULE
  *             - \ref I2C4_MODULE
  *             - \ref I2C5_MODULE
  *             - \ref QSPI0_MODULE
  *             - \ref QSPI1_MODULE
  *             - \ref SC0_MODULE
  *             - \ref SC1_MODULE
  *             - \ref WDT0_MODULE
  *             - \ref WDT1_MODULE
  *             - \ref WDT2_MODULE
  *             - \ref WWDT0_MODULE
  *             - \ref WWDT1_MODULE
  *             - \ref WWDT2_MODULE
  *             - \ref EPWM0_MODULE
  *             - \ref EPWM1_MODULE
  *             - \ref EPWM2_MODULE
  *             - \ref I2S0_MODULE
  *             - \ref I2S1_MODULE
  *             - \ref SSMCC_MODULE
  *             - \ref SSPCC_MODULE
  *             - \ref SPI0_MODULE
  *             - \ref SPI1_MODULE
  *             - \ref SPI2_MODULE
  *             - \ref SPI3_MODULE
  *             - \ref ECAP0_MODULE
  *             - \ref ECAP1_MODULE
  *             - \ref ECAP2_MODULE
  *             - \ref QEI0_MODULE
  *             - \ref QEI1_MODULE
  *             - \ref QEI2_MODULE
  *             - \ref ADC_MODULE
  *             - \ref EADC_MODULE
  * @details    This function is used to enable module clock.
  */
void CLK_EnableModuleClock(uint32_t u32ModuleIdx)
{
    uint32_t u32tmpVal = 0UL, u32tmpAddr = 0UL;

    u32tmpVal = (1UL << MODULE_IP_EN_Pos(u32ModuleIdx));
    u32tmpAddr = ptr_to_u32(&CLK->SYSCLK0);
    u32tmpAddr += ((MODULE_APBCLK(u32ModuleIdx) * 4UL));

    M32(u32tmpAddr) |= u32tmpVal;
}

/**
  * @brief      Disable module clock
  * @param[in]  u32ModuleIdx is module index. Including :
  *             - \ref PDMA0_MODULE
  *             - \ref PDMA1_MODULE
  *             - \ref PDMA2_MODULE
  *             - \ref PDMA3_MODULE
  *             - \ref WH0_MODULE
  *             - \ref WH1_MODULE
  *             - \ref HWS_MODULE
  *             - \ref EBI_MODULE
  *             - \ref SRAM0_MODULE
  *             - \ref SRAM1_MODULE
  *             - \ref ROM_MODULE
  *             - \ref TRA_MODULE
  *             - \ref DBG_MODULE
  *             - \ref CLKO_MODULE
  *             - \ref GTMR_MODULE
  *             - \ref GPA_MODULE
  *             - \ref GPB_MODULE
  *             - \ref GPC_MODULE
  *             - \ref GPD_MODULE
  *             - \ref GPE_MODULE
  *             - \ref GPF_MODULE
  *             - \ref GPG_MODULE
  *             - \ref GPH_MODULE
  *             - \ref GPI_MODULE
  *             - \ref GPJ_MODULE
  *             - \ref GPK_MODULE
  *             - \ref GPL_MODULE
  *             - \ref GPM_MODULE
  *             - \ref GPN_MODULE
  *             - \ref CA35_MODULE
  *             - \ref SYSCK0_MODULE
  *             - \ref SYSCK1_MODULE
  *             - \ref RTPST_MODULE
  *             - \ref LVRDB_MODULE
  *             - \ref DDR0_MODULE
  *             - \ref DDR6_MODULE
  *             - \ref CANFD0_MODULE
  *             - \ref CANFD1_MODULE
  *             - \ref CANFD2_MODULE
  *             - \ref CANFD3_MODULE
  *             - \ref SD0_MODULE
  *             - \ref SD1_MODULE
  *             - \ref NAND_MODULE
  *             - \ref USBD_MODULE
  *             - \ref USBH_MODULE
  *             - \ref HUSBH0_MODULE
  *             - \ref HUSBH1_MODULE
  *             - \ref GFX_MODULE
  *             - \ref VDEC_MODULE
  *             - \ref DCU_MODULE
  *             - \ref DCUP_MODULE
  *             - \ref GMAC0_MODULE
  *             - \ref GMAC1_MODULE
  *             - \ref CCAP0_MODULE
  *             - \ref CCAP1_MODULE
  *             - \ref TMR0_MODULE
  *             - \ref TMR1_MODULE
  *             - \ref TMR2_MODULE
  *             - \ref TMR3_MODULE
  *             - \ref TMR4_MODULE
  *             - \ref TMR5_MODULE
  *             - \ref TMR6_MODULE
  *             - \ref TMR7_MODULE
  *             - \ref TMR8_MODULE
  *             - \ref TMR9_MODULE
  *             - \ref TMR10_MODULE
  *             - \ref TMR11_MODULE
  *             - \ref UART0_MODULE
  *             - \ref UART1_MODULE
  *             - \ref UART2_MODULE
  *             - \ref UART3_MODULE
  *             - \ref UART4_MODULE
  *             - \ref UART5_MODULE
  *             - \ref UART6_MODULE
  *             - \ref UART7_MODULE
  *             - \ref UART8_MODULE
  *             - \ref UART9_MODULE
  *             - \ref UART10_MODULE
  *             - \ref UART11_MODULE
  *             - \ref UART12_MODULE
  *             - \ref UART13_MODULE
  *             - \ref UART14_MODULE
  *             - \ref UART15_MODULE
  *             - \ref UART16_MODULE
  *             - \ref RTC_MODULE
  *             - \ref DDRP_MODULE
  *             - \ref KPI_MODULE
  *             - \ref I2C0_MODULE
  *             - \ref I2C1_MODULE
  *             - \ref I2C2_MODULE
  *             - \ref I2C3_MODULE
  *             - \ref I2C4_MODULE
  *             - \ref I2C5_MODULE
  *             - \ref QSPI0_MODULE
  *             - \ref QSPI1_MODULE
  *             - \ref SC0_MODULE
  *             - \ref SC1_MODULE
  *             - \ref WDT0_MODULE
  *             - \ref WDT1_MODULE
  *             - \ref WDT2_MODULE
  *             - \ref WWDT0_MODULE
  *             - \ref WWDT1_MODULE
  *             - \ref WWDT2_MODULE
  *             - \ref EPWM0_MODULE
  *             - \ref EPWM1_MODULE
  *             - \ref EPWM2_MODULE
  *             - \ref I2S0_MODULE
  *             - \ref I2S1_MODULE
  *             - \ref SSMCC_MODULE
  *             - \ref SSPCC_MODULE
  *             - \ref SPI0_MODULE
  *             - \ref SPI1_MODULE
  *             - \ref SPI2_MODULE
  *             - \ref SPI3_MODULE
  *             - \ref ECAP0_MODULE
  *             - \ref ECAP1_MODULE
  *             - \ref ECAP2_MODULE
  *             - \ref QEI0_MODULE
  *             - \ref QEI1_MODULE
  *             - \ref QEI2_MODULE
  *             - \ref ADC_MODULE
  *             - \ref EADC_MODULE
  * @details    This function is used to disable module clock.
  */
void CLK_DisableModuleClock(uint32_t u32ModuleIdx)
{
    uint32_t u32tmpVal = 0UL, u32tmpAddr = 0UL;

    u32tmpVal = ~(1UL << MODULE_IP_EN_Pos(u32ModuleIdx));
    u32tmpAddr = ptr_to_u32(&CLK->SYSCLK0);
    u32tmpAddr += ((MODULE_APBCLK(u32ModuleIdx) * 4UL));

    M32(u32tmpAddr) &= u32tmpVal;
}

/**
  * @brief      Get Advanced PLL Operation Mode
  * @param[in]  u32PllIdx is Advanced PLL Clock Index. Including :
  *             - \ref DDRPLL
  *             - \ref APLL
  *             - \ref EPLL
  *             - \ref VPLL
  * @return     Advanced PLL Operation Mode
  * @details    This function get Advanced PLL Operation Mode.
  */
uint32_t CLK_GetPLLOpMode(uint32_t u32PllIdx)
{
    return (CLK->PLL[u32PllIdx].CTL0 & CLK_PLLnCTL0_MODE_Msk) >> CLK_PLLnCTL0_MODE_Pos;
}

/**
  * @brief      This function check selected clock source status
  * @param[in]  u32ClkMask is selected clock source. Including :
  *             - \ref CLK_STATUS_STABLE_HXT
  *             - \ref CLK_STATUS_STABLE_LXT
  *             - \ref CLK_STATUS_STABLE_SYSPLL
  *             - \ref CLK_STATUS_STABLE_LIRC
  *             - \ref CLK_STATUS_STABLE_HIRC
  *             - \ref CLK_STATUS_STABLE_CAPLL
  *             - \ref CLK_STATUS_STABLE_DDRPLL
  *             - \ref CLK_STATUS_STABLE_EPLL
  *             - \ref CLK_STATUS_STABLE_APLL
  *             - \ref CLK_STATUS_STABLE_VPLL
  * @retval     0  clock is not stable
  * @retval     1  clock is stable
  * @details    To wait for clock ready by specified clock source stable flag or timeout (~300ms)
  */
uint32_t CLK_WaitClockReady(uint32_t u32ClkMask)
{
    int32_t i32TimeOutCnt = 2160000;
    uint32_t u32Ret = 1U;

    while ((CLK->STATUS & u32ClkMask) != u32ClkMask)
    {
        if (i32TimeOutCnt-- <= 0)
        {
            u32Ret = 0U;
            break;
        }
    }

    return u32Ret;
}

#ifndef __aarch64__
/**
  * @brief      Enable System Tick counter
  * @param[in]  u32ClkSrc is System Tick clock source. Including:
  *             - \ref CLK_CLKSEL0_RTPSTSEL_HXT
  *             - \ref CLK_CLKSEL0_RTPSTSEL_LXT
  *             - \ref CLK_CLKSEL0_RTPSTSEL_HXT_DIV2
  *             - \ref CLK_CLKSEL0_RTPSTSEL_SYSCLK1_DIV2
  *             - \ref CLK_CLKSEL0_RTPSTSEL_HIRC
  * @param[in]  u32Count is System Tick reload value. It could be 0~0xFFFFFF.
  * @details    This function set System Tick clock source, reload value, enable System Tick counter and interrupt. \n
  *             The register write-protection function should be disabled before using this function.
  */
void CLK_EnableSysTick(uint32_t u32ClkSrc, uint32_t u32Count)
{
    /* Set System Tick counter disabled */
    SysTick->CTRL = 0UL;

    /* Set System Tick clock source */
    if (u32ClkSrc == CLK_CLKSEL0_RTPSTSEL_HIRC)
    {
        SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    }
    else
    {
        CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_RTPSTSEL_Msk) | u32ClkSrc;
    }

    /* Set System Tick reload value */
    SysTick->LOAD = u32Count;

    /* Clear System Tick current value and counter flag */
    SysTick->VAL = 0UL;

    /* Set System Tick interrupt enabled and counter enabled */
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

/**
  * @brief      Disable System Tick counter
  * @details    This function disable System Tick counter.
  */
void CLK_DisableSysTick(void)
{
    /* Set System Tick counter disabled */
    SysTick->CTRL = 0UL;
}
#endif

/**
  * @brief      Get 2 PLLs and 4 Advanced PLLs Clock Frequency
  * @param[in]  u32PllIdx is a PLL Clock Index. Including :
  *             - \ref CAPLL
  *             - \ref SYSPLL
  *             - \ref DDRPLL
  *             - \ref APLL
  *             - \ref EPLL
  *             - \ref VPLL
  * @return     PLL frequency
  * @details    This function get PLL frequency. The frequency unit is Hz.
  */
uint32_t CLK_GetPLLClockFreq(uint32_t u32PllIdx)
{
    uint32_t u32Pllout = 0;

    switch (u32PllIdx)
    {
    case CAPLL:
        u32Pllout = CLK_GetCAPLLClockFreq();
        break;

    case SYSPLL:
        u32Pllout = FREQ_180MHZ;
        break;

    /* Advanced PLL */
    case DDRPLL:
    case EPLL:
    case APLL:
    case VPLL:
        u32Pllout = CLK_GetAdvPLLClockFreq(u32PllIdx);
        break;

    default:
        break;
    }

    return u32Pllout;
}

/**
  * @brief      Set PLL frequency
  * @param[in]  u32PllIdx is a PLL Clock Index. Including :
  *             - \ref CAPLL
  *             - \ref SYSPLL
  *             - \ref DDRPLL
  *             - \ref APLL
  *             - \ref EPLL
  *             - \ref VPLL
  * @param[in]  u32OpMode is a Advanced PLL Operation Mode index. Including :
  *             - \ref PLL_OPMODE_INTEGER
  *             - \ref PLL_OPMODE_FRACTIONAL
  *             - \ref PLL_OPMODE_SPREAD_SPECTRUM
  * @param[in]  u64PllSrcClk is the PLL clock input frequency.
  * @param[in]  u64PllFreq is PLL frequency.
  * @return     PLL output clock frequency
  * @details    This function is used to configure PLLCTL register to set specified PLL frequency.  The frequency unit is Hz.
  */
uint64_t CLK_SetPLLClockFreq(uint32_t u32PllIdx, uint32_t u32OpMode, uint64_t u64PllSrcClk, uint64_t u64PllFreq)
{
    uint64_t u64Pllout = 0;

    switch (u32PllIdx)
    {
        case CAPLL:
            u64Pllout = CLK_GetCAPLLClockFreq();
            break;

        case SYSPLL:
            u64Pllout = FREQ_180MHZ;
            break;

        /* Advanced  PLL */
        case DDRPLL:
        case EPLL:
        case APLL:
        case VPLL:
            u64Pllout = CLK_SetAdvPLLFreq(u32PllIdx, u32OpMode, u64PllSrcClk, u64PllFreq);
            break;

        default:
            break;
    }

    return u64Pllout;
}

/**
  * @brief      Get selected module clock source
  * @param[in]  u32ModuleIdx is module index.
  *             - \ref PDMA0_MODULE
  *             - \ref PDMA1_MODULE
  *             - \ref PDMA2_MODULE
  *             - \ref PDMA3_MODULE
  *             - \ref WH0_MODULE
  *             - \ref WH1_MODULE
  *             - \ref HWS_MODULE
  *             - \ref EBI_MODULE
  *             - \ref SRAM0_MODULE
  *             - \ref SRAM1_MODULE
  *             - \ref ROM_MODULE
  *             - \ref TRA_MODULE
  *             - \ref DBG_MODULE
  *             - \ref CLKO_MODULE
  *             - \ref GTMR_MODULE
  *             - \ref GPA_MODULE
  *             - \ref GPB_MODULE
  *             - \ref GPC_MODULE
  *             - \ref GPD_MODULE
  *             - \ref GPE_MODULE
  *             - \ref GPF_MODULE
  *             - \ref GPG_MODULE
  *             - \ref GPH_MODULE
  *             - \ref GPI_MODULE
  *             - \ref GPJ_MODULE
  *             - \ref GPK_MODULE
  *             - \ref GPL_MODULE
  *             - \ref GPM_MODULE
  *             - \ref GPN_MODULE
  *             - \ref CA35_MODULE
  *             - \ref SYSCK0_MODULE
  *             - \ref SYSCK1_MODULE
  *             - \ref RTPST_MODULE
  *             - \ref LVRDB_MODULE
  *             - \ref DDR0_MODULE
  *             - \ref DDR6_MODULE
  *             - \ref CANFD0_MODULE
  *             - \ref CANFD1_MODULE
  *             - \ref CANFD2_MODULE
  *             - \ref CANFD3_MODULE
  *             - \ref SD0_MODULE
  *             - \ref SD1_MODULE
  *             - \ref NAND_MODULE
  *             - \ref USBD_MODULE
  *             - \ref USBH_MODULE
  *             - \ref HUSBH0_MODULE
  *             - \ref HUSBH1_MODULE
  *             - \ref GFX_MODULE
  *             - \ref VDEC_MODULE
  *             - \ref DCU_MODULE
  *             - \ref DCUP_MODULE
  *             - \ref GMAC0_MODULE
  *             - \ref GMAC1_MODULE
  *             - \ref CCAP0_MODULE
  *             - \ref CCAP1_MODULE
  *             - \ref TMR0_MODULE
  *             - \ref TMR1_MODULE
  *             - \ref TMR2_MODULE
  *             - \ref TMR3_MODULE
  *             - \ref TMR4_MODULE
  *             - \ref TMR5_MODULE
  *             - \ref TMR6_MODULE
  *             - \ref TMR7_MODULE
  *             - \ref TMR8_MODULE
  *             - \ref TMR9_MODULE
  *             - \ref TMR10_MODULE
  *             - \ref TMR11_MODULE
  *             - \ref UART0_MODULE
  *             - \ref UART1_MODULE
  *             - \ref UART2_MODULE
  *             - \ref UART3_MODULE
  *             - \ref UART4_MODULE
  *             - \ref UART5_MODULE
  *             - \ref UART6_MODULE
  *             - \ref UART7_MODULE
  *             - \ref UART8_MODULE
  *             - \ref UART9_MODULE
  *             - \ref UART10_MODULE
  *             - \ref UART11_MODULE
  *             - \ref UART12_MODULE
  *             - \ref UART13_MODULE
  *             - \ref UART14_MODULE
  *             - \ref UART15_MODULE
  *             - \ref UART16_MODULE
  *             - \ref RTC_MODULE
  *             - \ref DDRP_MODULE
  *             - \ref KPI_MODULE
  *             - \ref I2C0_MODULE
  *             - \ref I2C1_MODULE
  *             - \ref I2C2_MODULE
  *             - \ref I2C3_MODULE
  *             - \ref I2C4_MODULE
  *             - \ref I2C5_MODULE
  *             - \ref QSPI0_MODULE
  *             - \ref QSPI1_MODULE
  *             - \ref SC0_MODULE
  *             - \ref SC1_MODULE
  *             - \ref WDT0_MODULE
  *             - \ref WDT1_MODULE
  *             - \ref WDT2_MODULE
  *             - \ref WWDT0_MODULE
  *             - \ref WWDT1_MODULE
  *             - \ref WWDT2_MODULE
  *             - \ref EPWM0_MODULE
  *             - \ref EPWM1_MODULE
  *             - \ref EPWM2_MODULE
  *             - \ref I2S0_MODULE
  *             - \ref I2S1_MODULE
  *             - \ref SSMCC_MODULE
  *             - \ref SSPCC_MODULE
  *             - \ref SPI0_MODULE
  *             - \ref SPI1_MODULE
  *             - \ref SPI2_MODULE
  *             - \ref SPI3_MODULE
  *             - \ref ECAP0_MODULE
  *             - \ref ECAP1_MODULE
  *             - \ref ECAP2_MODULE
  *             - \ref QEI0_MODULE
  *             - \ref QEI1_MODULE
  *             - \ref QEI2_MODULE
  *             - \ref ADC_MODULE
  *             - \ref EADC_MODULE
  * @return     Selected module clock source setting
  * @details    This function get selected module clock source.
  */
uint32_t CLK_GetModuleClockSource(uint32_t u32ModuleIdx)
{
    uint32_t u32sel = 0;
    uint32_t u32SelTbl[5] = {0x0, 0x4, 0x8, 0xC, 0x10};

    /* Get clock source selection setting */
    if (MODULE_CLKSEL_Msk(u32ModuleIdx) != MODULE_NoMsk)
    {
        /* Get clock select control register address */
        u32sel = ptr_to_u32(&CLK->CLKSEL0) + (u32SelTbl[MODULE_CLKSEL(u32ModuleIdx)]);
        /* Get clock source selection setting */
        return ((M32(u32sel) & (MODULE_CLKSEL_Msk(u32ModuleIdx) << MODULE_CLKSEL_Pos(u32ModuleIdx))) >> MODULE_CLKSEL_Pos(u32ModuleIdx));
    }
    else
        return 0;
}

/**
  * @brief      Get selected module clock divider number
  * @param[in]  u32ModuleIdx is module index.
  *             - \ref PDMA0_MODULE
  *             - \ref PDMA1_MODULE
  *             - \ref PDMA2_MODULE
  *             - \ref PDMA3_MODULE
  *             - \ref WH0_MODULE
  *             - \ref WH1_MODULE
  *             - \ref HWS_MODULE
  *             - \ref EBI_MODULE
  *             - \ref SRAM0_MODULE
  *             - \ref SRAM1_MODULE
  *             - \ref ROM_MODULE
  *             - \ref TRA_MODULE
  *             - \ref DBG_MODULE
  *             - \ref CLKO_MODULE
  *             - \ref GTMR_MODULE
  *             - \ref GPA_MODULE
  *             - \ref GPB_MODULE
  *             - \ref GPC_MODULE
  *             - \ref GPD_MODULE
  *             - \ref GPE_MODULE
  *             - \ref GPF_MODULE
  *             - \ref GPG_MODULE
  *             - \ref GPH_MODULE
  *             - \ref GPI_MODULE
  *             - \ref GPJ_MODULE
  *             - \ref GPK_MODULE
  *             - \ref GPL_MODULE
  *             - \ref GPM_MODULE
  *             - \ref GPN_MODULE
  *             - \ref CA35_MODULE
  *             - \ref SYSCK0_MODULE
  *             - \ref SYSCK1_MODULE
  *             - \ref RTPST_MODULE
  *             - \ref LVRDB_MODULE
  *             - \ref DDR0_MODULE
  *             - \ref DDR6_MODULE
  *             - \ref CANFD0_MODULE
  *             - \ref CANFD1_MODULE
  *             - \ref CANFD2_MODULE
  *             - \ref CANFD3_MODULE
  *             - \ref SD0_MODULE
  *             - \ref SD1_MODULE
  *             - \ref NAND_MODULE
  *             - \ref USBD_MODULE
  *             - \ref USBH_MODULE
  *             - \ref HUSBH0_MODULE
  *             - \ref HUSBH1_MODULE
  *             - \ref GFX_MODULE
  *             - \ref VDEC_MODULE
  *             - \ref DCU_MODULE
  *             - \ref DCUP_MODULE
  *             - \ref GMAC0_MODULE
  *             - \ref GMAC1_MODULE
  *             - \ref CCAP0_MODULE
  *             - \ref CCAP1_MODULE
  *             - \ref TMR0_MODULE
  *             - \ref TMR1_MODULE
  *             - \ref TMR2_MODULE
  *             - \ref TMR3_MODULE
  *             - \ref TMR4_MODULE
  *             - \ref TMR5_MODULE
  *             - \ref TMR6_MODULE
  *             - \ref TMR7_MODULE
  *             - \ref TMR8_MODULE
  *             - \ref TMR9_MODULE
  *             - \ref TMR10_MODULE
  *             - \ref TMR11_MODULE
  *             - \ref UART0_MODULE
  *             - \ref UART1_MODULE
  *             - \ref UART2_MODULE
  *             - \ref UART3_MODULE
  *             - \ref UART4_MODULE
  *             - \ref UART5_MODULE
  *             - \ref UART6_MODULE
  *             - \ref UART7_MODULE
  *             - \ref UART8_MODULE
  *             - \ref UART9_MODULE
  *             - \ref UART10_MODULE
  *             - \ref UART11_MODULE
  *             - \ref UART12_MODULE
  *             - \ref UART13_MODULE
  *             - \ref UART14_MODULE
  *             - \ref UART15_MODULE
  *             - \ref UART16_MODULE
  *             - \ref RTC_MODULE
  *             - \ref DDRP_MODULE
  *             - \ref KPI_MODULE
  *             - \ref I2C0_MODULE
  *             - \ref I2C1_MODULE
  *             - \ref I2C2_MODULE
  *             - \ref I2C3_MODULE
  *             - \ref I2C4_MODULE
  *             - \ref I2C5_MODULE
  *             - \ref QSPI0_MODULE
  *             - \ref QSPI1_MODULE
  *             - \ref SC0_MODULE
  *             - \ref SC1_MODULE
  *             - \ref WDT0_MODULE
  *             - \ref WDT1_MODULE
  *             - \ref WDT2_MODULE
  *             - \ref WWDT0_MODULE
  *             - \ref WWDT1_MODULE
  *             - \ref WWDT2_MODULE
  *             - \ref EPWM0_MODULE
  *             - \ref EPWM1_MODULE
  *             - \ref EPWM2_MODULE
  *             - \ref I2S0_MODULE
  *             - \ref I2S1_MODULE
  *             - \ref SSMCC_MODULE
  *             - \ref SSPCC_MODULE
  *             - \ref SPI0_MODULE
  *             - \ref SPI1_MODULE
  *             - \ref SPI2_MODULE
  *             - \ref SPI3_MODULE
  *             - \ref ECAP0_MODULE
  *             - \ref ECAP1_MODULE
  *             - \ref ECAP2_MODULE
  *             - \ref QEI0_MODULE
  *             - \ref QEI1_MODULE
  *             - \ref QEI2_MODULE
  *             - \ref ADC_MODULE
  *             - \ref EADC_MODULE
  * @return     Selected module clock divider number setting
  * @details    This function get selected module clock divider number.
  */
uint32_t CLK_GetModuleClockDivider(uint32_t u32ModuleIdx)
{
    uint32_t u32div = 0;
    uint32_t u32DivTbl[5] = {0x0, 0x4, 0xc, 0xc, 0x10};

    if (MODULE_CLKDIV_Msk(u32ModuleIdx) != MODULE_NoMsk)
    {
        if (u32ModuleIdx == KPI_MODULE)
        {
            u32div = (CLK->CLKDIV4 & CLK_CLKDIV4_KPIDIV_Msk) >> CLK_CLKDIV4_KPIDIV_Pos;
            return u32div;
        }
        else if (u32ModuleIdx == ADC_MODULE)
        {
            u32div = (CLK->CLKDIV4 & CLK_CLKDIV4_ADCDIV_Msk) >> CLK_CLKDIV4_ADCDIV_Pos;
            return u32div;
        }
        else
        {
            /* Get clock divider control register address */
            u32div = ptr_to_u32(&CLK->CLKDIV0) + (u32DivTbl[MODULE_CLKDIV(u32ModuleIdx)]);
            /* Get clock divider number setting */
            return ((M32(u32div) & (MODULE_CLKDIV_Msk(u32ModuleIdx) << MODULE_CLKDIV_Pos(u32ModuleIdx))) >> MODULE_CLKDIV_Pos(u32ModuleIdx));
        }
    }
    else
        return 0;
}

/**
  * @brief      Disable Advanced PLL
  * @param[in]  u32PllIdx is Advanced PLL clock index. Including :
  *             - \ref DDRPLL
  *             - \ref APLL
  *             - \ref EPLL
  *             - \ref VPLL
  * @details    This function set Advanced PLL in Power-down mode. \n
  *             The register write-protection function should be disabled before using this function.
  */
void CLK_DisableAdvPLL(uint32_t u32PllIdx)
{
    CLK->PLL[u32PllIdx].CTL1 |= CLK_PLLnCTL1_PD_Msk;
}

/// @cond HIDDEN_SYMBOLS

/**
  * @brief      Get CAPLL clock frequency
  * @return     CAPLL frequency
  * @details    This function get PLL frequency. The frequency unit is Hz.
  */
static uint32_t CLK_GetCAPLLClockFreq(void)
{
    uint32_t u32M, u32N, u32P;
    uint32_t u32PllClk, u32RefClk;
    uint32_t val;

    val = CLK->PLL0CTL0;

    u32N = (val & CLK_PLL0CTL0_FBDIV_Msk);
    u32M = (val & CLK_PLL0CTL0_INDIV_Msk) >> (CLK_PLL0CTL0_INDIV_Pos);
    u32P = (val & CLK_PLL0CTL0_OUTDIV_Msk) >> (CLK_PLL0CTL0_OUTDIV_Pos);

    if (val & CLK_PLL0CTL0_BP_Msk)
    {
        u32PllClk = __HXT;
    }
    else
    {
        /* u32RefClk is shifted to avoid overflow */
        u32RefClk = __HXT / 100;

        /* Actual PLL output clock frequency */
        u32PllClk = ((u32RefClk * u32N) / ((0x1 << u32P) * u32M)) * 100;
    }

    return u32PllClk;
}

/**
  * @brief      Get Advanced PLL clock frequency
  * @param[in]  u32PllIdx is Advanced PLL clock index. Including :
  *             - \ref DDRPLL
  *             - \ref APLL
  *             - \ref EPLL
  *             - \ref VPLL
  * @return     Advanced PLL frequency
  * @details    This function get Advanced PLL frequency. The frequency unit is Hz.
  */
static uint32_t CLK_GetAdvPLLClockFreq(uint32_t u32PllIdx)
{
    uint32_t u32M, u32N, u32P, u32X, u32Mode;
    uint32_t u32CTLVal0, u32CTLVal1, u32PllClk = 0ul;
    uint64_t u64Tmp;

    /* If is a CAPLL or SYSPLL, it will return 0 directly. */
    if(u32PllIdx < SYSPLL)
        return 0U;

    /* Set PLL Operation mode. */
    u32CTLVal0 = CLK->PLL[u32PllIdx].CTL0;
    u32CTLVal1 = CLK->PLL[u32PllIdx].CTL1;

    /* If is power down state, it will return 0 directly. */
    if (u32CTLVal1 & CLK_PLLnCTL1_PD_Msk)
        return 0U;

    u32Mode = (u32CTLVal0 & CLK_PLLnCTL0_MODE_Msk) >> CLK_PLLnCTL0_MODE_Pos;
    u32N = (u32CTLVal0 & CLK_PLLnCTL0_FBDIV_Msk);
    u32M = (u32CTLVal0 & CLK_PLLnCTL0_INDIV_Msk)  >> CLK_PLLnCTL0_INDIV_Pos;
    u32P = (u32CTLVal1 & CLK_PLLnCTL1_OUTDIV_Msk) >> CLK_PLLnCTL1_OUTDIV_Pos;
    u32X = (u32CTLVal1 & CLK_PLLnCTL1_FRAC_Msk)  >> CLK_PLLnCTL1_FRAC_Pos;

    if (u32Mode == PLL_OPMODE_INTEGER)
    {
        /* Actual PLL output clock frequency */
        u32PllClk = (__HXT * u32N) / (u32P * (u32M));
    }
    else
    {
        /* Using u64Tmp to avoid calculation overflow */
        u64Tmp = u32X;
        u64Tmp = (u64Tmp * 1000) + 500;
		u64Tmp = (u64Tmp >> 24);
		u32X = u64Tmp;

        /* Actual PLL output clock frequency */
        u32PllClk = (__HXT / 1000) * ((u32N * 1000) + u32X) / (u32P * u32M);
    }

    return u32PllClk;
}

/**
  * @brief      Calculate Advanced PLL Integer Mode Clock Frequency
  * @param[in]  u64PllSrcClk is the PLL clock input frequency.
  * @param[in]  u64PllFreq is Advanced PLL frequency. The range of u64PllFreq is 85.7 MHz ~ 1000 MHz.
  * @param[in]  u32Reg are the new PLL register setting.
  * @return     PLL frequency
  * @details    This function get Current Advanced PLL clock frequency. The frequency unit is Hz.
  */
static uint64_t CLK_CalPLLFreq_Mode0(uint64_t u64PllSrcClk, uint64_t u64PllFreq, uint32_t *u32Reg)
{
    uint32_t u32Tmp, u32Min, u32MinN, u32MinM, u32MinP;

    uint64_t u64PllClk;
    uint64_t u64Con1, u64Con2, u64Con3;

    /* Find best solution */
    u32Min = (uint32_t) - 1;
    u32MinM = 0UL;
    u32MinN = 0UL;
    u32MinP = 0UL;

    /* Check PLL frequency range */
    /* Output clock range: 85.7MHz to 2400MHz */
    if ((u64PllFreq < FREQ_85700KHZ) || (u64PllFreq > FREQ_2400MHZ))
    {
        u32Reg[0] = 0x307d;
        u32Reg[1] = 0x10;
        u64PllClk = FREQ_1000MHZ;
    }
    else
    {
        uint32_t u32TmpM, u32TmpN, u32TmpP;
        uint32_t u32RngMinN, u32RngMinM, u32RngMinP;
        uint32_t u32RngMaxN, u32RngMaxM, u32RngMaxP;

        u32RngMinM = 1UL;
        u32RngMaxM = 63UL;

        u32RngMinM = ((u64PllSrcClk / FREQ_40MHZ) > 1) ?
                     (u64PllSrcClk / FREQ_40MHZ) : 1;
        u32RngMaxM = ((u64PllSrcClk / FREQ_1MHZ) < u32RngMaxM) ?
                     (u64PllSrcClk / FREQ_1MHZ) : u32RngMaxM;

        /* Find best solution */
        for (u32TmpM = u32RngMinM; u32TmpM < (u32RngMaxM + 1); u32TmpM++)
        {
            u64Con1 = u64PllSrcClk / u32TmpM;

            u32RngMinN = 16UL;
            u32RngMaxN = 2047UL;

            u32RngMinN = ((FREQ_600MHZ / u64Con1) > u32RngMinN) ?
                         (FREQ_600MHZ / u64Con1) : u32RngMinN;
            u32RngMaxN = ((FREQ_2400MHZ / u64Con1) < u32RngMaxN) ?
                         (FREQ_2400MHZ / u64Con1) : u32RngMaxN;

            for (u32TmpN = u32RngMinN; u32TmpN < (u32RngMaxN + 1); u32TmpN++)
            {
                u64Con2 = u64Con1 * u32TmpN;

                u32RngMinP = 1UL;
                u32RngMaxP = 7UL;

                u32RngMinP = ((u64Con2 / FREQ_2400MHZ) > 1) ? (u64Con2 /
                             FREQ_2400MHZ) : 1;
                u32RngMaxP = ((u64Con2 / FREQ_85700KHZ) < u32RngMaxP) ?
                             (u64Con2 / FREQ_85700KHZ) : u32RngMaxP;

                for (u32TmpP = u32RngMinP; u32TmpP < (u32RngMaxP + 1); u32TmpP++)
                {
                    u64Con3 = u64Con2 / u32TmpP;
                    if (u64Con3 > u64PllFreq)
                        u32Tmp = u64Con3 - u64PllFreq;
                    else
                        u32Tmp = u64PllFreq - u64Con3;

                    if (u32Tmp < u32Min)
                    {
                        u32Min = u32Tmp;
                        u32MinM = u32TmpM;
                        u32MinN = u32TmpN;
                        u32MinP = u32TmpP;

                        /* Break when get good results */
                        if (u32Min == 0UL)
                        {
                            u32Reg[0] = (u32MinM << 12) | (u32MinN);
                            u32Reg[1] = (u32MinP << 4);
                            return ((u64PllSrcClk * u32MinN) / (u32MinP * u32MinM));
                        }
                    }
                }
            }
        }

        /* Apply new PLL setting. */
        u32Reg[0] = (u32MinM << 12) | (u32MinN);
        u32Reg[1] = (u32MinP << 4);

        /* Actual PLL output clock frequency */
        u64PllClk = (u64PllSrcClk * u32MinN) / (u32MinP * u32MinM);
    }

    return u64PllClk;
}

/**
  * @brief      Set Advanced PLL frequency
  * @param[in]  u32PllIdx is a PLL clock index. Including :
  *             - \ref DDRPLL
  *             - \ref APLL
  *             - \ref EPLL
  *             - \ref VPLL
  * @param[in]  u32OpMode is a Advanced PLL Operation Mode index. Including :
  *             - \ref PLL_OPMODE_INTEGER
  *             - \ref PLL_OPMODE_FRACTIONAL
  *             - \ref PLL_OPMODE_SPREAD_SPECTRUM
  * @param[in]  u64PllSrcClk is the PLL clock input frequency.
  * @param[in]  u64PllFreq is PLL frequency.
  * @return     PLL frequency
  * @details    This function is used to configure PLLCTL register to set specified PLL frequency. \n
  */
static uint64_t CLK_SetAdvPLLFreq(uint32_t u32PllIdx, uint32_t u32OpMode, uint64_t u64PllSrcClk, uint64_t u64PllFreq)
{
    uint32_t u32CTLVal0, u32CTLVal1, u32CTLVal2;
    uint32_t u32Reg[3] = {0};
    uint64_t u64PllClk = 0;

    /* Set PLL Operation mode. */
    u32CTLVal0 = CLK->PLL[u32PllIdx].CTL0;
    u32CTLVal1 = CLK->PLL[u32PllIdx].CTL1;
    u32CTLVal2 = CLK->PLL[u32PllIdx].CTL2;

    /* Set PLL Operation mode. */
    u32CTLVal0 = (u32CTLVal0 & ~CLK_PLLnCTL0_MODE_Msk) | (u32OpMode << CLK_PLLnCTL0_MODE_Pos);

    if (u32OpMode == PLL_OPMODE_INTEGER)
    {
        u64PllClk  = CLK_CalPLLFreq_Mode0(u64PllSrcClk, u64PllFreq, &u32Reg[0]);
        u32CTLVal0 = (u32CTLVal0 & ~(CLK_PLLnCTL0_INDIV_Msk | CLK_PLLnCTL0_FBDIV_Msk)) | u32Reg[0];
        u32CTLVal1 = (u32CTLVal1 & ~CLK_PLLnCTL1_OUTDIV_Msk) | u32Reg[1];
    }
    else
    {
        return 0;
    }

	/* Enable and apply new PLL setting. */
    CLK->PLL[u32PllIdx].CTL0 = u32CTLVal0;
    CLK->PLL[u32PllIdx].CTL1 = u32CTLVal1 & ~CLK_PLLnCTL1_PD_Msk;
    CLK->PLL[u32PllIdx].CTL2 = u32CTLVal2;

    return u64PllClk;
}
/// @endcond HIDDEN_SYMBOLS

/*! @}*/ /* end of group CLK_EXPORTED_FUNCTIONS */

/*! @}*/ /* end of group CLK_Driver */

/*! @}*/ /* end of group Standard_Driver */

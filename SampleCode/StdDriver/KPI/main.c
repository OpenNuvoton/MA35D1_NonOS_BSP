/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate KPI Controller.
 *
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

void KPI_IRQHandler(void)
{
    uint32_t u32Column_Num;
    uint32_t u32Mask_Col = 1;
    uint32_t u32Key_Event[4];
    uint32_t i, j;

    u32Column_Num = ((KPI->KPICONF & KPI_KPICONF_KCOL_Msk) >> KPI_KPICONF_KCOL_Pos) + 1;

    for (i = 0; i < u32Column_Num; i++)
    {
        u32Mask_Col = u32Mask_Col * 2;
    }

    u32Mask_Col = u32Mask_Col - 1;
    u32Mask_Col = (u32Mask_Col) | (u32Mask_Col << 8) | (u32Mask_Col << 16) |
                  (u32Mask_Col << 24);

    u32Key_Event[0] = KPI->KPIKPE0;
    u32Key_Event[1] = KPI->KPIKPE1;
    u32Key_Event[2] = KPI->KPIKRE0;
    u32Key_Event[3] = KPI->KPIKRE1;

    // Clear interrupt
    KPI->KPIKPE0 = u32Key_Event[0];
    KPI->KPIKPE1 = u32Key_Event[1];
    KPI->KPIKRE0 = u32Key_Event[2];
    KPI->KPIKRE1 = u32Key_Event[3];

    // Mask unused column
    u32Key_Event[0] = u32Key_Event[0] & u32Mask_Col;
    u32Key_Event[1] = u32Key_Event[1] & u32Mask_Col;
    u32Key_Event[2] = u32Key_Event[2] & u32Mask_Col;
    u32Key_Event[3] = u32Key_Event[3] & u32Mask_Col;

    for (j = 0; j < 4; j++)
    {
        if (u32Key_Event[j] != 0)
        {
            for (i = 0; i < 32; i++)
            {
                if (u32Key_Event [j] & (1<<i))
                {
                    if (j < 2)
                        sysprintf("\n Press Key (Row%d, Column%d) \n", i/8, i%8);
                    else
                        sysprintf("\n Release Key (Row%d, Column%d) \n", i/8, i%8);
                }
            }
        }
    }
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HXT */
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;

    /* Waiting clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Enable IP clock */
    CLK_SetModuleClock(KPI_MODULE, CLK_CLKSEL4_KPISEL_LXT, CLK_CLKDIV4_KPI(1));
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(2));
    CLK_EnableModuleClock(UART0_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Set KPI multi-function pin */
    SYS->GPF_MFPL = SYS_GPF_MFPL_PF0MFP_KPI_COL0 | SYS_GPF_MFPL_PF1MFP_KPI_COL1 |
                    SYS_GPF_MFPL_PF2MFP_KPI_COL2 | SYS_GPF_MFPL_PF3MFP_KPI_COL3 |
                    SYS_GPF_MFPL_PF4MFP_KPI_ROW0 | SYS_GPF_MFPL_PF5MFP_KPI_ROW1 |
                    SYS_GPF_MFPL_PF6MFP_KPI_ROW2 | SYS_GPF_MFPL_PF7MFP_KPI_ROW3 ;

    /* Set KPI pin is pull high */
    PF->PUSEL &= ~(GPIO_PUSEL_PUSEL0_Msk | GPIO_PUSEL_PUSEL1_Msk | GPIO_PUSEL_PUSEL2_Msk |
                   GPIO_PUSEL_PUSEL3_Msk | GPIO_PUSEL_PUSEL4_Msk | GPIO_PUSEL_PUSEL5_Msk |
                   GPIO_PUSEL_PUSEL6_Msk | GPIO_PUSEL_PUSEL7_Msk);
    PF->PUSEL |= (GPIO_PUSEL_PULL_UP << GPIO_PUSEL_PUSEL0_Pos) |
                 (GPIO_PUSEL_PULL_UP << GPIO_PUSEL_PUSEL1_Pos) |
                 (GPIO_PUSEL_PULL_UP << GPIO_PUSEL_PUSEL2_Pos) |
                 (GPIO_PUSEL_PULL_UP << GPIO_PUSEL_PUSEL3_Pos) |
                 (GPIO_PUSEL_PULL_UP << GPIO_PUSEL_PUSEL4_Pos) |
                 (GPIO_PUSEL_PULL_UP << GPIO_PUSEL_PUSEL5_Pos) |
                 (GPIO_PUSEL_PULL_UP << GPIO_PUSEL_PUSEL6_Pos) |
                 (GPIO_PUSEL_PULL_UP << GPIO_PUSEL_PUSEL7_Pos);

    /* Lock protected registers */
    SYS_LockReg();;
}

int32_t main (void)
{
    uint32_t u32RowNum = 4, u32ColNum = 4;
    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    /* KPI setting */
    KPI_Open(KPI, u32RowNum, u32ColNum);
    KPI_ConfigKeyScanTiming(KPI, 50, 3, KPI_ROW_SCAN_DELAY4CLK);

    // Clear interrupt
    KPI->KPIKPE0 = KPI->KPIKPE0;
    KPI->KPIKPE1 = KPI->KPIKPE1;
    KPI->KPIKRE0 = KPI->KPIKRE0;
    KPI->KPIKRE1 = KPI->KPIKRE1;

    IRQ_SetHandler((IRQn_ID_t)KPI_IRQn, KPI_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)KPI_IRQn);
    KPI_ENABLE_INT(KPI, KPI_KPICONF_PKINTEN_Msk|KPI_KPICONF_RKINTEN_Msk|KPI_KPICONF_INTEN_Msk);

    sysprintf("\nThis sample code demonstrate KPI function\n");
    sysprintf("\nPlease Press KPI key.");

    while(1);
}


/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate how to use EPWM counter output waveform.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"


void SYS_Init(void)
{

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

    /* Enable IP module clock */
    CLK_EnableModuleClock(EPWM0_MODULE);

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Set PI multi-function pins for EPWM0 Channel 0,1,2,3,4,5 */
    SYS->GPI_MFPL &= ~(SYS_GPI_MFPL_PI0MFP_Msk | SYS_GPI_MFPL_PI1MFP_Msk | SYS_GPI_MFPL_PI2MFP_Msk 
                        | SYS_GPI_MFPL_PI3MFP_Msk | SYS_GPI_MFPL_PI4MFP_Msk | SYS_GPI_MFPL_PI5MFP_Msk);
    SYS->GPI_MFPL |= (SYS_GPI_MFPL_PI0MFP_EPWM0_CH0 | SYS_GPI_MFPL_PI1MFP_EPWM0_CH1 | SYS_GPI_MFPL_PI2MFP_EPWM0_CH2 
                    | SYS_GPI_MFPL_PI3MFP_EPWM0_CH3 | SYS_GPI_MFPL_PI4MFP_EPWM0_CH4 | SYS_GPI_MFPL_PI5MFP_EPWM0_CH5);
}

void UART0_Init()
{
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

int32_t main(void)
{
    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART to 115200-8n1 for print message */
    UART0_Init();

    sysprintf("+------------------------------------------------------------------------+\n");
    sysprintf("|                          EPWM Driver Sample Code                        |\n");
    sysprintf("|                                                                        |\n");
    sysprintf("+------------------------------------------------------------------------+\n");
    sysprintf("  This sample code will output waveform with EPWM0 channel 0~5.\n");
    sysprintf("  I/O configuration:\n");
    sysprintf("  EPWM0 channel 0: 360000 Hz, duty 90%%.\n");
    sysprintf("  EPWM0 channel 1: 320000 Hz, duty 80%%.\n");
    sysprintf("  EPWM0 channel 2: 250000 Hz, duty 75%%.\n");
    sysprintf("  EPWM0 channel 3: 180000 Hz, duty 70%%.\n");
    sysprintf("  EPWM0 channel 4: 160000 Hz, duty 60%%.\n");
    sysprintf("  EPWM0 channel 5: 150000 Hz, duty 50%%.\n");
    sysprintf("    waveform output pin: EPWM0_CH0(PI.0), EPWM0_CH1(PI.1), EPWM0_CH2(PI.2), EPWM0_CH3(PI.3), EPWM0_CH4(PI.4), EPWM0_CH5(PI.5)\n");

    /* EPWM0 channel 0~5 frequency and duty configuration are as follows */
    EPWM_ConfigOutputChannel(EPWM0, 0, 360000, 90);
    EPWM_ConfigOutputChannel(EPWM0, 1, 320000, 80);
    EPWM_ConfigOutputChannel(EPWM0, 2, 250000, 75);
    EPWM_ConfigOutputChannel(EPWM0, 3, 180000, 70);
    EPWM_ConfigOutputChannel(EPWM0, 4, 160000, 60);
    EPWM_ConfigOutputChannel(EPWM0, 5, 150000, 50);

    /* Enable output of EPWM0 channel 0~5 */
    EPWM_EnableOutput(EPWM0, 0x3F);

    /* Start EPWM0 counter */
    EPWM_Start(EPWM0, 0x3F);

    sysprintf("Press any key to stop.\n");
    sysgetchar();

    /* Start EPWM0 counter */
    EPWM_ForceStop(EPWM0, 0x3F);

    sysprintf("Done.");
    while(1);

}



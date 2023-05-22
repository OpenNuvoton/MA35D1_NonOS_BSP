/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate EPWM accumulator stop mode.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/**
 * @brief       EPWM1 IRQ Handler
 *
 * @param       None
 *
 * @return      None
 *
 * @details     ISR to handle EPWM1 interrupt event
 */
void EPWM1P0_IRQHandler(void)
{
    EPWM_ClearAccInt(EPWM1, 0);
    sysprintf("Check if output toggles 11 times then stop toggles.\n");
}

void SYS_Init(void)
{

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

    /* Enable IP module clock */
    CLK_EnableModuleClock(EPWM1_MODULE);

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Set PK.8 multi-function pins for EPWM1 Channel 0 */
    SYS->GPK_MFPH &= ~SYS_GPK_MFPH_PK8MFP_Msk;
    SYS->GPK_MFPH |= SYS_GPK_MFPH_PK8MFP_EPWM1_CH0;
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
    sysprintf("  This sample code demonstrate EPWM1 channel 0 accumulator stop mode.\n");
    sysprintf("  When accumulator interrupt happens, the output of EPWM1 channel 0 stops.\n");
    sysprintf("  Since interrupt accumulator count is set to 10, the output toggles 11 times then stops. \n");

    sysprintf("\n\nPress any key to start EPWM1 channel 0.\n");
    sysgetchar();

    /*--------------------------------------------------------------------------------------*/
    /* Set the EPWM1 Channel 0 as EPWM output function.                                       */
    /*--------------------------------------------------------------------------------------*/

    /* Set EPWM1 channel 0 output configuration */
    EPWM_ConfigOutputChannel(EPWM1, 0, 3000, 30);

    /* Enable EPWM Output path for EPWM1 channel 0 */
    EPWM_EnableOutput(EPWM1, EPWM_CH_0_MASK);

    /* Enable EPWM1 channel 0 accumulator, interrupt count 10, accumulator source select to zero point */
    EPWM_EnableAcc(EPWM1, 0, 10, EPWM_IFA_ZERO_POINT);

    /* Enable EPWM1 channel 0 accumulator interrupt */
    EPWM_EnableAccInt(EPWM1, 0);

    /* Enable EPWM1 channel 0 interrupt in the GIC interrupt controller */
    IRQ_SetHandler((IRQn_ID_t)EPWM1P0_IRQn, EPWM1P0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)EPWM1P0_IRQn);

    /* Enable EPWM1 channel 0 accumulator stop mode */
    EPWM_EnableAccStopMode(EPWM1, 0);

    /* Enable Timer for EPWM1 channel 0 */
    EPWM_Start(EPWM1, EPWM_CH_0_MASK);

    while(1);
}



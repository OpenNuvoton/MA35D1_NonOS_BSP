/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate how to use EPWM brake function.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"


/**
 * @brief       EPWM0 Brake0 IRQ Handler
 *
 * @param       None
 *
 * @return      None
 *
 * @details     ISR to handle EPWM0 Brake0 interrupt event
 */
void BRAKE0_IRQHandler(void)
{
    sysprintf("\nFault brake!\n");
    sysprintf("Press any key to unlock brake state. (EPWM0 channel 0 output will toggle again)\n");
    sysgetchar();

    /* Unlock protected registers */
    SYS_UnlockReg();
    // Clear brake interrupt flag
    EPWM_ClearFaultBrakeIntFlag(EPWM0, EPWM_FB_EDGE);
    /* Lock protected registers */
    SYS_LockReg();
}

void SYS_Init(void)
{

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

    /* Enable IP module clock */
    CLK_EnableModuleClock(EPWM0_MODULE);

    /* Enable GPIOD module clock */
    CLK_EnableModuleClock(GPD_MODULE);

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Set PI multi-function pins for EPWM0 Channel 0,1,2,3 */
    SYS->GPI_MFPL &= ~(SYS_GPI_MFPL_PI0MFP_Msk | SYS_GPI_MFPL_PI1MFP_Msk | SYS_GPI_MFPL_PI2MFP_Msk | SYS_GPI_MFPL_PI3MFP_Msk);
    SYS->GPI_MFPL |= (SYS_GPI_MFPL_PI0MFP_EPWM0_CH0 | SYS_GPI_MFPL_PI1MFP_EPWM0_CH1 | SYS_GPI_MFPL_PI2MFP_EPWM0_CH2 | SYS_GPI_MFPL_PI3MFP_EPWM0_CH3);

    /* Set J0 multi-function pin for EPWM1 brake pin 0 */
    SYS->GPJ_MFPL =  (SYS->GPJ_MFPL & ~SYS_GPJ_MFPL_PJ0MFP_Msk) | SYS_GPJ_MFPL_PJ0MFP_EPWM1_BRAKE0;
}

void UART0_Init()
{

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for sysprintf */
    UART0_Init();

    sysprintf("\nConnect PJ.0 (EPWM1 brake pin 0) to PD.0.\n");
    sysprintf("It will generate brake interrupt and EPWM0 channel 0 output stop toggling.\n");

    GPIO_SetMode(PD, BIT0, GPIO_MODE_OUTPUT);
    PD0 = 0;

    // EPWM0 frequency is 100Hz, duty 30%,
    EPWM_ConfigOutputChannel(EPWM0, 0, 100, 30);

    // Enable output of all EPWM channels
    EPWM_EnableOutput(EPWM0, BIT5|BIT4|BIT3|BIT2|BIT1|BIT0);

    /* Unlock protected registers */
    SYS_UnlockReg();
    // Enable brake and interrupt
    EPWM_EnableFaultBrake (EPWM0, EPWM_CH_0_MASK, 1, EPWM_FB_EDGE_BKP0);
    // Set Brake pin to EPWM1_BRAKE0
    EPWM_SetBrakePinSource(EPWM0, 0, 1);
    EPWM_EnableFaultBrakeInt (EPWM0, 0);
    // Enable brake noise filter : brake pin 0, filter count=7, filter clock=HCLK/128
    EPWM_EnableBrakeNoiseFilter(EPWM0, 0, 7, EPWM_NF_CLK_DIV_128);
    // Clear brake interrupt flag
    EPWM_ClearFaultBrakeIntFlag(EPWM0, EPWM_FB_EDGE);
    /* Lock protected registers */
    SYS_LockReg();

    IRQ_SetHandler((IRQn_ID_t)BRAKE0_IRQn, BRAKE0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)BRAKE0_IRQn);

    // Start
    EPWM_Start(EPWM0, 1);

    sysprintf("\nPress any key to generate a brake event\n");
    sysgetchar();
    PD0 = 1;

    while(1);
}



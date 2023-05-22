/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Use the timer periodic mode to generate timer interrupt every 1 second
 *
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

void TMR0_IRQHandler(void)
{
    static uint32_t sec = 1;
    sysprintf("%d sec\n", sec++);

    // clear timer interrupt flag
    TIMER_ClearIntFlag(TIMER0);

}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);
}

int main(void)
{
    int volatile i;

    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    sysprintf("\nThis sample code use timer to generate interrupt every 1 second \n");

    // Set timer frequency to 1HZ
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1);

    // Enable timer interrupt
    TIMER_EnableInt(TIMER0);
    IRQ_SetHandler((IRQn_ID_t)TMR0_IRQn, TMR0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR0_IRQn);

    // Start Timer 0
    TIMER_Start(TIMER0);

    while(1);

}

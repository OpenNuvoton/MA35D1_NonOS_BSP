/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Use pin PI.0 to demonstrates timer event counter function
 *
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

void TMR0_IRQHandler(void)
{
    TIMER_ClearIntFlag(TIMER0);
    sysprintf("Count 1000 falling events! Test complete\n");
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

    /* Set timer event counting pin */
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI0MFP_TM0;

    /* Lock protected registers */
    SYS_LockReg();
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
    //global_timer_init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    sysprintf("\nThis sample code use TM0_CNT_OUT(PI.0) to count input source event\n");
    sysprintf("Please connect input source with PI.0, press any key to continue\n");
    sysgetchar();

    // Give a dummy target frequency here. Will over write prescale and compare value with macro
    TIMER_Open(TIMER0, TIMER_ONESHOT_MODE, 100);

    // Update prescale and compare value to what we need in event counter mode.
    TIMER_SET_PRESCALE_VALUE(TIMER0, 0);
    TIMER_SET_CMP_VALUE(TIMER0, 1000);
    // Counter increase on falling edge
    TIMER_EnableEventCounter(TIMER0, TIMER_COUNTER_EVENT_FALLING);
    // Start Timer 0
    TIMER_Start(TIMER0);
    // Enable timer interrupt
    TIMER_EnableInt(TIMER0);
    IRQ_SetHandler((IRQn_ID_t)TMR0_IRQn, TMR0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR0_IRQn);

    while(1);

}

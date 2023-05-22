/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Use the timer pin PI.0 to demonstrate inter timer trigger mode
 *           function. Also display the measured input frequency to UART console.
 *
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

int volatile complete = 0;

// Timer 0 is working in event count mode, and Timer 1 in capture mode, so we read the
// capture value from Timer 1, _not_ timer 0.
void TMR1_IRQHandler(void)
{

    // Timer clock is 180 MHz, counter value records the duration for 100 event counts.
    sysprintf("Event frequency is %d Hz\n", 180000000 / TIMER_GetCounter(TIMER1) * 100);
    TIMER_ClearCaptureIntFlag(TIMER1);
    complete = 1;

}

void SYS_Init(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_PCLK0, 0);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_PCLK0, 0);

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

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    /* This sample code demonstrate inter timer trigger mode using Timer0 and Timer1
     * In this mode, Timer0 is working as counter, and triggers Timer1. Using Timer1
     * to calculate the amount of time used by Timer0 to count specified amount of events.
     * By dividing the time period recorded in Timer1 by the event counts, we get
     * the event frequency.
     */
    sysprintf("Inter timer trigger mode demo code\n");
    sysprintf("Please connect input source with Timer 0 counter pin PI.0, press any key to continue\n");
    sysgetchar();

    // Give a dummy target frequency here. Will over write prescale and compare value with macro
    TIMER_Open(TIMER0, TIMER_ONESHOT_MODE, 100);

    // Update prescale and compare value. Calculate average frequency every 100 events
    TIMER_SET_PRESCALE_VALUE(TIMER0, 0);
    TIMER_SET_CMP_VALUE(TIMER0, 100);

    // Update Timer 1 prescale value. So Timer 0 clock is 180MHz
    TIMER_SET_PRESCALE_VALUE(TIMER1, 0);

    // We need capture interrupt
    IRQ_SetHandler((IRQn_ID_t)TMR1_IRQn, TMR1_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR1_IRQn);

    while(1)
    {
        complete = 0;
        // Count event by timer 0, disable drop count (set to 0), disable timeout (set to 0). Enable interrupt after complete
        TIMER_EnableFreqCounter(TIMER0, 0, 0, TRUE);
        while(complete == 0);
    }

}

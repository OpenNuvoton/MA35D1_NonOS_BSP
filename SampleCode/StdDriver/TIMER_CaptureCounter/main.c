/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Show how to use the timer0 capture function to capture timer0 counter value.
 *
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

volatile uint32_t g_au32TMRINTCount[4] = {0};

void TMR0_IRQHandler(void)
{
    if(TIMER_GetCaptureIntFlag(TIMER0) == 1)
    {
        /* Clear Timer0 capture trigger interrupt flag */
        TIMER_ClearCaptureIntFlag(TIMER0);

        g_au32TMRINTCount[0]++;
    }
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable UART clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART clock source from HXT */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Enable TIMER peripheral clock */
    CLK_EnableModuleClock(TMR3_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_HXT, 0);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HXT, 0);

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Set multi-function pins for Timer3/Timer2 toggle-output pin and Timer0 event counter pin */
    SYS->GPJ_MFPH &= ~(SYS_GPJ_MFPH_PJ12MFP_Msk | SYS_GPJ_MFPH_PJ14MFP_Msk);
    SYS->GPJ_MFPH |= (SYS_GPJ_MFPH_PJ12MFP_TM2 | SYS_GPJ_MFPH_PJ14MFP_TM3);

    /* Set multi-function pin for Timer0 external capture pin */
    SYS->GPD_MFPH &= ~(SYS_GPD_MFPH_PD8MFP_Msk | SYS_GPD_MFPH_PD9MFP_Msk);
    SYS->GPD_MFPH |= (SYS_GPD_MFPH_PD8MFP_TM0 | SYS_GPD_MFPH_PD9MFP_TM0_EXT);

    /* Lock protected registers */
    SYS_LockReg();
}


int main(void)
{
    uint32_t u32InitCount;
    uint32_t au32CAPValue[10], u32CAPDiff;

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART for sysprintf */
    UART_Open(UART0, 115200);


    sysprintf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    sysprintf("+------------------------------------------+\n");
    sysprintf("|    Timer0 Capture Counter Sample Code    |\n");
    sysprintf("+------------------------------------------+\n\n");

    sysprintf("# Timer3 Settings:\n");
    sysprintf("    - Clock source is HXT\n");
    sysprintf("    - Time-out frequency is 1000 Hz\n");
    sysprintf("    - Toggle-output mode and frequency is 500 Hz\n");
    sysprintf("# Timer2 Settings:\n");
    sysprintf("    - Clock source is HXT\n");
    sysprintf("    - Time-out frequency is 2 Hz\n");
    sysprintf("    - Toggle-output mode and frequency is 1 Hz\n");
    sysprintf("# Timer0 Settings:\n");
    sysprintf("    - Clock source is HXT               \n");
    sysprintf("    - Continuous counting mode          \n");
    sysprintf("    - Interrupt enable                  \n");
    sysprintf("    - Compared value is 0xFFFFFF        \n");
    sysprintf("    - Event counter mode enable         \n");
    sysprintf("    - External capture mode enable      \n");
    sysprintf("    - Capture trigger interrupt enable  \n");
    sysprintf("# Connect TM3(PJ.14) toggle-output pin to TM0(PD.8) event counter pin.\n");
    sysprintf("# Connect TM2(PJ.12) toggle-output pin to TM0_EXT(PD.9) external capture pin.\n\n");

    /* Enable Timer0 GIC */
    IRQ_SetHandler((IRQn_ID_t)TMR0_IRQn, TMR0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR0_IRQn);

    /* Open Timer3 in toggle-output mode and toggle-output frequency is 500 Hz*/
    TIMER_Open(TIMER3, TIMER_TOGGLE_MODE, 1000);

    /* Open Timer2 in toggle-output mode and toggle-output frequency is 1 Hz */
    TIMER_Open(TIMER2, TIMER_TOGGLE_MODE, 2);

    /* Enable Timer0 event counter input and external capture function */
    TIMER_Open(TIMER0, TIMER_CONTINUOUS_MODE, 1);
    TIMER_SET_PRESCALE_VALUE(TIMER0, 0);
    TIMER_SET_CMP_VALUE(TIMER0, 0xFFFFFF);
    TIMER_EnableEventCounter(TIMER0, TIMER_COUNTER_EVENT_FALLING);
    TIMER_EnableCapture(TIMER0, TIMER_CAPTURE_FREE_COUNTING_MODE, TIMER_CAPTURE_EVENT_FALLING);
    TIMER_EnableInt(TIMER0);
    TIMER_EnableCaptureInt(TIMER0);

    /* case 1. */
    sysprintf("# Period between two falling edge captured event should be 500 counts.\n");

    /* Clear Timer0 interrupt counts to 0 */
    u32InitCount = g_au32TMRINTCount[0] = 0;

    /* Start Timer3, Timer2 and Timer0 counting */
    TIMER_Start(TIMER3);
    TIMER_Start(TIMER2);
    TIMER_Start(TIMER0);

    /* Check Timer0 capture trigger interrupt counts */
    while(g_au32TMRINTCount[0] < 10)
    {
        if(g_au32TMRINTCount[0] != u32InitCount)
        {
            au32CAPValue[u32InitCount] = TIMER_GetCaptureData(TIMER0);
            if(u32InitCount ==  0)
            {
                sysprintf("    [%2d]: %4d. (1st captured value)\n", g_au32TMRINTCount[0], au32CAPValue[u32InitCount]);
                if(au32CAPValue[u32InitCount] != 0)   // First capture event will reset counter value
                {
                    sysprintf("*** FAIL ***\n");
                    while(1);
                }
            }
            else if(u32InitCount ==  1)
            {
                sysprintf("    [%2d]: %4d. (2nd captured value) \n", g_au32TMRINTCount[0], au32CAPValue[u32InitCount]);
                if(au32CAPValue[u32InitCount] != 500)   // Second event gets two capture event duration counts directly
                {
                    sysprintf("*** FAIL ***\n");
                    while(1);
                }
            }
            else
            {
                u32CAPDiff = au32CAPValue[u32InitCount] - au32CAPValue[u32InitCount - 1];
                sysprintf("    [%2d]: %4d. Diff: %d.\n", g_au32TMRINTCount[0], au32CAPValue[u32InitCount], u32CAPDiff);
                if(u32CAPDiff != 500)
                {
                    sysprintf("*** FAIL ***\n");
                    while(1);
                }
            }
            u32InitCount = g_au32TMRINTCount[0];
        }
    }
    sysprintf("*** PASS ***\n\n");

    /* case 2. */
    TIMER_StopCapture(TIMER0);
    TIMER_Stop(TIMER0);
    while(TIMER_IS_ACTIVE(TIMER0));
    TIMER_ClearIntFlag(TIMER0);
    TIMER_ClearCaptureIntFlag(TIMER0);
    /* Enable Timer0 event counter input and external capture function */
    TIMER_Open(TIMER0, TIMER_CONTINUOUS_MODE, 1);
    TIMER_SET_PRESCALE_VALUE(TIMER0, 0);
    TIMER_SET_CMP_VALUE(TIMER0, 0xFFFFFF);
    TIMER_EnableEventCounter(TIMER0, TIMER_COUNTER_EVENT_FALLING);
    TIMER_EnableCapture(TIMER0, TIMER_CAPTURE_FREE_COUNTING_MODE, TIMER_CAPTURE_EVENT_GET_LOW_PERIOD);
    TIMER_EnableInt(TIMER0);
    TIMER_EnableCaptureInt(TIMER0);
    TIMER_Start(TIMER0);

    sysprintf("# Get first low duration should be 250 counts.\n");
    sysprintf("# And follows duration between two rising edge captured event should be 500 counts.\n");

    /* Clear Timer0 interrupt counts to 0 */
    u32InitCount = g_au32TMRINTCount[0] = 0;

    /* Enable Timer0 event counter input and external capture function */
    TIMER0->CMP = 0xFFFFFF;
    TIMER0->CTL = TIMER_CTL_CNTEN_Msk | TIMER_CTL_INTEN_Msk | TIMER_CTL_EXTCNTEN_Msk | TIMER_CONTINUOUS_MODE;
    TIMER0->EXTCTL = TIMER_EXTCTL_CAPEN_Msk | TIMER_CAPTURE_FREE_COUNTING_MODE | TIMER_CAPTURE_EVENT_GET_LOW_PERIOD | TIMER_EXTCTL_CAPIEN_Msk;

    /* Check Timer2 capture trigger interrupt counts */
    while(g_au32TMRINTCount[0] <= 10)
    {
        if(g_au32TMRINTCount[0] != u32InitCount)
        {
            au32CAPValue[u32InitCount] = TIMER_GetCaptureData(TIMER0);
            if(u32InitCount ==  0)
            {
                sysprintf("    [%2d]: %4d. (1st captured value)\n", g_au32TMRINTCount[0], au32CAPValue[u32InitCount]);
                if(au32CAPValue[u32InitCount] != 0)   // First capture event will reset counter value
                {
                    sysprintf("*** FAIL ***\n");
                    while(1);
                }
            }
            else if(u32InitCount ==  1)
            {
                sysprintf("    [%2d]: %4d. (2nd captured value)\n", g_au32TMRINTCount[0], au32CAPValue[u32InitCount]);
                if(au32CAPValue[u32InitCount] != 250)   // Get low duration counts directly
                {
                    sysprintf("*** FAIL ***\n");
                    while(1);
                }
            }
            else
            {
                u32CAPDiff = au32CAPValue[u32InitCount] - au32CAPValue[u32InitCount - 1];
                sysprintf("    [%2d]: %4d. Diff: %d.\n", g_au32TMRINTCount[0], au32CAPValue[u32InitCount], u32CAPDiff);
                if(u32CAPDiff != 500)
                {
                    sysprintf("*** FAIL ***\n");
                    while(1);
                }
            }
            u32InitCount = g_au32TMRINTCount[0];
        }
    }

    /* Stop Timer3, Timer0 and Timer2 counting */
    TIMER_Stop(TIMER3);
    TIMER_Stop(TIMER0);
    TIMER_Stop(TIMER2);

    sysprintf("*** PASS ***\n");

    while(1)
    {
        ;
    };
}



 /**************************************************************************//**
 * @file     main.c
 *
 * @brief    Implement timer counting in periodic mode.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint32_t g_au32TMRINTCount[4] = {0};


void TMR0_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER0) == 1)
    {
        /* Clear Timer2 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);

        g_au32TMRINTCount[0]++;
    }
}


void TMR1_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        /* Clear Timer1 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER1);

        g_au32TMRINTCount[1]++;
    }
}


void TMR2_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER2) == 1)
    {
        /* Clear Timer2 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER2);

        g_au32TMRINTCount[2]++;
    }
}


void TMR3_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER3) == 1)
    {
        /* Clear Timer3 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER3);

        g_au32TMRINTCount[3]++;
    }
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Enable TIMER module clock */
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_EnableModuleClock(TMR3_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HXT, 0);
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HXT, 0);
    CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_HXT, 0);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    uint32_t u32InitCount, au32Counts[4];

    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    sysprintf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    sysprintf("+--------------------------------------------+\n");
    sysprintf("|    Timer Periodic Interrupt Sample Code    |\n");
    sysprintf("+--------------------------------------------+\n\n");

    sysprintf("# Timer0 Settings:\n");
    sysprintf("    - Clock source is HXT       \n");
    sysprintf("    - Time-out frequency is 1 Hz\n");
    sysprintf("    - Periodic mode             \n");
    sysprintf("    - Interrupt enable          \n");
    sysprintf("# Timer1 Settings:\n");
    sysprintf("    - Clock source is HXT       \n");
    sysprintf("    - Time-out frequency is 2 Hz\n");
    sysprintf("    - Periodic mode             \n");
    sysprintf("    - Interrupt enable          \n");
    sysprintf("# Timer2 Settings:\n");
    sysprintf("    - Clock source is HXT       \n");
    sysprintf("    - Time-out frequency is 4 Hz\n");
    sysprintf("    - Periodic mode             \n");
    sysprintf("    - Interrupt enable          \n");
    sysprintf("# Timer3 Settings:\n");
    sysprintf("    - Clock source is HXT       \n");
    sysprintf("    - Time-out frequency is 8 Hz\n");
    sysprintf("    - Periodic mode             \n");
    sysprintf("    - Interrupt enable          \n");
    sysprintf("# Check Timer0 ~ Timer3 interrupt counts are reasonable or not.\n\n");

    /* Open Timer0 in periodic mode, enable interrupt and 1 interrupt tick per second */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1);
    TIMER_EnableInt(TIMER0);

    /* Open Timer1 in periodic mode, enable interrupt and 2 interrupt ticks per second */
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 2);
    TIMER_EnableInt(TIMER1);

    /* Open Timer2 in periodic mode, enable interrupt and 4 interrupt ticks per second */
    TIMER_Open(TIMER2, TIMER_PERIODIC_MODE, 4);
    TIMER_EnableInt(TIMER2);

    /* Open Timer3 in periodic mode, enable interrupt and 8 interrupt ticks per second */
    TIMER_Open(TIMER3, TIMER_PERIODIC_MODE, 8);
    TIMER_EnableInt(TIMER3);

    /* Enable Timer0 ~ Timer3 GIC */
    IRQ_SetHandler((IRQn_ID_t)TMR0_IRQn, TMR0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR0_IRQn);
    IRQ_SetHandler((IRQn_ID_t)TMR1_IRQn, TMR1_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR1_IRQn);
    IRQ_SetHandler((IRQn_ID_t)TMR2_IRQn, TMR2_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR2_IRQn);
    IRQ_SetHandler((IRQn_ID_t)TMR3_IRQn, TMR3_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR3_IRQn);

    /* Clear Timer0 ~ Timer3 interrupt counts to 0 */
    g_au32TMRINTCount[0] = g_au32TMRINTCount[1] = g_au32TMRINTCount[2] = g_au32TMRINTCount[3] = 0;
    u32InitCount = g_au32TMRINTCount[0];

    /* Start Timer0 ~ Timer3 counting */
    TIMER_Start(TIMER0);
    TIMER_Start(TIMER1);
    TIMER_Start(TIMER2);
    TIMER_Start(TIMER3);

    /* Check Timer0 ~ Timer3 interrupt counts */
    sysprintf("# Timer interrupt counts :\n");
    while(u32InitCount < 20)
    {
        if(g_au32TMRINTCount[0] != u32InitCount)
        {
            au32Counts[0] = g_au32TMRINTCount[0];
            au32Counts[1] = g_au32TMRINTCount[1];
            au32Counts[2] = g_au32TMRINTCount[2];
            au32Counts[3] = g_au32TMRINTCount[3];
            sysprintf("    TMR0:%3d    TMR1:%3d    TMR2:%3d    TMR3:%3d\n",
                   au32Counts[0], au32Counts[1], au32Counts[2], au32Counts[3]);
            u32InitCount = g_au32TMRINTCount[0];

            if((au32Counts[1] > (au32Counts[0] * 2 + 1)) || (au32Counts[1] < (au32Counts[0] * 2 - 1)) ||
                    (au32Counts[2] > (au32Counts[0] * 4 + 1)) || (au32Counts[2] < (au32Counts[0] * 4 - 1)) ||
                    (au32Counts[3] > (au32Counts[0] * 8 + 1)) || (au32Counts[3] < (au32Counts[0] * 8 - 1)))
            {
                sysprintf("*** FAIL ***\n");
                while(1) {}
            }
        }
    }

    sysprintf("*** PASS ***\n");

    while(1);
}

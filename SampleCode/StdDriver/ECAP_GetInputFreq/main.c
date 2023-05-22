/**************************************************************************//**
 * @file     main.c
 * @brief    Show how to use ECAP interface to get input frequency
 *
 * @copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint32_t u32Status;
uint32_t u32IC0Hold;

void TMR2_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER2) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER2);

        /*PA.0 gpio toggle */
        PA0 ^= 1;
    }
}
/*---------------------------------------------------------------------------------------------------------*/
/*  ECAP0 IRQ Handler                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
void ECAP0_IRQHandler(void)
{
    /* Get input Capture status */
    u32Status = ECAP_GET_INT_STATUS(ECAP0);

    /* Check input capture channel 0 flag */
    if((u32Status & ECAP_STATUS_CAPTF0_Msk) == ECAP_STATUS_CAPTF0_Msk)
    {
        /* Clear input capture channel 0 flag */
        ECAP_CLR_CAPTURE_FLAG(ECAP0, ECAP_STATUS_CAPTF0_Msk);

        /* Get input capture counter hold value */
        u32IC0Hold = ECAP0->HLD0;
    }

    /* Check input capture channel 1 flag */
    if((u32Status & ECAP_STATUS_CAPTF1_Msk) == ECAP_STATUS_CAPTF1_Msk)
    {
        /* Clear input capture channel 1 flag */
        ECAP_CLR_CAPTURE_FLAG(ECAP0, ECAP_STATUS_CAPTF1_Msk);
    }

    /* Check input capture channel 2 flag */
    if((u32Status & ECAP_STATUS_CAPTF2_Msk) == ECAP_STATUS_CAPTF2_Msk)
    {
        /* Clear input capture channel 2 flag */
        ECAP_CLR_CAPTURE_FLAG(ECAP0, ECAP_STATUS_CAPTF2_Msk);
    }

    /* Check input capture compare-match flag */
    if((u32Status & ECAP_STATUS_CAPCMPF_Msk) == ECAP_STATUS_CAPCMPF_Msk)
    {
        /* Clear input capture compare-match flag */
        ECAP_CLR_CAPTURE_FLAG(ECAP0,ECAP_STATUS_CAPCMPF_Msk);
    }

    /* Check input capture overflow flag */
    if((u32Status & ECAP_STATUS_CAPOVF_Msk) == ECAP_STATUS_CAPOVF_Msk)
    {
        /* Clear input capture overflow flag */
        ECAP_CLR_CAPTURE_FLAG(ECAP0,ECAP_STATUS_CAPOVF_Msk);
    }
}


void SYS_Init(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable IP clock */
    CLK_EnableModuleClock(GPA_MODULE);
    CLK_EnableModuleClock(ECAP0_MODULE);
    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select TMR2 module clock source */
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HXT, 0);

    /* Select UART clock source from HXT */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Set multi-function pins */
    SYS->GPE_MFPL &= ~(SYS_GPE_MFPH_PE15MFP_Msk | SYS_GPE_MFPH_PE14MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE15MFP_UART0_RXD | SYS_GPE_MFPH_PE14MFP_UART0_TXD);
    SYS->GPG_MFPL &= ~(SYS_GPG_MFPL_PG5MFP_Msk | SYS_GPG_MFPL_PG6MFP_Msk | SYS_GPG_MFPL_PG7MFP_Msk);
    SYS->GPG_MFPL |= (SYS_GPG_MFPL_PG5MFP_ECAP0_IC0 | SYS_GPG_MFPL_PG6MFP_ECAP0_IC1 | SYS_GPG_MFPL_PG7MFP_ECAP0_IC2);

    /* Lock protected registers */
    SYS_LockReg();
}

void UART0_Init()
{
    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);
}

void ECAP0_Init(void)
{
    /* Enable ECAP0*/
    ECAP_Open(ECAP0, ECAP_DISABLE_COMPARE);

    /* Select Reload function */
    ECAP_SEL_RELOAD_TRIG_SRC(ECAP0, (ECAP_CTL1_CAP0RLDEN_Msk|ECAP_CTL1_CAP1RLDEN_Msk));

    /* Enable ECAP0 Input Channel 0*/
    ECAP_ENABLE_INPUT_CHANNEL(ECAP0, ECAP_CTL0_IC0EN_Msk);

    /* Enable ECAP0 source from IC0 */
    ECAP_SEL_INPUT_SRC(ECAP0, ECAP_IC0, ECAP_CAP_INPUT_SRC_FROM_IC);

    /* Select IC0 detect rising edge */
    ECAP_SEL_CAPTURE_EDGE(ECAP0, ECAP_IC0, ECAP_RISING_EDGE);

    /* Input Channel 0 interrupt enabled */
    ECAP_EnableINT(ECAP0, ECAP_CTL0_CAPIEN0_Msk);

    /* Set ecap0 interrupt callback function */
    IRQ_SetHandler((IRQn_ID_t)ECAP0_IRQn, ECAP0_IRQHandler);
}

void Timer2_Init(void)
{

    /* Open Timer2 in periodic mode, enable interrupt and 1 interrupt tick per second */
    TIMER_Open(TIMER2,TIMER_PERIODIC_MODE,10000);
    TIMER_EnableInt(TIMER2);

    /* Enable Timer2 GIC */
    IRQ_SetHandler((IRQn_ID_t)TMR2_IRQn, TMR2_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR2_IRQn);

}

/* Delay execution for given amount of ticks */
void Delay(uint32_t ticks)  {
	uint32_t tgtTicks = msTicks0 + ticks;             // target tick count to delay
	while (msTicks0 < tgtTicks);
}


int32_t main(void)
{
    uint32_t u32Hz=0, u32Hz_DET=0;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init global timer */
    global_timer_init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART for sysprintf */
    UART0_Init();

    sysprintf("\n");
    sysprintf("+----------------------------------------------------------+\n");
    sysprintf("|      Enhanced Input Capture Timer Driver Sample Code     |\n");
    sysprintf("+----------------------------------------------------------+\n");
    sysprintf("\n");
    sysprintf("  !! GPIO PA.0 toggle periodically    !!\n");
    sysprintf("  !! Connect PA.0 --> PG.5(ECAP0_IC0) !!\n\n");
    sysprintf("     Press any key to start test\n\n");
    sysgetchar();

    /* Initial ECAP0 function */
    ECAP0_Init();

    /* Initial Timer2 function */
    Timer2_Init();

    /* Configure PA.0 as output mode */
    GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);

    /* Start Timer2 counting */
    TIMER_Start(TIMER2);

    /* Delay 200ms */
    Delay(200);

    /* Init & clear ECAP interrupt status flags */
    u32Status = ECAP_GET_INT_STATUS(ECAP0);
    ECAP0->STATUS = u32Status;

    /* ECAP_CNT starts up-counting */
    ECAP_CNT_START(ECAP0);

    while(1)
    {
        if(u32Status != 0)
        {
            /* Input Capture status is changed, and get a new hold value of input capture counter */
            u32Status = 0;

            /* Calculate the IC0 input frequency */
            u32Hz_DET = ( (180000000) / (u32IC0Hold + 1) ) * 2;

            sysprintf("u32Hz %d u32Hz_DET %d\n",u32Hz,u32Hz_DET);
            if(u32Hz != u32Hz_DET)
            {
                /* If IC0 input frequency is changed, Update frequency */
                u32Hz = u32Hz_DET;
            }
            else
            {
                sysprintf("\nECAP0_IC0 input frequency is %d (Hz),u32IC0Hold=0x%08x\n", u32Hz,u32IC0Hold);
                //TIMER_Stop(TIMER2); //Disable timer Counting.
                //break;
            }
        }

    }
    /* Disable External Interrupt */
    IRQ_Disable(ECAP0_IRQn);
    IRQ_Disable(TMR2_IRQn);

    /* Disable ECAP function */
    ECAP_Close(ECAP0);

    /* Disable Timer0 IP clock */
    CLK_DisableModuleClock(TMR2_MODULE);

    /* Disable ECAP IP clock */
    CLK_DisableModuleClock(ECAP0_MODULE);

    sysprintf("\nExit ECAP sample code\n");

    while(1);
}




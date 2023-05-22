/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Change duty cycle and period of output waveform in PWM down count type.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

volatile uint32_t gu32Period;

void TMR0_IRQHandler(void)
{
    static uint32_t u32Toggle = 0;

    if(TPWM_GET_PERIOD_INT_FLAG(TIMER0))
    {
        if(u32Toggle == 0)
        {
            /* Set PWM period to generate output frequency 36000 Hz */
            TPWM_SET_PERIOD(TIMER0, ((gu32Period/2)-1));

            /* Set PWM duty, 40% */
            TPWM_SET_CMPDAT(TIMER0, (((gu32Period/2)*4) / 10));
        }
        else
        {
            /* Set PWM period to generate output frequency 18000 Hz */
            TPWM_SET_PERIOD(TIMER0, (gu32Period-1));

            /* Set PWM duty, 50% */
            TPWM_SET_CMPDAT(TIMER0, (gu32Period/2));
        }
        u32Toggle ^= 1;
        TPWM_CLEAR_PERIOD_INT_FLAG(TIMER0);
    }
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);

    /* Peripheral clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_PCLK0, 0);

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Set Timer0 PWM CH0(T0) pin */
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI0MFP_TM0;

    /* Lock protected registers */
    SYS_LockReg();
}


int main(void)
{

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Configure UART0 and set UART16 Baudrate */
    UART_Open(UART0, 115200);

    sysprintf("+-----------------------------------------------+\n");
    sysprintf("|    Timer PWM Change Duty Cycle Sample Code    |\n");
    sysprintf("+-----------------------------------------------+\n\n");

    sysprintf("# Timer0 PWM_CH0 frequency of first period is 18000 Hz and duty is 50%%.\n");
    sysprintf("# Timer0 PWM_CH0 frequency of second period is 36000 Hz and duty is 40%%.\n");
    sysprintf("# I/O configuration:\n");
    sysprintf("    - Timer0 PWM_CH0 on PI.0\n\n");

    /* Change Timer to PWM counter mode */
    TPWM_ENABLE_PWM_MODE(TIMER0);

    /* Set PWM mode as independent mode*/
    TPWM_ENABLE_INDEPENDENT_MODE(TIMER0);

    /* Set Timer0 PWM output frequency is 18000 Hz, duty 50% in up count type */
    TPWM_ConfigOutputFreqAndDuty(TIMER0, 18000, 50);

    /* Get initial period and comparator value */
    gu32Period = TPWM_GET_PERIOD(TIMER0) + 1;

    /* Set PWM down count type */
    TPWM_SET_COUNTER_TYPE(TIMER0, TPWM_DOWN_COUNT);

    /* Enable output of PWM_CH0 */
    TPWM_ENABLE_OUTPUT(TIMER0, TPWM_CH0);

    /* Enable period event interrupt */
    TPWM_ENABLE_PERIOD_INT(TIMER0);
    IRQ_SetHandler((IRQn_ID_t)TMR0_IRQn, TMR0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR0_IRQn);

    /* Start Timer PWM counter */
    TPWM_START_COUNTER(TIMER0);

    sysprintf("*** Check Timer0 PWM_CH0 output waveform by oscilloscope ***\n");

    while(1);
}

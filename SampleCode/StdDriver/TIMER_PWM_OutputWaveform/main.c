/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate output different duty waveform in Timer0~Timer3 PWM.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_EnableModuleClock(TMR3_MODULE);

    /* Peripheral clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_PCLK0, 0);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_PCLK0, 0);
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_PCLK1, 0);
    CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_PCLK1, 0);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Set Timer0~3 PWM output pins */
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI0MFP_TM0 | SYS_GPI_MFPL_PI2MFP_TM1;
    SYS->GPJ_MFPH |= SYS_GPJ_MFPH_PJ12MFP_TM2 | SYS_GPJ_MFPH_PJ14MFP_TM3;

    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    sysprintf("+-------------------------------------------------+\n");
    sysprintf("|    Timer0~Timer3 PWM Output Duty Sample Code    |\n");
    sysprintf("+-------------------------------------------------+\n\n");

    sysprintf("# Timer0 PWM_CH0 output frequency is 18000 Hz and duty is 50%%.\n");
    sysprintf("# Timer1 PWM_CH0 output frequency is 10000 Hz and duty is 10%%.\n");
    sysprintf("# Timer2 PWM_CH0 output frequency is  9000 Hz and duty is 75%%.\n");
    sysprintf("# Timer3 PWM_CH0 output frequency is  4000 Hz and duty is 20%%.\n");
    sysprintf("# I/O configuration:\n");
    sysprintf("    - Timer0 PWM_CH0 on PI.0\n");
    sysprintf("    - Timer1 PWM_CH0 on PI.2\n");
    sysprintf("    - Timer2 PWM_CH0 on PJ.12\n");
    sysprintf("    - Timer3 PWM_CH0 on PJ.14\n\n");

    /* Change Timer to PWM counter mode */
    TPWM_ENABLE_PWM_MODE(TIMER0);
    TPWM_ENABLE_PWM_MODE(TIMER1);
    TPWM_ENABLE_PWM_MODE(TIMER2);
    TPWM_ENABLE_PWM_MODE(TIMER3);

    /* Set PWM mode as independent mode*/
    TPWM_ENABLE_INDEPENDENT_MODE(TIMER0);
    TPWM_ENABLE_INDEPENDENT_MODE(TIMER1);
    TPWM_ENABLE_INDEPENDENT_MODE(TIMER2);
    TPWM_ENABLE_INDEPENDENT_MODE(TIMER3);

    /* Enable output of PWM_CH0 */
    TPWM_ENABLE_OUTPUT(TIMER0, TPWM_CH0);
    TPWM_ENABLE_OUTPUT(TIMER1, TPWM_CH0);
    TPWM_ENABLE_OUTPUT(TIMER2, TPWM_CH0);
    TPWM_ENABLE_OUTPUT(TIMER3, TPWM_CH0);

    /* Set Timer0 PWM output frequency is 18000 Hz, duty 50% in up count type */
    TPWM_ConfigOutputFreqAndDuty(TIMER0, 18000, 50);

    /* Set Timer1 PWM output frequency is 10000 Hz, duty 10% in up count type */
    TPWM_ConfigOutputFreqAndDuty(TIMER1, 10000, 10);

    /* Set Timer2 PWM output frequency is 9000 Hz, duty 75% in up count type */
    TPWM_ConfigOutputFreqAndDuty(TIMER2, 9000, 75);

    /* Set Timer3 PWM output frequency is 4000 Hz, duty 20% in up count type */
    TPWM_ConfigOutputFreqAndDuty(TIMER3, 4000, 20);

    /* Set PWM up count type */
    TPWM_SET_COUNTER_TYPE(TIMER0, TPWM_UP_COUNT);
    TPWM_SET_COUNTER_TYPE(TIMER1, TPWM_UP_COUNT);
    TPWM_SET_COUNTER_TYPE(TIMER2, TPWM_UP_COUNT);
    TPWM_SET_COUNTER_TYPE(TIMER3, TPWM_UP_COUNT);

    /* Start Timer PWM counter */
    TPWM_START_COUNTER(TIMER0);
    TPWM_START_COUNTER(TIMER1);
    TPWM_START_COUNTER(TIMER2);
    TPWM_START_COUNTER(TIMER3);

    sysprintf("*** Check Timer0~Timer3 PWM_CH0 output waveform by oscilloscope ***\n");

    while(1);
}

/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate Timer PWM Complementary mode and Dead-Time function.
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

    /* Peripheral clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_PCLK0, 0);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_PCLK0, 0);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Set Timer0 PWM CH0(TM0) and Timer1 PWM CH0(TM1) */
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI0MFP_TM0 | SYS_GPI_MFPL_PI2MFP_TM1;
    /* Set Timer0 PWM CH1(T0_EXT) and Timer1 PWM CH1(T1_EXT) */
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI1MFP_TM0_EXT | SYS_GPI_MFPL_PI3MFP_TM1_EXT;

    /* Lock protected registers */
    SYS_LockReg();
}


int main(void)
{
    uint32_t u32Period, u32CMP, u32Prescaler, u32DeadTime;

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);

    sysprintf("+--------------------------------------------------------------+\n");
    sysprintf("|    Timer PWM Complementary mode and Dead-Time Sample Code    |\n");
    sysprintf("+--------------------------------------------------------------+\n\n");

    /* Configure Timer0 PWM */
    /* Change Timer to PWM counter mode */
    TPWM_ENABLE_PWM_MODE(TIMER0);

    /* Set PWM mode as complementary mode*/
    TPWM_ENABLE_COMPLEMENTARY_MODE(TIMER0);

    /* Set Timer0 PWM output frequency is 6000 Hz, duty 40% */
    TPWM_ConfigOutputFreqAndDuty(TIMER0, 6000, 40);

    /* Enable output of PWM_CH0 and PWM_CH1 */
    TPWM_ENABLE_OUTPUT(TIMER0, (TPWM_CH1|TPWM_CH0));

    /* Get u32Prescaler, u32Period and u32CMP after called TPWM_ConfigOutputFreqAndDuty() API */
    u32Prescaler = (TIMER0->PWMCLKPSC + 1);
    u32Period = (TIMER0->PWMPERIOD + 1);
    u32CMP = TIMER0->PWMCMPDAT;
    u32DeadTime = u32CMP/2;

    sysprintf("# Timer0 PWM output frequency is 6000 Hz and duty 40%%.\n");
    sysprintf("    - Counter clock source:    PCLK0 \n");
    sysprintf("    - Counter clock prescaler: %d \n", u32Prescaler);
    sysprintf("    - Counter type:            Up count type \n");
    sysprintf("    - Operation mode:          Complementary in auto-reload mode \n");
    sysprintf("    - Period value:            %d \n", u32Period);
    sysprintf("    - Comparator value:        %d \n", u32CMP);
    sysprintf("# I/O configuration:\n");
    sysprintf("    - Timer0 PWM_CH0 on PI.0, PWM_CH1 on PI.1\n\n");


    /* Configure Timer1 PWM */
    sysprintf("# Timer1 PWM output frequency is 6000 Hz and duty 40%% with dead-time insertion.\n");
    sysprintf("    - Counter clock source:    PCLK0 \n");
    sysprintf("    - Counter clock prescaler: %d \n", u32Prescaler);
    sysprintf("    - Counter type:            Up count type \n");
    sysprintf("    - Operation mode:          Complementary in auto-reload mode \n");
    sysprintf("    - Period value:            %d \n", u32Period);
    sysprintf("    - Comparator value:        %d \n", u32CMP);
    sysprintf("    - Dead-Time interval:      %d \n", u32DeadTime);
    sysprintf("# I/O configuration:\n");
    sysprintf("    - Timer1 PWM_CH0 on PI.2, PWM_CH1 on PI.3\n\n");

    /* Change Timer to PWM counter mode */
    TPWM_ENABLE_PWM_MODE(TIMER1);

    /* Set PWM mode as complementary mode*/
    TPWM_ENABLE_COMPLEMENTARY_MODE(TIMER1);

    /* Set Timer3 PWM output frequency is 6000 Hz, duty 40% */
    TPWM_ConfigOutputFreqAndDuty(TIMER1, 6000, 40);

    /* Enable output of PWM_CH0 and PWM_CH1 */
    TPWM_ENABLE_OUTPUT(TIMER1, (TPWM_CH1|TPWM_CH0));

    /* Enable and configure dead-time interval is (u32DeadTime * TMR1_PWMCLK * prescaler) */
    SYS_UnlockReg(); // Unlock protected registers
    TPWM_EnableDeadTimeWithPrescale(TIMER1, (u32DeadTime-1));
    SYS_LockReg(); // Lock protected registers

    sysprintf("*** Check Timer0 and Timer1 PWM output waveform by oscilloscope ***\n");

    /* Start Timer0 and Timer1 PWM counter by trigger Timer0 sync. start */
    TPWM_SET_COUNTER_SYNC_MODE(TIMER0, TPWM_CNTR_SYNC_START_BY_TIMER0);
    TPWM_SET_COUNTER_SYNC_MODE(TIMER1, TPWM_CNTR_SYNC_START_BY_TIMER0);
    TPWM_TRIGGER_COUNTER_SYNC(TIMER0);

    while(1);
}

/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate how to use Timer0 PWM brake function.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

void TMR0_IRQHandler(void)
{
    sysprintf("\nFault brake!\n");
    sysprintf("Press any key to unlock brake state. (TIMER0 PWM output will toggle again)\n");
    sysgetchar();

    // Clear brake interrupt flag
    SYS_UnlockReg();
    TIMER0->PWMINTSTS1 = TIMER_PWMINTSTS1_BRKEIF0_Msk | TIMER_PWMINTSTS1_BRKEIF1_Msk | TIMER_PWMINTSTS1_BRKESTS0_Msk | TIMER_PWMINTSTS1_BRKESTS1_Msk;
    SYS_LockReg();
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

    /* Set Timer0 PWM output pins and EPWM1 brake pin 0 (TPWM_TM_BRAKE0),
       Timers share the same brake pins with EPWM */
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI0MFP_TM0;
    SYS->GPD_MFPH |= SYS_GPD_MFPH_PD10MFP_EPWM1_BRAKE0;

    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);

    sysprintf("\nL->H state change on PD.10 will generate brake interrupt,\n");
    sysprintf("and Timer0 PWM output will stop until brake state cleared.\n");

    /* Change Timer to PWM counter mode */
    TPWM_ENABLE_PWM_MODE(TIMER0);

    /* Set PWM mode as independent mode*/
    TPWM_ENABLE_INDEPENDENT_MODE(TIMER0);

    /* Set PWM up count type */
    TPWM_SET_COUNTER_TYPE(TIMER0, TPWM_UP_COUNT);

    /* Enable output of PWM_CH0 */
    TPWM_ENABLE_OUTPUT(TIMER0, TPWM_CH0);

    /* Set Timer0 PWM output frequency is 18000 Hz, duty 50% in up count type */
    TPWM_ConfigOutputFreqAndDuty(TIMER0, 18000, 50);

    /* Start Timer PWM counter */
    TPWM_START_COUNTER(TIMER0);

    // Enable brake and interrupt, PWM output stays at low after brake event
    SYS_UnlockReg();
    TPWM_SET_BRAKE_PIN_SOURCE(TIMER0, TPWM_TM_BRAKE0);
    TPWM_EnableFaultBrake(TIMER0, TPWM_OUTPUT_LOW, TPWM_OUTPUT_LOW, TPWM_BRAKE_SOURCE_EDGE_BKPIN);
    TPWM_EnableFaultBrakeInt(TIMER0, TPWM_BRAKE_EDGE);
    SYS_LockReg();

    IRQ_SetHandler((IRQn_ID_t)TMR0_IRQn, TMR0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR0_IRQn);

    while(1);
}

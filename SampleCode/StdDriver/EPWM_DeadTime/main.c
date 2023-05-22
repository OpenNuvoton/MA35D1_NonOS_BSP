/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate how to use EPWM Dead Zone function.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"


/**
 * @brief       EPWM0 IRQ Handler
 *
 * @param       None
 *
 * @return      None
 *
 * @details     ISR to handle EPWM0 interrupt event
 */
void EPWM0P0_IRQHandler(void)
{
    static uint32_t cnt;
    static uint32_t out;

    // Channel 0 frequency is 100Hz, every 1 second enter this IRQ handler 100 times.
    if(++cnt == 100)
    {
        if(out)
            EPWM_EnableOutput(EPWM0, EPWM_CH_0_MASK | EPWM_CH_1_MASK | EPWM_CH_2_MASK | EPWM_CH_3_MASK);
        else
            EPWM_DisableOutput(EPWM0, EPWM_CH_0_MASK | EPWM_CH_1_MASK | EPWM_CH_2_MASK | EPWM_CH_3_MASK);
        out ^= 1;
        cnt = 0;
    }
    // Clear channel 0 period interrupt flag
    EPWM_ClearPeriodIntFlag(EPWM0, 0);
}

void SYS_Init(void)
{

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

    /* Enable IP module clock */
    CLK_EnableModuleClock(EPWM0_MODULE);

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Set PI multi-function pins for EPWM0 Channel 0,1,2,3 */
    SYS->GPI_MFPL &= ~(SYS_GPI_MFPL_PI0MFP_Msk | SYS_GPI_MFPL_PI1MFP_Msk | SYS_GPI_MFPL_PI2MFP_Msk | SYS_GPI_MFPL_PI3MFP_Msk);
    SYS->GPI_MFPL |= (SYS_GPI_MFPL_PI0MFP_EPWM0_CH0 | SYS_GPI_MFPL_PI1MFP_EPWM0_CH1 | SYS_GPI_MFPL_PI2MFP_EPWM0_CH2 | SYS_GPI_MFPL_PI3MFP_EPWM0_CH3);
}

void UART0_Init()
{
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for sysprintf */
    UART0_Init();

    sysprintf("+------------------------------------------------------------------------+\n");
    sysprintf("|                          EPWM Driver Sample Code                        |\n");
    sysprintf("|                                                                        |\n");
    sysprintf("+------------------------------------------------------------------------+\n");
    sysprintf("  This sample code will output EPWM0 channel 0~3 with different\n");
    sysprintf("  frequency and duty, enable dead zone function of all EPWM0 pairs.\n");
    sysprintf("  And also enable/disable EPWM output every 1 second.\n");
    sysprintf("  I/O configuration:\n");
    sysprintf("    waveform output pin: EPWM0_CH0(PI.0), EPWM0_CH1(PI.1), EPWM0_CH2(PI.2), EPWM0_CH3(PI.3)\n");

    /*Set Pwm mode as complementary mode*/
    EPWM_ENABLE_COMPLEMENTARY_MODE(EPWM0);

    // EPWM0 channel 0 frequency is 10000Hz, duty 30%,
    EPWM_ConfigOutputChannel(EPWM0, 0, 10000, 30);
    SYS_UnlockReg();
    EPWM_EnableDeadZone(EPWM0, 0, 400);
    SYS_LockReg();

    // EPWM0 channel 2 frequency is 30000Hz, duty 50%
    EPWM_ConfigOutputChannel(EPWM0, 2, 30000, 50);
    SYS_UnlockReg();
    EPWM_EnableDeadZone(EPWM0, 2, 200);
    SYS_LockReg();

    // Enable output of EPWM0 channel 0~3
    EPWM_EnableOutput(EPWM0, BIT3|BIT2|BIT1|BIT0);

    // Enable EPWM0 channel 0 period interrupt, use channel 0 to measure time.
    EPWM_EnablePeriodInt(EPWM0, 0, 0);
    IRQ_SetHandler((IRQn_ID_t)EPWM0P0_IRQn, EPWM0P0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)EPWM0P0_IRQn);

    // Start
    EPWM_Start(EPWM0, BIT3|BIT2|BIT1|BIT0);

    while(1);
}



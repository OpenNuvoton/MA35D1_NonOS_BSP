/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate ADC function by repeatedly convert the input of ADC
 *           channel 4 (PB.12) and shows the result on UART console.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

uint32_t volatile u32Busy = 0;

void ADC0_IRQHandler(void)
{
    // Clear interrupt flag
    ADC_CLR_INT_FLAG(ADC0, ADC_ISR_MF_Msk);

    // Get ADC convert result
    sysprintf("Convert result is %x\n", (uint32_t)ADC_GET_CONVERSION_DATA(ADC0, 4));

    u32Busy = 0;
}


void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable IP clock */
    CLK_EnableModuleClock(ADC_MODULE);
    CLK_EnableModuleClock(GPB_MODULE);
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(ADC_MODULE, 0, CLK_CLKDIV4_ADC(20));  // Set ADC clock rate to 9MHz
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Set PB.12 to input mode */
    PB->MODE &= ~GPIO_MODE_MODE12_Msk;

    /* Set multi-function pin ADC channel 4 input*/
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~SYS_GPB_MFPH_PB12MFP_Msk) | SYS_GPB_MFPH_PB12MFP_ADC0_CH4;

    /* Disable digital input path to prevent leakage */
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT12);

    /* Set multi-function pins for UART RXD, TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* To update the variable SystemCoreClock */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();
}

int32_t main (void)
{
    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);
    sysprintf("\nThis sample code demonstrate ADC channel 4 conversion and prints the result on UART\n");

    // Enable channel 0
    ADC_Open(ADC0, ADC_INPUT_MODE_NORMAL_CONV, ADC_HIGH_SPEED_MODE, ADC_CH_4_MASK);

    // Power on ADC
    ADC_POWER_ON(ADC0);


    // Enable ADC convert complete interrupt
    ADC_EnableInt(ADC0, ADC_IER_MIEN_Msk);
    IRQ_SetHandler((IRQn_ID_t)ADC0_IRQn, ADC0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)ADC0_IRQn);

    while(1)
    {
        // Trigger ADC conversion if it is idle
        if(!u32Busy)
        {
            u32Busy = 1;
            ADC_START_CONV(ADC0);
        }
    }

}


/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate ADC 4-wire touch panel convert function.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#define Z_TH    20

volatile uint32_t u32PenDown = 2;

void ADC0_IRQHandler(void)
{
    if(u32PenDown == 0)
    {
        // Clear interrupt flag
        ADC_CLR_INT_FLAG(ADC0, ADC_ISR_PEDEF_Msk);
        // Switch from detect pen down mode to convert X/Y value
        ADC_DisableInt(ADC0, ADC_IER_PEDEIEN_Msk);
        ADC_EnableInt(ADC0, ADC_IER_MIEN_Msk);
        ADC_CONVERT_XY_MODE(ADC0);
        u32PenDown = 1;
        ADC_START_CONV(ADC0);
    }
    else if (u32PenDown == 1){
        // Clear interrupt flag
        ADC_CLR_INT_FLAG(ADC0, ADC_ISR_MF_Msk);
        // Get ADC convert result
        sysprintf("Convert result: X=%d, Y=%d, Z=%d\n",
               (uint32_t)ADC_GET_CONVERSION_XDATA(ADC0),
               (uint32_t)ADC_GET_CONVERSION_YDATA(ADC0),
               (uint32_t)ADC_GET_CONVERSION_Z1DATA(ADC0));
        if(ADC_GET_CONVERSION_Z1DATA(ADC0) < Z_TH)
        {
            // Pen up, switch from convert X/Y value to detect pen down event
            ADC_DisableInt(ADC0, ADC_IER_MIEN_Msk);
            u32PenDown = 2;
        }else
             ADC_START_CONV(ADC0);
    }
}


void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable IP clock */
    CLK_EnableModuleClock(ADC_MODULE);
    CLK_EnableModuleClock(GPB_MODULE);
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(TMR2_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(ADC_MODULE, 0, CLK_CLKDIV4_ADC(450));  // Set ADC clock rate to 400kHz
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HXT, 0);
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set multi-function pins for UART RXD, TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Set PB.12~15 to input mode */
    PB->MODE &= ~(GPIO_MODE_MODE12_Msk | GPIO_MODE_MODE13_Msk | GPIO_MODE_MODE14_Msk | GPIO_MODE_MODE15_Msk);

    /* Set multi-function pin ADC channel 4~7 input*/
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk | SYS_GPB_MFPH_PB14MFP_Msk | SYS_GPB_MFPH_PB15MFP_Msk)) | 
                    SYS_GPB_MFPH_PB12MFP_ADC0_CH4 | SYS_GPB_MFPH_PB13MFP_ADC0_CH5 | SYS_GPB_MFPH_PB14MFP_ADC0_CH6 | SYS_GPB_MFPH_PB15MFP_ADC0_CH7;

    /* Disable digital input path to prevent leakage */
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT12);
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT13);
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT14);
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT15);

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

    sysprintf("\nThis sample code demonstrate 4-wire touch panel convert function and prints the result on UART\n");

    // Enable channel 0
    ADC_Open(ADC0, ADC_INPUT_MODE_4WIRE_TOUCH, ADC_HIGH_SPEED_MODE, ADC_CH_0_MASK);

    IRQ_SetHandler((IRQn_ID_t)ADC0_IRQn, ADC0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)ADC0_IRQn);

    while(1)
    {
        // Convert X/Y value if touch detected
        if(u32PenDown == 2)
        {
            // Start to detect pen down event
            ADC_DETECT_PD_MODE(ADC0);
            TIMER_Delay(TIMER2, 200);
            ADC_CLR_INT_FLAG(ADC0, ADC_ISR_PEDEF_Msk|ADC_ISR_PEUEF_Msk);

            // Enable ADC pen down complete interrupt
            ADC_EnableInt(ADC0, ADC_IER_PEDEIEN_Msk);

            // Power on ADC
            ADC_POWER_ON(ADC0);
            u32PenDown = 0;
        }
    }
}


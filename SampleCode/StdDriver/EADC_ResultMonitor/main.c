/**************************************************************************//**
 * @file     main.c
 * @brief    Monitor the conversion result of channel 2 by the digital compare function.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables and constants                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint32_t g_u32AdcCmp0IntFlag;
volatile uint32_t g_u32AdcCmp1IntFlag;


void SYS_Init(void)
{

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

    /* Enable EADC module clock */
    CLK_EnableModuleClock(EADC_MODULE);

    /* EADC clock source is 180 MHz, set divider to 18, ADC clock is 180/18 MHz */
    CLK_SetModuleClock(EADC_MODULE, 0, CLK_CLKDIV4_EADC(18));

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Set PB.0 ~ PB.3 to input mode */
    PB->MODE &= ~(GPIO_MODE_MODE0_Msk | GPIO_MODE_MODE1_Msk | GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);
    /* Configure the GPB0 - GPB3 ADC analog input pins.  */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk |
                       SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_EADC0_CH0 | SYS_GPB_MFPL_PB1MFP_EADC0_CH1 |
                      SYS_GPB_MFPL_PB2MFP_EADC0_CH2 | SYS_GPB_MFPL_PB3MFP_EADC0_CH3);

    /* Disable the GPB0 - GPB3 digital input path to avoid the leakage current. */
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT3|BIT2|BIT1|BIT0);
}

void UART0_Init()
{

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

extern void EADC03_IRQHandler(void);
void EADC_FunctionTest()
{
    sysprintf("\n");
    sysprintf("+----------------------------------------------------------------------+\n");
    sysprintf("|           EADC compare function (result monitor) sample code         |\n");
    sysprintf("+----------------------------------------------------------------------+\n");
    sysprintf("\nIn this test, software will compare the conversion result of channel 2.\n");

    /* Set input mode as single-end and enable the A/D converter */
    EADC_Open(EADC0, EADC_CTL_DIFFEN_SINGLE_END);

    /* Configure the sample module 0 for analog input channel 2 and ADINT0 trigger source */
    EADC_ConfigSampleModule(EADC0, 0, EADC_ADINT0_TRIGGER, 2);

    /* Enable EADC comparator 0. Compare condition: conversion result < 0x800; match Count=5 */
    sysprintf("   Set the compare condition of comparator 0: channel 2 is less than 0x800; match count is 5.\n");
    EADC_ENABLE_CMP0(EADC0, 0, EADC_CMP_CMPCOND_LESS_THAN, 0x800, 5);

    /* Enable EADC comparator 1. Compare condition: conversion result >= 0x800; match Count=5 */
    sysprintf("   Set the compare condition of comparator 1 : channel 2 is greater than or equal to 0x800; match count is 5.\n");
    EADC_ENABLE_CMP1(EADC0, 0, EADC_CMP_CMPCOND_GREATER_OR_EQUAL, 0x800, 5);

    /* Enable sample module 0 for ADINT0 */
    EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, 0, BIT0);
    /* Enable ADINT0 interrupt */
    EADC_ENABLE_INT(EADC0, BIT0);

    /* Clear the A/D ADINT3 interrupt flag for safe */
    EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF3_Msk);
    /* Enable sample module 0 for ADINT3 */
    EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, 3, BIT0);
    /* Enable ADINT3 interrupt */
    EADC_ENABLE_INT(EADC0, BIT3);
    IRQ_SetHandler((IRQn_ID_t)EADC03_IRQn, EADC03_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)EADC03_IRQn);

    /* Clear the EADC comparator 0 interrupt flag for safe */
    EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADCMPF0_Msk);
    /* Enable ADC comparator 0 interrupt */
    EADC_ENABLE_CMP_INT(EADC0, 0);

    /* Clear the EADC comparator 1 interrupt flag for safe */
    EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADCMPF1_Msk);
    /* Enable ADC comparator 1 interrupt */
    EADC_ENABLE_CMP_INT(EADC0, 1);

    /* Reset the EADC interrupt indicator and trigger sample module 0 to start A/D conversion */
    g_u32AdcCmp0IntFlag = 0;
    g_u32AdcCmp1IntFlag = 0;
    EADC_START_CONV(EADC0, BIT0);

    /* Wait EADC compare interrupt */
    while((g_u32AdcCmp0IntFlag == 0) && (g_u32AdcCmp1IntFlag == 0));

    /* Disable the sample module 0 interrupt */
    EADC_DISABLE_SAMPLE_MODULE_INT(EADC0, 0, BIT0);

    /* Disable ADC comparator interrupt */
    EADC_DISABLE_CMP_INT(EADC0, 0);
    EADC_DISABLE_CMP_INT(EADC0, 1);
    /* Disable compare function */
    EADC_DISABLE_CMP0(EADC0);
    EADC_DISABLE_CMP1(EADC0);

    if(g_u32AdcCmp0IntFlag == 1)
    {
        sysprintf("Comparator 0 interrupt occurs.\nThe conversion result of channel 2 is less than 0x800\n");
    }
    else
    {
        sysprintf("Comparator 1 interrupt occurs.\nThe conversion result of channel 2 is greater than or equal to 0x800\n");
    }

}

void EADC03_IRQHandler(void)
{
    if(EADC_GET_INT_FLAG(EADC0, EADC_STATUS2_ADCMPF0_Msk))
    {
        g_u32AdcCmp0IntFlag = 1;
        EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADCMPF0_Msk);/* Clear the A/D compare flag 0 */
    }

    if(EADC_GET_INT_FLAG(EADC0, EADC_STATUS2_ADCMPF1_Msk))
    {
        g_u32AdcCmp1IntFlag = 1;
        EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADCMPF1_Msk);/* Clear the A/D compare flag 1 */
    }
    EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF3_Msk);
}

int32_t main(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for sysprintf */
    UART0_Init();

    sysprintf("\nSystem clock rate: %d Hz", SystemCoreClock);

    /* EADC function test */
    EADC_FunctionTest();

    /* Disable EADC IP clock */
    CLK_DisableModuleClock(EADC_MODULE);

    /* Disable External Interrupt */
    IRQ_Disable(EADC03_IRQn);

    sysprintf("Exit EADC sample code\n");

    while(1);

}


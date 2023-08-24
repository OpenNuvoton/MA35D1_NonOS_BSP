/**************************************************************************//**
 * @file     main.c
 * @brief    EADC convert internal channel VBAT.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables and constants                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint32_t g_u32AdcIntFlag;


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
}

void UART0_Init()
{

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

void EADC00_IRQHandler(void);
void EADC_FunctionTest()
{
    uint8_t  u8Option;
    int32_t  i32ConversionData;

    sysprintf("\n");
    sysprintf("+----------------------------------------------------------------------+\n");
    sysprintf("|                      EADC VBAT test                                  |\n");
    sysprintf("+----------------------------------------------------------------------+\n");

	/* Set input mode as single-end and enable the A/D converter */
	EADC_Open(EADC0, EADC_CTL_DIFFEN_SINGLE_END);

	/* Configure the sample module 8 for analog input channel 0 and software trigger source.*/
	EADC_ConfigSampleModule(EADC0, 8, EADC_SOFTWARE_TRIGGER, 0);

	/* Set sample module 8 external sampling time to 1 */
	//EADC_SetExtendSampleTime(EADC0, 8, 1);

	/* Set to Low speed */
	EADC0->CTL &= ~(EADC_CTL_SPEED_Msk);

	/* Set Vref = External Vref pin */
	EADC0->REFADJCTL |= EADC_REFADJCTL_PDREF_Msk;

	/* Clear the A/D ADINT0 interrupt flag for safe */
	EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF0_Msk);

	/* Enable the sample module 8 interrupt.  */
	EADC_ENABLE_INT(EADC0, BIT0);//Enable sample module A/D ADINT0 interrupt.
	EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, 0, BIT8);//Enable sample module 8 interrupt.
	IRQ_SetHandler((IRQn_ID_t)EADC00_IRQn, EADC00_IRQHandler);
	IRQ_SetTarget((IRQn_ID_t)EADC00_IRQn, (1<<1));
	IRQ_Enable ((IRQn_ID_t)EADC00_IRQn);

	/* Reset the ADC interrupt indicator and trigger sample module 8 to start A/D conversion */
	g_u32AdcIntFlag = 0;
	EADC_START_CONV(EADC0, BIT8);

	/* Wait ADC interrupt (g_u32AdcIntFlag will be set at IRQ_Handler function) */
	while(g_u32AdcIntFlag == 0);

	/* Disable the ADINT0 interrupt */
	EADC_DISABLE_INT(EADC0, BIT0);

	/* Get the conversion result of the sample module 8 */
	i32ConversionData = EADC_GET_CONV_DATA(EADC0, 8);
	sysprintf("Conversion result of VBAT/4: 0x%X (%d)\n\n", i32ConversionData, i32ConversionData);
}


void EADC00_IRQHandler(void)
{
    dcache_invalidate_by_mva((const void *)&g_u32AdcIntFlag, 4);
    g_u32AdcIntFlag = 1;
    EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF0_Msk);      /* Clear the A/D ADINT0 interrupt flag */
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
    IRQ_Disable(EADC00_IRQn);

    sysprintf("Exit EADC sample code\n");

    while(1);

}

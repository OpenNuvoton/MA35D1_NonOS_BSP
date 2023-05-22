/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate how to trigger EADC by EPWM.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables and constants                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint32_t g_u32AdcIntFlag, g_u32COVNUMFlag = 0;


void SYS_Init(void)
{

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

    /* Enable EADC module clock */
    CLK_EnableModuleClock(EADC_MODULE);

    /* EADC clock source is 180 MHz, set divider to 18, ADC clock is 180/8 MHz */
    CLK_SetModuleClock(EADC_MODULE, 0, CLK_CLKDIV4_EADC(18));

    /* Enable EPWM0 module clock */
    CLK_EnableModuleClock(EPWM0_MODULE);

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

    /* Set PG multi-function pins for EPWM0 Channel 0 */
    SYS->GPG_MFPL = (SYS->GPG_MFPL & (~SYS_GPG_MFPL_PG0MFP_Msk));
    SYS->GPG_MFPL |= SYS_GPG_MFPL_PG0MFP_EPWM0_CH0;
}

void UART0_Init()
{
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

void EPWM0_Init()
{

    /* Set EPWM0 timer clock prescaler */
    EPWM_SET_PRESCALER(EPWM0, 0, 10);

    /* Set up counter type */
    EPWM0->CTL1 &= ~EPWM_CTL1_CNTTYPE0_Msk;

    /* Set EPWM0 timer duty */
    EPWM_SET_CMR(EPWM0, 0, 1000);

    /* Set EPWM0 timer period */
    EPWM_SET_CNR(EPWM0, 0, 2000);

    /* EPWM period point trigger ADC enable */
    EPWM_EnableADCTrigger(EPWM0, 0, EPWM_TRG_ADC_EVEN_PERIOD);

    /* Set output level at zero, compare up, period(center) and compare down of specified channel */
    EPWM_SET_OUTPUT_LEVEL(EPWM0, BIT0, EPWM_OUTPUT_HIGH, EPWM_OUTPUT_LOW, EPWM_OUTPUT_NOTHING, EPWM_OUTPUT_NOTHING);

    /* Enable output of EPWM0 channel 0 */
    EPWM_EnableOutput(EPWM0, BIT0);

}

void EADC00_IRQHandler(void)
{
    EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF0_Msk);      /* Clear the A/D ADINT0 interrupt flag */
    g_u32AdcIntFlag = 1;
    g_u32COVNUMFlag++;
}

void EADC_FunctionTest()
{
    uint8_t  u8Option;
    int32_t  i32ConversionData[6] = {0};

    sysprintf("\n");
    sysprintf("+----------------------------------------------------------------------+\n");
    sysprintf("|                       EPWM trigger mode test                          |\n");
    sysprintf("+----------------------------------------------------------------------+\n");

    sysprintf("\nIn this test, software will get 6 conversion result from the specified channel.\n");

    while(1)
    {
        sysprintf("Select input mode:\n");
        sysprintf("  [1] Single end input (channel 2 only)\n");
        sysprintf("  [2] Differential input (channel pair 1 only)\n");
        sysprintf("  Other keys: exit single mode test\n");
        u8Option = sysgetchar();
        if(u8Option == '1')
        {
            /* Set input mode as single-end and enable the A/D converter */
            EADC_Open(EADC0, EADC_CTL_DIFFEN_SINGLE_END);

            /* Configure the sample module 0 for analog input channel 2 and enable EPWM0 trigger source */
            EADC_ConfigSampleModule(EADC0, 0, EADC_EPWM0TG0_TRIGGER, 2);

            /* Clear the A/D ADINT0 interrupt flag for safe */
            EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF0_Msk);

            /* Enable the sample module 0 interrupt */
            EADC_ENABLE_INT(EADC0, BIT0);//Enable sample module A/D ADINT0 interrupt.
            EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, 0, BIT0);//Enable sample module 0 interrupt.
            IRQ_SetHandler((IRQn_ID_t)EADC00_IRQn, EADC00_IRQHandler);
            IRQ_Enable ((IRQn_ID_t)EADC00_IRQn);

            sysprintf("Conversion result of channel 2:\n");

            /* Reset the EADC indicator and enable EPWM0 channel 0 counter */
            g_u32AdcIntFlag = 0;
            g_u32COVNUMFlag = 0;
            EPWM_Start(EPWM0, BIT0); //EPWM0 channel 0 counter start running.

            while(1)
            {
                /* Wait ADC interrupt (g_u32AdcIntFlag will be set at IRQ_Handler function) */
                while(g_u32AdcIntFlag == 0);

                /* Reset the ADC interrupt indicator */
                g_u32AdcIntFlag = 0;

                /* Get the conversion result of the sample module 0 */
                i32ConversionData[g_u32COVNUMFlag - 1] = EADC_GET_CONV_DATA(EADC0, 0);

                if(g_u32COVNUMFlag > 6)
                    break;
            }

            /* Disable EPWM0 channel 0 counter */
            EPWM_ForceStop(EPWM0, BIT0); //EPWM0 counter stop running.

            /* Disable sample module 0 interrupt */
            EADC_DISABLE_SAMPLE_MODULE_INT(EADC0, 0, BIT0);

            for(g_u32COVNUMFlag = 0; (g_u32COVNUMFlag) < 6; g_u32COVNUMFlag++)
                sysprintf("                                0x%X (%d)\n", i32ConversionData[g_u32COVNUMFlag], i32ConversionData[g_u32COVNUMFlag]);

        }
        else if(u8Option == '2')
        {

            /* Set input mode as differential and enable the A/D converter */
            EADC_Open(EADC0, EADC_CTL_DIFFEN_DIFFERENTIAL);
            /* Configure the sample module 0 for analog input channel 2 and software trigger source.*/
            EADC_ConfigSampleModule(EADC0, 0, EADC_EPWM0TG0_TRIGGER, 2);

            /* Clear the A/D ADINT0 interrupt flag for safe */
            EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF0_Msk);

            /* Enable the sample module 0 interrupt */
            EADC_ENABLE_INT(EADC0, BIT0);//Enable sample module A/D ADINT0 interrupt.
            EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, 0, BIT0);//Enable sample module 0 interrupt.
            IRQ_SetHandler((IRQn_ID_t)EADC00_IRQn, EADC00_IRQHandler);
            IRQ_Enable ((IRQn_ID_t)EADC00_IRQn);

            sysprintf("Conversion result of channel 2:\n");

            /* Reset the EADC indicator and enable EPWM0 channel 0 counter */
            g_u32AdcIntFlag = 0;
            g_u32COVNUMFlag = 0;
            EPWM_Start(EPWM0, BIT0); //EPWM0 channel 0 counter start running.

            while(1)
            {
                /* Wait ADC interrupt (g_u32AdcIntFlag will be set at IRQ_Handler function) */
                while(g_u32AdcIntFlag == 0);

                /* Reset the ADC interrupt indicator */
                g_u32AdcIntFlag = 0;

                /* Get the conversion result of the sample module 0   */
                i32ConversionData[g_u32COVNUMFlag - 1] = EADC_GET_CONV_DATA(EADC0, 0);

                if(g_u32COVNUMFlag > 6)
                    break;
            }

            /* Disable EPWM0 channel 0 counter */
            EPWM_ForceStop(EPWM0, BIT0); //EPWM0 counter stop running.

            /* Disable sample module 0 interrupt */
            EADC_DISABLE_SAMPLE_MODULE_INT(EADC0, 0, BIT0);

            for(g_u32COVNUMFlag = 0; (g_u32COVNUMFlag) < 6; g_u32COVNUMFlag++)
                sysprintf("                                0x%X (%d)\n", i32ConversionData[g_u32COVNUMFlag], i32ConversionData[g_u32COVNUMFlag]);

        }
        else
            return ;

    }
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

    /* Init EPWM for EADC */
    EPWM0_Init();

    sysprintf("\nSystem clock rate: %d Hz", SystemCoreClock);

    /* EADC function test */
    EADC_FunctionTest();

    /* Disable EADC IP clock */
    CLK_DisableModuleClock(EADC_MODULE);

    /* Disable EPWM0 IP clock */
    CLK_DisableModuleClock(EPWM0_MODULE);

    /* Disable External Interrupt */
    IRQ_Disable((IRQn_ID_t)EADC00_IRQn);

    sysprintf("Exit EADC sample code\n");

    while(1);

}




/**************************************************************************//**
 * @file     main.c
 * @brief    Demonstrate how to trigger multiple sample modules and got conversion results in order of priority.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables and constants                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint32_t g_u32EadcInt0Flag, g_u32EadcInt1Flag, g_u32EadcInt2Flag, g_u32EadcInt3Flag;

uint32_t g_u32IntModule[4];    /* save the sample module number for ADINT0~3 */
volatile uint32_t g_u32IntSequence[4];  /* save the interrupt sequence for ADINT0~3 */
volatile uint32_t g_u32IntSequenceIndex;

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

    /* Set PB.0, PB.4 ~ PB.7 to input mode */
    PB->MODE &= ~(GPIO_MODE_MODE0_Msk | GPIO_MODE_MODE4_Msk | GPIO_MODE_MODE5_Msk |
                  GPIO_MODE_MODE6_Msk | GPIO_MODE_MODE7_Msk);
    /* Configure the EADC analog input pins.  */
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB0MFP_Msk) | SYS_GPB_MFPL_PB0MFP_EADC0_CH0;
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB4MFP_Msk) | SYS_GPB_MFPL_PB4MFP_EADC0_CH4;
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB5MFP_Msk) | SYS_GPB_MFPL_PB5MFP_EADC0_CH5;
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB6MFP_Msk) | SYS_GPB_MFPL_PB6MFP_EADC0_CH6;
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB7MFP_Msk) | SYS_GPB_MFPL_PB7MFP_EADC0_CH7;

    /* Disable the GPB0, GPB4~7 digital input path to avoid the leakage current. */
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT7|BIT6|BIT5|BIT4|BIT0);
}

void UART0_Init()
{

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

void EADC00_IRQHandler(void);
void EADC01_IRQHandler(void);
void EADC02_IRQHandler(void);
void EADC03_IRQHandler(void);
void EADC_FunctionTest()
{
    uint8_t  u8Option;
    int32_t  i32ConversionData, i;

    sysprintf("\n");
    sysprintf("+----------------------------------------------------------------------+\n");
    sysprintf("|                 EADC Pending Priority sample code                    |\n");
    sysprintf("+----------------------------------------------------------------------+\n");

    /* Set the EADC and enable the A/D converter */
    EADC_Open(EADC0, ptr_to_u32(NULL));

    while (1)
    {
        sysprintf("Select test items:\n");
        sysprintf("  [1] Assign interrupt ADINT0~3 to Sample Module 0~3\n");
        sysprintf("  [2] Assign interrupt ADINT3~0 to Sample Module 0~3\n");
        sysprintf("  Other keys: exit EADC test\n");
        u8Option = sysgetchar();

        if (u8Option == '1')
        {
            g_u32IntModule[0] = 0;  /* Assign ADINT0 to Sample module 0 */
            g_u32IntModule[1] = 1;  /* Assign ADINT1 to Sample module 1 */
            g_u32IntModule[2] = 2;  /* Assign ADINT2 to Sample module 2 */
            g_u32IntModule[3] = 3;  /* Assign ADINT3 to Sample module 3 */
        }
        else if (u8Option == '2')
        {
            g_u32IntModule[0] = 3;  /* Assign ADINT0 to Sample module 3 */
            g_u32IntModule[1] = 2;  /* Assign ADINT1 to Sample module 2 */
            g_u32IntModule[2] = 1;  /* Assign ADINT2 to Sample module 1 */
            g_u32IntModule[3] = 0;  /* Assign ADINT3 to Sample module 0 */
        }
        else
            break;  /* exit while loop */

        /* Configure the sample module for analog input channel and software trigger source. */
        EADC_ConfigSampleModule(EADC0, 0, EADC_SOFTWARE_TRIGGER, 4);
        EADC_ConfigSampleModule(EADC0, 1, EADC_SOFTWARE_TRIGGER, 5);
        EADC_ConfigSampleModule(EADC0, 2, EADC_SOFTWARE_TRIGGER, 6);
        EADC_ConfigSampleModule(EADC0, 3, EADC_SOFTWARE_TRIGGER, 7);

        /* Clear the A/D ADINTx interrupt flag for safe */
        EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF0_Msk | EADC_STATUS2_ADIF1_Msk | EADC_STATUS2_ADIF2_Msk | EADC_STATUS2_ADIF3_Msk);

        /* Enable the sample module interrupt.  */
        EADC_ENABLE_INT(EADC0, BIT0 | BIT1 | BIT2 | BIT3);

        EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, 0, BIT0 << g_u32IntModule[0]);
        EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, 1, BIT0 << g_u32IntModule[1]);
        EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, 2, BIT0 << g_u32IntModule[2]);
        EADC_ENABLE_SAMPLE_MODULE_INT(EADC0, 3, BIT0 << g_u32IntModule[3]);

        IRQ_SetHandler((IRQn_ID_t)EADC00_IRQn, EADC00_IRQHandler);
        IRQ_Enable ((IRQn_ID_t)EADC00_IRQn);
        IRQ_SetHandler((IRQn_ID_t)EADC01_IRQn, EADC01_IRQHandler);
        IRQ_Enable ((IRQn_ID_t)EADC01_IRQn);
        IRQ_SetHandler((IRQn_ID_t)EADC02_IRQn, EADC02_IRQHandler);
        IRQ_Enable ((IRQn_ID_t)EADC02_IRQn);
        IRQ_SetHandler((IRQn_ID_t)EADC03_IRQn, EADC03_IRQHandler);
        IRQ_Enable ((IRQn_ID_t)EADC03_IRQn);

        /* Reset the EADC interrupt indicator and trigger sample module to start A/D conversion */
        g_u32IntSequenceIndex = 0;
        g_u32EadcInt0Flag = 0;
        g_u32EadcInt1Flag = 0;
        g_u32EadcInt2Flag = 0;
        g_u32EadcInt3Flag = 0;

        /* Start EADC conversion for sample module 0 ~ 3 at the same time */
        EADC_START_CONV(EADC0, BIT0 | BIT1 | BIT2 | BIT3);

        /* Wait all EADC interrupt (g_u32EadcIntxFlag will be set at EADC_INTx_IRQHandler() function) */
        while ((g_u32EadcInt0Flag == 0) || (g_u32EadcInt1Flag == 0) ||
                (g_u32EadcInt2Flag == 0) || (g_u32EadcInt3Flag == 0));

        //        for (i=0; i<80000; i++) __NOP();

        /* Get the conversion result of the sample module */
        sysprintf("The ADINTx interrupt sequence is:\n");

        for (i = 0; i < 4; i++)
        {
            i32ConversionData = EADC_GET_CONV_DATA(EADC0, g_u32IntModule[i]);
            sysprintf("ADINT%d: #%d, Module %d, Conversion result: 0x%X (%d)\n", i, g_u32IntSequence[i], g_u32IntModule[i], i32ConversionData, i32ConversionData);
        }

        sysprintf("\n");

        /* Disable the ADINTx interrupt */
        EADC_DISABLE_INT(EADC0, BIT0 | BIT1 | BIT2 | BIT3);

        EADC_DISABLE_SAMPLE_MODULE_INT(EADC0, 0, BIT0 << g_u32IntModule[0]);
        EADC_DISABLE_SAMPLE_MODULE_INT(EADC0, 1, BIT0 << g_u32IntModule[1]);
        EADC_DISABLE_SAMPLE_MODULE_INT(EADC0, 2, BIT0 << g_u32IntModule[2]);
        EADC_DISABLE_SAMPLE_MODULE_INT(EADC0, 3, BIT0 << g_u32IntModule[3]);

        IRQ_Disable(EADC00_IRQn);
        IRQ_Disable(EADC01_IRQn);
        IRQ_Disable(EADC02_IRQn);
        IRQ_Disable(EADC03_IRQn);
    }   /* End of while(1) */

    /* Disable the A/D converter */
    EADC_Close(EADC0);
}



void EADC00_IRQHandler(void)
{
    g_u32EadcInt0Flag = 1;
    /* Clear the A/D ADINT0 interrupt flag */
    EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF0_Msk);

    /* Save the interrupt sequence about ADINT0 */
    g_u32IntSequence[0] = g_u32IntSequenceIndex++;
}

void EADC01_IRQHandler(void)
{
    g_u32EadcInt1Flag = 1;
    /* Clear the A/D ADINT1 interrupt flag */
    EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF1_Msk);

    /* Save the interrupt sequence about ADINT1 */
    g_u32IntSequence[1] = g_u32IntSequenceIndex++;
}

void EADC02_IRQHandler(void)
{
    g_u32EadcInt2Flag = 1;
    /* Clear the A/D ADINT2 interrupt flag */
    EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF2_Msk);

    /* Save the interrupt sequence about ADINT2 */
    g_u32IntSequence[2] = g_u32IntSequenceIndex++;
}

void EADC03_IRQHandler(void)
{
    g_u32EadcInt3Flag = 1;
    /* Clear the A/D ADINT3 interrupt flag */
    EADC_CLR_INT_FLAG(EADC0, EADC_STATUS2_ADIF3_Msk);

    /* Save the interrupt sequence about ADINT3 */
    g_u32IntSequence[3] = g_u32IntSequenceIndex++;
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
    IRQ_Disable(EADC01_IRQn);
    IRQ_Disable(EADC02_IRQn);
    IRQ_Disable(EADC03_IRQn);

    sysprintf("Exit EADC sample code\n");

    while(1);

}


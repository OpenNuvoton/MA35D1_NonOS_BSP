/**************************************************************************//**
 * @file     main.c
 * @brief    Show the usage of GPIO interrupt function.
 *
 * @copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/**
 * @brief       GPIO PA IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The PA default IRQ.
 */
void GPA_IRQHandler(void)
{
    /* To check if PA.2 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PA, BIT2))
    {
        GPIO_CLR_INT_FLAG(PA, BIT2);
        sysprintf("PA.2 INT occurred.\n");
    }
    else
    {
        /* Un-expected interrupt. Just clear all PA interrupts */
    	GPIO_CLR_INT_FLAG(PA, PA->INTSRC);
        sysprintf("Un-expected interrupts.\n");
    }
}

/**
 * @brief       GPIO PC IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The PC default IRQ.
 */
void GPC_IRQHandler(void)
{
    /* To check if PC.5 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PC, BIT5))
    {
        GPIO_CLR_INT_FLAG(PC, BIT5);
        sysprintf("PC.5 INT occurred.\n");
    }
    else
    {
        /* Un-expected interrupt. Just clear all PC interrupts */
        GPIO_CLR_INT_FLAG(PC, PC->INTSRC);
        sysprintf("Un-expected interrupts.\n");
    }
}

void SYS_Init(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable IP clock */
    CLK_EnableModuleClock(GPA_MODULE);
    CLK_EnableModuleClock(GPB_MODULE);
    CLK_EnableModuleClock(GPC_MODULE);
    CLK_EnableModuleClock(GPD_MODULE);
    CLK_EnableModuleClock(GPE_MODULE);
    CLK_EnableModuleClock(GPF_MODULE);
    CLK_EnableModuleClock(GPG_MODULE);
    CLK_EnableModuleClock(GPH_MODULE);
    CLK_EnableModuleClock(GPI_MODULE);
    CLK_EnableModuleClock(GPJ_MODULE);
    CLK_EnableModuleClock(GPK_MODULE);
    CLK_EnableModuleClock(GPL_MODULE);
    CLK_EnableModuleClock(GPM_MODULE);
    CLK_EnableModuleClock(GPN_MODULE);
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART clock source from HXT */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Set multi-function pins */
    SYS->GPC_MFPL &= ~SYS_GPC_MFPL_PC5MFP_Msk;
    SYS->GPA_MFPL &= ~SYS_GPA_MFPL_PA2MFP_Msk;
    SYS->GPE_MFPL &= ~(SYS_GPE_MFPH_PE15MFP_Msk | SYS_GPE_MFPH_PE14MFP_Msk);
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE15MFP_UART0_RXD | SYS_GPE_MFPH_PE14MFP_UART0_TXD);
    /* Lock protected registers */
    SYS_LockReg();
}

void UART0_Init()
{
    /* Init UART to 115200-8n1 for print message */
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

    sysprintf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    sysprintf("+------------------------------------------------+\n");
    sysprintf("|    GPIO PA.2 and PC.5 Interrupt Sample Code    |\n");
    sysprintf("+------------------------------------------------+\n\n");

    /*-----------------------------------------------------------------------------------------------------*/
    /* GPIO Interrupt Function Test                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    sysprintf("PA.2 and PC.5 are used to test interrupt ......\n");

    /* Configure PA.2 as Input mode and enable interrupt by rising edge trigger */
    GPIO_SetMode(PA, BIT2, GPIO_MODE_INPUT);
    GPIO_EnableInt(PA, 2, GPIO_INT_RISING);
    IRQ_SetHandler((IRQn_ID_t)GPA_IRQn, GPA_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)GPA_IRQn);

    /* Configure PC.5 as Quasi-bidirection mode and enable interrupt by falling edge trigger */
    GPIO_SetMode(PC, BIT5, GPIO_MODE_QUASI);
    GPIO_EnableInt(PC, 5, GPIO_INT_FALLING);
    IRQ_SetHandler((IRQn_ID_t)GPC_IRQn, GPC_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)GPC_IRQn);

    /* Enable interrupt de-bounce function and select de-bounce sampling cycle time is 1024 clocks of HXT clock */
    GPIO_SET_DEBOUNCE_TIME(PA, GPIO_DBCTL_DBCLKSRC_HXT, GPIO_DBCTL_DBCLKSEL_1024);
    GPIO_SET_DEBOUNCE_TIME(PC, GPIO_DBCTL_DBCLKSRC_HXT, GPIO_DBCTL_DBCLKSEL_1024);
    GPIO_ENABLE_DEBOUNCE(PA, BIT2);
    GPIO_ENABLE_DEBOUNCE(PC, BIT5);

    /* Waiting for interrupts */
    while(1);
}


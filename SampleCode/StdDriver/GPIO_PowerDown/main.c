/**************************************************************************//**
 * @file     main.c
 * @brief    Show how to wake up system from Power-down mode by GPIO interrupt.
 *
 * @copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/*  Function for System Entry to Power Down Mode                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void PowerDownFunction(void)
{
    /* Check if all the debug messages are finished */
    UART_WAIT_TX_EMPTY(UART0);

    /* Enter to Power-down mode */
    SYS_NPD();
}

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
    /* To check if PA.3 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PA, BIT3))
    {
        GPIO_CLR_INT_FLAG(PA, BIT3);
        sysprintf("PA.3 INT occurred.\n");
    }
    else
    {
        /* Un-expected interrupt. Just clear all PA interrupts */
        GPIO_CLR_INT_FLAG(PA, PA->INTSRC);
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

    /* Set multi-function pins for UART */
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
    sysprintf("+-------------------------------------------------------+\n");
    sysprintf("|    GPIO Power-Down and Wake-up by PA.3 Sample Code    |\n");
    sysprintf("+-------------------------------------------------------+\n\n");

    /* Configure PA.3 as Input mode and enable interrupt by rising edge trigger */
    GPIO_SetMode(PA, BIT3, GPIO_MODE_INPUT);
    GPIO_EnableInt(PA, 3, GPIO_INT_RISING);
    IRQ_SetHandler((IRQn_ID_t)GPA_IRQn, GPA_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)GPA_IRQn);

    /* Enable interrupt de-bounce function and select de-bounce sampling cycle time is 1024 clocks of LIRC clock */
    GPIO_SET_DEBOUNCE_TIME(PA, GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_1024);
    GPIO_ENABLE_DEBOUNCE(PA, BIT3);

    /* Unlock protected registers before entering Power-down mode */
    SYS_UnlockReg();

    /* Waiting for PA.3 rising-edge interrupt event */
    while(1)
    {
        sysprintf("Enter to Power-Down ......\n");

        /* Enter to Power-down mode */
        PowerDownFunction();

        sysprintf("System waken-up done.\n\n");
    }

}


/**************************************************************************//**
 * @file     main.c
 * @brief    Show the usage of GPIO external interrupt function and de-bounce function.
 *
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/**
 * @brief       External INT0 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The External INT0 default IRQ.
 */
void EINT0_IRQHandler(void)
{
    sysprintf("EINT0_IRQHandler..........\n");
    /* To check if PA.6 external interrupt occurred */
    if(GPIO_GET_INT_FLAG(PG, BIT0))
    {
        GPIO_CLR_INT_FLAG(PG, BIT0);
        sysprintf("PG.0 EINT0 occurred.\n");
    }

    /* To check if PI.4 external interrupt occurred */
    if(GPIO_GET_INT_FLAG(PI, BIT4))
    {
        GPIO_CLR_INT_FLAG(PI, BIT4);
        sysprintf("PI.4 EINT0 occurred.\n");
    }

}

/**
 * @brief       External INT1 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The External INT1 default IRQ.
 */
void EINT1_IRQHandler(void)
{

    /* To check if PB.10 external interrupt occurred */
    if(GPIO_GET_INT_FLAG(PB, BIT10))
    {
        GPIO_CLR_INT_FLAG(PB, BIT10);
        sysprintf("PB.10 EINT1 occurred.\n");
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
    
    /* Set multi-function pins for EINT0/EINT1 */
    SYS->GPG_MFPL &= ~(SYS_GPG_MFPL_PG0MFP_Msk);
    SYS->GPI_MFPL &= ~(SYS_GPI_MFPL_PI4MFP_Msk);
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB10MFP_Msk);
    SYS->GPG_MFPL |= SYS_GPG_MFPL_PG0MFP_INT0;
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI4MFP_INT0;
    SYS->GPB_MFPH |= SYS_GPB_MFPH_PB10MFP_INT1;
    
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
    sysprintf("+------------------------------------------------------------+\n");
    sysprintf("|    GPIO EINT0/EINT1 Interrupt and De-bounce Sample Code    |\n");
    sysprintf("+------------------------------------------------------------+\n\n");

    /*-----------------------------------------------------------------------------------------------------*/
    /* GPIO External Interrupt Function Test                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    sysprintf("EINT0(PG.0 and PI.4) and EINT1(PB.10) are used to test interrupt\n");

    /* Configure PG.0 as EINT0 pin and enable interrupt by falling edge trigger */
    GPIO_SetMode(PG, BIT0, GPIO_MODE_INPUT);
    GPIO_EnableInt(PG, 0, GPIO_INT_FALLING);

    /* Configure PI.4 as EINT0 pin and enable interrupt by rising edge trigger */
    GPIO_SetMode(PI, BIT4, GPIO_MODE_INPUT);
    GPIO_EnableInt(PI, 4, GPIO_INT_RISING);
    IRQ_SetHandler((IRQn_ID_t)EINT0_IRQn, EINT0_IRQHandler);
	IRQ_Enable ((IRQn_ID_t)EINT0_IRQn);

    /* Configure PB.10 as EINT1 pin and enable interrupt by falling and rising edge trigger */
    GPIO_SetMode(PB, BIT10, GPIO_MODE_INPUT);
    GPIO_EnableInt(PB, 10, GPIO_INT_BOTH_EDGE);
    IRQ_SetHandler((IRQn_ID_t)EINT1_IRQn, EINT1_IRQHandler);
	IRQ_Enable ((IRQn_ID_t)EINT1_IRQn);

    /* Enable interrupt de-bounce function and select de-bounce sampling cycle time is 1024 clocks of HXT clock */
    GPIO_SET_DEBOUNCE_TIME(PB, GPIO_DBCTL_DBCLKSRC_HXT, GPIO_DBCTL_DBCLKSEL_1024);
    GPIO_SET_DEBOUNCE_TIME(PG, GPIO_DBCTL_DBCLKSRC_HXT, GPIO_DBCTL_DBCLKSEL_1024);
    GPIO_SET_DEBOUNCE_TIME(PI, GPIO_DBCTL_DBCLKSRC_HXT, GPIO_DBCTL_DBCLKSEL_1024);
    GPIO_ENABLE_DEBOUNCE(PG, BIT0);
    GPIO_ENABLE_DEBOUNCE(PI, BIT4);
    GPIO_ENABLE_DEBOUNCE(PB, BIT10);

    /* Waiting for interrupts */
    while(1);
}

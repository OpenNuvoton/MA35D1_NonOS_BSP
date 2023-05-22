/**************************************************************************//**
 * @file     main.c
 *
 * @brief
 *           Show the usage of QEI compare function.
 *
 * @copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#define QEI0A   PA0
#define QEI0B   PA1

/* Delay execution for given amount of ticks */
void Delay(uint32_t ticks)  {
	uint32_t tgtTicks = msTicks0 + ticks;             // target tick count to delay
	while (msTicks0 < tgtTicks);
}

void QEI0_IRQHandler(void)
{
    if(QEI_GET_INT_FLAG(QEI0, QEI_STATUS_CMPF_Msk))     /* Compare-match flag */
    {
        sysprintf("Compare-match INT!\n\n");
        QEI_CLR_INT_FLAG(QEI0, QEI_STATUS_CMPF_Msk);
    }

    if(QEI_GET_INT_FLAG(QEI0, QEI_STATUS_OVUNF_Msk))    /* Counter Overflow or underflow flag */
    {
        sysprintf("Overflow INT!\n\n");
        QEI_CLR_INT_FLAG(QEI0, QEI_STATUS_OVUNF_Msk);
    }
}

void SYS_Init(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable IP clock */
    CLK_EnableModuleClock(GPA_MODULE);
    CLK_EnableModuleClock(QEI0_MODULE);
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART clock source from HXT */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Set multi-function pins */
    SYS->GPE_MFPL &= ~(SYS_GPE_MFPH_PE15MFP_Msk | SYS_GPE_MFPH_PE14MFP_Msk);
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE15MFP_UART0_RXD | SYS_GPE_MFPH_PE14MFP_UART0_TXD);
    SYS->GPG_MFPL &= ~(SYS_GPG_MFPL_PG0MFP_Msk | SYS_GPG_MFPL_PG1MFP_Msk | SYS_GPG_MFPL_PG2MFP_Msk);
    SYS->GPG_MFPL |= (SYS_GPG_MFPL_PG0MFP_QEI0_INDEX | SYS_GPG_MFPL_PG1MFP_QEI0_B | SYS_GPG_MFPL_PG2MFP_QEI0_A);

    /* Lock protected registers */
    SYS_LockReg();
}

void UART0_Init()
{
    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);
}

int32_t main(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for sysprintf */
    UART0_Init();

    /* Init global timer */
    global_timer_init();

    sysprintf("\n\nCPU @ %dHz\n", SystemCoreClock);

    sysprintf("+--------------------------------------+\n");
    sysprintf("|          QEI Driver Sample Code      |\n");
    sysprintf("+--------------------------------------+\n\n");
    sysprintf("  >> Please connect PA.0 and PG.2 << \n");
    sysprintf("  >> Please connect PA.1 and PG.1 << \n");
    sysprintf("     Press any key to start test\n\n");
    sysgetchar();

    /* Configure PA.0 and PA.1 as output mode */
    GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PA, BIT1, GPIO_MODE_OUTPUT);

    QEI0A = 0;
    QEI0B = 0;

    /* Set QEI counting mode as X4 Compare-counting mode,
       set maximum counter value and enable IDX, QEA and QEB input */
    QEI_Open(QEI0, QEI_CTL_X4_COMPARE_COUNTING_MODE, 0x20000);

    /* Set counter compare value */
    QEI_SET_CNT_CMP(QEI0, 0x10000);

    /* Enable compare function */
    QEI_ENABLE_CNT_CMP(QEI0);

    /* Enable QEI interrupt */
    QEI_EnableInt(QEI0, QEI_CTL_CMPIEN_Msk | QEI_CTL_OVUNIEN_Msk);

    /* Set QEI interrupt callback function */
    IRQ_SetHandler((IRQn_ID_t)QEI0_IRQn, QEI0_IRQHandler);

    /* Start QEI function */
    QEI_Start(QEI0);

    /* Wait compare-match and overflow interrupt happened */
    while(1)
    {
        QEI0A = 1;
        QEI0B = 1;
        QEI0A = 0;
        QEI0B = 0;
    }

}

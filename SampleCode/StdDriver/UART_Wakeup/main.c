/**************************************************************************//**
 * @file     main.c
 *
 * @brief
 *           Show how to wake up system from Power-down mode by UART interrupt.
 *
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile int32_t g_bWait         = TRUE;

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void UART1_TEST_HANDLE(void);
void UART_FunctionTest(void);


void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HXT */
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;

    /* Waiting clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Enable IP clock */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(2));
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL2_UART1SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART1(1));
    CLK_EnableModuleClock(UART1_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);
    /* Set multi-function pins for UART1 */
    SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA2MFP_Msk | SYS_GPA_MFPL_PA3MFP_Msk |
                       SYS_GPA_MFPL_PA0MFP_Msk | SYS_GPA_MFPL_PA1MFP_Msk);
    SYS->GPA_MFPL |= SYS_GPA_MFPL_PA2MFP_UART1_RXD | SYS_GPA_MFPL_PA3MFP_UART1_TXD |
                     SYS_GPA_MFPL_PA0MFP_UART1_nCTS | SYS_GPA_MFPL_PA1MFP_UART1_nRTS;

    /* Lock protected registers */
    SYS_LockReg();
}

void UART_Init()
{
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

int main1(void)
{
    __WFI();
}

int32_t main(void)
{
    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART for sysprintf and test */
    UART_Init();

    sysprintf("\n\nCPU @ %d Hz\n", SystemCoreClock);

    sysprintf("\nUART CTS Wake-Up Sample Program\n");

    /* UART sample function */
    UART_FunctionTest();

    while(1);

}

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void UART1_IRQHandler(void)
{
    UART1_TEST_HANDLE();
}

/*---------------------------------------------------------------------------------------------------------*/
/* UART Callback function                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void UART1_TEST_HANDLE(void)
{
    uint32_t u32IntSts = UART1->INTSTS;

   	if(u32IntSts & (UART_INTSTS_WKIF_Msk))
    {
        sysprintf("\n UART Wake-Up ");

        g_bWait = 1;
        UART1->INTSTS = UART_INTSTS_WKIF_Msk;
        UART1->WKSTS = UART1->WKSTS;
    }
}



/*---------------------------------------------------------------------------------------------------------*/
/*  UART Function Test                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void UART_FunctionTest()
{
    SYS_UnlockReg();

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART1, 115200);

    g_bWait = 0;

    UART1->WKCTL = UART_WKCTL_WKCTSEN_Msk;

    /* Enable UART RDA/THRE/Time-out interrupt */
    IRQ_SetHandler((IRQn_ID_t)UART1_IRQn, UART1_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)UART1_IRQn);
    
    UART_EnableInt(UART1, (UART_INTEN_WKIEN_Msk));

    UART1->WKSTS = UART1->WKSTS; // clecar status

    /* Wait debug message finish */
    UART_WAIT_TX_EMPTY(UART0);

    SYS_NPD();

    while(!g_bWait);

    /* Disable UART RDA/THRE/Time-out interrupt */
    UART_DisableInt(UART1, UART_INTEN_WKIEN_Msk);
    g_bWait = TRUE;
    sysprintf("\n\nUART Sample Demo End.\n");

}





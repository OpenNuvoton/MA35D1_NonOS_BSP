/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate Wormhole Controller (WHC) transmit and receive function.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t g_au32RxBuf[WHC_BUFFER_LEN];
uint32_t g_au32TxBuf[WHC_BUFFER_LEN];

/**
 * @brief       IRQ Handler for WHC0 Interrupt
 *
 * @param       None
 *
 * @details     The WHC0_IRQHandler is default IRQ of WHC0.
 */
void WHC0_IRQHandler(void)
{
    /* Clear WHC RX message channel interrupt flag */
    WHC_CLR_INT_FLAG(WHC0, WHC_INTSTS_RX0IF_Msk);

    /* Receive message from a WHC RX message channel */
    WHC_Recv(WHC0, 0, g_au32RxBuf);

    sysprintf("Received message: %08x, %08x, %08x, %08x\n", g_au32RxBuf[0], g_au32RxBuf[1], g_au32RxBuf[2], g_au32RxBuf[3]);
}

void SYS_Init(void)
{
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable WHC module clock */
    CLK_EnableModuleClock(WH0_MODULE);

    /* Select UART module clock source as HXT */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* To update the variable SystemCoreClock */
    SystemCoreClockUpdate();

	/* Set GPE multi-function pins for UART RXD and TXD */
	SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE15MFP_Msk | SYS_GPE_MFPH_PE14MFP_Msk);
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE15MFP_UART0_RXD | SYS_GPE_MFPH_PE14MFP_UART0_TXD);
}

int32_t main (void)
{
    uint32_t i = 0;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    sysprintf("\nThis sample code demonstrate Wormhole Controller transmit and receive function\n");

    /* Enable WHC GIC */
    IRQ_SetHandler((IRQn_ID_t)WRHO0_IRQn, WHC0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)WRHO0_IRQn);

    /* Enable WHC RX message channel interrupt */
    WHC_ENABLE_INT(WHC0, WHC_INTEN_RX0IEN_Msk);

    while(1)
    {
    	/* Check if WHC TX message channel is available or not */
        if(WHC_IS_TX_READY(WHC0, 0))
        {
        	g_au32TxBuf[0] = i++;
        	g_au32TxBuf[1] = i++;
        	g_au32TxBuf[2] = i++;
        	g_au32TxBuf[3] = i++;

        	/* Send message through a WHC TX message channel */
            WHC_Send(WHC0, 0, g_au32TxBuf);
        }
    }
}


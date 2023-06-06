/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate smartcard UART mode by connecting PK.12 and PK.13 pins.
 *
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

// This is the string we used in loopback demo
uint8_t au8TxBuf[] = "Hello World!";


/**
  * @brief  The interrupt services routine of smartcard port 0
  * @param  None
  * @retval None
  */
void SC0_IRQHandler(void)
{
    // Print SCUART received data to UART port
    // Data length here is short, so we're not care about UART FIFO over flow.
    while(!SCUART_GET_RX_EMPTY(SC0)) {
        sysputchar(SCUART_READ(SC0));
        fflush(stdout);
    }
    // RDA is the only interrupt enabled in this sample, this status bit
    // automatically cleared after Rx FIFO empty. So no need to clear interrupt
    // status here.

    return;
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable IP clock */
    CLK_EnableModuleClock(SC0_MODULE);
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(SC0_MODULE, CLK_CLKSEL4_SC0SEL_HXT, CLK_CLKDIV1_SC0(1));
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Set PK.12 and PK.13 pin for SC UART mode */
    /* Smartcard CLK pin is used for TX, and DAT pin is used for Rx */
    SYS->GPK_MFPH |= (SYS_GPK_MFPH_PK12MFP_SC0_CLK | SYS_GPK_MFPH_PK13MFP_SC0_DAT);

    /* Set multi-function pins for UART */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();
    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);
    sysprintf("This sample code demos smartcard interface UART mode\n");
    sysprintf("Please connect SC0 CLK pin(PK.12) with SC0 DAT pin(PK.13)\n");
    sysprintf("Hit any key to continue\n");
    sysgetchar();

    // Open smartcard interface 0 in UART mode. The line config will be 115200-8n1
    // Can call SCUART_SetLineConfig() later if necessary
    SCUART_Open(SC0, 115200);

    // Enable receive interrupt, no need to use other interrupts in this demo
    SCUART_ENABLE_INT(SC0, SC_INTEN_RDAIEN_Msk);
    IRQ_SetHandler((IRQn_ID_t)SC0_IRQn, SC0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)SC0_IRQn);

    // Send the demo string out from SC0_CLK pin
    // Received data from SC0_DAT pin will be print out to UART console
    SCUART_Write(SC0, au8TxBuf, sizeof(au8TxBuf));

    // Loop forever. There's no where to go without an operating system.
    while(1);
}





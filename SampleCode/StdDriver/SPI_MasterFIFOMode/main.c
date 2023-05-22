/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Configure SPI0 as Master mode and demonstrate how to communicate
 *           with an off-chip SPI Slave device with FIFO mode. This sample
 *           code needs to work with SPI_SlaveFifoMode sample code.
 *
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#define TEST_COUNT  16

uint32_t g_au32SourceData[TEST_COUNT];
uint32_t g_au32DestinationData[TEST_COUNT];
volatile uint32_t g_u32TxDataCount;
volatile uint32_t g_u32RxDataCount;

void SYS_Init(void)
{
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

    /* Select clock source of SPI0 */
    CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL4_SPI0SEL_PCLK1, MODULE_NoMsk);

    /* Enable SPI0 peripheral clock */
    CLK_EnableModuleClock(SPI0_MODULE);

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Setup SPI0 multi-function pins */
    SYS->GPL_MFPH &= ~(SYS_GPL_MFPH_PL12MFP_Msk | SYS_GPL_MFPH_PL13MFP_Msk | SYS_GPL_MFPH_PL14MFP_Msk | SYS_GPL_MFPH_PL15MFP_Msk);
    SYS->GPL_MFPH |= SYS_GPL_MFPH_PL12MFP_SPI0_SS0 | SYS_GPL_MFPH_PL13MFP_SPI0_CLK | SYS_GPL_MFPH_PL14MFP_SPI0_MOSI | SYS_GPL_MFPH_PL15MFP_SPI0_MISO;
}

void SPI_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init SPI                                                                                                */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Configure as a master, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Set IP clock divider. SPI clock rate = 2MHz */
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 32, 2000000);

    /* Enable the automatic hardware slave select function. Select the SS pin and configure as low-active. */
    SPI_EnableAutoSS(SPI0, SPI_SS, SPI_SS_ACTIVE_LOW);
}

void SPI0_IRQHandler(void)
{
    /* Check RX EMPTY flag */
    while(SPI_GET_RX_FIFO_EMPTY_FLAG(SPI0) == 0)
    {
        /* Read RX FIFO */
        g_au32DestinationData[g_u32RxDataCount++] = SPI_READ_RX(SPI0);
    }
    /* Check TX FULL flag and TX data count */
    while((SPI_GET_TX_FIFO_FULL_FLAG(SPI0) == 0) && (g_u32TxDataCount < TEST_COUNT))
    {
        /* Write to TX FIFO */
        SPI_WRITE_TX(SPI0, g_au32SourceData[g_u32TxDataCount++]);
    }
    if(g_u32TxDataCount >= TEST_COUNT)
        SPI_DisableInt(SPI0, SPI_FIFO_TXTH_INT_MASK); /* Disable TX FIFO threshold interrupt */

    /* Check the RX FIFO time-out interrupt flag */
    if(SPI_GetIntFlag(SPI0, SPI_FIFO_RXTO_INT_MASK))
    {
        /* If RX FIFO is not empty, read RX FIFO. */
        while((SPI0->STATUS & SPI_STATUS_RXEMPTY_Msk) == 0)
            g_au32DestinationData[g_u32RxDataCount++] = SPI_READ_RX(SPI0);
    }
}


int main(void)
{
    uint32_t u32DataCount;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();

    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UART_Open(UART0, 115200);

    /* Init SPI */
    SPI_Init();

    sysprintf("\n\n");
    sysprintf("+----------------------------------------------------------------------+\n");
    sysprintf("|             SPI Master Mode Sample Code                              |\n");
    sysprintf("+----------------------------------------------------------------------+\n");
    sysprintf("\n");
    sysprintf("Configure SPI0 as a master.\n");
    sysprintf("Bit length of a transaction: 32\n");
    sysprintf("The I/O connection for SPI0:\n");
    sysprintf("    SPI0_SS(PL12)\n    SPI0_CLK(PL13)\n");
    sysprintf("    SPI0_MISO(PL15)\n    SPI0_MOSI(PL14)\n\n");
    sysprintf("SPI controller will enable FIFO mode and transfer %d data to a off-chip slave device.\n", TEST_COUNT);
    sysprintf("In the meanwhile the SPI controller will receive %d data from the off-chip slave device.\n", TEST_COUNT);
    sysprintf("After the transfer is done, the %d received data will be printed out.\n", TEST_COUNT);
    sysprintf("The SPI master configuration is ready.\n");

    for(u32DataCount = 0; u32DataCount < TEST_COUNT; u32DataCount++)
    {
        /* Write the initial value to source buffer */
        g_au32SourceData[u32DataCount] = 0x00550000 + u32DataCount;
        /* Clear destination buffer */
        g_au32DestinationData[u32DataCount] = 0;
    }

    sysprintf("Before starting the data transfer, make sure the slave device is ready. Press any key to start the transfer.\n");
    sysgetchar();
    sysprintf("\n");

    /* Set TX FIFO threshold, enable TX FIFO threshold interrupt and RX FIFO time-out interrupt */
    SPI_SetFIFO(SPI0, 2, 2);
    SPI_EnableInt(SPI0, SPI_FIFO_TXTH_INT_MASK | SPI_FIFO_RXTO_INT_MASK);

    g_u32TxDataCount = 0;
    g_u32RxDataCount = 0;
    IRQ_SetHandler((IRQn_ID_t)SPI0_IRQn, SPI0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)SPI0_IRQn);

    /* Wait for transfer done */
    while(g_u32RxDataCount < TEST_COUNT);

    /* Print the received data */
    sysprintf("Received data:\n");
    for(u32DataCount = 0; u32DataCount < TEST_COUNT; u32DataCount++)
    {
        sysprintf("%d:\t0x%X\n", u32DataCount, g_au32DestinationData[u32DataCount]);
    }
    /* Disable TX FIFO threshold interrupt and RX FIFO time-out interrupt */
    SPI_DisableInt(SPI0, SPI_FIFO_TXTH_INT_MASK | SPI_FIFOCTL_RXTOIEN_Msk);
    IRQ_Disable(SPI0_IRQn);
    sysprintf("The data transfer was done.\n");

    sysprintf("\n\nExit SPI driver sample code.\n");

    /* Reset SPI0 */
    SPI_Close(SPI0);
    while(1);
}


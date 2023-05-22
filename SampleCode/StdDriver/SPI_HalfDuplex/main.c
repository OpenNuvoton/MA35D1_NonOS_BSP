/**************************************************************************//**
 * @file     main.c
 *
 * @brief
 *           Demonstrate SPI half-duplex mode.
 *           SPI0 will be configured as Master mode and SPI1 will be configured as Slave mode.
 *           Both SPI0 and SPI1 will be configured as half-duplex mode.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#define TEST_COUNT  4

uint32_t g_au32DestinationData[TEST_COUNT];
volatile uint32_t g_u32RxDataCount;

/* Function prototype declaration */
void SYS_Init(void);
void SPI_Init(void);

void SYS_Init(void)
{
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

    /* Select clock source of SPI0 and SPI1 */
    CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL4_SPI0SEL_PCLK1, MODULE_NoMsk);
    CLK_SetModuleClock(SPI1_MODULE, CLK_CLKSEL4_SPI1SEL_PCLK2, MODULE_NoMsk);

    /* Enable SPI0 peripheral clock */
    CLK_EnableModuleClock(SPI0_MODULE);
    /* Enable SPI1 peripheral clock */
    CLK_EnableModuleClock(SPI1_MODULE);

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Setup SPI0 multi-function pins */
    SYS->GPL_MFPH &= ~(SYS_GPL_MFPH_PL12MFP_Msk | SYS_GPL_MFPH_PL13MFP_Msk | SYS_GPL_MFPH_PL14MFP_Msk);
    SYS->GPL_MFPH |= SYS_GPL_MFPH_PL12MFP_SPI0_SS0 | SYS_GPL_MFPH_PL13MFP_SPI0_CLK | SYS_GPL_MFPH_PL14MFP_SPI0_MOSI;

    /* Configure SPI1 related multi-function pins */
    SYS->GPC_MFPH &= ~(SYS_GPC_MFPH_PC8MFP_Msk | SYS_GPC_MFPH_PC9MFP_Msk | SYS_GPC_MFPH_PC10MFP_Msk);
    SYS->GPC_MFPH |= (SYS_GPC_MFPH_PC8MFP_SPI1_SS0 | SYS_GPC_MFPH_PC9MFP_SPI1_CLK | SYS_GPC_MFPH_PC10MFP_SPI1_MOSI);
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

    /* Configure SPI1 */
    /* Configure SPI1 as a slave, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Configure SPI1 as a low level active device. SPI peripheral clock rate = f_PCLK0 */
    SPI_Open(SPI1, SPI_SLAVE, SPI_MODE_0, 32, ptr_to_u32(NULL));
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
    sysprintf("|             SPI Half-duplex Mode Sample Code                         |\n");
    sysprintf("+----------------------------------------------------------------------+\n");
    sysprintf("\n");
    sysprintf("Configure SPI0 as a master and SPI1 as a slave.\n");
    sysprintf("Set both SPI0 and SPI1 to half-duplex.\n");
    sysprintf("Bit length of a transaction: 32\n");
    sysprintf("Please connect below I/O connections for SPI0 and SPI1:\n");
    sysprintf("    SPI0_SS(PL12)   <->   SPI1_SS(PC8)\n");
    sysprintf("    SPI0_CLK(PL13)  <->   SPI1_CLK(PC9)\n");
    sysprintf("    SPI0_MOSI(PL14) <->   SPI1_MOSI(PC10)\n\n");
    sysprintf("After the transfer is done, the received data will be printed out.\n");


    /* Set slave SPI1 to half-duplex mode */
    SPI1->CTL |= SPI_CTL_HALFDPX_Msk;
    /* Enable half-duplex will produce TXFBCLR (SPIx_FIFOCTL[9]) and RXFBCLR (SPIx_FIFOCTL[8])*/
    while (SPI1->STATUS & SPI_STATUS_TXRXRST_Msk) {}
    /* Set slave SPI1 data direction to output */
    SPI1->CTL |= SPI_CTL_DATDIR_Msk;

    /* Slave SPI1 prepare data to TX FIFO */
    SPI_WRITE_TX(SPI1, 0x55AA0000);
    SPI_WRITE_TX(SPI1, 0x55AA0001);
    SPI_WRITE_TX(SPI1, 0x55AA0002);
    SPI_WRITE_TX(SPI1, 0x55AA0003);

    /* Set master SPI0 to half-duplex mode */
    SPI0->CTL |= SPI_CTL_HALFDPX_Msk;
    /* Enable half-duplex will produce TXFBCLR (SPIx_FIFOCTL[9]) and RXFBCLR (SPIx_FIFOCTL[8])*/
    while (SPI0->STATUS & SPI_STATUS_TXRXRST_Msk) {}
    /* Set master SPI0 data direction to input */
    SPI0->CTL &= ~SPI_CTL_DATDIR_Msk;

    /* Master SPI0 receive four data from slave SPI1 */
    for (g_u32RxDataCount=0; g_u32RxDataCount<4; g_u32RxDataCount++)
    {
        /* Master write TX for generating clock */
        SPI_WRITE_TX(SPI0, 0);
        /* Wait for Rx FIFO not empty */
        while (SPI_GET_RX_FIFO_EMPTY_FLAG(SPI0)) {}
        /* Read data from RX register */
        g_au32DestinationData[g_u32RxDataCount] = SPI_READ_RX(SPI0);
    }

    /* Print the received data */
    sysprintf("Received data:\n");
    for(u32DataCount = 0; u32DataCount < TEST_COUNT; u32DataCount++)
    {
        sysprintf("%d:\t0x%X\n", u32DataCount, g_au32DestinationData[u32DataCount]);
    }
    sysprintf("The data transfer was done.\n");

    sysprintf("\n\nExit SPI driver sample code.\n");

    /* Reset SPI0 */
    SPI_Close(SPI0);
    /* Reset SPI1 */
    SPI_Close(SPI1);

    while(1);
}



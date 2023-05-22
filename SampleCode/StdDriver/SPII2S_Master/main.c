/**************************************************************************//**
 * @file     main.c
 *
 * @brief
 *           Configure SPI1 as I2S Master mode and demonstrate how I2S works in Master mode.
 *           This sample code needs to work with SPII2S_Slave.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

volatile uint32_t g_u32TxValue;
volatile uint32_t g_u32DataCount;

/* Function prototype declaration */
void SYS_Init(void);

void SPI1_IRQHandler(void);

int32_t main(void)
{
    uint32_t u32RxValue1, u32RxValue2;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 to 115200-8n1 for printing messages */
    UART_Open(UART0, 115200);

    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("|            SPII2S Driver Sample Code (master mode)        |\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("  I2S configuration:\n");
    sysprintf("      Sample rate 16 kHz\n");
    sysprintf("      Word width 16 bits\n");
    sysprintf("      Stereo mode\n");
    sysprintf("      I2S format\n");
    sysprintf("      TX value: 0x55005501, 0x55025503, ..., 0x55FE55FF, wraparound\n");
    sysprintf("  The I/O connection for I2S1 (SPI1):\n");
    sysprintf("      I2S1_LRCLK (PC8)\n      I2S1_BCLK(PC9)\n");
    sysprintf("      I2S1_DI (PC11)\n      I2S1_DO (PC10)\n\n");
    sysprintf("  NOTE: Connect with a I2S slave device.\n");
    sysprintf("        This sample code will transmit a TX value 50000 times, and then change to the next TX value.\n");
    sysprintf("        When TX value or the received value changes, the new TX value or the current TX value and the new received value will be printed.\n");
    sysprintf("  Press any key to start ...");
    sysgetchar();
    sysprintf("\n");

    /* Master mode, 16-bit word width, stereo mode, I2S format. Set TX FIFO threshold to 2 and RX FIFO threshold to 1. */
    SPII2S_Open(SPI1, I2S_MODE_MASTER, 16000, I2S_DATABIT_16, SPII2S_STEREO, I2S_FORMAT_I2S);

    /* Initiate data counter */
    g_u32DataCount = 0;
    /* Initiate TX value and RX value */
    g_u32TxValue = 0x55005501;
    u32RxValue1 = 0;
    u32RxValue2 = 0;
    /* Enable TX threshold level interrupt */
    SPII2S_EnableInt(SPI1, SPII2S_FIFO_TXTH_INT_MASK);
    IRQ_SetHandler((IRQn_ID_t)SPI1_IRQn, SPI1_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)SPI1_IRQn);

    sysprintf("Start I2S ...\nTX value: 0x%X\n", g_u32TxValue);

    while(1)
    {
        /* Check RX FIFO empty flag */
        if((SPI1->I2SSTS & SPI_I2SSTS_RXEMPTY_Msk) == 0)
        {
            /* Read RX FIFO */
            u32RxValue2 = SPII2S_READ_RX_FIFO(SPI1);
            if(u32RxValue1 != u32RxValue2)
            {
                u32RxValue1 = u32RxValue2;
                /* If received value changes, print the current TX value and the new received value. */
                sysprintf("TX value: 0x%X;  RX value: 0x%X\n", g_u32TxValue, u32RxValue1);
            }
        }
        if(g_u32DataCount >= 50000)
        {
            g_u32TxValue = 0x55005500 | ((g_u32TxValue + 0x00020002) & 0x00FF00FF); /* g_u32TxValue: 0x55005501, 0x55025503, ..., 0x55FE55FF */
            sysprintf("TX value: 0x%X\n", g_u32TxValue);
            g_u32DataCount = 0;
        }
    }
}

void SYS_Init(void)
{
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

    /* Enable peripheral clock */
    CLK_EnableModuleClock(SPI1_MODULE);

    /* Select clock source of SPI1 */
    CLK_SetModuleClock(SPI1_MODULE, CLK_CLKSEL4_SPI1SEL_PCLK2, MODULE_NoMsk);

    CLK_EnableModuleClock(PDMA2_MODULE);

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Configure SPI1 related multi-function pins. */
    /* GPC[11:8] : SPI1_CLK (I2S1_BCLK), SPI1_MISO (I2S1_DI), SPI1_MOSI (I2S1_DO), SPI1_SS (I2S1_LRCLK). */
    SYS->GPC_MFPH &= ~(SYS_GPC_MFPH_PC8MFP_Msk | SYS_GPC_MFPH_PC9MFP_Msk | SYS_GPC_MFPH_PC10MFP_Msk | SYS_GPC_MFPH_PC11MFP_Msk);
    SYS->GPC_MFPH |= SYS_GPC_MFPH_PC8MFP_SPI1_SS0 | SYS_GPC_MFPH_PC9MFP_SPI1_CLK | SYS_GPC_MFPH_PC10MFP_SPI1_MOSI | SYS_GPC_MFPH_PC11MFP_SPI1_MISO;
}

void SPI1_IRQHandler()
{
    /* Write 2 TX values to TX FIFO */
    SPII2S_WRITE_TX_FIFO(SPI1, g_u32TxValue);
    SPII2S_WRITE_TX_FIFO(SPI1, g_u32TxValue);
    g_u32DataCount += 2;
}


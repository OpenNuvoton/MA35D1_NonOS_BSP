/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Access SPI flash through SPI interface.
 *
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#define TEST_NUMBER 1   /* page numbers */
#define TEST_LENGTH 256 /* length */

#define SPI_FLASH_PORT  SPI0

uint8_t SrcArray[TEST_LENGTH];
uint8_t DestArray[TEST_LENGTH];

uint16_t SpiFlash_ReadMidDid(void)
{
    uint8_t u8RxData[6], u8IDCnt = 0;

    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x90, Read Manufacturer/Device ID
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x90);

    // send 24-bit '0', dummy
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // receive 16-bit
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);

    while(!SPI_GET_RX_FIFO_EMPTY_FLAG(SPI_FLASH_PORT))
        u8RxData[u8IDCnt ++] = SPI_READ_RX(SPI_FLASH_PORT);

    return ( (u8RxData[4]<<8) | u8RxData[5] );
}

void SpiFlash_ChipErase(void)
{
    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x06, Write enable
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x06);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);

    //////////////////////////////////////////

    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0xC7, Chip Erase
    SPI_WRITE_TX(SPI_FLASH_PORT, 0xC7);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);

    SPI_ClearRxFIFO(SPI_FLASH_PORT);
}

uint8_t SpiFlash_ReadStatusReg(void)
{
    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x05, Read status register
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x05);

    // read status
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);

    // skip first rx data
    SPI_READ_RX(SPI_FLASH_PORT);

    return (SPI_READ_RX(SPI_FLASH_PORT) & 0xff);
}

void SpiFlash_WriteStatusReg(uint8_t u8Value)
{
    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x06, Write enable
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x06);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);

    ///////////////////////////////////////

    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x01, Write status register
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x01);

    // write status
    SPI_WRITE_TX(SPI_FLASH_PORT, u8Value);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);
}

void SpiFlash_WaitReady(void)
{
    uint8_t ReturnValue;

    do
    {
        ReturnValue = SpiFlash_ReadStatusReg();
        ReturnValue = ReturnValue & 1;
    }
    while(ReturnValue!=0);   // check the BUSY bit
}

void SpiFlash_NormalPageProgram(uint32_t StartAddress, uint8_t *u8DataBuffer)
{
    uint32_t i = 0;

    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x06, Write enable
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x06);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);


    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x02, Page program
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x02);

    // send 24-bit start address
    SPI_WRITE_TX(SPI_FLASH_PORT, (StartAddress>>16) & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT, (StartAddress>>8)  & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT, StartAddress       & 0xFF);

    // write data
    while(1)
    {
        if(!SPI_GET_TX_FIFO_FULL_FLAG(SPI_FLASH_PORT))
        {
            SPI_WRITE_TX(SPI_FLASH_PORT, u8DataBuffer[i++]);
            if(i > 255) break;
        }
    }

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);

    SPI_ClearRxFIFO(SPI_FLASH_PORT);
}

void SpiFlash_NormalRead(uint32_t StartAddress, uint8_t *u8DataBuffer)
{
    uint32_t i;

    // /CS: active
    SPI_SET_SS_LOW(SPI_FLASH_PORT);

    // send Command: 0x03, Read data
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x03);

    // send 24-bit start address
    SPI_WRITE_TX(SPI_FLASH_PORT, (StartAddress>>16) & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT, (StartAddress>>8)  & 0xFF);
    SPI_WRITE_TX(SPI_FLASH_PORT, StartAddress       & 0xFF);

    while(SPI_IS_BUSY(SPI_FLASH_PORT));
    // clear RX buffer
    SPI_ClearRxFIFO(SPI_FLASH_PORT);

    // read data
    for(i=0; i<256; i++)
    {
        SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
        while(SPI_IS_BUSY(SPI_FLASH_PORT));
        u8DataBuffer[i] = SPI_READ_RX(SPI_FLASH_PORT);
    }

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);
}

void SYS_Init(void)
{
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

    /* Select PCLK0 as the clock source of SPI0 */
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

/* Main */
int main(void)
{
    uint32_t u32ByteCount, u32FlashAddress, u32PageNumber;
    uint32_t nError = 0;
    uint16_t u16ID;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    /* Configure SPI_FLASH_PORT as a master, MSB first, 8-bit transaction, SPI Mode-0 timing, clock is 20MHz */
    SPI_Open(SPI_FLASH_PORT, SPI_MASTER, SPI_MODE_0, 8, 20000000);

    /* Disable auto SS function, control SS signal manually. */
    SPI_DisableAutoSS(SPI_FLASH_PORT);

    sysprintf("\n+------------------------------------------------------------------------+\n");
    sysprintf("|                      SPI Sample with SPI Flash                         |\n");
    sysprintf("+------------------------------------------------------------------------+\n");

    if((u16ID = SpiFlash_ReadMidDid()) != 0xEF14)
    {
        sysprintf("Wrong ID, 0x%x\n", u16ID);
        while(1);
    }
    else
        sysprintf("Flash found: W25X16 ...\n");

    sysprintf("Erase chip ...");

    /* Erase SPI flash */
    SpiFlash_ChipErase();

    /* Wait ready */
    SpiFlash_WaitReady();

    sysprintf("[OK]\n");

    /* init source data buffer */
    for(u32ByteCount=0; u32ByteCount<TEST_LENGTH; u32ByteCount++)
    {
        SrcArray[u32ByteCount] = u32ByteCount;
    }

    sysprintf("Start to normal write data to Flash ...");
    /* Program SPI flash */
    u32FlashAddress = 0;
    for(u32PageNumber=0; u32PageNumber<TEST_NUMBER; u32PageNumber++)
    {
        /* page program */
        SpiFlash_NormalPageProgram(u32FlashAddress, SrcArray);
        SpiFlash_WaitReady();
        u32FlashAddress += 0x100;
    }

    sysprintf("[OK]\n");

    /* clear destination data buffer */
    for(u32ByteCount=0; u32ByteCount<TEST_LENGTH; u32ByteCount++)
    {
        DestArray[u32ByteCount] = 0;
    }

    sysprintf("Normal Read & Compare ...");

    /* Read SPI flash */
    u32FlashAddress = 0;
    for(u32PageNumber=0; u32PageNumber<TEST_NUMBER; u32PageNumber++)
    {
        /* page read */
        SpiFlash_NormalRead(u32FlashAddress, DestArray);
        u32FlashAddress += 0x100;

        for(u32ByteCount=0; u32ByteCount<TEST_LENGTH; u32ByteCount++)
        {
            if(DestArray[u32ByteCount] != SrcArray[u32ByteCount])
                nError ++;
        }
    }

    if(nError == 0)
        sysprintf("[OK]\n");
    else
        sysprintf("[FAIL]\n");

    while(1);
}

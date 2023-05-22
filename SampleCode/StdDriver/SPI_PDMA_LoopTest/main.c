/**************************************************************************//**
 * @file     main.c
 *
 * @brief
 *           Demonstrate SPI data transfer with PDMA.
 *           SPI3 will be configured as Master mode and SPI0 will be configured as Slave mode.
 *           Both TX PDMA function and RX PDMA function will be enabled.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#define SPI_MASTER_TX_DMA_CH 0
#define SPI_MASTER_RX_DMA_CH 1
#define SPI_SLAVE_TX_DMA_CH  2
#define SPI_SLAVE_RX_DMA_CH  3

#define TEST_COUNT 64

/* Function prototype declaration */
void SYS_Init(void);
void SPI_Init(void);
void SpiLoopTest_WithPDMA(void);

/* Global variable declaration */
uint32_t g_au32MasterToSlaveTestPattern[TEST_COUNT];
uint32_t g_au32SlaveToMasterTestPattern[TEST_COUNT];
uint32_t g_au32MasterRxBuffer[TEST_COUNT];
uint32_t g_au32SlaveRxBuffer[TEST_COUNT];
uint32_t *pau32MasterToSlaveTestPattern;
uint32_t *pau32SlaveToMasterTestPattern;
uint32_t *pau32MasterRxBuffer;
uint32_t *pau32SlaveRxBuffer;

void SYS_Init(void)
{
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

    /* Enable SPI3 peripheral clock */
    CLK_EnableModuleClock(SPI3_MODULE);
    /* Enable SPI0 peripheral clock */
    CLK_EnableModuleClock(SPI0_MODULE);

    /* Select clock source of SPI3 and SPI0 */
    CLK_SetModuleClock(SPI3_MODULE, CLK_CLKSEL4_SPI3SEL_PCLK2, MODULE_NoMsk);
    CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL4_SPI0SEL_PCLK1, MODULE_NoMsk);

    /* Enable PDMA clock source */
    CLK_EnableModuleClock(PDMA2_MODULE);

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Setup SPI0 multi-function pins */
    SYS->GPG_MFPL &= ~(SYS_GPG_MFPL_PG0MFP_Msk);
    SYS->GPG_MFPL |= SYS_GPG_MFPL_PG0MFP_SPI0_SS0;
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB9MFP_Msk);
    SYS->GPB_MFPH |= SYS_GPB_MFPH_PB9MFP_SPI0_CLK;
    SYS->GPL_MFPH &= ~(SYS_GPL_MFPH_PL14MFP_Msk | SYS_GPL_MFPH_PL15MFP_Msk);
    SYS->GPL_MFPH |= SYS_GPL_MFPH_PL14MFP_SPI0_MOSI | SYS_GPL_MFPH_PL15MFP_SPI0_MISO;

    /* Setup SPI3 multi-function pins */
    SYS->GPG_MFPL &= ~(SYS_GPG_MFPL_PG4MFP_Msk | SYS_GPG_MFPL_PG5MFP_Msk | SYS_GPG_MFPL_PG6MFP_Msk | SYS_GPG_MFPL_PG7MFP_Msk);
    SYS->GPG_MFPL |= (SYS_GPG_MFPL_PG4MFP_SPI3_SS0 | SYS_GPG_MFPL_PG5MFP_SPI3_CLK | SYS_GPG_MFPL_PG6MFP_SPI3_MOSI | SYS_GPG_MFPL_PG7MFP_SPI3_MISO);

}

void SPI_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init SPI                                                                                                */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Configure SPI3 */
    /* Configure SPI3 as a master, SPI clock rate 2MHz,
       clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    SPI_Open(SPI3, SPI_MASTER, SPI_MODE_0, 32, 2000000);
    /* Enable the automatic hardware slave selection function. Select the SPI3_SS pin and configure as low-active. */
    SPI_EnableAutoSS(SPI3, SPI_SS, SPI_SS_ACTIVE_LOW);

    /* Configure SPI0 */
    /* Configure SPI0 as a slave, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Configure SPI0 as a low level active device. */
    SPI_Open(SPI0, SPI_SLAVE, SPI_MODE_0, 32, 12000000);
}

void SpiLoopTest_WithPDMA(void)
{
    uint32_t u32DataCount, u32TestCycle;
    uint32_t u32RegValue, u32Abort;
    int32_t i32Err;

    sysprintf("\nSPI3/SPI0 Loop test with PDMA ");

    pau32MasterToSlaveTestPattern = nc_ptr(g_au32MasterToSlaveTestPattern);
    pau32SlaveToMasterTestPattern = nc_ptr(g_au32SlaveToMasterTestPattern);

    /* Source data initiation */
    for(u32DataCount = 0; u32DataCount < TEST_COUNT; u32DataCount++)
    {
        *(pau32MasterToSlaveTestPattern + u32DataCount) = 0x55000000 | (u32DataCount + 1);
        *(pau32SlaveToMasterTestPattern + u32DataCount) = 0xAA000000 | (u32DataCount + 1);
    }

    pau32MasterRxBuffer = nc_ptr(g_au32MasterRxBuffer);
    pau32SlaveRxBuffer = nc_ptr(g_au32SlaveRxBuffer);

    /* Enable PDMA channels */
    PDMA_Open(PDMA2,(1 << SPI_MASTER_TX_DMA_CH) | (1 << SPI_MASTER_RX_DMA_CH) | (1 << SPI_SLAVE_RX_DMA_CH) | (1 << SPI_SLAVE_TX_DMA_CH));

    /*=======================================================================
      SPI master PDMA TX channel configuration:
      -----------------------------------------------------------------------
        Word length = 32 bits
        Transfer Count = TEST_COUNT
        Source = g_au32MasterToSlaveTestPattern
        Source Address = Increasing
        Destination = SPI3->TX
        Destination Address = Fixed
        Burst Type = Single Transfer
    =========================================================================*/
    /* Set transfer width (32 bits) and transfer count */
    PDMA_SetTransferCnt(PDMA2,SPI_MASTER_TX_DMA_CH, PDMA_WIDTH_32, TEST_COUNT);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(PDMA2,SPI_MASTER_TX_DMA_CH, ptr_to_u32(pau32MasterToSlaveTestPattern), PDMA_SAR_INC, ptr_to_u32(&SPI3->TX), PDMA_DAR_FIX);
    /* Set request source; set basic mode. */
    PDMA_SetTransferMode(PDMA2,SPI_MASTER_TX_DMA_CH, PDMA_SPI3_TX, FALSE, 0);
    /* Single request type. SPI only support PDMA single request type. */
    PDMA_SetBurstType(PDMA2,SPI_MASTER_TX_DMA_CH, PDMA_REQ_SINGLE, 0);
    /* Disable table interrupt */
    PDMA2->DSCT[SPI_MASTER_TX_DMA_CH].CTL |= PDMA_DSCT_CTL_TBINTDIS_Msk;

    /*=======================================================================
      SPI master PDMA RX channel configuration:
      -----------------------------------------------------------------------
        Word length = 32 bits
        Transfer Count = TEST_COUNT
        Source = SPI3->RX
        Source Address = Fixed
        Destination = g_au32MasterRxBuffer
        Destination Address = Increasing
        Burst Type = Single Transfer
    =========================================================================*/
    /* Set transfer width (32 bits) and transfer count */
    PDMA_SetTransferCnt(PDMA2,SPI_MASTER_RX_DMA_CH, PDMA_WIDTH_32, TEST_COUNT);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(PDMA2,SPI_MASTER_RX_DMA_CH, ptr_to_u32(&SPI3->RX), PDMA_SAR_FIX, ptr_to_u32(pau32MasterRxBuffer), PDMA_DAR_INC);
    /* Set request source; set basic mode. */
    PDMA_SetTransferMode(PDMA2,SPI_MASTER_RX_DMA_CH, PDMA_SPI3_RX, FALSE, 0);
    /* Single request type. SPI only support PDMA single request type. */
    PDMA_SetBurstType(PDMA2,SPI_MASTER_RX_DMA_CH, PDMA_REQ_SINGLE, 0);
    /* Disable table interrupt */
    PDMA2->DSCT[SPI_MASTER_RX_DMA_CH].CTL |= PDMA_DSCT_CTL_TBINTDIS_Msk;

    /*=======================================================================
      SPI slave PDMA RX channel configuration:
      -----------------------------------------------------------------------
        Word length = 32 bits
        Transfer Count = TEST_COUNT
        Source = SPI0->RX
        Source Address = Fixed
        Destination = g_au32SlaveRxBuffer
        Destination Address = Increasing
        Burst Type = Single Transfer
    =========================================================================*/
    /* Set transfer width (32 bits) and transfer count */
    PDMA_SetTransferCnt(PDMA2,SPI_SLAVE_RX_DMA_CH, PDMA_WIDTH_32, TEST_COUNT);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(PDMA2,SPI_SLAVE_RX_DMA_CH, ptr_to_u32(&SPI0->RX), PDMA_SAR_FIX, ptr_to_u32(pau32SlaveRxBuffer), PDMA_DAR_INC);
    /* Set request source; set basic mode. */
    PDMA_SetTransferMode(PDMA2,SPI_SLAVE_RX_DMA_CH, PDMA_SPI0_RX, FALSE, 0);
    /* Single request type. SPI only support PDMA single request type. */
    PDMA_SetBurstType(PDMA2,SPI_SLAVE_RX_DMA_CH, PDMA_REQ_SINGLE, 0);
    /* Disable table interrupt */
    PDMA2->DSCT[SPI_SLAVE_RX_DMA_CH].CTL |= PDMA_DSCT_CTL_TBINTDIS_Msk;

    /*=======================================================================
      SPI slave PDMA TX channel configuration:
      -----------------------------------------------------------------------
        Word length = 32 bits
        Transfer Count = TEST_COUNT
        Source = g_au32SlaveToMasterTestPattern
        Source Address = Increasing
        Destination = SPI0->TX
        Destination Address = Fixed
        Burst Type = Single Transfer
    =========================================================================*/
    /* Set transfer width (32 bits) and transfer count */
    PDMA_SetTransferCnt(PDMA2,SPI_SLAVE_TX_DMA_CH, PDMA_WIDTH_32, TEST_COUNT);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(PDMA2,SPI_SLAVE_TX_DMA_CH, ptr_to_u32(pau32SlaveToMasterTestPattern), PDMA_SAR_INC, ptr_to_u32(&SPI0->TX), PDMA_DAR_FIX);
    /* Set request source; set basic mode. */
    PDMA_SetTransferMode(PDMA2,SPI_SLAVE_TX_DMA_CH, PDMA_SPI0_TX, FALSE, 0);
    /* Single request type. SPI only support PDMA single request type. */
    PDMA_SetBurstType(PDMA2,SPI_SLAVE_TX_DMA_CH, PDMA_REQ_SINGLE, 0);
    /* Disable table interrupt */
    PDMA2->DSCT[SPI_SLAVE_TX_DMA_CH].CTL |= PDMA_DSCT_CTL_TBINTDIS_Msk;

    /* Enable SPI slave DMA function */
    SPI_TRIGGER_TX_RX_PDMA(SPI0);
    /* Enable SPI master DMA function */
    SPI_TRIGGER_TX_RX_PDMA(SPI3);

    i32Err = 0;
    for(u32TestCycle = 0; u32TestCycle < 10000; u32TestCycle++)
    {
        if((u32TestCycle & 0x1FF) == 0)
            sysputchar('.');

        while(1)
        {
            /* Get interrupt status */
            u32RegValue = PDMA_GET_INT_STATUS(PDMA2);

            /* Check the PDMA transfer done interrupt flag */
            if(u32RegValue & PDMA_INTSTS_TDIF_Msk)
            {

                /* Check the PDMA transfer done flags */
                if((PDMA_GET_TD_STS(PDMA2) & ((1 << SPI_MASTER_TX_DMA_CH) | (1 << SPI_MASTER_RX_DMA_CH) | (1 << SPI_SLAVE_TX_DMA_CH) | (1 << SPI_SLAVE_RX_DMA_CH))) ==
                        ((1 << SPI_MASTER_TX_DMA_CH) | (1 << SPI_MASTER_RX_DMA_CH) | (1 << SPI_SLAVE_TX_DMA_CH) | (1 << SPI_SLAVE_RX_DMA_CH)))
                {

                    /* Clear the PDMA transfer done flags */
                    PDMA_CLR_TD_FLAG(PDMA2,(1 << SPI_MASTER_TX_DMA_CH) | (1 << SPI_MASTER_RX_DMA_CH) | (1 << SPI_SLAVE_TX_DMA_CH) | (1 << SPI_SLAVE_RX_DMA_CH));

                    /* Disable SPI3 master's PDMA transfer function */
                    SPI_DISABLE_TX_RX_PDMA(SPI3);

                    /* Check the transfer data */
                    for(u32DataCount = 0; u32DataCount < TEST_COUNT; u32DataCount++)
                    {
                        if(*(pau32MasterToSlaveTestPattern + u32DataCount) != *(pau32SlaveRxBuffer + u32DataCount))
                        {
                        	sysprintf("[%d] *(pau32MasterToSlaveTestPattern + u32DataCount)[%x], *(pau32SlaveRxBuffer + u32DataCount)[%x]\n", u32DataCount, *(pau32MasterToSlaveTestPattern + u32DataCount), *(pau32SlaveRxBuffer + u32DataCount));
                            i32Err = 1;
                            break;
                        }
                        if(*(pau32SlaveToMasterTestPattern + u32DataCount) != *(pau32MasterRxBuffer + u32DataCount))
                        {
                        	sysprintf("[%d] *(pau32SlaveToMasterTestPattern + u32DataCount)[%x], *(pau32MasterRxBuffer + u32DataCount)[%x]\n", u32DataCount, *(pau32SlaveToMasterTestPattern + u32DataCount), *(pau32MasterRxBuffer + u32DataCount));
                            i32Err = 1;
                            break;
                        }
                    }

                    if(u32TestCycle >= 10000)
                        break;

                    /* Source data initiation */
                    for(u32DataCount = 0; u32DataCount < TEST_COUNT; u32DataCount++)
                    {
                        *(pau32MasterToSlaveTestPattern + u32DataCount) += 1;
                        *(pau32SlaveToMasterTestPattern + u32DataCount) += 1;
                    }
                    /* Re-trigger */
                    /* Slave PDMA TX channel configuration */
                    /* Set transfer width (32 bits) and transfer count */
                    PDMA_SetTransferCnt(PDMA2,SPI_SLAVE_TX_DMA_CH, PDMA_WIDTH_32, TEST_COUNT);
                    /* Set request source; set basic mode. */
                    PDMA_SetTransferMode(PDMA2,SPI_SLAVE_TX_DMA_CH, PDMA_SPI0_TX, FALSE, 0);

                    /* Slave PDMA RX channel configuration */
                    /* Set transfer width (32 bits) and transfer count */
                    PDMA_SetTransferCnt(PDMA2,SPI_SLAVE_RX_DMA_CH, PDMA_WIDTH_32, TEST_COUNT);
                    /* Set request source; set basic mode. */
                    PDMA_SetTransferMode(PDMA2,SPI_SLAVE_RX_DMA_CH, PDMA_SPI0_RX, FALSE, 0);

                    /* Master PDMA TX channel configuration */
                    /* Set transfer width (32 bits) and transfer count */
                    PDMA_SetTransferCnt(PDMA2,SPI_MASTER_TX_DMA_CH, PDMA_WIDTH_32, TEST_COUNT);
                    /* Set request source; set basic mode. */
                    PDMA_SetTransferMode(PDMA2,SPI_MASTER_TX_DMA_CH, PDMA_SPI3_TX, FALSE, 0);

                    /* Master PDMA RX channel configuration */
                    /* Set transfer width (32 bits) and transfer count */
                    PDMA_SetTransferCnt(PDMA2,SPI_MASTER_RX_DMA_CH, PDMA_WIDTH_32, TEST_COUNT);
                    /* Set request source; set basic mode. */
                    PDMA_SetTransferMode(PDMA2,SPI_MASTER_RX_DMA_CH, PDMA_SPI3_RX, FALSE, 0);

                    /* Enable master's DMA transfer function */
                    SPI_TRIGGER_TX_RX_PDMA(SPI3);
                    break;
                }
            }
            /* Check the DMA transfer abort interrupt flag */
            if(u32RegValue & PDMA_INTSTS_ABTIF_Msk)
            {
                /* Get the target abort flag */
                u32Abort = PDMA_GET_ABORT_STS(PDMA2);
                /* Clear the target abort flag */
                PDMA_CLR_ABORT_FLAG(PDMA2,u32Abort);
                i32Err = 1;
                break;
            }
            /* Check the DMA time-out interrupt flag */
            if(u32RegValue & (PDMA_INTSTS_REQTOF0_Msk|PDMA_INTSTS_REQTOF1_Msk))
            {
                /* Clear the time-out flag */
                PDMA2->INTSTS = u32RegValue & (PDMA_INTSTS_REQTOF0_Msk|PDMA_INTSTS_REQTOF1_Msk);
                i32Err = 1;
                break;
            }
        }

        if(i32Err)
            break;
    }

    /* Disable all PDMA channels */
    PDMA_Close(PDMA2);

    if(i32Err)
    {
        sysprintf(" [FAIL]\n");
    }
    else
    {
        sysprintf(" [PASS]\n");
    }

    return;
}

int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();

    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UART_Open(UART0, 115200);

    /* Init SPI */
    SPI_Init();

    sysprintf("\n\n");
    sysprintf("+--------------------------------------------------------------+\n");
    sysprintf("|                  SPI + PDMA Sample Code                      |\n");
    sysprintf("+--------------------------------------------------------------+\n");
    sysprintf("\n");
    sysprintf("Configure SPI3 as a master and SPI0 as a slave.\n");
    sysprintf("Bit length of a transaction: 32\n");
    sysprintf("The I/O connection for SPI3/SPI0 loopback:\n");
    sysprintf("    SPI3_SS  (PG4) <--> SPI0_SS(PG0)\n    SPI3_CLK(PG5)  <--> SPI0_CLK(PB9)\n");
    sysprintf("    SPI3_MISO(PG7) <--> SPI0_MISO(PL15)\n    SPI3_MOSI(PG6) <--> SPI0_MOSI(PL14)\n\n");
    sysprintf("Please connect SPI3 with SPI0, and press any key to start transmission ...");
    sysgetchar();
    sysprintf("\n");

    SpiLoopTest_WithPDMA();

    sysprintf("\n\nExit SPI driver sample code.\n");

    /* Close SPI3 */
    SPI_Close(SPI3);
    /* Close SPI0 */
    SPI_Close(SPI0);
    while(1);
}

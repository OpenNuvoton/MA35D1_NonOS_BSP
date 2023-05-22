/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate Wormhole Controller (WHC) transmit and receive function.
 *           Load executable image into RTP M4 SRAM from Cortex-A35 side.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"


extern uint32_t rtp_file_data, rtp_file_end;

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

void A35_Init(void)
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

void RTP_M4_Init(void)
{
    /* Enable RTP M4 clock */
    CLK_EnableModuleClock(RTPST_MODULE);

    /* Assign UART16 to RTP M4 */
    SSPCC->PSSET8  |= SSPCC_PSSET8_UART16_Msk;
    SSPCC->IOKSSET |= (SSPCC_IOxSSET_PIN2_Msk | SSPCC_IOxSSET_PIN3_Msk);

    /* Assign GPK multi-function pins for UART RXD and TXD */
    SYS->GPK_MFPL &= ~(SYS_GPK_MFPL_PK2MFP_Msk | SYS_GPK_MFPL_PK3MFP_Msk);
    SYS->GPK_MFPL |= (SYS_GPK_MFPL_PK2MFP_UART16_RXD | SYS_GPK_MFPL_PK3MFP_UART16_TXD);
}

void SYS_Init(void)
{
    A35_Init();
    RTP_M4_Init();
}

int32_t main (void)
{
    uint32_t i = 0;
    uint32_t u32ImageSize = 0;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Load executable image to RTP M4 SRAM and disable RTP M4 core reset */
    u32ImageSize = ptr_to_u32(&rtp_file_end) - ptr_to_u32(&rtp_file_data);
    dcache_clean_invalidate_by_mva(&rtp_file_data, u32ImageSize);
    memcpy((void *) 0x24000000, &rtp_file_data, u32ImageSize);
    SYS->IPRST0 &= ~SYS_IPRST0_CM4RST_Msk;  /* Note: This bit is write protected. */

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


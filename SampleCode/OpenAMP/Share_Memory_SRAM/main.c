/*************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate OpenAMP share memory control using SRAM.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NuMicro.h"
#include "mmio.h"
#include "rpmsg.h"
#include "openamp.h"
#include "mbox_whc.h"

#define TX_RX_SIZE      SHM_TX_RX_SIZE

extern vu32 WHC0_RX_Flag;
extern uint32_t rtp_file_data, rtp_file_end;
extern struct rpmsg_virtio_device rvdev;

uint8_t received_rpmsg[TX_RX_SIZE] __attribute__((aligned(32)));
uint8_t transmit_rpmsg[TX_RX_SIZE] __attribute__((aligned(32)));

uint32_t g_au32RxBuf[WHC_BUFFER_LEN];
uint32_t g_au32TxBuf[WHC_BUFFER_LEN];

uint8_t u8RxStart = FALSE;

static uint32_t rx_status = 0;

static int rx_callback(struct rpmsg_endpoint *rp_chnl, void *data, size_t len, uint32_t src, void *priv);
static void rpmsg_list_init(struct metal_list *list);

/*--------------------------------------------------------------------------*/
static void rpmsg_list_init(struct metal_list *list)
{
    list->next = list->prev = list;
}

void UART0_Init()
{
    /* Enable UART0 clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Set multi-function pins */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Initialize UART */
    UART0_Init();

    /* Enable WHC module clock */
    CLK_EnableModuleClock(WH0_MODULE);

    /* Enable RTP M4 clock and DRAM access */
    CLK_EnableModuleClock(RTPST_MODULE);
    SYS->MISCFCR0 |= 0x4;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Assign UART16 to RTP M4 */
    SSPCC->PSSET8  |= SSPCC_PSSET8_UART16_Msk;
    SSPCC->IOKSSET |= (SSPCC_IOxSSET_PIN2_Msk | SSPCC_IOxSSET_PIN3_Msk);

    /* Assign GPK multi-function pins for UART RXD and TXD */
    SYS->GPK_MFPL &= ~(SYS_GPK_MFPL_PK2MFP_Msk | SYS_GPK_MFPL_PK3MFP_Msk);
    SYS->GPK_MFPL |= (SYS_GPK_MFPL_PK2MFP_UART16_RXD | SYS_GPK_MFPL_PK3MFP_UART16_TXD);

    /* Lock protected registers */
    SYS_LockReg();
}

static int rx_callback(struct rpmsg_endpoint *rp_chnl, void *data, size_t len, uint32_t src, void *priv)
{
    uint32_t *u32Command = (uint32_t *)data;
    uint32_t i;

    if (*u32Command == COMMAND_RECEIVE_M4_MSG)
    {
        if (len > sizeof(received_rpmsg))
            len = sizeof(received_rpmsg);

        sysprintf("# Receive %d bytes data from RTP: \n", len);

        for (i = 0; i < len; i++)
        {
            received_rpmsg[i] = *((uint8_t *) (u64)src + i);
            sysprintf("0x%x\n", received_rpmsg[i]);
        }

        rx_status = 1;
    }
    else
    {
        sysprintf("\n Unknown command!! \n");
    }

    return 0;
}

int main()
{
    struct rpmsg_endpoint resmgr_ept;
    uint32_t i, u32ImageSize;

    global_timer_init();

    /* Initialize System, IP clock and multi-function I/O */
    SYS_Init();

    /* Initialize UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    SYS_UnlockReg();

    /* Load executable image to RTP M4 SRAM and disable RTP M4 core reset */
    u32ImageSize = ptr_to_u32(&rtp_file_end) - ptr_to_u32(&rtp_file_data);
    for (i = 0; i < u32ImageSize; i++)
    {
        *((uint8_t *) 0x24000000 + i) = *((uint8_t *) &rtp_file_data + i);
    }
    SYS->IPRST0 &= ~SYS_IPRST0_CM4RST_Msk;

    SYS_LockReg();

    /* Connect UART to PC, and open a terminal tool to receive following message */
    sysprintf("+------------------------------------------------------------+\n");
    sysprintf("| This sample code demonstrate OpenAMP share memory function |\n");
    sysprintf("+------------------------------------------------------------+\n");
    sysprintf("| Share Memory Address    : 0x%08x                       |\n", SHM_START_ADDRESS);
    sysprintf("| TX/RX Buffer Size (Byte):   %8d                       |\n", SHM_TX_RX_SIZE);
    sysprintf("| mbox channel            :   %8d                       |\n", mbox_ch);
    sysprintf("+------------------------------------------------------------+\n\n");

    MA35D1_OpenAMP_Init(RPMSG_REMOTE, NULL);

    rpmsg_list_init(&resmgr_ept.node);
    rpmsg_list_init(&rvdev.rdev.endpoints);

    OPENAMP_create_endpoint(&resmgr_ept, "rpmsg-sample", RPMSG_ADDR_ANY, rx_callback, NULL);

    /* Initialize Tx data */
    for (i = 0; i < TX_RX_SIZE; i++)
    {
        transmit_rpmsg[i] = 255 - i;
    }

    /* A35 TX 10 bytes */
    OPENAMP_send_data(&resmgr_ept, transmit_rpmsg, 10);
    while (1)
    {
        if (OPENAMP_check_TxAck(&resmgr_ept) == 1)
        {
            break;
        }
    }

    sysprintf("# Write 10 bytes data to RTP finish!\n\n");

    /* A35 RX */
    while (1)
    {
        /* Check M4 Tx and then send ACK to M4 */
        OPENAMP_check_for_message(&resmgr_ept);

        if (rx_status)
        {
            rx_status = 0;
        }
    }

    sysprintf("\n Test END !!\n");

    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/

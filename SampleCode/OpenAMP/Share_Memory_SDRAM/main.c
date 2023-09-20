/*************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Demonstrate OpenAMP share memory control.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NuMicro.h"
#include "mmio.h"
#include "rpmsg.h"
#include "openamp.h"
#include "mbox_whc.h"

#define TEST_SECONDS    10
#define TX_RX_SIZE      SHM_TX_RX_SIZE

extern vu32 WHC0_RX_Flag;
extern uint32_t rtp_file_data, rtp_file_end;
extern struct rpmsg_virtio_device rvdev;

uint8_t received_rpmsg[TX_RX_SIZE] __attribute__((aligned(32)));
uint8_t transmit_rpmsg[TX_RX_SIZE] __attribute__((aligned(32)));
uint8_t u8RxStart = FALSE;

uint32_t au32RxTmp[4];
uint32_t u32TxSize;
uint32_t u32RxSize;

uint32_t g_au32RxBuf[WHC_BUFFER_LEN];
uint32_t g_au32TxBuf[WHC_BUFFER_LEN];

static uint32_t rx_status = 0;
static volatile uint64_t _start_time = 0;

static int rx_callback(struct rpmsg_endpoint *rp_chnl, void *data, size_t len, uint32_t src, void *priv);
static void rpmsg_list_init(struct metal_list *list);

/*--------------------------------------------------------------------------*/
static void rpmsg_list_init(struct metal_list *list)
{
    list->next = list->prev = list;
}

void start_timer(void)
{
    _start_time = EL0_GetCurrentPhysicalValue();
}

uint32_t get_ticks(void)
{
    uint64_t t_off;
    t_off = EL0_GetCurrentPhysicalValue() - _start_time;
    t_off = t_off / 12000;
    return (uint32_t)t_off;
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

    /* Enable RTP M4 clock */
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
        if (u8RxStart != TRUE)
        {
            u8RxStart = TRUE;
            start_timer();
        }

        memcpy((void *)((char *)received_rpmsg), (const void *) ((u64)src), len > sizeof(received_rpmsg) ? sizeof(received_rpmsg) : len);

#ifdef CHECK_RX_DATA
        /* !! Note: sysprintf will reduce performance. */
        sysprintf("\n Receive %d bytes data from RTP (SRAM): \n", len);
#endif

        u32RxSize += len;
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
    uint32_t i, ret, u32ImageSize;
    uint64_t t0;

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
    sysprintf("\nThis sample code demonstrate OpenAMP share memory function (A35 side)\n\n");
    sysprintf("Share Memory Address    : 0x%08x\n", SHM_START_ADDRESS);
    sysprintf("TX/RX Buffer Size (Byte): %d\n\n", SHM_TX_RX_SIZE);

    MA35D1_OpenAMP_Init(RPMSG_REMOTE, NULL);

    rpmsg_list_init(&resmgr_ept.node);
    rpmsg_list_init(&rvdev.rdev.endpoints);

    OPENAMP_create_endpoint(&resmgr_ept, "rpmsg-sample", RPMSG_ADDR_ANY, rx_callback, NULL);

    sysprintf("+---------------------------------------+\n");
    sysprintf("|                                       |\n");
    sysprintf("|  A35 Send data to RTP M4 (10 seconds) |\n");
    sysprintf("|                                       |\n");
    sysprintf("+---------------------------------------+\n");

    /* Initialize Tx data */
    for (i = 0; i < TX_RX_SIZE; i++)
    {
        transmit_rpmsg[i] = i;
    }

    /* Inform M4 that A35 TX test starts */
    OPENAMP_send_data(&resmgr_ept, transmit_rpmsg, 0);
    while (1)
    {
        if (OPENAMP_check_TxAck(&resmgr_ept) == 1)
            break;
    }

    start_timer();
    u32TxSize = 0;
    while (1)
    {
        t0 = get_ticks();
        if (t0 >= (TEST_SECONDS * 1000))
            break;

        /* Send message to RTP M4 and wait RTP M4 response ACK */
        ret = OPENAMP_send_data(&resmgr_ept, transmit_rpmsg, TX_RX_SIZE);
        if (ret < 0)
        {
            sysprintf("Failed to send message\r\n");
            sysprintf("Stop test ...\n");
            while (1);
        }

        while (1)
        {
            if (OPENAMP_check_TxAck(&resmgr_ept) == 1)
                break;
        }

        u32TxSize += ret;
    }

    sysprintf("# Transfer Time (ms)  : %d\n", t0);
    sysprintf("# Transfer Size (byte): %d\n", u32TxSize);
    sysprintf("# Transfer Speed (B/s): %d\n\n", u32TxSize / t0 * 1000);

    sysprintf("+--------------------------------------------+\n");
    sysprintf("|                                            |\n");
    sysprintf("|  A35 receive data from RTP M4 (10 seconds) |\n");
    sysprintf("|                                            |\n");
    sysprintf("+--------------------------------------------+\n");

    /* Inform M4 that A35 RX test starts */
    u8RxStart = TRUE;
    OPENAMP_send_data(&resmgr_ept, transmit_rpmsg, 0);
    while (1)
    {
        if (OPENAMP_check_TxAck(&resmgr_ept) == 1)
            break;
    }

    u8RxStart = FALSE;
    u32RxSize = 0;
    while (1)
    {
        if (u8RxStart)
        {
            t0 = get_ticks();
            if (t0 >= (TEST_SECONDS * 1000))
                break;
        }

        /* Check M4 Tx and send ACK to M4 */
        OPENAMP_check_for_message(&resmgr_ept);

        if (rx_status)
        {
            rx_status = 0;

#ifdef CHECK_RX_DATA
            /* !! Note: comparing will reduce performance. */
            for (i = 0; i < TX_RX_SIZE; i++)
            {
                if (received_rpmsg[i] != (i %256))
                {
                    sysprintf(" The data received is wrong [i %d = %d]\n", i, received_rpmsg[i]);
                    sysprintf(" Stop test ...\n\n");
                    while (1);
                }
            }
#endif
        }
    }

    sysprintf("# Receive Time (ms)  : %d\n", t0);
    sysprintf("# Receive Size (byte): %d\n", u32RxSize);
    sysprintf("# Receive Speed (B/s): %d\n\n", u32RxSize / t0 * 1000);

    sysprintf("\n Test END !!\n");

    while (1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/

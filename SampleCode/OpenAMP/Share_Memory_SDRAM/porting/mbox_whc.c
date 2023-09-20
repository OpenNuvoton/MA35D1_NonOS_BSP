/**************************************************************************//**
 * @file     mbox_whc.c
 *
 * @brief    OpenAMP mailbox example source file
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include "openamp/open_amp.h"
#include "NuMicro.h"
#include "openamp_conf.h"

vu32 WHC0_RX_Flag = RX_NO_MSG;
vu32 WHC0_TX_Flag = TX_NO_ACK;

uint32_t au32RxCom[4];
uint32_t au32TxCom[4];
uint32_t au32RxBuf[4];
uint32_t au32TxBuf[4];

void WHC0_IRQHandler(void);

//Initialize WHC
int Mbox_Init(void)
{
    /* Enable WHC GIC */
    IRQ_SetHandler((IRQn_ID_t)WRHO0_IRQn, WHC0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)WRHO0_IRQn);

    if (mbox_ch == 0)
    {
        WHC_ENABLE_INT(WHC0, WHC_INTEN_RX0IEN_Msk);
    }
    else if (mbox_ch == 1)
    {
        WHC_ENABLE_INT(WHC0, WHC_INTEN_RX1IEN_Msk);
    }
    else if (mbox_ch == 2)
    {
        WHC_ENABLE_INT(WHC0, WHC_INTEN_RX2IEN_Msk);
    }

    return 0;
}

// Poll mailbox
int Mbox_Poll(struct rpmsg_endpoint *ept)
{
    uint32_t u32Len = 0;

    if (WHC0_RX_Flag == RX_NEW_MSG)
    {
        u32Len = (au32RxBuf[3] << 16) | (au32RxBuf[2] << 8) | au32RxBuf[1];
        ept->cb(ept, &au32RxBuf, (size_t)u32Len, SHM_RX_START_ADDRESS, NULL);
        WHC0_RX_Flag = RX_NO_MSG;

        au32TxBuf[0] = COMMAND_SEND_ACK_TO_M4;
        WHC_Send(WHC0, mbox_ch, au32TxBuf);
        while ((WHC0->TXSTS & 0xf) != 0xf); // wait Tx

        return 0;
    }

    return -1;
}

// OpenAMP callback function
int Mbox_Notify(void *priv, uint32_t id)
{
    return 0;
}

// WHC IRQ handler
// !! Note: sysprintf will delay Rx processing.
void WHC0_IRQHandler(void)
{
    uint32_t i;

    if (WHC_IS_RX_READY(WHC0, mbox_ch))
    {
        WHC_Recv(WHC0, mbox_ch, &au32RxCom[0]);

        if (au32RxCom[0] == COMMAND_RECEIVE_M4_MSG)
        {
            for (i = 0; i < 4; i++)
                au32RxBuf[i] = au32RxCom[i];

            WHC0_RX_Flag = RX_NEW_MSG;
        }
        else if (au32RxCom[0] == COMMAND_RECEIVE_M4_ACK)
        {
            WHC0_TX_Flag = TX_ACK;
        }

        // Clear interrupt flag
        WHC_CLR_INT_FLAG(WHC0, WHC_INTSTS_RX0IF_Msk);
    }
}

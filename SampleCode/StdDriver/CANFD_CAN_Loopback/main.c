/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Use CAN mode function to do internal loopback test.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "stdio.h"
#include "string.h"
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
CANFD_FD_MSG_T      g_sRxMsgFrame;
CANFD_FD_MSG_T      g_sTxMsgFrame;
volatile uint8_t   g_u8RxFifo0CompleteFlag = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void SYS_Init(void);
void CANFD0_TEST_HANDLE(void);
void CANFD00_IRQHandler(void);

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle CAN FD0 Line0 interrupt event                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void CANFD00_IRQHandler(void)
{
    sysprintf("IR =0x%08X \n", CANFD0->IR);
    /*Clear the Interrupt flag */
    CANFD_ClearStatusFlag(CANFD0, CANFD_IR_TOO_Msk | CANFD_IR_RF0N_Msk);
    CANFD_ReadRxFifoMsg(CANFD0, 0, &g_sRxMsgFrame);
    g_u8RxFifo0CompleteFlag = 1;
}

void *_MemSet(void *s, int c, size_t count)
{
    unsigned char *p = s;

    while (count--)
        if ((unsigned char)c == *p++)
    return (void *)(p - 1);
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
	CLK_SetPLLClockFreq(APLL, PLL_OPMODE_INTEGER, FREQ_PLLSRC, 150000000);
	CLK_WaitClockReady(CLK_STATUS_STABLE_APLL);

    /* Enable IP clock */
    CLK_SetModuleClock(CANFD0_MODULE, CLK_CLKSEL4_CANFD0SEL_APLL, 0);
    CLK_EnableModuleClock(CANFD0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(2));
    CLK_EnableModuleClock(UART0_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);
    /* Set PK multi-function pins for CANFD0 RXD and TXD */
    SYS->GPI_MFPL &= ~(SYS_GPI_MFPL_PI2MFP_Msk | SYS_GPI_MFPL_PI3MFP_Msk);
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI2MFP_CAN0_RXD | SYS_GPI_MFPL_PI3MFP_CAN0_TXD;
}

/*---------------------------------------------------------------------------------------------------------*/
/*                             CAN Tx Rx Function Test                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void CAN_TxRx_Test(CANFD_FD_MSG_T *psTxMsg, E_CANFD_ID_TYPE eFrameIdType, uint32_t u32Id, uint8_t u8Len)
{
    uint8_t u8Cnt;

    /*Set the ID Number*/
    psTxMsg->u32Id = u32Id;
    /*Set the ID Type*/
    psTxMsg->eIdType = eFrameIdType;
    /*Set the frame type*/
    psTxMsg->eFrmType = eCANFD_DATA_FRM;
    /*Set FD frame format attribute */
    psTxMsg->bFDFormat = 0;
    /*Set the bitrate switch attribute*/
    psTxMsg->bBitRateSwitch = 0;
    /*Set data length*/
    psTxMsg->u32DLC = u8Len;

    for (u8Cnt = 0; u8Cnt < psTxMsg->u32DLC; u8Cnt++) psTxMsg->au8Data[u8Cnt] = u8Cnt;

    g_u8RxFifo0CompleteFlag = 0;

    /* use message buffer 0 */
    if (eFrameIdType == eCANFD_SID)
        sysprintf("Send to transmit message 0x%08x (11-bit)\n", psTxMsg->u32Id);
    else
        sysprintf("Send to transmit message 0x%08x (29-bit)\n", psTxMsg->u32Id);

    if (CANFD_TransmitTxMsg(CANFD0, 0, psTxMsg) != 1)
    {
        sysprintf("Failed to transmit message\n");
    }

    /*Wait the Rx FIFO0 received message*/
    while (!g_u8RxFifo0CompleteFlag)
    {
    }

    sysprintf("Rx buf 0: Received message 0x%08X\n", g_sRxMsgFrame.u32Id);
    sysprintf("Message Data : ");

    for (u8Cnt = 0; u8Cnt <  g_sRxMsgFrame.u32DLC; u8Cnt++)
    {
        sysprintf("%02d ,", g_sRxMsgFrame.au8Data[u8Cnt]);
    }

    sysprintf("\n\n");
    _MemSet(&g_sRxMsgFrame, 0, sizeof(g_sRxMsgFrame));
}

/*---------------------------------------------------------------------------------------------------------*/
/*                CAN Function Test                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
void CAN_Loopback(void)
{
    uint8_t u8Loop;
    CANFD_FD_T sCANFD_Config;

    /*Get the CAN configuration value*/
    CANFD_GetDefaultConfig(&sCANFD_Config, CANFD_OP_CAN_MODE);
    /*Enable internal loopback mode*/
    sCANFD_Config.sBtConfig.bEnableLoopBack = TRUE;
    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 1000000;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 0;
    /*Open the CAN FD feature*/
    CANFD_Open(CANFD0, &sCANFD_Config);

    /* receive 0x110~0x11F in CAN rx fifo0 buffer by setting mask 0 */
    CANFD_SetSIDFltr(CANFD0, 0, CANFD_RX_FIFO0_STD_MASK(0x110, 0x7F0));
    /* receive 0x22F in CAN rx fifo0 buffer by setting mask 1 */
    CANFD_SetSIDFltr(CANFD0, 1, CANFD_RX_FIFO0_STD_MASK(0x22F, 0x7FF));
    /* receive 0x333 in CAN rx fifo0 buffer by setting mask 2 */
    CANFD_SetSIDFltr(CANFD0, 2, CANFD_RX_FIFO0_STD_MASK(0x333, 0x7FF));

    /* receive 0x220~0x22F (29-bit id) in CAN rx fifo0 buffer by setting mask 0 */
    CANFD_SetXIDFltr(CANFD0, 0, CANFD_RX_FIFO0_EXT_MASK_LOW(0x220), CANFD_RX_FIFO0_EXT_MASK_HIGH(0x1FFFFFF0));
    /* receive 0x3333 (29-bit id) in CAN rx fifo0 buffer by setting mask 1 */
    CANFD_SetXIDFltr(CANFD0, 1, CANFD_RX_FIFO0_EXT_MASK_LOW(0x3333), CANFD_RX_FIFO0_EXT_MASK_HIGH(0x1FFFFFFF));
    /* receive 0x44444 (29-bit id) in CAN rx fifo0 buffer by setting mask 2 */
    CANFD_SetXIDFltr(CANFD0, 2, CANFD_RX_FIFO0_EXT_MASK_LOW(0x44444), CANFD_RX_FIFO0_EXT_MASK_HIGH(0x1FFFFFFF));
    /* Enable Standard ID and  Extended ID Filter as RX FOFI0*/
    CANFD_SetGFC(CANFD0, eCANFD_ACC_NON_MATCH_FRM_RX_FIFO0, eCANFD_ACC_NON_MATCH_FRM_RX_FIFO0, 1, 1);
    /* Enable RX fifo0 new message interrupt using interrupt line 0. */
    IRQ_SetHandler((IRQn_ID_t)CANFD00_IRQn, CANFD00_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)CANFD00_IRQn);
    CANFD_EnableInt(CANFD0, (CANFD_IE_TOOE_Msk | CANFD_IE_RF0NE_Msk), 0, 0, 0);
    /* CAN FD0 Run to Normal mode  */
    CANFD_RunToNormal(CANFD0, TRUE);

    for (u8Loop = 1 ; u8Loop < 8; u8Loop++)
    {
        CAN_TxRx_Test(&g_sTxMsgFrame, eCANFD_SID, 0x110 + u8Loop, u8Loop);
    }

    CAN_TxRx_Test(&g_sTxMsgFrame, eCANFD_SID, 0x22F, 8);
    CAN_TxRx_Test(&g_sTxMsgFrame, eCANFD_SID, 0x333, 8);

    for (u8Loop = 1 ; u8Loop < 8; u8Loop++)
    {
        CAN_TxRx_Test(&g_sTxMsgFrame, eCANFD_XID, 0x220 + u8Loop, u8Loop);
    }

    CAN_TxRx_Test(&g_sTxMsgFrame, eCANFD_XID, 0x3333, 8);
    CAN_TxRx_Test(&g_sTxMsgFrame, eCANFD_XID, 0x44444, 8);
}

/*---------------------------------------------------------------------------------------------------------*/
/*                                         Main Function                                                   */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);
	
    /* print a note to terminal */
    sysprintf("\n CAN Mode Loopback example\r\n");

    /* CAN Loopback Test */
    CAN_Loopback();
    sysprintf("\n CAN Mode Loopback Test Done\r\n");

    while (1);
}

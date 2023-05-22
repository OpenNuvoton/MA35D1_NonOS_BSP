/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    Transmit and receive CAN message through CAN interface.
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

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void SYS_Init(void);
void CAN_Init(void);
void CAN_TxRxTest(void);

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
/*                                    Init CAN                                                             */
/*---------------------------------------------------------------------------------------------------------*/
void CAN_Init(void)
{
    CANFD_FD_T sCANFD_Config;

    sysprintf("+---------------------------------------------------------------+\n");
    sysprintf("|     Pin Configure                                             |\n");
    sysprintf("+---------------------------------------------------------------+\n");
    sysprintf("|  CAN0_TXD(PI3)                         CAN_TXD(Any board)     |\n");
    sysprintf("|  CAN0_RXD(PI2)                         CAN_RXD(Any board)     |\n");
    sysprintf("|          |-----------| CANBUS  |-----------|                  |\n");
    sysprintf("|  ------> |           |<------->|           |<------           |\n");
    sysprintf("|  CAN0_TX |   CAN     |  CAN_H  |    CAN    |CAN_TX            |\n");
    sysprintf("|          |Transceiver|         |Transceiver|                  |\n");
    sysprintf("|  <------ |           |<------->|           |------>           |\n");
    sysprintf("|  CAN0_RX |           |  CAN_L  |           |CAN_RX            |\n");
    sysprintf("|          |-----------|         |-----------|                  |\n");
    sysprintf("|                                                               |\n");
    sysprintf("+---------------------------------------------------------------+\n\n");

    CANFD_GetDefaultConfig(&sCANFD_Config, CANFD_OP_CAN_MODE);
    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 1000000;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 0;
    CANFD_Open(CANFD0, &sCANFD_Config);

    /* receive 0x111 in CAN rx message buffer 0 by setting mask 0 */
    CANFD_SetSIDFltr(CANFD0, 0, CANFD_RX_BUFFER_STD(0x111, 0));
    /* receive 0x22F in CAN rx message buffer 0 by setting mask 1 */
    CANFD_SetSIDFltr(CANFD0, 1, CANFD_RX_BUFFER_STD(0x22F, 0));
    /* receive 0x333 in CAN rx message buffer 0 by setting mask 2 */
    CANFD_SetSIDFltr(CANFD0, 2, CANFD_RX_BUFFER_STD(0x333, 0));

    /* receive 0x222 (29-bit id) in CAN rx message buffer 1 by setting mask 3 */
    CANFD_SetXIDFltr(CANFD0, 0, CANFD_RX_BUFFER_EXT_LOW(0x222, 1), CANFD_RX_BUFFER_EXT_HIGH(0x222, 1));
    /* receive 0x3333 (29-bit id) in CAN rx message buffer 1 by setting mask 3 */
    CANFD_SetXIDFltr(CANFD0, 1, CANFD_RX_BUFFER_EXT_LOW(0x3333, 1), CANFD_RX_BUFFER_EXT_HIGH(0x3333, 1));
    /* receive 0x44444 (29-bit id) in CAN rx message buffer 1 by setting mask 3 */
    CANFD_SetXIDFltr(CANFD0, 2, CANFD_RX_BUFFER_EXT_LOW(0x44444, 1), CANFD_RX_BUFFER_EXT_HIGH(0x44444, 1));
    /* CAN FD0 Run to Normal mode  */
    CANFD_RunToNormal(CANFD0, TRUE);
}


/*---------------------------------------------------------------------------------------------------------*/
/*  CAN Function Test                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void CAN_TxRxTest(void)
{
    uint8_t u8Item;
    uint8_t u8Cnt;
    uint8_t u8ErrFlag = 0;
    uint8_t u8TxTestNum = 0;
    uint8_t u8RxTestNum = 0;
    uint8_t u8RxTempLen = 0;
    CANFD_FD_MSG_T      sRxFrame;
    CANFD_FD_MSG_T      sTxMsgFrame;

    /* CAN interface initialization*/
    CAN_Init();

    sysprintf("+--------------------------------------------------------------------------+\n");
    sysprintf("|                       CAN Function Test                                  |\n");
    sysprintf("+--------------------------------------------------------------------------+\n");
    sysprintf("|  Description :                                                           |\n");
    sysprintf("|    The sample code needs two boards. One is master(CAN transmitter) and  |\n");
    sysprintf("|    the other is slave(CAN receiver). Master will send 6 messages with    |\n");
    sysprintf("|    different sizes of data and ID to the slave. Slave will check if      |\n");
    sysprintf("|    received data is correct after getting 6 messages data.               |\n");
    sysprintf("|  Please select Master or Slave test                                      |\n");
    sysprintf("|  [0] Master(CAN transmitter)    [1] Slave(CAN receiver)                  |\n");
    sysprintf("+--------------------------------------------------------------------------+\n\n");
    u8Item = sysgetchar();

    if (u8Item == '0')
    {
        /* Send 6 messages with different ID and data size */
        for (u8TxTestNum = 0; u8TxTestNum < 6 ; u8TxTestNum++)
        {
            sysprintf("CAN Bus Transmitter :\n");

            /* Set the ID Number */
            if (u8TxTestNum == 0)      sTxMsgFrame.u32Id = 0x111;
            else if (u8TxTestNum == 1) sTxMsgFrame.u32Id = 0x22F;
            else if (u8TxTestNum == 2) sTxMsgFrame.u32Id = 0x333;
            else if (u8TxTestNum == 3) sTxMsgFrame.u32Id = 0x222;
            else if (u8TxTestNum == 4) sTxMsgFrame.u32Id = 0x3333;
            else if (u8TxTestNum == 5) sTxMsgFrame.u32Id = 0x44444;

            /*Set the ID type*/
            if (u8TxTestNum < 3)
                sTxMsgFrame.eIdType = eCANFD_SID;
            else
                sTxMsgFrame.eIdType = eCANFD_XID;

            /*Set the frame type*/
            sTxMsgFrame.eFrmType = eCANFD_DATA_FRM;
            /*Set the bitrate switch */
            sTxMsgFrame.bBitRateSwitch = 0;

            /*Set the data lenght */
            if (u8TxTestNum == 0  ||  u8TxTestNum == 3)     sTxMsgFrame.u32DLC = 2;
            else if (u8TxTestNum == 1 || u8TxTestNum == 4)  sTxMsgFrame.u32DLC = 4;
            else if (u8TxTestNum == 2 || u8TxTestNum == 5)  sTxMsgFrame.u32DLC = 8;

            for (u8Cnt = 0; u8Cnt < sTxMsgFrame.u32DLC; u8Cnt++) sTxMsgFrame.au8Data[u8Cnt] = u8Cnt + u8TxTestNum;

            if (u8TxTestNum < 3)
                sysprintf("Send to transmit message 0x%08x (11-bit)\n", sTxMsgFrame.u32Id);
            else
                sysprintf("Send to transmit message 0x%08x (29-bit)\n", sTxMsgFrame.u32Id);

            /* use message buffer 0 */
            if (CANFD_TransmitTxMsg(CANFD0, 0, &sTxMsgFrame) != 1)
            {
                sysprintf("Failed to transmit message\n");
            }
        }

        sysprintf("\n Transmit Done\n");
    }
    else
    {
        sysprintf("Start to CAN Bus Receiver :\n");

        /* Receive  6 messages with different ID and data size */
        do
        {
            /* check for any received messages on CAN FD0 message buffer 0 */
            if (CANFD_ReadRxBufMsg(CANFD0, 0, &sRxFrame) == 1)
            {

                sysprintf("Rx buf 0: Received message 0x%08X( 11-bit)\n", sRxFrame.u32Id);
                sysprintf("Message Data : ");

                for (u8Cnt = 0; u8Cnt < sRxFrame.u32DLC; u8Cnt++)
                {
                    sysprintf("%02d ,", sRxFrame.au8Data[u8Cnt]);

                    if (sRxFrame.au8Data[u8Cnt] != u8Cnt + u8RxTestNum)
                    {
                        u8ErrFlag = 1;
                    }
                }

                sysprintf(" \n\n");

                /* Check Standard ID number */
                if ((sRxFrame.u32Id != 0x111) && (sRxFrame.u32Id != 0x22F) && (sRxFrame.u32Id != 0x333))
                {
                    u8ErrFlag = 1;
                }

                if (u8RxTestNum == 0)      u8RxTempLen = 2;
                else if (u8RxTestNum == 1) u8RxTempLen = 4;
                else if (u8RxTestNum == 2) u8RxTempLen = 8;

                /* Check Data lenght */
                if ((u8RxTempLen != sRxFrame.u32DLC) || (sRxFrame.eIdType != eCANFD_SID))
                {
                    u8ErrFlag = 1;
                }

                if (u8ErrFlag == 1)
                {
                    sysprintf("CAN STD ID or Data Error \n");
                    sysgetchar();
                }

                u8RxTestNum++;
            }

            /* check for any received messages on CANF D0 message buffer 1 */
            if (CANFD_ReadRxBufMsg(CANFD0, 1, &sRxFrame) == 1)
            {

                sysprintf("Rx buf 1: Received message 0x%08X (29-bit)\n", sRxFrame.u32Id);
                sysprintf("Message Data : ");

                for (u8Cnt = 0; u8Cnt < sRxFrame.u32DLC; u8Cnt++)
                {
                    sysprintf("%02d ,", sRxFrame.au8Data[u8Cnt]);

                    if (sRxFrame.au8Data[u8Cnt] != u8Cnt + u8RxTestNum)
                    {
                        u8ErrFlag = 1;
                    }
                }

                sysprintf(" \n\n");

                /* Check Extend ID number */
                if ((sRxFrame.u32Id != 0x222) && (sRxFrame.u32Id != 0x3333) && (sRxFrame.u32Id != 0x44444))
                {
                    u8ErrFlag = 1;
                }

                if (u8RxTestNum == 3)      u8RxTempLen = 2;
                else if (u8RxTestNum == 4) u8RxTempLen = 4;
                else if (u8RxTestNum == 5) u8RxTempLen = 8;

                /* Check Data lenght */
                if ((u8RxTempLen != sRxFrame.u32DLC) || (sRxFrame.eIdType != eCANFD_XID))
                {
                    u8ErrFlag = 1;
                }

                if (u8ErrFlag == 1)
                {
                    sysprintf("CAN EXD ID or Data Error \n");
                    sysgetchar();
                }

                u8RxTestNum++;

            }
        } while (u8RxTestNum < 6);

        sysprintf("\n Receive OK & Check OK\n");
    }
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

    /*---------------------------------------------------------------------------------------------------------*/
    /*                                                 SAMPLE CODE                                             */
    /*---------------------------------------------------------------------------------------------------------*/
    sysprintf("\n\nCPU @ %dHz\n", SystemCoreClock);
    sysprintf("+---------------------------------------+\n");
    sysprintf("|        CAN mode transmission test     |\n");
    sysprintf("+---------------------------------------+\n");

    /* CAN sample function */
    CAN_TxRxTest();

    while (1) {}
}

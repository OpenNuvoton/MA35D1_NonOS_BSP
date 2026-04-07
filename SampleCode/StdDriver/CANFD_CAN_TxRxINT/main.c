/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    An example of interrupt control using CAN bus communication.
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
volatile uint8_t   g_u8RxFifO0CompleteFlag = 0;
volatile uint8_t   g_u8BusOffFlag = 0;
volatile uint32_t  g_u32BusOffRecoveryCounter = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void SYS_Init(void);
void CAN_Init(void);
void CAN_SendMessage(CANFD_FD_MSG_T *psTxMsg, E_CANFD_ID_TYPE eIdType, uint32_t u32Id, uint8_t u8Len);
void CAN_ShowRecvMessage(void);
void CAN_RxTest(void);
void CAN_TxTest(void);
uint8_t CAN_BusOffRecovery(void);

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle CAN FD0 Line0 interrupt event                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void CANFD00_IRQHandler(void)
{
    uint32_t u32IntStatus;

    u32IntStatus = CANFD0->IR;
    sysprintf("IR =0x%08X \n", u32IntStatus);

    /* Check Error Warning status */
    if (u32IntStatus & CANFD_IR_EW_Msk)
    {
        sysprintf("Error warning flag is set.\n");
        CANFD_ClearStatusFlag(CANFD0, CANFD_IR_EW_Msk);
    }

    /* Check Error Passive status */
    if (u32IntStatus & CANFD_IR_EP_Msk)
    {
        sysprintf("Error passive flag is set.\n");
        CANFD_ClearStatusFlag(CANFD0, CANFD_IR_EP_Msk);
    }

    /* Check Bus-Off status */
    if (u32IntStatus & CANFD_IR_BO_Msk)
    {
        if (CANFD0->PSR & CANFD_PSR_BO_Msk)
        {
            sysprintf("Bus-Off detected! Recovery will be triggered on next transmission attempt.\n");
            g_u8BusOffFlag = 1;
        }

        /* Always clear Bus-Off interrupt flag to prevent repeated interrupts */
        CANFD_ClearStatusFlag(CANFD0, CANFD_IR_BO_Msk);
    }

    /* Check Rx FIFO0 New Message interrupt */
    if (u32IntStatus & CANFD_IR_RF0N_Msk)
    {
        /* Receive the Rx Fifo0 buffer */
        CANFD_ReadRxFifoMsg(CANFD0, 0, &g_sRxMsgFrame);
        g_u8RxFifO0CompleteFlag = 1;
        /* Clear Rx FIFO0 New Message interrupt flag */
        CANFD_ClearStatusFlag(CANFD0, CANFD_IR_RF0N_Msk);
    }

    /* Clear other interrupt flags */
    CANFD_ClearStatusFlag(CANFD0, CANFD_IR_TOO_Msk);
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
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
    CLK_EnableModuleClock(TMR0_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);
    /* Set PI multi-function pins for CANFD0 RXD and TXD */
    SYS->GPI_MFPL &= ~(SYS_GPI_MFPL_PI2MFP_Msk | SYS_GPI_MFPL_PI3MFP_Msk);
    SYS->GPI_MFPL |= (SYS_GPI_MFPL_PI2MFP_CAN0_RXD | SYS_GPI_MFPL_PI3MFP_CAN0_TXD);
}

/*---------------------------------------------------------------------------------------------------------*/
/*              CAN Function Test Menu(Master)                                                             */
/*---------------------------------------------------------------------------------------------------------*/
void CAN_TestItem(void)
{
    sysprintf("\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("|              CAN Tx Function Test (Master)                |\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("| [1] Standard ID = 0x111    (Date Length 8bytes)           |\n");
    sysprintf("| [2] Standard ID = 0x22F    (Date Length 8bytes)           |\n");
    sysprintf("| [3] Standard ID = 0x333    (Date Length 8bytes)           |\n");
    sysprintf("| [4] Extended ID = 0x221    (Date Length 8bytes)           |\n");
    sysprintf("| [5] Extended ID = 0x3333   (Date Length 8bytes)           |\n");
    sysprintf("| [6] Extended ID = 0x44444  (Date Length 8bytes)           |\n");
    sysprintf("| Select ID number and master will send message to slave ...|\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("| Quit                                              - [ESC] |\n");
    sysprintf("+-----------------------------------------------------------+\n\n");
}


/*---------------------------------------------------------------------------------------------------------*/
/*                             CAN Function Tx Test                                                        */
/*---------------------------------------------------------------------------------------------------------*/
void CAN_TxTest(void)
{
    uint8_t u8Item;

    do
    {
        CAN_TestItem();
        u8Item = sysgetchar();

        switch (u8Item)
        {
            case '1':
                /*Standard ID =0x111,Data lenght 8 bytes*/
                CAN_SendMessage(&g_sTxMsgFrame, eCANFD_SID, 0x111, 8);
                break;

            case '2':
                /*Standard ID =0x22F,Data lenght 8 bytes*/
                CAN_SendMessage(&g_sTxMsgFrame, eCANFD_SID, 0x22F, 8);
                break;

            case '3':
                /*Standard ID =0x333,Data lenght 8 bytes*/
                CAN_SendMessage(&g_sTxMsgFrame, eCANFD_SID, 0x333, 8);
                break;

            case '4':
                /*Extend ID =0x111,Data lenght 8 bytes*/
                CAN_SendMessage(&g_sTxMsgFrame, eCANFD_XID, 0x221, 8);
                break;

            case '5':
                /*Extend ID =0x3333,Data lenght 8 bytes*/
                CAN_SendMessage(&g_sTxMsgFrame, eCANFD_XID, 0x3333, 8);
                break;

            case '6':
                /*Extend ID =0x44444,Data lenght 8 bytes*/
                CAN_SendMessage(&g_sTxMsgFrame, eCANFD_XID, 0x44444, 8);
                break;

            default:
                break;
        }

    } while (u8Item != 27);
}


/*---------------------------------------------------------------------------------------------------------*/
/*                             CAN Send Message Function                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void CAN_SendMessage(CANFD_FD_MSG_T *psTxMsg, E_CANFD_ID_TYPE eIdType, uint32_t u32Id, uint8_t u8Len)
{
    uint8_t u8Cnt;

    /* Check if CAN is in Bus-Off state before transmitting */
    if (g_u8BusOffFlag)
    {
        sysprintf("CAN is in Bus-Off state. Starting recovery process...\n");

        if (CAN_BusOffRecovery())
        {
            sysprintf("Bus-Off recovery successful. Proceeding with transmission.\n");
        }
        else
        {
            sysprintf("Bus-Off recovery failed. Cannot transmit.\n");
            return;
        }
    }

    psTxMsg->u32Id = u32Id;
    psTxMsg->eIdType = eIdType;
    psTxMsg->eFrmType = eCANFD_DATA_FRM;
    psTxMsg->bBitRateSwitch = 0;
    psTxMsg->u32DLC = u8Len;

    for (u8Cnt = 0; u8Cnt < psTxMsg->u32DLC; u8Cnt++) psTxMsg->au8Data[u8Cnt] = u8Cnt;

    g_u8RxFifO0CompleteFlag = 0;

    /* use message buffer 0 */
    if (eIdType == eCANFD_SID)
        sysprintf("Send to transmit message 0x%08x (11-bit)\n", psTxMsg->u32Id);
    else
        sysprintf("Send to transmit message 0x%08x (29-bit)\n", psTxMsg->u32Id);

    sysprintf("Data Message(%02d bytes) : ", psTxMsg->u32DLC);

	for (u8Cnt = 0; u8Cnt < psTxMsg->u32DLC; u8Cnt++)
	{
		psTxMsg->au8Data[u8Cnt] = u8Cnt;
		sysprintf("%02d,", psTxMsg->au8Data[u8Cnt]);
	}

	sysprintf("\n");

    if (CANFD_TransmitTxMsg(CANFD0, 0, psTxMsg) != 1)
    {
        sysprintf("Failed to transmit message\n");
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/*                             CAN CAN Function Rx Test                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void CAN_RxTest(void)
{
    uint8_t u8Cnt = 0;
    sysprintf("Start CAN bus reception :\n");

    do
    {
        while (!g_u8RxFifO0CompleteFlag) {
        /* Wait for interrupt to set flag */
        }

        CAN_ShowRecvMessage();
        g_u8RxFifO0CompleteFlag = 0;
        memset(&g_sRxMsgFrame, 0, sizeof(g_sRxMsgFrame));
        u8Cnt++;
    } while (u8Cnt < 6);
}


/*---------------------------------------------------------------------------------------------------------*/
/*                           CAN Receive Message Function                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void CAN_ShowRecvMessage(void)
{
    uint8_t u8Cnt;

    if (g_sRxMsgFrame.eIdType == eCANFD_SID)
        sysprintf("Rx FIFO1(Standard ID) ID = 0x%08X\n", g_sRxMsgFrame.u32Id);
    else
        sysprintf("Rx FIFO1(Extended ID) ID = 0x%08X\n", g_sRxMsgFrame.u32Id);

    sysprintf("Message Data(%02d bytes) : ", g_sRxMsgFrame.u32DLC);

    for (u8Cnt = 0; u8Cnt <  g_sRxMsgFrame.u32DLC; u8Cnt++)
    {
        sysprintf("%02d ,", g_sRxMsgFrame.au8Data[u8Cnt]);
    }

    sysprintf("\n\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/*                                 CAN Bus-Off Recovery Function                                           */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t CAN_BusOffRecovery(void)
{
    sysprintf("Starting CAN Bus-Off recovery sequence...\n");

    /* CAN0 run to initial mode */
    CANFD_RunToNormal(CANFD0, FALSE);

    /* Cancel all transmit requests */
    CANFD0->TXBCR = 0xFFFFFFFF;

    /* Clear all interrupt flag */
    CANFD_ClearStatusFlag(CANFD0, 0xFFFFFFFF);

    /* CAN0 run to normal mode */
    CANFD_RunToNormal(CANFD0, TRUE);

    /* 50ms delay after recovery process */
    TIMER_Delay(TIMER0,50000);

    /* Check if recovery was successful by verifying Bus-Off status */
    if (CANFD0->PSR & CANFD_PSR_BO_Msk)
    {
        /* Still in Bus-Off state, recovery failed */
        sysprintf("CAN Bus-Off recovery failed. Still in Bus-Off state.\n");
        /* Recovery failed */
        return 0;
    }
    else
    {
        /* Recovery successful, clear Bus-Off flag */
        g_u8BusOffFlag = 0;
        g_u32BusOffRecoveryCounter++;
        sysprintf("CAN Bus-Off recovery completed. Recovery count: %u\n", g_u32BusOffRecoveryCounter);
        /* Recovery successful */
        return 1;
    }
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
    sysprintf("|   CAN0_TX|   CAN     |  CAN_H  |    CAN    |CAN_TX            |\n");
    sysprintf("|          |Transceiver|         |Transceiver|                  |\n");
    sysprintf("|  <------ |           |<------->|           |------>           |\n");
    sysprintf("|   CAN0_RX|           |  CAN_L  |           |CAN_RX            |\n");
    sysprintf("|          |-----------|         |-----------|                  |\n");
    sysprintf("|                                                               |\n");
    sysprintf("+---------------------------------------------------------------+\n\n");

    /*Get the CAN configuration value*/
    CANFD_GetDefaultConfig(&sCANFD_Config, CANFD_OP_CAN_MODE);
    sCANFD_Config.sBtConfig.sNormBitRate.u32BitRate = 1000000;
    sCANFD_Config.sBtConfig.sDataBitRate.u32BitRate = 0;

    /*Reset CAN FD IP*/
    SYS_UnlockReg();
    SYS_ResetModule(CANFD0_RST);
    SYS_LockReg();

    /*Open the CAN feature*/
    CANFD_Open(CANFD0, &sCANFD_Config);

    /* receive 0x110~0x11F in CAN rx fifo0 buffer by setting mask 0 */
    CANFD_SetSIDFltr(CANFD0, 0, CANFD_RX_FIFO0_STD_MASK(0x110, 0x7F0));
    /* receive 0x220 ~ 0x22F in CAN rx fifo0 buffer by setting mask 1 */
    CANFD_SetSIDFltr(CANFD0, 1, CANFD_RX_FIFO0_STD_MASK(0x22F, 0x7FF));
    /* receive 0x333 in CAN rx fifo0 buffer by setting mask 2 */
    CANFD_SetSIDFltr(CANFD0, 2, CANFD_RX_FIFO0_STD_MASK(0x333, 0x7FF));

    /* receive 0x220 (29-bit id) in CAN rx fifo0 buffer by setting mask 0 */
    CANFD_SetXIDFltr(CANFD0, 0, CANFD_RX_FIFO0_EXT_MASK_LOW(0x220), CANFD_RX_FIFO0_EXT_MASK_HIGH(0x1FFFFFF0));
    /* receive 0x3333 (29-bit id) in CAN rx fifo0 buffer by setting mask 1 */
    CANFD_SetXIDFltr(CANFD0, 1, CANFD_RX_FIFO0_EXT_MASK_LOW(0x3333), CANFD_RX_FIFO0_EXT_MASK_HIGH(0x1FFFFFFF));
    /* receive 0x4444 (29-bit id) in CAN rx fifo0 buffer by setting mask 2 */
    CANFD_SetXIDFltr(CANFD0, 2, CANFD_RX_FIFO0_EXT_MASK_LOW(0x44444), CANFD_RX_FIFO0_EXT_MASK_HIGH(0x1FFFFFFF));
    /* Enable Standard ID and  Extended ID Filter as RX FOFI0*/
    CANFD_SetGFC(CANFD0, eCANFD_ACC_NON_MATCH_FRM_RX_FIFO0, eCANFD_ACC_NON_MATCH_FRM_RX_FIFO0, 1, 1);
    /* Enable RX fifo0 new message interrupt using interrupt line 0. */
    IRQ_SetHandler((IRQn_ID_t)CANFD00_IRQn, CANFD00_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)CANFD00_IRQn);
    /* Enable RX fifo0 new message interrupt, Bus-Off interrupt, Error Warning and Error Passive interrupts using interrupt line 0. */
    CANFD_EnableInt(CANFD0, (CANFD_IE_RF0NE_Msk | CANFD_IE_BOE_Msk | CANFD_IE_EWE_Msk | CANFD_IE_EPE_Msk), 0, 0, 0);    /* CAN FD0 Run to Normal mode  */
    CANFD_RunToNormal(CANFD0, TRUE);
}


/*---------------------------------------------------------------------------------------------------------*/
/*                             CAN Tx Rx Interrupt Function Test                                           */
/*---------------------------------------------------------------------------------------------------------*/
void CAN_TxRxINTTest(void)
{
    uint8_t u8Item;

    /* CAN interface initialization*/
    CAN_Init();

    sysprintf("+--------------------------------------------------------------------------+\n");
    sysprintf("|                      CAN Function Test                                   |\n");
    sysprintf("+--------------------------------------------------------------------------+\n");
    sysprintf("|  Description :                                                           |\n");
    sysprintf("|    The sample code needs two boards. One is master(CAN transmitter) and  |\n");
    sysprintf("|    the other is slave(CAN receiver). Master will send 6 messages with    |\n");
    sysprintf("|    different sizes of data and ID to the slave. Slave will check if      |\n");
    sysprintf("|    received data is correct after getting 6 messages data.               |\n");
    sysprintf("|    Bus-Off recovery feature is enabled for error handling.               |\n");
    sysprintf("|  Please select Master or Slave test                                      |\n");
    sysprintf("|  [0] Master(CAN transmitter)    [1] Slave(CAN receiver)                  |\n");
    sysprintf("+--------------------------------------------------------------------------+\n\n");

    u8Item = sysgetchar();

    if (u8Item == '0')
    {
        CAN_TxTest();
    }
    else
    {
        CAN_RxTest();
    }

    sysprintf("CANFD Sample Code End.\n");

    if (g_u32BusOffRecoveryCounter > 0)
    {
        sysprintf("Total Bus-Off recovery cycles: %u\n", g_u32BusOffRecoveryCounter);
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

    /* print a note to terminal */
    sysprintf("\nCAN bus communication example\r\n");

    CAN_TxRxINTTest();

    while (1) {}
}

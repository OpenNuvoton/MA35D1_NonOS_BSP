/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Transmit and receive data in UART RS485 mode.
 *
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#define RXBUFSIZE 1024

#define IS_USE_RS485NMM   1      //1:Select NMM_Mode , 0:Select AAD_Mode
#define MATCH_ADDRSS1     0xC0
#define MATCH_ADDRSS2     0xA2
#define UNMATCH_ADDRSS1   0xB1
#define UNMATCH_ADDRSS2   0xD3

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_u8SendData[12] = {0};

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void RS485_HANDLE(void);
void RS485_SendAddressByte(uint8_t u8data);
void RS485_SendDataByte(uint8_t *pu8TxBuf, uint32_t u32WriteBytes);
void RS485_9bitModeMaster(void);
void RS485_9bitModeSlave(void);
void RS485_FunctionTest(void);


void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HXT */
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;

    /* Waiting clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Enable IP clock */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(2));
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL2_UART1SEL_HXT, CLK_CLKDIV1_UART1(1));
    CLK_EnableModuleClock(UART1_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set multi-function pins for Debug UART RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);
    /* Set multi-function pins for UART1 */
    SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA0MFP_Msk | SYS_GPA_MFPL_PA1MFP_Msk | SYS_GPA_MFPL_PA2MFP_Msk | SYS_GPA_MFPL_PA3MFP_Msk);
    SYS->GPA_MFPL |= SYS_GPA_MFPL_PA0MFP_UART1_nCTS | SYS_GPA_MFPL_PA1MFP_UART1_nRTS |
                     SYS_GPA_MFPL_PA2MFP_UART1_RXD | SYS_GPA_MFPL_PA3MFP_UART1_TXD;

    /* Lock protected registers */
    SYS_LockReg();
}

void UART_Init()
{
    UART_Open(UART0, 115200);
}

void UART1_Init()
{
    UART_Open(UART1, 115200);
}


int main(void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init UART for sysprintf */
    UART_Init();

    /* Init UART1 */
    UART1_Init();

    sysprintf("\n\nCPU @ %dHz\n", SystemCoreClock);

    sysprintf("+------------------------+\n");
    sysprintf("| RS485 function test     |\n");
    sysprintf("+------------------------+\n");

    RS485_FunctionTest();

    while(1);
}

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 1 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void UART1_IRQHandler(void)
{
    RS485_HANDLE();
}

/*---------------------------------------------------------------------------------------------------------*/
/* RS485 Callback function                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void RS485_HANDLE()
{
    volatile uint32_t addr=0;
    volatile uint32_t regRX=0xFF;
    volatile uint32_t u32IntSts = UART1->INTSTS;

    if((u32IntSts & UART_INTSTS_RLSINT_Msk)&&(u32IntSts & UART_INTSTS_RDAINT_Msk))           /* RLS INT & RDA INT */  //For RS485 Detect Address
    {
        if(UART1->FIFOSTS & UART_FIFOSTS_ADDRDETF_Msk)   /* ADD_IF, RS485 mode */
        {
            addr = UART1->DAT;
            UART_RS485_CLEAR_ADDR_FLAG(UART1);        /* clear ADD_IF flag */
            sysprintf("\nAddr=0x%x,Get:",addr);

#if (IS_USE_RS485NMM ==1) //RS485_NMM
            /* if address match, enable RX to receive data, otherwise to disable RX. */
            /* In NMM mode,user can decide multi-address filter. In AAD mode,only one address can set */
            if (( addr == MATCH_ADDRSS1)||( addr == MATCH_ADDRSS2))
            {
                UART1->FIFO &= ~ UART_FIFO_RXOFF_Msk;  /* Enable RS485 RX */
            }
            else
            {
                sysprintf("\n");
                UART1->FIFO |= UART_FIFO_RXOFF_Msk;      /* Disable RS485 RX */
                UART1->FIFO |= UART_FIFO_RXRST_Msk;      /* Clear data from RX FIFO */
            }
#endif
        }
    }
    else if((u32IntSts & UART_INTSTS_RDAINT_Msk) || (u32IntSts & UART_INTSTS_RXTOINT_Msk) )     /* Rx Ready or Time-out INT*/
    {
        /* Handle received data */
        sysprintf("%2d,",UART1->DAT);
    }
    else if(u32IntSts & UART_INTSTS_BUFEINT_Msk)     /* Buffer Error INT */
    {
        sysprintf("\nBuffer Error...\n");
        UART_ClearIntFlag(UART1, UART_INTSTS_BUFEINT_Msk);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  RS485 Transmit Control  (Address Byte: Parity Bit =1 , Data Byte:Parity Bit =0)                        */
/*---------------------------------------------------------------------------------------------------------*/
void RS485_SendAddressByte(uint8_t u8data)
{
    UART_SetLineConfig(UART1, 0, UART_WORD_LEN_8, UART_PARITY_MARK, UART_STOP_BIT_1);
    UART_WRITE(UART1,u8data);
}

void RS485_SendDataByte(uint8_t *pu8TxBuf, uint32_t u32WriteBytes)
{
    UART_SetLineConfig(UART1, 0, UART_WORD_LEN_8, UART_PARITY_SPACE, UART_STOP_BIT_1);
    UART_Write(UART1,pu8TxBuf,u32WriteBytes);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  RS485 Transmit Test                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void RS485_9bitModeMaster()
{
    int32_t i32;
    uint8_t g_u8SendDataGroup1[10] = {0};
    uint8_t g_u8SendDataGroup2[10] = {0};
    uint8_t g_u8SendDataGroup3[10] = {0};
    uint8_t g_u8SendDataGroup4[10] = {0};

    sysprintf("\n\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("|               RS485 9-bit Master Test                     |\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("| The function will send different address with 10 data bytes|\n");
    sysprintf("| to test RS485 9-bit mode. Please connect TX/RX to another |\n");
    sysprintf("| board and wait its ready to receive.                      |\n");
    sysprintf("| Press any key to start...                                 |\n");
    sysprintf("+-----------------------------------------------------------+\n\n");
    sysgetchar();

    /* Set RS485-Master as AUD mode*/
    UART1->MODEM &= ~UART_MODEM_RTSACTLV_Msk;
    UART_SelectRS485Mode(UART1, UART_ALTCTL_RS485AUD_Msk, 0);

    /* Prepare Data to transmit*/
    for(i32=0; i32<10; i32++)
    {
        g_u8SendDataGroup1[i32] = i32;
        g_u8SendDataGroup2[i32] = i32+10;
        g_u8SendDataGroup3[i32] = i32+20;
        g_u8SendDataGroup4[i32] = i32+30;
    }
    /* Send For different Address and data for test */
    sysprintf("Send Address %x and data 0~9\n",MATCH_ADDRSS1);
    RS485_SendAddressByte( MATCH_ADDRSS1 );
    RS485_SendDataByte(g_u8SendDataGroup1,10);

    sysprintf("Send Address %x and data 10~19\n",UNMATCH_ADDRSS1);
    RS485_SendAddressByte( UNMATCH_ADDRSS1 );
    RS485_SendDataByte(g_u8SendDataGroup2,10);

    sysprintf("Send Address %x and data 20~29\n",MATCH_ADDRSS2);
    RS485_SendAddressByte( MATCH_ADDRSS2 );
    RS485_SendDataByte(g_u8SendDataGroup3,10);

    sysprintf("Send Address %x and data 30~39\n",UNMATCH_ADDRSS2);
    RS485_SendAddressByte( UNMATCH_ADDRSS2 );
    RS485_SendDataByte(g_u8SendDataGroup4,10);
    sysprintf("Transfer Done\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/*  RS485 Receive Test  (IS_USE_RS485NMM: 0:AAD  1:NMM)                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void RS485_9bitModeSlave()
{
    /* Set Data Format*/ /* Only need parity enable whenever parity ODD/EVEN */
    UART_SetLineConfig(UART1, 0, UART_WORD_LEN_8, UART_PARITY_EVEN, UART_STOP_BIT_1);

    /* Set RX Trigger Level = 1 */
    UART1->FIFO &= ~UART_FIFO_RFITL_Msk;
    UART1->FIFO |= UART_FIFO_RFITL_1BYTE;

#if(IS_USE_RS485NMM == 1)
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("|    Normal Multidrop Operation Mode                        |\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("| The function is used to test 9-bit slave mode.            |\n");
    sysprintf("| Only Address %2x and %2x,data can receive                  |\n",MATCH_ADDRSS1,MATCH_ADDRSS2);
    sysprintf("+-----------------------------------------------------------+\n");

    /* Set RX_DIS enable before set RS485-NMM mode */
    UART1->FIFO |= UART_FIFO_RXOFF_Msk;

    /* Set RS485-NMM Mode */
    UART1->MODEM &= ~UART_MODEM_RTSACTLV_Msk;
    UART_SelectRS485Mode(UART1, UART_ALTCTL_RS485NMM_Msk|UART_ALTCTL_ADDRDEN_Msk, 0);

#else
    sysprintf("Auto Address Match Operation Mode\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("| The function is used to test 9-bit slave mode.            |\n");
    sysprintf("|    Auto Address Match Operation Mode                      |\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("|Only Address %2x,data can receive                          |\n",MATCH_ADDRSS1);
    sysprintf("+-----------------------------------------------------------+\n");

    /* Set RS485-AAD Mode and address match is 0xC0 */
    UART_SelectRS485Mode(UART1, UART_ALTCTL_RS485AAD_Msk|UART_ALTCTL_ADDRDEN_Msk, MATCH_ADDRSS1);
#endif

    /* Enable RDA\RLS\Time-out Interrupt  */
    UART_ENABLE_INT(UART1, (UART_INTEN_RDAIEN_Msk | UART_INTEN_RLSIEN_Msk));
    IRQ_SetHandler((IRQn_ID_t)UART1_IRQn, UART1_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)UART1_IRQn);

    sysprintf("Ready to receive data...(Press any key to stop test)\n");
    sysgetchar();

    UART_DISABLE_INT(UART1, (UART_INTEN_RDAIEN_Msk | UART_INTEN_RLSIEN_Msk | UART_INTEN_RXTOIEN_Msk));

    /* Set UART Function */
    UART_Open(UART1, 115200);
    sysprintf("\n\nEnd test\n");
}


/*---------------------------------------------------------------------------------------------------------*/
/*  RS485 Function Test                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void RS485_FunctionTest()
{
    uint32_t u32Item;
    sysprintf("\n\n");
    sysprintf("+-------------------------------------------------------------+\n");
    sysprintf("|            IO Setting                                       |\n");
    sysprintf("+-------------------------------------------------------------+\n");
    sysprintf("|  ______                        _______                      |\n");
    sysprintf("| |      |                      |       |                     |\n");
    sysprintf("| |Master|---TXD0 <====> RXD0---| Slave |                     |\n");
    sysprintf("| |      |---RTS0 <====> RTS0---|       |                     |\n");
    sysprintf("| |______|                      |_______|                     |\n");
    sysprintf("|                                                             |\n");
    sysprintf("+-------------------------------------------------------------+\n\n");
    sysprintf("+-------------------------------------------------------------+\n");
    sysprintf("|       RS485 Function Test                                   |\n");
    sysprintf("+-------------------------------------------------------------+\n");
    sysprintf("|  Please select Master or Slave test                         |\n");
    sysprintf("|  [0] Master    [1] Slave                                    |\n");
    sysprintf("+-------------------------------------------------------------+\n\n");
    u32Item = sysgetchar();

    /*
        The sample code is used to test RS485 9-bit mode and needs
        two Module test board to complete the test.
        Master:
            1.Set AUD mode and HW will control RTS pin. LEV_RTS is set to '0'.
            2.Master will send four different address with 10 bytes data to test Slave.
            3.Address bytes : the parity bit should be '1'. (Set UA_LCR = 0x2B)
            4.Data bytes : the parity bit should be '0'. (Set UA_LCR = 0x3B)
            5.RTS pin is low in idle state. When master is sending,
              RTS pin will be pull high.

        Slave:
            1.Set AAD and AUD mode firstly. LEV_RTS is set to '0'.
            2.The received byte, parity bit is '1' , is considered "ADDRESS".
            3.The received byte, parity bit is '0' , is considered "DATA".  (Default)
            4.AAD: The slave will ignore any data until ADDRESS match ADDR_MATCH value.
              When RLS and RDA interrupt is happened,it means the ADDRESS is received.
              Check if RS485_ADD_DETF is set and read UA_RBR to clear ADDRESS stored in rx_fifo.

              NMM: The slave will ignore data byte until disable RX_DIS.
              When RLS and RDA interrupt is happened,it means the ADDRESS is received.
              Check the ADDRESS is match or not by user in UART_IRQHandler.
              If the ADDRESS is match,clear RX_DIS bit to receive data byte.
              If the ADDRESS is not match,set RX_DIS bit to avoid data byte stored in FIFO.
    */

    if(u32Item =='0')
        RS485_9bitModeMaster();
    else
        RS485_9bitModeSlave();
}



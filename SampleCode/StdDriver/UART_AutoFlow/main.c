/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Transmit and receive data using auto flow control.
 *
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"


#define RXBUFSIZE 256

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_u8SendData[12] = {0};
uint8_t g_u8RecData[RXBUFSIZE]  = {0};

volatile uint32_t g_u32comRbytes = 0;
volatile uint32_t g_u32comRhead  = 0;
volatile uint32_t g_u32comRtail  = 0;
volatile int32_t g_bWait         = TRUE;
volatile int32_t g_i32pointer = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void UART_TEST_HANDLE(void);
void AutoFlow_FunctionTest(void);


void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HXT */
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;

    /* Waiting clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Enable IP clock */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL2_UART1SEL_HXT, CLK_CLKDIV1_UART1(1));
    CLK_EnableModuleClock(UART1_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPE multi-function pins for UART0 RXD and TXD */
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
    /* Init UART0 for sysprintf */
    UART_Init();

    /* Init UART1 */
    UART1_Init();

    sysprintf("+--------------------------+\n");
    sysprintf("|  Auto-Flow function test  |\n");
    sysprintf("+--------------------------+\n");

    AutoFlow_FunctionTest();

    while(1);
}

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void UART1_IRQHandler(void)
{
    UART_TEST_HANDLE();
}

/*---------------------------------------------------------------------------------------------------------*/
/* UART Callback function                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void UART_TEST_HANDLE()
{
    uint8_t u8InChar=0xFF;
    uint32_t u32IntSts= UART1->INTSTS;

    if(u32IntSts & UART_INTSTS_RDAINT_Msk)
    {
        sysprintf("\nInput:");

        /* Get all the input characters */
        while( (!UART_GET_RX_EMPTY(UART1)) )
        {
            /* Get the character from UART Buffer */
            u8InChar = UART_READ(UART1);    /* Rx trigger level is 1 byte*/

            g_i32pointer++;

            sysprintf("%c ", u8InChar);

            if(u8InChar == '0')
            {
                g_bWait = FALSE;
            }

            /* Check if buffer full */
            if(g_u32comRbytes < RXBUFSIZE)
            {
                /* Enqueue the character */
                g_u8RecData[g_u32comRtail] = u8InChar;
                g_u32comRtail = (g_u32comRtail == (RXBUFSIZE-1)) ? 0 : (g_u32comRtail+1);
                g_u32comRbytes++;
            }
        }
        sysprintf("\nTransmission Test:");
    }

    if(u32IntSts & UART_INTSTS_THREINT_Msk)
    {
        uint16_t tmp;
        tmp = g_u32comRtail;
        if(g_u32comRhead != tmp)
        {
            u8InChar = g_u8RecData[g_u32comRhead];
            UART_WRITE(UART1,u8InChar);
            g_u32comRhead = (g_u32comRhead == (RXBUFSIZE-1)) ? 0 : (g_u32comRhead+1);
            g_u32comRbytes--;
        }
    }

    if(UART1->FIFOSTS & (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk))
    {
        UART1->FIFOSTS = (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk);
    }

}


/*---------------------------------------------------------------------------------------------------------*/
/*  UART Function Test                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void AutoFlow_FunctionTest()
{
    uint8_t u8Item;
    uint32_t u32i;
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("|     Pin Configure                                         |\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("|  _______                                      _______     |\n");
    sysprintf("| |       |                                    |       |    |\n");
    sysprintf("| |Master |---TXD1    <============>    RXD1---| Slave |    |\n");
    sysprintf("| |       |---RTS1    <============>    CTS1---|       |    |\n");
    sysprintf("| |_______|---CTS1    <============>    RTS1---|_______|    |\n");
    sysprintf("|                                                           |\n");
    sysprintf("+-----------------------------------------------------------+\n\n");

    /* Set Trigger Level */
    UART1->MODEM |= UART_MODEM_RTSACTLV_Msk; // Set RTS high level trigger
    UART1->MODEMSTS |= UART_MODEMSTS_CTSACTLV_Msk; // Set CTS high level trigger
    UART1->FIFO = (UART1->FIFO &~ UART_FIFO_RTSTRGLV_Msk) | UART_FIFO_RTSTRGLV_14BYTES;

    /* Enable RTS and CTS auto flow control */
    UART1->INTEN |= UART_INTEN_ATORTSEN_Msk | UART_INTEN_ATOCTSEN_Msk;

    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("|       AutoFlow Function Test                              |\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("|  Description :                                            |\n");
    sysprintf("|    The sample code needs two boards. One is Master and    |\n");
    sysprintf("|    the other is slave. Master will send 1k bytes data     |\n");
    sysprintf("|    to slave.Slave will check if received data is correct  |\n");
    sysprintf("|    after getting 1k bytes data.                           |\n");
    sysprintf("|  Please select Master or Slave test                       |\n");
    sysprintf("|  [0] Master    [1] Slave                                  |\n");
    sysprintf("+-----------------------------------------------------------+\n\n");
    u8Item = sysgetchar();


    if(u8Item=='0')
    {
        for(u32i=0; u32i<(RXBUFSIZE-1); u32i++)
        {
            UART_WRITE(UART1,((u32i+1)&0xFF));

            while(!(UART1->FIFOSTS & UART_FIFOSTS_TXEMPTYF_Msk));
        }
        sysprintf("\n Transmit Done\n");
    }
    else
    {
        g_i32pointer = 0;

        /* Enable RDA\RLS\RTO Interrupt  */
        UART_ENABLE_INT(UART1, (UART_INTEN_RDAIEN_Msk | UART_INTEN_RLSIEN_Msk | UART_INTEN_RXTOIEN_Msk));

        /* Set RX Trigger Level = 8 */
        UART1->FIFO = (UART1->FIFO &~ UART_FIFO_RFITL_Msk) | UART_FIFO_RFITL_8BYTES;

        /* Set Timeout time 0x3E bit-time */
        UART_SetTimeoutCnt(UART1,0x3E);

        IRQ_SetHandler((IRQn_ID_t)UART1_IRQn, UART1_IRQHandler);
        IRQ_Enable ((IRQn_ID_t)UART1_IRQn);

        sysprintf("Starting to receive %d bytes data...\n", RXBUFSIZE);

        while(g_i32pointer<(RXBUFSIZE-1))
        {
            //sysprintf("%d\r",g_i32pointer);
        }

        /* Compare Data */
        for(u32i=0; u32i!=(RXBUFSIZE-1); u32i++)
        {
            if(g_u8RecData[u32i] != ((u32i+1)&0xFF) )
            {
                sysprintf("Compare Data Failed\n");
                while(1);
            }
        }
        sysprintf("\n Receive OK & Check OK\n");
    }

    IRQ_Disable(UART1_IRQn);

    UART_DISABLE_INT(UART1, (UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk | UART_INTEN_RXTOIEN_Msk));

}



/**************************************************************************//**
 * @file     main.c
 *
 * @brief
 *           Transmit and receive data from PC terminal through RS232 interface.
 *
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include "stdio.h"
#include "NuMicro.h"

#define RXBUFSIZE   1024

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_u8RecData[RXBUFSIZE]  = {0};

volatile uint32_t g_u32comRbytes = 0;
volatile uint32_t g_u32comRhead  = 0;
volatile uint32_t g_u32comRtail  = 0;
volatile int32_t g_bWait         = TRUE;

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void UART_TEST_HANDLE(void);
void UART_FunctionTest(void);


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

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Lock protected registers */
    SYS_LockReg();
}

void UART_Init()
{
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/* UART Test Sample                                                                                        */
/* Test Item                                                                                               */
/* It sends the received data to HyperTerminal.                                                            */
/*---------------------------------------------------------------------------------------------------------*/

int32_t main(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for sysprintf and test */
    UART_Init();

    sysprintf("\nUART Sample Program\n");

    /* UART sample function */
    UART_FunctionTest();

    while(1);

}

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void UART0_IRQHandler(void)
{
    UART_TEST_HANDLE();
}

/*---------------------------------------------------------------------------------------------------------*/
/* UART Callback function                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void UART_TEST_HANDLE()
{
    uint8_t u8InChar = 0xFF;
    uint32_t u32IntSts = UART0->INTSTS;

    if((u32IntSts & UART_INTSTS_RDAINT_Msk) || (u32IntSts & UART_INTSTS_RXTOINT_Msk))
    {
        sysprintf("\nInput:");

        /* Get all the input characters */
        while(UART_GET_RX_EMPTY(UART0) == 0)
        {
            /* Get the character from UART Buffer */
            u8InChar = UART_READ(UART0);

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
                g_u32comRtail = (g_u32comRtail == (RXBUFSIZE - 1)) ? 0 : (g_u32comRtail + 1);
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
            while(UART_IS_TX_FULL(UART0));  /* Wait Tx is not full to transmit data */
            UART_WRITE(UART0, u8InChar);
            g_u32comRhead = (g_u32comRhead == (RXBUFSIZE - 1)) ? 0 : (g_u32comRhead + 1);
            g_u32comRbytes--;
        }
    }

    if(UART0->FIFOSTS & (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk))
    {
        UART0->FIFOSTS = (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  UART Function Test                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void UART_FunctionTest()
{
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("|  UART Function Test                                       |\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("|  Description :                                            |\n");
    sysprintf("|    The sample code will print input char on terminal      |\n");
    sysprintf("|    Please enter any to start     (Press '0' to exit)      |\n");
    sysprintf("+-----------------------------------------------------------+\n");

    /*
        Using a RS232 cable to connect UART0 and PC.
        UART0 is set to debug port. UART0 is enable RDA and RLS interrupt.
        When inputting char to terminal screen, RDA interrupt will happen and
        UART0 will print the received char on screen.
    */

    UART_SetTimeoutCnt(UART0, 0x10); // Set Rx Time-out counter

    // Set RX FIFO Interrupt Trigger Level
    UART0->FIFO &= ~ UART_FIFO_RFITL_Msk;
    UART0->FIFO |= UART_FIFO_RFITL_4BYTES;

    /* Enable UART RDA/THRE/Time-out interrupt */
    IRQ_SetHandler((IRQn_ID_t)UART0_IRQn, UART0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)UART0_IRQn);
    UART_EnableInt(UART0, (UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk | UART_INTEN_RXTOIEN_Msk));
    while(g_bWait);

    /* Disable UART RDA/THRE/Time-out interrupt */
    UART_DisableInt(UART0, (UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk | UART_INTEN_RXTOIEN_Msk));

    // Reset RX FIFO Interrupt Trigger Level
    UART0->FIFO &= ~ UART_FIFO_RFITL_Msk;

    g_bWait = TRUE;
    sysprintf("\nUART Sample Demo End.\n");

}





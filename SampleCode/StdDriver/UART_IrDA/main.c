/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Transmit and receive UART data in UART IrDA mode.
 *
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

#define RXBUFSIZE 1024

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
void IrDA_FunctionTxTest(void);
void IrDA_FunctionRxTest(void);
void IrDA_FunctionTest(void);


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
    SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA2MFP_Msk | SYS_GPA_MFPL_PA3MFP_Msk);
    SYS->GPA_MFPL |= SYS_GPA_MFPL_PA2MFP_UART1_RXD | SYS_GPA_MFPL_PA3MFP_UART1_TXD;

    /* Lock protected registers */
    SYS_LockReg();
}

void UART_Init()
{
    UART_Open(UART0, 115200);
}

void UART1_Init()
{
    UART_Open(UART1, 57600);
}

int main(void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init UART0 for sysprintf */
    UART_Init();

    /* Init UART1 */
    UART1_Init();


    sysprintf("\n\nCPU @ %dHz\n", SystemCoreClock);

    sysprintf("+------------------------+\n");
    sysprintf("| IrDA function test |\n");
    sysprintf("+------------------------+\n");

    IrDA_FunctionTest();

    while(1);
}

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void UART1_IRQHandler(void)
{

}

/*---------------------------------------------------------------------------------------------------------*/
/*  IrDA Function Test                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void IrDA_FunctionTest()
{
    uint8_t u8item;

    sysprintf("+-------------------------------------------------------------+\n");
    sysprintf("|     Pin Configure                                           |\n");
    sysprintf("+-------------------------------------------------------------+\n");
    sysprintf("|  ______                                      _______        |\n");
    sysprintf("| |      |                                    |       |       |\n");
    sysprintf("| |Master|---TXD0(pin46) <====> RXD0(pin45)---|Slave  |       |\n");
    sysprintf("| |      |                                    |       |       |\n");
    sysprintf("| |______|                                    |_______|       |\n");
    sysprintf("|                                                             |\n");
    sysprintf("+-------------------------------------------------------------+\n");

    sysprintf("\n\n");
    sysprintf("+-------------------------------------------------------------+\n");
    sysprintf("|     IrDA Function Test                                      |\n");
    sysprintf("+-------------------------------------------------------------+\n");
    sysprintf("|  Description :                                              |\n");
    sysprintf("|    The sample code needs two boards. One is Master and      |\n");
    sysprintf("|    the other is slave.  Master will send data based on      |\n");
    sysprintf("|    terminal input and Slave will sysprintf received data on    |\n");
    sysprintf("|    terminal screen.                                         |\n");
    sysprintf("|  Please select Master or Slave test                         |\n");
    sysprintf("|  [0] Master    [1] Slave                                    |\n");
    sysprintf("+-------------------------------------------------------------+\n\n");
    u8item = sysgetchar();

    if(u8item=='0')
        IrDA_FunctionTxTest();
    else
        IrDA_FunctionRxTest();

    sysprintf("\nIrDA Sample Code End.\n");

}

/*---------------------------------------------------------------------------------------------------------*/
/*  IrDA Function Transmit Test                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void IrDA_FunctionTxTest()
{
    uint8_t u8OutChar;

    sysprintf("\n\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("|     IrDA Function Tx Mode Test                            |\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("| 1). Input char by UART terminal.                         |\n");
    sysprintf("| 2). UART will send a char according to step 1.           |\n");
    sysprintf("| 3). Return step 1. (Press '0' to exit)                    |\n");
    sysprintf("+-----------------------------------------------------------+\n\n");

    sysprintf("\nIRDA Sample Code Start. \n");

    /* Select IrDA Tx mode and set baud rate */
    UART_SelectIrDAMode(UART1, 57600, TRUE); // TRUE is TX mode

    /* Wait Terminal input to send data to UART TX pin */
    do
    {
        u8OutChar = sysgetchar();
        sysprintf("   Input: %c , Send %c out\n",u8OutChar,u8OutChar);
        UART_WRITE(UART1,u8OutChar);
    }
    while(u8OutChar !='0');

}

/*---------------------------------------------------------------------------------------------------------*/
/*  IrDA Function Receive Test                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
void IrDA_FunctionRxTest()
{
    uint8_t u8InChar=0xFF;

    sysprintf("\n\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("|     IrDA Function Rx Mode Test                            |\n");
    sysprintf("+-----------------------------------------------------------+\n");
    sysprintf("| 1). Polling RDA_Flag to check data input though UART     |\n");
    sysprintf("| 2). If received data is '0', the program will exit.       |\n");
    sysprintf("|     Otherwise, print received data on terminal            |\n");
    sysprintf("+-----------------------------------------------------------+\n\n");

    /* Select IrDA Rx mode and set baud rate */
    UART_SelectIrDAMode(UART1, 57600, FALSE); // FALSE is RX mode

    sysprintf("Waiting...\n");

    /* Use polling method to wait master data */
    do
    {
        if( UART_IS_RX_READY(UART1))
        {
            u8InChar = UART_READ(UART1);
            sysprintf("   Input: %c \n",u8InChar);
        }
    }
    while(u8InChar !='0');

}


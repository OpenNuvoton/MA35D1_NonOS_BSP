/**************************************************************************//**
 * @file     main.c
 *
 * @brief
 *           The modbus master demo code.
 *
 * @copyright (C) 2025 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include "stdio.h"
#include "NuMicro.h"
#include "modbus_master.h"
/*---------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------*/
uint16_t  Input_Result[2];
volatile uint32_t u32_TimeTick = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void HAL_Delay(uint32_t Delay);
void SYS_Init(void);
void Timer_Init(void);
void TMR0_IRQHandler(void);
void UART1_IRQHandler(void);
extern void Modbus_Master_Rece_Handler(void);

uint32_t HAL_GetTick(void)
{
    return u32_TimeTick;
}

void HAL_Delay(uint32_t Delay)
{
    uint32_t tickstart = HAL_GetTick();
    uint32_t wait = Delay;

    while ((HAL_GetTick() - tickstart) < wait);
}

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
    CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL2_UART1SEL_HXT, CLK_CLKDIV1_UART1(2));
    CLK_EnableModuleClock(UART1_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
    CLK_EnableModuleClock(TMR0_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set multi-function pins for UART0 */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);
    /* Set multi-function pins for UART1 */
    SYS->GPA_MFPL &= ~(SYS_GPA_MFPL_PA2MFP_Msk | SYS_GPA_MFPL_PA3MFP_Msk);
    SYS->GPA_MFPL |= SYS_GPA_MFPL_PA2MFP_UART1_RXD | SYS_GPA_MFPL_PA3MFP_UART1_TXD;

    /* Lock protected registers */
    SYS_LockReg();
}

void Timer_Init(void)
{
    /* Open Timer0 in periodic mode, enable interrupt and 1000 interrupt tick per second */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);

    /* Enable Timer0 interrupt */
    TIMER_EnableInt(TIMER0);

    /* Enable Timer0 NVIC */
    IRQ_SetHandler((IRQn_ID_t)TMR0_IRQn, TMR0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR0_IRQn);

    /* Start Timer0 counting */
    TIMER_Start(TIMER0);
}

void UART_Init()
{
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

void UART1_Init()
{
    /* Configure UART1 */
    UART_Open(UART1, 57600);
    UART_EnableInt(UART1, UART_INTEN_RDAIEN_Msk);
    IRQ_SetHandler((IRQn_ID_t)UART1_IRQn, UART1_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)UART1_IRQn);
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

    sysprintf("\nModBus Demo\n");

    /* Init function */
    UART1_Init();
    Timer_Init();

    ModbusMaster_begin();

    while (1)
    {
        uint8_t result;
        result = ModbusMaster_readInputRegisters(0x01, 0x2, 2); //master send readInputRegisters command to slave

        if (result == 0x00)
        {
            //user can read input register from adress 0x02 and get 2 byte context
            Input_Result[0] = ModbusMaster_getResponseBuffer(0x00);
            Input_Result[1] = ModbusMaster_getResponseBuffer(0x01);
        }

        HAL_Delay(1000);
    }

}

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle Timer 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void TMR0_IRQHandler(void)
{
    if (TIMER_GetIntFlag(TIMER0) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);

        u32_TimeTick++;
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 1 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void UART1_IRQHandler(void)
{
    uint32_t u32IntSts = UART1->INTSTS;

    if (u32IntSts & UART_INTSTS_RDAINT_Msk)
    {
        Modbus_Master_Rece_Handler();
    }
}


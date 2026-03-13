/*************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    A project template for MA35D1 MPU.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2026 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "main.h"

static volatile uint32_t gTMR0DelayCnt = 0;
static volatile uint32_t gTMR1DelayCnt = 0;

/*--------------------------------------------------------------------------*/
void TMR0_IRQHandler(void)
{
    TIMER_ClearIntFlag(TIMER0);
    if (gTMR0DelayCnt > 0)
        gTMR0DelayCnt--;
}

void TMR1_IRQHandler(void)
{
    TIMER_ClearIntFlag(TIMER1);
    if (gTMR1DelayCnt > 0)
        gTMR1DelayCnt--;
}

void Timer0_Init()
{
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
    IRQ_SetHandler((IRQn_ID_t)TMR0_IRQn, TMR0_IRQHandler);
    IRQ_Enable((IRQn_ID_t)TMR0_IRQn);
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000000);
    TIMER_EnableInt(TIMER0);
    TIMER_Start(TIMER0);
}

void Timer1_Init()
{
    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, 0);
    IRQ_SetHandler((IRQn_ID_t)TMR1_IRQn, TMR1_IRQHandler);
    IRQ_Enable((IRQn_ID_t)TMR1_IRQn);
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER1);
    TIMER_Start(TIMER1);
}

void Timer0_Delay_us(uint32_t ticks_us)
{
    gTMR0DelayCnt = ticks_us;
    while (gTMR0DelayCnt)
    {
        ;
    }
}

void Timer1_Delay_ms(uint32_t ticks_ms)
{
    gTMR1DelayCnt = ticks_ms;
    while (gTMR1DelayCnt)
    {
        ;
    }
}

void QSPI_Init()
{
    /* Configure QSPI_FLASH_PORT as a master, MSB first, 8-bit transaction, QSPI Mode-0 timing, clock low speed */
    QSPI_Open(QSPI_FLASH_PORT, QSPI_MASTER, QSPI_MODE_0, 8, QSPI_CLOCK);

    /* Set GPIO driver strength */
    PD->DSL = 0x333333;

    /* Enable the automatic hardware slave select function. Select the SS pin and configure as low-active. */
    QSPI_EnableAutoSS(QSPI_FLASH_PORT, QSPI_SS, QSPI_SS_ACTIVE_LOW);
}

void UART0_Init()
{
    /* Enable UART0 clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Set multi-function pins */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);
}


void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* System Clock Initial                                                                                    */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable IP clock */
    CLK_SetModuleClock(SYSCK1_MODULE, CLK_CLKSEL0_SYSCK1SEL_SYSPLL, MODULE_NoMsk);
    CLK_SetModuleClock(QSPI0_MODULE, CLK_CLKSEL4_QSPI0SEL_PCLK0, MODULE_NoMsk);
    CLK_EnableModuleClock(QSPI0_MODULE);
    CLK_EnableModuleClock(GPD_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* I/O Multi-function Initial                                                                              */
    /*---------------------------------------------------------------------------------------------------------*/
    SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD0MFP_Msk | SYS_GPD_MFPL_PD1MFP_Msk | SYS_GPD_MFPL_PD2MFP_Msk | SYS_GPD_MFPL_PD3MFP_Msk
                       | SYS_GPD_MFPL_PD4MFP_Msk | SYS_GPD_MFPL_PD5MFP_Msk);
    SYS->GPD_MFPL |= SYS_GPD_MFPL_PD0MFP_QSPI0_SS0 | SYS_GPD_MFPL_PD1MFP_QSPI0_CLK | SYS_GPD_MFPL_PD2MFP_QSPI0_MOSI0 | SYS_GPD_MFPL_PD3MFP_QSPI0_MISO0
                     | SYS_GPD_MFPL_PD4MFP_QSPI0_MOSI1 | SYS_GPD_MFPL_PD5MFP_QSPI0_MISO1;

    /* Lock protected registers */
    SYS_LockReg();
}

/*
 * This is a template project for MA35D1 series MPU. Users could based on this project to create their
 * own application.
 *
 * This template application uses external crystal as HCLK source and configures UART0 to print out
 * "Hello World", users may need to do extra system configuration based on their system design.
 */

int main()
{
    uint32_t clk;

    /* Initialize UART to 115200-8n1 for print message */
    UART0_Init();

    global_timer_init();
    Timer0_Init();
    Timer1_Init();

    /* Initialize System, IP clock and multi-function I/O */
    SYS_Init();

    /* Connect UART to PC, and open a terminal tool to receive following message */
    sysprintf("Winbond W25N04KW SPI NAND Flash sample code ...\n");

    QSPI_Init();

    SPI_NAND_APP_MainRoutine();

    /* Got no where to go, just loop forever */
    while(1);

}

/*** (C) COPYRIGHT 2026 Nuvoton Technology Corp. ***/

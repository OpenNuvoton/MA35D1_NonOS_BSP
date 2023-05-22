/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Implement WDT time-out interrupt event to wake up system.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint32_t g_u32WDTINTCounts = 0;
volatile uint8_t g_u8IsWDTWakeupINT = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Define Functions Prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void WDT_TEST_HANDLE(WDT_T *wdt);


/**
 * @brief       IRQ Handler for WDT0 Interrupt
 *
 * @param       None
 *
 * @details     The WDT0_IRQHandler is default IRQ of WDT0.
 */
void WDT0_IRQHandler(void)
{
    WDT_TEST_HANDLE(WDT0);
}

/**
 * @brief       IRQ Handler for WDT1 Interrupt
 *
 * @param       None
 *
 * @details     The WDT1_IRQHandler is default IRQ of WDT1.
 */
void WDT1_IRQHandler(void)
{
    WDT_TEST_HANDLE(WDT1);
}

void WDT_TEST_HANDLE(WDT_T *wdt)
{
    if (WDT_GET_TIMEOUT_INT_FLAG(wdt) == 1)
    {
        /* Clear WDT time-out interrupt flag */
        WDT_CLEAR_TIMEOUT_INT_FLAG(wdt);

        g_u32WDTINTCounts++;
    }

    if (WDT_GET_TIMEOUT_WAKEUP_FLAG(wdt) == 1)
    {
        /* Clear WDT time-out wake-up flag */
        WDT_CLEAR_TIMEOUT_WAKEUP_FLAG(wdt);

        g_u8IsWDTWakeupINT = 1;
    }
}

void PowerDownFunction(void)
{
    /* Check if all the debug messages are finished */
    UART_WAIT_TX_EMPTY(UART0);

    /* Enter to Power-down mode */
    SYS_NPD();
}

void SYS_Init(void)
{
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable GPIOA peripheral clock */
    CLK_EnableModuleClock(GPA_MODULE);

    /* Select UART module clock source as HXT */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Select WDT0 module clock source as LIRC */
    CLK_SetModuleClock(WDT0_MODULE, CLK_CLKSEL3_WDT0SEL_LIRC, 0);

    /* Select WDT1 module clock source as LIRC */
    CLK_SetModuleClock(WDT1_MODULE, CLK_CLKSEL3_WDT1SEL_LIRC, 0);

    /* To update the variable SystemCoreClock */
    SystemCoreClockUpdate();

    /* Set GPE multi-function pins for UART RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE15MFP_Msk | SYS_GPE_MFPH_PE14MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE15MFP_UART0_RXD | SYS_GPE_MFPH_PE14MFP_UART0_TXD);
}

int main1(void)
{
    __WFI();
}

int main(void)
{
	uint8_t u8Item;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    sysprintf("\n\nCPU @ %d Hz (RSTSTS = 0x%08x)\n", SystemCoreClock, SYS->RSTSTS);
    sysprintf("+-------------------------------------------------+\n");
    sysprintf("|        WDT Time-out Wake-up Sample Code         |\n");
    sysprintf("+-------------------------------------------------+\n");
    sysprintf("| WDT Settings:                                   |\n");
    sysprintf("|    - Clock source is LIRC @ 32 kHz              |\n");
    sysprintf("|    - Time-out interval is 2^16 * WDT clock      |\n");
    sysprintf("|      (around 2.048 s)                           |\n");
    sysprintf("|    - Interrupt enable                           |\n");
    sysprintf("|    - Wake-up function enable                    |\n");
    sysprintf("|                                                 |\n");
    sysprintf("| System will generate a WDT time-out interrupt   |\n");
    sysprintf("| event after 2.048 s, and will be wake up from   |\n");
    sysprintf("| power-down mode also.                           |\n");
    sysprintf("| Use PA.0 high/low period time to check WDT      |\n");
    sysprintf("| time-out wake-up interval.                      |\n");
    sysprintf("+-------------------------------------------------+\n\n");

    sysprintf("Please select WDT0 or WDT1 test\n");
    sysprintf("[0] WDT0    [1] WDT1\n");
    u8Item = sysgetchar() - 0x30;
    sysprintf("\nWDT%d test...\n\n", u8Item);

    /* Use PA.0 to check WDT time-out interval */
    PA->MODE = 0xFFFFFFFD;
    PA0 = 1;

    /* Enable WDT GIC */
    IRQ_SetHandler((IRQn_ID_t)WDT0_IRQn, WDT0_IRQHandler);
    IRQ_Enable((IRQn_ID_t)WDT0_IRQn);
    IRQ_SetHandler((IRQn_ID_t)WDT1_IRQn, WDT1_IRQHandler);
    IRQ_Enable((IRQn_ID_t)WDT1_IRQn);

    /* Because of all bits can be written in WDT Control Register are write-protected;
       To program it needs to disable register protection first. */
    SYS_UnlockReg();

    g_u32WDTINTCounts = g_u8IsWDTWakeupINT = 0;
    if (u8Item == 0)
    {
        /* Configure WDT0 settings and start WDT0 counting */
        WDT_Open(WDT0, WDT_TIMEOUT_2POW16, WDT_RESET_DELAY_18CLK, FALSE, TRUE);

        /* Enable WDT0 interrupt function */
        WDT_EnableInt(WDT0);
    }
    else
    {
        /* Configure WDT1 settings and start WDT1 counting */
        WDT_Open(WDT1, WDT_TIMEOUT_2POW16, WDT_RESET_DELAY_18CLK, FALSE, TRUE);

        /* Enable WDT1 interrupt function */
        WDT_EnableInt(WDT1);
    }

    while(1)
    {
        /* Unlock protected registers before entering Power-down mode */
        SYS_UnlockReg();
        sysprintf("System enter to power-down mode ...\n");

        /* Enter to Power-down mode */
        PowerDownFunction();

        /* Check if WDT time-out interrupt and wake-up occurred or not */
        while(g_u8IsWDTWakeupINT == 0);

        g_u8IsWDTWakeupINT = 0;
        PA0 ^= 1;

        sysprintf("System has been waken up done.\n");
        sysprintf("WDT interrupt counts: %d.\n\n", g_u32WDTINTCounts);
    }

}

/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Use RTC alarm interrupt event to wake up system.
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint8_t g_u8IsRTCAlarmINT = 0;


/**
 * @brief       IRQ Handler for RTC Interrupt
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The RTC_IRQHandler is default IRQ of RTC.
 */
void RTC_IRQHandler(void)
{
    /* To check if RTC alarm interrupt occurred */
    if(RTC_GET_ALARM_INT_FLAG() == 1)
    {
        /* Clear RTC alarm interrupt flag */
        RTC_CLEAR_ALARM_INT_FLAG();

        g_u8IsRTCAlarmINT++;
    }
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HXT and LXT */
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk; // XTAL12M (HXT) Enabled
    CLK->PWRCTL |= CLK_PWRCTL_LXTEN_Msk; // 32K (LXT) Enabled

    /* Waiting clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);
    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);

    /* Enable IP clock */
    CLK_EnableModuleClock(RTC_MODULE);
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

void UART_Init(void)
{
    UART_Open(UART0, 115200);
}

int main1(void)
{
    __WFI();
}

int main(void)
{
    S_RTC_TIME_DATA_T sWriteRTC, sReadRTC;

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART for sysprintf */
    UART_Init();

    /* Lock protected registers */
    SYS_LockReg();

    sysprintf("\n\nCPU @ %dHz\n", SystemCoreClock);
    sysprintf("+-------------------------------------+\n");
    sysprintf("|    RTC Alarm Wake-up Sample Code    |\n");
    sysprintf("+-------------------------------------+\n\n");

    /* Enable RTC GIC */
    IRQ_SetHandler((IRQn_ID_t)RTC_IRQn, RTC_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)RTC_IRQn);

    /* Open RTC */
    sWriteRTC.u32Year       = 2020;
    sWriteRTC.u32Month      = 12;
    sWriteRTC.u32Day        = 25;
    sWriteRTC.u32DayOfWeek  = RTC_FRIDAY;
    sWriteRTC.u32Hour       = 23;
    sWriteRTC.u32Minute     = 59;
    sWriteRTC.u32Second     = 50;
    sWriteRTC.u32TimeScale  = RTC_CLOCK_24;
    RTC_Open(&sWriteRTC);

    /* Set RTC alarm date/time */
    sWriteRTC.u32Year       = 2020;
    sWriteRTC.u32Month      = 12;
    sWriteRTC.u32Day        = 25;
    sWriteRTC.u32DayOfWeek  = RTC_FRIDAY;
    sWriteRTC.u32Hour       = 23;
    sWriteRTC.u32Minute     = 59;
    sWriteRTC.u32Second     = 55;
    RTC_SetAlarmDateAndTime(&sWriteRTC);

    /* Enable RTC alarm interrupt and wake-up function will be enabled also */
    RTC_EnableInt(RTC_INTEN_ALMIEN_Msk);

    sysprintf("# Set RTC current date/time: 2020/12/25 23:59:50.\n");
    sysprintf("# Set RTC alarm date/time:   2020/12/25 23:59:55.\n");
    sysprintf("# Wait system waken-up by RTC alarm interrupt event.\n");

    g_u8IsRTCAlarmINT = 0;

    /* System enter to Power-down */
    /* To program PWRCTL register, it needs to disable register protection first. */
    SYS_UnlockReg();
    sysprintf("\nSystem enter to power-down mode ...\n");
    /* To check if all the debug messages are finished */
    UART_WAIT_TX_EMPTY(UART0);

    SYS_NPD();

    while(g_u8IsRTCAlarmINT == 0);

    /* Read current RTC date/time */
    RTC_GetDateAndTime(&sReadRTC);
    sysprintf("System has been waken-up and current date/time is:\n");
    sysprintf("    %d/%02d/%02d %02d:%02d:%02d\n",
           sReadRTC.u32Year, sReadRTC.u32Month, sReadRTC.u32Day, sReadRTC.u32Hour, sReadRTC.u32Minute, sReadRTC.u32Second);


    sysprintf("\n\n");
    sysprintf("# Set next RTC alarm date/time: 2020/12/26 00:00:05.\n");
    sysprintf("# Wait system waken-up by RTC alarm interrupt event.\n");
    RTC_SetAlarmDate(2020, 12, 26);
    RTC_SetAlarmTime(0, 0, 5, RTC_CLOCK_24, 0);

    g_u8IsRTCAlarmINT = 0;

    /* System enter to Power-down */
    /* To program PWRCTL register, it needs to disable register protection first. */
    SYS_UnlockReg();
    sysprintf("\nSystem enter to power-down mode ...\n");
    /* To check if all the debug messages are finished */
    UART_WAIT_TX_EMPTY(UART0);

    SYS_NPD();

    while(g_u8IsRTCAlarmINT == 0);

    /* Read current RTC date/time */
    RTC_GetDateAndTime(&sReadRTC);
    sysprintf("System has been waken-up and current date/time is:\n");
    sysprintf("    %d/%02d/%02d %02d:%02d:%02d\n",
           sReadRTC.u32Year, sReadRTC.u32Month, sReadRTC.u32Day, sReadRTC.u32Hour, sReadRTC.u32Minute, sReadRTC.u32Second);

    while(1);
}



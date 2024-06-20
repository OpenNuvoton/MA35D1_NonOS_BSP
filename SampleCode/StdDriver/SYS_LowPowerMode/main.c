/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate how to enter to different Power-down mode and
 *           wake-up by TIMER0.
 *
 *           When the A35 enters NPD/DPD, in order to reduce power consumption,
 *           this sample will make DDR in self-refresh mode and turn off
 *           DDR-PLL clock, so the power down process program(pd.c) must be
 *           stored in SRAM.
 *
 *           This sample must be compiled into two bin files and executed at
 *           DDR 0x80400000 & SRAM 0x28010000 respectively.
 *
 *           .cproject needs to be compiled once with
 *           --remove-section=.pd and --only-section=.pd to generate a bin file.
 *
 * @copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

extern uint32_t  Core1DataBase, Core1DataLimit;

void TMR0_IRQHandler(void)
{
    // Clear wake up flag
    TIMER_ClearWakeupFlag(TIMER0);
    // Clear interrupt flag
    TIMER_ClearIntFlag(TIMER0);
}

void Set_TMR0_WakeupSrc(void)
{
    TIMER0->CMP = 32000;

    /* Enable Timer0 interrupt */
    TIMER_EnableInt(TIMER0);
    /* Enable Timer0 wake up and interrupt */
    TIMER0->CTL = 0x60800001;

    IRQ_SetHandler((IRQn_ID_t)TMR0_IRQn, TMR0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR0_IRQn);
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable UART clock */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Select UART clock source from HXT */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Enable Timer clock */
    CLK_EnableModuleClock(TMR0_MODULE);
    /* Select Timer clock source from LIRC */
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_LIRC, 0);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();
}

void UART0_Init()
{
    /* Set multi-function pins */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Initial UART to 115200-8n1 for print message */
    UART0->INTEN = 0;
    UART0->LINE = 0x7;
    UART0->BAUD = 0x300000CE;
}

int main(void)
{
    uint32_t reg;
    uint32_t file_size;
    volatile int cnt;
    uint8_t u8Item;

    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART0_Init();

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* ice disconnect */
    SYS->CSDBGCTL |= (1<<3);

    reg = SYS->PMUSTS;
    sysprintf("Power Manager Status: 0x%08x\n\n", reg);
    if(reg & (SYS_PMUSTS_A35PDWKIF_Msk | SYS_PMUSTS_PMUIF_Msk))
    {
        /* Clear power down flag */
        SYS->PMUSTS |= SYS_PMUSTS_A35PDWKIF_Msk | SYS_PMUSTS_PMUIF_Msk;
        /* Clear Core 1 Warm-boot */
        SYS->CA35WRBPAR1= 0;
    }

    if(SYS->DDRCQCSR & 0x0002FF00) {
        sysprintf("Q ch deny interrupt flag set %08x\n", SYS->DDRCQCSR);
        reg = SYS->DDRCQCSR;
        SYS->DDRCQCSR = reg;
    }

    CLK->CLKSEL0 |= CLK_CLKSEL0_SYSCK0SEL_Msk;
    CLK->SYSCLK0 |= CLK_SYSCLK0_VDECEN_Msk;
    CLK->SYSCLK0 |= CLK_SYSCLK0_DCUEN_Msk;
    CLK->SYSCLK0 |= CLK_SYSCLK0_GMAC0EN_Msk;
    CLK->SYSCLK0 |= CLK_SYSCLK0_GMAC1EN_Msk;

    SYS->CA35WRBPAR0= 0x7761726D;
    SYS->CA35WRBADR0= 0x80400000;

    /* Load PD code to Core 1 and execute */
    file_size = ptr_to_u32(&Core1DataLimit) - ptr_to_u32(&Core1DataBase);
    memcpy((void *)0x28030000, (const void *)(nc_ptr(&Core1DataBase)), file_size);

    SYS->CA35WRBADR1= 0x28030000;
    cnt=1000;
    while(cnt--);
    sev();

    sysprintf("+-------------------------------------------+\n");
    sysprintf("|    MA35D1 Low Power Mode Sample Code      |\n");
    sysprintf("|    Please Select Low Power Mode Mode      |\n");
    sysprintf("+-------------------------------------------+\n");
    sysprintf("| [1] IDLE                                  |\n");
    sysprintf("| [2] NPD                                   |\n");
    sysprintf("| [3] DPD                                   |\n");
    sysprintf("+-------------------------------------------+\n");
    u8Item = sysgetchar();

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Wake-up by Timer0 */
    Set_TMR0_WakeupSrc();

    switch(u8Item) {
        case '1':
            sysprintf("\nSystem enters to IDLE mode ... ");
            while(!UART_IS_TX_EMPTY(UART0));
            asm volatile ("wfi");
            break;
        case '2':
            sysprintf("\nSystem enters to NPD mode ... ");
            while(!UART_IS_TX_EMPTY(UART0));
            /* Unlock protected registers before setting Power-down mode */
            SYS_UnlockReg();
            CA35_NPD(PD_HW_CTRL);
            break;
        case '3':
            sysprintf("\nSystem enters to DPD mode ... ");
            while(!UART_IS_TX_EMPTY(UART0));
            /* Unlock protected registers before setting Power-down mode */
            SYS_UnlockReg();
            CA35_DPD(PD_HW_CTRL);
            break;
        default:
            break;
    }

    sysprintf("Wake-up!\n");
    while(1);
}

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
 *           SYS_LowPowerMode_DRAM.bin stored in DRAM 0x80400000
 *           PD_SRAM.bin stored in SRAM 0x28010000
 *
 * @copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

extern uint32_t  Core1DataBase, Core1DataLimit;
extern void ddr_dpd_wk(void);

#define _measure_resumetime  0

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
    TIMER0->CTL = 0x60800002;

    IRQ_SetHandler((IRQn_ID_t)TMR0_IRQn, TMR0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR0_IRQn);
}

void GPD_IRQHandler(void)
{
    GPIO_CLR_INT_FLAG(PD, BIT8);
}

void Set_KEY1_WakeupSrc(void)
{
    /* Configure PD.8 as Input mode and enable interrupt by rising edge trigger */
    GPIO_SetMode(PD, BIT8, GPIO_MODE_INPUT);
    GPIO_EnableInt(PD, 8, GPIO_INT_FALLING);
    IRQ_SetHandler((IRQn_ID_t)GPD_IRQn, GPD_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)GPD_IRQn);
}

void GPIO_Init_for_PD_Mode(void)
{
    CLK->SYSCLK1 |= 0x3FFF0000;
    SYS->GPA_MFPH = 0;
    SYS->GPA_MFPL = 0;
    SYS->GPB_MFPH = 0;
    SYS->GPB_MFPL = 0;
    SYS->GPC_MFPH = 0;
    SYS->GPC_MFPL = 0;
    SYS->GPD_MFPH = 0;
    SYS->GPD_MFPL = 0;
    SYS->GPE_MFPH = 0;
    SYS->GPE_MFPL = 0;
    SYS->GPF_MFPH = 0;
    SYS->GPF_MFPL = 0;
    SYS->GPG_MFPH = 0;
    SYS->GPG_MFPL = 0;
    SYS->GPH_MFPH = 0;
    SYS->GPH_MFPL = 0;
    SYS->GPI_MFPH = 0;
    SYS->GPI_MFPL = 0;
    SYS->GPJ_MFPH = 0;
    SYS->GPJ_MFPL = 0;
    SYS->GPK_MFPH = 0;
    SYS->GPK_MFPL = 0;
    SYS->GPL_MFPH = 0;
    SYS->GPL_MFPL = 0;
    SYS->GPM_MFPH = 0;
    SYS->GPM_MFPL = 0;
    SYS->GPN_MFPH = 0;
    SYS->GPN_MFPL = 0;

    PA->MODE = 0;
    PB->MODE = 0;
    PC->MODE = 0;
    PD->MODE = 0;
    PE->MODE = 0;
    PF->MODE = 0;
    PG->MODE = 0;
    PH->MODE = 0;
    PI->MODE = 0;
    PJ->MODE = 0;
    PK->MODE = 0;
    PL->MODE = 0;
    PM->MODE = 0;
    PN->MODE = 0;
    PA->PUSEL = 0x55555555;
    PB->PUSEL = 0x55555555;
    PC->PUSEL = 0x55555555;
    PD->PUSEL = 0x55555555;
    PE->PUSEL = 0x55555555;
    PF->PUSEL = 0x55555555;
    PG->PUSEL = 0x55555555;
    PH->PUSEL = 0x55555555;
    PI->PUSEL = 0x55555555;
    PJ->PUSEL = 0x55555555;
    PK->PUSEL = 0x55555555;
    PL->PUSEL = 0x55555555;
    PM->PUSEL = 0x55555555;
    PN->PUSEL = 0x55555555;
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

    CLK_EnableModuleClock(GPD_MODULE);
    CLK_EnableModuleClock(GPH_MODULE);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();


    SYS->GPH_MFPH &= ~SYS_GPH_MFPH_PH14MFP_Msk;
    GPIO_SetMode(PH, BIT14, GPIO_MODE_OUTPUT);

    SYS->GPK_MFPL &= ~SYS_GPK_MFPL_PK7MFP_Msk;
    SYS->GPK_MFPL |= SYS_GPK_MFPL_PK7MFP_CLKO;
    CLK->SYSCLK1 |= CLK_SYSCLK1_CLKOCKEN_Msk;
    CLK->CLKSEL4 |= CLK_CLKSEL4_CKOSEL_DDRPLL;
    CLK->CLKOCTL = 0x12;

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
    unsigned int volatile *ptr;
    void (*fptr)(void);

    reg = SYS->PMUSTS;
    if(reg & SYS_PMUSTS_A35PDWKIF_Msk)
    {
#if (_measure_resumetime == 1)
        PH14 = 1;
#endif
        sysprintf("\nA35 DPD mode Wake-up.\nPower Manager Status[8]: 0x%08x\n\n", reg);
        /* Clear power down flag */
        SYS->PMUSTS |= SYS_PMUSTS_A35PDWKIF_Msk;
        /* Clear Core 1 Warm-boot */
        SYS->CA35WRBPAR1= 0;
        while(1);
    }

    /* Unlock protected registers */
    SYS_UnlockReg();

    GPIO_Init_for_PD_Mode();

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

    if(SYS->DDRCQCSR & 0x0002FF00) {
        sysprintf("Q ch deny interrupt flag set %08x\n", SYS->DDRCQCSR);
        reg = SYS->DDRCQCSR;
        SYS->DDRCQCSR = reg;
    }

    CLK->CLKSEL0 |= CLK_CLKSEL0_SYSCK0SEL_Msk;
    CLK->SYSCLK0 |= 0x7f000000;

    fptr = ddr_dpd_wk;

    SYS->CA35WRBPAR0= 0x7761726D;
    SYS->CA35WRBADR0= ptr_to_u32((void *)fptr);

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


#if (_measure_resumetime == 0)
    /* Wake-up by Timer0 */
    Set_TMR0_WakeupSrc();
#endif

#if (_measure_resumetime == 1)
    PH14 = 1;
    /* Waiting for KEY1 PD.8 rising-edge interrupt event */
    Set_KEY1_WakeupSrc();
    PH14 = 0;
#endif

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
            CA35_NPD(PD_SW_CTRL);
            break;
        case '3':
            sysprintf("\nSystem enters to DPD mode ... ");
            while(!UART_IS_TX_EMPTY(UART0));
            /* Unlock protected registers before setting Power-down mode */
            SYS_UnlockReg();
            CA35_DPD(PD_SW_CTRL);
            break;
        default:
            break;
    }

#if (_measure_resumetime == 1)
            PH14 = 1;
#endif

    sysprintf("Wake-up!\n");
    while(1);
}

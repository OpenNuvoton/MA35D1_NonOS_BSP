/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate hardware semaphore (HWSEM) lock/unlock function to
 *           protect a critical section in dual core system which each core
 *           may enter it at a same time.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include "NuMicro.h"

#define HWSEM_CH      0       // 0~7
#define CORE0_KEY     0xA5
#define CORE1_KEY     0x5A

/* Delay execution for given amount of ticks */
void Delay0(uint32_t ticks)  {
	uint32_t tgtTicks = msTicks0 + ticks;             // target tick count to delay
	while (msTicks0 < tgtTicks);
}

void Delay1(uint32_t ticks)  {
	uint32_t tgtTicks = msTicks1 + ticks;             // target tick count to delay
	while (msTicks1 < tgtTicks);
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

void HWSEM_Init()
{
	// Enable clock
	CLK_EnableModuleClock(HWS_MODULE);

	// Reset HWSEM
    SYS->IPRST0 = SYS_IPRST0_HWSEMRST_Msk;
    SYS->IPRST0 = 0;
}

/* This task takes 3 sec to finish*/
void Task_Critical(uint32_t ch, uint8_t key)
{
	// Enter critical section
	HWSEM_Spin_Lock(HWSEM0, ch, key);

	Delay0(3000);

	// Leave critical section
	HWSEM_UNLOCK(HWSEM0, ch, key);
}

/* main function */
int main(void)
{
	SYS_UnlockReg();

	HWSEM_Init();

	UART0_Init();

	global_timer_init();

	// start task0
	sysprintf("---Task#0 start---\n");

	Task_Critical(HWSEM_CH, CORE0_KEY);

	sysprintf("---Task#0 finished---\n");

	while(1) {};
}

/* main1 function */
int main1(void)
{
	global_timer_init();

	Delay1(100);

	// start task1
	sysprintf("---Task#1 start---\n");

	Task_Critical(HWSEM_CH, CORE1_KEY);

	sysprintf("---Task#1 finished---\n");

	while(1) {};
}

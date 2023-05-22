/**************************************************************************//**
 * @file    main.c
 * @brief   Dual Core demo
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "NuMicro.h"

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

void UART16_Init() {

	/* Enable UART16 clock */
    CLK_EnableModuleClock(UART16_MODULE);
    CLK_SetModuleClock(UART16_MODULE, CLK_CLKSEL3_UART16SEL_HXT, CLK_CLKDIV3_UART16(1));

    /* Set multi-function pins */
    SYS->GPK_MFPL &= ~(SYS_GPK_MFPL_PK2MFP_Msk | SYS_GPK_MFPL_PK3MFP_Msk);
    SYS->GPK_MFPL |= (SYS_GPK_MFPL_PK2MFP_UART16_RXD | SYS_GPK_MFPL_PK3MFP_UART16_TXD);

	/* Init UART to 115200-8n1 for print message */
	UART_Open(UART16, 115200);
}


/* main1 function */
int main1(void)
{
	UART16_Init();
	global_timer_init();
	while(1) {
		sysprintf("This is core 1 %dms\n", msTicks1);
		Delay1(10);
	}
}

/* main function */
int main(void)
{
	UART0_Init();
	global_timer_init();
	while(1) {
		sysprintf("This is core 0 %dms\n", msTicks0);
		Delay0(10);
	}
}

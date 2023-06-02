/*************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    A project template for Core1 Only, with message output to UART16
 *               and execution address set to 0x88000000.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NuMicro.h"

/*--------------------------------------------------------------------------*/
/* Delay execution for given amount of ticks */
void Delay1(uint32_t ticks)  {
	uint32_t tgtTicks = msTicks1 + ticks;             // target tick count to delay
	while (msTicks1 < tgtTicks);
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

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* System Clock Initial                                                                                    */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Setting UART16 SSPCC for TZNS */
    SSPCC->PSSET8 = (SSPCC->PSSET8 & ~SSPCC_PSSET8_UART16_Msk) | 0x1<<SSPCC_PSSET8_UART16_Pos;
    /*---------------------------------------------------------------------------------------------------------*/
    /* I/O Multi-function Initial                                                                              */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Lock protected registers */
    SYS_LockReg();
}

/*
 * This is a template project for MA35D1 series MPU. Users could based on this project to create their
 * own application.
 *
 * This template application uses external crystal as HCLK source and configures UART16 to print out
 * "Hello World", users may need to do extra system configuration based on their system design.
 */

int main()
{
    /* Initialize UART to 115200-8n1 for print message */
    UART16_Init();

    global_timer_init();

    /* Initialize System, IP clock and multi-function I/O */
    SYS_Init();

    /* Connect UART to PC, and open a terminal tool to receive following message */
    sysprintf("Hello World\n");

    /* Got no where to go, just loop forever */
	while(1) {
		sysprintf("Hello World!, This is core1 %dms\n", msTicks1);
		Delay1(10);
	}

}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/

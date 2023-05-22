/*************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    A project template for MA35D1 MPU.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NuMicro.h"

/*--------------------------------------------------------------------------*/
/* Delay execution for given amount of ticks */
void Delay0(uint32_t ticks)  {
	uint32_t tgtTicks = msTicks0 + ticks;             // target tick count to delay
	while (msTicks0 < tgtTicks);
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
 * This template application uses external crystal as HCLK source and configures UART0 to print out
 * "Hello World", users may need to do extra system configuration based on their system design.
 */

int main()
{

    /* Initialize UART to 115200-8n1 for print message */
    UART0_Init();

    global_timer_init();

    /* Initialize System, IP clock and multi-function I/O */
    SYS_Init();

    /* Connect UART to PC, and open a terminal tool to receive following message */
    sysprintf("Hello World\n");

    /* Got no where to go, just loop forever */
    while(1);

}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/

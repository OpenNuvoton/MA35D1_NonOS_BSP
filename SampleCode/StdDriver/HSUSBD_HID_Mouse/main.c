/**************************************************************************//**
 * @file    main.c
 * @brief    Simulate an USB mouse and draws circle on the screen
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "NuMicro.h"
#include "hid_mouse.h"

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

    /* Initial UART to 115200-8n1 for print message */
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
    CLK_EnableModuleClock(USBD_MODULE);

	/*---------------------------------------------------------------------------------------------------------*/
    /* I/O Multi-function Initial                                                                              */
    /*---------------------------------------------------------------------------------------------------------*/
    /* HSUSBD VBUS detect pin - PF15 */
    SYS->GPF_MFPH &= ~SYS_GPE_MFPH_PE15MFP_Msk;
	SYS->GPF_MFPH |= SYS_GPF_MFPH_PF15MFP_HSUSB0_VBUSVLD;

    /* Lock protected registers */
    SYS_LockReg();
}


int32_t main (void)
{
    /* Initialize UART to 115200-8n1 for print message */
    UART0_Init();

    global_timer_init();

    /* Initialize System, IP clock and multi-function I/O */
    SYS_Init();

    sysprintf("\n\nNuMicro HSUSBD HID Mouse Sample\n");

    HSUSBD_Open(&gsHSInfo, HID_ClassRequest, NULL);

    /* Endpoint configuration */
    HID_Init();

    /* Enable USBD interrupt */
    IRQ_SetHandler((IRQn_ID_t)HSUSBD_IRQn, HSUSBD_IRQHandler);
    IRQ_Enable((IRQn_ID_t)HSUSBD_IRQn);

    /* Start transaction */
    while(1)
    {
        if (HSUSBD_IS_ATTACHED())
        {
            HSUSBD_Start();
            break;
        }
    }

    while(1)
    {
        HID_Process();
    }
}



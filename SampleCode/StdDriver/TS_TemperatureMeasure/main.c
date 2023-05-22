/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Measure the current temperature by Temperature Sensor.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint32_t g_u32Temperature = 0;


void SYS_Init(void)
{
	/* Enable UART module clock */
	CLK_EnableModuleClock(UART0_MODULE);

	/* Select UART module clock source as HXT */
	CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

	/* To update the variable SystemCoreClock */
	SystemCoreClockUpdate();

	/* Set GPE multi-function pins for UART RXD and TXD */
	SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE15MFP_Msk | SYS_GPE_MFPH_PE14MFP_Msk);
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE15MFP_UART0_RXD | SYS_GPE_MFPH_PE14MFP_UART0_TXD);
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

    /* Begin measuring the temperature */
	TS_Open();

   	while (1)
	{
		if (TS_GET_DATAVALID_FLAG())
		{
			g_u32Temperature = TS_GET_TEMPERATURE();
			sysprintf("Measured Temperature = %03d\r", g_u32Temperature);

			TS_CLEAR_DATAVALID_FLAG();
			break;
		}
	}
}


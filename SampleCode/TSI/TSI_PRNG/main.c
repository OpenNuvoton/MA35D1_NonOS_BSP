/**************************************************************************//**
 * @file     main.c
 * @brief    This sample program demonstrates how to use the TSI commands to
 *           make H/W PRNG to generate random numbers.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"

#define USER_SEED       0xa7ae6d1c

uint32_t rnd_buff_pool[4096] __attribute__((aligned(32)));

void SYS_Init(void)
{
	/* Enable UART module clock */
	CLK_EnableModuleClock(UART0_MODULE);

	/* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
	CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

	/* enable Wormhole 1 clock */
	CLK_EnableModuleClock(WH1_MODULE);

	/* Set GPE multi-function pins for UART0 RXD and TXD */
	SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);
}

void UART0_Init()
{
	/* Configure UART0 and set UART0 baud rate */
	UART_Open(UART0, 115200);
}

void delay_us(int usec)
{
	uint64_t   t0;
	t0  = EL0_GetCurrentPhysicalValue();
	while ((EL0_GetCurrentPhysicalValue() - t0) < (usec * 12));
}

int32_t main(void)
{
	uint32_t *rnd_buff = nc_ptr(rnd_buff_pool);
	int i, item, ret;

	/* Unlock protected registers */
	SYS_UnlockReg();

	/* Init System, IP clock and multi-function I/O */
	SYS_Init();

	/* Init UART0 for sysprintf */
	UART0_Init();

	if (TSI_Init() != 0)
	{
		sysprintf("TSI Init failed!\n");
		while (1);
	}

	ret = TSI_TRNG_Init(0, 0);
	if (ret != 0)
	{
		sysprintf("TSI_TRNG_Init failed! (%d)\n", ret);
		while (1);
	}

	while (1)
	{
		sysprintf("\n\n");
		sysprintf("+---------------------------------------------------------------+\n");
		sysprintf("|  TSI PRNG                                                     |\n");
		sysprintf("+---------------------------------------------------------------+\n");
		sysprintf("| [1] PRNG re-seed by user                                      |\n");
		sysprintf("| [2] PRNG re-seed by TRNG                                      |\n");
		sysprintf("| [3] Generate random numbers                                   |\n");
		sysprintf("| [4] Generate mass random numbers                              |\n");
		sysprintf("+---------------------------------------------------------------+\n");

		item = sysgetchar();
		switch (item)
		{
			case '1':
				sysprintf("\nPRNG re-seed by user seed =>\n");
				ret = TSI_PRNG_ReSeed(1, USER_SEED);
				if (ret != 0)
				{
					sysprintf("TSI_PRNG_ReSeed failed: %d\n", ret);
					while (1);
				}

				TSI_PRNG_Gen_Random(nc_ptr(&rnd_buff[0]), nc_ptr(&rnd_buff[1]));
				sysprintf("0x%08x 0x%08x\n", rnd_buff[0], rnd_buff[1]);
				break;

			case '2':
				sysprintf("\nPRNG re-seed by TRNG =>\n");
				ret = TSI_PRNG_ReSeed(0, 0);
				if (ret != 0)
				{
					sysprintf("TSI_PRNG_ReSeed failed: %d\n", ret);
					while (1);
				}

				TSI_PRNG_Gen_Random(nc_ptr(&rnd_buff[0]), nc_ptr(&rnd_buff[1]));
				sysprintf("0x%08x 0x%08x\n", rnd_buff[0], rnd_buff[1]);
				break;

			case '3':
				for (i = 0; i < 10; i++)
				{
					TSI_PRNG_Gen_Random(nc_ptr(&rnd_buff[0]), nc_ptr(&rnd_buff[1]));
					sysprintf("0x%08x 0x%08x\n", rnd_buff[0], rnd_buff[1]);
				}
				sysprintf("\n");
				break;

			case '4':
				TSI_PRNG_Gen_Random_Mass(64, ptr_to_u32(rnd_buff));
				for (i = 0; i < 64; i++)
					sysprintf("0x%08x ", rnd_buff[i]);
				sysprintf("\n\n");
				break;
		}
	}
}

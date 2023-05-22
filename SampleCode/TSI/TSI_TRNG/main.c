/**************************************************************************//**
 * @file     main.c
 * @brief    This sample program demonstrates how to use the TSI commands to
 *           initialize TRNG and generate random numbers.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"

uint32_t  user_nonce[48] = {
	0xc47b0294, 0xdbbbee0f, 0xec4757f2, 0x2ffeee35, 0x87ca4730, 0xc3d33b69, 0x1df38bab, 0x63ac0a6b,
	0xd38da3ab, 0x584a50ea, 0xb93f2603, 0x09a5c691, 0x09a5c691, 0x024f91ac, 0x6063ce20, 0x229160d9,
	0x49e00388, 0x1ab6b0cd, 0xe657cb40, 0x87c5aa81, 0xd611eab8, 0xa7ae6d1c, 0x3a181a28, 0x9391bbec,
	0x22186179, 0xb6476813, 0x67e64213, 0x47cc0c01, 0xf53bc314, 0x73545902, 0xd8a14864, 0xb31262d1,
	0x2bf77bc3, 0xd81c9e3a, 0xa0657c50, 0x51a2fe50, 0x91ff8818, 0x6de4dc00, 0xba468631, 0x7601971c,
	0xdec69b2f, 0x336e9662, 0xef73d94a, 0x618226a3, 0x3cdd3154, 0xf361b408, 0x55d394b4, 0xfc3d7775,
};

uint32_t user_seed[12] = {
	0x8b35e0ef, 0xa221fe17, 0x0d498127, 0x641719f1, 0x4e5197b1, 0x7c84d929, 0xab60aa80, 0x08889570,
	0xee42614d, 0x73c2ace4, 0xbaed0e9c, 0x9a12145d,
};

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

	if (TSI_Init() != 0) {
		sysprintf("TSI Init failed!\n");
		while (1);
	}

	sysprintf("\n\n");
	sysprintf("+---------------------------------------------------------------+\n");
	sysprintf("|  Select TRNG seeding method                                   |\n");
	sysprintf("+---------------------------------------------------------------+\n");
	sysprintf("| [1] Self-seeding (Seed is from TRNG noise)                    |\n");
	sysprintf("| [2] Nonce seeding.                                            |\n");
	sysprintf("| [3] User seed                                                 |\n");
	sysprintf("+---------------------------------------------------------------+\n");

	while (1)
	{
		item = sysgetchar();
		if ((item == '1') || (item == '2') || (item == '3'))
			break;
	}

	if (item == '1')
		ret = TSI_TRNG_Init(0, 0);
	else if (item == '2')
		ret = TSI_TRNG_Init(1, ptr_to_u32(user_nonce));
	else
		ret = TSI_TRNG_Init(2, ptr_to_u32(user_seed));

	if (ret != 0)
	{
		sysprintf("TSI_TRNG_Init failed! (%d)\n", ret);
		while (1);
	}

	while (1)
	{
		sysprintf("\nGenerate random numbers ==>\n");

		TSI_TRNG_Gen_Random(16, ptr_to_u32(rnd_buff));

		for (i = 0; i < 16; i++)
			sysprintf("0x%08x ", rnd_buff[i]);
		sysprintf("\n");

		delay_us(10000);

		if (sysIsKbHit())
		{
			sysgetchar();
			sysprintf("\nPress any key to continue...\n");
			sysgetchar();
		}
	}
}

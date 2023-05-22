/**************************************************************************//**
 * @file     main.c
 * @brief    Show how mbedTLS SHA256 function works.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "NuMicro.h"
#include "tsi_cmd.h"
#include "common.h"

#define MBEDTLS_EXIT_SUCCESS    0
#define MBEDTLS_EXIT_FAILURE    -1

extern int mbedtls_sha256_self_test(int verbose);

uint64_t get_time_ms(void)
{
	return EL0_GetCurrentPhysicalValue() / 12000;
}

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

int32_t main(void)
{
	int  i32Ret = MBEDTLS_EXIT_SUCCESS;
	uint64_t t0;

	/* Unlock protected registers */
	SYS_UnlockReg();

	/* Init System, IP clock and multi-function I/O */
	SYS_Init();

	/* Init UART0 for sysprintf */
	UART_Open(UART0, 115200);

	if (TSI_Init() != 0)
	{
		sysprintf("TSI Init failed!\n");
		while (1);
	}

	sysprintf("MBEDTLS SHA256 self test ...\n");

#ifdef MBEDTLS_SHA256_ALT
	sysprintf("Hardware Accellerator Enabled.\n");
#else
	sysprintf("Pure software crypto running.\n");
#endif

	t0 = get_time_ms();
	i32Ret = mbedtls_sha256_self_test(1);
	sysprintf("Total elapsed time is %d ms\n", (uint32_t)(get_time_ms() - t0));

	if(i32Ret < 0)
	{
		sysprintf("Test fail!\n");
	}
	sysprintf("Test Done!\n");
	while(1);

}

int mbedtls_platform_entropy_poll( void *data, unsigned char *output, size_t len, size_t *olen )
{
	return 0;
}


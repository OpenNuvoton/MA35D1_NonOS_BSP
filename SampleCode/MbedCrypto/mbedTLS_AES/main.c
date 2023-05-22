/**************************************************************************//**
 * @file     main.c
 * @brief    Show how mbedTLS AES function works.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "tsi_cmd.h"
#include "common.h"
#include "mbedtls/aes.h"

#define MBEDTLS_EXIT_SUCCESS    0
#define MBEDTLS_EXIT_FAILURE    -1

extern int mbedtls_aes_self_test( int verbose );
extern int mbedtls_gcm_self_test( int verbose );
extern int mbedtls_ccm_self_test( int verbose );

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

void UART0_Init()
{
	/* Configure UART0 and set UART0 baud rate */
	UART_Open(UART0, 115200);
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
	UART0_Init();

	if (TSI_Init() != 0)
	{
		sysprintf("TSI Init failed!\n");
		while (1);
	}

	sysprintf("MBEDTLS AES self test ...\n");

#ifdef MBEDTLS_AES_ALT
	sysprintf("Hardware Accellerator Enabled.\n");
#else
	sysprintf("Pure software crypto running.\n");
#endif

	t0 = get_time_ms();
	i32Ret = mbedtls_aes_self_test(1);
	if(i32Ret < 0)
	{
		sysprintf("AES test fail!\n");
		while (1);
	}
	sysprintf("\nAES test total elapsed time is %d ms\n\n", (uint32_t)(get_time_ms() - t0));

#if 0
	t0 = get_time_ms();
	i32Ret = mbedtls_gcm_self_test(1);
	if(i32Ret < 0)
	{
		sysprintf("AES-GCM test fail!\n");
		while (1);
	}
	sysprintf("\nAES-GCM test total elapsed time is %d ms\n\n", (uint32_t)(get_time_ms() - t0));

	t0 = get_time_ms();
	i32Ret = mbedtls_ccm_self_test(1);
	if(i32Ret < 0)
	{
		sysprintf("AES-CCM test fail!\n");
		while (1);
	}
	sysprintf("\nAES-CCM test total elapsed time is %d ms\n\n", (uint32_t)(get_time_ms() - t0));
#endif
	sysprintf("Test Done!\n");
	while(1);

}

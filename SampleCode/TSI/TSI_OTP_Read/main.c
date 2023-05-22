/**************************************************************************//**
 * @file     main.c
 * @brief    This sample program demonstrates how to use the TSI commands to
 *           read OTP contents.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"

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
	uint32_t otp_data, otp_data2;
	int ret;

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

	sysprintf("+--------------------------------------------+\n");
	sysprintf("|  MA35D1 TSI OTP read demo                  |\n");
	sysprintf("+--------------------------------------------+\n");

	/*
	 *  Power-on Setting: 0x100~0x103
	 */
	ret = TSI_OTP_Read(0x100, &otp_data);
	if (ret)
		goto err_out;
	sysprintf("Power-on Setting = 0x%x\n", otp_data);

	/*
	 *  DPM Setting: 0x104~0x107
	 */
	ret = TSI_OTP_Read(0x104, &otp_data);
	if (ret)
		goto err_out;
	sysprintf("DPM Setting = 0x%x\n", otp_data);

	/*
	 *  PLM Setting: 0x108~0x10B
	 */
	ret = TSI_OTP_Read(0x108, &otp_data);
	if (ret)
		goto err_out;
	sysprintf("PLM Setting = 0x%x\n", otp_data);

	/*
	 *  MAC0 Address: 0x10C~0x113
	 */
	ret = TSI_OTP_Read(0x10C, &otp_data);
	if (ret)
		goto err_out;
	ret = TSI_OTP_Read(0x110, &otp_data2);
	if (ret)
		goto err_out;
	sysprintf("MAC0 address = 0x%08x, 0x%08x\n", otp_data, otp_data2);

	/*
	 *  MAC1 Address: 0x114~0x11B
	 */
	ret = TSI_OTP_Read(0x114, &otp_data);
	if (ret)
		goto err_out;
	ret = TSI_OTP_Read(0x118, &otp_data2);
	if (ret)
		goto err_out;
	sysprintf("MAC1 address = 0x%08x, 0x%08x\n", otp_data, otp_data2);

	sysprintf("\nDone.\n");
	while (1);

err_out:
	sysprintf("Failed!\n");
	TSI_Print_Error(ret);
	while (1);
}

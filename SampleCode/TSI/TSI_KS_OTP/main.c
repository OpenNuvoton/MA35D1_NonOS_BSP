/**************************************************************************//**
 * @file     main.c
 * @brief    This sample program demonstrates how to use the TSI commands to
 *           program and read Key Store OTP keys.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"

#define KS_KEY_MAX          (KS_MAX_OTP_KEY_SIZE / 32)
#define KEY_SEED            0x1A39175D

struct ks_otp_keys {
	uint32_t   key[KS_KEY_MAX];
	int        size;
	uint32_t   meta;
};

struct ks_otp_keys  _my_keys_mem[KS_OTP_KEY_CNT];
struct ks_otp_keys  *_my_otp_keys;

int   key_size_list[KS_OTP_KEY_CNT] = { 128, 128, 128, 256, 256, 256, 256, 256, 256 };
int   key_size_meta[KS_OTP_KEY_CNT] = { KS_META_128, KS_META_128, KS_META_128,
										KS_META_256, KS_META_256, KS_META_256,
										KS_META_256, KS_META_256, KS_META_256 };

static uint8_t  tsi_buff_mem[4096] __attribute__((aligned(32)));

static void initialize_my_test_keys(void)
{
	int         i, j;
	uint32_t    k32 = KEY_SEED;

	sysprintf("Initialize my test vectors...");

	_my_otp_keys = nc_ptr(_my_keys_mem);

	for (i = 0; i < KS_OTP_KEY_CNT; i++)
	{
		for (j = 0; j < KS_KEY_MAX; j++)
		{
			_my_otp_keys[i].key[j] = k32;
			k32 += KEY_SEED;
		}
		_my_otp_keys[i].size = key_size_list[i];
		_my_otp_keys[i].meta = key_size_meta[i] | KS_META_CPU | KS_META_READABLE;
	}
	sysprintf("Done.\n");
}

static void ks_otp_try_read_all(void)
{
	uint32_t *au32RKey = nc_ptr(tsi_buff_mem);
	int i, knum, ret, wcnt;
	int found = 0;

	sysprintf("\n\nTry to read OTP key if available...\n");
	for (knum = 0; knum < KS_OTP_KEY_CNT; knum++)
	{
		if (knum < 3)
			wcnt = 128 / 32;
		else
			wcnt = 256 / 32;

		ret = TSI_KS_Read(KS_OTP, knum, au32RKey, wcnt);
		if (ret == 0)
		{
			sysprintf("OTP key %d, size %d:\n    ", knum, wcnt * 32);
			for (i = 0; i < wcnt; i++)
				sysprintf("0x%x ", au32RKey[i]);
			sysprintf("\n");
			found = 1;
		}
	}
	if (!found)
		sysprintf("No readable OTP keys found.\n");
	return;
}

static void ks_otp_write_one_key(void)
{
	uint32_t *au32RKey = nc_ptr(tsi_buff_mem);
	int i, knum, ret, wcnt;

	sysprintf("\n\nTry to program an OTP key...\n");
	for (knum = 0; knum < KS_OTP_KEY_CNT; knum++)
	{
		if (knum < 3)
			wcnt = 128 / 32;
		else
			wcnt = 256 / 32;

		ret = TSI_KS_Write_OTP(knum, _my_otp_keys[knum].meta, _my_otp_keys[knum].key);
		if (ret == 0)
		{
			sysprintf("Program OTP key %d success.\n");
			return;
		}
		else
		{
			sysprintf("OTP key %d cannot be programmed.\n");
			sysprintf("It may be a revoked key or be an unreadable key.\n");
		}
	}
	sysprintf("\nFailed to program an OTP key.\n");
	return;
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
	int item, ret, knum, kidx = 0;

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

	sysprintf("*******************************\n");
	sysprintf("**        WARNING !!         **\n");
	sysprintf("*******************************\n");
	sysprintf("This sample code will write random data to the OTP of your MA35D1 chip.\n");
	sysprintf("Please make sure you want to do this!\n\n");
	sysprintf("press any key to start...\n");
	sysgetchar();

	_my_otp_keys = nc_ptr(_my_keys_mem);

	initialize_my_test_keys();

	while (1)
	{
		sysprintf("\n");
		sysprintf("+---------------------------------------------------------------+\n");
		sysprintf("|  MA35D1 TSI Key Store SRAM operation menu                     |\n");
		sysprintf("+---------------------------------------------------------------+\n");
		sysprintf("| [1] Read all Key Store OTP keys                               |\n");
		sysprintf("| [2] Write a Key Store OTP key                                 |\n");
		sysprintf("+---------------------------------------------------------------+\n");

		sysprintf("\nSelect [0 ~ 5]: \n");

		item = sysgetchar();

		switch (item)
		{
			case '1':
				ks_otp_try_read_all();
				break;

			case '2':
				ks_otp_write_one_key();
				break;

			default:
				sysprintf("Not supported.\n");
				break;
		}
		sysprintf("\nPress any key to continue...\n");
		sysgetchar();
	}
}

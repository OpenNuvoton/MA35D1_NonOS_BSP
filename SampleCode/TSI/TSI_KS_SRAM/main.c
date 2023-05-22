/**************************************************************************//**
 * @file     main.c
 * @brief    This sample program demonstrates how to use the TSI commands to
 *           program/read/erase/revoke Key Store SRAM keys.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"

#define KS_KEY_MAX          (KS_MAX_KEY_SIZE / 32)
#define KEY_SEED            0x1A39175D

struct ks_keys {
	uint32_t   key[KS_KEY_MAX];
	int        size;
	int        number;
	uint32_t   meta;
};

struct ks_keys  _my_keys_mem[KS_SRAM_KEY_CNT];
struct ks_keys  *_my_keys;

int   key_size_list[] = { 128, 128, 128, 256, 256, 128, 163, 192, 224, 233, 255,
						  256, 283, 384, 409, 512, 521, 571, 1024, 2048, 4096 };
int   key_size_meta[] = { KS_META_128, KS_META_128, KS_META_128, KS_META_256, KS_META_256,
						  KS_META_128, KS_META_163, KS_META_192, KS_META_224, KS_META_233,
						  KS_META_255, KS_META_256, KS_META_283, KS_META_384, KS_META_409,
						  KS_META_512, KS_META_521, KS_META_571, KS_META_1024,KS_META_2048,
						  KS_META_4096 };

struct ks_record {
	int used;
	int key_size;
};

static struct ks_record _key_records[KS_SRAM_KEY_CNT];

static uint8_t  tsi_buff_mem[4096] __attribute__((aligned(32)));

static void initialize_my_test_keys(void)
{
	int         i, j, k, klist_size;
	uint32_t    k32 = KEY_SEED;

	sysprintf("Initialize my test vectors...");

	klist_size = sizeof(key_size_list)/4;
	k = 0;
	for (i = 0; i < KS_SRAM_KEY_CNT; i++) {
		for (j = 0; j < KS_KEY_MAX; j++) {
			_my_keys[i].key[j] = k32;
			k32 += KEY_SEED;
		}
		_my_keys[i].number = -1;    /* not used */
		_my_keys[i].size = key_size_list[k];
		_my_keys[i].meta = key_size_meta[k] | KS_META_CPU | KS_META_READABLE;
		k = (k + 1) % klist_size;
	}
	sysprintf("Done.\n");
}

static int ks_sram_dump_info(void)
{
	uint32_t *au32RKey = nc_ptr(tsi_buff_mem);
	int i, knum, ret, wcnt, rsize;

	for (knum = 0; knum < KS_SRAM_KEY_CNT; knum++)
	{
		if (!_key_records[knum].used)
			continue;

		sysprintf("KEY %d, size %d\n", knum, _key_records[knum].key_size);
		wcnt = (_key_records[knum].key_size + 31) / 32;
		ret = TSI_KS_Read(KS_SRAM, knum, au32RKey, wcnt);
		if (ret == 0)
		{
			sysprintf("    ");
			for (i = 0; i < wcnt; i++)
				sysprintf("0x%x ", au32RKey[i]);
			sysprintf("\n");
		}
		else
		{
			sysprintf("TSI_KS_Read failed!!\n");
			TSI_Print_Error(ret);
		}
	}

	ret = TSI_KS_GetRemainSize(&rsize);
	if (ret != 0)
	{
		sysprintf("TSI_KS_GetRemainSize command failed!!\n");
		TSI_Print_Error(ret);
		return -1;
	}
	sysprintf("\nKey Store SRAM remain size: %d\n", rsize);
	return 0;
}

static void ks_erase_one_key(void)
{
	uint32_t *au32RKey = nc_ptr(tsi_buff_mem);
	int i, knum, ret, wcnt, rsize;

	for (knum = 0; knum < KS_SRAM_KEY_CNT; knum++)
	{
		if (_key_records[knum].used)
		{
			sysprintf("Erase Key Store SRAM key %d\n", knum);
			ret = TSI_KS_EraseKey(KS_SRAM, knum);
			if (ret != 0)
			{
				sysprintf("TSI_KS_EraseKey failed!!\n");
				TSI_Print_Error(ret);
			}
			_key_records[knum].used = 0;
			return;
		}
	}
	sysprintf("No SRAM key.\n");
}

static void ks_revoke_one_key(void)
{
	uint32_t *au32RKey = nc_ptr(tsi_buff_mem);
	int i, knum, ret, wcnt, rsize;

	for (knum = 0; knum < KS_SRAM_KEY_CNT; knum++)
	{
		if (_key_records[knum].used)
		{
			sysprintf("Revoke Key Store SRAM key %d\n", knum);
			ret = TSI_KS_RevokeKey(KS_SRAM, knum);
			if (ret != 0)
			{
				sysprintf("TSI_KS_EraseKey failed!!\n");
				TSI_Print_Error(ret);
			}
			_key_records[knum].used = 0;
			return;
		}
	}
	sysprintf("No SRAM key.\n");
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

	_my_keys = nc_ptr(_my_keys_mem);

	initialize_my_test_keys();
	TSI_KS_EraseAll();
	memset(_key_records, 0, sizeof(_key_records));

	while (1)
	{
		sysprintf("\n");
		sysprintf("+---------------------------------------------------------------+\n");
		sysprintf("|  MA35D1 TSI Key Store SRAM operation menu                     |\n");
		sysprintf("+---------------------------------------------------------------+\n");
		sysprintf("| [0] Dump Key Store SRAM current status                        |\n");
		sysprintf("| [1] Write a Key Store SRAM key                                |\n");
		sysprintf("| [2] Erase a Key Store SRAM key                                |\n");
		sysprintf("| [3] Revoke a Key Store SRAM key                               |\n");
		sysprintf("| [4] Erase all Key Store SRAM keys                             |\n");
		sysprintf("+---------------------------------------------------------------+\n");

		sysprintf("\nSelect [0 ~ 5]: \n");

		item = sysgetchar();

		switch (item)
		{
			case '0':
				ks_sram_dump_info();
				break;

			case '1':
				ret = TSI_KS_Write_SRAM(_my_keys[kidx].meta, _my_keys[kidx].key, &knum);
				if (ret != 0)
				{
					sysprintf("TSI_KS_Write_SRAM failed!\n");
					TSI_Print_Error(ret);
					break;
				}
				sysprintf("Write to Key Store SRAM key %d success.\n", knum);
				_key_records[knum].used = 1;
				_key_records[knum].key_size = _my_keys[kidx].size;
				kidx = (kidx + 1) % KS_SRAM_KEY_CNT;
				break;

			case '2':
				ks_erase_one_key();
				break;

			case '3':
				ks_revoke_one_key();
				break;

			case '4':
				TSI_KS_EraseAll();
				memset(_key_records, 0, sizeof(_key_records));
				break;

			default:
				sysprintf("Not supported.\n");
				break;
		}
		sysprintf("\nPress any key to continue...\n");
		sysgetchar();
	}
}

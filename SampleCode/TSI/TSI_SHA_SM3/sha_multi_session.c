/**************************************************************************//**
 * @file     sha_kat.c
 * @brief    SHA known answer test
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"

#define SN_CNT		4         /* session count, must be <= 4 */
#define BUFF_LEN	0x10000
#define TEST_LEN	(0x4000+32)
#define BLK_LEN    	4096

static uint8_t  _msg_mem[SN_CNT][BUFF_LEN] __attribute__((aligned(32)));
static uint8_t  _digest_mem[SN_CNT][256] __attribute__((aligned(32)));

extern int TSI_run_sha(int inswap, int outswap, int mode_sel, int hmac,
					   int mode, int keylen, int ks, int ks_num,
					   int wcnt, int data_cnt, uint32_t src_addr, uint32_t dest_addr);

int  SHA_Multi_Session_Test()
{
	uint8_t  *sha_msg[SN_CNT];
	uint32_t  *digest_out[SN_CNT];	
	int      sid[SN_CNT];
	int      i, j, dcnt, mlen, offs, ret;

	sysprintf("\n\n+---------------------------------------+\n");
	sysprintf("|  MA35D1 SHA Multi-Sessions Test       |\n");
	sysprintf("+---------------------------------------+\n");

	sysprintf("Generate test patterns.\n");

	for (i = 0; i < SN_CNT; i++)
	{
		sha_msg[i] = nc_ptr(&_msg_mem[i][0]);
		digest_out[i] = nc_ptr(&_digest_mem[i][0]);

		for (j = 0; j < TEST_LEN; j++)
			sha_msg[i][j] = (i << 2) | (j << 1) + 1;
	}

	sysprintf("Run SHA answers\n");

	for (i = 0; i < SN_CNT; i++)
	{
		if (TSI_run_sha(1,                /* inswap        */
						1,                /* outswap       */
						0,                /* mode_sel      */
						0,                /* hmac          */
						SHA_MODE_SHA256,  /* mode          */
					    0,                /* keylen (hmac) */
					    0,                /* ks            */
					    0,                /* ks_num        */
					    8,                /* dogest wcnt   */
					    TEST_LEN,         /* data_cnt      */
					    ptr_to_u32(&sha_msg[i][0]),   /* src_addr      */
					    ptr_to_u32(&digest_out[i][0]) /* dest_addr     */
					    ) != 0)

		{
			sysprintf("\nTSI SHA command test failed!!\n");
			while (1);
		}
		
		sysprintf("ANS%d: ", i);
		for (j = 0; j < 8; j++)
			sysprintf("%x ", digest_out[i][j]);
		sysprintf("\n");
	}

	sysprintf("Run multi-sessions SHA answers\n");

	for (i = 0; i < SN_CNT; i++)
	{
		ret = TSI_Open_Session(C_CODE_SHA, &sid[i]);
		if (ret != 0)
		{
			sysprintf("Failed to open SHA session! err = 0x%x\n", ret);
			while (1);
		}
		sysprintf("Open SHA session: 0x%x\n", sid[i]);
	}

	for (i = 0; i < SN_CNT; i++)
	{
		ret = TSI_SHA_Start(sid[i], 1, 1, SHA_MODE_SEL_SHA2, 0, SHA_MODE_SHA256, 0, 0, 0);
		if (ret != 0)
		{
			sysprintf("TSI_SHA_Start failed! err = 0x%x\n", ret);
			while (1);
		}
		sysprintf("[0x%x] TSI_SHA_Start done.\n", sid[i]);
	}

	sysprintf("Run SHA update/final...\n");

	for (dcnt = TEST_LEN, offs = 0; dcnt > 0; offs += mlen)
	{
		if (dcnt > BLK_LEN)
			mlen = BLK_LEN;
		else
			mlen = dcnt;
			
		dcnt -= mlen;

		sysprintf("dcnt = %d, mlen = %d\n", dcnt, mlen);

		for (i = 0; i < SN_CNT; i++)
		{
			if (dcnt)			
			{
				ret = TSI_SHA_Update(sid[i], mlen, ptr_to_u32(&sha_msg[i][offs]));
				if (ret != 0)
				{
					sysprintf("Session %d, TSI_SHA_Update failed!\n", i);
					while (1);
				}
			}
			else
			{
				ret = TSI_SHA_Finish(sid[i], 8, mlen, ptr_to_u32(&sha_msg[i][offs]),
									 ptr_to_u32(&digest_out[i][0]));
				if (ret != 0)
				{
					sysprintf("Session %d, TSI_SHA_Finish failed!\n", i);
					while (1);
				}
			}
		}
	}

	for (i = 0; i < SN_CNT; i++)
	{
		sysprintf("OUT%d: ", i);
		for (j = 0; j < 8; j++)
			sysprintf("%x ", digest_out[i][j]);
		sysprintf("\n");
	}

	for (i = 0; i < SN_CNT; i++)
		TSI_Close_Session(C_CODE_SHA, sid[i]);

	while (1);
}

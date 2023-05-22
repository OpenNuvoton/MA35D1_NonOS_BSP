/**************************************************************************//**
 * @file     sm3_kat.c
 * @brief    SM3 known answer test
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"

static uint8_t  tsi_dgst_mem[256] __attribute__((aligned(32)));
static uint8_t  sm3_msg_mem[1024] __attribute__((aligned(32)));
static uint8_t  g_sm3_digest[1024] __attribute__((aligned(4)));
static uint8_t  *g_sm3_msg;
static int      g_msg_len;

extern void do_swap(uint8_t *buff, int len);
extern int  do_compare(uint32_t *output, uint32_t *expect, int cmp_len);
extern int TSI_run_sha(int inswap, int outswap, int mode_sel, int hmac,
				int mode, int keylen, int ks, int ks_num,
				int wcnt, int data_cnt, uint32_t src_addr, uint32_t dest_addr);

struct sm3_tv_t
{
	char      plaintext[1024];
	int       len;
	uint8_t   digest[32];
};

static struct sm3_tv_t  _sm3_vector[] =
{
	{
		"a", 1,
		0x62, 0x34, 0x76, 0xAC, 0x18, 0xF6, 0x5A, 0x29,
		0x09, 0xE4, 0x3C, 0x7F, 0xEC, 0x61, 0xB4, 0x9C,
		0x7E, 0x76, 0x4A, 0x91, 0xA1, 0x8C, 0xCB, 0x82,
		0xF1, 0x91, 0x7A, 0x29, 0xC8, 0x6C, 0x5E, 0x88
	},
	{   /* A.1. Example 1 */
		"abc", 3,
		0x66, 0xC7, 0xF0, 0xF4, 0x62, 0xEE, 0xED, 0xD9,
		0xD1, 0xF2, 0xD4, 0x6B, 0xDC, 0x10, 0xE4, 0xE2,
		0x41, 0x67, 0xC4, 0x87, 0x5C, 0xF2, 0xF7, 0xA2,
		0x29, 0x7D, 0xA0, 0x2B, 0x8F, 0x4B, 0xA8, 0xE0
	},
	{
		"abcdefghijklmnopqrstuvwxyz", 26,
		0xB8, 0x0F, 0xE9, 0x7A, 0x4D, 0xA2, 0x4A, 0xFC,
		0x27, 0x75, 0x64, 0xF6, 0x6A, 0x35, 0x9E, 0xF4,
		0x40, 0x46, 0x2A, 0xD2, 0x8D, 0xCC, 0x6D, 0x63,
		0xAD, 0xB2, 0x4D, 0x5C, 0x20, 0xA6, 0x15, 0x95
	},
	{
		/* A.1. Example 2 */
		"abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd", 64,
		0xDE, 0xBE, 0x9F, 0xF9, 0x22, 0x75, 0xB8, 0xA1,
		0x38, 0x60, 0x48, 0x89, 0xC1, 0x8E, 0x5A, 0x4D,
		0x6F, 0xDB, 0x70, 0xE5, 0x38, 0x7E, 0x57, 0x65,
		0x29, 0x3D, 0xCB, 0xA3, 0x9C, 0x0C, 0x57, 0x32
	},
	{
		"abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd", 256,
		0xB9, 0x65, 0x76, 0x4C, 0x8B, 0xEB, 0xB0, 0x91,
		0xC7, 0x60, 0x2B, 0x74, 0xAF, 0xD3, 0x4E, 0xEF,
		0xB5, 0x31, 0xDC, 0xCB, 0x4E, 0x00, 0x76, 0xD9,
		0xB7, 0xCD, 0x81, 0x31, 0x99, 0xB4, 0x59, 0x71
	}
};

int  SM3_test()
{
	uint32_t  *answer_digest = nc_ptr(tsi_dgst_mem);

	if (TSI_run_sha(1,                          /* inswap        */
					1,                          /* outswap       */
					SHA_MODE_SEL_SM3,           /* mode_sel      */
					0,                          /* hmac          */
					SHA_MODE_SHA256,            /* mode          */
					0,                          /* keylen (hmac) */
					0,                          /* ks            */
					0,                          /* ks_num        */
					256/32,                     /* wcnt          */
					g_msg_len,                  /* data_cnt      */
					ptr_to_u32(g_sm3_msg),      /* src_addr      */
					ptr_to_u32(answer_digest)   /* dest_addr     */
					) != 0)
	{
		sysprintf("\nTSI SHA command test failed!!\n");
		while (1);
	}

	do_swap((uint8_t *)&g_sm3_digest[0], 256/8);
	if (do_compare(answer_digest, (uint32_t *)g_sm3_digest, 256/32) < 0)
	{
		while (1);
	}
	do_swap((uint8_t *)&g_sm3_digest[0], 256/8);

	/*--------------------------------------------*/
	/*  INSWAP test                               */
	/*--------------------------------------------*/
	sysprintf("SHA DMA mode test with INSWAP...\n");

	if (TSI_run_sha(1,                          /* inswap        */
					0,                          /* outswap       */
					SHA_MODE_SEL_SM3,           /* mode_sel      */
					0,                          /* hmac          */
					SHA_MODE_SHA256,            /* mode          */
					0,                          /* keylen (hmac) */
					0,                          /* ks            */
					0,                          /* ks_num        */
					256/32,                     /* wcnt          */
					g_msg_len,                  /* data_cnt      */
					ptr_to_u32(g_sm3_msg),      /* src_addr      */
					ptr_to_u32(answer_digest)   /* dest_addr     */
					) != 0)
	{
		sysprintf("\nTSI SHA command test failed!!\n");
		while (1);
	}
	if (do_compare(answer_digest, (uint32_t *)g_sm3_digest, 256/32) < 0)
	{
		while (1);
	}

	/*--------------------------------------------*/
	/*  No INSWAP & OUTSWAP test                  */
	/*--------------------------------------------*/
	sysprintf("SHA DMA mode test without SWAP...\n");
	do_swap(g_sm3_msg, g_msg_len);

	if (TSI_run_sha(0,                          /* inswap        */
					0,                          /* outswap       */
					SHA_MODE_SEL_SM3,           /* mode_sel      */
					0,                          /* hmac          */
					SHA_MODE_SHA256,            /* mode          */
					0,                          /* keylen (hmac) */
					0,                          /* ks            */
					0,                          /* ks_num        */
					256/32,                     /* wcnt          */
					g_msg_len,                  /* data_cnt      */
					ptr_to_u32(g_sm3_msg),      /* src_addr      */
					ptr_to_u32(answer_digest)   /* dest_addr     */
					) != 0)
	{
		sysprintf("\nTSI SHA command test failed!!\n");
		while (1);
	}
	if (do_compare(answer_digest, (uint32_t *)g_sm3_digest, 256/32) < 0)
	{
		while (1);
	}
	do_swap(g_sm3_msg, g_msg_len);
	return 0;
}

int  SM3_KAT()
{
	int   i;
	struct sm3_tv_t  *tv;

	sysprintf("\n\n+---------------------------------------+\n");
	sysprintf("|  MA35D1 SM3 Known-Answer-Test         |\n");
	sysprintf("+---------------------------------------+\n");

	g_sm3_msg = nc_ptr(sm3_msg_mem);

	/* SM3 digest is inversed to us */
	for (i = 0; i < sizeof(_sm3_vector)/sizeof(struct sm3_tv_t); i++)
		do_swap(&(_sm3_vector[i].digest[0]), 32);

	for (i = 0; i < sizeof(_sm3_vector)/sizeof(struct sm3_tv_t); i++)
	{
		sysprintf("Run SM3 test vector %d\n", i+1);
		g_msg_len = _sm3_vector[i].len;
		memcpy(g_sm3_msg, _sm3_vector[i].plaintext, strlen(_sm3_vector[i].plaintext));
		memcpy(g_sm3_digest, _sm3_vector[i].digest, 32);
		SM3_test();
		sysprintf("[PASS]\n");
	}
	sysprintf("\nSM3 KAT done.\n");
	return 0;
}

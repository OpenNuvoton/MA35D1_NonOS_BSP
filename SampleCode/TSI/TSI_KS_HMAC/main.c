/**************************************************************************//**
 * @file     main.c
 * @brief    This sample program demonstrates how to use the TSI commands to
 *           perform HMAC-SHA operation with HMAC keys from Key Store.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"

struct hmac_context {
	uint8_t  hmac_msg[4096];
	uint8_t  hmac_key[1024];
	uint8_t  mac[256];
	uint32_t out_dgst[64];
	int      key_len;
	int      msg_len;
	int      mac_len;
	int      dgst_len;
	int      sha_mode;
};
struct hmac_context  hmac_ctx_mem;

extern uint32_t  VectorDataBase_HMAC, VectorDataLimit_HMAC;
static uint8_t    *file_base;
static uint32_t   file_idx, file_size;
char  g_line[24*1024];

static void init_test_file(void)
{
	file_base = (uint8_t *)&VectorDataBase_HMAC;
	file_size = ptr_to_u32(&VectorDataLimit_HMAC) - ptr_to_u32(&VectorDataBase_HMAC);
	file_idx = 0;
}

static int  read_file(uint8_t *buffer, int length)
{
	if (file_idx + 1 >= file_size)
		return -1;
	memcpy(buffer, &file_base[file_idx], length);
	file_idx += length;
	return 0;
}

static int  get_line(void)
{
	int         i;
	uint8_t     ch;

	if (file_idx + 1 >= file_size)
		return -1;   // EOF

	for (i = 0;  ; i++)
	{
		if (read_file(&ch, 1) < 0)
			return 0;

		if (ch == 0x0D)
		{
			if (read_file(&ch, 1) < 0)
				return -1;

			if (ch != 0x0A)
			{
				sysprintf("%d - 0x0D 0x0A error!\n", __LINE__);
				while (1);
			}
			break;
		}

		g_line[i] = ch;
	}
	g_line[i] = 0;
	return 0;
}

static int is_hex_char(char c)
{
	if ((c >= '0') && (c <= '9'))
		return 1;
	if ((c >= 'a') && (c <= 'f'))
		return 1;
	if ((c >= 'A') && (c <= 'F'))
		return 1;
	return 0;
}

static uint8_t  char_to_hex(uint8_t c)
{
	if ((c >= '0') && (c <= '9'))
		return c - '0';
	if ((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;
	if ((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;
	return 0;
}

static int  str_to_hex(uint8_t *str, uint8_t *hex, int swap)
{
	int         i, count = 0, actual_len;
	uint8_t     val8;

	while (*str)
	{
		if (!is_hex_char(*str))
			return count;

		val8 = char_to_hex(*str);
		str++;

		if (!is_hex_char(*str))
			return count;

		val8 = (val8 << 4) | char_to_hex(*str);
		str++;

		hex[count] = val8;
		//sysprintf("hex = 0x%x\n", val8);
		count++;
	}

	actual_len = count;

	for ( ; count % 4 ; count++)
		hex[count] = 0;

	if (!swap)
		return actual_len;

	// SWAP
	for (i = 0; i < count; i+=4)
	{
		val8 = hex[i];
		hex[i] = hex[i+3];
		hex[i+3] = val8;

		val8 = hex[i+1];
		hex[i+1] = hex[i+2];
		hex[i+2] = val8;
	}
	return actual_len;
}

static int  str_to_decimal(char *str)
{
	int         val32;
	uint8_t     val8;

	val32 = 0;
	while (*str)
	{
		if ((*str < '0') || (*str > '9'))
		{
			return val32;
		}
		val32 = (val32 * 10) + (*str - '0');
		str++;
	}
	return val32;
}

static char * find_alphanumeric(char *p)
{
	for (; *p != '\0'; p++)
	{
		if (((*p >= 'A') && (*p <= 'Z')) || ((*p >= 'a') && (*p <= 'z')) ||
			((*p >= '0') && (*p <= '9')))
			return p;
	}
	return NULL;
}

static int line_num = 0;

int  get_next_pattern(struct hmac_context *ctx)
{
	int    line_num = 1;
	int    blen;
	char   *p;

	ctx->key_len = 0;

	memset(ctx->hmac_msg, 0x0, 128);

	while (get_line() == 0)
	{
		line_num++;

		if (g_line[0] == '#')
			continue;

		p = find_alphanumeric(g_line);

		if (p == NULL)
			continue;

		if (strncmp(p ,"Klen", 4) == 0)
		{
			p += 4;
			p = find_alphanumeric(p);
			ctx->key_len = str_to_decimal(p);
			sysprintf("Key length = %d\n", ctx->key_len);
			continue;
		}

		if (strncmp(p ,"Tlen", 4) == 0)
		{
			p += 4;
			p = find_alphanumeric(p);
			ctx->mac_len = str_to_decimal(p);
			sysprintf("HMAC length = %d\n", ctx->mac_len);
			continue;
		}

		if (strncmp(p ,"Key", 3) == 0)
		{
			p += 3;
			p = find_alphanumeric(p);
			if (str_to_hex(p, ctx->hmac_key, 0) != ctx->key_len)
			{
				sysprintf("key len mismatch!\n");
				return -1;
			}
			continue;
		}

		if (strncmp(p ,"Msg", 3) == 0)
		{
			p += 3;
			p = find_alphanumeric(p);
			ctx->msg_len = str_to_hex(p, ctx->hmac_msg, 0);
			sysprintf("Message length = %d\n", ctx->msg_len);
			continue;
		}

		if (strncmp(p ,"Mac", 3) == 0)
		{
			sysprintf("%s\n", p);
			p += 3;
			p = find_alphanumeric(p);
			str_to_hex(p, ctx->mac, 0);
			return 0;
		}

		if ((strncmp(p ,"L=", 2) == 0) || (strncmp(p ,"L =", 3) == 0))
		{
			p += 2;
			p = find_alphanumeric(p);
			ctx->dgst_len = str_to_decimal(p);
			switch (ctx->dgst_len)
			{
				case 20:
					ctx->sha_mode = SHA_MODE_SHA1;
					sysprintf("SHA1...\n");
					break;
				case 28:
					ctx->sha_mode = SHA_MODE_SHA224;
					sysprintf("SHA224...\n");
					break;
				case 32:
					sysprintf("SHA256...\n");
					ctx->sha_mode = SHA_MODE_SHA256;
					break;
				case 48:
					sysprintf("SHA384...\n");
					ctx->sha_mode = SHA_MODE_SHA384;
					break;
				case 64:
					sysprintf("SHA512...\n");
					ctx->sha_mode = SHA_MODE_SHA512;
					break;
			}
		}
		sysprintf("LINE %d = %s\n", line_num, g_line);
	}
	return -1;
}

void do_swap(uint8_t *buff, int len)
{
	int       i;
	uint8_t   val8;

	len = (len+3) & 0xfffffffc;
	for (i = 0; i < len; i+=4)
	{
		val8 = buff[i];
		buff[i] = buff[i+3];
		buff[i+3] = val8;
		val8 = buff[i+1];
		buff[i+1] = buff[i+2];
		buff[i+2] = val8;
	}
}

static int  do_compare(uint8_t *output, uint8_t *expect, int cmp_len)
{
	int   i;

	if (memcmp(expect, output, cmp_len))
	{
		sysprintf("\nMismatch!! - %d\n", cmp_len);
		for (i = 0; i < cmp_len; i++)
			sysprintf("0x%02x    0x%02x\n", expect[i], output[i]);
		return -1;
	}
	return 0;
}

static int TSI_run_sha(int inswap, int outswap, int mode_sel, int hmac,
				int mode, int keylen, int ks, int ks_num,
				int wcnt, int data_cnt, uint32_t src_addr, uint32_t dest_addr)
{
	int   sid, ret, dptr, run_len;

	ret = TSI_Open_Session(C_CODE_SHA, &sid);
	if (ret != 0)
		goto err_out;

	ret = TSI_SHA_Start(sid, inswap, outswap, mode_sel, hmac, mode, keylen, ks, ks_num);
	if (ret != 0)
		goto err_out;

	ret = TSI_SHA_Finish(sid, wcnt, data_cnt, src_addr, dest_addr);
	if (ret != 0)
		goto err_out;

	TSI_Close_Session(C_CODE_SHA, sid);
	return 0;

err_out:
	sysprintf("TSI ERROR!!! 0x%x\n", ret);
	TSI_Close_Session(C_CODE_SHA, sid);
	TSI_Print_Error(ret);
	return ret;
}

static int  do_tsi_hmac_test(struct hmac_context *ctx, int ks_num)
{
	int         ret, sid, mode_sel, mode, remain_len;
	uint32_t    msg_addr, update_len;

	if (ctx->dgst_len == 20)
	{
		mode_sel = SHA_MODE_SEL_SHA1;
		mode = SHA_MODE_SHA1;
	}
	else if (ctx->dgst_len == 28)
	{
		mode_sel = SHA_MODE_SEL_SHA2;
		mode = SHA_MODE_SHA224;
	}
	else if (ctx->dgst_len == 32)
	{
		mode_sel = SHA_MODE_SEL_SHA2;
		mode = SHA_MODE_SHA256;
	}
	else if (ctx->dgst_len == 48)
	{
		mode_sel = SHA_MODE_SEL_SHA2;
		mode = SHA_MODE_SHA384;
	}
	else if (ctx->dgst_len == 64)
	{
		mode_sel = SHA_MODE_SEL_SHA2;
		mode = SHA_MODE_SHA512;
	}
	else
	{
		sysprintf("Invalid digest length %d!\n", ctx->dgst_len);
		return -1;
	}

	if (TSI_run_sha(1,                         /* inswap        */
					1,                         /* outswap       */
					0,                         /* mode_sel      */
					1,                         /* hmac          */
					ctx->sha_mode,             /* mode          */
					0,                         /* keylen (hmac) */
					1,                         /* ks            */
					ks_num,                    /* ks_num        */
					ctx->dgst_len / 4,         /* wcnt          */
					ctx->msg_len,              /* data_cnt      */
					ptr_to_u32(ctx->hmac_msg), /* src_addr      */
					ptr_to_u32(ctx->out_dgst)  /* dest_addr     */
					) != 0)
	{
		sysprintf("\nTSI SHA command test failed!!\n");
		while (1);
	}

	/*--------------------------------------------*/
	/*  Compare                                   */
	/*--------------------------------------------*/
	if (do_compare((uint8_t *)ctx->out_dgst, ctx->mac, ctx->mac_len) != 0)
	{
		while (1);
	}
	sysprintf("Data verify OK.\n\n");
	return 0;

err_out:
	TSI_Print_Error(ret);
	return ret;
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
	int   ret, key_num, keysz;
	struct hmac_context  *ctx;

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

	sysprintf("\n\n");
	sysprintf("+---------------------------------------------------------------+\n");
	sysprintf("|  MA35D1 TSI HAMC-SHA demo with HMAC keys from Key Store.      |\n");
	sysprintf("+---------------------------------------------------------------+\n");

	ctx = nc_ptr(&hmac_ctx_mem);

	init_test_file();

	while (1)
	{
		memset(ctx->hmac_key, 0, 256);

		if (get_next_pattern(ctx) < 0)
			break;

		TSI_KS_EraseAll();

		if (ctx->dgst_len > 32)
		{
			/* block size > 512 bits */
			keysz = KS_META_1024;

			if (ctx->key_len > 128)
			{
				sysprintf("Key length > 1024 bits, not supported. Skip this test vector.\n");
				continue;
			}
		}
		else
		{
			/* block size is 512 bits */
			keysz = KS_META_512;

			if (ctx->key_len > 64)
			{
				sysprintf("Key length > 512 bits, not supported. Skip this test vector.\n");
				continue;
			}
		}

		do_swap(ctx->hmac_key, 128);

		ret = TSI_KS_Write_SRAM(KS_META_HMAC | keysz, (uint32_t *)ctx->hmac_key, &key_num);
		if (ret != 0)
		{
			sysprintf("Write KS_SRAM key failed!\n");
			TSI_Print_Error(ret);
			while (1);
		}

		ret = do_tsi_hmac_test(ctx, key_num);
		if (ret != 0)
		{
			sysprintf("HMAC test failed!!\n");
			return -1;
		}

		if (key_num >= 31)
				TSI_KS_EraseAll();
	}
	sysprintf("\n\nAll test vector passed.\n");
	while (1);
}

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

#define TSI_SHA_BLK_LEN    128

static uint8_t  sha_msg_mem[0x10000] __attribute__((aligned(32)));
static uint8_t  tsi_dgst_mem[256] __attribute__((aligned(32)));
static uint8_t  g_sha_digest[1024] __attribute__((aligned(4)));

static uint8_t    *g_sha_msg;
static int        g_msg_bit_len;
static int        g_digest_len;
static int        g_sha_mode;

extern uint32_t VectorDataBase, VectorDataLimit;

static uint8_t  *file_base;
static uint32_t file_idx, file_size;

static char  g_line[64*1024];

static int  read_file(uint8_t *buffer, int length)
{
	if (file_idx+1 >= file_size)
		return -1;
	memcpy(buffer, &file_base[file_idx], length);
	file_idx += length;
	return 0;
}

static int  get_line(void)
{
	int         i;
	uint8_t     ch;

	if (file_idx + 1 >= file_size)  // EOF
		return -1;

	//memset(g_line, 0, sizeof(g_line));
	for (i = 0;  ; i++)
	{
		g_line[i] = 0;
		if (read_file(&ch, 1) < 0)
			return 0;
		if ((ch == 0x0D) || (ch == 0x0A))
			break;
		g_line[i] = ch;
	}
	while (1)
	{
		if (read_file(&ch, 1) < 0)
			return 0;
		if ((ch != 0x0D) && (ch != 0x0A))
			break;
	}
	file_idx--;
	return 0;
}

static int  is_hex_char(char c)
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
		{
			//sysprintf("ERROR - not hex!!\n");
			return count;
		}
		val8 = char_to_hex(*str);
		str++;
		if (!is_hex_char(*str))
		{
			//sysprintf("ERROR - not hex!!\n");
			return count;
		}
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

	/* SWAP */
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

static int  str_to_decimal(uint8_t *str)
{
	int     val32;
	uint8_t val8;

	val32 = 0;
	while (*str)
	{
		if ((*str < '0') || (*str > '9'))
			return val32;
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

static int  get_next_pattern(void)
{
	int     line_num = 1;
	char    *p;

	while (get_line() == 0)
	{
		// sysprintf("LINE %d = %s\n", line_num, g_line);
		line_num++;

		if (g_line[0] == '#')
			continue;

		p = find_alphanumeric(g_line);

		if ((strncmp(p ,"L =", 3) == 0) || (strncmp(p ,"L=", 2) == 0))
		{
			p++;
			p = find_alphanumeric(p);
			g_digest_len = str_to_decimal(p);
			continue;
		}
		if (strncmp(p, "Len", 3) == 0)
		{
			p += 3;
			p = find_alphanumeric(p);
			g_msg_bit_len = str_to_decimal(p);
			continue;
		}
		if (strncmp(p, "Msg", 3) == 0)
		{
			p += 3;
			p = find_alphanumeric(p);
			str_to_hex(p, &g_sha_msg[0], 0);
			continue;
		}
		if (strncmp(p, "MD", 2) == 0)
		{
			p += 2;
			p = find_alphanumeric(p);
			str_to_hex(p, &g_sha_digest[0], 1);
			return 0;
		}
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

int  do_compare(uint32_t *output, uint32_t *expect, int cmp_len)
{
	int   i;

	for (i = 0; i < cmp_len; i++)
	{
		if (output[i] != expect[i])
		{
			sysprintf("\nMismatch!! - %d\n", cmp_len);
			for (i = 0; i < cmp_len; i++)
				sysprintf("expect: 0x%0x    SHA digest: 0x%0x\n", expect[i], output[i]);
			return -1;
		}
	}
	return 0;
}

int TSI_run_sha(int inswap, int outswap, int mode_sel, int hmac,
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

	for (dptr = 0; dptr < data_cnt; dptr += run_len)   // SHA-512 block size is 1024 bits
	{
		run_len = data_cnt - dptr;
		if (run_len > TSI_SHA_BLK_LEN)
			run_len = TSI_SHA_BLK_LEN;

		if (run_len < TSI_SHA_BLK_LEN)
		{
			// sysprintf(" Finish %d\n", run_len);
			ret = TSI_SHA_Finish(sid, wcnt, run_len, src_addr + dptr, dest_addr);
			if (ret != 0)
				goto err_out;
			TSI_Close_Session(C_CODE_SHA, sid);
			return 0;
		}
		else
		{
			// sysprintf(".");
			ret = TSI_SHA_Update(sid, TSI_SHA_BLK_LEN, src_addr + dptr);
			if (ret != 0)
				goto err_out;
		}
	}

	// sysprintf("\nTSI_SHA_Finish 0.\n");
	ret = TSI_SHA_Finish(sid, wcnt, 0, src_addr + dptr, dest_addr);
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

static int SHA_test()
{
	uint32_t  *answer_digest = nc_ptr(tsi_dgst_mem);

	if ((g_msg_bit_len < 8) || (g_msg_bit_len % 8) != 0)
	{
		sysprintf("Key length is %d, not multiple of 8!\n", g_msg_bit_len);
		return 0;
	}
	sysprintf("data len = %d\n", g_msg_bit_len/8);

	switch (g_digest_len)
	{
		case 20:
			g_sha_mode = SHA_MODE_SHA1;
			break;
		case 28:
			g_sha_mode = SHA_MODE_SHA224;
			break;
		case 32:
			g_sha_mode = SHA_MODE_SHA256;
			break;
		case 48:
			g_sha_mode = SHA_MODE_SHA384;
			break;
		case 64:
			g_sha_mode = SHA_MODE_SHA512;
			break;
		default:
			sysprintf("g_digest_len = %d is wrong!!\n", g_digest_len);
			return -1;
	}

	sysprintf("SHA with INSWAP & OUTSWAP...\n");

	if (TSI_run_sha(1,                        /* inswap        */
					1,                        /* outswap       */
					0,                        /* mode_sel      */
					0,                        /* hmac          */
					g_sha_mode,               /* mode          */
					0,                        /* keylen (hmac) */
					0,                        /* ks            */
					0,                        /* ks_num        */
					g_digest_len/4,           /* wcnt          */
					g_msg_bit_len/8,          /* data_cnt      */
					ptr_to_u32(g_sha_msg),    /* src_addr      */
					ptr_to_u32(answer_digest) /* dest_addr     */
					) != 0)
	{
		sysprintf("\nTSI SHA command test failed!!\n");
		while (1);
	}
	do_swap((uint8_t *)&g_sha_digest[0], g_digest_len);

	if (do_compare(answer_digest, (uint32_t *)g_sha_digest, g_digest_len/4) < 0)
		while (1);

	do_swap((uint8_t *)&g_sha_digest[0], g_digest_len);

	/*--------------------------------------------*/
	/*  INSWAP test                               */
	/*--------------------------------------------*/
	sysprintf("SHA with INSWAP...\n");

	if (TSI_run_sha(1,                        /* inswap        */
					0,                        /* outswap       */
					0,                        /* mode_sel      */
					0,                        /* hmac          */
					g_sha_mode,               /* mode          */
					0,                        /* keylen (hmac) */
					0,                        /* ks            */
					0,                        /* ks_num        */
					g_digest_len/4,           /* wcnt          */
					g_msg_bit_len/8,          /* data_cnt      */
					ptr_to_u32(g_sha_msg),    /* src_addr      */
					ptr_to_u32(answer_digest) /* dest_addr     */
					) != 0)
	{
		sysprintf("\nTSI SHA command test failed!!\n");
		while (1);
	}
	if (do_compare(answer_digest, (uint32_t *)g_sha_digest, g_digest_len/4) < 0)
	{
		while (1);
	}

	/*--------------------------------------------*/
	/*  No INSWAP & OUTSWAP test                  */
	/*--------------------------------------------*/
	sysprintf("SHA without SWAP...\n");

	do_swap(g_sha_msg, g_msg_bit_len/8);

	if (TSI_run_sha(0,                        /* inswap        */
					0,                        /* outswap       */
					0,                        /* mode_sel      */
					0,                        /* hmac          */
					g_sha_mode,               /* mode          */
					0,                        /* keylen (hmac) */
					0,                        /* ks            */
					0,                        /* ks_num        */
					g_digest_len/4,           /* wcnt          */
					g_msg_bit_len/8,          /* data_cnt      */
					ptr_to_u32(g_sha_msg),    /* src_addr      */
					ptr_to_u32(answer_digest) /* dest_addr     */
					) != 0)
	{
		sysprintf("\nTSI SHA command test failed!!\n");
		while (1);
	}
	if (do_compare(answer_digest, (uint32_t *)g_sha_digest, g_digest_len/4) < 0)
	{
		while (1);
	}
	do_swap(g_sha_msg, g_msg_bit_len/8);
	return 0;
}

int  SHA_KAT()
{
	int  loop;

	sysprintf("\n\n+---------------------------------------+\n");
	sysprintf("|  MA35D1 SHA Known-Answer-Test         |\n");
	sysprintf("+---------------------------------------+\n");

	g_sha_msg = nc_ptr(sha_msg_mem);

	file_base = nc_ptr(&VectorDataBase);
	file_size = ptr_to_u32(&VectorDataLimit) - ptr_to_u32(&VectorDataBase);
	file_idx = 0;

	for (loop = 1; ; loop++)
	{
		sysprintf("Run test vector %d...\n", loop);

		if (get_next_pattern() < 0)
			break;
		SHA_test();
	}
	sysprintf("\nSHA KAT done.\n");
	return 0;
}

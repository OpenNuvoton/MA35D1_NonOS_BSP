/**************************************************************************//**
 * @file     hmac_md5_kat.c
 * @brief    HMAC and MD5 known answer test
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"

extern uint32_t  VectorDataBase_HMAC, VectorDataLimit_HMAC;
extern uint32_t  VectorDataBase_MD5, VectorDataLimit_MD5;

static uint8_t *file_base;
static uint32_t file_idx, file_size;

static uint8_t  hmac_msg_mem[0x10000] __attribute__((aligned(32)));
static char     g_line[64*1024];

uint32_t tsi_dgst_buf[16];
uint8_t  g_hmac_mac[4096];

uint8_t  *g_hmac_msg;

int      g_key_len, g_msg_len, g_mac_len, g_digest_len;
uint32_t g_sha_mode;

static int  read_file(uint8_t *buffer, int length)
{
	if (file_idx+1 >= file_size)
		return -1;
	memcpy(buffer, &file_base[file_idx], length);
	file_idx += length;
	return 0;
}

int  get_line(void)
{
	int         i;
	uint8_t     ch;

	if (file_idx+1 >= file_size)
	{
		//sysprintf("EOF.\n");
		return -1;
	}
	// memset(g_line, 0, sizeof(g_line));
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

int  is_hex_char(char c)
{
	if ((c >= '0') && (c <= '9'))
		return 1;
	if ((c >= 'a') && (c <= 'f'))
		return 1;
	if ((c >= 'A') && (c <= 'F'))
		return 1;
	return 0;
}

uint8_t  char_to_hex(uint8_t c)
{
	if ((c >= '0') && (c <= '9'))
		return c - '0';
	if ((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;
	if ((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;
	return 0;
}

int  str_to_hex(uint8_t *str, uint8_t *hex, int swap)
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

int  str_to_decimal(uint8_t *str)
{
	int     val32;

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
	if (p == NULL)
		return NULL;

	for (; *p != '\0'; p++)
	{
		if (((*p >= 'A') && (*p <= 'Z')) || ((*p >= 'a') && (*p <= 'z')) ||
			((*p >= '0') && (*p <= '9')))
			return p;
	}
	return NULL;
}

int  get_next_pattern(void)
{
	int    line_num = 1;
	char   *p;

	sysprintf("\nProgress %d%c...\n", (file_idx*100)/file_size, '%');

	g_key_len = 0;

	memset(g_hmac_msg, 0x0, 128);

	while (get_line() == 0)
	{
		line_num++;

		// sysprintf("%s\n", g_line);

		if (g_line[0] == '#')
			continue;

		p = find_alphanumeric(g_line);
		if (p == NULL)
			continue;

		if (strncmp(p ,"Klen", 4) == 0)
		{
			p += 4;
			p = find_alphanumeric(p);
			g_key_len = str_to_decimal(p);
			sysprintf("Key len = %d\n", g_key_len);
			continue;
		}
		if (strncmp(p ,"Tlen", 4) == 0)
		{
			p += 4;
			p = find_alphanumeric(p);
			g_mac_len = str_to_decimal(p);
			sysprintf("HMAC len = %d\n", g_mac_len);
			continue;
		}
		if (strncmp(p ,"Key", 3) == 0)
		{
			p += 3;
			p = find_alphanumeric(p);
			if (str_to_hex(p, &g_hmac_msg[0], 0) != g_key_len)
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
			g_msg_len = str_to_hex(p, &g_hmac_msg[(g_key_len+3)&0xfffffffc], 0);
			sysprintf("Message len = %d\n", g_msg_len);
			continue;
		}
		if (strncmp(p ,"Mac", 3) == 0)
		{
			p += 3;
			p = find_alphanumeric(p);
			str_to_hex(p, &g_hmac_mac[0], 0);
			return 0;
		}
		if ((strncmp(p ,"L=", 2) == 0) || (strncmp(p ,"L =", 3) == 0))
		{
			p += 2;
			p = find_alphanumeric(p);
			g_digest_len = str_to_decimal(p);
			switch (g_digest_len)
			{
				case 20:
					g_sha_mode = SHA_MODE_SHA1;
					sysprintf("SHA1...\n");
					break;
				case 28:
					g_sha_mode = SHA_MODE_SHA224;
					sysprintf("SHA224...\n");
					break;
				case 32:
					sysprintf("SHA256...\n");
					g_sha_mode = SHA_MODE_SHA256;
					break;
				case 48:
					sysprintf("SHA384...\n");
					g_sha_mode = SHA_MODE_SHA384;
					break;
				case 64:
					sysprintf("SHA512...\n");
					g_sha_mode = SHA_MODE_SHA512;
					break;
			}
		}
		//sysprintf("LINE %d = %s\n", line_num, g_line);
	}
	return -1;
}

int  do_compare(uint8_t *output, uint8_t *expect, int cmp_len)
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

int  hmac_md5_test(int md5)
{
	int         i;
	uint32_t    *dptr;
	uint32_t    *answer_digest;

	answer_digest = nc_ptr(tsi_dgst_buf);

	if (TSI_run_sha(1,                     /* inswap        */
			1,                             /* outswap       */
			(md5 ? SHA_MODE_SEL_MD5 : 0),  /* mode_sel      */
			1,                             /* hmac          */
			g_sha_mode,                    /* mode          */
			g_key_len,                     /* keylen (hmac) */
			0,                             /* ks            */
			0,                             /* ks_num        */
			g_digest_len/4,                /* wcnt          */
			g_msg_len + ((g_key_len + 3) & 0xfffffffc), /* data_cnt */
			ptr_to_u32(g_hmac_msg),        /* src_addr      */
			ptr_to_u32(tsi_dgst_buf)       /* dest_addr     */
			) != 0)
	{
		sysprintf("\nTSI SHA command test failed!!\n");
		while (1);
	}

	/*--------------------------------------------*/
	/*  Compare                                   */
	/*--------------------------------------------*/
	if (do_compare((uint8_t *)answer_digest, &g_hmac_mac[0], g_mac_len) < 0)
	{
		while (1);
	}
	sysprintf("Data verify OK.\n\n");
	return 0;
}

int  HMAC_KAT()
{
	int  ret;

	sysprintf("\n\n+---------------------------------------+\n");
	sysprintf("|  MA35D1 HMAC Known-Answer-Test        |\n");
	sysprintf("+---------------------------------------+\n");

	g_hmac_msg = nc_ptr(hmac_msg_mem);

	file_base = (uint8_t *)&VectorDataBase_HMAC;
	file_size = ptr_to_u32(&VectorDataLimit_HMAC) - ptr_to_u32(&VectorDataBase_HMAC);
	file_idx = 0;
	while (1)
	{
		if (get_next_pattern() < 0)
			break;

		hmac_md5_test(0);
	}
	sysprintf("\nHMAC KAT done.\n");
	return 0;
}

int  MD5_KAT()
{
	int  ret;

	sysprintf("\n\n+---------------------------------------+\n");
	sysprintf("|  MA35D1 MD5 Known-Answer-Test         |\n");
	sysprintf("+---------------------------------------+\n");

	g_hmac_msg = nc_ptr(hmac_msg_mem);

	file_base = (uint8_t *)&VectorDataBase_MD5;
	file_size = ptr_to_u32(&VectorDataLimit_MD5) - ptr_to_u32(&VectorDataBase_MD5);
	file_idx = 0;
	while (1)
	{
		if (get_next_pattern() < 0)
			break;

		hmac_md5_test(1);
	}
	sysprintf("\nMD5 KAT done.\n");
	return 0;
}

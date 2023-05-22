/**************************************************************************//**
 * @file     aes_gcm_kat.c
 * @brief    AES GCM mode known answer test
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"

#define MAX_IV_LEN     512
#define MAX_PT_LEN     2048
#define MAX_CT_LEN     2048
#define MAX_AAD_LEN    2048
#define MAX_TAG_LEN    512

static char      *file_base;
static uint32_t  file_idx, file_size;

static char  g_line[32*1024];

static uint8_t  g_src_buff_mem[32*1024] __attribute__((aligned(32)));
static uint8_t  g_dst_buff_mem[32*1024] __attribute__((aligned(32)));
static uint8_t  g_tsi_param_mem[256] __attribute__((aligned(32)));
static uint32_t fdbck_mem[24] __attribute__((aligned(32)));

static uint8_t  *g_src_buff;
static uint8_t  *g_dst_buff;
static uint32_t *fdbck;

static uint8_t  g_zeros[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
static int  pass_cnt = 0;

struct gcm_tv_t
{
	int      Keylen;
	int      IVlen;
	int      PTlen;
	int      AADlen;
	int      Taglen;
	int      Count;
	uint8_t  Key[64];
	uint8_t  IV[MAX_IV_LEN];
	uint8_t  PT[MAX_PT_LEN];
	uint8_t  AAD[MAX_AAD_LEN];
	uint8_t  CT[MAX_CT_LEN];
	uint8_t  Tag[MAX_TAG_LEN];
};

static struct gcm_tv_t   g_tv __attribute__((aligned(32)));
static struct gcm_tv_t   *p_tv;

extern uint32_t  VectorDataBase_GCM_Encrypt, VectorDataLimit_GCM_Encrypt;
extern uint32_t  VectorDataBase_GCM_Decrypt, VectorDataLimit_GCM_Decrypt;

static int  select_test_file(char *filename)
{
	if (strcmp(filename, "Encrypt") == 0)
	{
		file_base = (char *)&VectorDataBase_GCM_Encrypt;
		file_size = ptr_to_u32(&VectorDataLimit_GCM_Encrypt) - ptr_to_u32(&VectorDataBase_GCM_Encrypt);
	}
	else
	{
		file_base = (char *)&VectorDataBase_GCM_Decrypt;
		file_size = ptr_to_u32(&VectorDataLimit_GCM_Decrypt) - ptr_to_u32(&VectorDataBase_GCM_Decrypt);
	}
	file_idx = 0;
	return 1;
}

static int  read_file(uint8_t *buffer, int length)
{
	if (file_idx+1 >= file_size)
		return -1;
	memcpy(buffer, file_base+file_idx, length);
	file_idx += length;
	return 0;
}

static int  get_line(void)
{
	int         i;
	uint8_t     ch;

	if (file_idx+1 >= file_size)
	{
		//sysprintf("EOF.\n");
		return -1;
	}

	//memset(g_line, 0, sizeof(g_line));

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

#if 0
	while (1)
	{
		if (read_file(&ch, 1) < 0)
			return 0;

		if ((ch != 0x0D) && (ch != 0x0A))
			break;
	}
	file_idx--;
#endif
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

	if (str == NULL)
	{
		memset(hex, 0, 4);
		return 0;
	}

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

static int  str_to_decimal(uint8_t *str)
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

static int  get_next_pattern(void)
{
	int     line_num = 1;
	char    *p;
	int     count_found = 0;

	// sysprintf("get_next_pattern\n");
	while (1)
	{
		if (get_line() != 0)
			break;

		//sysprintf("LINE %d = %s\n", line_num, g_line);
		line_num++;

		if (g_line[0] == '#')
			continue;

		if (count_found && (g_line[0] == 0))
			return 0;

		p = find_alphanumeric(g_line);
		if (p == NULL)
			continue;           /* alphanumeric character not found, go to next line */

		if (strncmp(p, "Count", 5) == 0)
		{
			p += 5;
			p = find_alphanumeric(p);
			p_tv->Count = str_to_decimal(p);
			count_found = 1;
			continue;
		}
		if (strncmp(p, "Keylen", 6) == 0)
		{
			p += 6;
			p = find_alphanumeric(p);
			p_tv->Keylen = str_to_decimal(p);
			sysprintf("Keylen = %d\n", p_tv->Keylen);
			continue;
		}
		if (strncmp(p ,"IVlen", 5) == 0)
		{
			p += 5;
			p = find_alphanumeric(p);
			p_tv->IVlen = str_to_decimal(p) / 8;
			sysprintf("IVlen = %d\n", p_tv->IVlen);
			if (p_tv->IVlen > MAX_IV_LEN)
			{
				sysprintf("IVlen over limitation!\n");
				while (1);
			}
			continue;
		}
		if (strncmp(p ,"PTlen", 5) == 0)
		{
			p += 5;
			p = find_alphanumeric(p);
			p_tv->PTlen = str_to_decimal(p) / 8;
			sysprintf("PTlen = %d\n", p_tv->PTlen);
			if (p_tv->PTlen > MAX_PT_LEN)
			{
				sysprintf("PTlen over limitation!\n");
				while (1);
			}
			continue;
		}
		if (strncmp(p ,"AADlen", 6) == 0)
		{
			p += 6;
			p = find_alphanumeric(p);
			p_tv->AADlen = str_to_decimal(p) / 8;
			sysprintf("AADlen = %d\n", p_tv->AADlen);
			if (p_tv->AADlen > MAX_AAD_LEN)
			{
				sysprintf("AADlen over limitation!\n");
				while (1);
			}
			continue;
		}
		if (strncmp(p ,"Taglen", 6) == 0)
		{
			p += 6;
			p = find_alphanumeric(p);
			p_tv->Taglen = str_to_decimal(p) / 8;
			sysprintf("Taglen = %d\n", p_tv->Taglen);
			if (p_tv->Taglen > MAX_TAG_LEN)
			{
				sysprintf("Taglen over limitation!\n");
				while (1);
			}
			continue;
		}
		if (strncmp(p ,"Key", 3) == 0)
		{
			// sysprintf("LINE %d = %s\n", line_num, g_line);
			p += 3;
			p = find_alphanumeric(p);
			str_to_hex(p, p_tv->Key, 1);
			// sysprintf("Key = %s\n", p_tv->Key);
			continue;
		}
		if (strncmp(p ,"IV", 2) == 0)
		{
			p += 2;
			p = find_alphanumeric(p);
			str_to_hex(p, p_tv->IV, 0);
			continue;
		}
		if (strncmp(p ,"PT", 2) == 0)
		{
			p += 2;
			p = find_alphanumeric(p);
			str_to_hex(p, p_tv->PT, 0);
			continue;
		}
		if (strncmp(p ,"AAD", 3) == 0)
		{
			p += 3;
			p = find_alphanumeric(p);
			str_to_hex(p, p_tv->AAD, 0);
			continue;
		}
		if (strncmp(p ,"CT", 2) == 0)
		{
			p += 2;
			p = find_alphanumeric(p);
			str_to_hex(p, p_tv->CT, 0);
			continue;
		}
		if (strncmp(p ,"Tag", 3) == 0)
		{
			p += 3;
			p = find_alphanumeric(p);
			str_to_hex(p, p_tv->Tag, 0);
			continue;
		}
		if (strncmp(p ,"FAIL", 4) == 0)
		{
			sysprintf("It's a FAIL test vector.\n");
			return -3;
		}
	}
	return -1;
}

void  dump_buff_hex(uint8_t *pucBuff, int nBytes)
{
	uint64_t  addr, end_addr;
	int       nIdx, i;

	addr = (uint64_t)pucBuff;
	end_addr = addr + nBytes - 1;

	if ((addr % 16) != 0)
	{
		sysprintf("0x%04x_%04x  ", (addr>>16)&0xffff, addr & 0xffff);
		for (i = 0; i < addr % 16; i++)
			sysprintf(".. ");

		for ( ; (addr % 16) != 0; addr++)
			sysprintf("%02x ", readb((uint8_t *)addr) & 0xff);
		sysprintf("\n");
	}

	for ( ; addr <= end_addr; )
	{
		sysprintf("0x%04x_%04x  ", (addr>>16)&0xffff, addr & 0xffff);
		for (i = 0; i < 16; i++, addr++)
		{
			if (addr > end_addr)
				break;
			sysprintf("%02x ", readb((uint8_t *)addr) & 0xff);
		}
		sysprintf("\n");
	}
	sysprintf("\n");
}

int  do_aes_gcm_test(int encrypt)
{
	int       len, info_len, xlen, dma_len, val32, result;
	int       keysz;
	uint8_t   *p;
	int       sid, ret;
	uint32_t  *param = nc_ptr(g_tsi_param_mem);

	p = (uint8_t *)g_src_buff;
	memcpy(p, p_tv->IV, p_tv->IVlen);
	p += p_tv->IVlen;
	if (p_tv->IVlen == 12)   /* IV length is 96 bits? */
	{
		memcpy(p, g_zeros, 4);
		p[3] = 0x01;
		p += 4;
	}
	else
	{
		/* padding 0s to be block aligned */
		if ((p_tv->IVlen % 16) != 0)
		{
			len = 16 - (p_tv->IVlen % 16);
			memcpy(p, g_zeros, len);
			p += len;
		}

		/* 64 bits 0 */
		memcpy(p, g_zeros, 8);
		p += 8;

		/* IV len 64 bits */
		memcpy(p, g_zeros, 4);
		p += 4;
		val32 = p_tv->IVlen * 8;
		p[0] = (val32 >> 24) & 0xff;
		p[1] = (val32 >> 18) & 0xff;
		p[2] = (val32 >> 8) & 0xff;
		p[3] = val32 & 0xff;
		p += 4;
	}

	if (p_tv->AADlen)
	{
		memcpy(p, p_tv->AAD, p_tv->AADlen);
		p += p_tv->AADlen;

		if ((p_tv->AADlen % 16) != 0)
		{
			len = 16 - (p_tv->AADlen % 16);
			memcpy(p, g_zeros, len);
			p += len;
		}
	}

	info_len = ptr_to_u32(p) - ptr_to_u32(g_src_buff);

	if (p_tv->PTlen)
	{
		if (encrypt)
			memcpy(p, p_tv->PT, p_tv->PTlen);
		else
			memcpy(p, p_tv->CT, p_tv->PTlen);
		p += p_tv->PTlen;

		if ((p_tv->PTlen % 16) != 0)
		{
			len = 16 - (p_tv->PTlen % 16);
			memcpy(p, g_zeros, len);
			p += len;
		}
	}

	if (p_tv->Keylen == 128)
		keysz = AES_KEY_SIZE_128;
	else if (p_tv->Keylen == 192)
		keysz = AES_KEY_SIZE_192;
	else if (p_tv->Keylen == 256)
		keysz = AES_KEY_SIZE_256;
	else
	{
		sysprintf("Invalid Keylen %d\n", p_tv->Keylen);
		while (1);
	}

	dma_len = ptr_to_u32(p) - ptr_to_u32(g_src_buff);

	sysprintf("dma_len = %d\n", dma_len);

	param[0] = p_tv->IVlen;
	param[1] = p_tv->AADlen;
	param[2] = p_tv->PTlen;
	param[3] = ptr_to_u32(g_src_buff);
	param[4] = ptr_to_u32(g_dst_buff);

	ret = TSI_Open_Session(C_CODE_AES, &sid);
	if (ret != 0)
		goto err_out;

	ret = TSI_AES_Set_Key(sid, keysz, ptr_to_u32(p_tv->Key));
	if (ret != 0)
		goto err_out;

	ret = TSI_AES_Set_Mode( sid,         /* sid        */
							0,           /* kinswap    */
							0,           /* koutswap   */
							1,           /* inswap     */
							1,           /* outswap    */
							0,           /* sm4en      */
							encrypt,     /* encrypt    */
							AES_MODE_GCM,/* mode       */
							keysz,       /* keysz      */
							0,           /* ks         */
							0            /* ks_num     */
							);
	if (ret != 0)
		goto err_out;

	for (len = 0; len < dma_len; len += xlen)
	{
		if (len == 0)
		{
			param[3] = ptr_to_u32(g_src_buff);
			param[4] = ptr_to_u32(g_dst_buff);

			xlen = info_len+16;
			if (xlen > dma_len)
				xlen = dma_len;
		}
		else
		{
			param[3] = ptr_to_u32(g_src_buff) + len;
			param[4] = ptr_to_u32(g_dst_buff) + len - info_len;

			xlen = dma_len - len;
			if (xlen > 16)
				xlen = 16;
		}

		ret = TSI_AES_GCM_Run(sid, (len + xlen >= dma_len) ? 1 : 0, xlen, ptr_to_u32(param));
		if (ret != 0)
			goto err_out;
	}

	TSI_Close_Session(C_CODE_AES, sid);

	if ((p_tv->PTlen % 16) == 0)
		p = g_dst_buff + (p_tv->PTlen);
	else
		p = g_dst_buff + (p_tv->PTlen) + (16 - (p_tv->PTlen % 16));

	result = 0;
	if (p_tv->PTlen > 0)
	{
		if (encrypt)
		{
			if (memcmp(g_dst_buff, p_tv->CT, p_tv->PTlen) != 0)
				result = -1;
		}
		else
		{
			if (memcmp(g_dst_buff, p_tv->PT, p_tv->PTlen) != 0)
				result = -1;
		}
	}
	if (memcmp(p, p_tv->Tag, p_tv->Taglen) != 0)
		result = -1;

	if (result != 0)
	{
		sysprintf("AES GCM test vector failed at =>\n");
		sysprintf("Count = %d\n", p_tv->Count);

		sysprintf("DMA source:\n");
		dump_buff_hex(g_src_buff, dma_len);

		sysprintf("Output:\n");
		dump_buff_hex(g_dst_buff, dma_len);

		if (p_tv->PTlen > 0)
		{
			sysprintf("Answer:\n");
			if (encrypt)
				dump_buff_hex(p_tv->CT, p_tv->PTlen);
			else
				dump_buff_hex(p_tv->PT, p_tv->PTlen);
		}

		sysprintf("Tag output:\n");
		dump_buff_hex(p, 16);
		sysprintf("Answer Tag:\n");
		dump_buff_hex(p_tv->Tag, 16);
		return -1;
	}
	sysprintf("[PASS] %d\n", ++pass_cnt);
	return 0;

err_out:
	sysprintf("ERROR!!! 0x%x\n", ret);
	TSI_Close_Session(C_CODE_AES, sid);
	TSI_Print_Error(ret);
	return ret;
}

int  AES_GCM_KAT()
{
	int  ret;

	g_src_buff = nc_ptr(g_src_buff_mem);
	g_dst_buff = nc_ptr(g_dst_buff_mem);
	fdbck      = nc_ptr(fdbck_mem);
	p_tv       = nc_ptr(&g_tv);

	select_test_file("Encrypt");
	memset(p_tv, 0, sizeof(struct gcm_tv_t));
	pass_cnt = 0;
	while (get_next_pattern() == 0)
	{
		if (do_aes_gcm_test(1) != 0)
			while (1);
	}

	select_test_file("Decrypt");
	memset(p_tv, 0, sizeof(struct gcm_tv_t));
	pass_cnt = 0;
	while (1)
	{
		ret = get_next_pattern();
		if (ret == -3)
			continue;
		if (ret != 0)
			break;

		if (do_aes_gcm_test(0) != 0)
			while (1);
	}

	sysprintf("\n\nAll test passed.\n");
	return 0;
}



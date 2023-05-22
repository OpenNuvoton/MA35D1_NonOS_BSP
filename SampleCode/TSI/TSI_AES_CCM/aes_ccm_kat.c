/**************************************************************************//**
 * @file     aes_ccm_kat.c
 * @brief    AES CCM mode known answer test
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"

#define MAX_IV_LEN     1024
#define MAX_PT_LEN     4096
#define MAX_CT_LEN     4096

static char      *file_base;
static uint32_t  file_idx, file_size;

static char  g_line[8192];

static uint8_t  g_src_buff_mem[16*1024] __attribute__((aligned(32)));
static uint8_t  g_dst_buff_mem[16*1024]__attribute__((aligned(32)));
static uint32_t fdbck_mem[24] __attribute__((aligned(32)));
static uint8_t  work_buff_mem[512] __attribute__((aligned(32)));
static uint8_t  tsi_param_mem[512] __attribute__((aligned(32)));

static uint8_t  *g_src_buff;
static uint8_t  *g_dst_buff;
static uint32_t *fdbck;

static uint8_t g_zeros[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

struct ccm_tv_t
{
	int      Count;
	int      Tlen;
	int      Nlen;
	int      Alen;
	int      Plen;
	int      CT_len;
	int      p_blk_cnt;
	int      keylen;
	int      result_pass;

	uint8_t  Key[64];
	uint8_t  Adata[1024];
	uint8_t  Nonce[1024];
	uint8_t  Payload[1024];
	uint8_t  CT[1024];
};

static struct ccm_tv_t   g_tv __attribute__((aligned(32)));
static struct ccm_tv_t   *p_tv;

extern uint32_t VectorDataBase_CCM, VectorDataLimit_CCM;

static int  select_test_file(char *filename)
{
	file_base = (char *)&VectorDataBase_CCM;
	file_size = ptr_to_u32(&VectorDataLimit_CCM) - ptr_to_u32(&VectorDataBase_CCM);
	file_idx = 0;
	return 1;
}

static char * peer_file(void)
{
	return (file_base+file_idx);
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

static char * str_to_decimal(uint8_t *str, int *val)
{
	int         val32;
	uint8_t     val8;

	val32 = 0;
	while (*str)
	{
		if ((*str < '0') || (*str > '9'))
		{
			break;
		}
		val32 = (val32 * 10) + (*str - '0');
		str++;
	}
	*val = val32;
	return str;
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

static int  get_next_pattern(void)
{
	char    *p;
	int     len, count_found = 0;

	p_tv->result_pass = 1;

	while (1)
	{
		if (get_line() != 0)
			break;

		if (count_found && (g_line[0] == 0))
			return 0;

		if (g_line[0] == '#')
			continue;

		p = find_alphanumeric(g_line);
		if (p == NULL)
			continue;           /* alphanumeric character not found, go to next line */

		if (strncmp(p, "Count", 5) == 0)
		{
			p += 5;
			p = find_alphanumeric(p);
			p = str_to_decimal(p, &p_tv->Count);
			count_found = 1;
			continue;
		}
		if (strncmp(p ,"Key", 3) == 0)
		{
			p += 3;
			p = find_alphanumeric(p);
			len = str_to_hex(p, p_tv->Key, 0);
			do_swap((uint8_t *)&p_tv->Key[0], len);
			p_tv->keylen = len * 8;
			continue;
		}
		if (strncmp(p ,"Adata", 5) == 0)
		{
			p += 5;
			p = find_alphanumeric(p);
			str_to_hex(p, p_tv->Adata, 0);
			continue;
		}
		if (strncmp(p ,"Nonce", 5) == 0)
		{
			p += 5;
			p = find_alphanumeric(p);
			str_to_hex(p, p_tv->Nonce, 0);
			continue;
		}
		if (strncmp(p ,"Payload", 7) == 0)
		{
			p += 7;
			p = find_alphanumeric(p);
			str_to_hex(p, p_tv->Payload, 0);
			continue;
		}
		if (strncmp(p ,"CT", 2) == 0)
		{
			sysprintf("%s\n", p);
			p += 2;
			p = find_alphanumeric(p);
			p_tv->CT_len = str_to_hex(p, p_tv->CT, 0);
			sysprintf("CT_len = %d\n", p_tv->CT_len);
			continue;
		}
		if (strncmp(p ,"Result", 6) == 0)
		{
			p += 7;
			p = find_alphanumeric(p);
			if (strncmp(p ,"Pass", 4) == 0)
				p_tv->result_pass = 1;
			else
				p_tv->result_pass = 0;
			p += 4;
			continue;
		}

		while ((p != NULL) && (*p != 0))
		{
			if (strncmp(p, "Tlen", 4) == 0)
			{
				p += 4;
				p = find_alphanumeric(p);
				p = str_to_decimal(p, &p_tv->Tlen);
				sysprintf("Tlen = %d\n", p_tv->Tlen);
				p = find_alphanumeric(p);
				continue;
			}
			if (strncmp(p, "Alen", 4) == 0)
			{
				p += 4;
				p = find_alphanumeric(p);
				p = str_to_decimal(p, &p_tv->Alen);
				sysprintf("Alen = %d\n", p_tv->Alen);
				p = find_alphanumeric(p);
				continue;
			}
			if (strncmp(p, "Nlen", 4) == 0)
			{
				p += 4;
				p = find_alphanumeric(p);
				p = str_to_decimal(p, &p_tv->Nlen);
				sysprintf("Nlen = %d\n", p_tv->Nlen);
				p = find_alphanumeric(p);
				continue;
			}
			if (strncmp(p, "Plen", 4) == 0)
			{
				p += 4;
				p = find_alphanumeric(p);
				p = str_to_decimal(p, &p_tv->Plen);
				sysprintf("Plen = %d\n", p_tv->Plen);
				p = find_alphanumeric(p);
				continue;
			}
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

int  do_aes_ccm_test(int encrypt)
{
	int       q, i, len_left, xlen;
	int       keysz;
	int       use_len;
	uint8_t   *b, *src;
	uint32_t  info_len, dma_len;
	uint8_t   *c;   // c[16]
	int       sid, ret;
	uint32_t  *param = nc_ptr(tsi_param_mem);

	c = nc_ptr(work_buff_mem);

	if ((p_tv->keylen != 128) && (p_tv->keylen!= 192) && (p_tv->keylen != 256))
	{
		sysprintf("Invalid Key length %d\n", p_tv->keylen);
		while (1);
	}

	b = (uint8_t *)g_src_buff;

	if ( (p_tv->Tlen == 2) || (p_tv->Tlen > 16) || ((p_tv->Tlen % 2) != 0) )
	{
		sysprintf("Invalid Tlen %d!!\n", p_tv->Tlen);
		while (1);
	}

	/* Also implies q is within bounds */
	if ( (p_tv->Nlen < 7) || (p_tv->Nlen > 13) )        /* nounce */
	{
		sysprintf("Invalid Nlen %d!!\n", p_tv->Nlen);
		while (1);
	}

	if( p_tv->Alen > 0xFF00 )      /* additional data length  */
	{
		sysprintf("Invalid Alen %d!!\n", p_tv->Alen);
		while (1);
	}

	q = 15 - p_tv->Nlen;    /* n + q = 15; n is length of nounce, i.e. Nlen */

	/*-----------------------------------------------------------------------*/
	/*  Block B0                                                             */
	/*       Formatting of the Control Information and the Nonce             */
	/*-----------------------------------------------------------------------*/
	/* First block B_0:
	 * 0        .. 0        flags
	 * 1        .. iv_len   nonce (aka iv)
	 * iv_len+1 .. 15       length
	 *
	 * With flags as (bits):
	 * 7        0
	 * 6        add present?
	 * 5 .. 3   (t - 2) / 2
	 * 2 .. 0   q - 1
	 */
	b[0] = 0;
	b[0] |= ( p_tv->Alen > 0 ) << 6;
	b[0] |= ( ( p_tv->Tlen - 2 ) / 2 ) << 3;
	b[0] |= q - 1;

	memcpy( b + 1, p_tv->Nonce, p_tv->Nlen );

	for( i = 0, len_left = p_tv->Plen; i < q; i++, len_left >>= 8 )
		b[15-i] = (unsigned char)( len_left & 0xFF );

	if( len_left > 0 )
	{
		sysprintf("B0 error! len_left > 0!!\n");
		while (1);
	}
	b += 16;

	/*-----------------------------------------------------------------------*/
	/*    Formatting of the Associated Data                                  */
	/*-----------------------------------------------------------------------*/
	/*
	 * If there is additional data, update CBC-MAC with
	 * add_len, add, 0 (padding to a block boundary)
	 */
	if (p_tv->Alen > 0)
	{
		len_left = p_tv->Alen;
		src = p_tv->Adata;

		memset( b, 0, 16 );
		b[0] = (unsigned char)( ( p_tv->Alen >> 8 ) & 0xFF );
		b[1] = (unsigned char)( ( p_tv->Alen      ) & 0xFF );

		use_len = (len_left < (16 - 2)) ? len_left : 16 - 2;
		memcpy( b + 2, src, use_len );
		len_left -= use_len;
		src += use_len;
		b += 16;

		while( len_left > 0 )
		{
			use_len = len_left > 16 ? 16 : len_left;

			memset( b, 0, 16 );
			memcpy( b, src, use_len );
			b += 16;

			len_left -= use_len;
			src += use_len;
		}
	}

	info_len = ptr_to_u32(b) - ptr_to_u32(g_src_buff);

	/*-----------------------------------------------------------------------*/
	/*    Formatting of the payload                                          */
	/*-----------------------------------------------------------------------*/
	p_tv->p_blk_cnt = 0;
	if (p_tv->Plen > 0)
	{
		len_left = p_tv->Plen;
		src = p_tv->Payload;

		while( len_left > 0 )       /* fill payload by 16 bytes block */
		{
			use_len = len_left > 16 ? 16 : len_left;

			memset( b, 0, 16 );     /* naturally padding zero if len_left < 16 */
			memcpy( b, src, use_len );
			b += 16;

			len_left -= use_len;
			src += use_len;
			p_tv->p_blk_cnt++;
		}
	}

	if (p_tv->keylen == 128)
		keysz = AES_KEY_SIZE_128;
	else if (p_tv->keylen == 192)
		keysz = AES_KEY_SIZE_192;
	else if (p_tv->keylen == 256)
		keysz = AES_KEY_SIZE_256;
	else
	{
		sysprintf("Invalid Keylen %d\n", p_tv->keylen);
		while (1);
	}

	dma_len = ptr_to_u32(b) - ptr_to_u32(g_src_buff);

	/*-----------------------------------------------------------------------*/
	/*  Block Ctr0                                                           */
	/*       Formatting of the first Counter Block                           */
	/*-----------------------------------------------------------------------*/
	memset(c, 0, 16);
	q = 15 - p_tv->Nlen;    /* n + q = 15; n is length of nounce, i.e. Nlen */
	c[0] = (q - 1) & 0x7;
	memcpy( &c[1], p_tv->Nonce, p_tv->Nlen );
	do_swap((uint8_t *)&c[0], 16);

	param[0] = 0;
	param[1] = dma_len - p_tv->p_blk_cnt * 16;
	param[2] = p_tv->p_blk_cnt * 16;

	ret = TSI_Open_Session(C_CODE_AES, &sid);
	if (ret != 0)
		goto err_out;

	ret = TSI_AES_Set_Key(sid, keysz, ptr_to_u32(p_tv->Key));
	if (ret != 0)
		goto err_out;

	ret = TSI_AES_Set_IV(sid, ptr_to_u32(c));
	if (ret != 0)
		goto err_out;

	ret = TSI_AES_Set_Mode( sid,         /* sid        */
							0,           /* kinswap    */
							0,           /* koutswap   */
							1,           /* inswap     */
							1,           /* outswap    */
							0,           /* sm4en      */
							encrypt,     /* encrypt    */
							AES_MODE_CCM,/* mode       */
							keysz,       /* keysz      */
							0,           /* ks         */
							0            /* ks_num     */
							);
	if (ret != 0)
		goto err_out;

	for (i = 0; i < dma_len; i += xlen)
	{
		if (i == 0)
		{
			param[3] = ptr_to_u32(g_src_buff);
			param[4] = ptr_to_u32(g_dst_buff);

			xlen = info_len+16;
			if (xlen > dma_len)
				xlen = dma_len;
		}
		else
		{
			param[3] = ptr_to_u32(g_src_buff) + i;
			param[4] = ptr_to_u32(g_dst_buff) + i - info_len;

			xlen = dma_len - i;
			if (xlen > 16)
				xlen = 16;
		}

		ret = TSI_AES_GCM_Run(sid, (i + xlen >= dma_len) ? 1 : 0, xlen, ptr_to_u32(param));
		if (ret != 0)
			goto err_out;
	}

	TSI_Close_Session(C_CODE_AES, sid);

	/*
	 *  Check Cipher Text
	 */
	if ((p_tv->Plen > 0) && (memcmp(g_dst_buff, p_tv->CT, p_tv->Plen) != 0))
	{
		if (p_tv->result_pass == 0)
		{
			sysprintf("Result should fail. [PASS]\n");
			return 0;
		}

		sysprintf("AES CCM test vector failed at ciphertext compare =>\n");
		sysprintf("Count = %d\n", p_tv->Count);

		sysprintf("DMA source:\n");
		dump_buff_hex(g_src_buff, dma_len);

		sysprintf("Output:\n");
		dump_buff_hex(g_dst_buff, dma_len);

		return -1;
	}

	/*
	 *  Check Tag
	 */
	b = g_dst_buff + p_tv->Plen;
	if ((p_tv->Plen % 16) != 0)
		b += 16 - (p_tv->Plen % 16);

	if (memcmp(b, &p_tv->CT[p_tv->Plen], p_tv->CT_len - p_tv->Plen) != 0)
	{
		if (p_tv->result_pass == 0)
		{
			sysprintf("Result should fail. [PASS]\n");
			return 0;
		}

		sysprintf("AES CCM test vector failed at tag compare =>\n");
		sysprintf("Count = %d\n", p_tv->Count);

		sysprintf("DMA source:\n");
		dump_buff_hex(g_src_buff, dma_len);

		sysprintf("Output:\n");
		dump_buff_hex(g_dst_buff, dma_len);

		return -1;
	}

	if (p_tv->result_pass == 0)
	{
		sysprintf("Result should fail, but passed!\n");
		while (1);
	}
	sysprintf("[PASS]\n");
	return 0;

err_out:
	sysprintf("ERROR!!! 0x%x\n", ret);
	TSI_Close_Session(C_CODE_AES, sid);
	TSI_Print_Error(ret);
	return ret;
}

int  AES_CCM_KAT()
{
	g_src_buff = nc_ptr(g_src_buff_mem);
	g_dst_buff = nc_ptr(g_dst_buff_mem);
	fdbck      = nc_ptr(fdbck_mem);
	p_tv       = nc_ptr(&g_tv);

	select_test_file(NULL);
	while (get_next_pattern() == 0)
	{
		sysprintf("\nProgress %d%c...\n", (file_idx*100)/file_size, '%');
		if (do_aes_ccm_test(1) != 0)
			return -1;
	}
	sysprintf("\nAll test vectors passed.\n");
	return 0;
}

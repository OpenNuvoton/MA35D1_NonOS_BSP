/**************************************************************************//**
 * @file     main.c
 * @brief    This sample program demonstrates how to use the TSI commands to
 *           perform AES encrypt and decrypt with keys from Key Store.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"

#define TEXT_MAX_LEN        256

typedef struct kat_t
{
	uint32_t    ctrl;
	int         keylen;
	char        key[64+2];
	char        iv[32+2];
	char        plainT[TEXT_MAX_LEN];
	char        cipherT[TEXT_MAX_LEN];
} KAT_T;

#define AES_128_ECB     ((AES_MODE_ECB << AES_OPMODE_Pos) | (AES_KEY_SIZE_128 << AES_KEY_SIZE_Pos))
#define AES_128_CBC     ((AES_MODE_CBC << AES_OPMODE_Pos) | (AES_KEY_SIZE_128 << AES_KEY_SIZE_Pos))
#define AES_128_CFB     ((AES_MODE_CFB << AES_OPMODE_Pos) | (AES_KEY_SIZE_128 << AES_KEY_SIZE_Pos))
#define AES_128_OFB     ((AES_MODE_OFB << AES_OPMODE_Pos) | (AES_KEY_SIZE_128 << AES_KEY_SIZE_Pos))
#define AES_192_ECB     ((AES_MODE_ECB << AES_OPMODE_Pos) | (AES_KEY_SIZE_192 << AES_KEY_SIZE_Pos))
#define AES_192_CBC     ((AES_MODE_CBC << AES_OPMODE_Pos) | (AES_KEY_SIZE_192 << AES_KEY_SIZE_Pos))
#define AES_192_CFB     ((AES_MODE_CFB << AES_OPMODE_Pos) | (AES_KEY_SIZE_192 << AES_KEY_SIZE_Pos))
#define AES_192_OFB     ((AES_MODE_OFB << AES_OPMODE_Pos) | (AES_KEY_SIZE_192 << AES_KEY_SIZE_Pos))
#define AES_256_ECB     ((AES_MODE_ECB << AES_OPMODE_Pos) | (AES_KEY_SIZE_256 << AES_KEY_SIZE_Pos))
#define AES_256_CBC     ((AES_MODE_CBC << AES_OPMODE_Pos) | (AES_KEY_SIZE_256 << AES_KEY_SIZE_Pos))
#define AES_256_CFB     ((AES_MODE_CFB << AES_OPMODE_Pos) | (AES_KEY_SIZE_256 << AES_KEY_SIZE_Pos))
#define AES_256_OFB     ((AES_MODE_OFB << AES_OPMODE_Pos) | (AES_KEY_SIZE_256 << AES_KEY_SIZE_Pos))

#define SM4_ECB         (AES_SM4EN | (AES_MODE_ECB << AES_OPMODE_Pos) | (AES_KEY_SIZE_128 << AES_KEY_SIZE_Pos))


KAT_T  g_test_vector_mem[] = {
	{
		AES_128_CBC, 16,
		"10a58869d74be5a374cf867cfb473859",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"6d251e6944b051e04eaa6fb4dbf78465"
	},
	{
		AES_128_CBC, 16,
		"80000000000000000000000000000000",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"0edd33d3c621e546455bd8ba1418bec8",
	},
	{
		AES_128_CBC, 16,
		"ff800000000000000000000000000000",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"42ffb34c743de4d88ca38011c990890b"
	},
	{
		AES_128_CBC, 16,
		"ffff8000000000000000000000000000",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"c6a0b3e998d05068a5399778405200b4"
	},
	{
		AES_128_CBC, 16,
		"ffffffff800000000000000000000000",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"ed62e16363638360fdd6ad62112794f0"
	},
	{
		AES_128_CBC, 16,
		"fffffffffffffffffffffffffffffffe",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"9ba4a9143f4e5d4048521c4f8877d88e"
	},
	{
		AES_128_CBC, 16,
		"ffffffffffffffffffffffffffffffff",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"a1f6258c877d5fcd8964484538bfc92c"
	},
	{
		AES_128_CBC, 16,
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"80000000000000000000000000000000",
		"3ad78e726c1ec02b7ebfe92b23d9ec34"
	},
	{
		AES_128_CBC, 16,
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"ffc00000000000000000000000000000",
		"77e2b508db7fd89234caf7939ee5621a"
	},
	{
		AES_128_CBC, 16,
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"ffffffffffffffffffffffffffffffe0",
		"8568261797de176bf0b43becc6285afb"
	},
	{
		AES_128_CBC, 16,
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"fffffffffffffffffffffffffffffff0",
		"f9b0fda0c4a898f5b9e6f661c4ce4d07"
	},
	{
		AES_128_CFB, 16,
		"00000000000000000000000000000000",
		"cb9fceec81286ca3e989bd979b0cb284",
		"00",
		"92"
	},
	{
		AES_128_CFB, 16,
		"10a58869d74be5a374cf867cfb473859",
		"00000000000000000000000000000000",
		"00",
		"6d"
	},
	{
		AES_128_ECB, 16,
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"f34481ec3cc627bacd5dc3fb08f273e6",
		"0336763e966d92595a567cc9ce537f5e"
	},
	{
		AES_128_ECB, 16,
		"10a58869d74be5a374cf867cfb473859",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"6d251e6944b051e04eaa6fb4dbf78465"
	},
	{
		AES_128_OFB, 16,
		"00000000000000000000000000000000",
		"f34481ec3cc627bacd5dc3fb08f273e6",
		"00000000000000000000000000000000",
		"0336763e966d92595a567cc9ce537f5e"
	},
	{
		AES_192_CBC, 24,
		"000000000000000000000000000000000000000000000000",
		"00000000000000000000000000000000",
		"1b077a6af4b7f98229de786d7516b639",
		"275cfc0413d8ccb70513c3859b1d0f72"
	},
	{
		AES_192_CBC, 24,
		"e9f065d7c13573587f7875357dfbb16c53489f6a4bd0f7cd",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"0956259c9cd5cfd0181cca53380cde06"
	},
	{
		AES_192_CBC, 24,
		"800000000000000000000000000000000000000000000000",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"de885dc87f5a92594082d02cc1e1b42c"
	},
	{
		AES_192_CBC, 24,
		"ff8000000000000000000000000000000000000000000000",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"eba83ff200cff9318a92f8691a06b09f"
	},
	{
		AES_192_CBC, 24,
		"fffffffff800000000000000000000000000000000000000",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"cc4ba8a8e029f8b26d8afff9df133bb6"
	},
	{
		AES_192_CBC, 24,
		"fffffffffffffffffffffffffffffffffffffffffffffffe",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"018596e15e78e2c064159defce5f3085"
	},
	{
		AES_192_CBC, 24,
		"ffffffffffffffffffffffffffffffffffffffffffffffff",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"dd8a493514231cbf56eccee4c40889fb"
	},
	{
		AES_192_CBC, 24,
		"000000000000000000000000000000000000000000000000",
		"00000000000000000000000000000000",
		"80000000000000000000000000000000",
		"6cd02513e8d4dc986b4afe087a60bd0c"
	},
	{
		AES_192_CBC, 24,
		"000000000000000000000000000000000000000000000000",
		"00000000000000000000000000000000",
		"fffffffffffffffffffffffffffffffe",
		"cef41d16d266bdfe46938ad7884cc0cf"
	},
	{
		AES_192_CBC, 24,
		"000000000000000000000000000000000000000000000000",
		"00000000000000000000000000000000",
		"ffffffffffffffffffffffffffffffff",
		"b13db4da1f718bc6904797c82bcf2d32"
	},
	{
		AES_192_CFB, 24,
		"000000000000000000000000000000000000000000000000",
		"9c2d8842e5f48f57648205d39a239af1",
		"00",
		"c9"
	},
	{
		AES_192_CFB, 24,
		"e9f065d7c13573587f7875357dfbb16c53489f6a4bd0f7cd",
		"00000000000000000000000000000000",
		"00",
		"09"
	},
	{
		AES_192_ECB, 24,
		"000000000000000000000000000000000000000000000000",
		"00000000000000000000000000000000",
		"1b077a6af4b7f98229de786d7516b639",
		"275cfc0413d8ccb70513c3859b1d0f72"
	},
	{
		AES_192_ECB, 24,
		"e9f065d7c13573587f7875357dfbb16c53489f6a4bd0f7cd",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"0956259c9cd5cfd0181cca53380cde06"
	},
	{
		AES_192_OFB, 24,
		"000000000000000000000000000000000000000000000000",
		"1b077a6af4b7f98229de786d7516b639",
		"00000000000000000000000000000000",
		"275cfc0413d8ccb70513c3859b1d0f72"
	},
	{
		AES_256_CBC, 32,
		"0000000000000000000000000000000000000000000000000000000000000000",
		"00000000000000000000000000000000",
		"761c1fe41a18acf20d241650611d90f1",
		"623a52fcea5d443e48d9181ab32c7421"
	},
	{
		AES_256_CBC, 32,
		"c47b0294dbbbee0fec4757f22ffeee3587ca4730c3d33b691df38bab076bc558",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"46f2fb342d6f0ab477476fc501242c5f"
	},
	{
		AES_256_CBC, 32,
		"8000000000000000000000000000000000000000000000000000000000000000",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"e35a6dcb19b201a01ebcfa8aa22b5759"
	},
	{
		AES_256_CBC, 32,
		"ffff800000000000000000000000000000000000000000000000000000000000",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"610b71dfc688e150d8152c5b35ebc14d"
	},
	{
		AES_256_CBC, 32,
		"ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe0",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"cf78618f74f6f3696e0a4779b90b5a77"
	},
	{
		AES_256_CBC, 32,
		"fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"b07d4f3e2cd2ef2eb545980754dfea0f"
	},
	{
		AES_256_CBC, 32,
		"ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"4bf85f1b5d54adbc307b0a048389adcb"
	},
	{
		AES_256_CBC, 32,
		"0000000000000000000000000000000000000000000000000000000000000000",
		"00000000000000000000000000000000",
		"80000000000000000000000000000000",
		"ddc6bf790c15760d8d9aeb6f9a75fd4e"
	},
	{
		AES_256_CBC, 32,
		"0000000000000000000000000000000000000000000000000000000000000000",
		"00000000000000000000000000000000",
		"fffffffffffffffffffffffffffffffe",
		"7bfe9d876c6d63c1d035da8fe21c409d"
	},
	{
		AES_256_CBC, 32,
		"0000000000000000000000000000000000000000000000000000000000000000",
		"00000000000000000000000000000000",
		"ffffffffffffffffffffffffffffffff",
		"acdace8078a32b1a182bfa4987ca1347"
	},
	{
		AES_256_CFB, 32,
		"0000000000000000000000000000000000000000000000000000000000000000",
		"014730f80ac625fe84f026c60bfd547d",
		"00",
		"5c"
	},
	{
		AES_256_CFB, 32,
		"c47b0294dbbbee0fec4757f22ffeee3587ca4730c3d33b691df38bab076bc558",
		"00000000000000000000000000000000",
		"00",
		"46"
	},
	{
		AES_256_ECB, 32,
		"0000000000000000000000000000000000000000000000000000000000000000",
		"00000000000000000000000000000000",
		"014730f80ac625fe84f026c60bfd547d",
		"5c9d844ed46f9885085e5d6a4f94c7d7"
	},
	{
		AES_256_ECB, 32,
		"c47b0294dbbbee0fec4757f22ffeee3587ca4730c3d33b691df38bab076bc558",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"46f2fb342d6f0ab477476fc501242c5f"
	},
	{
		AES_256_CFB, 32,
		"0000000000000000000000000000000000000000000000000000000000000000",
		"014730f80ac625fe84f026c60bfd547d",
		"00000000000000000000000000000000",
		"5c9d844ed46f9885085e5d6a4f94c7d7"
	},
	{   /* A.2.1.1 SM4-ECB Example 1 */
		SM4_ECB, 16,
		"0123456789ABCDEFFEDCBA9876543210",
		"00000000000000000000000000000000",
		"aaaaaaaabbbbbbbbccccccccddddddddeeeeeeeeffffffffaaaaaaaabbbbbbbb",
		"5ec8143de509cff7b5179f8f474b86192f1d305a7fb17df985f81c8482192304"
	},
	{   /* A.2.1.1 SM4-ECB Example 1 */
		SM4_ECB, 16,
		"FEDCBA98765432100123456789ABCDEF",
		"00000000000000000000000000000000",
		"aaaaaaaabbbbbbbbccccccccddddddddeeeeeeeeffffffffaaaaaaaabbbbbbbb",
		"C5876897E4A59BBBA72A10C83872245B12DD90BC2D200692B529A4155AC9E600"
	}
};

struct aes_context
{
	int       aes_keysz;
	int       is_sm4;
	int       aes_mode;
	uint32_t  aes_key[8];
	uint32_t  aes_iv[4];
	uint8_t   plain_text[TEXT_MAX_LEN];
	uint8_t   cipher_text[TEXT_MAX_LEN];
	uint8_t   output[TEXT_MAX_LEN];
	uint8_t   zero_16[16];
	KAT_T     *vector;
};

struct aes_context _aes_context_mem __attribute__((aligned(32)));

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


static int  str2hex(char *str, uint8_t *hex)
{
	int  val8, count = 0;

	while (*str)
	{
		val8 = char_to_hex(*str);
		str++;
		val8 = (val8 << 4) | char_to_hex(*str);
		str++;

		hex[count] = val8;
		count++;
	}
	return count;
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

static int  read_test_vector(struct aes_context *ctx, KAT_T *t)
{
	int       i, count;
	uint32_t  *key_ptr;
	uint8_t   hex_data[256];

	/*
	 *  read AES key
	 */
	if (str2hex(t->key, hex_data) != t->keylen)
		return -1;

	if (t->keylen == 16)
		ctx->aes_keysz = AES_KEY_SIZE_128;
	else if (t->keylen == 24)
		ctx->aes_keysz = AES_KEY_SIZE_192;
	else   /* t->keylen == 32 */
		ctx->aes_keysz = AES_KEY_SIZE_256;

	if (t->ctrl & AES_SM4EN)
	{
		ctx->aes_mode = AES_MODE_ECB;
		ctx->is_sm4 = 1;
	}
	else
	{
		ctx->aes_mode = (t->ctrl & AES_OPMODE_Msk) >> AES_OPMODE_Pos;
		ctx->is_sm4 = 0;
	}

	key_ptr = ctx->aes_key;
	for (i = 0; i < t->keylen; i+=4, key_ptr++)
	{
		*key_ptr = (hex_data[i]<<24) | (hex_data[i+1]<<16) | (hex_data[i+2]<<8) | hex_data[i+3];
		//sysprintf("Key %d = 0x%x\n", i/4, *key_ptr);
	}

	/*
	 *  read AES initial vector
	 */
	if (str2hex(t->iv, hex_data) != 16)
		return -1;

	key_ptr = ctx->aes_iv;
	for (i = 0; i < 16; i+=4, key_ptr++)
	{
		*key_ptr = (hex_data[i]<<24) | (hex_data[i+1]<<16) | (hex_data[i+2]<<8) | hex_data[i+3];
		//sysprintf("IV %d = 0x%x\n", i/4, *key_ptr);
	}

	/*
	 *  read plain text
	 */
	count = str2hex(t->plainT, ctx->plain_text);

	/*
	 *  read cipher text
	 */
	str2hex(t->cipherT, ctx->cipher_text);

	return count;
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
	int loop, i, len, sid, key_num, ret;
	struct aes_context *ctx;
	KAT_T *tv;

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
	sysprintf("|  MA35D1 TSI AES encrypt and decrypt with Key Store keys.      |\n");
	sysprintf("+---------------------------------------------------------------+\n");

	tv = nc_ptr(g_test_vector_mem);
	ctx = nc_ptr(&_aes_context_mem);

	TSI_KS_EraseAll();

	key_num = 0;

	for (i = 0; i < sizeof(g_test_vector_mem) / sizeof(KAT_T); i++)
	{
		int       remain_size;
		uint32_t  ks_sts, ks_otpsts, ks_metadata;

		sysprintf("AES test vecotr %d...\n", i);

		len = read_test_vector(ctx, &tv[i]);
		if (len < 0)
		{
			sysprintf("Failed to read test vector!\n");
			return -1;
		}

		sysprintf("Write AES key to KS_SRAM...\n");
		ret = TSI_KS_Write_SRAM(KS_META_AES | KS_META_256, ctx->aes_key, &key_num);
		if (ret != 0)
			goto tsi_err;

		sysprintf("Using KS_SRAM key %d...\n", key_num);

		ret = TSI_KS_GetRemainSize(&remain_size);
		if (ret != 0)
			goto tsi_err;

		ret = TSI_KS_GetStatus(&ks_sts, &ks_otpsts, &ks_metadata);
		if (ret != 0)
			goto tsi_err;

		sysprintf("KS_SRAM remain size: %d\n", remain_size);
		sysprintf("ks_sts=0x%x, ks_otpsts=0x%x, ks_metadata=0x%x\n", ks_sts, ks_otpsts, ks_metadata);

		ret = TSI_Open_Session(C_CODE_AES, &sid);
		if (ret != 0)
			goto tsi_err;

		ret = TSI_AES_Set_IV(sid, ptr_to_u32(ctx->aes_iv));
		if (ret != 0)
			goto tsi_err;

		//ret = TSI_AES_Set_Key(sid, ctx->aes_keysz, ptr_to_u32(ctx->aes_key));
		//if (ret != 0)
		//  goto tsi_err;
		ret = TSI_AES_Set_Mode(sid,                  /* sid        */
							   0,                    /* kinswap    */
							   0,                    /* koutswap   */
							   1,                    /* inswap     */
							   1,                    /* outswap    */
							   ctx->is_sm4,          /* sm4en      */
							   1,                    /* encrypt    */
							   ctx->aes_mode,        /* mode       */
							   ctx->aes_keysz,       /* keysz      */
							   SEL_KEY_FROM_KS_SRAM, /* ks         */
							   key_num               /* ks_num     */
							   );
		if (ret != 0)
			goto tsi_err;

		ret = TSI_AES_Run(sid, 1, len, ptr_to_u32(ctx->plain_text), ptr_to_u32(ctx->output));
		if (ret != 0)
			goto tsi_err;

		TSI_Close_Session(C_CODE_AES, sid);

		if (memcmp(ctx->cipher_text, ctx->output, len) != 0)
		{
			sysprintf("AES test vector encode failed at =>\n");
			sysprintf("KEY        = %s\n", tv[i].key);
			sysprintf("IV         = %s\n", tv[i].iv);
			sysprintf("PLAINTEXT  = %s\n", tv[i].plainT);
			sysprintf("CIPHERTEXT = %s\n", tv[i].cipherT);

			sysprintf("Encode output:\n");
			dump_buff_hex(ctx->output, len);
			return -1;
		}
		sysprintf("[PASS]\n");

		if (key_num >= 31)
		{
			ret = TSI_KS_EraseAll();
			if (ret != 0)
				goto tsi_err;
		}
	}
	sysprintf("\nAll test vectors passed.\n");
	while (1);

tsi_err:
	TSI_Close_Session(C_CODE_AES, sid);
	TSI_Print_Error(ret);
	while (1);
}

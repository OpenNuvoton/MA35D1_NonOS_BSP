/**************************************************************************//**
 * @file     aes_kat.c
 * @brief    AES known answer test
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"

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

#define TEXT_MAX_LEN    256

typedef struct kat_t
{
	uint32_t    ctrl;
	int         keylen;
	char        key[128];
	char        iv[128];
	char        plainT[TEXT_MAX_LEN];
	char        cipherT[TEXT_MAX_LEN];
} KAT_T;

static KAT_T  g_test_vector[] =
{
	{   /* A.2.1.1 SM4-ECB Example 1 */
		SM4_ECB, 16,
		"0123456789ABCDEFFEDCBA9876543210",
		"00000000000000000000000000000000",
		"aaaaaaaabbbbbbbbccccccccddddddddeeeeeeeeffffffffaaaaaaaabbbbbbbb",
		"5ec8143de509cff7b5179f8f474b86192f1d305a7fb17df985f81c8482192304"
	},
	{
		AES_128_CBC, 16,
		"10a58869d74be5a374cf867cfb473859",
		"00000000000000000000000000000000",
		"00000000000000000000000000000000",
		"6d251e6944b051e04eaa6fb4dbf78465"
	},
	{   /* A.2.1.1 SM4-ECB Example 1 */
		SM4_ECB, 16,
		"0123456789ABCDEFFEDCBA9876543210",
		"00000000000000000000000000000000",
		"aaaaaaaabbbbbbbbccccccccddddddddeeeeeeeeffffffffaaaaaaaabbbbbbbb",
		"5ec8143de509cff7b5179f8f474b86192f1d305a7fb17df985f81c8482192304"
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
	{   /* A.2.1.1 SM4-ECB Example 1 */
		SM4_ECB, 16,
		"0123456789ABCDEFFEDCBA9876543210",
		"00000000000000000000000000000000",
		"aaaaaaaabbbbbbbbccccccccddddddddeeeeeeeeffffffffaaaaaaaabbbbbbbb",
		"5ec8143de509cff7b5179f8f474b86192f1d305a7fb17df985f81c8482192304"
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
	{   /* A.2.1.1 SM4-ECB Example 1 */
		SM4_ECB, 16,
		"0123456789ABCDEFFEDCBA9876543210",
		"00000000000000000000000000000000",
		"aaaaaaaabbbbbbbbccccccccddddddddeeeeeeeeffffffffaaaaaaaabbbbbbbb",
		"5ec8143de509cff7b5179f8f474b86192f1d305a7fb17df985f81c8482192304"
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
	{   /* A.2.1.1 SM4-ECB Example 1 */
		SM4_ECB, 16,
		"0123456789ABCDEFFEDCBA9876543210",
		"00000000000000000000000000000000",
		"aaaaaaaabbbbbbbbccccccccddddddddeeeeeeeeffffffffaaaaaaaabbbbbbbb",
		"5ec8143de509cff7b5179f8f474b86192f1d305a7fb17df985f81c8482192304"
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
	{   /* A.2.1.1 SM4-ECB Example 1 */
		SM4_ECB, 16,
		"0123456789ABCDEFFEDCBA9876543210",
		"00000000000000000000000000000000",
		"aaaaaaaabbbbbbbbccccccccddddddddeeeeeeeeffffffffaaaaaaaabbbbbbbb",
		"5ec8143de509cff7b5179f8f474b86192f1d305a7fb17df985f81c8482192304"
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
	{   /* A.2.1.1 SM4-ECB Example 1 */
		SM4_ECB, 16,
		"0123456789ABCDEFFEDCBA9876543210",
		"00000000000000000000000000000000",
		"aaaaaaaabbbbbbbbccccccccddddddddeeeeeeeeffffffffaaaaaaaabbbbbbbb",
		"5ec8143de509cff7b5179f8f474b86192f1d305a7fb17df985f81c8482192304"
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
};

static  uint8_t    *pPlainText, *pCipherText, *pOutput;
static  int        aes_keysz;
static  uint32_t   *aes_key, *aes_iv;
static  int        sm4en;
static  int        aes_mode;

uint8_t buff_pool[0x10000] __attribute__((aligned(32)));

static uint8_t  g_hex[256];

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

int  str2hex(char *str, uint8_t *hex)
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

void  word_swap_hex(uint8_t *hex, int len)
{
	int      i;
	uint8_t  val8;

	len = (len + 3) & 0xfffffffc;

	for (i = 0; i < len; i+=4)
	{
		val8 = hex[i];
		hex[i] = hex[i+3];
		hex[i+3] = val8;

		val8 = hex[i+1];
		hex[i+1] = hex[i+2];
		hex[i+2] = val8;
	}
}

void  swap_registers(uint32_t * reg_base, int reg_cnt)
{
	int       i;
	uint32_t  *reg = reg_base;
	uint32_t  data32;

	for (i = 0; i < reg_cnt; i++, reg++)
	{
		data32 = ((*reg>>24)&0xff) | ((*reg>>8)&0xff00) | ((*reg<<8)&0xff0000) | ((*reg<<24)&0xff000000);
		*reg = data32;
	}
}

int  read_test_vector(KAT_T *t)
{
	int         i, count;
	uint32_t    *key_ptr;

	/*
	 *  read AES key
	 */
	if (str2hex(t->key, g_hex) != t->keylen)
		return -1;

	if (t->keylen == 16)
		aes_keysz = AES_KEY_SIZE_128;
	else if (t->keylen == 24)
		aes_keysz = AES_KEY_SIZE_192;
	else   /* t->keylen == 32 */
		aes_keysz = AES_KEY_SIZE_256;

	if (t->ctrl & AES_SM4EN)
	{
		aes_mode = AES_MODE_ECB;
		sm4en = 1;
	}
	else
	{
		aes_mode = (t->ctrl & AES_OPMODE_Msk) >> AES_OPMODE_Pos;
		sm4en = 0;
	}

	key_ptr = aes_key;
	for (i = 0; i < t->keylen; i+=4, key_ptr++)
	{
		*key_ptr = (g_hex[i]<<24) | (g_hex[i+1]<<16) | (g_hex[i+2]<<8) | g_hex[i+3];
		// sysprintf("Key %d = 0x%x\n", i/4, *key_ptr);
	}

	/*
	 *  read AES initial vector
	 */
	if (str2hex(t->iv, g_hex) != 16)
		return -1;

	key_ptr = aes_iv;
	for (i = 0; i < 16; i+=4, key_ptr++)
	{
		*key_ptr = (g_hex[i]<<24) | (g_hex[i+1]<<16) | (g_hex[i+2]<<8) | g_hex[i+3];
		// sysprintf("IV %d = 0x%x\n", i/4, *key_ptr);
	}

	/*
	 *  read plain text
	 */
	count = str2hex(t->plainT, pPlainText);

	/*
	 *  read cipher text
	 */
	str2hex(t->cipherT, pCipherText);

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

int  AES_KAT(void)
{
	int        i, sid;
	int        len, ret;
	uint32_t   *fdbck1, *fdbck2;
	uint8_t    *pOutput;

	aes_key     = nc_ptr(&buff_pool[0]);
	aes_iv      = nc_ptr(&buff_pool[0x100]);
	fdbck1       = nc_ptr(&buff_pool[0x200]);
	fdbck2       = nc_ptr(&buff_pool[0x300]);
	pPlainText  = nc_ptr(&buff_pool[0x400]);
	pCipherText = nc_ptr(&buff_pool[0x2400]);
	pOutput      = nc_ptr(&buff_pool[0x4400]);

	for (i = 0; i < sizeof(g_test_vector)/sizeof(KAT_T); i++)
	{
		sysprintf("AES KAT test vecotr %d...\n", i);

		len = read_test_vector((KAT_T *)&g_test_vector[i]);
		if (len < 0)
		{
			sysprintf("Failed to read test vector!\n");
			return -1;
		}

		/*-------------------------------------------------*/
		/*   Encode test                                   */
		/*-------------------------------------------------*/
		memset(pOutput, 0, len);

		ret = TSI_Open_Session(C_CODE_AES, &sid);
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_IV(sid, ptr_to_u32(aes_iv));
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_Key(sid, aes_keysz, ptr_to_u32(aes_key));
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_Mode(sid,         /* sid        */
							   0,           /* kinswap    */
							   0,           /* koutswap   */
							   1,           /* inswap     */
							   1,           /* outswap    */
							   sm4en,       /* sm4en      */
							   1,           /* encrypt    */
							   aes_mode,    /* mode       */
							   aes_keysz,   /* keysz      */
							   0,           /* ks         */
							   0            /* ks_num     */
							   );
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Run(sid, 1, len, ptr_to_u32(pPlainText), ptr_to_u32(pOutput));
		if (ret != 0)
			goto err_out;

		ret = TSI_Access_Feedback(sid, 0, g_test_vector[i].keylen/4, ptr_to_u32(fdbck1));
		if (ret != 0)
			goto err_out;

		TSI_Close_Session(C_CODE_AES, sid);

		if (memcmp(pCipherText, pOutput, len) != 0)
		{
			sysprintf("AES test vector encode test failed!!\n");
			sysprintf("Expect output:\n");
			dump_buff_hex(pCipherText, len);
			sysprintf("Encode output:\n");
			dump_buff_hex(pOutput, len);
			goto err_out;
		}

		/*-------------------------------------------------*/
		/*   Decode test                                   */
		/*-------------------------------------------------*/
		memset(pOutput, 0, len);

		ret = TSI_Open_Session(C_CODE_AES, &sid);
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_IV(sid, ptr_to_u32(aes_iv));
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_Key(sid, aes_keysz, ptr_to_u32(aes_key));
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_Mode(sid,         /* sid        */
							   0,           /* kinswap    */
							   0,           /* koutswap   */
							   1,           /* inswap     */
							   1,           /* outswap    */
							   sm4en,       /* sm4en      */
							   0,           /* encrypt    */
							   aes_mode,    /* mode       */
							   aes_keysz,   /* keysz      */
							   0,           /* ks         */
							   0            /* ks_num     */
							   );
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Run(sid, 1, len, ptr_to_u32(pCipherText), ptr_to_u32(pOutput));
		if (ret != 0)
			goto err_out;

		TSI_Close_Session(C_CODE_AES, sid);

		if (memcmp(pPlainText, pOutput, len) != 0)
		{
			sysprintf("AES test vector decode test failed!!\n");
			sysprintf("Decode output:\n");
			dump_buff_hex(pOutput, len);
			return -1;
		}

		/*-------------------------------------------------*/
		/*   Encode with output swap test                  */
		/*-------------------------------------------------*/
		memset(pOutput, 0, len);

		ret = TSI_Open_Session(C_CODE_AES, &sid);
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_IV(sid, ptr_to_u32(aes_iv));
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_Key(sid, aes_keysz, ptr_to_u32(aes_key));
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_Mode(sid,         /* sid        */
							   0,           /* kinswap    */
							   0,           /* koutswap   */
							   1,           /* inswap     */
							   0,           /* outswap    */
							   sm4en,       /* sm4en      */
							   1,           /* encrypt    */
							   aes_mode,    /* mode       */
							   aes_keysz,   /* keysz      */
							   0,           /* ks         */
							   0            /* ks_num     */
							   );
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Run(sid, 1, len, ptr_to_u32(pPlainText), ptr_to_u32(pOutput));
		if (ret != 0)
			goto err_out;

		TSI_Close_Session(C_CODE_AES, sid);

		word_swap_hex(pOutput, len);

		if (memcmp(pCipherText, pOutput, len) != 0)
		{
			sysprintf("AES test vector encode output swap failed!!\n");
			sysprintf("Encode output:\n");
			dump_buff_hex(pOutput, len);
			goto err_out;
		}

		/*-------------------------------------------------*/
		/*   Encode with input swap test                   */
		/*-------------------------------------------------*/
		memset(pOutput, 0, len);
		word_swap_hex(pPlainText, len);

		ret = TSI_Open_Session(C_CODE_AES, &sid);
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_IV(sid, ptr_to_u32(aes_iv));
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_Key(sid, aes_keysz, ptr_to_u32(aes_key));
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_Mode(sid,         /* sid        */
							   0,           /* kinswap    */
							   0,           /* koutswap   */
							   0,           /* inswap     */
							   1,           /* outswap    */
							   sm4en,       /* sm4en      */
							   1,           /* encrypt    */
							   aes_mode,    /* mode       */
							   aes_keysz,   /* keysz      */
							   0,           /* ks         */
							   0            /* ks_num     */
							   );
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Run(sid, 1, len, ptr_to_u32(pPlainText), ptr_to_u32(pOutput));
		if (ret != 0)
			goto err_out;

		TSI_Close_Session(C_CODE_AES, sid);

		if (memcmp(pCipherText, pOutput, len) != 0)
		{
			sysprintf("AES test vector encode input swap failed!!\n");
			sysprintf("Encode output:\n");
			dump_buff_hex(pOutput, len);
			goto err_out;
		}

		word_swap_hex(pPlainText, len);   /* swap back to original test vector data */

		/*-------------------------------------------------*/
		/*   AES Engine Input Key and Initial Vector Swap  */
		/*-------------------------------------------------*/
		memset(pOutput, 0, len);

		swap_registers(aes_key, 8);
		swap_registers(aes_iv, 4);

		ret = TSI_Open_Session(C_CODE_AES, &sid);
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_IV(sid, ptr_to_u32(aes_iv));
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_Key(sid, aes_keysz, ptr_to_u32(aes_key));
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_Mode(sid,         /* sid        */
							   1,           /* kinswap    */
							   0,           /* koutswap   */
							   1,           /* inswap     */
							   1,           /* outswap    */
							   sm4en,       /* sm4en      */
							   1,           /* encrypt    */
							   aes_mode,    /* mode       */
							   aes_keysz,   /* keysz      */
							   0,           /* ks         */
							   0            /* ks_num     */
							   );
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Run(sid, 1, len, ptr_to_u32(pPlainText), ptr_to_u32(pOutput));
		if (ret != 0)
			goto err_out;

		TSI_Close_Session(C_CODE_AES, sid);

		if (memcmp(pCipherText, pOutput, len) != 0)
		{
			sysprintf("AES Engine Input Key and Initial Vector Swap Test failed!\n");
			sysprintf("Encode output:\n");
			dump_buff_hex(pOutput, len);
			goto err_out;
		}

		swap_registers(aes_key, 8);    /* swap back to original test vector data */
		swap_registers(aes_iv, 4);     /* swap back to original test vector data */

		/*-------------------------------------------------*/
		/*   AES Engine Output Feedback Swap Test          */
		/*-------------------------------------------------*/
		memset(pOutput, 0, len);

		ret = TSI_Open_Session(C_CODE_AES, &sid);
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_IV(sid, ptr_to_u32(aes_iv));
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_Key(sid, aes_keysz, ptr_to_u32(aes_key));
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Set_Mode(sid,         /* sid        */
							   0,           /* kinswap    */
							   1,           /* koutswap   */
							   1,           /* inswap     */
							   1,           /* outswap    */
							   sm4en,       /* sm4en      */
							   1,           /* encrypt    */
							   aes_mode,    /* mode       */
							   aes_keysz,   /* keysz      */
							   0,           /* ks         */
							   0            /* ks_num     */
							   );
		if (ret != 0)
			goto err_out;

		ret = TSI_AES_Run(sid, 1, len, ptr_to_u32(pPlainText), ptr_to_u32(pOutput));
		if (ret != 0)
			goto err_out;

		ret = TSI_Access_Feedback(sid, 0, g_test_vector[i].keylen/4, ptr_to_u32(fdbck2));
		if (ret != 0)
			goto err_out;

		swap_registers(fdbck2, 4);

		TSI_Close_Session(C_CODE_AES, sid);

		if (memcmp(fdbck1, fdbck2, len) != 0)
		{
			sysprintf("AES Engine Output Feedback Swap Test failed!!\n");
			sysprintf("fdbck1:\n");
			dump_buff_hex((uint8_t *)fdbck1, len);
			sysprintf("fdbck2:\n");
			dump_buff_hex((uint8_t *)fdbck2, len);
			goto err_out;
		}
	}

	sysprintf("All test vector passed.\n");
	return 0;

err_out:
	sysprintf("TSI ERROR!!! 0x%x\n", ret);
	TSI_Close_Session(C_CODE_AES, sid);
	TSI_Print_Error(ret);
	return ret;
}

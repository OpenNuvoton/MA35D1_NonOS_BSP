/*
 * Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 * Copyright (C) 2023, Nuvoton Technology Corporation, All Rights Reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */

#include "common.h"

#include "mbedtls/platform_util.h"
#include "mbedtls/aes.h"
#include "mbedtls/error.h"

#if defined(MBEDTLS_AES_C)
#if defined(MBEDTLS_AES_ALT)

#include <string.h>
#include "NuMicro.h"
#include "tsi_cmd.h"


 /* Parameter validation macros based on platform_util.h */
#define AES_VALIDATE_RET( cond )    \
	MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_AES_BAD_INPUT_DATA )
#define AES_VALIDATE( cond )        \
	MBEDTLS_INTERNAL_VALIDATE( cond )


#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif

#define AES_BLOCK_SIZE  (16)



__ALIGNED(4) static uint8_t s_u8in[AES_BLOCK_SIZE];
__ALIGNED(4) static uint8_t s_u8out[AES_BLOCK_SIZE];

#if defined(MBEDTLS_CIPHER_MODE_CBC)
__STATIC_INLINE uint32_t nu_get32_le(const uint8_t* pos)
{
	uint32_t val;

	val = *pos++;
	val += (*pos++ << 8);
	val += (*pos++ << 16);
	val += (*pos++ << 24);

	return val;
}

__STATIC_INLINE void nu_set32_le(uint8_t* pos, uint32_t val)
{
	*pos++ = val & 0xFF;
	*pos++ = (val >> 8) & 0xFF;
	*pos++ = (val >> 16) & 0xFF;
	*pos = (val >> 24) & 0xFF;
}
#endif


/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize(void *v, size_t n)
{
	volatile unsigned char *p = (unsigned char*)v;
	while(n--) *p++ = 0;
}

/* AES DMA compatible backup buffer if user buffer doesn't meet requirements
 *
 * AES DMA buffer location requires to be:
 * (1) Word-aligned
 * (2) Located in 0x2xxxxxxx region. Check linker files to ensure global variables are placed in this region.
 *
 * AES DMA buffer size MAX_DMA_CHAIN_SIZE must be a multiple of 16-byte block size.
 * Its value is estimated to trade memory footprint off against performance.
 *
 */
#define MAX_DMA_CHAIN_SIZE (AES_BLOCK_SIZE*6)

void mbedtls_aes_init(mbedtls_aes_context *ctx)
{
	AES_VALIDATE(ctx != NULL);

	memset(ctx, 0, sizeof(mbedtls_aes_context));
}

void mbedtls_aes_free(mbedtls_aes_context *ctx)
{
	if(ctx == NULL)
		return;

	mbedtls_zeroize(ctx, sizeof(mbedtls_aes_context));
}

/*
 * AES key schedule (encryption)
 */
int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key,
						   unsigned int keybits)
{
	unsigned int i;
	uint8_t *pu8;

	AES_VALIDATE_RET(ctx != NULL);
	AES_VALIDATE_RET(key != NULL);

	switch(keybits)
	{
	case 128:
		ctx->keySize = 16;
		ctx->keySizeOp = AES_KEY_SIZE_128;
		break;
	case 192:
		ctx->keySize = 24;
		ctx->keySizeOp = AES_KEY_SIZE_192;
		break;
	case 256:
		ctx->keySize = 32;
		ctx->keySizeOp = AES_KEY_SIZE_256;
		break;
	default:
		return(MBEDTLS_ERR_AES_INVALID_KEY_LENGTH);
	}

	pu8 = (uint8_t *)&ctx->keys[0];

	for(i = 0; i < ctx->keySize; i++)
	{
		pu8[i] = key[i];
	}

	return(0);
}

/*
 * AES key schedule (decryption)
 */
int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key,
						   unsigned int keybits)
{
	int ret;

	AES_VALIDATE_RET(ctx != NULL);
	AES_VALIDATE_RET(key != NULL);

	/* Also checks keybits */
	if((ret = mbedtls_aes_setkey_enc(ctx, key, keybits)) != 0)
		goto exit;

exit:
	return(ret);
}

/* Do AES encrypt/decrypt with H/W accelerator
 *
 * NOTE: As input/output buffer doesn't follow constraint of DMA buffer, static allocated
 *       DMA compatible buffer is used for DMA instead and this needs extra copy.
 *
 * NOTE: dataSize requires to be:
 *       1) Multiple of block size 16
 *       2) <= MAX_DMA_CHAIN_SIZE
 */
static int __nvt_aes_crypt(mbedtls_aes_context *ctx,
						   const unsigned char *input,
						   unsigned char *output, size_t dataSize)
{
	int  ret, sid;

	if(dataSize > AES_BLOCK_SIZE)
		return -1;

	memcpy(nc_ptr(s_u8in), input, dataSize);

	ret = TSI_Open_Session(C_CODE_AES, &sid);
	if (ret != 0)
		goto err_out;

	dcache_invalidate_by_mva(ctx->iv, sizeof(ctx->iv));

	ret = TSI_AES_Set_IV(sid, ptr_to_u32(ctx->iv));
	if (ret != 0)
		goto err_out;

	dcache_invalidate_by_mva(ctx->keys, ctx->keySize);

	ret = TSI_AES_Set_Key(sid, ctx->keySizeOp, ptr_to_u32(ctx->keys));
	if (ret != 0)
		goto err_out;

	ret = TSI_AES_Set_Mode(sid,             /* sid        */
						   1,               /* kinswap    */
						   0,               /* koutswap   */
						   1,               /* inswap     */
						   1,               /* outswap    */
						   0,               /* sm4en      */
						   ctx->encDec,     /* encrypt    */
						   ctx->opMode,     /* mode       */
						   ctx->keySizeOp,  /* keysz      */
						   0,               /* ks         */
						   0                /* ks_num     */
						   );
	if (ret != 0)
		goto err_out;

	ret = TSI_AES_Run(sid, 1, dataSize, ptr_to_u32(s_u8in), ptr_to_u32(s_u8out));
	if (ret != 0)
		goto err_out;

	TSI_Close_Session(C_CODE_AES, sid);

	memcpy(output, nc_ptr(s_u8out), dataSize);

	return 0;

err_out:
	sysprintf("TSI AES ERROR!!! 0x%x\n", ret);
	TSI_Close_Session(C_CODE_AES, sid);
	TSI_Print_Error(ret);
	return -1;
}

/*
 * AES-ECB block encryption
 */
void mbedtls_aes_encrypt(mbedtls_aes_context *ctx,
						 const unsigned char input[AES_BLOCK_SIZE],
						 unsigned char output[AES_BLOCK_SIZE])
{
	ctx->encDec = 1;
	__nvt_aes_crypt(ctx, input, output, AES_BLOCK_SIZE);
}

/*
 * AES-ECB block decryption
 */
void mbedtls_aes_decrypt(mbedtls_aes_context *ctx,
						 const unsigned char input[AES_BLOCK_SIZE],
						 unsigned char output[AES_BLOCK_SIZE])
{
	ctx->encDec = 0;
	__nvt_aes_crypt(ctx, input, output, AES_BLOCK_SIZE);
}

/*
 * AES-ECB block encryption/decryption
 */
int mbedtls_aes_crypt_ecb(mbedtls_aes_context *ctx,
						  int mode,
						  const unsigned char input[AES_BLOCK_SIZE],
						  unsigned char output[AES_BLOCK_SIZE])
{

	AES_VALIDATE_RET(ctx != NULL);
	AES_VALIDATE_RET(input != NULL);
	AES_VALIDATE_RET(output != NULL);
	AES_VALIDATE_RET(mode == MBEDTLS_AES_ENCRYPT ||
		mode == MBEDTLS_AES_DECRYPT);

	ctx->opMode = AES_MODE_ECB;
	if(mode == MBEDTLS_AES_ENCRYPT)
		mbedtls_aes_encrypt(ctx, input, output);
	else
		mbedtls_aes_decrypt(ctx, input, output);


	return(0);
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)
/*
 * AES-CBC buffer encryption/decryption
 */
int mbedtls_aes_crypt_cbc(mbedtls_aes_context *ctx,
						  int mode,
						  size_t len,
						  unsigned char iv[AES_BLOCK_SIZE],
						  const unsigned char *input,
						  unsigned char *output)
{
	int  ret, sid;

	ctx->opMode = AES_MODE_CBC;

	if (mode == MBEDTLS_AES_ENCRYPT)
		ctx->encDec = 1;
	else
		ctx->encDec = 0;

	/* Fetch IV byte data in little-endian */
	ctx->iv[0] = nu_get32_le(iv);
	ctx->iv[1] = nu_get32_le(iv + 4);
	ctx->iv[2] = nu_get32_le(iv + 8);
	ctx->iv[3] = nu_get32_le(iv + 12);

	ret = TSI_Open_Session(C_CODE_AES, &sid);
	if (ret != 0)
		goto err_out;

	dcache_invalidate_by_mva(ctx->iv, sizeof(ctx->iv));

	ret = TSI_AES_Set_IV(sid, ptr_to_u32(ctx->iv));
	if (ret != 0)
		goto err_out;

	dcache_invalidate_by_mva(ctx->keys, ctx->keySize);
	ret = TSI_AES_Set_Key(sid, ctx->keySizeOp, ptr_to_u32(ctx->keys));
	if (ret != 0)
		goto err_out;

	ret = TSI_AES_Set_Mode(sid,             /* sid        */
						   1,               /* kinswap    */
						   1,               /* koutswap   */
						   1,               /* inswap     */
						   1,               /* outswap    */
						   0,               /* sm4en      */
						   ctx->encDec,     /* encrypt    */
						   ctx->opMode,     /* mode       */
						   ctx->keySizeOp,  /* keysz      */
						   0,               /* ks         */
						   0                /* ks_num     */
						   );
	if (ret != 0)
		goto err_out;

	memcpy(nc_ptr(s_u8in), input, len);

	ret = TSI_AES_Run(sid, 1, len, ptr_to_u32(s_u8in), ptr_to_u32(s_u8out));
	if (ret != 0)
		goto err_out;

	TSI_Close_Session(C_CODE_AES, sid);

	if (mode == MBEDTLS_AES_ENCRYPT)
		memcpy(iv, nc_ptr(s_u8out), 16);
	else
		memcpy(iv, input, 16);

	memcpy(output, nc_ptr(s_u8out), len);

	return 0;

err_out:
	sysprintf("TSI AES ERROR!!! 0x%x\n", ret);
	TSI_Close_Session(C_CODE_AES, sid);
	TSI_Print_Error(ret);
	return -1;
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
int mbedtls_aes_crypt_cfb128(mbedtls_aes_context *ctx,
							 int mode,
							 size_t length,
							 size_t *iv_off,
							 unsigned char iv[AES_BLOCK_SIZE],
							 const unsigned char *input,
							 unsigned char *output)
{
	int  ret, sid;

	ctx->opMode = AES_MODE_CFB;

	if (mode == MBEDTLS_AES_ENCRYPT)
		ctx->encDec = 1;
	else
		ctx->encDec = 0;

	/* Fetch IV byte data in little-endian */
	ctx->iv[0] = nu_get32_le(iv);
	ctx->iv[1] = nu_get32_le(iv + 4);
	ctx->iv[2] = nu_get32_le(iv + 8);
	ctx->iv[3] = nu_get32_le(iv + 12);

	ret = TSI_Open_Session(C_CODE_AES, &sid);
	if (ret != 0)
		goto err_out;

	dcache_invalidate_by_mva(ctx->iv, sizeof(ctx->iv));

	ret = TSI_AES_Set_IV(sid, ptr_to_u32(ctx->iv));
	if (ret != 0)
		goto err_out;

	dcache_invalidate_by_mva(ctx->keys, ctx->keySize);
	ret = TSI_AES_Set_Key(sid, ctx->keySizeOp, ptr_to_u32(ctx->keys));
	if (ret != 0)
		goto err_out;

	ret = TSI_AES_Set_Mode(sid,             /* sid        */
						   1,               /* kinswap    */
						   1,               /* koutswap   */
						   1,               /* inswap     */
						   1,               /* outswap    */
						   0,               /* sm4en      */
						   ctx->encDec,     /* encrypt    */
						   ctx->opMode,     /* mode       */
						   ctx->keySizeOp,  /* keysz      */
						   0,               /* ks         */
						   0                /* ks_num     */
						   );
	if (ret != 0)
		goto err_out;

	memcpy(nc_ptr(s_u8in), input, length);

	ret = TSI_AES_Run(sid, 1, length, ptr_to_u32(s_u8in), ptr_to_u32(s_u8out));
	if (ret != 0)
		goto err_out;

	memcpy(output, nc_ptr(s_u8out), length);

	TSI_Close_Session(C_CODE_AES, sid);
	return 0;

err_out:
	sysprintf("TSI AES ERROR!!! 0x%x\n", ret);
	TSI_Close_Session(C_CODE_AES, sid);
	TSI_Print_Error(ret);
	return -1;
}


/*
 * AES-CFB8 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb8(mbedtls_aes_context *ctx,
						   int mode,
						   size_t length,
						   unsigned char iv[AES_BLOCK_SIZE],
						   const unsigned char *input,
						   unsigned char *output)
{
	unsigned char c;
	unsigned char ov[AES_BLOCK_SIZE + 1];

	AES_VALIDATE_RET(ctx != NULL);
	AES_VALIDATE_RET(mode == MBEDTLS_AES_ENCRYPT ||
		mode == MBEDTLS_AES_DECRYPT);
	AES_VALIDATE_RET(iv != NULL);
	AES_VALIDATE_RET(input != NULL);
	AES_VALIDATE_RET(output != NULL);

	while(length--)
	{
		memcpy(ov, iv, AES_BLOCK_SIZE);
		mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);

		if(mode == MBEDTLS_AES_DECRYPT)
			ov[AES_BLOCK_SIZE] = *input;

		c = *output++ = (unsigned char)(iv[0] ^ *input++);

		if(mode == MBEDTLS_AES_ENCRYPT)
			ov[AES_BLOCK_SIZE] = c;

		memcpy(iv, ov + 1, AES_BLOCK_SIZE);
	}

	return(0);
}
#endif /*MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
/*
 * AES-CTR buffer encryption/decryption
 */
int mbedtls_aes_crypt_ctr(mbedtls_aes_context *ctx,
						  size_t length,
						  size_t *nc_off,
						  unsigned char nonce_counter[AES_BLOCK_SIZE],
						  unsigned char stream_block[AES_BLOCK_SIZE],
						  const unsigned char *input,
						  unsigned char *output)
{
	int c, i;
	size_t n = *nc_off;

	AES_VALIDATE_RET(ctx != NULL);
	AES_VALIDATE_RET(nc_off != NULL);
	AES_VALIDATE_RET(nonce_counter != NULL);
	AES_VALIDATE_RET(stream_block != NULL);
	AES_VALIDATE_RET(input != NULL);
	AES_VALIDATE_RET(output != NULL);

	while(length--)
	{
		if(n == 0)
		{
			mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, nonce_counter, stream_block);

			for(i = AES_BLOCK_SIZE; i > 0; i--)
				if(++nonce_counter[i - 1] != 0)
					break;
		}
		c = *input++;
		*output++ = (unsigned char)(c ^ stream_block[n]);

		n = (n + 1) & 0x0F;
	}

	*nc_off = n;

	return(0);
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#if defined(MBEDTLS_CIPHER_MODE_OFB)
/*
 * AES-CTR buffer encryption/decryption
 */
int mbedtls_aes_crypt_ofb(mbedtls_aes_context* ctx,
						  size_t length,
						  size_t* iv_off,
						  unsigned char iv[AES_BLOCK_SIZE],
						  const unsigned char* input,
						  unsigned char* output)
{
	int  ret, sid;

	if(length == 0)
	{
		/* Just do nothing */
		return (0);
	}

	if((ctx == NULL) || (iv == NULL) || (input == NULL) || (output == NULL))
	{
		return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
	}

	if((iv_off != NULL) && (*iv_off != 0))
	{
		/* only support iv_off == 0 */
		return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
	}


	if(*iv_off != 0)
	{
		sysprintf("iv_off is not zero!\n");

	}

	ctx->opMode = AES_MODE_OFB;
	ctx->encDec = 1;

	/* Fetch IV byte data in little-endian */
	ctx->iv[0] = nu_get32_le(iv);
	ctx->iv[1] = nu_get32_le(iv + 4);
	ctx->iv[2] = nu_get32_le(iv + 8);
	ctx->iv[3] = nu_get32_le(iv + 12);

	ret = TSI_Open_Session(C_CODE_AES, &sid);
	if (ret != 0)
		goto err_out;

	dcache_invalidate_by_mva(ctx->iv, sizeof(ctx->iv));

	ret = TSI_AES_Set_IV(sid, ptr_to_u32(ctx->iv));
	if (ret != 0)
		goto err_out;

	dcache_invalidate_by_mva(ctx->keys, ctx->keySize);
	ret = TSI_AES_Set_Key(sid, ctx->keySizeOp, ptr_to_u32(ctx->keys));
	if (ret != 0)
		goto err_out;

	ret = TSI_AES_Set_Mode(sid,             /* sid        */
						   1,               /* kinswap    */
						   1,               /* koutswap   */
						   1,               /* inswap     */
						   1,               /* outswap    */
						   0,               /* sm4en      */
						   ctx->encDec,     /* encrypt    */
						   ctx->opMode,     /* mode       */
						   ctx->keySizeOp,  /* keysz      */
						   0,               /* ks         */
						   0                /* ks_num     */
						   );
	if (ret != 0)
		goto err_out;

	memcpy(nc_ptr(s_u8in), input, length);

	ret = TSI_AES_Run(sid, 1, length, ptr_to_u32(s_u8in), ptr_to_u32(s_u8out));
	if (ret != 0)
		goto err_out;

	memcpy(output, nc_ptr(s_u8out), length);

	TSI_Close_Session(C_CODE_AES, sid);
	return 0;

err_out:
	sysprintf("TSI AES ERROR!!! 0x%x\n", ret);
	TSI_Close_Session(C_CODE_AES, sid);
	TSI_Print_Error(ret);
	return -1;
}
#endif /* MBEDTLS_CIPHER_MODE_OFB */

#endif /* MBEDTLS_AES_ALT */

#endif /* MBEDTLS_AES_C */

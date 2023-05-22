/*
 *  FIPS-180-2 compliant SHA-256 implementation
 *
 *  Copyright (C) 2006-2021, ARM Limited, All Rights Reserved
 *  Copyright (c) 2022 Nuvoton Technology Corp. All rights reserved.
 *
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file implements STMicroelectronics SHA256 with HW services based on mbed TLS API
 */
/*
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#ifdef MBEDTLS_ALLOW_PRIVATE_ACCESS
#undef MBEDTLS_ALLOW_PRIVATE_ACCESS
#endif

#include "common.h"

#include "mbedtls/sha256.h"
#include "mbedtls/error.h"

#if defined(MBEDTLS_SHA256_C)
#if defined(MBEDTLS_SHA256_ALT)
#include <string.h>
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "NuMicro.h"
#include "tsi_cmd.h"

#define SHA256_VALIDATE_RET(cond)                           \
	MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_SHA256_BAD_INPUT_DATA )
#define SHA256_VALIDATE(cond)  MBEDTLS_INTERNAL_VALIDATE( cond )

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif


static void mbedtls_zeroize(void *v, size_t n)
{
	volatile unsigned char *p = (unsigned char *)v;
	while (n--)
	{
		*p++ = 0;
	}
}

void mbedtls_sha256_init(mbedtls_sha256_context *ctx)
{
	SHA256_VALIDATE( ctx != NULL );

	mbedtls_zeroize(ctx, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_free(mbedtls_sha256_context *ctx)
{
	if (ctx == NULL)
	{
		return;
	}
	mbedtls_zeroize(ctx, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_clone(mbedtls_sha256_context *dst,
						  const mbedtls_sha256_context *src)
{
	SHA256_VALIDATE( dst != NULL );
	SHA256_VALIDATE( src != NULL );

	*dst = *src;
}

int mbedtls_sha256_starts(mbedtls_sha256_context *ctx, int is224)
{
	uint32_t u32OpMode;
	int      ret;

	SHA256_VALIDATE_RET( ctx != NULL );
	SHA256_VALIDATE_RET( is224 == 0 || is224 == 1 );

	ctx->MBEDTLS_PRIVATE(is224) = is224;
	ctx->first = 1;

	if(ctx->MBEDTLS_PRIVATE(is224))
		u32OpMode = SHA_MODE_SHA224;
	else
		u32OpMode = SHA_MODE_SHA256;

	ret = TSI_Open_Session(C_CODE_SHA, &ctx->sid);
	if (ret != 0)
		goto err_out;

	ret = TSI_SHA_Start(ctx->sid, 1, 1, 0, 0, u32OpMode, 0, 0, 0);
	if (ret != 0)
		return ret;

	return 0;

err_out:
	sysprintf("TSI SHA ERROR!!! 0x%x\n", ret);
	TSI_Close_Session(C_CODE_SHA, ctx->sid);
	TSI_Print_Error(ret);
	return -1;
}

int mbedtls_internal_sha256_process( mbedtls_sha256_context *ctx, const unsigned char data[NU_SHA256_BLOCK_SIZE] )
{
	ARG_UNUSED(ctx);
	ARG_UNUSED(data);

	return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
}

int mbedtls_sha256_update(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen)
{
	uint32_t ofs = 0;
	int     ret;

	SHA256_VALIDATE_RET( ctx != NULL );
	SHA256_VALIDATE_RET( ilen == 0 || input != NULL );

	/* Process all data block by block. the remain data will leave to next time. */
	while (ilen > 0)
	{
		/* Process the buffer if it is full */
		if (ctx->buffer_len >= NU_SHA256_BLOCK_SIZE)
		{
			dcache_invalidate_by_mva(ctx->buffer, NU_SHA256_BLOCK_SIZE);
			ret = TSI_SHA_Update(ctx->sid, NU_SHA256_BLOCK_SIZE, ptr_to_u32(ctx->buffer));
			if (ret != 0)
				return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
			ctx->buffer_len = 0;
		}
		else
		{
			/* prepare input block */
			if(ilen > NU_SHA256_BLOCK_SIZE - ctx->buffer_len)
			{
				memcpy(ctx->buffer + ctx->buffer_len, input + ofs, NU_SHA256_BLOCK_SIZE - ctx->buffer_len);
				ilen -= NU_SHA256_BLOCK_SIZE - ctx->buffer_len;
				ofs  += NU_SHA256_BLOCK_SIZE - ctx->buffer_len;
				ctx->buffer_len = NU_SHA256_BLOCK_SIZE;
			}
			else
			{
				memcpy(ctx->buffer + ctx->buffer_len, input + ofs, ilen);
				ctx->buffer_len += ilen;
				ilen = 0;
			}
		}
	}
	return 0;
}

static uint8_t  _sha256_dma_buff[32] __attribute__((aligned(32)));

int mbedtls_sha256_finish(mbedtls_sha256_context *ctx, unsigned char output[32])
{
	int32_t ret;

	SHA256_VALIDATE_RET( ctx != NULL );
	SHA256_VALIDATE_RET( (unsigned char *)output != NULL );

	/* Process the buffer if it is not emtpy */
	if (ctx->buffer_len > 0)
	{
		dcache_invalidate_by_mva(ctx->buffer, NU_SHA256_BLOCK_SIZE);
		ret = TSI_SHA_Finish(ctx->sid, 8, ctx->buffer_len,
							 ptr_to_u32(ctx->buffer), ptr_to_u32(_sha256_dma_buff));
		if (ret != 0)
			return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;

		memcpy(output, nc_ptr(_sha256_dma_buff), 32);
		TSI_Close_Session(C_CODE_SHA, ctx->sid);
		ctx->buffer_len = 0;
	}
	return 0;
}

#endif /* MBEDTLS_SHA256_ALT*/
#endif /* MBEDTLS_SHA256_C */

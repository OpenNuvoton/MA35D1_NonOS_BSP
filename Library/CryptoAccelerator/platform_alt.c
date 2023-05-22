/*
 *  Platform abstraction layer
 *
 *  Copyright The Mbed TLS Contributors
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
 */

#include "common.h"

#if defined(MBEDTLS_PLATFORM_C)

#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"

#include "NuMicro.h"


#if defined(MBEDTLS_PLATFORM_EXIT_ALT)
#if defined(MBEDTLS_PLATFORM_STD_EXIT)
 /*
  * Make dummy function to prevent NULL pointer dereferences
  */
static void platform_exit_uninit(int status)
{
    ((void)status);
    for(;;) {}
}


#endif /* !MBEDTLS_PLATFORM_STD_EXIT */

void (*mbedtls_exit)(int status) = MBEDTLS_PLATFORM_STD_EXIT;

int mbedtls_platform_set_exit(void (*exit_func)(int status))
{
    mbedtls_exit = exit_func;
    return(0);
}
#endif /* MBEDTLS_PLATFORM_EXIT_ALT */

#if defined(MBEDTLS_PLATFORM_SETUP_TEARDOWN_ALT)
/*
 * Placeholder platform setup that does nothing by default
 */
int mbedtls_platform_setup( mbedtls_platform_context *ctx )
{
    (void)ctx;

    return( 0 );
}

/*
 * Placeholder platform teardown that does nothing by default
 */
void mbedtls_platform_teardown( mbedtls_platform_context *ctx )
{
    (void)ctx;
}
#endif /* MBEDTLS_PLATFORM_SETUP_TEARDOWN_ALT */


static char  hex_char_tbl[] = "0123456789abcdef";

static char get_Nth_nibble_char(uint32_t val32, uint32_t idx)
{
    return hex_char_tbl[ (val32 >> (idx * 4U)) & 0xfU ];
}

static char  ch2hex(char ch)
{
    if (ch <= '9')
    {
        ch = ch - '0';
    }
    else if ((ch <= 'z') && (ch >= 'a'))
    {
        ch = ch - 'a' + 10U;
    }
    else
    {
        ch = ch - 'A' + 10U;
    }
    return ch;
}

void MbedTLS_ALT_Hex2Reg(char input[], uint32_t volatile reg[])
{
    char      hex;
    int       si, ri;
    uint32_t  i, val32;

    si = (int)strlen(input) - 1;
    ri = 0;
    
    // printf("<%d>\n", si);

    while (si >= 0)
    {
        val32 = 0UL;
        for (i = 0UL; (i < 8UL) && (si >= 0); i++)
        {
            hex = ch2hex(input[si]);
            val32 |= (uint32_t)hex << (i * 4UL);
            si--;
        }
        reg[ri++] = val32;
    }
}

void MbedTLS_ALT_Reg2Hex(int32_t count, uint32_t volatile reg[], char output[])
{
    int32_t    idx, ri;
    uint32_t   i;

    output[count] = 0U;
    idx = count - 1;

    for (ri = 0; idx >= 0; ri++)
    {
        for (i = 0UL; (i < 8UL) && (idx >= 0); i++)
        {
            output[idx] = get_Nth_nibble_char(reg[ri], i);
            idx--;
        }
    }
}

static const struct curve_map curve_map_tbl[] =
{
    { MBEDTLS_ECP_DP_SECP192R1,  CURVE_P_192,  48 },
    { MBEDTLS_ECP_DP_SECP224R1,  CURVE_P_224,  56 },
    { MBEDTLS_ECP_DP_SECP256R1,  CURVE_P_256,  64 },
    { MBEDTLS_ECP_DP_SECP384R1,  CURVE_P_384,  96 },
    { MBEDTLS_ECP_DP_SECP521R1,  CURVE_P_521,  131},
    { MBEDTLS_ECP_DP_BP256R1,    CURVE_BP_256, 64 },
    { MBEDTLS_ECP_DP_BP384R1,    CURVE_BP_384, 96 },
    { MBEDTLS_ECP_DP_BP512R1,    CURVE_BP_512, 128},
    { MBEDTLS_ECP_DP_SECP192K1,  CURVE_KO_192, 48 },
    { MBEDTLS_ECP_DP_SECP224K1,  CURVE_KO_224, 56 },
    { MBEDTLS_ECP_DP_SECP256K1,  CURVE_KO_256, 64 },
};

E_ECC_CURVE  MbedTLS_ALT_get_curve(mbedtls_ecp_group* grp, int *echar)
{
    int  i;	
    
    for (i = 0; i < sizeof(curve_map_tbl) / sizeof(struct curve_map); i++)
    {
    	if (curve_map_tbl[i].id == grp->id)
    	{
    		sysprintf("MbedTLS_ALT_get_curve - %d, %d, %d\n", grp->id, curve_map_tbl[i].curve, curve_map_tbl[i].echar);
    		*echar = curve_map_tbl[i].echar;
    		return curve_map_tbl[i].curve;
    	}
	}
	return CURVE_UNDEF;
}

void MbedTLS_ALT_ECC_Copy(uint32_t *dest, uint32_t *src, uint32_t size)
{
    uint32_t u32Data, *pu32Dest, *pu32Src;
    int32_t i;
    uint32_t len;
    uint8_t* pu8;

    len = (uint32_t)size;
    pu32Dest = (uint32_t*)dest;
    pu32Src = (uint32_t*)src;
    for(i = 0; i < len / 4; i++)
    {
        *pu32Dest++ = *pu32Src++;
    }

    len = size & 0x3;
    if(len > 0)
    {
        pu8 = (uint8_t*)pu32Src;
        u32Data = 0;
        for(i = 0; i < len; i++)
        {
            u32Data += (*pu8++) << (i*8);
        }

        *pu32Dest = u32Data;
    }
}

/* Add mission parameters of the curve */
int MbedTLS_ALT_ECC_FixCurve(mbedtls_ecp_group* grp)
{

    if(grp->MBEDTLS_PRIVATE(T) == NULL)
    {
        grp->MBEDTLS_PRIVATE(T) = mbedtls_calloc(1, sizeof(mbedtls_ecp_point));
        if(grp->MBEDTLS_PRIVATE(T) == NULL)
        {
            return MBEDTLS_ERR_ECP_ALLOC_FAILED;
        }

        mbedtls_ecp_point_init(grp->MBEDTLS_PRIVATE(T));
        mbedtls_mpi_lset(&grp->MBEDTLS_PRIVATE(T)->MBEDTLS_PRIVATE(Z), 0);
        grp->MBEDTLS_PRIVATE(T_size) = 1;
    }


    if(mbedtls_mpi_size(&grp->A) < 1)
    {
        if(grp->id == MBEDTLS_ECP_DP_SECP192R1)
        {
            mbedtls_mpi_read_string(&grp->A, 16, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFC");
            //mbedtls_mpi_read_string(&grp->MBEDTLS_PRIVATE(T)->MBEDTLS_PRIVATE(X), 16, "188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012");
            //mbedtls_mpi_read_string(&grp->MBEDTLS_PRIVATE(T)->MBEDTLS_PRIVATE(Y), 16, "07192b95ffc8da78631011ed6b24cdd573f977a11e794811");
        }
        else if(grp->id == MBEDTLS_ECP_DP_SECP224R1)
        {
            mbedtls_mpi_read_string(&grp->A, 16, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFE");
            //mbedtls_mpi_read_string(&grp->MBEDTLS_PRIVATE(T)->MBEDTLS_PRIVATE(X), 16, "b70e0cbd6bb4bf7f321390b94a03c1d356c21122343280d6115c1d21");
            //mbedtls_mpi_read_string(&grp->MBEDTLS_PRIVATE(T)->MBEDTLS_PRIVATE(Y), 16, "bd376388b5f723fb4c22dfe6cd4375a05a07476444d5819985007e34");
        }
        else if(grp->id == MBEDTLS_ECP_DP_SECP256R1)
        {
            mbedtls_mpi_read_string(&grp->A, 16, "FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFC");
            //mbedtls_mpi_read_string(&grp->MBEDTLS_PRIVATE(T)->MBEDTLS_PRIVATE(X), 16, "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296");
            //mbedtls_mpi_read_string(&grp->MBEDTLS_PRIVATE(T)->MBEDTLS_PRIVATE(Y), 16, "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5");
        }
        else if(grp->id == MBEDTLS_ECP_DP_SECP384R1)
        {
            mbedtls_mpi_read_string(&grp->A, 16, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFC");
            //mbedtls_mpi_read_string(&grp->MBEDTLS_PRIVATE(T)->MBEDTLS_PRIVATE(X), 16, "aa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab7");
            //mbedtls_mpi_read_string(&grp->MBEDTLS_PRIVATE(T)->MBEDTLS_PRIVATE(Y), 16, "3617de4a96262c6f5d9e98bf9292dc29f8f41dbd289a147ce9da3113b5f0b8c00a60b1ce1d7e819d7a431d7c90ea0e5f");
        }
        else if(grp->id == MBEDTLS_ECP_DP_SECP521R1)
        {
            mbedtls_mpi_read_string(&grp->A, 16, "1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC");
            //mbedtls_mpi_read_string(&grp->MBEDTLS_PRIVATE(T)->MBEDTLS_PRIVATE(X), 16, "0c6858e06b70404e9cd9e3ecb662395b4429c648139053fb521f828af606b4d3dbaa14b5e77efe75928fe1dc127a2ffa8de3348b3c1856a429bf97e7e31c2e5bd66");
            //mbedtls_mpi_read_string(&grp->MBEDTLS_PRIVATE(T)->MBEDTLS_PRIVATE(Y), 16, "11839296a789a3bc0045c8a5fb42c7d1bd998f54449579b446817afbd17273e662c97ee72995ef42640c550b9013fad0761353c7086a272c24088be94769fd16650");
        }
        else if(grp->id == MBEDTLS_ECP_DP_CURVE25519)
        {
            mbedtls_mpi_read_string(&grp->A, 16, "0000000000000000000000000000000000000000000000000000000000076D06");
            mbedtls_mpi_read_string(&grp->B, 16, "0000000000000000000000000000000000000000000000000000000000000001");
            mbedtls_mpi_read_string(&grp->P, 16, "7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffed");
            mbedtls_mpi_read_string(&grp->N, 16, "1000000000000000000000000000000014def9dea2f79cd65812631a5cf5d3ed");
            //mbedtls_mpi_read_string(&grp->MBEDTLS_PRIVATE(T)->MBEDTLS_PRIVATE(X), 16, "0000000000000000000000000000000000000000000000000000000000000009");
            //mbedtls_mpi_read_string(&grp->MBEDTLS_PRIVATE(T)->MBEDTLS_PRIVATE(Y), 16, "20ae19a1b8a086b4e01edd2c7748d14c923d4d7e6d7c61b229e9c5a27eced3d9");
        }
    }
    return 0;
}

#endif /* MBEDTLS_PLATFORM_C */

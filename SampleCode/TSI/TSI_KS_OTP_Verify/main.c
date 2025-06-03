/**************************************************************************//**
 * @file     main.c
 * @brief    This sample program demonstrates how to enable Crypto AES and ECC
 *           to verify if OTP keys are fone.
 *           This sample demonstrates how to verify the correctness of OTP keys
 *           using the Crypto AES and ECC engines. Users must know the expected
 *           content of the OTP keys in advance and fill them into the
 *           `my_opt_keys[]` array in the code. The verification process works
 *           by performing AES/ECC operations separately on both the known
 *           plaintext and the OTP keys. If the outputs match, it confirms that
 *           the programmed OTP key is identical to the declared plaintext.
 *
 * @copyright (C) 2025 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"

#define KEY_TYPE_NONE    0  /*  for ignore key */
#define KEY_TYPE_AES     1
#define KEY_TYPE_ECC     2

typedef struct key_table
{
    int   knum;
    int   key_type;
    char  name[64];
    int   keylen;
    char  key[128];
} KEYTBL_T;

#define AES_256_ECB     ((AES_MODE_ECB << AES_OPMODE_Pos) | (AES_KEY_SIZE_256 << AES_KEY_SIZE_Pos))

static const uint32_t aes_iv[4] __attribute__((aligned(32))) = { 0 };

static uint8_t aes_input[64] __attribute__((aligned(32)));

static uint32_t key_buff[64] __attribute__((aligned(32)));

static uint8_t out_buff1[1024] __attribute__((aligned(32)));
static uint8_t out_buff2[1024] __attribute__((aligned(32)));

/*
 *  Example OTP user key .jason file for NuWriter
 *      {
 *          "mac0": "001938722942",
 *          "mac1": "001939214545",
 *          "huk0": {
 *              "key": "81c326ad28501fe75f65c7c0a0fa7f39"
 *          },
 *          "huk1": {
 *              "key": "3ea98d4b7b83793d546f9bf380e35712"
 *          },
 *          "huk2": {
 *              "key": "a2a263ac49e447426dc7e05195b9e542"
 *          },
 *          "key3": {
 *              "key": "d37d7ab68bb84d7e89111b615b32dfa0e4299ef5190ea05b6d692d32df060068",
 *              "meta": "aes256-cpu-readable"
 *          },
 *          "key4": {
 *              "key": "fa2737fb93488d19caef11ae7faf6b7f4bcd67b286e3fc54e8a65c2b74aeccb0",
 *              "meta": "eccp256-cpu-readable"
 *          },
 *          "key5": {
 *              "key": "d4ccd6dae698208aa8c3a6f39e45510d03be09b2f124bfc067856c324f9b4d09",
 *              "meta": "eccp256-cpu-readable"
 *          }
 *     }
 *
 *  Example OTP secure boot key .jason file for NuWriter
 *     {
 *          "publicx": "72F84F681092E3A05C1437E3E40534962A5C70556025D348FF9DB97D6AF83EB5",
 *          "publicy": "8D32DAC7AB6F90332E8E0060E159E0B31502BB4FB2D78369F02D1F5B0C335AD3",
 *          "aeskey": "0A0BC81E5AFBF836C5E0CFBEA12C1E269A2EBC3B0B6EC39EEE1C7142F760EBC4"
} *    }
 */

KEYTBL_T my_opt_keys[9] =
{
    {
        0,
        KEY_TYPE_NONE,
        "HUK-0",
        128/8,
        ""
    },
    {
        1,
        KEY_TYPE_NONE,
        "HUK-1",
        128/8,
        ""
    },
    {
        2,
        KEY_TYPE_NONE,
        "HUK-2",
        128/8,
        ""
    },
    {
        3,
        KEY_TYPE_AES,
        "USER KEY - AES",
        256,
        "d37d7ab68bb84d7e89111b615b32dfa0e4299ef5190ea05b6d692d32df060068"
    },
    {
        4,
        KEY_TYPE_ECC,
        "USER KEY - ECC X",
        256,
        "fa2737fb93488d19caef11ae7faf6b7f4bcd67b286e3fc54e8a65c2b74aeccb0"
    },
    {
        5,
        KEY_TYPE_ECC,
        "USER KEY - ECC Y",
        256,
        "d4ccd6dae698208aa8c3a6f39e45510d03be09b2f124bfc067856c324f9b4d09"
    },
    {
        6,
        KEY_TYPE_ECC,
        "SECURE BOOT KEY - ECC X",
        256,
        "72F84F681092E3A05C1437E3E40534962A5C70556025D348FF9DB97D6AF83EB5"
    },
    {
        7,
        KEY_TYPE_ECC,
        "SECURE BOOT KEY - ECC Y",
        256,
        "8D32DAC7AB6F90332E8E0060E159E0B31502BB4FB2D78369F02D1F5B0C335AD3"
    },
    {
        8,
        KEY_TYPE_AES,
        "SECURE BOOT KEY - AES",
        256,
        "0A0BC81E5AFBF836C5E0CFBEA12C1E269A2EBC3B0B6EC39EEE1C7142F760EBC4"
    },
};

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

int str2hex(char *str, uint8_t *hex)
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

void swap_ecc_str(char *hex, int len)
{
    char c_tmp[2];
    int i, c_len;

    c_len = len / 4;   /* ex. 64 characters for 256 bits */

    for (i = 0; i < c_len / 2; i += 2)
    {
        c_tmp[0] = hex[i];
        c_tmp[1] = hex[i+1];

        hex[i] = hex[c_len - (i+1) - 1];
        hex[i+1] = hex[c_len - i - 1];

        hex[c_len - (i+1) - 1] = c_tmp[0];
        hex[c_len - i - 1] = c_tmp[1];
    }
}


void  dump_ecc_string(uint8_t *pucBuff, int blen)
{
    uint64_t  addr, end_addr;

    addr = (uint64_t)pucBuff;
    end_addr = addr + blen / 4;

    for ( ; addr < end_addr; addr++)
        sysprintf("%c", readb((uint8_t *)addr) & 0xff);
    sysprintf("\n");
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

int check_aes_otp_key(KEYTBL_T *kt)
{
    int i, sid, ret;
    uint32_t *out1, *out2;
    uint8_t  hex_buff[256];
    uint32_t *p;

    out1 = nc_ptr(out_buff1);
    out2 = nc_ptr(out_buff2);

    i = str2hex(kt->key, hex_buff);

    p = nc_ptr(key_buff);

    if (kt->knum == 8)
    {
        for (i = 0; i < kt->keylen / 8; i += 4, p++)
            *p = (hex_buff[i] << 24) | (hex_buff[i+1] << 16) | (hex_buff[i+2] << 8) | hex_buff[i+3];
    }
    else
    {
        for (i = 0; i < kt->keylen / 8; i += 4, p++)
            *p = (hex_buff[i+3] << 24) | (hex_buff[i+2] << 16) | (hex_buff[i+1] << 8) | hex_buff[i];
    }

    /* input data; any random data here */
    str2hex("0336763e966d92595a567cc9ce537f5e", aes_input);

    ret = TSI_Open_Session(C_CODE_AES, &sid);
    if (ret != 0)
        goto tsi_err;

    ret = TSI_AES_Set_IV(sid, ptr_to_u32(aes_iv));
    if (ret != 0)
        goto tsi_err;

    ret = TSI_AES_Set_Key(sid, AES_KEY_SIZE_256, ptr_to_u32(key_buff));
    if (ret != 0)
        goto tsi_err;

    ret = TSI_AES_Set_Mode(sid,                  /* sid        */
                           0,                    /* kinswap    */
                           0,                    /* koutswap   */
                           1,                    /* inswap     */
                           1,                    /* outswap    */
                           0,                    /* sm4en      */
                           1,                    /* encrypt    */
                           AES_MODE_ECB,         /* mode       */
                           AES_KEY_SIZE_256,     /* keysz      */
                           0,                    /* ks         */
                           0                     /* ks_num     */
                           );
    if (ret != 0)
        goto tsi_err;

    ret = TSI_AES_Run(sid, 1, 16, ptr_to_u32(aes_input), ptr_to_u32(out1));
    if (ret != 0)
        goto tsi_err;

    TSI_Close_Session(C_CODE_AES, sid);

    sysprintf("AES OUT1 = ");
    for (i = 0; i < 8; i++)
        sysprintf("0x%x ", out1[i]);
    sysprintf("\n");

    ret = TSI_Open_Session(C_CODE_AES, &sid);
    if (ret != 0)
        goto tsi_err;

    ret = TSI_AES_Set_IV(sid, ptr_to_u32(aes_iv));
    if (ret != 0)
        goto tsi_err;

    ret = TSI_AES_Set_Mode(sid,                  /* sid        */
                           0,                    /* kinswap    */
                           0,                    /* koutswap   */
                           1,                    /* inswap     */
                           1,                    /* outswap    */
                           0,                    /* sm4en      */
                           1,                    /* encrypt    */
                           AES_MODE_ECB,         /* mode       */
                           AES_KEY_SIZE_256,     /* keysz      */
                           SEL_KEY_FROM_KS_OTP,  /* ks         */
                           kt->knum              /* ks_num     */
                           );
    if (ret != 0)
        goto tsi_err;

    ret = TSI_AES_Run(sid, 1, 16, ptr_to_u32(aes_input), ptr_to_u32(out2));
    if (ret != 0)
        goto tsi_err;

    TSI_Close_Session(C_CODE_AES, sid);

    sysprintf("AES OUT2 = ");
    for (i = 0; i < 8; i++)
        sysprintf("0x%x ", out2[i]);
    sysprintf("\n");

    if (memcmp(out1, out2, 16) != 0)
    {
        sysprintf("AES key mismatched!!\n");
        return -1;
    }

    return 0;

tsi_err:
    TSI_Close_Session(C_CODE_AES, sid);
    TSI_Print_Error(ret);
    return -1;
}

int check_ecc_otp_key(KEYTBL_T *kt)
{
    int i, ret;
    char     *out1, *out2;
    char     *kptr;

    sysprintf("kt->key #%d = ", kt->knum);
    dump_ecc_string(kt->key, 256);

    out1 = nc_ptr(out_buff1);
    out2 = nc_ptr(out_buff2);
    kptr = nc_ptr(key_buff);

    memcpy(kptr, kt->key, kt->keylen / 4);
    kptr[kt->keylen / 4] = 0;

    if (kt->knum < 6)
        swap_ecc_str(kptr, kt->keylen);

    sysprintf("key after swapped = ");
    dump_ecc_string(kptr, 256);

    memset(out1, 0, 256);
    memset(out2, 0, 256);

    ret = TSI_ECC_GenPublicKey(CURVE_P_256,   /* curve_id  */
                       0,                     /* is_ecdh   */
                       ECC_KEY_SEL_USER,      /* psel      */
                       0,                     /* d_knum    */
                       ptr_to_u32(kptr),      /* priv_key  */
                       ptr_to_u32(out_buff1)  /* pub_key   */
                       );
    if (ret != 0) {
        sysprintf("ECC key generation failed!!\n");
        TSI_Print_Error(ret);
        while (1);
    }

    sysprintf("ECC result1 (by user input): ");
    dump_ecc_string(out1, 256);

    ret = TSI_ECC_GenPublicKey(CURVE_P_256,   /* curve_id  */
                       0,                     /* is_ecdh   */
                       ECC_KEY_SEL_KS_OTP,    /* psel      */
                       kt->knum,              /* d_knum    */
                       0,                     /* priv_key  */
                       ptr_to_u32(out2)       /* pub_key   */
                       );
    if (ret != 0) {
        sysprintf("ECC key generation failed!!\n");
        TSI_Print_Error(ret);
        while (1);
    }

    sysprintf("ECC result2 (by OTP key): ");
    dump_ecc_string(out2, 256);

    if (memcmp(out1, out2, 256/4) != 0)
    {
        sysprintf("ECC key mismatched!!\n");
        dump_ecc_string(out1, 256);
        dump_ecc_string(out2, 256);
        return -1;
    }
    return 0;
}

static void ks_otp_try_read_all(void)
{
    uint32_t *au32RKey = nc_ptr(out_buff1);
    int i, knum, ret, wcnt;
    int found = 0;

    sysprintf("\nTry to read OTP key if available...\n");
    for (knum = 0; knum < KS_OTP_KEY_CNT; knum++)
    {
        if (knum < 3)
            wcnt = 128 / 32;
        else
            wcnt = 256 / 32;

        ret = TSI_KS_Read(KS_OTP, knum, au32RKey, wcnt);
        if (ret == 0)
        {
            sysprintf("OTP key %d, size %d:\n    ", knum, wcnt * 32);
            for (i = 0; i < wcnt; i++)
                sysprintf("0x%x ", au32RKey[i]);
            sysprintf("\n");
            found = 1;
        }
        else
            sysprintf("Read OTP key #%d failed. It may be empty or unreadable.\n", knum);
    }
    if (!found)
        sysprintf("No readable OTP keys found.\n");
    return;
}

int32_t main(void)
{
    int loop, i, len, sid, ret;
    uint8_t *out1, *out2;
    KEYTBL_T *kt;

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
    sysprintf("|  MA35D1 TSI - Verify Key Store OTP keys                       |\n");
    sysprintf("+---------------------------------------------------------------+\n");

    ks_otp_try_read_all();
    sysprintf("\n\n");

    for (i = 0; i < sizeof(my_opt_keys) / sizeof(KEYTBL_T); i++)
    {
        int       remain_size;
        uint32_t  ks_sts, ks_otpsts, ks_metadata;

        kt = nc_ptr(&my_opt_keys[i]);

        if (kt->key_type == KEY_TYPE_NONE)
            continue;

        sysprintf("\n[#%d] Verify OTP key #%d [%s]...\n", kt->knum, kt->knum, kt->name);

        if (kt->key_type == KEY_TYPE_AES)
            ret = check_aes_otp_key(kt);

        if (kt->key_type == KEY_TYPE_ECC)
            ret = check_ecc_otp_key(kt);

        if (ret == 0)
            sysprintf("[PASS]\n");
        else
            sysprintf("[FAIL]\n");

    }
    sysprintf("\nAll test vectors verified.\n");

    while (1);
}

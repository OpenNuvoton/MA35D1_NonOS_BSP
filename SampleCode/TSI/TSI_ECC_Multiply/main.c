/**************************************************************************//**
 * @file     main.c
 * @brief    This sample program demonstrates how to use the TSI commands to
 *           perform ECC point multiplication.
 *
 * @copyright (C) 2025 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"
#include "crypto.h"

#define MAX_KEY_LEN     168

typedef struct kp_tv_t
{
    char   curve_name[16];
    E_ECC_CURVE   curve;
    int    keylen;
    char   d[MAX_KEY_LEN];
    char   in_Qx[MAX_KEY_LEN];
    char   in_Qy[MAX_KEY_LEN];
    char   out_Qx[MAX_KEY_LEN];
    char   out_Qy[MAX_KEY_LEN];
}  KP_TV_T;

KP_TV_T _kp_pool[] __attribute__((aligned(32))) =
{
    {
        "P-256", CURVE_P_256, 256,
        "c68e25e09229a26642f67e90a15ece1fe9a8c6acaf9abb336cb29a3602fc9a96",
        "90861554b0621e6cfcf38ccc8462b3f8a74798acd018de69f285b8b5fdb0051a",
        "e1f6b170d4ce21bb7c16857f8b37ab3ec20ff632cb5f025689f8816310b0f449",
        "eb1193fbb08353f570bb821a1f120288c4e1d9bd65219d5218bd51e195819c3e",
        "40f09c54ca75dc36168dc226c2e0e35b505851298fde14dd19d598732a68f795"
    },
    {   /* public key */
        "P-256", CURVE_P_256, 256,
        "c68e25e09229a26642f67e90a15ece1fe9a8c6acaf9abb336cb29a3602fc9a96",
        "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296",
        "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5",
        "28a15ee9d111b8f82e94dd5b55e820cb77e3ed86b95aa0165bcef93ff7a6e79a",
        "fa83470ec4db951db9923e35c920a47c53bc3ddc61692de0355da20e6085fae3"
    },
};

char param_mem[2048] __attribute__((aligned(32)));
char out_mem[1280] __attribute__((aligned(32)));

int ecc_strcmp(char *s1, char *s2)
{
    char  c1, c2;

    while (*s1 == '0') s1++;
    while (*s2 == '0') s2++;

    for ( ; *s1 || *s2; s1++, s2++)
    {
        if ((*s1 >= 'A') && (*s1 <= 'Z'))
            c1 = *s1 + 32;
        else
            c1 = *s1;

        if ((*s2 >= 'A') && (*s2 <= 'Z'))
            c2 = *s2 + 32;
        else
            c2 = *s2;

        if (c1 != c2)
            return 1;
    }
    return 0;
}

int ecc_prints(char *head, char *s)
{
    int i;
    sysprintf("%s", head);

    for (i = 0; s[i] && (i < MAX_KEY_LEN); i++)
        sysprintf("%c", s[i]);
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

void delay_us(int usec)
{
    uint64_t   t0;
    t0  = EL0_GetCurrentPhysicalValue();
    while ((EL0_GetCurrentPhysicalValue() - t0) < (usec * 12));
}

int32_t main(void)
{
    int         i, ret;
    char        name[16];
    KP_TV_T     *tv;
    char        *param_block;
    char        *out_block, *out_x, *out_y;

    param_block = nc_ptr(param_mem);
    out_block = nc_ptr(out_mem);
    out_x = nc_ptr(out_mem);
    out_y = nc_ptr(&out_mem[576]);

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

    tv = nc_ptr(_kp_pool);

    sysprintf("+---------------------------------------------+\n");
    sysprintf("|   Crypto ECC Multiply Demo                  |\n");
    sysprintf("+---------------------------------------------+\n");

    for (i = 0; i < sizeof(_kp_pool) / sizeof(KP_TV_T); i++) {
        memset(name, 0, sizeof(name));
        strncpy(name, tv[i].curve_name, 5);
        sysprintf("Run curve %s test.......\n", name);


        memset(param_block, 0, 576 * 3);
        memset(out_block, 0, 576 * 2);

        memcpy(&param_block[0], tv[i].in_Qx, MAX_KEY_LEN);
        memcpy(&param_block[576], tv[i].in_Qy, MAX_KEY_LEN);
        memcpy(&param_block[1152], tv[i].d, MAX_KEY_LEN);

        ecc_prints("Multiplier : ", tv[i].d);
        ecc_prints("in_Qx : ", tv[i].in_Qx);
        ecc_prints("in_Qy : ", tv[i].in_Qy);

        ret = TSI_ECC_Multiply(tv[i].curve,     /* curve_id   */
                       0,                       /* type       */
                       0x3,                     /* msel       */
                       0x3,                     /* sps        */
                       0,                       /* m_knum     */
                       0,                       /* x_knum     */
                       0,                       /* y_knum     */
                       ptr_to_u32(param_block), /* param_addr */
                       ptr_to_u32(out_block)    /* dest_addr  */
                       );
        if (ret != 0) {
            sysprintf("ECC point multiply failed!!\n");
            TSI_Print_Error(ret);
            while (1);
        }

        ecc_prints("Output point X: ", out_x);
        ecc_prints("Output point Y: ", out_y);

        sysprintf("ECC done, compare...\n");
        if (ecc_strcmp(tv[i].out_Qx, out_x) != 0) {
            sysprintf("Output point X [%s] is not matched with expected [%s]!\n", out_x, tv[i].out_Qx);
            while (1);
        }
        if (ecc_strcmp(tv[i].out_Qy, out_y) != 0) {
            sysprintf("Output point Y [%s] is not matched with expected [%s]!\n", out_y, tv[i].out_Qy);
            while (1);
        }
        sysprintf("[PASS]\n");
    }
    sysprintf("ECC public key generation test vector compared passed.\n");
    while (1);
}

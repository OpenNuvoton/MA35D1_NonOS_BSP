/**************************************************************************//**
 * @file     main.c
 * @brief    This sample program demonstrates how to use the TSI commands to
 *           read and program OTP contents.
 *
 * @copyright (C) 2025 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"

/* Linux:  Link encap:Ethernet  HWaddr 00:19:38:72:29:43 */
uint8_t gmac0_addr[6] = { 0x00, 0x19, 0x38, 0x72, 0x29, 0x43 };

/* Linux:  Link encap:Ethernet  HWaddr 00:19:39:21:45:47 */
uint8_t gmac1_addr[6] = { 0x00, 0x19, 0x39, 0x21, 0x45, 0x47 };

uint32_t huk0[4] = { 0xad26c383, 0xe71f5028, 0xc0c7655f, 0x397ffaa1 };
uint32_t huk1[4] = { 0x4b8da93e, 0x3d79837b, 0xf39b6f54, 0x1257e380 };
uint32_t huk2[4] = { 0xac63a2a2, 0x4247e449, 0x51e0c76d, 0x42e5b995 };

static uint8_t  tsi_buff_mem[4096] __attribute__((aligned(32)));

int32_t dump_otp_and_huk(void)
{
    uint32_t otp_data;
    uint32_t *data = nc_ptr(tsi_buff_mem);  /* obtain a non-cacheable buffer */
    int i, ret;

    /*
     *  Power-on Setting: 0x100~0x103
     */
    ret = TSI_OTP_Read(0x100, &otp_data);
    if (ret)
        goto err_out;
    sysprintf("Power-on Setting = 0x%x\n", otp_data);

    /*
     *  DPM Setting: 0x104~0x107
     */
    ret = TSI_OTP_Read(0x104, &otp_data);
    if (ret)
        goto err_out;
    sysprintf("DPM Setting = 0x%x\n", otp_data);

    /*
     *  PLM Setting: 0x108~0x10B
     */
    ret = TSI_OTP_Read(0x108, &otp_data);
    if (ret)
        goto err_out;
    sysprintf("PLM Setting = 0x%x\n", otp_data);

    /*
     *  MAC0 Address: 0x10C~0x113
     */
    ret = TSI_OTP_Read(0x10C, &data[0]);
    if (ret)
        goto err_out;
    ret = TSI_OTP_Read(0x110, &data[1]);
    if (ret)
        goto err_out;
    sysprintf("MAC0 address = 0x%08x, 0x%08x\n", data[0], data[1]);

    /*
     *  MAC1 Address: 0x114~0x11B
     */
    ret = TSI_OTP_Read(0x114, &data[0]);
    if (ret)
        goto err_out;
    ret = TSI_OTP_Read(0x118, &data[1]);
    if (ret)
        goto err_out;
    sysprintf("MAC1 address = 0x%08x, 0x%08x\n", data[0], data[1]);

    for (i = 0; i < 3; i++)
    {
        memset(data, 0, 4 * 4);
        ret = TSI_KS_Read(KS_OTP, i, data, 4);
        if (ret)
        {
            sysprintf("Failed to read HUK%d! The HUK%d may have never been programmed.\n", i, i);
        }
        sysprintf("HUK%d = 0x%x 0x%x 0x%x 0x%x\n", i, data[0], data[1], data[2], data[3]);
    }
    return 0;

err_out:
    sysprintf("Failed!\n");
    TSI_Print_Error(ret);
    while (1);
}

int32_t do_program(void)
{
    uint32_t *data = nc_ptr(tsi_buff_mem);  /* obtain a non-cacheable buffer */
    int i, ret;

    sysprintf("Program GMAC0 address...\n");
    /*
     *  Program GMAC0 address
     */
    data[0] = (gmac0_addr[3] << 24) | (gmac0_addr[2] << 16) | (gmac0_addr[1] << 8) | gmac0_addr[0];
    data[1] = (gmac0_addr[5] << 8) | gmac0_addr[4];

    ret = TSI_OTP_Program(0x10C, data[0]);
    if (ret)
        goto err_out;

    ret = TSI_OTP_Program(0x110, data[1]);
    if (ret)
        goto err_out;

    sysprintf("Program GMAC1 address...\n");
    /*
     *  Program GMAC1 address
     */
    data[0] = (gmac1_addr[3] << 24) | (gmac1_addr[2] << 16) | (gmac1_addr[1] << 8) | gmac1_addr[0];
    data[1] = (gmac1_addr[5] << 8) | gmac1_addr[4];

    ret = TSI_OTP_Program(0x114, data[0]);
    if (ret)
        goto err_out;

    ret = TSI_OTP_Program(0x118, data[1]);
    if (ret)
        goto err_out;

    /*
     *  Program HUK0/1/2
     */
    sysprintf("Program HUK0...\n");

    for (i = 0; i < 4; i++)  /* copy key to non-cacheable buffer */
        data[i] = huk0[i];

    ret = TSI_KS_Write_OTP(0, KS_META_CPU | KS_META_READABLE | KS_META_128, data);
    if (ret)
        goto err_ks_out;

    sysprintf("Program HUK1...\n");

    for (i = 0; i < 4; i++)
        data[i] = huk1[i];

    ret = TSI_KS_Write_OTP(1, KS_META_CPU | KS_META_READABLE | KS_META_128, data);
    if (ret)
        goto err_ks_out;

    sysprintf("Program HUK2...\n");

    for (i = 0; i < 4; i++)
        data[i] = huk2[i];

    ret = TSI_KS_Write_OTP(2, KS_META_CPU | KS_META_READABLE | KS_META_128, data);
    if (ret)
        goto err_ks_out;

    return 0;

err_out:
    sysprintf("Failed!\n");
    TSI_Print_Error(ret);
    while (1);

err_ks_out:
    sysprintf("Failed! Note that Key Store HUK0/1/2 cannot be over-programmed.\n");
    TSI_Print_Error(ret);
    while (1);
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

    sysprintf("+--------------------------------------------+\n");
    sysprintf("|  MA35D1 TSI OTP program demo               |\n");
    sysprintf("+--------------------------------------------+\n\n");

    sysprintf("==============================================\n");
    sysprintf("[Before program] Dump MAC address and HUK\n");
    sysprintf("==============================================\n");
    dump_otp_and_huk();

    sysprintf("This program is going to write OTP MAC address and HUK...\n");
    sysprintf("Press 'y' to continue...\n");
    if (sysgetchar() != 'y')
    {
        sysprintf("\nUser aborted.\n");
        while (1);
    }
    sysprintf("\n\n");
    sysprintf("==============================================\n");
    sysprintf("[After program] Dump MAC address and HUK\n");
    sysprintf("==============================================\n");

    do_program();

    dump_otp_and_huk();

    sysprintf("\nDone.\n");
    while (1);

}

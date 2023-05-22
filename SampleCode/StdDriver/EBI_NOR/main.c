/**************************************************************************//**
 * @file     main.c
 * @brief    Configure EBI interface to access MX29LV320T (NOR Flash) on EBI interface.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
extern void NOR_MX29LV320T_RESET(uint32_t u32Bank);
extern int32_t NOR_MX29LV320T_CheckStatus(uint32_t u32DstAddr, uint16_t u16Data, uint32_t u32TimeoutMs);
extern uint16_t NOR_MX29LV320T_READ(uint32_t u32Bank, uint32_t u32DstAddr);
extern int32_t NOR_MX29LV320T_WRITE(uint32_t u32Bank, uint32_t u32DstAddr, uint16_t u16Data);
extern void NOR_MX29LV320T_GET_ID(uint32_t u32Bank, uint16_t *pu16IDTable);
extern int32_t NOR_MX29LV320T_EraseChip(uint32_t u32Bank, uint32_t u32IsCheckBlank);

void Configure_EBI_16BIT_Pins(void)
{
    /* EBI CS0 pin on PG.1 */
    SYS->GPG_MFPL |=SYS_GPG_MFPL_PG1MFP_EBI_nCS0;
    /* EBI RD and WR pins on PG.6 and PG.7 */
    SYS->GPG_MFPL |= SYS_GPG_MFPL_PG6MFP_EBI_nRD | SYS_GPG_MFPL_PG7MFP_EBI_nWR;

    /* EBI ALE pin on PB.9 */
    SYS->GPB_MFPH |= SYS_GPB_MFPH_PB9MFP_EBI_ALE;
    /* EBI MCLK pin on PB.10 */
    SYS->GPB_MFPH |=SYS_GPB_MFPH_PB10MFP_EBI_MCLK;
    /* EBI WRL and WRH pins on PL.11 and PL.10 */
    SYS->GPL_MFPH |=SYS_GPL_MFPH_PL11MFP_EBI_nWRL;
    SYS->GPL_MFPH |=SYS_GPL_MFPH_PL10MFP_EBI_nWRH;

    /* EBI AD0~4 pins on PG.11~15 */
    SYS->GPG_MFPH |= SYS_GPG_MFPH_PG11MFP_EBI_AD0;
    SYS->GPG_MFPH |= SYS_GPG_MFPH_PG12MFP_EBI_AD1;
    SYS->GPG_MFPH |= SYS_GPG_MFPH_PG13MFP_EBI_AD2;
    SYS->GPG_MFPH |= SYS_GPG_MFPH_PG14MFP_EBI_AD3;
    SYS->GPG_MFPH |= SYS_GPG_MFPH_PG15MFP_EBI_AD4;

    /* EBI AD5~8 pins on PL.6~9 */
    SYS->GPL_MFPL |= SYS_GPL_MFPL_PL6MFP_EBI_AD5;
    SYS->GPL_MFPL |= SYS_GPL_MFPL_PL7MFP_EBI_AD6;
    SYS->GPL_MFPH |= SYS_GPL_MFPH_PL8MFP_EBI_AD7;
    SYS->GPL_MFPH |= SYS_GPL_MFPH_PL9MFP_EBI_AD8;

    /* EBI AD9~10 pins on PD.10~11 */
    SYS->GPD_MFPH |= SYS_GPD_MFPH_PD10MFP_EBI_AD9;
    SYS->GPD_MFPH |= SYS_GPD_MFPH_PD11MFP_EBI_AD10;
    /* EBI AD11~14 pin on PL.0~3 */
    SYS->GPL_MFPL |= SYS_GPL_MFPL_PL0MFP_EBI_AD11;
    SYS->GPL_MFPL |= SYS_GPL_MFPL_PL1MFP_EBI_AD12;
    SYS->GPL_MFPL |= SYS_GPL_MFPL_PL2MFP_EBI_AD13;
    SYS->GPL_MFPL |= SYS_GPL_MFPL_PL3MFP_EBI_AD14;
    /* EBI AD15 pin on PG.0 */
    SYS->GPG_MFPL |= SYS_GPG_MFPL_PG0MFP_EBI_AD15;

    /* EBI ADR0~7 pins on PI.0~7 */
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI0MFP_EBI_ADR0;
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI1MFP_EBI_ADR1;
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI2MFP_EBI_ADR2;
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI3MFP_EBI_ADR3;
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI4MFP_EBI_ADR4;
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI5MFP_EBI_ADR5;
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI6MFP_EBI_ADR6;
    SYS->GPI_MFPL |= SYS_GPI_MFPL_PI7MFP_EBI_ADR7;
    /* EBI ADR8~11 pins on PK.0~3 */
    SYS->GPK_MFPL |= SYS_GPK_MFPL_PK0MFP_EBI_ADR8;
    SYS->GPK_MFPL |= SYS_GPK_MFPL_PK1MFP_EBI_ADR9;
    SYS->GPK_MFPL |= SYS_GPK_MFPL_PK2MFP_EBI_ADR10;
    SYS->GPK_MFPL |= SYS_GPK_MFPL_PK3MFP_EBI_ADR11;
    /* EBI ADR12~15 pins on PJ.12~15 */
    SYS->GPJ_MFPH |= SYS_GPJ_MFPH_PJ12MFP_EBI_ADR12;
    SYS->GPJ_MFPH |= SYS_GPJ_MFPH_PJ13MFP_EBI_ADR13;
    SYS->GPJ_MFPH |= SYS_GPJ_MFPH_PJ14MFP_EBI_ADR14;
    SYS->GPJ_MFPH |= SYS_GPJ_MFPH_PJ15MFP_EBI_ADR15;
    /* EBI ADR16~19 pins on PG.2~5 */
    SYS->GPG_MFPL |= SYS_GPG_MFPL_PG2MFP_EBI_ADR16;
    SYS->GPG_MFPL |= SYS_GPG_MFPL_PG3MFP_EBI_ADR17;
    SYS->GPG_MFPL |= SYS_GPG_MFPL_PG4MFP_EBI_ADR18;
    SYS->GPG_MFPL |= SYS_GPG_MFPL_PG5MFP_EBI_ADR19;

}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Enable peripheral clock */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));
    CLK_EnableModuleClock(EBI_MODULE);

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    uint32_t u32Addr, u32MaxEBISize;
    uint16_t u16WData, u16RData;
    uint16_t u16IDTable[2];

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for sysprintf */
    UART_Open(UART0, 115200);

    sysprintf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    sysprintf("+-----------------------------------------+\n");
    sysprintf("|    EBI Nor Flash Sample Code on Bank0   |\n");
    sysprintf("+-----------------------------------------+\n\n");

    sysprintf("************************************************************************\n");
    sysprintf("* Please connect MX29LV320T nor flash to EBI bank1 before accessing !! *\n");
    sysprintf("* EBI pins settings:                                                   *\n");
    sysprintf("*   - AD0  ~  AD4   on PG.11 ~ PG.15                                   *\n");
    sysprintf("*   - AD5  ~  AD8   on PL.6  ~ PL.9                                    *\n");
    sysprintf("*   - AD9  ~  AD10  on PD.10 ~ PD.11                                   *\n");
    sysprintf("*   - AD11  ~ AD14  on PL.0  ~ PL.3                                    *\n");
    sysprintf("*   - AD15          on PG.0                                            *\n");
    sysprintf("*   - ADR0 ~  ADR7   on PI.0 ~ PI.7                                    *\n");
    sysprintf("*   - ADR8  ~ ADR11  on PK.0 ~ PK.3                                    *\n");
    sysprintf("*   - ADR12 ~ ADR15  on PJ.12 ~ PJ.15                                  *\n");
    sysprintf("*   - ADR16 ~ ADR19  on PG.2  ~ PG.5                                   *\n");
    sysprintf("*   - nWR  on PG.7                                                     *\n");
    sysprintf("*   - nRD  on PG.6                                                     *\n");
    sysprintf("*   - nWRL on PL.11                                                    *\n");
    sysprintf("*   - nWRH on PL.10                                                    *\n");
    sysprintf("*   - nCS0 on PG.1                                                     *\n");
    sysprintf("*   - ALE  on PB.9                                                     *\n");
    sysprintf("*   - MCLK on PB.10                                                    *\n");
    sysprintf("************************************************************************\n\n");

    /* Configure multi-function pins for EBI 16-bit application */
    Configure_EBI_16BIT_Pins();

    /* Initialize EBI bank1 to access external nor */
    EBI_Open(EBI_BANK0, EBI_BUSWIDTH_16BIT, EBI_TIMING_SLOWEST, EBI_OPMODE_ADSEPARATE, EBI_CS_ACTIVE_LOW);

    /* Step 1, check ID */
    NOR_MX29LV320T_GET_ID(EBI_BANK0, (uint16_t *)u16IDTable);
    sysprintf(">> Manufacture ID: 0x%X, Device ID: 0x%X .... ", u16IDTable[0], u16IDTable[1]);
    if((u16IDTable[0] != 0xC2) || (u16IDTable[1] != 0x22A8))
    {
        sysprintf("FAIL !!!\n\n");
        while(1);
    }
    else
    {
        sysprintf("PASS !!!\n\n");
    }


    /* Step 2, erase chip */
    if(NOR_MX29LV320T_EraseChip(EBI_BANK0, TRUE) < 0)
        while(1);


    /* Step 3, program flash and compare data */
    sysprintf(">> Run program flash test ......\n");
    u32MaxEBISize = EBI_MAX_SIZE;
    for(u32Addr = 0; u32Addr < u32MaxEBISize; u32Addr += 2)
    {
        u16WData = (0x7657 + u32Addr / 2) & 0xFFFF;
        if(NOR_MX29LV320T_WRITE(EBI_BANK0, u32Addr, u16WData) < 0)
        {
            sysprintf("Program [0x%08X]: [0x%08X] FAIL !!!\n\n", (uint32_t)EBI_BANK0_BASE_ADDR + (0x100000 * EBI_BANK0) + u32Addr, u16WData);
            while(1);
        }
        else
        {
            /* Show UART message ...... */
            if((u32Addr % 256) == 0)
                sysprintf("Program [0x%08X]:[0x%08X] !!!       \r", (uint32_t)EBI_BANK0_BASE_ADDR + (0x100000 * EBI_BANK0) + u32Addr, u16WData);
        }
    }

    for(u32Addr = 0; u32Addr < u32MaxEBISize; u32Addr += 2)
    {
        u16WData = (0x7657 + u32Addr / 2) & 0xFFFF;
        u16RData = NOR_MX29LV320T_READ(EBI_BANK0, u32Addr);
        if(u16WData != u16RData)
        {
            sysprintf("Compare [0x%08X] FAIL !!! (W:0x%08X, R:0x%08X)\n\n", (uint32_t)EBI_BANK0_BASE_ADDR + (0x100000 * EBI_BANK0) + u32Addr, u16WData, u16RData);
            while(1);
        }
        else
        {
            /* Show UART message ...... */
            if((u32Addr % 256) == 0)
                sysprintf("Read [0x%08X]: [0x%08X] !!!         \r", (uint32_t)EBI_BANK0_BASE_ADDR + (0x100000 * EBI_BANK0) + u32Addr, u16RData);
        }
    }
    sysprintf(">> Program flash OK !!!                             \n\n");

    /* Disable EBI function */
    EBI_Close(EBI_BANK0);

    /* Disable EBI clock */
    CLK_DisableModuleClock(EBI_MODULE);

    while(1);
}


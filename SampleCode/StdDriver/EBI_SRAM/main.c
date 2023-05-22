/**************************************************************************//**
 * @file     main.c
 * @brief    Configure EBI interface to access IS61WV204816BLL(SRAM) on EBI interface.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

extern void SRAM_IS61WV204816BLL(uint32_t u32MaxSize);
void AccessEBIWithPDMA(void);

void Configure_EBI_16BIT_Pins(void)
{
    /* EBI CS0 pin on PG.1 */
    SYS->GPG_MFPL |=SYS_GPG_MFPL_PG1MFP_EBI_nCS0;
    /* EBI RD and WR pins on PG.6 and PG.7 */
    SYS->GPG_MFPL |=SYS_GPG_MFPL_PG6MFP_EBI_nRD;
    SYS->GPG_MFPL |=SYS_GPG_MFPL_PG7MFP_EBI_nWR;
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
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(EBI_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for sysprintf */
    UART_Open(UART0, 115200);

    sysprintf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    sysprintf("+--------------------------------------------------------+\n");
    sysprintf("|    EBI SRAM Sample Code on Bank0 with PDMA transfer    |\n");
    sysprintf("+--------------------------------------------------------+\n\n");

    sysprintf("********************************************************************\n");
    sysprintf("* Please connect IS61WV204816BLL to EBI bank0 before accessing !!  *\n");
    sysprintf("* EBI pins settings:                                               *\n");
    sysprintf("*   - AD0  ~  AD4   on PG.11 ~ PG.15                               *\n");
    sysprintf("*   - AD5  ~  AD8   on PL.6  ~ PL.9                                *\n");
    sysprintf("*   - AD9  ~  AD10  on PD.10 ~ PD.11                               *\n");
    sysprintf("*   - AD11  ~ AD14  on PL.0  ~ PL.3                                *\n");
    sysprintf("*   - AD15          on PG.0                                        *\n");
    sysprintf("*   - ADR16 ~ ADR19 on PG.2  ~ PG.5                                *\n");
    sysprintf("*   - nWR  on PG.7                                                 *\n");
    sysprintf("*   - nRD  on PG.6                                                 *\n");
    sysprintf("*   - nWRL on PL.11                                                *\n");
    sysprintf("*   - nWRH on PL.10                                                *\n");
    sysprintf("*   - nCS0 on PG.1                                                 *\n");
    sysprintf("*   - ALE  on PB.9                                                 *\n");
    sysprintf("*   - MCLK on PB.10                                                *\n");
    sysprintf("********************************************************************\n\n");

    /* Configure multi-function pins for EBI 16-bit application */
    Configure_EBI_16BIT_Pins();

    /* Initialize EBI bank0 to access external SRAM */
    EBI_Open(EBI_BANK0, EBI_BUSWIDTH_16BIT, EBI_TIMING_SLOWEST, 0, EBI_CS_ACTIVE_LOW);

    /* Start to test EBI SRAM */
    SRAM_IS61WV204816BLL( 512 * 1024);

    /* EBI SRAM with PDMA test */
    AccessEBIWithPDMA();

    /* Disable EBI function */
    EBI_Close(EBI_BANK0);

    /* Disable EBI clock */
    CLK_DisableModuleClock(EBI_MODULE);

    sysprintf("*** SRAM Test OK ***\n");

    while(1);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Global variables for PDMA                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t PDMA_TEST_LENGTH = 64;
uint32_t SrcArray[64];
uint32_t DestArray[64];
uint32_t volatile u32IsTestOver = 0;

/**
 * @brief       DMA IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The DMA default IRQ
 */
void PDMA2_IRQHandler(void)
{
    uint32_t status = PDMA_GET_INT_STATUS(PDMA2);

    if(status & PDMA_INTSTS_ABTIF_Msk)    /* abort */
    {
        if(PDMA_GET_ABORT_STS(PDMA2) & PDMA_ABTSTS_ABTIF2_Msk)
            u32IsTestOver = 2;
        PDMA_CLR_ABORT_FLAG(PDMA2, PDMA_ABTSTS_ABTIF2_Msk);
    }
    else if(status & PDMA_INTSTS_TDIF_Msk)      /* done */
    {
        if(PDMA_GET_TD_STS(PDMA2) & PDMA_TDSTS_TDIF2_Msk)
            u32IsTestOver = 1;
        PDMA_CLR_TD_FLAG(PDMA2, PDMA_TDSTS_TDIF2_Msk);
    }
    else
        sysprintf("unknown interrupt 0x%08x!!\n",status);
}

void AccessEBIWithPDMA(void)
{
    uint32_t i;
    uint32_t u32Result0 = 0x5A5A, u32Result1 = 0x5A5A;

    sysprintf("[[ Access EBI with PDMA ]]\n");

    /* Enable PDMA clock source */
    CLK_EnableModuleClock(PDMA2_MODULE);

    for(i=0; i<64; i++)
    {
        SrcArray[i] = 0x76570000 + i;
        u32Result0 += SrcArray[i];
    }

    /* Open Channel 2 */
    PDMA_Open(PDMA2, (1<<2));

    //burst size is 4
    PDMA_SetBurstType(PDMA2, 2, PDMA_REQ_BURST, PDMA_BURST_4);

    /* transfer width is one word(32 bit) */
    PDMA_SetTransferCnt(PDMA2, 2, PDMA_WIDTH_32, PDMA_TEST_LENGTH);
    PDMA_SetTransferAddr(PDMA2, 2, ptr_to_u32(SrcArray), PDMA_SAR_INC, EBI_BANK0_BASE_ADDR, PDMA_DAR_INC);
    PDMA_SetTransferMode(PDMA2, 2, PDMA_MEM, FALSE, 0);

    PDMA_EnableInt(PDMA2, 2, PDMA_INT_TRANS_DONE);
    /* Enable GIC for PDMA */
    IRQ_SetHandler((IRQn_ID_t)PDMA2_IRQn, PDMA2_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)PDMA2_IRQn);

    u32IsTestOver = 0;
    PDMA_Trigger(PDMA2, 2);
    while(u32IsTestOver == 0);
    /* Transfer internal SRAM to EBI SRAM done */

    /* Clear internal SRAM data */
    for(i=0; i<64; i++)
    {
        SrcArray[i] = 0x0;
    }

    /* transfer width is one word(32 bit) */
    PDMA_SetTransferCnt(PDMA2, 2, PDMA_WIDTH_32, PDMA_TEST_LENGTH);
    PDMA_SetTransferAddr(PDMA2, 2, EBI_BANK0_BASE_ADDR, PDMA_SAR_INC, ptr_to_u32(SrcArray), PDMA_DAR_INC);
    PDMA_SetTransferMode(PDMA2, 2, PDMA_MEM, FALSE, 0);

    u32IsTestOver = 0;
    PDMA_Trigger(PDMA2, 2);
    while(u32IsTestOver == 0);
    /* Transfer EBI SRAM to internal SRAM done */
    for(i=0; i<64; i++)
    {
        u32Result1 += SrcArray[i];
    }

    if(u32IsTestOver == 1)
    {
        if((u32Result0 == u32Result1) && (u32Result0 != 0x5A5A))
        {
            sysprintf("        PASS (0x%X)\n\n", u32Result0);
        }
        else
        {
            sysprintf("        FAIL - data matched (0x%X)\n\n", u32Result0);
            while(1);
        }
    }
    else
    {
        sysprintf("        PDMA fail\n\n");
        while(1);
    }

    PDMA_Close(PDMA2);
}


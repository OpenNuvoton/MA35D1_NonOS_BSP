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
    /* EBI AD0 ~ AD15 pins */
    SYS->GPK_MFPH |= SYS_GPK_MFPH_PK9MFP_EBI_AD0;
    SYS->GPK_MFPH |= SYS_GPK_MFPH_PK10MFP_EBI_AD1;
    SYS->GPK_MFPH |= SYS_GPK_MFPH_PK11MFP_EBI_AD2;
    SYS->GPM_MFPL |= SYS_GPM_MFPL_PM0MFP_EBI_AD3;
    SYS->GPM_MFPL |= SYS_GPM_MFPL_PM1MFP_EBI_AD4;
    SYS->GPM_MFPL |= SYS_GPM_MFPL_PM2MFP_EBI_AD5;
    SYS->GPL_MFPL |= SYS_GPL_MFPL_PL7MFP_EBI_AD6;
    SYS->GPL_MFPH |= SYS_GPL_MFPH_PL8MFP_EBI_AD7;
    SYS->GPL_MFPH |= SYS_GPL_MFPH_PL9MFP_EBI_AD8;
    SYS->GPA_MFPH |= SYS_GPA_MFPH_PA9MFP_EBI_AD9;
    SYS->GPA_MFPH |= SYS_GPA_MFPH_PA10MFP_EBI_AD10;
    SYS->GPA_MFPH |= SYS_GPA_MFPH_PA11MFP_EBI_AD11;
    SYS->GPA_MFPH |= SYS_GPA_MFPH_PA12MFP_EBI_AD12;
    SYS->GPA_MFPH |= SYS_GPA_MFPH_PA13MFP_EBI_AD13;
    SYS->GPL_MFPH |= SYS_GPL_MFPH_PL15MFP_EBI_AD14;
    SYS->GPK_MFPH |= SYS_GPK_MFPH_PK8MFP_EBI_AD15;

    /* EBI ADDR16 ~ ADDR19 pins */
    SYS->GPB_MFPH |= SYS_GPB_MFPH_PB12MFP_EBI_ADR16;
    SYS->GPB_MFPH |= SYS_GPB_MFPH_PB13MFP_EBI_ADR17;
    SYS->GPB_MFPH |= SYS_GPB_MFPH_PB14MFP_EBI_ADR18;
    SYS->GPB_MFPH |= SYS_GPB_MFPH_PB15MFP_EBI_ADR19;

    /* EBI ALE pin on PB.11 */
    SYS->GPB_MFPH |= SYS_GPB_MFPH_PB11MFP_EBI_ALE;
    /* EBI nCS0 pin on PJ.0 */
    SYS->GPJ_MFPL |= SYS_GPJ_MFPL_PJ0MFP_EBI_nCS0;

    /* EBI nWR and nRD pins on PL.5 and PL.4 */
    SYS->GPL_MFPL |= SYS_GPL_MFPL_PL4MFP_EBI_nRD;
    SYS->GPL_MFPL |= SYS_GPL_MFPL_PL5MFP_EBI_nWR;

    /* EBI WRL and WRH pins on PL.11 and PL.10 */
    SYS->GPL_MFPH |= SYS_GPL_MFPH_PL11MFP_EBI_nWRL | SYS_GPL_MFPH_PL10MFP_EBI_nWRH;
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
    sysprintf("*   - AD0  ~  AD2   on PK.9  ~ PK.11                               *\n");
    sysprintf("*   - AD3  ~  AD5   on PM.0  ~ PM.2                                *\n");
    sysprintf("*   - AD6  ~  AD8   on PL.7  ~ PL.9                                *\n");
    sysprintf("*   - AD9   ~ AD13  on PA.9  ~ PA.13                               *\n");
    sysprintf("*   - AD14          on PL.15                                       *\n");
    sysprintf("*   - AD15          on PK.8                                        *\n");
    sysprintf("*   - ADR16 ~ ADR19 on PB.12 ~ PB.15                               *\n");
    sysprintf("*   - nWR  on PL.5                                                 *\n");
    sysprintf("*   - nRD  on PL.4                                                 *\n");
    sysprintf("*   - nWRL on PL.11                                                *\n");
    sysprintf("*   - nWRH on PL.10                                                *\n");
    sysprintf("*   - nCS0 on PJ.0                                                 *\n");
    sysprintf("*   - ALE  on PB.11                                                *\n");
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
#ifdef __ICCARM__
#pragma data_alignment=4
uint8_t au8SrcArray[256];
uint8_t au8DestArray[256];
#else
__attribute__((aligned(4))) uint8_t au8SrcArray[256];
__attribute__((aligned(4))) uint8_t au8DestArray[256];
#endif

uint32_t volatile g_u32IsTestOver = 0;

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

    if(status & PDMA_INTSTS_ABTIF_Msk) /* abort */
    {
        if(PDMA_GET_ABORT_STS(PDMA2) & PDMA_ABTSTS_ABTIF2_Msk)
            g_u32IsTestOver = 2;
        PDMA_CLR_ABORT_FLAG(PDMA2, PDMA_ABTSTS_ABTIF2_Msk);
    }
    else if(status & PDMA_INTSTS_TDIF_Msk) /* done */
    {
        if(PDMA_GET_TD_STS(PDMA2) & PDMA_TDSTS_TDIF2_Msk)
            g_u32IsTestOver = 1;
        PDMA_CLR_TD_FLAG(PDMA2, PDMA_TDSTS_TDIF2_Msk);
    }
    else
        sysprintf("unknown interrupt 0x%08x!!\n",status);
}

void AccessEBIWithPDMA(void)
{
    uint32_t i;
    uint32_t u32Result0 = 0x5A5A, u32Result1 = 0x5A5A;
    uint8_t *ps=(uint8_t *)nc_addr64(nc_addr64(au8SrcArray));
    uint32_t u32TimeOutCnt = 0;

    sysprintf("[[ Access EBI with PDMA ]]\n");

    /* Enable PDMA clock source */
    CLK_EnableModuleClock(PDMA2_MODULE);

    for(i=0; i<256; i++)
    {
        ps[i] = 0x76570000 + i;
        u32Result0 += ps[i];
    }

    /* Open Channel 2 */
    PDMA_Open(PDMA2,1 << 2);
    /* Transfer count is PDMA_TEST_LENGTH, transfer width is 32 bits(one word) */
    PDMA_SetTransferCnt(PDMA2,2, PDMA_WIDTH_32, PDMA_TEST_LENGTH);
    /* Set source address is au8SrcArray, destination address is au8DestArray, Source/Destination increment size is 32 bits(one word) */
    PDMA_SetTransferAddr(PDMA2,2, ptr_to_u32(au8SrcArray), PDMA_SAR_INC, ptr_to_u32(EBI_BANK0_BASE_ADDR), PDMA_DAR_INC);
    /* Request source is memory to memory */
    PDMA_SetTransferMode(PDMA2,2, PDMA_MEM, FALSE, 0);
    /* Transfer type is burst transfer and burst size is 4 */
    PDMA_SetBurstType(PDMA2,2, PDMA_REQ_BURST, PDMA_BURST_4);

    /* Enable interrupt */
    PDMA_EnableInt(PDMA2,2, PDMA_INT_TRANS_DONE);

    /* Enable GIC for PDMA */
    IRQ_SetHandler((IRQn_ID_t)PDMA2_IRQn, PDMA2_IRQHandler);

    /* Enable IRQ */
    IRQ_Enable ((IRQn_ID_t)PDMA2_IRQn);

    g_u32IsTestOver = 0;

    /* Generate a software request to trigger transfer with PDMA channel 2  */
    PDMA_Trigger(PDMA2,2);

    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
    while(g_u32IsTestOver == 0)
    {
        if(--u32TimeOutCnt == 0)
        {
            sysprintf("Wait for PDMA time-out!\n");
            while(1);
        }
    }
    /* Transfer internal SRAM to EBI SRAM done */

    /* Clear internal SRAM data */
    for(i = 0; i < 256; i++)
    {
        ps[i] = 0x0;
    }

    /* transfer width is one word(32 bit) */
    /* Transfer count is PDMA_TEST_LENGTH, transfer width is 32 bits(one word) */
    PDMA_SetTransferCnt(PDMA2,2, PDMA_WIDTH_32, PDMA_TEST_LENGTH);
    /* Set source address is au8SrcArray, destination address is au8DestArray, Source/Destination increment size is 32 bits(one word) */
    PDMA_SetTransferAddr(PDMA2, 2, ptr_to_u32(EBI_BANK0_BASE_ADDR), PDMA_SAR_INC, ptr_to_u32(au8SrcArray), PDMA_DAR_INC);
    /* Request source is memory to memory */
    PDMA_SetTransferMode(PDMA2,2, PDMA_MEM, FALSE, 0);

    g_u32IsTestOver = 0;

    /* Generate a software request to trigger transfer with PDMA channel 2  */
    PDMA_Trigger(PDMA2,2);

    u32TimeOutCnt = SystemCoreClock; /* 1 second time-out */
    while(g_u32IsTestOver == 0)
    {
        if(--u32TimeOutCnt == 0)
        {
            sysprintf("Wait for PDMA time-out!\n");
            while(1);
        }
    }

    /* Transfer EBI SRAM to internal SRAM done */
    for(i = 0; i < 256; i++)
    {
        u32Result1 += ps[i];
    }

    if(g_u32IsTestOver == 1)
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


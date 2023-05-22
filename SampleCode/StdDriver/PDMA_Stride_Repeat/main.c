/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Use PDMA2 channel 0 to transfer data from memory to memory with stride and repeat.
 *
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t PDMA_TEST_LENGTH = 16;
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
 * @details     The DMA default IRQ.
 */
void PDMA2_IRQHandler(void)
{
    volatile uint32_t status = PDMA_GET_INT_STATUS(PDMA2);

    if(status & PDMA_INTSTS_ABTIF_Msk)    /* abort */
    {
        /* Check if channel 0 has abort error */
        if(PDMA_GET_ABORT_STS(PDMA2) & PDMA_ABTSTS_ABTIF0_Msk)
            g_u32IsTestOver = 2;
        /* Clear abort flag of channel 0 */
        PDMA_CLR_ABORT_FLAG(PDMA2,PDMA_ABTSTS_ABTIF0_Msk);
    }
    else if(status & PDMA_INTSTS_TDIF_Msk)      /* done */
    {
        /* Check transmission of channel 0 has been transfer done */
        if(PDMA_GET_TD_STS(PDMA2) & PDMA_TDSTS_TDIF0_Msk)
            g_u32IsTestOver = 1;
        /* Clear transfer done flag of channel 0 */
        PDMA_CLR_TD_FLAG(PDMA2,PDMA_TDSTS_TDIF0_Msk);
    }
    else
        sysprintf("unknown interrupt !!\n");
}

void SYS_Init(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable IP clock */
    CLK_EnableModuleClock(PDMA2_MODULE);
    CLK_EnableModuleClock(PDMA3_MODULE);
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART clock source from HXT */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Set multi-function pins for UART */
    SYS->GPE_MFPL &= ~(SYS_GPE_MFPH_PE15MFP_Msk | SYS_GPE_MFPH_PE14MFP_Msk);
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE15MFP_UART0_RXD | SYS_GPE_MFPH_PE14MFP_UART0_TXD);
    /* Lock protected registers */
    SYS_LockReg();
}

void UART0_Init()
{
    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);
}

int main(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    /* If user want to write protected register, please issue SYS_UnlockReg() to unlock protected register. */
    SYS_LockReg();

    /* Init UART for sysprintf */
    UART0_Init();

    sysprintf("\n\nCPU @ %dHz\n", SystemCoreClock);
    sysprintf("+------------------------------------------------------------------+ \n");
    sysprintf("|    PDMA2 Memory to Memory Driver Sample Code (Stride_Repeat)      | \n");
    sysprintf("+------------------------------------------------------------------+ \n");


    /*------------------------------------------------------------------------------------------------------

                         au8SrcArray                         au8DestArray
                         ---------------------------   -->   ---------------------------
                       /| [0]  | [1]  |  [2] |  [3] |       | [0]  | [1]  |  [2] |  [3] |\
                        | [4]  | [5]  |  [6] |  [7] |       | [8]  | [9]  | [10] | [11] |
                        | [8]  | [9]  | [10] | [11] |       |      |      |      |      |
                        |      |      |      |      |       |      |      |      |      |
                        |      |      |      |      |       |            ...            |
                        |      |      |      |      |       |      |      |      |      |
                        |      |      |      |      |       |      |      |      |      |
       PDMA_TEST_LENGTH |            ...            |       | [51] | [50] | [49] | [48] | PDMA_TEST_LENGTH
                        |      |      |      |      |       | [59] | [58] | [57] | [56] |
                        |      |      |      |      |       |  [4] |  [5] |  [6] |  [7] |
                        |      |      |      |      |       | [12] | [13] | [14] | [15] |
                        |      |      |      |      |       |      |      |      |      |
                        | [52] | [53] | [54] | [55] |       |            ...            |
                        | [56] | [57] | [58] | [59] |       |      |      |      |      |/
                       \| [60] | [61] | [62] | [63] |       | [60] | [61] | [62] | [63] |
                         ---------------------------         ---------------------------
                         \                         /         \                         /
                               32bits(one word)                     32bits(one word)

      PDMA2 transfer configuration:

        Channel = 0
        Operation mode = basic mode
        Request source = PDMA_MEM(memory to memory)
        transfer done and table empty interrupt = enable

        Transfer count = PDMA_TEST_LENGTH
        Transfer width = 32 bits(one word)
        Source address = au8SrcArray
        Source address increment size = 32 bits(one word)
        Destination address = au8DestArray
        Destination address increment size = 32 bits(one word)
        Transfer type = burst transfer

        Stride Setting:
        Destination stride count = 1
        Source stride count = 2
        Transfer count of per stride = 1

        Repeat Setting:
        Destination address interval count = PDMA_TEST_LENGTH/2
        Source address interval count = 1
        Repeat count = 1

        Total transfer length = PDMA_TEST_LENGTH * 32 bits
    ------------------------------------------------------------------------------------------------------*/
    /* Open Channel 0 */
    PDMA_Open(PDMA2,1 << 0);
    /* Transfer count is PDMA_TEST_LENGTH, transfer width is 32 bits(one word) */
    PDMA_SetTransferCnt(PDMA2,0, PDMA_WIDTH_32, PDMA_TEST_LENGTH);
    /* Set source address is au8SrcArray, destination address is au8DestArray, Source/Destination increment size is 32 bits(one word) */
    PDMA_SetTransferAddr(PDMA2,0, ptr_to_u32(au8SrcArray), PDMA_SAR_INC, ptr_to_u32(au8DestArray), PDMA_DAR_INC);
    /* Destination stride count is 1, Source stride count is 2, Transfer count of per stride is 1 */
    PDMA_SetStride(PDMA2,0, 1, 2, 1);
    /* Destination address interval count is 1, Source address interval count is 2, Repeat count is 1 */
    PDMA_SetRepeat(PDMA2,0, PDMA_TEST_LENGTH/2, 1, 1);
    /* Request source is memory to memory */
    PDMA_SetTransferMode(PDMA2,0, PDMA_MEM, FALSE, 0);
    /* Transfer type is burst transfer and burst size is 1 */
    PDMA_SetBurstType(PDMA2,0, PDMA_REQ_BURST, PDMA_BURST_1);

    /* Enable interrupt */
    PDMA_EnableInt(PDMA2,0, PDMA_INT_TRANS_DONE);

    /* Enable GIC for PDMA2 */
    IRQ_SetHandler((IRQn_ID_t)PDMA2_IRQn, PDMA2_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)PDMA2_IRQn);
    g_u32IsTestOver = 0;

    /* Generate a software request to trigger transfer with PDMA channel 2  */
    PDMA_Trigger(PDMA2,0);

    /* Waiting for transfer done */
    while(g_u32IsTestOver == 0);

    /* Check transfer result */
    if(g_u32IsTestOver == 1)
        sysprintf("test done...\n");
    else if(g_u32IsTestOver == 2)
        sysprintf("target abort...\n");

    /* Close channel 2 */
    PDMA_Close(PDMA2);

    while(1);
}

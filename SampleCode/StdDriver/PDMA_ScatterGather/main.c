/**************************************************************************//**
 * @file     main.c
 * @brief
 *           Use PDMA2 channel 5 to transfer data from memory to memory by scatter-gather mode.
 *
 *
 * @copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

uint32_t PDMA_TEST_LENGTH = 64;
#ifdef __ICCARM__
#pragma data_alignment=4
uint8_t au8SrcArray[256];
uint8_t au8DestArray0[256];
uint8_t au8DestArray1[256];
#else
__attribute__((aligned(4))) uint8_t au8SrcArray[256];
__attribute__((aligned(4))) uint8_t au8DestArray0[256];
__attribute__((aligned(4))) uint8_t au8DestArray1[256];
#endif

typedef struct dma_desc_t
{
    uint32_t ctl;
    uint32_t src;
    uint32_t dest;
    uint32_t offset;
} DMA_DESC_T;

DMA_DESC_T DMA_DESC[2];

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
	int i;
    uint32_t u32Src, u32Dst0, u32Dst1;
    uint8_t *pau8SrcArray;
    uint8_t *pau8DestArray0;
    uint8_t *pau8DestArray1;
    DMA_DESC_T *pDMA_DESC[2];

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
    sysprintf("+-----------------------------------------------------------------------+ \n");
    sysprintf("|         PDMA Memory to Memory Driver Sample Code (Scatter-gather)     | \n");
    sysprintf("+-----------------------------------------------------------------------+ \n");

    pau8SrcArray = nc_ptr(nc_addr64(au8SrcArray));
    pau8DestArray0 = nc_ptr(nc_addr64(au8DestArray0));
    pau8DestArray1 = nc_ptr(nc_addr64(au8DestArray1));
	for (i = 0; i < 256; i++)
		pau8SrcArray[i] = i;

    pDMA_DESC[0] = (DMA_DESC_T *)(nc_ptr(nc_addr64(&DMA_DESC[0])));
    pDMA_DESC[1] = (DMA_DESC_T *)(nc_ptr(nc_addr64(&DMA_DESC[1])));
    u32Src = ptr_to_u32(au8SrcArray);
    u32Dst0 = ptr_to_u32(au8DestArray0);
    u32Dst1 = ptr_to_u32(au8DestArray1);

    /* This sample will transfer data by finished two descriptor table in sequence.(descriptor table 1 -> descriptor table 2) */

    /*----------------------------------------------------------------------------------
      PDMA2 transfer configuration:

        Channel = 4
        Operation mode = scatter-gather mode
        First scatter-gather descriptor table = DMA_DESC[0]
        Request source = PDMA_MEM(memory to memory)

        Transmission flow:
           ------------------------      -----------------------
          |                        |    |                       |
          |  DMA_DESC[0]           | -> |  DMA_DESC[1]          | -> transfer done
          |  (Descriptor table 1)  |    |  (Descriptor table 2) |
          |                        |    |                       |
           ------------------------      -----------------------

    ----------------------------------------------------------------------------------*/

    /* Open Channel 4 */
    PDMA_Open(PDMA2,1 << 4);

    /* Enable Scatter Gather mode, assign the first scatter-gather descriptor table is table 1,
       and set transfer mode as memory to memory */
    PDMA_SetTransferMode(PDMA2, 4, PDMA_MEM, 1, ptr_to_u32(&DMA_DESC[0]));
    /*------------------------------------------------------------------------------------------------------

                         au8SrcArray                         au8DestArray0
                         ---------------------------   -->   ---------------------------
                       /| [0]  | [1]  |  [2] |  [3] |       | [0]  | [1]  |  [2] |  [3] |\
                        |      |      |      |      |       |      |      |      |      |
       PDMA_TEST_LENGTH |            ...            |       |            ...            | PDMA_TEST_LENGTH
                        |      |      |      |      |       |      |      |      |      |
                       \| [60] | [61] | [62] | [63] |       | [60] | [61] | [62] | [63] |/
                         ---------------------------         ---------------------------
                         \                         /         \                         /
                               32bits(one word)                     32bits(one word)

      Descriptor table 1 configuration:

        Operation mode = scatter-gather mode
        Next descriptor table = DMA_DESC[1](Descriptor table 2)
        transfer done and table empty interrupt = disable

        Transfer count = PDMA_TEST_LENGTH
        Transfer width = 32 bits(one word)
        Source address = au8SrcArray
        Source address increment size = 32 bits(one word)
        Destination address = au8DestArray0
        Destination address increment size = 32 bits(one word)
        Transfer type = burst transfer

        Total transfer length = PDMA_TEST_LENGTH * 32 bits
    ------------------------------------------------------------------------------------------------------*/
    pDMA_DESC[0]->ctl =
        ((PDMA_TEST_LENGTH - 1) << PDMA_DSCT_CTL_TXCNT_Pos) | /* Transfer count is PDMA_TEST_LENGTH */ \
        PDMA_WIDTH_32 |   /* Transfer width is 32 bits(one word) */ \
        PDMA_SAR_INC |    /* Source increment size is 32 bits(one word) */ \
        PDMA_DAR_INC |    /* Destination increment size is 32 bits(one word) */ \
        PDMA_REQ_BURST |  /* Transfer type is burst transfer type */ \
        PDMA_BURST_128 |  /* Burst size is 128. No effect in single transfer type */ \
        PDMA_TBINTDIS_DISABLE |   /* Disable transfer done and table empty interrupt */ \
        PDMA_OP_SCATTER;  /* Operation mode is scatter-gather mode */

    /* Configure source address */
    pDMA_DESC[0]->src = u32Src;
    /* Configure destination address */
    pDMA_DESC[0]->dest = u32Dst0;
    /* Configure next descriptor table address */
    pDMA_DESC[0]->offset = ptr_to_u32(&DMA_DESC[1]); /* next descriptor table is table 2 */


    /*------------------------------------------------------------------------------------------------------

                         au8DestArray0                       au8DestArray1
                         ---------------------------   -->   ---------------------------
                       /| [0]  | [1]  |  [2] |  [3] |       | [0]  | [1]  |  [2] |  [3] |\
                        |      |      |      |      |       |      |      |      |      |
       PDMA_TEST_LENGTH |            ...            |       |            ...            | PDMA_TEST_LENGTH
                        |      |      |      |      |       |      |      |      |      |
                       \| [60] | [61] | [62] | [63] |       | [60] | [61] | [62] | [63] |/
                         ---------------------------         ---------------------------
                         \                         /         \                         /
                               32bits(one word)                     32bits(one word)

      Descriptor table 2 configuration:

        Operation mode = basic mode
        transfer done and table empty interrupt = enable

        Transfer count = PDMA_TEST_LENGTH
        Transfer width = 32 bits(one word)
        Source address = au8DestArray0
        Source address increment size = 32 bits(one word)
        Destination address = au8DestArray1
        Destination address increment size = 32 bits(one word)
        Transfer type = burst transfer

        Total transfer length = PDMA_TEST_LENGTH * 32 bits
    ------------------------------------------------------------------------------------------------------*/
    pDMA_DESC[1]->ctl =
        ((PDMA_TEST_LENGTH - 1) << PDMA_DSCT_CTL_TXCNT_Pos) | /* Transfer count is PDMA_TEST_LENGTH */ \
        PDMA_WIDTH_32 |   /* Transfer width is 32 bits(one word) */ \
        PDMA_SAR_INC |    /* Source increment size is 32 bits(one word) */ \
        PDMA_DAR_INC |    /* Destination increment size is 32 bits(one word) */ \
        PDMA_REQ_BURST |  /* Transfer type is burst transfer type */ \
        PDMA_BURST_128 |  /* Burst size is 128. No effect in single transfer type */ \
        PDMA_OP_BASIC;    /* Operation mode is basic mode */

    pDMA_DESC[1]->src = u32Dst0;
    pDMA_DESC[1]->dest = u32Dst1;
    pDMA_DESC[1]->offset = 0; /* No next operation table. No effect in basic mode */

    /* Generate a software request to trigger transfer with PDMA2 channel 4 */
    PDMA_Trigger(PDMA2,4);

    /* Waiting for transfer done */
    //while((PDMA_GET_TD_STS(PDMA2)&(1<<4))!=(1<<4));

    /* Waiting for transfer done */
    while(PDMA_IS_CH_BUSY(PDMA2,4));

    sysprintf("test done...\n");

    /* Close Channel 4 */
    PDMA_Close(PDMA2);

    for (i=0; i < 256; i++)
		if(pau8SrcArray[i]!=pau8DestArray1[i])
			sysprintf("data compare failed\n");
    sysprintf("Data Compare Passed\n");
    while(1);
}


/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate EPWM accumulator interrupt trigger PDMA.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint16_t g_u32Period[2] = {31999,15999};
volatile uint32_t g_u32IsTestOver = 0;


/**
 * @brief       EPWM1 IRQ Handler
 *
 * @param       None
 *
 * @return      None
 *
 * @details     ISR to handle EPWM1 interrupt event
 */
void EPWM1P1_IRQHandler(void)
{

}

/**
 * @brief       PDMA IRQ Handler
 *
 * @param       None
 *
 * @return      None
 *
 * @details     ISR to handle PDMA interrupt event
 */
void PDMA2_IRQHandler(void)
{
    uint32_t status = PDMA_GET_INT_STATUS(PDMA2);

    if(status & PDMA_INTSTS_ABTIF_Msk)    /* abort */
    {
        if(PDMA_GET_ABORT_STS(PDMA2) & PDMA_ABTSTS_ABTIF0_Msk)
            g_u32IsTestOver = 2;
        PDMA_CLR_ABORT_FLAG(PDMA2,PDMA_ABTSTS_ABTIF0_Msk);
    }
    else if(status & PDMA_INTSTS_TDIF_Msk)      /* done */
    {
        if(PDMA_GET_TD_STS(PDMA2) & PDMA_TDSTS_TDIF0_Msk)
            g_u32IsTestOver = 1;
        PDMA_CLR_TD_FLAG(PDMA2,PDMA_TDSTS_TDIF0_Msk);
    }
    else
        sysprintf("unknown interrupt !!\n");
}


void SYS_Init(void)
{

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

    /* Enable IP module clock */
    CLK_EnableModuleClock(EPWM1_MODULE);

    /* Enable PDMA module clock */
    CLK_EnableModuleClock(PDMA2_MODULE);

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Set PK.8 multi-function pins for EPWM1 Channel 0 */
    SYS->GPK_MFPH &= ~SYS_GPK_MFPH_PK8MFP_Msk;
    SYS->GPK_MFPH |= SYS_GPK_MFPH_PK8MFP_EPWM1_CH0;
}

void UART0_Init()
{
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

int32_t main(void)
{
    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART to 115200-8n1 for print message */
    UART0_Init();

    sysprintf("+------------------------------------------------------------------------+\n");
    sysprintf("|                          EPWM Driver Sample Code                        |\n");
    sysprintf("|                                                                        |\n");
    sysprintf("+------------------------------------------------------------------------+\n");
    sysprintf("  This sample code demonstrate EPWM1 channel 0 accumulator interrupt trigger PDMA.\n");
    sysprintf("  When accumulator interrupt happens, EPWM1 channel 0 period register will be updated \n");
    sysprintf("  to g_u32Count array content, 31999(0x7CFF), by PDMA.\n");

    sysprintf("\n\nPress any key to start EPWM1 channel 0.\n");
    sysgetchar();

    /*--------------------------------------------------------------------------------------*/
    /* Set the EPWM1 Channel 0 as EPWM output function.                                       */
    /*--------------------------------------------------------------------------------------*/

    /* Assume EPWM output frequency is 250Hz and duty ratio is 30%, user can calculate EPWM settings by follows.
       duty ratio = (CMR+1)/(CNR+1)
       cycle time = CNR+1
       High level = CMR+1
       EPWM clock source frequency = PCLK = 180000000
       (CNR+1) = EPWM clock source frequency/prescaler/EPWM output frequency
               = 180000000/10/300 = 60000
       (Note: CNR is 16 bits, so if calculated value is larger than 65536, user should increase prescale value.)
       CNR = 59999
       duty ratio = 30% ==> (CMR+1)/(CNR+1) = 30%
       CMR = 19999
       Prescale value is 9 : prescaler= 10
    */

    /* Set EPWM1 channel 0 output configuration */
    EPWM_ConfigOutputChannel(EPWM1, 0, 300, 30);

    /* Enable EPWM Output path for EPWM1 channel 0 */
    EPWM_EnableOutput(EPWM1, EPWM_CH_0_MASK);

    /* Enable EPWM1 channel 0 accumulator, interrupt count 50, accumulator source select to zero point */
    EPWM_EnableAcc(EPWM1, 0, 50, EPWM_IFA_ZERO_POINT);

    /* Enable EPWM1 channel 0 accumulator interrupt */
    EPWM_EnableAccInt(EPWM1, 0);

    /* Enable EPWM1 channel 0 accumulator interrupt trigger PDMA */
    EPWM_EnableAccPDMA(EPWM1, 0);

    /* Enable Timer for EPWM1 channel 0 */
    EPWM_Start(EPWM1, EPWM_CH_0_MASK);

    /*--------------------------------------------------------------------------------------*/
    /* Configure PDMA peripheral mode form memory to EPWM                                    */
    /*--------------------------------------------------------------------------------------*/
    /* Open Channel 0 */
    PDMA_Open(PDMA2,BIT0);

    /* Transfer width is half word(16 bit) and transfer count is 1 */
    PDMA_SetTransferCnt(PDMA2,0, PDMA_WIDTH_16, 1);

    /* Set source address as g_u32Count array(increment) and destination address as EPWM1 channel 0 period register(no increment) */
    PDMA_SetTransferAddr(PDMA2,0, ptr_to_u32(&g_u32Period[0]), PDMA_SAR_INC, ptr_to_u32(&(EPWM1->PERIOD[0])), PDMA_DAR_FIX);

    /* Select PDMA request source as PDMA_EPWM1_CH0_TX(EPWM1 channel 0 accumulator interrupt) */
    PDMA_SetTransferMode(PDMA2,0, PDMA_EPWM1_CH0_TX, FALSE, 0);

    /* Set PDMA as single request type for EPWM */
    PDMA_SetBurstType(PDMA2,0, PDMA_REQ_SINGLE, PDMA_BURST_1);

    PDMA_EnableInt(PDMA2,0, PDMA_INT_TRANS_DONE);
    IRQ_SetHandler((IRQn_ID_t)PDMA2_IRQn, PDMA2_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)PDMA2_IRQn);

    g_u32IsTestOver = 0;

    /* Wait for PDMA transfer done */
    while (g_u32IsTestOver != 1);

    sysprintf("\n\nEPWM1 channel0 period register is updated to %d(0x%x)\n",EPWM_GET_CNR(EPWM1, 0),EPWM_GET_CNR(EPWM1, 0));
    sysprintf("Press any key to stop EPWM1 channel 0.\n");
    sysgetchar();

    /* Set EPWM1 channel 0 loaded value as 0 */
    EPWM_Stop(EPWM1, EPWM_CH_0_MASK);

    /* Wait until EPWM1 channel 0 Timer Stop */
    while((EPWM1->CNT[0] & EPWM_CNT0_CNT_Msk) != 0);

    /* Disable Timer for EPWM1 channel 0 */
    EPWM_ForceStop(EPWM1, EPWM_CH_0_MASK);

    /* Disable EPWM Output path for EPWM1 channel 0 */
    EPWM_DisableOutput(EPWM1, EPWM_CH_0_MASK);

    /* Disable PDMA GIC */
    IRQ_Disable(PDMA2_IRQn);

    PDMA_Close(PDMA2);

    while(1);
}



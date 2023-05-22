/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Capture the EPWM1 Channel 0 waveform by EPWM1 Channel 2, and use PDMA to transfer captured data.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint16_t g_u32Count[4];
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

/*--------------------------------------------------------------------------------------*/
/* Capture function to calculate the input waveform information                         */
/* g_u32Count[4] : Keep the internal counter value when input signal rising / falling     */
/*               happens                                                                */
/*                                                                                      */
/* time    A    B     C     D                                                           */
/*           ___   ___   ___   ___   ___   ___   ___   ___                              */
/*      ____|   |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_____                        */
/* index              0 1   2 3                                                         */
/*                                                                                      */
/* The capture internal counter down count from 0x10000, and reload to 0x10000 after    */
/* input signal falling happens (Time B/C/D)                                            */
/*--------------------------------------------------------------------------------------*/
void CalPeriodTime(EPWM_T *EPWM, uint32_t u32Ch)
{
    uint16_t u16RisingTime, u16FallingTime, u16HighPeriod, u16LowPeriod, u16TotalPeriod;

    g_u32IsTestOver = 0;
    /* Wait PDMA interrupt (g_u32IsTestOver will be set at IRQ_Handler function) */
    while(g_u32IsTestOver == 0);

    u16RisingTime = *(uint16_t *)(ptr_to_u32(&g_u32Count[1]) | NON_CACHE);

    u16FallingTime = *(uint16_t *)(ptr_to_u32(&g_u32Count[0]) | NON_CACHE);

    u16HighPeriod = *(uint16_t *)(ptr_to_u32(&g_u32Count[1]) | NON_CACHE)
                    - *(uint16_t *)(ptr_to_u32(&g_u32Count[2]) | NON_CACHE);

    u16LowPeriod = 0x10000 - *(uint16_t *)(ptr_to_u32(&g_u32Count[1]) | NON_CACHE);

    u16TotalPeriod = 0x10000 - *(uint16_t *)(ptr_to_u32(&g_u32Count[2]) | NON_CACHE);

    sysprintf("\nCapture Result: Rising Time = %d, Falling Time = %d \nHigh Period = %d, Low Period = %d, Total Period = %d.\n\n",
           u16RisingTime, u16FallingTime, u16HighPeriod, u16LowPeriod, u16TotalPeriod);
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

    /* Set PL multi-function pins for EPWM1 Channel 0 and 2 */
    SYS->GPL_MFPL &= ~(SYS_GPL_MFPL_PL0MFP_Msk | SYS_GPL_MFPL_PL2MFP_Msk);
    SYS->GPL_MFPL |= (SYS_GPL_MFPL_PL0MFP_EPWM1_CH0 | SYS_GPL_MFPL_PL2MFP_EPWM1_CH2);
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
    sysprintf("  This sample code will use EPWM1 channel 2 to capture the signal from EPWM1 channel 0.\n");
    sysprintf("  And the captured data is transferred by PDMA channel 0.\n");
    sysprintf("  I/O configuration:\n");
    sysprintf("    EPWM1 channel 2(PL.2) <--> EPWM1 channel 0(PL.0)\n\n");
    sysprintf("Use EPWM1 Channel 2(PL.2) to capture the EPWM1 Channel 0(PL.0) Waveform\n");

    while(1)
    {
        sysprintf("\n\nPress any key to start EPWM Capture Test\n");
        sysgetchar();

        /*--------------------------------------------------------------------------------------*/
        /* Set the EPWM1 Channel 0 as EPWM output function.                                       */
        /*--------------------------------------------------------------------------------------*/

        /* Set EPWM1 channel 0 output configuration */
        EPWM_ConfigOutputChannel(EPWM1, 0, 300, 30);

        /* Enable EPWM Output path for EPWM1 channel 0 */
        EPWM_EnableOutput(EPWM1, EPWM_CH_0_MASK);

        /* Enable Timer for EPWM1 channel 0 */
        EPWM_Start(EPWM1, EPWM_CH_0_MASK);

        /*--------------------------------------------------------------------------------------*/
        /* Configure PDMA peripheral mode form EPWM to memory                                    */
        /*--------------------------------------------------------------------------------------*/
        /* Open Channel 0 */
        PDMA_Open(PDMA2,BIT0);

        /* Transfer width is half word(16 bit) and transfer count is 4 */
        PDMA_SetTransferCnt(PDMA2,0, PDMA_WIDTH_16, 4);

        /* Set source address as EPWM capture channel PDMA register(no increment) and destination address as g_u32Count array(increment) */
        PDMA_SetTransferAddr(PDMA2,0, ptr_to_u32(&(EPWM1->PDMACAP[1])), PDMA_SAR_FIX, ptr_to_u32(&g_u32Count[0]), PDMA_DAR_INC);

        /* Select PDMA request source as EPWM RX(EPWM1 channel 2 should be EPWM1 pair 2) */
        PDMA_SetTransferMode(PDMA2,0, PDMA_EPWM1_P2_RX, FALSE, 0);

        /* Set PDMA as single request type for EPWM */
        PDMA_SetBurstType(PDMA2,0, PDMA_REQ_SINGLE, PDMA_BURST_4);

        PDMA_EnableInt(PDMA2,0, PDMA_INT_TRANS_DONE);
        IRQ_SetHandler((IRQn_ID_t)PDMA2_IRQn, PDMA2_IRQHandler);
        IRQ_Enable ((IRQn_ID_t)PDMA2_IRQn);

        /* Enable PDMA for EPWM1 channel 2 capture function, and set capture order as falling first, */
        /* And select capture mode as both rising and falling to do PDMA transfer. */
        EPWM_EnablePDMA(EPWM1, 2, FALSE, EPWM_CAPTURE_PDMA_RISING_FALLING_LATCH);

        /*--------------------------------------------------------------------------------------*/
        /* Set the EPWM1 channel 2 for capture function                                          */
        /*--------------------------------------------------------------------------------------*/

        /* Set EPWM1 channel 2 capture configuration */
        EPWM_ConfigCaptureChannel(EPWM1, 2, 52, 0);

        /* Enable Timer for EPWM1 channel 2 */
        EPWM_Start(EPWM1, EPWM_CH_2_MASK);

        /* Enable Capture Function for EPWM1 channel 2 */
        EPWM_EnableCapture(EPWM1, EPWM_CH_2_MASK);

        /* Enable falling capture reload */
        EPWM1->CAPCTL |= EPWM_CAPCTL_FCRLDEN2_Msk;

        /* Wait until EPWM1 channel 2 Timer start to count */
        while((EPWM1->CNT[2]) == 0);

        /* Capture the Input Waveform Data */
        CalPeriodTime(EPWM1, 2);
        /*---------------------------------------------------------------------------------------------------------*/
        /* Stop EPWM1 channel 0 (Recommended procedure method 1)                                                    */
        /* Set EPWM Timer loaded value(Period) as 0. When EPWM internal counter(CNT) reaches to 0, disable EPWM Timer */
        /*---------------------------------------------------------------------------------------------------------*/

        /* Set EPWM1 channel 0 loaded value as 0 */
        EPWM_Stop(EPWM1, EPWM_CH_0_MASK);

        /* Wait until EPWM1 channel 0 Timer Stop */
        while((EPWM1->CNT[0] & EPWM_CNT0_CNT_Msk) != 0);

        /* Disable Timer for EPWM1 channel 0 */
        EPWM_ForceStop(EPWM1, EPWM_CH_0_MASK);

        /* Disable EPWM Output path for EPWM1 channel 0 */
        EPWM_DisableOutput(EPWM1, EPWM_CH_0_MASK);

        /*---------------------------------------------------------------------------------------------------------*/
        /* Stop EPWM1 channel 2 (Recommended procedure method 1)                                                    */
        /* Set EPWM Timer loaded value(Period) as 0. When EPWM internal counter(CNT) reaches to 0, disable EPWM Timer */
        /*---------------------------------------------------------------------------------------------------------*/

        /* Set loaded value as 0 for EPWM1 channel 2 */
        EPWM_Stop(EPWM1, EPWM_CH_2_MASK);

        /* Wait until EPWM1 channel 2 current counter reach to 0 */
        while((EPWM1->CNT[2] & EPWM_CNT0_CNT_Msk) != 0);

        /* Disable Timer for EPWM1 channel 2 */
        EPWM_ForceStop(EPWM1, EPWM_CH_2_MASK);

        /* Disable Capture Function and Capture Input path for  EPWM1 channel 2*/
        EPWM_DisableCapture(EPWM1, EPWM_CH_2_MASK);

        /* Clear Capture Interrupt flag for EPWM1 channel 2 */
        EPWM_ClearCaptureIntFlag(EPWM1, 2, EPWM_CAPTURE_INT_FALLING_LATCH);

        /* Disable PDMA GIC */
        IRQ_Disable(PDMA2_IRQn);

        PDMA_Close(PDMA2);
    }
}



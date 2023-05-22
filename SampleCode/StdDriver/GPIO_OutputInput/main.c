/**************************************************************************//**
 * @file     main.c
 * @brief    Show how to set GPIO pin mode and use pin data input/output control.
 *
 * @copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include "stdio.h"
#include "NuMicro.h"

void SYS_Init(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable IP clock */
    CLK_EnableModuleClock(GPA_MODULE);
    CLK_EnableModuleClock(GPB_MODULE);
    CLK_EnableModuleClock(GPC_MODULE);
    CLK_EnableModuleClock(GPD_MODULE);
    CLK_EnableModuleClock(GPE_MODULE);
    CLK_EnableModuleClock(GPF_MODULE);
    CLK_EnableModuleClock(GPG_MODULE);
    CLK_EnableModuleClock(GPH_MODULE);
    CLK_EnableModuleClock(GPI_MODULE);
    CLK_EnableModuleClock(GPJ_MODULE);
    CLK_EnableModuleClock(GPK_MODULE);
    CLK_EnableModuleClock(GPL_MODULE);
    CLK_EnableModuleClock(GPM_MODULE);
    CLK_EnableModuleClock(GPN_MODULE);
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

int32_t main(void)
{

    int32_t i32Err, i32TimeOutCnt;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for sysprintf */
    UART0_Init();

    sysprintf("\n\nCPU @ %dHz\n", SystemCoreClock);

    sysprintf("+-------------------------------------------------+\n");
    sysprintf("|    PA.3(Output) and PD.8(Input) Sample Code     |\n");
    sysprintf("+-------------------------------------------------+\n\n");

    /*-----------------------------------------------------------------------------------------------------*/
    /* GPIO Basic Mode Test --- Use Pin Data Input/Output to control GPIO pin                              */
    /*-----------------------------------------------------------------------------------------------------*/
    sysprintf("  >> Please connect PA.3 and PD.8 first << \n");
    sysprintf("     Press any key to start test by using [Pin Data Input/Output Control] \n\n");
    sysgetchar();

    /* Configure PA.3 as Output mode and PD.8 as Input mode then close it */
    GPIO_SetMode(PA, BIT3, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PD, BIT8, GPIO_MODE_INPUT);

    i32Err = 0;
    sysprintf("GPIO PA.3(output mode) connect to PD.8(input mode) ......");

    /* Use Pin Data Input/Output Control to pull specified I/O or get I/O pin status */
    /* Set PA.3 output pin value is low */
    PA3 = 0;

    /* Set time out counter */
    i32TimeOutCnt = 100;

    /* Wait for PD.8 input pin status is low for a while */
    while(PD8 != 0)
    {
        if(i32TimeOutCnt > 0)
        {
            i32TimeOutCnt--;
        }
        else
        {
            i32Err = 1;
            break;
        }
    }

    /* Set PA.3 output pin value is high */
    PA3 = 1;

    /* Set time out counter */
    i32TimeOutCnt = 100;

    /* Wait for PD.8 input pin status is high for a while */
    while(PD8 != 1)
    {
        if(i32TimeOutCnt > 0)
        {
            i32TimeOutCnt--;
        }
        else
        {
            i32Err = 1;
            break;
        }
    }

    /* Print test result */
    if(i32Err)
    {
        sysprintf("  [FAIL].\n");
    }
    else
    {
        sysprintf("  [OK].\n");
    }

    /* Configure PA.3 and PD.8 to default Quasi-bidirectional mode */
    GPIO_SetMode(PA, BIT3, GPIO_MODE_QUASI);
    GPIO_SetMode(PD, BIT8, GPIO_MODE_QUASI);

    while(1);

}

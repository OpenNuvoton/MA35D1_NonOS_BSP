/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Show how to reload the WWDT counter value.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint8_t g_u32WWDTINTCounts;

/*---------------------------------------------------------------------------------------------------------*/
/* Define Functions Prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void WWDT_TEST_HANDLE(WWDT_T *wwdt);


/**
 * @brief       IRQ Handler for WWDT0 Interrupt
 *
 * @param       None
 *
 * @details     The WWDT0_IRQHandler is default IRQ of WWDT0.
 */
void WWDT0_IRQHandler(void)
{
	WWDT_TEST_HANDLE(WWDT0);
}

/**
 * @brief       IRQ Handler for WWDT1 Interrupt
 *
 * @param       None
 *
 * @details     The WWDT1_IRQHandler is default IRQ of WWDT1.
 */
void WWDT1_IRQHandler(void)
{
	WWDT_TEST_HANDLE(WWDT1);
}

void WWDT_TEST_HANDLE(WWDT_T *wwdt)
{
    if(WWDT_GET_INT_FLAG(wwdt) == 1)
    {
        /* Clear WWDT compare match interrupt flag */
        WWDT_CLEAR_INT_FLAG(wwdt);

        PA0 ^= 1;

        g_u32WWDTINTCounts++;

        if(g_u32WWDTINTCounts < 10)
        {
            /* To reload the WWDT counter value to 0x3F */
            WWDT_RELOAD_COUNTER(wwdt);
        }

        //sysprintf("WWDT%d compare match interrupt occurred. (%d)\n", (wwdt == WWDT1) ? 1 : 0, g_u32WWDTINTCounts);
    }
}

void SYS_Init(void)
{
    /* Enable UART module clock */
	CLK_EnableModuleClock(UART0_MODULE);

	/* Enable GPIOA peripheral clock */
	CLK_EnableModuleClock(GPA_MODULE);

	/* Select UART module clock source as HXT */
	CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

	/* Select WWDT0 module clock source as LIRC */
	CLK_SetModuleClock(WWDT0_MODULE, CLK_CLKSEL3_WWDT0SEL_LIRC, 0);

	/* Select WWDT1 module clock source as LIRC */
	CLK_SetModuleClock(WWDT1_MODULE, CLK_CLKSEL3_WWDT1SEL_LIRC, 0);

	/* To update the variable SystemCoreClock */
	SystemCoreClockUpdate();

	/* Set GPE multi-function pins for UART RXD and TXD */
	SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE15MFP_Msk | SYS_GPE_MFPH_PE14MFP_Msk);
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE15MFP_UART0_RXD | SYS_GPE_MFPH_PE14MFP_UART0_TXD);
}

int main(void)
{
	uint8_t u8Item;
	uint32_t u32PeriodTime, u32MaxTimeOutPeriod;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    u32PeriodTime = (1000000 * 1024 / 32000) * (64 - 32);
    u32MaxTimeOutPeriod = (1000000 * 1024 / 32000) * 64;

    sysprintf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    sysprintf("+----------------------------------------------------------------+\n");
    sysprintf("|            WWDT Compare March Interrupt Sample Code            |\n");
    sysprintf("+----------------------------------------------------------------+\n");
    sysprintf("| WWDT Settings:                                                 |\n");
    sysprintf("|   - Clock source is LIRC (32 kHz)                              |\n");
    sysprintf("|   - WWDT counter prescale period is 1024, and max time-out     |\n");
    sysprintf("|     period is 1024 * (64 * WWDT_CLK)                           |\n");
    sysprintf("|   - Interrupt enable                                           |\n");
    sysprintf("|   - Window Compare value is 32                                 |\n");
    sysprintf("|                                                                |\n");
    sysprintf("| System will generate first WWDT compare match interrupt event  |\n");
    sysprintf("| after %7d us.                                            |\n", u32PeriodTime);
    sysprintf("| 1.) use PA.0 high/low period to check WWDT compare match       |\n");
    sysprintf("|     interrupt period time                                      |\n");
    sysprintf("| 2.) reload WWDT counter value to avoid WWDT time-out reset     |\n");
    sysprintf("|     system occurred when interrupt counts less than 11         |\n");
    sysprintf("| 3.) do not reload WWDT counter value to generate WWDT time-out |\n");
    sysprintf("|     reset system event when interrupt counts large than 10     |\n");
    sysprintf("+----------------------------------------------------------------+\n\n");

    sysprintf("Please select WWDT0 or WWDT1 test\n");
	sysprintf("[0] WWDT0    [1] WWDT1\n");
	u8Item = sysgetchar() - 0x30;
	sysprintf("%d\n", u8Item);

    /* To check if system has been reset by WWDT time-out reset or not */
	if (u8Item == 0)
	{
		if(WWDT_GET_RESET_FLAG(WWDT0) == 1)
		{
			sysprintf("*** System has been reset by WWDT0 time-out reset event. [WWDT_CTL: 0x%08X] ***\n\n", WWDT0->CTL);
			WWDT_CLEAR_RESET_FLAG(WWDT0);
			while(1);
		}
	}
	else
	{
		if(WWDT_GET_RESET_FLAG(WWDT1) == 1)
		{
			sysprintf("*** System has been reset by WWDT1 time-out reset event. [WWDT_CTL: 0x%08X] ***\n\n", WWDT1->CTL);
			WWDT_CLEAR_RESET_FLAG(WWDT1);
			while(1);
		}
	}

    /* Use PA.0 to check WWDT compare match interrupt period time */
    PA->MODE = 0xFFFFFFFD;
    PA0 = 1;

    /* Enable WWDT GIC */
    IRQ_SetHandler((IRQn_ID_t)WWDT0_IRQn, WWDT0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)WWDT0_IRQn);
    IRQ_SetHandler((IRQn_ID_t)WWDT1_IRQn, WWDT1_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)WWDT1_IRQn);

    /*  Max time-out period is 1024*(64*WWDT_CLK);
        WWDT compare value is 32;
        Enable WWDT compare match interrupt;

        Note: WWDT_CTL register can be written only once after chip is powered on or reset. */

    g_u32WWDTINTCounts = 0;
    if (u8Item == 0)
	{
    	/* Configure WWDT0 settings, enable compare match interrupt and start WWDT0 counting */
    	WWDT_Open(WWDT0, WWDT_PRESCALER_1024, 32, TRUE);
	}
    else
    {
    	/* Configure WWDT1 settings, enable compare match interrupt and start WWDT1 counting */
    	WWDT_Open(WWDT1, WWDT_PRESCALER_1024, 32, TRUE);
    }

    while(1);
}

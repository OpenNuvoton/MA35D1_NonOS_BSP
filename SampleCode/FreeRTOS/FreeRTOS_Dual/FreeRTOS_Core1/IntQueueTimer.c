/*
 * FreeRTOS V202212.01
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/**************************************************************************//**
 * @file     IntQueueTimer.c
 *
 * @brief    Register timers for interrupt nesting demo.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

/*
 * This file initialises three timers as follows:
 *
 * Basic timer channels 0 and 1 provide the interrupts that are used with the
 * IntQ standard demo tasks, which test interrupt nesting and using queues from
 * interrupts.  The interrupts use slightly different frequencies so will
 * occasionally nest.
 *
 * Basic timer channel 2 provides a much higher frequency timer that tests the
 * nesting of interrupts that don't use the FreeRTOS API.
 *
 * All the timers can nest with the tick interrupt - creating a maximum
 * interrupt nesting depth of 4 (which is shown as a max nest count of 3 as the
 * tick interrupt does not increment the nesting count variable).
 *
 */

#include "NuMicro.h"
#include "FreeRTOS.h"

#include "IntQueueTimer.h"
#include "IntQueue.h"

/* The frequencies at which the first two timers expire are slightly offset to
ensure they don't remain synchronised.  The frequency of the interrupt that
operates above the max syscall interrupt priority is 10 times faster so really
hammers the interrupt entry and exit code. */
#define tmrTIMER_3_FREQUENCY	( 2000UL )
#define tmrTIMER_4_FREQUENCY	( 2003UL )
#define tmrTIMER_5_FREQUENCY	( 20000UL )

/* The high frequency interrupt is given a priority above the maximum at which
interrupt safe FreeRTOS calls can be made.  The priority of the lower frequency
timers must still be above the tick interrupt priority. */
#define tmrLOWER_PRIORITY		( configMAX_API_CALL_INTERRUPT_PRIORITY + 1 )
#define tmrMEDIUM_PRIORITY		( configMAX_API_CALL_INTERRUPT_PRIORITY + 0 )
#define tmrHIGHER_PRIORITY		( configMAX_API_CALL_INTERRUPT_PRIORITY - 1 )
/*-----------------------------------------------------------*/

void TMR3_IRQHandler(void);
void TMR4_IRQHandler(void);
void TMR5_IRQHandler(void);
/*-----------------------------------------------------------*/

/* Used to provide a means of ensuring the intended interrupt nesting depth is
actually being reached. */
extern uint64_t ullPortInterruptNesting;
static volatile uint32_t ulMaxRecordedNesting = 1;

/* Used to ensure the high frequency timer is running at the expected
frequency. */
static volatile uint32_t ulHighFrequencyTimerCounts = 0;
/*-----------------------------------------------------------*/

/**
 * @brief Register TIMER0/TIMER1/TIMER2 three timer with
 * specific frequency & priority
 */
void vInitialiseTimerForIntQueueTest( void )
{
    BaseType_t xStatus;

    SYS_UnlockReg();

    CLK_EnableModuleClock(TMR3_MODULE);
    CLK_EnableModuleClock(TMR4_MODULE);
    CLK_EnableModuleClock(TMR5_MODULE);

    CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_HXT, 0);
    CLK_SetModuleClock(TMR4_MODULE, CLK_CLKSEL1_TMR4SEL_HXT, 0);
    CLK_SetModuleClock(TMR5_MODULE, CLK_CLKSEL1_TMR5SEL_HXT, 0);

    TIMER_Open(TIMER3, TIMER_PERIODIC_MODE, tmrTIMER_3_FREQUENCY);
    TIMER_Open(TIMER4, TIMER_PERIODIC_MODE, tmrTIMER_4_FREQUENCY);
    TIMER_Open(TIMER5, TIMER_PERIODIC_MODE, tmrTIMER_5_FREQUENCY);

    TIMER_EnableInt(TIMER3);
    TIMER_EnableInt(TIMER4);
    TIMER_EnableInt(TIMER5);

    IRQ_SetPriority((IRQn_ID_t)TMR3_IRQn, tmrLOWER_PRIORITY << portPRIORITY_SHIFT);
    IRQ_SetPriority((IRQn_ID_t)TMR4_IRQn, tmrMEDIUM_PRIORITY << portPRIORITY_SHIFT);
    IRQ_SetPriority((IRQn_ID_t)TMR5_IRQn, tmrHIGHER_PRIORITY << portPRIORITY_SHIFT);

    IRQ_SetHandler((IRQn_ID_t)TMR3_IRQn, TMR3_IRQHandler);
    IRQ_SetHandler((IRQn_ID_t)TMR4_IRQn, TMR4_IRQHandler);
    IRQ_SetHandler((IRQn_ID_t)TMR5_IRQn, TMR5_IRQHandler);

    IRQ_Enable((IRQn_ID_t)TMR3_IRQn);
    IRQ_Enable((IRQn_ID_t)TMR4_IRQn);
    IRQ_Enable((IRQn_ID_t)TMR5_IRQn);

    TIMER_Start(TIMER3);
    TIMER_Start(TIMER4);
    TIMER_Start(TIMER5);

    SYS_LockReg();
}
/*-----------------------------------------------------------*/

void TMR3_IRQHandler(void)
{
    BaseType_t xYieldRequired;

    TIMER_ClearIntFlag(TIMER3);

    xYieldRequired = xFirstTimerHandler();

    if( ullPortInterruptNesting > ulMaxRecordedNesting )
    {
        ulMaxRecordedNesting = ullPortInterruptNesting;
    }

    portYIELD_FROM_ISR( xYieldRequired );
}

void TMR4_IRQHandler(void)
{
    BaseType_t xYieldRequired;

    TIMER_ClearIntFlag(TIMER4);

    xYieldRequired = xSecondTimerHandler();

    if( ullPortInterruptNesting > ulMaxRecordedNesting )
    {
        ulMaxRecordedNesting = ullPortInterruptNesting;
    }

    portYIELD_FROM_ISR( xYieldRequired );
}

void TMR5_IRQHandler(void)
{
    BaseType_t xYieldRequired;

    TIMER_ClearIntFlag(TIMER5);

    ulHighFrequencyTimerCounts++;
    xYieldRequired = pdFALSE;

    if( ullPortInterruptNesting > ulMaxRecordedNesting )
    {
        ulMaxRecordedNesting = ullPortInterruptNesting;
    }

    portYIELD_FROM_ISR( xYieldRequired );
}

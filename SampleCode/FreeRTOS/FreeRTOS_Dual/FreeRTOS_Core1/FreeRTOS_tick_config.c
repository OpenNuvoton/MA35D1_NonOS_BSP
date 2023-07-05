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
 * @file     FreeRTOS_tick_config.c
 *
 * @brief    Timer interrupt for FreeRTOS tick.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

/* Nuvoton includes. */
#include "NuMicro.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/*-----------------------------------------------------------*/

/* TIMER10 used to generate the tick interrupt, change to other timers if you like */
void vConfigureTickInterrupt( void )
{
    extern void FreeRTOS_Tick_Handler( void );

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable IP clock */
    CLK_EnableModuleClock(TMR10_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(TMR10_MODULE, CLK_CLKSEL2_TMR10SEL_HXT, 0);

    /* Set timer frequency to configTICK_RATE_HZ */
    TIMER_Open(TIMER10, TIMER_PERIODIC_MODE, configTICK_RATE_HZ);

    /* The priority must be the lowest possible. */
    IRQ_SetPriority((IRQn_ID_t)TMR10_IRQn, portLOWEST_USABLE_INTERRUPT_PRIORITY << portPRIORITY_SHIFT);

    /* Enable timer interrupt, connect to handler */
    TIMER_EnableInt(TIMER10);
    IRQ_SetHandler((IRQn_ID_t)TMR10_IRQn, FreeRTOS_Tick_Handler);
    IRQ_Enable((IRQn_ID_t)TMR10_IRQn);

    vClearTickInterrupt();

    /* Start timer */
    TIMER_Start(TIMER10);

    /* Lock protected registers */
    SYS_LockReg();
}
/*-----------------------------------------------------------*/

void vClearTickInterrupt( void )
{
    TIMER_ClearIntFlag(TIMER10);

    __asm volatile( "DSB SY" );
    __asm volatile( "ISB SY" );
}
/*-----------------------------------------------------------*/

/* IRQ take over by FreeRTOS kernel */
void vApplicationIRQHandler( uint32_t ulICCIAR )
{
    /* Interrupts cannot be re-enabled until the source of the interrupt is
    cleared. The ID of the interrupt is obtained by bitwise ANDing the ICCIAR
    value with 0x3FF. */

    IRQHandler_t handler;
    IRQn_ID_t num = (int32_t)ulICCIAR;

    /* Call the function installed in the array of installed handler
    functions. */
    handler = IRQ_GetHandler(num);
    if(handler != 0)
        (*handler)();
    IRQ_EndOfInterrupt(num);
}

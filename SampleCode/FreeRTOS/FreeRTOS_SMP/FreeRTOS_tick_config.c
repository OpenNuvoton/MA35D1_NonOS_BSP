/**************************************************************************//**
 * @file     FreeRTOS_tick_config.c
 *
 * @brief    Timer interrupt for FreeRTOS tick using ARMv8 Generic Timer.
 *
 *           Only core 0 runs the OS tick.  The FreeRTOS-SMP kernel's
 *           xTaskIncrementTick() has no per-core gating - it unconditionally
 *           increments xTickCount inside a critical section.
 *           If both cores called FreeRTOS_Tick_Handler the tick would
 *           advance at 2x the intended rate.
 *
 *           Core 0's tick handler already handles time-slicing for all
 *           cores: the kernel calls prvYieldCore(x) (SGI) for any
 *           remote core that needs a context switch.
 *
 * @copyright (C) 2026 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

/* Nuvoton includes. */
#include "NuMicro.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/*-----------------------------------------------------------*/
/* Prototypes for the standard FreeRTOS callback/hook functions
 * implemented within this file.                               */
/*-----------------------------------------------------------*/
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/* Standard FreeRTOS hook functions.                         */
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
    /* Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
    volatile size_t xFreeHeapSpace;

    /* This is just a trivial example of an idle hook.  It is called on each
    cycle of the idle task.  It must *NOT* attempt to block.  In this case the
    idle task just queries the amount of FreeRTOS heap that remains.  See the
    memory management section on the http://www.FreeRTOS.org web site for memory
    management options.  If there is a lot of heap memory free then the
    configTOTAL_HEAP_SIZE value in FreeRTOSConfig.h can be reduced to free up
    RAM. */
    xFreeHeapSpace = xPortGetFreeHeapSize();

    /* Remove compiler warning about xFreeHeapSpace being set but never used. */
    ( void ) xFreeHeapSpace;
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{

}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task.  Note: In the SMP kernel, this callback is only called
for core 0's idle task.  Core 1's idle task uses internal static buffers
allocated inside prvCreateIdleTasks(). */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, configSTACK_DEPTH_TYPE *puxIdleTaskStackSize )
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *puxIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* SMP: The new kernel (v11.1+) requires a separate memory callback for
passive idle tasks (cores 1 through configNUMBER_OF_CORES-1).  Core 0's
idle task uses vApplicationGetIdleTaskMemory() above; all other cores'
idle tasks use this function.  For a dual-core system this is called once
with xPassiveIdleTaskIndex == 0 (for core 1's idle task). */
void vApplicationGetPassiveIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, configSTACK_DEPTH_TYPE *puxIdleTaskStackSize, BaseType_t xPassiveIdleTaskIndex )
{
    /* We only have one passive idle task (core 1).  Use static buffers. */
    static StaticTask_t xPassiveIdleTaskTCBs[ configNUMBER_OF_CORES - 1 ];
    static StackType_t uxPassiveIdleTaskStacks[ configNUMBER_OF_CORES - 1 ][ configMINIMAL_STACK_SIZE ];

    *ppxIdleTaskTCBBuffer = &xPassiveIdleTaskTCBs[ xPassiveIdleTaskIndex ];
    *ppxIdleTaskStackBuffer = uxPassiveIdleTaskStacks[ xPassiveIdleTaskIndex ];
    *puxIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, configSTACK_DEPTH_TYPE *puxTimerTaskStackSize )
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *puxTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
/*-----------------------------------------------------------*/

void vMainAssertCalled( const char *pcFileName, uint32_t ulLineNumber )
{
    /* Disable interrupts at the CPU level (DAIF.I) rather than using
     * taskENTER_CRITICAL().  In SMP mode taskENTER_CRITICAL() acquires the
     * task+ISR spinlocks.  If an assert fires inside vTaskSwitchContext()
     * or any other path that already holds a spinlock, calling
     * taskENTER_CRITICAL() here would attempt to re-acquire the same
     * non-reentrant lock on this core, causing an immediate self-deadlock
     * that permanently orphans both locks and hangs the other core too.
     *
     * portDISABLE_INTERRUPTS() (DAIF mask) is safe to call from any
     * context and does not interact with the SMP lock protocol. */
    ( void ) portDISABLE_INTERRUPTS();
    sysprintf( "ASSERT!  Line %lu of file %s\r\n", ulLineNumber, pcFileName );
    for( ;; );
}
/*-----------------------------------------------------------*/

/* The generic timer frequency is 12 MHz on MA35D1. */
#define configGENERIC_TIMER_FREQ_HZ     ( 12000000UL )

/* The timer load value to achieve the desired tick rate. */
#define configGENERIC_TIMER_LOAD_VALUE  ( configGENERIC_TIMER_FREQ_HZ / configTICK_RATE_HZ )

/*-----------------------------------------------------------*/

/*
 * vConfigureTickInterrupt() sets up the ARMv8 Non-Secure Physical Timer (CNTP)
 * to generate periodic interrupts at configTICK_RATE_HZ.
 *
 * The CNTP timer is a PPI (Private Peripheral Interrupt, IRQ 30) - each core
 * has its own independent instance.  However, only core 0 runs the tick
 * because the SMP V202110.00 kernel's xTaskIncrementTick() unconditionally
 * increments xTickCount (no per-core gating).  If both cores ran the tick
 * handler, xTickCount would advance at 2x the intended rate.
 *
 * Core 0's tick handler already handles scheduling for all cores: when a
 * remote core needs to yield, the kernel sends it an SGI via prvYieldCore().
 *
 * This function is called once by core 0 during xPortStartScheduler().
 */
void vConfigureTickInterrupt( void )
{
    extern void FreeRTOS_Tick_Handler( void );

    /* Disable the timer while configuring. */
    EL0_SetControl( 0 );

    /* Set the compare value for the next tick.
     * We use the compare value (CNTP_CVAL_EL0) approach:
     * Set it to current physical counter + load value. */
    EL0_SetPhysicalCompareValue( EL0_GetCurrentPhysicalValue() + configGENERIC_TIMER_LOAD_VALUE );

    /* The priority must be the lowest possible for FreeRTOS tick. */
    IRQ_SetPriority( (IRQn_ID_t)NonSecPhysicalTimer_IRQn,
                     portLOWEST_USABLE_INTERRUPT_PRIORITY << portPRIORITY_SHIFT );

    /* Install FreeRTOS_Tick_Handler as the interrupt handler. */
    IRQ_SetHandler( (IRQn_ID_t)NonSecPhysicalTimer_IRQn, FreeRTOS_Tick_Handler );

    /* Enable the timer interrupt. */
    IRQ_Enable( (IRQn_ID_t)NonSecPhysicalTimer_IRQn );

    /* Enable the timer (CNTP_CTL_EL0.ENABLE = 1, IMASK = 0). */
    EL0_SetControl( 1U );
}
/*-----------------------------------------------------------*/

/*
 * vClearTickInterrupt() clears the generic timer interrupt by programming
 * the next compare value. The generic timer fires when CNTPCT_EL0 >= CNTP_CVAL_EL0,
 * so we advance CNTP_CVAL_EL0 by one tick period to clear the condition and
 * schedule the next tick.
 */
void vClearTickInterrupt( void )
{
    /* Advance the compare value by one tick period.
     * Reading the current compare value and adding the load value ensures
     * we don't accumulate drift even if we're slightly late handling the interrupt. */
    EL0_SetPhysicalCompareValue( EL0_GetPhysicalCompareValue() + configGENERIC_TIMER_LOAD_VALUE );

    __asm volatile( "DSB SY" );
    __asm volatile( "ISB SY" );
}
/*-----------------------------------------------------------*/

/* IRQ take over by FreeRTOS kernel.
 *
 * This is a STRONG definition of vApplicationIRQHandler, which overrides the
 * weak FPU-saving version in portASM.S.  Because this strong version is
 * linked, NO FPU registers are saved/restored in the IRQ path - giving
 * maximum interrupt latency performance.
 *
 * If your ISR callbacks (or functions they call) use floating-point or
 * NEON instructions, you have two options:
 *
 *   1. Remove or rename this function so the weak vApplicationIRQHandler
 *      in portASM.S is linked instead.  Then implement
 *      vApplicationFPUSafeIRQHandler() with the same body as below.
 *      The port will automatically save/restore all 32 Q registers +
 *      FPSR/FPCR around your handler.
 *
 *   2. Keep this function and manually save/restore the FPU registers
 *      you use in your ISR code.
 */
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

    /* NOTE: Do NOT call IRQ_EndOfInterrupt() here.
     * FreeRTOS_IRQ_Handler in portASM.S writes the ICCIAR value to
     * ICCEOIR after this function returns (the standard FreeRTOS
     * Cortex-A pattern).  Calling IRQ_EndOfInterrupt() here would
     * result in a double EOI which can cause GICv2 priority-drop
     * issues. */
}

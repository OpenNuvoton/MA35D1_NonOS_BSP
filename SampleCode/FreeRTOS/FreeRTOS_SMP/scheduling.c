/**************************************************************************//**
 * @file     scheduling.c
 *
 * @brief    FreeRTOS-SMP scheduling demos for MA35 dual-core Cortex-A35.
 *
 *           Demo selection via #define switches:
 *             DEMO1_CRITICAL_AND_ISR    - Combined critical-section preemption
 *                                         + ISR semaphore yield
 *             DEMO2_IRQ_NESTING_STORM   - Two HW timers at different frequencies
 *                                         and GIC priorities; checks
 *                                         ullPortInterruptNesting and SP alignment
 *             DEMO3_MIGRATION_CRITICAL  - Critical section migrates with TCB;
 *                                         spinlock blocks other core
 *             DEMO4_FPU_PINNED          - Tests FPU context correctness when a
 *                                         disturber task is pinned to the same core
 *             DEMO5_PINGPONG            - Cross-core ping-pong via
 *                                         xTaskNotifyGive / SGI yield
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2026 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include "NuMicro.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include <math.h>

/*-----------------------------------------------------------
 * Demo selection - enable / disable individual demos.
 *----------------------------------------------------------*/
#define DEMO1_CRITICAL_AND_ISR      1
#define DEMO2_IRQ_NESTING_STORM     1
#define DEMO3_MIGRATION_CRITICAL    1
#define DEMO4_FPU_PINNED            1
#define DEMO5_PINGPONG              1

/*-----------------------------------------------------------
 * Common constants and helpers.
 *----------------------------------------------------------*/
#define DEMO_STACK_SIZE             ( configMINIMAL_STACK_SIZE * 2 )

/* Per-core interrupt nesting depth. */
extern uint64_t ullPortInterruptNesting[];

static void prvPrintResult( const char *pcTestName, BaseType_t xPass )
{
    if( xPass )
        sysprintf( "  [PASS] %s\r\n", pcTestName );
    else
        sysprintf( "  [FAIL] %s\r\n", pcTestName );

    configASSERT( xPass );
}

/*===========================================================================
 * DEMO 1 - Preemption & Yield
 *
 * Part A: A low-priority task running on core 0 is preempted by a
 *         higher-priority task on the same core.  Both tasks are pinned
 *         to core 0.  A busy "blocker" task occupies core 1.
 *         The low-priority task records its core before and after - the
 *         high-priority task must have interrupted it on core 0.
 * Part B: taskYIELD() from a running task.
 * Part C: A software timer callback gives a semaphore from ISR context
 *         and calls portYIELD_FROM_ISR to wake a blocked task.
 *===========================================================================*/
#if ( DEMO1_CRITICAL_AND_ISR == 1 )

static volatile BaseType_t xHighPriorityRan  = pdFALSE;
static volatile BaseType_t xHighPriCore      = -1;
static volatile BaseType_t xLowWasPreempted  = pdFALSE;
static volatile BaseType_t xYieldDone        = pdFALSE;
static SemaphoreHandle_t   xISRSem           = NULL;
static TimerHandle_t       xISRTimer         = NULL;
static SemaphoreHandle_t   xDemo1Done        = NULL;
static SemaphoreHandle_t   xPreemptDone      = NULL;

/* High-priority task - pinned to core 0 (same as the low-prio task). */
static void prvHighPriorityTask( void *pv )
{
    ( void ) pv;
    xHighPriCore = ( BaseType_t ) portGET_CORE_ID();
    sysprintf( "  [Demo1] High-priority task running on core %d\r\n",
               ( int ) xHighPriCore );
    xHighPriorityRan = pdTRUE;
    vTaskDelete( NULL );
}

/* Low-priority busy task on core 0.  It spins until the high-priority
 * task has run, proving it was preempted on the same core. */
static void prvLowPriorityTask( void *pv )
{
    ( void ) pv;
    BaseType_t xMyCore = ( BaseType_t ) portGET_CORE_ID();
    sysprintf( "  [Demo1] Low-priority task spinning on core %d\r\n",
               ( int ) xMyCore );

    /* Busy-wait until the high-prio task has run (it preempts us). */
    while( xHighPriorityRan == pdFALSE )
    {
        /* Spin - the scheduler will preempt us when HiPri becomes ready. */
    }

    /* If we get here, we were preempted and then resumed. */
    xLowWasPreempted = pdTRUE;
    sysprintf( "  [Demo1] Low-priority task resumed on core %d after preemption\r\n",
               ( int ) portGET_CORE_ID() );

    xSemaphoreGive( xPreemptDone );
    vTaskDelete( NULL );
}

/* Blocker - occupies core 1 so the scheduler can't put HiPri there. */
static void prvBlockerTask( void *pv )
{
    ( void ) pv;
    /* Busy-wait until the preemption test is done. */
    while( xHighPriorityRan == pdFALSE )
    {
    }
    vTaskDelete( NULL );
}

static void prvYieldTask( void *pv )
{
    ( void ) pv;
    sysprintf( "  [Demo1] Yield task on core %d - calling taskYIELD()\r\n",
               ( int ) portGET_CORE_ID() );
    taskYIELD();
    sysprintf( "  [Demo1] Yield task resumed after taskYIELD()\r\n" );
    xYieldDone = pdTRUE;
    vTaskDelete( NULL );
}

static void prvISRTimerCallback( TimerHandle_t xTimer )
{
    ( void ) xTimer;
    sysprintf( "  [Demo1] Timer CB on core %d - giving semaphore from ISR\r\n",
               ( int ) portGET_CORE_ID() );

    BaseType_t xWoken = pdFALSE;
    xSemaphoreGiveFromISR( xISRSem, &xWoken );
    portYIELD_FROM_ISR( xWoken );
}

static void prvDemo1Task( void *pv )
{
    ( void ) pv;
    TaskHandle_t xH;

    sysprintf( "\r\n--- Demo 1: Preemption & Yield ---\r\n" );

    /* Part A - same-core preemption.
     *
     * Pin a low-priority busy task and the high-priority task both
     * to core 0.  A blocker task occupies core 1 so core 1 is not
     * free.  When HiPri is created it must preempt LowPri on core 0. */
    xHighPriorityRan = pdFALSE;
    xLowWasPreempted = pdFALSE;
    xHighPriCore     = -1;

    /* Blocker on core 1 (prio 5 - keeps core 1 busy). */
    xH = NULL;
    xTaskCreate( prvBlockerTask, "Blk", DEMO_STACK_SIZE, NULL,
                 tskIDLE_PRIORITY + 5, &xH );
    configASSERT( xH );
    vTaskCoreAffinitySet( xH, ( 1U << 1 ) );

    /* Low-priority task on core 0 (prio 3 - will be preempted). */
    xH = NULL;
    xTaskCreate( prvLowPriorityTask, "LoPri", DEMO_STACK_SIZE, NULL,
                 tskIDLE_PRIORITY + 3, &xH );
    configASSERT( xH );
    vTaskCoreAffinitySet( xH, ( 1U << 0 ) );

    /* Small delay so LowPri starts spinning. */
    vTaskDelay( pdMS_TO_TICKS( 20 ) );

    /* High-priority task on core 0 (prio 6 - must preempt LoPri). */
    xH = NULL;
    xTaskCreate( prvHighPriorityTask, "HiPri", DEMO_STACK_SIZE, NULL,
                 tskIDLE_PRIORITY + 6, &xH );
    configASSERT( xH );
    vTaskCoreAffinitySet( xH, ( 1U << 0 ) );

    /* Wait for the preemption test to complete. */
    xSemaphoreTake( xPreemptDone, pdMS_TO_TICKS( 2000 ) );

    prvPrintResult( "Demo1-A  High-priority task ran", xHighPriorityRan );
    prvPrintResult( "Demo1-A  High-priority ran on core 0 (same core)",
                    ( xHighPriCore == 0 ) ? pdTRUE : pdFALSE );
    prvPrintResult( "Demo1-A  Low-priority was preempted and resumed",
                    xLowWasPreempted );

    /* Part B - voluntary yield. */
    xYieldDone = pdFALSE;
    xTaskCreate( prvYieldTask, "Yield", DEMO_STACK_SIZE, NULL,
                 tskIDLE_PRIORITY + 3, NULL );
    vTaskDelay( pdMS_TO_TICKS( 100 ) );
    prvPrintResult( "Demo1-B  taskYIELD()", xYieldDone );

    /* Part C - ISR semaphore wakeup via timer callback. */
    xTimerStart( xISRTimer, portMAX_DELAY );

    if( xSemaphoreTake( xISRSem, pdMS_TO_TICKS( 2000 ) ) == pdTRUE )
    {
        sysprintf( "  [Demo1] Task woken by ISR semaphore on core %d\r\n",
                   ( int ) portGET_CORE_ID() );
        prvPrintResult( "Demo1-C  Yield from ISR (semaphore wakeup)", pdTRUE );
    }
    else
    {
        sysprintf( "  [Demo1] TIMEOUT waiting for ISR semaphore!\r\n" );
        prvPrintResult( "Demo1-C  Yield from ISR (semaphore wakeup)", pdFALSE );
    }

    sysprintf( "--- Demo 1 complete ---\r\n" );
    xSemaphoreGive( xDemo1Done );
    vTaskDelete( NULL );
}

#endif /* DEMO1_CRITICAL_AND_ISR */

/*===========================================================================
 * DEMO 2 - Interrupt Nesting Storm
 *
 * Setup:
 *   - TIMER2 fires at 1 MHz (GIC priority = lower urgency, higher number)
 *   - TIMER3 fires at 1 MHz (GIC priority = higher urgency, lower number)
 *   Since TIMER3 has higher GIC urgency it can preempt TIMER2's ISR,
 *   producing genuine interrupt nesting on the same core.
 *
 * Validation:
 *   - Inside each ISR, read ullPortInterruptNesting[coreID] and confirm > 0.
 *   - After both timers run for a while, check that nesting depth returned
 *     to 0 from task context.
 *   - Check SP alignment (16-byte) inside the ISR.
 *===========================================================================*/
#if ( DEMO2_IRQ_NESTING_STORM == 1 )

/* --- statistics counters (written from ISR, read from task) --- */
static volatile uint32_t ulTmr2Count     = 0;
static volatile uint32_t ulTmr3Count     = 0;
static volatile uint32_t ulNestSeen      = 0;  /* times nesting depth > 1 */
static volatile BaseType_t xAlignOK      = pdTRUE;
static volatile BaseType_t xNestingBad   = pdFALSE; /* set if nesting <= 0 in ISR */
static SemaphoreHandle_t xDemo2Done      = NULL;
static volatile uint32_t ulTmr2WhichCore[2] = { 0, 0 };
static volatile uint32_t ulTmr3WhichCore[2] = { 0, 0 };

/*
 * GIC priority assignments (GIC: lower number = higher urgency):
 *   TIMER3 - highest FreeRTOS-safe priority
 *   TIMER2 - lower urgency, can be preempted by TIMER3
 *
 * Both priorities are >= configMAX_API_CALL_INTERRUPT_PRIORITY so they are
 * masked by FreeRTOS critical sections (safe to use FreeRTOS ISR APIs).
 */
#define DEMO2_TMR2_GIC_PRIO   ( ( configMAX_API_CALL_INTERRUPT_PRIORITY + 2 ) \
                                  << portPRIORITY_SHIFT )
#define DEMO2_TMR3_GIC_PRIO   ( configMAX_API_CALL_INTERRUPT_PRIORITY \
                                  << portPRIORITY_SHIFT )

/* Duration to let the timers run (ms). */
#define DEMO2_RUN_MS           500

static void prvTmr2Handler( void )
{
    if( TIMER_GetIntFlag( TIMER2 ) == 1 )
    {
        TIMER_ClearIntFlag( TIMER2 );

        BaseType_t xCore = ( BaseType_t ) portGET_CORE_ID();
        ulTmr2WhichCore[xCore]++;
        uint64_t ullNest = ullPortInterruptNesting[ xCore ];

        /* Must be >= 1 while we are inside an ISR. */
        if( ullNest < 1 )
            xNestingBad = pdTRUE;

        /* Check 16-byte SP alignment. */
        uint64_t ullSP;
        __asm volatile ( "MOV %0, SP" : "=r" ( ullSP ) );
        if( ( ullSP & 0xFUL ) != 0 )
            xAlignOK = pdFALSE;

        ulTmr2Count++;
    }
}

static void prvTmr3Handler( void )
{
    if( TIMER_GetIntFlag( TIMER3 ) == 1 )
    {
        TIMER_ClearIntFlag( TIMER3 );

        BaseType_t xCore = ( BaseType_t ) portGET_CORE_ID();
        ulTmr3WhichCore[xCore]++;
        uint64_t ullNest = ullPortInterruptNesting[ xCore ];

        if( ullNest < 1 )
            xNestingBad = pdTRUE;

        /* If nesting depth > 1, this ISR preempted another ISR */
        if( ullNest > 1 )
            ulNestSeen++;

        uint64_t ullSP;
        __asm volatile ( "MOV %0, SP" : "=r" ( ullSP ) );
        if( ( ullSP & 0xFUL ) != 0 )
            xAlignOK = pdFALSE;

        ulTmr3Count++;
    }
}

static void prvDemo2Task( void *pv )
{
    ( void ) pv;

    sysprintf( "\r\n--- Demo 2: Interrupt Nesting Storm ---\r\n" );

    /* Reset counters. */
    ulTmr2Count = 0;
    ulTmr3Count = 0;
    ulNestSeen  = 0;
    xAlignOK    = pdTRUE;
    xNestingBad = pdFALSE;

    /* Enable clocks for TIMER2 and TIMER3. */
    SYS_UnlockReg();
    CLK_EnableModuleClock( TMR2_MODULE );
    CLK_EnableModuleClock( TMR3_MODULE );
    CLK_SetModuleClock( TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HXT, 0 );
    CLK_SetModuleClock( TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_HXT, 0 );
    SYS_LockReg();

    /* TIMER2: 1 MHz, lower urgency. */
    TIMER_Open( TIMER2, TIMER_PERIODIC_MODE, 1000000 );
    TIMER_EnableInt( TIMER2 );
    IRQ_SetHandler( ( IRQn_ID_t ) TMR2_IRQn, prvTmr2Handler );
    IRQ_SetPriority( ( IRQn_ID_t ) TMR2_IRQn, DEMO2_TMR2_GIC_PRIO );
    IRQ_Enable( ( IRQn_ID_t ) TMR2_IRQn );

    /* TIMER3: 1 MHz, higher urgency - can nest inside TIMER2's ISR. */
    TIMER_Open( TIMER3, TIMER_PERIODIC_MODE, 1000000 );
    TIMER_EnableInt( TIMER3 );
    IRQ_SetHandler( ( IRQn_ID_t ) TMR3_IRQn, prvTmr3Handler );
    IRQ_SetPriority( ( IRQn_ID_t ) TMR3_IRQn, DEMO2_TMR3_GIC_PRIO );
    IRQ_Enable( ( IRQn_ID_t ) TMR3_IRQn );

    /* Start both timers. */
    TIMER_Start( TIMER2 );
    TIMER_Start( TIMER3 );

    /* Let them run. */
    vTaskDelay( pdMS_TO_TICKS( DEMO2_RUN_MS ) );

    /* Stop and clean up. */
    TIMER_Stop( TIMER2 );
    TIMER_Stop( TIMER3 );
    TIMER_DisableInt( TIMER2 );
    TIMER_DisableInt( TIMER3 );
    IRQ_Disable( ( IRQn_ID_t ) TMR2_IRQn );
    IRQ_Disable( ( IRQn_ID_t ) TMR3_IRQn );

    /* --- Validation --- */
    BaseType_t xCore = ( BaseType_t ) portGET_CORE_ID();
    uint64_t ullFinalNest = ullPortInterruptNesting[ xCore ];

    sysprintf( "  [Demo2] TMR2 IRQs: %lu; distribution: Core 0 = %lu, Core 1 = %lu\r\n",
               ( unsigned long ) ulTmr2Count,
               ( unsigned long ) ulTmr2WhichCore[0],
               ( unsigned long ) ulTmr2WhichCore[1] );
    sysprintf( "  [Demo2] TMR3 IRQs: %lu; distribution: Core 0 = %lu, Core 1 = %lu\r\n",
               ( unsigned long ) ulTmr3Count,
               ( unsigned long ) ulTmr3WhichCore[0],
               ( unsigned long ) ulTmr3WhichCore[1] );
    sysprintf( "  [Demo2] Nesting observed (depth>1): %lu times\r\n",
               ( unsigned long ) ulNestSeen );
    sysprintf( "  [Demo2] Final ullPortInterruptNesting[%d] = %lu\r\n",
               ( int ) xCore, ( unsigned long ) ullFinalNest );

    /* Check 1: Both timers actually fired. */
    prvPrintResult( "Demo2-A  TIMER2 interrupts fired",
                    ( ulTmr2Count > 0 ) ? pdTRUE : pdFALSE );
    prvPrintResult( "Demo2-A  TIMER3 interrupts fired",
                    ( ulTmr3Count > 0 ) ? pdTRUE : pdFALSE );

    /* Check 2: ullPortInterruptNesting was >= 1 inside every ISR. */
    prvPrintResult( "Demo2-B  portCHECK_IF_IN_ISR() correct in ISRs",
                    ( xNestingBad == pdFALSE ) ? pdTRUE : pdFALSE );

    /* Check 3: nesting depth back to 0 in task context. */
    prvPrintResult( "Demo2-C  Nesting depth returned to 0 in task context",
                    ( ullFinalNest == 0 ) ? pdTRUE : pdFALSE );

    /* Check 4: SP stayed 16-byte aligned in all ISRs. */
    prvPrintResult( "Demo2-D  Stack pointer 16-byte aligned in ISRs", xAlignOK );

    /* Check 5: Actual nesting happened (advisory - depends on timing). */
    if( ulNestSeen > 0 )
        prvPrintResult( "Demo2-E  Genuine interrupt nesting observed", pdTRUE );
    else
        sysprintf( "  [NOTE] Demo2-E  No nesting observed (timing-dependent, not a failure)\r\n" );

    sysprintf( "--- Demo 2 complete ---\r\n" );
    xSemaphoreGive( xDemo2Done );
    vTaskDelete( NULL );
}

#endif /* DEMO2_IRQ_NESTING_STORM */

/*===========================================================================
 * DEMO 3 - Migration & Critical Section
 *
 * Goal: Verify that uxCriticalNesting travels correctly with the TCB and
 *       that spinlocks prevent Core 1 from entering a critical section
 *       held by Core 0.
 *
 * Strategy to guarantee migration:
 *   1. Task A (prio 3, unpinned) starts, records its core, enters a
 *      critical section, does work, exits the critical section.
 *   2. After the first critical section, Task A signals the orchestrator.
 *   3. The orchestrator pins a high-prio "evictor" task to Task A's
 *      original core, forcing the scheduler to migrate Task A to the
 *      other core.
 *   4. Task A enters a second critical section on the new core, does
 *      work, exits successfully.
 *   5. Task B (same prio) tries to enter a critical section concurrently,
 *      validating spinlock contention.
 *
 * Validation:
 *   - Task A started and ended on different cores.
 *   - Both critical sections completed without assertion or lost mask.
 *   - Task B acquired/released its critical section.
 *===========================================================================*/
#if ( DEMO3_MIGRATION_CRITICAL == 1 )

static volatile BaseType_t xD3_TaskAFirstCore    = -1;
static volatile BaseType_t xD3_TaskASecondCore   = -1;
static volatile BaseType_t xD3_TaskADone         = pdFALSE;
static volatile BaseType_t xD3_TaskBDone         = pdFALSE;
static volatile BaseType_t xD3_Crit1OK           = pdFALSE;
static volatile BaseType_t xD3_Crit2OK           = pdFALSE;
static volatile uint32_t   ulD3_SharedVar        = 0;
static SemaphoreHandle_t   xDemo3Done            = NULL;
static SemaphoreHandle_t   xD3_Phase1Done        = NULL;  /* Task A signals after 1st crit */
static SemaphoreHandle_t   xD3_MigrateReady      = NULL;  /* Orchestrator signals Task A to continue */
static SemaphoreHandle_t   xD3_EvictorEntered    = NULL;  /* Evictor signals it has entered critical section */
static SemaphoreHandle_t   xD3_MigrateDone       = NULL;  /* Task A signals migration done */

/* Busy-loop count for critical section work. */
#define DEMO3_CRIT_ITERS   500000UL

/* Evictor - high-prio task pinned to Task A's original core.
 * Its mere existence on that core forces the scheduler to move
 * lower-priority Task A to the other core. */
static void prvEvictorTask( void *pv )
{
    ( void ) pv;
    xSemaphoreGive( xD3_EvictorEntered );
    
    /* Wait until Task A signals it's done with migration. */
    for ( ;; )
    {
        vTaskDelay( pdMS_TO_TICKS( 10 ) );
    }
}

/* Task A - the migratable task.  No printing while it might migrate. */
static void prvMigTaskA( void *pv )
{
    ( void ) pv;
    volatile uint32_t ul;

    /* Phase 1: first critical section on original core. */
    xD3_TaskAFirstCore = ( BaseType_t ) portGET_CORE_ID();

    sysprintf( "  [Demo3] Task A started on core %d, entering 1st critical section\r\n",
               ( int ) xD3_TaskAFirstCore );

    taskENTER_CRITICAL();
    {
        ulD3_SharedVar = 0;
        for( ul = 0; ul < DEMO3_CRIT_ITERS; ul++ )
        {
            ulD3_SharedVar++;
        }
    }
    taskEXIT_CRITICAL();

    xD3_Crit1OK = ( ulD3_SharedVar == DEMO3_CRIT_ITERS ) ? pdTRUE : pdFALSE;

    /* Signal orchestrator: "I finished my first critical section." */
    xSemaphoreGive( xD3_Phase1Done );

    sysprintf( "  [Demo3] Task A completed 1st critical section on core %d, waiting for migration\r\n",
               ( int ) xD3_TaskAFirstCore );

    /* Wait for the orchestrator to force our migration. */
    xSemaphoreTake( xD3_MigrateReady, portMAX_DELAY );

    while (portGET_CORE_ID() == xD3_TaskAFirstCore )
    {
        /* Wait until we are actually migrated to the other core. */
        vTaskDelay( pdMS_TO_TICKS( 10 ) );
    }
    xD3_TaskASecondCore = ( BaseType_t ) portGET_CORE_ID();

    /* Let orchestrator know migration should be done now. */
    xSemaphoreGive( xD3_MigrateDone );

    sysprintf( "  [Demo3] Task A resumed on core %d, entering 2nd critical section\r\n", 
                ( int ) xD3_TaskASecondCore );

    /* Phase 2: second critical section - should now be on the other core. */

    taskENTER_CRITICAL();
    {
        ulD3_SharedVar = 0;
        for( ul = 0; ul < DEMO3_CRIT_ITERS; ul++ )
        {
            ulD3_SharedVar++;
        }
    }
    taskEXIT_CRITICAL();

    xD3_Crit2OK = ( ulD3_SharedVar == DEMO3_CRIT_ITERS ) ? pdTRUE : pdFALSE;
    xD3_TaskADone = pdTRUE;
    vTaskDelete( NULL );
}

/* Task B - contends for the critical section from another core. */
static void prvMigTaskB( void *pv )
{
    ( void ) pv;

    /* Wait a bit so Task A is inside its first critical section. */
    vTaskDelay( pdMS_TO_TICKS( 5 ) );

    sysprintf( "  [Demo3] Task B on core %d trying to enter critical section\r\n",
               ( int ) portGET_CORE_ID() );

    taskENTER_CRITICAL();
    {
        /* Just verify we got in. */
        volatile uint32_t dummy = 42;
        ( void ) dummy;
    }
    taskEXIT_CRITICAL();

    sysprintf( "  [Demo3] Task B on core %d entered and exited critical section\r\n",
               ( int ) portGET_CORE_ID() );

    xD3_TaskBDone = pdTRUE;
    vTaskDelete( NULL );
}

static void prvDemo3Task( void *pv )
{
    ( void ) pv;
    TaskHandle_t xTaskAHandle = NULL;

    sysprintf( "\r\n--- Demo 3: Migration & Critical Section ---\r\n" );

    xD3_TaskADone      = pdFALSE;
    xD3_TaskBDone      = pdFALSE;
    xD3_Crit1OK        = pdFALSE;
    xD3_Crit2OK        = pdFALSE;
    xD3_TaskAFirstCore = -1;
    xD3_TaskASecondCore= -1;
    ulD3_SharedVar     = 0;

    /* Create Task A (prio 3, unpinned). */
    xTaskCreate( prvMigTaskA, "MigA", DEMO_STACK_SIZE, NULL,
                 tskIDLE_PRIORITY + 3, &xTaskAHandle );
    configASSERT( xTaskAHandle );

    /* Create Task B (prio 3, unpinned) - will contend for critical section. */
    xTaskCreate( prvMigTaskB, "MigB", DEMO_STACK_SIZE, NULL,
                 tskIDLE_PRIORITY + 3, NULL );

    /* Wait for Task A to finish its first critical section. */
    xSemaphoreTake( xD3_Phase1Done, portMAX_DELAY );

    /* Now force migration: pin a high-priority evictor to Task A's
     * original core.  Task A (prio 3, blocked on xD3_MigrateReady) will
     * be migrated to the other core when it becomes ready again. */
    BaseType_t xOrigCore = xD3_TaskAFirstCore;
    TaskHandle_t xEvH = NULL;
    xTaskCreateAffinitySet( prvEvictorTask, "Evict", DEMO_STACK_SIZE, NULL,
                 tskIDLE_PRIORITY + 6, ( 1U << xOrigCore ), &xEvH );
    configASSERT( xEvH );
    vTaskCoreAffinitySet( xEvH, ( 1U << xOrigCore ) );

    /* Wait for evictor occupies the core. */
    xSemaphoreTake( xD3_EvictorEntered, portMAX_DELAY );

    /* Signal Task A to continue - it should wake on the other core. */
    xSemaphoreGive( xD3_MigrateReady );

    vTaskDelay( pdMS_TO_TICKS( 100 ) );

    /* Wait until Task A signals it's done with migration. */
    xSemaphoreTake( xD3_MigrateDone, portMAX_DELAY ); 
    vTaskDelete( xEvH );

    /* Wait for everything to settle. */
    vTaskDelay( pdMS_TO_TICKS( 2000 ) );

    /* --- Validation --- */
    sysprintf( "  [Demo3] Task A: core %d -> core %d\r\n",
               ( int ) xD3_TaskAFirstCore, ( int ) xD3_TaskASecondCore );

    prvPrintResult( "Demo3-A  First critical section completed",  xD3_Crit1OK );
    prvPrintResult( "Demo3-B  Second critical section completed", xD3_Crit2OK );
    prvPrintResult( "Demo3-C  Task A exited both critical sections",
                    xD3_TaskADone );
    prvPrintResult( "Demo3-D  Task B acquired/released critical section",
                    xD3_TaskBDone );
    prvPrintResult( "Demo3-E  Task A migrated across cores",
                    ( xD3_TaskAFirstCore != xD3_TaskASecondCore &&
                      xD3_TaskAFirstCore >= 0 &&
                      xD3_TaskASecondCore >= 0 ) ? pdTRUE : pdFALSE );

    sysprintf( "--- Demo 3 complete ---\r\n" );
    xSemaphoreGive( xDemo3Done );
    vTaskDelete( NULL );
}

#endif /* DEMO3_MIGRATION_CRITICAL */

/*===========================================================================
 * DEMO 4 - FPU Task Pinned to a Specific Core
 *
 * Tests FPU context correctness when a higher-priority disturber task
 * preempts an FPU task on the SAME core.  Both tasks call vPortTaskUsesFPU()
 * and perform floating-point operations.
 *
 * Test flow:
 *   1. FPU task (lower priority) continuously computes sin^2+cos^2 == 1
 *   2. Disturber task (higher priority, same core) periodically preempts it
 *   3. Disturber trashes FPU registers with a distinctive pattern
 *   4. When FPU task resumes, its FPU registers must be correctly restored
 *      - If broken, sin^2+cos^2 will not equal 1
 *
 * A counter tracks how many times the disturber ran (preemptions).
 *===========================================================================*/
#if ( DEMO4_FPU_PINNED == 1 )

#define FPU_PIN_CORE        1
#define FPU_ITERATIONS      100000
#define FPU_EPSILON         ( 1.0e-6 )

static volatile BaseType_t xFPUPass = pdFALSE;
static volatile BaseType_t xFPUCore = -1;
static volatile uint32_t   ulDisturbCount = 0;   /* times disturber ran (preemptions) */
static volatile BaseType_t xFPUTaskRunning = pdFALSE; /* flag so disturber knows when to stop */
static SemaphoreHandle_t   xDemo4Done = NULL;

static void prvFPUTask( void *pv )
{
    ( void ) pv;

    vPortTaskUsesFPU();

    sysprintf( "\r\n--- Demo 4: FPU Task Pinned to Core %d ---\r\n",
                ( int ) portGET_CORE_ID() );

    BaseType_t xOK = pdTRUE;
    uint32_t i;

    xFPUTaskRunning = pdTRUE;

    for( i = 0; i < FPU_ITERATIONS; i++ )
    {
        double x = ( double ) i * 0.1;
        double s = sin( x );
        double c = cos( x );
        double r = ( s * s ) + ( c * c );

        if( fabs( r - 1.0 ) > FPU_EPSILON )
        {
            sysprintf( "  [Demo4] FPU ERROR i=%lu  sin^2+cos^2 = %d.%06d\r\n",
                       ( unsigned long ) i,
                       ( int ) r,
                       ( int ) ( ( r - ( int ) r ) * 1000000.0 ) );
            xOK = pdFALSE;
            break;
        }
    }

    xFPUTaskRunning = pdFALSE;
    xFPUCore = ( BaseType_t ) portGET_CORE_ID();
    xFPUPass = xOK;

    sysprintf( "  [Demo4] FPU task done on core %d  iters=%lu  pass=%s\r\n",
               ( int ) xFPUCore, ( unsigned long ) i,
               xOK ? "yes" : "no" );
    sysprintf( "  [Demo4] Disturber preempted FPU task %lu times\r\n",
               ( unsigned long ) ulDisturbCount );

    prvPrintResult( "Demo4-A  FPU pinned sin^2+cos^2 identity", xFPUPass );
    prvPrintResult( "Demo4-B  Disturber actually preempted FPU task",
                    ( ulDisturbCount > 0 ) ? pdTRUE : pdFALSE );

    sysprintf( "--- Demo 4 complete ---\r\n" );

    xSemaphoreGive( xDemo4Done );
    vTaskDelete( NULL );
}

/* Disturber: HIGHER priority than FPU task, pinned to SAME core.
 * Immediately runs when FPU task yields, trashes FPU registers with a
 * distinctive pattern, then yields back.  Each run = one preemption. */
static void prvFPUDisturbTask( void *pv )
{
    ( void ) pv;
    vPortTaskUsesFPU();

    while( xFPUTaskRunning == pdFALSE )
    {
        /* Wait for FPU task to start. */
        vTaskDelay( pdMS_TO_TICKS( 1 ) );
    }

    while( xFPUTaskRunning != pdFALSE )
    {
        /* Write a distinctive pattern into FPU registers.  If the port
         * fails to save/restore the FPU task's context, these values
         * will corrupt its sin/cos computation. */
        volatile double d = 0.0;
        uint32_t j;
        for( j = 0; j < 50; j++ )
        {
            d += 99999.0 * sin( ( double ) j * 3.14 );
        }
        ( void ) d;

        ulDisturbCount++;

        /* A small delay to allow FPU task to run. */
        vTaskDelay( pdMS_TO_TICKS( 1 ) );
    }

    vTaskDelete( NULL );
}

#endif /* DEMO4_FPU_PINNED */

/*===========================================================================
 * DEMO 5 - Cross-Core Ping-Pong (SGI / Yield Test)
 *
 * Setup:
 *   - Task on Core 0 (sender) calls xTaskNotifyGive() to wake a
 *     high-priority task pinned to Core 1 (receiver).
 *   - The kernel internally calls portYIELD_CORE(1) which sends SGI0
 *     to Core 1.
 *   - The receiver increments a counter and notifies back.
 *===========================================================================*/
#if ( DEMO5_PINGPONG == 1 )

/* Print status every this many rounds. */
#define DEMO5_PRINT_INTERVAL    100

static volatile uint32_t   ulPingCount     = 0;
static volatile uint32_t   ulPongCount     = 0;
static volatile BaseType_t xReceiverCore   = -1;
static TaskHandle_t        xSenderHandle   = NULL;
static TaskHandle_t        xReceiverHandle = NULL;

/* Receiver - pinned to core 1.  Waits for notification from sender. */
static void prvReceiverTask( void *pv )
{
    ( void ) pv;

    for( ;; )
    {
        /* Block until sender notifies us. */
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

        xReceiverCore = ( BaseType_t ) portGET_CORE_ID();
        ulPongCount++;

        /* Notify sender back. */
        xTaskNotifyGive( xSenderHandle );
    }
}

/* Sender - pinned to core 0. */
static void prvSenderTask( void *pv )
{
    ( void ) pv;
    uint32_t ulLastPrint = 0;

    sysprintf( "\r\n--- Demo 5: Cross-Core Ping-Pong ---\r\n" );

    for( ;; )
    {
        /* Wake the receiver on core 1. */
        xTaskNotifyGive( xReceiverHandle );

        ulPingCount++;

        /* Wait for receiver to reply. */
        ulTaskNotifyTake( pdTRUE, pdMS_TO_TICKS( 1000 ) );

        /* Periodic status print. */
        if( ( ulPingCount - ulLastPrint ) >= DEMO5_PRINT_INTERVAL )
        {
            sysprintf( "  pings=%lu [sender %d], pongs=%lu [receiver %d]\r\n",
                       ( unsigned long ) ulPingCount,
                       ( int ) portGET_CORE_ID(),
                       ( unsigned long ) ulPongCount,
                       ( int ) xReceiverCore );
            ulLastPrint = ulPingCount;
        }

        vTaskDelay( pdMS_TO_TICKS( 10 ) );
    }
}

#endif /* DEMO5_PINGPONG */

/*===========================================================================
 * Result collector - waits for all enabled demos, prints summary.
 *===========================================================================*/

static void prvResultCollector( void *pv )
{
    ( void ) pv;

#if ( DEMO1_CRITICAL_AND_ISR == 1 )
    xSemaphoreTake( xDemo1Done, portMAX_DELAY );
#endif
#if ( DEMO2_IRQ_NESTING_STORM == 1 )
    xSemaphoreTake( xDemo2Done, portMAX_DELAY );
#endif
#if ( DEMO3_MIGRATION_CRITICAL == 1 )
    xSemaphoreTake( xDemo3Done, portMAX_DELAY );
#endif
#if ( DEMO4_FPU_PINNED == 1 )
    xSemaphoreTake( xDemo4Done, portMAX_DELAY );
#endif

    sysprintf( "\r\n" );
    sysprintf( "+-----------------------------------------------+\r\n" );
    sysprintf( "|  All SMP Scheduling Demos Finished.           |\r\n" );
    sysprintf( "+-----------------------------------------------+\r\n\r\n" );

    vTaskDelete( NULL );
}

/*===========================================================================
 * scheduling demo
 * mainSELECTED_APPLICATION == 1.
 *
 * Creates tasks for all selected demos and starts the scheduler.
 *===========================================================================*/

void main_scheduling_demo( void )
{
    sysprintf( "\r\n" );
    sysprintf( "+-----------------------------------------------+\r\n" );
    sysprintf( "|  MA35 FreeRTOS-SMP Scheduling Demos           |\r\n" );
    sysprintf( "+-----------------------------------------------+\r\n" );

    /* ---- Demo 1: Critical + ISR ---- */
#if ( DEMO1_CRITICAL_AND_ISR == 1 )
    xISRSem      = xSemaphoreCreateBinary();
    xDemo1Done   = xSemaphoreCreateBinary();
    xPreemptDone = xSemaphoreCreateBinary();
    configASSERT( xISRSem && xDemo1Done && xPreemptDone );
    xISRTimer  = xTimerCreate( "D1Tmr", pdMS_TO_TICKS( 100 ),
                                pdFALSE, NULL, prvISRTimerCallback );
    configASSERT( xISRTimer );
    xTaskCreate( prvDemo1Task, "D1", DEMO_STACK_SIZE, NULL,
                 tskIDLE_PRIORITY + 3, NULL );
#endif

    /* ---- Demo 2: IRQ Nesting Storm ---- */
#if ( DEMO2_IRQ_NESTING_STORM == 1 )
    xDemo2Done = xSemaphoreCreateBinary();
    configASSERT( xDemo2Done );
    xTaskCreate( prvDemo2Task, "D2", DEMO_STACK_SIZE * 2, NULL,
                 tskIDLE_PRIORITY + 2, NULL );
#endif

    /* ---- Demo 3: Migration ---- */
#if ( DEMO3_MIGRATION_CRITICAL == 1 )
    xDemo3Done    = xSemaphoreCreateBinary();
    xD3_Phase1Done = xSemaphoreCreateBinary();
    xD3_MigrateReady = xSemaphoreCreateBinary();
    xD3_EvictorEntered = xSemaphoreCreateBinary();
    xD3_MigrateDone = xSemaphoreCreateBinary();
    configASSERT( xDemo3Done && xD3_Phase1Done && xD3_MigrateReady && xD3_MigrateDone );
    xTaskCreate( prvDemo3Task, "D3", DEMO_STACK_SIZE, NULL,
                 tskIDLE_PRIORITY + 5, NULL );
#endif

    /* ---- Demo 4: FPU Pinned ---- */
#if ( DEMO4_FPU_PINNED == 1 )
    {
        xDemo4Done = xSemaphoreCreateBinary();
        configASSERT( xDemo4Done );

        ulDisturbCount = 0;
        xFPUTaskRunning = pdFALSE;

        TaskHandle_t xFPUHandle = NULL;
        xTaskCreate( prvFPUTask, "FPU", DEMO_STACK_SIZE * 2, NULL,
                     tskIDLE_PRIORITY + 4, &xFPUHandle );
        configASSERT( xFPUHandle );
        vTaskCoreAffinitySet( xFPUHandle, ( 1U << FPU_PIN_CORE ) );

        /* Disturber: SAME core, HIGHER priority - guarantees preemption. */
        TaskHandle_t xDistHandle = NULL;
        xTaskCreate( prvFPUDisturbTask, "Dist", DEMO_STACK_SIZE * 2, NULL,
                     tskIDLE_PRIORITY + 5, &xDistHandle );
        configASSERT( xDistHandle );
        vTaskCoreAffinitySet( xDistHandle, ( 1U << FPU_PIN_CORE ) );
    }
#endif

    /* ---- Demo 5: Cross-Core Ping-Pong ---- */
#if ( DEMO5_PINGPONG == 1 )
    {
        ulPingCount   = 0;
        ulPongCount   = 0;
        xReceiverCore = -1;

        /* Create receiver first (so handle is valid when sender starts). */
        xTaskCreate( prvReceiverTask, "Recv", DEMO_STACK_SIZE, NULL,
                     tskIDLE_PRIORITY + 4, &xReceiverHandle );
        configASSERT( xReceiverHandle );
        vTaskCoreAffinitySet( xReceiverHandle, ( 1U << 1 ) );

        xTaskCreate( prvSenderTask, "Send", DEMO_STACK_SIZE, NULL,
                     tskIDLE_PRIORITY + 4, &xSenderHandle );
        configASSERT( xSenderHandle );
        vTaskCoreAffinitySet( xSenderHandle, ( 1U << 0 ) );
    }
#endif

    /* ---- Result collector (lowest demo priority, waits for all) ---- */
    xTaskCreate( prvResultCollector, "Res", DEMO_STACK_SIZE, NULL,
                 tskIDLE_PRIORITY + 1, NULL );

    /* Start the scheduler - never returns. */
    vTaskStartScheduler();
}

/*===========================================================================
 * ping-pong demo
 * mainSELECTED_APPLICATION == 0.
 *
 * Creates the sender/receiver tasks and starts the scheduler.
 *===========================================================================*/
#if ( DEMO5_PINGPONG == 1 )

void main_pingpong_demo( void )
{
    sysprintf( "+-----------------------------------------------+\r\n" );
    sysprintf( "|  MA35 FreeRTOS-SMP Cross-Core Ping-Pong       |\r\n" );
    sysprintf( "+-----------------------------------------------+\r\n" );

    ulPingCount   = 0;
    ulPongCount   = 0;
    xReceiverCore = -1;

    /* Create receiver first (so handle is valid when sender starts). */
    xTaskCreate( prvReceiverTask, "Recv", DEMO_STACK_SIZE, NULL,
                 tskIDLE_PRIORITY + 4, &xReceiverHandle );
    configASSERT( xReceiverHandle );
    vTaskCoreAffinitySet( xReceiverHandle, ( 1U << 1 ) );

    xTaskCreate( prvSenderTask, "Send", DEMO_STACK_SIZE, NULL,
                 tskIDLE_PRIORITY + 4, &xSenderHandle );
    configASSERT( xSenderHandle );
    vTaskCoreAffinitySet( xSenderHandle, ( 1U << 0 ) );

    /* Start the scheduler - never returns. */
    vTaskStartScheduler();
}

#endif /* DEMO5_PINGPONG */

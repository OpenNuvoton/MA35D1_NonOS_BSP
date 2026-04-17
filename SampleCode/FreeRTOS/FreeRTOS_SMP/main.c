/**************************************************************************//**
 * @file     main.c
 *
 * @brief    FreeRTOS-SMP project for dual-core MA35 (Cortex-A35).
 *           Core 0 starts the scheduler; core 1 enters via main1().
 *
 * @note     The ARMv8 Generic Timer (CNTP, PPI 30) is used for the tick.
 *           SGI0 is used for inter-core yield signalling (IPI).
 *
 * @copyright (C) 2026 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include "NuMicro.h"
#include "FreeRTOS.h"
#include "task.h"

/* mainSELECTED_APPLICATION is used to select between two demo applications,
 * as described at the top of this file.
 *
 * When mainSELECTED_APPLICATION is set to 0 the simple ping-pong example will
 * be run.
 *
 * When mainSELECTED_APPLICATION is set to 1 the SMP tasking verification
 * demo will be run (critical sections, ISR nesting, multi-core scheduling,
 * FPU affinity).
 */
#define mainSELECTED_APPLICATION	1

/*
 * See the comments at the top of this file and above the
 * mainSELECTED_APPLICATION definition.
 */
#if ( mainSELECTED_APPLICATION == 0 )
extern void main_pingpong_demo( void );
#elif ( mainSELECTED_APPLICATION == 1 )
extern void main_scheduling_demo( void );
#else
#error Invalid mainSELECTED_APPLICATION setting.
#endif

/*-----------------------------------------------------------*/
/* External references needed for secondary core boot.       */
/*-----------------------------------------------------------*/

/* The per-core TCB array from the kernel (tasks.c).
 * Core 1 waits for pxCurrentTCBs[1] != NULL before entering the
 * scheduler, ensuring the SMP kernel has assigned a task (idle) to it. */
extern void * volatile pxCurrentTCBs[];

/* Safe printf initialization */
extern void vSafePrintfInit(void);

/* Assembly entry point: installs FreeRTOS vector table and restores
 * the first task context for the calling core. */
extern void vPortRestoreTaskContext( void );

/* Boot secondary core. */
extern void RunCore1( void );

/*-----------------------------------------------------------*/
/* Secondary core (core 1) entry point.                      */
/*-----------------------------------------------------------*/
void main1( void )
{
    /* Install SGI0 yield handler on core 1. */
    IRQ_SetHandler( (IRQn_ID_t)portYIELD_SGIn, vSGIYieldHandler );
    IRQ_SetPriority( (IRQn_ID_t)portYIELD_SGIn,
                    configMAX_API_CALL_INTERRUPT_PRIORITY << portPRIORITY_SHIFT );
    IRQ_Enable( (IRQn_ID_t)portYIELD_SGIn );

    /* Spin until the SMP scheduler is fully running. */
    while( ( pxCurrentTCBs[ 1 ] == NULL ) ||
            ( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ) )
    {
        __asm volatile ( "yield" );
    }

    /* Ensure we see all writes from core 0 (pxCurrentTCBs, ready lists,
     * xSchedulerRunning, etc.) before we proceed. */
    __asm volatile ( "DSB SY" ::: "memory" );
    __asm volatile ( "ISB SY" );

    /* Disable interrupts before entering the scheduler.The ERET into
     * the first task will restore SPSR_EL3 which has interrupts unmasked,
     * so they get re-enabled atomically when the task starts running. */
    ( void ) portDISABLE_INTERRUPTS();

    /* Enter the scheduler. */
    vPortRestoreTaskContext();

    /* Should never reach here. */
    for( ;; );
}

void UART0_Init()
{
    /* Enable UART0 clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Set multi-function pins */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);
}

void SYS_Init()
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Init UART for sysprintf */
    UART0_Init();

    /* Lock protected registers */
    SYS_LockReg();
}

/*-----------------------------------------------------------*/
/* Primary core (core 0) entry point.                        */
/*-----------------------------------------------------------*/
int main(void)
{
    SYS_Init();

    /* Create the recursive mutex used by the thread-safe sysprintf().
     * Must be called before any FreeRTOS task calls sysprintf(). */
    vSafePrintfInit();

    sysprintf("\nCPU @ %d Hz\n", SystemCoreClock);
    sysprintf("FreeRTOS-SMP starting on %d cores\n", configNUMBER_OF_CORES);

    /* Install SGI0 yield handler on core 0. */
    IRQ_SetHandler( (IRQn_ID_t)portYIELD_SGIn, vSGIYieldHandler );
    IRQ_SetPriority( (IRQn_ID_t)portYIELD_SGIn,
                     configMAX_API_CALL_INTERRUPT_PRIORITY << portPRIORITY_SHIFT );
    IRQ_Enable( (IRQn_ID_t)portYIELD_SGIn );

    /* Boot core 1 */
    RunCore1();

#if( mainSELECTED_APPLICATION == 0 )
    {
        main_pingpong_demo();
    }
#elif( mainSELECTED_APPLICATION == 1 )
    {
        main_scheduling_demo();
    }
#endif

    /* Should never be reached */
    return 0;
}

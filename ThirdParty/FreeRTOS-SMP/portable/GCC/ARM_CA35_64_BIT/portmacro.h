/*
 * @copyright (C) 2026 Nuvoton Technology Corp. All rights reserved.
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

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
	extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the given hardware
 * and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	size_t
#define portBASE_TYPE	long

typedef portSTACK_TYPE StackType_t;
typedef portBASE_TYPE BaseType_t;
typedef uint64_t UBaseType_t;

typedef uint64_t TickType_t;
#define portMAX_DELAY ( ( TickType_t ) 0xffffffffffffffff )

/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
not need to be guarded with a critical section. */
#define portTICK_TYPE_IS_ATOMIC 1

/*-----------------------------------------------------------*/

/* Hardware specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT			16
#define portPOINTER_SIZE_TYPE 		uint64_t

/*-----------------------------------------------------------*/

/* SMP support. */

/* Get the current core ID from MPIDR_EL1.
 * Must be a #define macro so that FreeRTOS.h's #ifndef portGET_CORE_ID check
 * passes.  The inline function provides the actual implementation. */
static inline BaseType_t xPortGetCoreID( void )
{
	uint64_t mpidr;
	__asm volatile ( "MRS %0, MPIDR_EL1" : "=r" ( mpidr ) ); // cpuid()
	return ( BaseType_t ) ( mpidr & 0xFFUL );
}
#define portGET_CORE_ID()	xPortGetCoreID()

/* SGI used for inter-core yield. We use SGI0 for FreeRTOS yield. */
#define portYIELD_SGIn	( SGI0_IRQn )

/* SGI handler for inter-core yield */
void vSGIYieldHandler( void );

/* Yield a specific core by sending an SGI (Software Generated Interrupt). */
extern void vPortYieldCore( BaseType_t xCoreID );
#define portYIELD_CORE( xCoreID )	vPortYieldCore( xCoreID )

/* Check if we are in an ISR context. */
extern uint64_t ullPortInterruptNesting[];
#define portCHECK_IF_IN_ISR()		( ullPortInterruptNesting[ portGET_CORE_ID() ] > 0 )

/* SMP recursive spinlock for task and ISR level critical sections.
 *
 * The FreeRTOS SMP kernel expects port lock functions to be RECURSIVE
 * (re-entrant from the same core).  vPortRecursiveLock() tracks per-lock
 * ownership and recursion count, matching the ARM_CR82 reference SMP port.
 *
 * Lock indices must match the eLockType_t enum in port.c. */
typedef enum
{
	eLockISR = 0,
	eLockTask,
	eLockCount
} eLockType_t;

extern void vPortRecursiveLock( uint32_t ulCoreID,
								eLockType_t eLock,
								BaseType_t xAcquire );

#define portGET_ISR_LOCK( xCoreID )			vPortRecursiveLock( ( uint32_t ) ( xCoreID ), eLockISR, pdTRUE )
#define portRELEASE_ISR_LOCK( xCoreID )		vPortRecursiveLock( ( uint32_t ) ( xCoreID ), eLockISR, pdFALSE )
#define portGET_TASK_LOCK( xCoreID )		vPortRecursiveLock( ( uint32_t ) ( xCoreID ), eLockTask, pdTRUE )
#define portRELEASE_TASK_LOCK( xCoreID )	vPortRecursiveLock( ( uint32_t ) ( xCoreID ), eLockTask, pdFALSE )

/*-----------------------------------------------------------*/

/* Task utilities. */

/* Called at the end of an ISR that can cause a context switch. */
#define portEND_SWITCHING_ISR( xSwitchRequired )	\
{													\
extern uint64_t ullPortYieldRequired[];				\
													\
	if( xSwitchRequired != pdFALSE )				\
	{												\
		ullPortYieldRequired[ portGET_CORE_ID() ] = pdTRUE;	\
	}												\
}

#define portYIELD_FROM_ISR( x ) portEND_SWITCHING_ISR( x )
#if defined( GUEST )
	#define portYIELD() __asm volatile ( "SVC 0" ::: "memory" )
#else
	#define portYIELD() __asm volatile ( "SMC 0" ::: "memory" )
#endif
/*-----------------------------------------------------------
 * Critical section control
 *----------------------------------------------------------*/

extern UBaseType_t uxPortSetInterruptMask( void );
extern void vPortClearInterruptMask( UBaseType_t uxNewMaskValue );

/*
 * SMP-compatible portDISABLE_INTERRUPTS: saves and returns the DAIF state
 * so it can be restored later with portRESTORE_INTERRUPTS.
 */
static inline UBaseType_t portDISABLE_INTERRUPTS( void )
{
	UBaseType_t ulState;
	__asm volatile ( "MRS %0, DAIF" : "=r" ( ulState ) :: "memory" );
	__asm volatile ( "MSR DAIFSET, #2" ::: "memory" );
	__asm volatile ( "DSB SY" );
	__asm volatile ( "ISB SY" );
	return ulState;
}

static inline void portENABLE_INTERRUPTS( void )
{
	__asm volatile ( "MSR DAIFCLR, #2" ::: "memory" );
	__asm volatile ( "DSB SY" );
	__asm volatile ( "ISB SY" );
}

#define portRESTORE_INTERRUPTS( ulState )							\
{																	\
	__asm volatile ( "MSR DAIF, %0" :: "r" ( ulState ) : "memory" );\
	__asm volatile ( "ISB" ::: "memory" );							\
}

/* On SMP the kernel provides vTaskEnterCritical / vTaskExitCritical
 * (in tasks.c) which implement the full SMP lock protocol and track
 * nesting in the TCB (portCRITICAL_NESTING_IN_TCB == 1).  The port's
 * vPortEnterCritical only masks the GIC priority - it does NOT acquire
 * SMP locks and must not be used as portENTER_CRITICAL. */
extern void vTaskEnterCritical( void );
extern void vTaskExitCritical( void );
#define portENTER_CRITICAL()		vTaskEnterCritical()
#define portEXIT_CRITICAL()			vTaskExitCritical()
#define portSET_INTERRUPT_MASK_FROM_ISR()		uxPortSetInterruptMask()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)	vPortClearInterruptMask(x)

/* SMP-required interrupt mask macros (FreeRTOS v11.1+).
 * portSET_INTERRUPT_MASK must save and return a value that can later be
 * passed to portCLEAR_INTERRUPT_MASK to restore the previous state.
 * Our portDISABLE_INTERRUPTS() already returns the DAIF state. */
#define portSET_INTERRUPT_MASK()				portDISABLE_INTERRUPTS()
#define portCLEAR_INTERRUPT_MASK( x )			portRESTORE_INTERRUPTS( x )

/* SMP-required ISR critical section macros (FreeRTOS v11.1+).
 * These delegate to the kernel's SMP-aware critical section management
 * which acquires the ISR spinlock and tracks nesting. */
extern UBaseType_t vTaskEnterCriticalFromISR( void );
extern void vTaskExitCriticalFromISR( UBaseType_t uxSavedInterruptStatus );
#define portENTER_CRITICAL_FROM_ISR()			vTaskEnterCriticalFromISR()
#define portEXIT_CRITICAL_FROM_ISR( x )			vTaskExitCriticalFromISR( x )

/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site.  These are
not required for this port but included in case common demo code that uses these
macros is used. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters )	void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters )	void vFunction( void *pvParameters )

/* Prototype of the FreeRTOS tick handler.  This must be installed as the
handler for whichever peripheral is used to generate the RTOS tick. */
void FreeRTOS_Tick_Handler( void );

/* Any task that uses the floating point unit MUST call vPortTaskUsesFPU()
before any floating point instructions are executed. */
void vPortTaskUsesFPU( void );
#define portTASK_USES_FLOATING_POINT() vPortTaskUsesFPU()

#define portLOWEST_INTERRUPT_PRIORITY ( ( ( uint32_t ) configUNIQUE_INTERRUPT_PRIORITIES ) - 1UL )
#define portLOWEST_USABLE_INTERRUPT_PRIORITY ( portLOWEST_INTERRUPT_PRIORITY - 1UL )

/* Architecture specific optimisations. */
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
	#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#endif

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1

	/* Store/clear the ready priorities in a bit map. */
	#define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
	#define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

	/*-----------------------------------------------------------*/

	#define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31 - __builtin_clz( uxReadyPriorities ) )

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

#ifdef configASSERT
	void vPortValidateInterruptPriority( void );
	#define portASSERT_IF_INTERRUPT_PRIORITY_INVALID() 	vPortValidateInterruptPriority()
#endif /* configASSERT */

#define portNOP() __asm volatile( "NOP" )
#define portINLINE __inline

#ifdef __cplusplus
	} /* extern C */
#endif


/* The number of bits to shift for an interrupt priority is dependent on the
number of bits implemented by the interrupt controller. */
#if configUNIQUE_INTERRUPT_PRIORITIES == 16
	#define portPRIORITY_SHIFT 4
	#define portMAX_BINARY_POINT_VALUE	3
#elif configUNIQUE_INTERRUPT_PRIORITIES == 32
	#define portPRIORITY_SHIFT 3
	#define portMAX_BINARY_POINT_VALUE	2
#elif configUNIQUE_INTERRUPT_PRIORITIES == 64
	#define portPRIORITY_SHIFT 2
	#define portMAX_BINARY_POINT_VALUE	1
#elif configUNIQUE_INTERRUPT_PRIORITIES == 128
	#define portPRIORITY_SHIFT 1
	#define portMAX_BINARY_POINT_VALUE	0
#elif configUNIQUE_INTERRUPT_PRIORITIES == 256
	#define portPRIORITY_SHIFT 0
	#define portMAX_BINARY_POINT_VALUE	0
#else
	#error Invalid configUNIQUE_INTERRUPT_PRIORITIES setting.  configUNIQUE_INTERRUPT_PRIORITIES must be set to the number of unique priorities implemented by the target hardware
#endif

/* Interrupt controller access addresses. */
#define portICCPMR_PRIORITY_MASK_OFFSET  						( 0x04 )
#define portICCIAR_INTERRUPT_ACKNOWLEDGE_OFFSET 				( 0x0C )
#define portICCEOIR_END_OF_INTERRUPT_OFFSET 					( 0x10 )
#define portICCBPR_BINARY_POINT_OFFSET							( 0x08 )
#define portICCRPR_RUNNING_PRIORITY_OFFSET						( 0x14 )

#define portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS 		( configINTERRUPT_CONTROLLER_BASE_ADDRESS + configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET )
#define portICCPMR_PRIORITY_MASK_REGISTER 					( *( ( volatile uint32_t * ) ( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCPMR_PRIORITY_MASK_OFFSET ) ) )
#define portICCIAR_INTERRUPT_ACKNOWLEDGE_REGISTER_ADDRESS 	( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCIAR_INTERRUPT_ACKNOWLEDGE_OFFSET )
#define portICCEOIR_END_OF_INTERRUPT_REGISTER_ADDRESS 		( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCEOIR_END_OF_INTERRUPT_OFFSET )
#define portICCPMR_PRIORITY_MASK_REGISTER_ADDRESS 			( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCPMR_PRIORITY_MASK_OFFSET )
#define portICCBPR_BINARY_POINT_REGISTER 					( *( ( const volatile uint32_t * ) ( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCBPR_BINARY_POINT_OFFSET ) ) )
#define portICCRPR_RUNNING_PRIORITY_REGISTER 				( *( ( const volatile uint32_t * ) ( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCRPR_RUNNING_PRIORITY_OFFSET ) ) )

#define portMEMORY_BARRIER() __asm volatile( "" ::: "memory" )

#endif /* PORTMACRO_H */

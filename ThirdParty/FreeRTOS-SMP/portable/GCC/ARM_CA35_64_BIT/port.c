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

/* Standard includes. */
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* BSP includes for SMP support. */
#include "cpu.h"
#include "core_ca.h"

#ifndef configINTERRUPT_CONTROLLER_BASE_ADDRESS
	#error configINTERRUPT_CONTROLLER_BASE_ADDRESS must be defined.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-Cortex-A-Embedded-Processors.html
#endif

#ifndef configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET
	#error configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET must be defined.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-Cortex-A-Embedded-Processors.html
#endif

#ifndef configUNIQUE_INTERRUPT_PRIORITIES
	#error configUNIQUE_INTERRUPT_PRIORITIES must be defined.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-Cortex-A-Embedded-Processors.html
#endif

#ifndef configSETUP_TICK_INTERRUPT
	#error configSETUP_TICK_INTERRUPT() must be defined.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-Cortex-A-Embedded-Processors.html
#endif /* configSETUP_TICK_INTERRUPT */

#ifndef configMAX_API_CALL_INTERRUPT_PRIORITY
	#error configMAX_API_CALL_INTERRUPT_PRIORITY must be defined.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-Cortex-A-Embedded-Processors.html
#endif

#if configMAX_API_CALL_INTERRUPT_PRIORITY == 0
	#error configMAX_API_CALL_INTERRUPT_PRIORITY must not be set to 0
#endif

#if configMAX_API_CALL_INTERRUPT_PRIORITY > configUNIQUE_INTERRUPT_PRIORITIES
	#error configMAX_API_CALL_INTERRUPT_PRIORITY must be less than or equal to configUNIQUE_INTERRUPT_PRIORITIES as the lower the numeric priority value the higher the logical interrupt priority
#endif

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1
	/* Check the configuration. */
	#if( configMAX_PRIORITIES > 32 )
		#error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 difference priorities as tasks that share a priority will time slice.
	#endif
#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

/* In case security extensions are implemented. */
#if configMAX_API_CALL_INTERRUPT_PRIORITY <= ( configUNIQUE_INTERRUPT_PRIORITIES / 2 )
	#error configMAX_API_CALL_INTERRUPT_PRIORITY must be greater than ( configUNIQUE_INTERRUPT_PRIORITIES / 2 )
#endif

/* Some vendor specific files default configCLEAR_TICK_INTERRUPT() in
portmacro.h. */
#ifndef configCLEAR_TICK_INTERRUPT
	#define configCLEAR_TICK_INTERRUPT()
#endif

/* In all GICs 255 can be written to the priority mask register to unmask all
(but the lowest) interrupt priority. */
#define portUNMASK_VALUE				( 0xFFUL )

/* Tasks are not created with a floating point context, but can be given a
floating point context after they have been created.  A variable is stored as
part of the tasks context that holds portNO_FLOATING_POINT_CONTEXT if the task
does not have an FPU context, or any other value if the task does have an FPU
context. */
#define portNO_FLOATING_POINT_CONTEXT	( ( StackType_t ) 0 )

/* Constants required to setup the initial task context. */
#define portSP_ELx						( ( StackType_t ) 0x01 )
#define portSP_EL0						( ( StackType_t ) 0x00 )

#if defined( GUEST )
	#define portEL1						( ( StackType_t ) 0x04 )
	#define portINITIAL_PSTATE				( portEL1 | portSP_EL0 )
#else
	#define portEL3						( ( StackType_t ) 0x0c )
	/* At the time of writing, the BSP only supports EL3. */
	#define portINITIAL_PSTATE			( portEL3 | portSP_EL0 )
#endif


/* Used by portASSERT_IF_INTERRUPT_PRIORITY_INVALID() when ensuring the binary
point is zero. */
#define portBINARY_POINT_BITS			( ( uint8_t ) 0x03 )

/* Masks all bits in the APSR other than the mode bits. */
#define portAPSR_MODE_BITS_MASK			( 0x0C )

/* The I bit in the DAIF bits. */
#define portDAIF_I						( 0x80 )

/* Macro to unmask all GIC interrupt priorities, this is used only within 
port.c for GIC priority mask register access. */
#define portUNMASK_GIC_PRIORITY()									\
{																	\
	portDISABLE_INTERRUPTS();										\
	portICCPMR_PRIORITY_MASK_REGISTER = portUNMASK_VALUE;			\
	__asm volatile (	"DSB SY		\n"								\
						"ISB SY		\n" );							\
	portENABLE_INTERRUPTS();										\
}

/* Hardware specifics used when sanity checking the configuration. */
#define portINTERRUPT_PRIORITY_REGISTER_OFFSET		0x400UL
#define portMAX_8_BIT_VALUE							( ( uint8_t ) 0xff )
#define portBIT_0_SET								( ( uint8_t ) 0x01 )

/*-----------------------------------------------------------*/

/*
 * Starts the first task executing.  This function is necessarily written in
 * assembly code so is implemented in portASM.s.
 */
extern void vPortRestoreTaskContext( void );

/*-----------------------------------------------------------*/

/* Saved as part of the task context.  If ullPortTaskHasFPUContext is non-zero
then floating point context must be saved and restored for the task. */
uint64_t ullPortTaskHasFPUContext[ configNUMBER_OF_CORES ] = { 0 };

/* Set to 1 to pend a context switch from an ISR. */
uint64_t ullPortYieldRequired[ configNUMBER_OF_CORES ] = { 0 };

/* Counts the interrupt nesting depth.  A context switch is only performed if
if the nesting depth is 0. */
uint64_t ullPortInterruptNesting[ configNUMBER_OF_CORES ] = { 0 };

/* Used in the ASM code. */
__attribute__(( used )) const uint64_t ullICCEOIR = portICCEOIR_END_OF_INTERRUPT_REGISTER_ADDRESS;
__attribute__(( used )) const uint64_t ullICCIAR = portICCIAR_INTERRUPT_ACKNOWLEDGE_REGISTER_ADDRESS;
__attribute__(( used )) const uint64_t ullICCPMR = portICCPMR_PRIORITY_MASK_REGISTER_ADDRESS;

/* SMP recursive spinlocks for task-level and ISR-level critical sections.
 *
 * The FreeRTOS SMP kernel expects the port lock functions to be RECURSIVE
 * (re-entrant from the same core).  For example, vTaskSuspendAll() acquires
 * the task lock, then xTaskResumeAll() calls taskENTER_CRITICAL() which
 * tries to acquire the task lock again on the same core.  With a plain
 * non-reentrant spinlock this would deadlock.
 *
 * We track per-lock ownership (which core holds the lock) and a recursion
 * count.  When the owning core re-acquires, we simply bump the count.
 * On release, we decrement the count and only truly unlock the spinlock
 * when the count reaches zero.
 *
 */

/* Lock indices are defined in portmacro.h as eLockType_t:
 *   eLockISR = 0, eLockTask = 1, eLockCount = 2 */

/* Per-lock state: the raw spinlock word, recursion count, and owning core.
 * ulOwner uses portMAX_CORE_ID_SENTINEL (0xFF) to mean "unowned". */
#define portLOCK_UNOWNED		( ( uint32_t ) 0xFFU )

static unsigned int ulSpinLock[ eLockCount ] = { 0, 0 };
static uint32_t ulRecursionCount[ eLockCount ] = { 0, 0 };
static uint32_t ulOwner[ eLockCount ] = { portLOCK_UNOWNED, portLOCK_UNOWNED };

/*
 * vPortRecursiveLock - acquire or release a recursive SMP spinlock.
 *
 * @param ulCoreID   The calling core (0 or 1).
 * @param eLock      Which lock (eLockISR or eLockTask).
 * @param xAcquire   pdTRUE to acquire, pdFALSE to release.
 */
void vPortRecursiveLock( uint32_t ulCoreID,
						 eLockType_t eLock,
						 BaseType_t xAcquire )
{
	configASSERT( eLock < eLockCount );

	if( xAcquire != pdFALSE )
	{
		/* Acquire path. */
		if( ulOwner[ eLock ] == ulCoreID )
		{
			/* Already owned by this core - just bump the recursion count. */
			configASSERT( ulRecursionCount[ eLock ] > 0 );
			ulRecursionCount[ eLock ]++;
		}
		else
		{
			/* Not owned by this core - spin until we get the raw lock.
			 * cpu_spin_lock is a non-reentrant LDAXR/STXR spinlock with
			 * acquire semantics.  We only reach here when this core does
			 * NOT already hold the lock, so no self-deadlock. */
			cpu_spin_lock( &ulSpinLock[ eLock ] );

			/* We now own the lock.  Set up the recursive state. */
			configASSERT( ulRecursionCount[ eLock ] == 0 );
			ulRecursionCount[ eLock ] = 1;
			ulOwner[ eLock ] = ulCoreID;
		}
	}
	else
	{
		/* Release path. */
		configASSERT( ulOwner[ eLock ] == ulCoreID );
		configASSERT( ulRecursionCount[ eLock ] > 0 );

		ulRecursionCount[ eLock ]--;

		if( ulRecursionCount[ eLock ] == 0 )
		{
			ulOwner[ eLock ] = portLOCK_UNOWNED;
			cpu_spin_unlock( &ulSpinLock[ eLock ] );
		}
	}
}

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Setup the initial stack of the task.  The stack is set exactly as
	expected by the portRESTORE_CONTEXT() macro. */

	/* First all the general purpose registers. */
	pxTopOfStack--;
	*pxTopOfStack = 0x0101010101010101ULL;	/* R1 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) pvParameters; /* R0 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0303030303030303ULL;	/* R3 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0202020202020202ULL;	/* R2 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0505050505050505ULL;	/* R5 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0404040404040404ULL;	/* R4 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0707070707070707ULL;	/* R7 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0606060606060606ULL;	/* R6 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0909090909090909ULL;	/* R9 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0808080808080808ULL;	/* R8 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1111111111111111ULL;	/* R11 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1010101010101010ULL;	/* R10 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1313131313131313ULL;	/* R13 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1212121212121212ULL;	/* R12 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1515151515151515ULL;	/* R15 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1414141414141414ULL;	/* R14 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1717171717171717ULL;	/* R17 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1616161616161616ULL;	/* R16 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1919191919191919ULL;	/* R19 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1818181818181818ULL;	/* R18 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2121212121212121ULL;	/* R21 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2020202020202020ULL;	/* R20 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2323232323232323ULL;	/* R23 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2222222222222222ULL;	/* R22 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2525252525252525ULL;	/* R25 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2424242424242424ULL;	/* R24 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2727272727272727ULL;	/* R27 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2626262626262626ULL;	/* R26 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2929292929292929ULL;	/* R29 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2828282828282828ULL;	/* R28 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00;	/* XZR - has no effect, used so there are an even number of registers. */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00;	/* R30 - procedure call link register. */
	pxTopOfStack--;

	*pxTopOfStack = portINITIAL_PSTATE;
	pxTopOfStack--;

	*pxTopOfStack = ( StackType_t ) pxCode; /* Exception return address. */
	pxTopOfStack--;

	/* The task will start without a floating point context.  A task that uses
	the floating point hardware must call vPortTaskUsesFPU() before executing
	any floating point instructions.  Paired with a zero padding slot to match
	the STP/LDP pair in portSAVE_CONTEXT/portRESTORE_CONTEXT. */
	*pxTopOfStack = ( StackType_t ) 0x00;	/* Padding (XZR in the STP pair). */
	pxTopOfStack--;
	*pxTopOfStack = portNO_FLOATING_POINT_CONTEXT;

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
uint32_t ulAPSR;

	#if( configASSERT_DEFINED == 1 )
	{
		volatile uint32_t ulOriginalPriority;
		volatile uint8_t * const pucFirstUserPriorityRegister = ( volatile uint8_t * const ) ( configINTERRUPT_CONTROLLER_BASE_ADDRESS + portINTERRUPT_PRIORITY_REGISTER_OFFSET );
		volatile uint8_t ucMaxPriorityValue;

		/* Determine how many priority bits are implemented in the GIC.

		Save the interrupt priority value that is about to be clobbered. */
		ulOriginalPriority = *pucFirstUserPriorityRegister;

		/* Determine the number of priority bits available.  First write to
		all possible bits. */
		*pucFirstUserPriorityRegister = portMAX_8_BIT_VALUE;

		/* Read the value back to see how many bits stuck. */
		ucMaxPriorityValue = *pucFirstUserPriorityRegister;

		/* Shift to the least significant bits. */
		while( ( ucMaxPriorityValue & portBIT_0_SET ) != portBIT_0_SET )
		{
			ucMaxPriorityValue >>= ( uint8_t ) 0x01;
		}

		/* Sanity check configUNIQUE_INTERRUPT_PRIORITIES matches the read
		value. */

		configASSERT( ucMaxPriorityValue >= portLOWEST_INTERRUPT_PRIORITY );


		/* Restore the clobbered interrupt priority register to its original
		value. */
		*pucFirstUserPriorityRegister = ulOriginalPriority;
	}
	#endif /* configASSERT_DEFINED */


	/* At the time of writing, the BSP only supports EL3. */
	__asm volatile ( "MRS %0, CurrentEL" : "=r" ( ulAPSR ) );
	ulAPSR &= portAPSR_MODE_BITS_MASK;

#if defined( GUEST )
	#warning Building for execution as a guest under XEN. THIS IS NOT A FULLY TESTED PATH.
	configASSERT( ulAPSR == portEL1 );
	if( ulAPSR == portEL1 )
#else
	configASSERT( ulAPSR == portEL3 );
	if( ulAPSR == portEL3 )
#endif
	{
		/* Only continue if the binary point value is set to its lowest possible
		setting.  See the comments in vPortValidateInterruptPriority() below for
		more information. */
		configASSERT( ( portICCBPR_BINARY_POINT_REGISTER & portBINARY_POINT_BITS ) <= portMAX_BINARY_POINT_VALUE );

		if( ( portICCBPR_BINARY_POINT_REGISTER & portBINARY_POINT_BITS ) <= portMAX_BINARY_POINT_VALUE )
		{
			/* Interrupts are turned off in the CPU itself to ensure a tick does
			not execute	while the scheduler is being started.  Interrupts are
			automatically turned back on in the CPU when the first task starts
			executing. */
			portDISABLE_INTERRUPTS();

			/* Start the timer that generates the tick ISR. */
			configSETUP_TICK_INTERRUPT();

			/* Start the first task executing. */
			vPortRestoreTaskContext();
		}
	}

	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* Not implemented in ports where there is nothing to return to.
	Artificially force an assert. */
	configASSERT( pdFALSE );
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

void FreeRTOS_Tick_Handler( void )
{
	BaseType_t xCoreID = portGET_CORE_ID();

	/* Must be the lowest possible priority. */
	#if !defined( QEMU )
	{
		configASSERT( portICCRPR_RUNNING_PRIORITY_REGISTER == ( uint32_t ) ( portLOWEST_USABLE_INTERRUPT_PRIORITY << portPRIORITY_SHIFT ) );
	}
	#endif

	/* Interrupts should not be enabled before this point. */
	#if( configASSERT_DEFINED == 1 )
	{
		uint32_t ulMaskBits;

		__asm volatile( "mrs %0, daif" : "=r"( ulMaskBits ) :: "memory" );
		configASSERT( ( ulMaskBits & portDAIF_I ) != 0 );
	}
	#endif /* configASSERT_DEFINED */

	/* Set interrupt mask before altering scheduler structures.   The tick
	handler runs at the lowest priority, so interrupts cannot already be masked,
	so there is no need to save and restore the current mask value.  It is
	necessary to turn off interrupts in the CPU itself while the ICCPMR is being
	updated. */
	portICCPMR_PRIORITY_MASK_REGISTER = ( uint32_t ) ( configMAX_API_CALL_INTERRUPT_PRIORITY << portPRIORITY_SHIFT );
	__asm volatile (	"dsb sy		\n"
						"isb sy		\n" ::: "memory" );

	/* Ok to enable interrupts after the interrupt source has been cleared. */
	configCLEAR_TICK_INTERRUPT();
	portENABLE_INTERRUPTS();

	/* SMP: Acquire the ISR critical section before calling xTaskIncrementTick().
	 *
	 * The SMP kernel's xTaskIncrementTick() may call prvYieldForTask() which
	 * asserts portGET_CRITICAL_NESTING_COUNT(xCoreID) > 0.  The ISR-level
	 * critical section (portENTER_CRITICAL_FROM_ISR) acquires the ISR spinlock
	 * and increments the nesting count in the current TCB, satisfying this
	 * requirement and providing mutual exclusion against other cores modifying
	 * the ready lists concurrently. */
	UBaseType_t uxSavedInterruptStatus = portENTER_CRITICAL_FROM_ISR();

	/* Increment the RTOS tick. */
	if( xTaskIncrementTick() != pdFALSE )
	{
		ullPortYieldRequired[ xCoreID ] = pdTRUE;
	}

	portEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

	/* Ensure all interrupt priorities are active again. */
	portUNMASK_GIC_PRIORITY();
}
/*-----------------------------------------------------------*/

void vPortTaskUsesFPU( void )
{
	/* A task is registering the fact that it needs an FPU context.  Set the
	FPU flag (which is saved as part of the task context). */
	ullPortTaskHasFPUContext[ portGET_CORE_ID() ] = pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortClearInterruptMask( UBaseType_t uxNewMaskValue )
{
	if( uxNewMaskValue == pdFALSE )
	{
		portUNMASK_GIC_PRIORITY();
	}
}
/*-----------------------------------------------------------*/

UBaseType_t uxPortSetInterruptMask( void )
{
uint32_t ulReturn;

	/* Interrupt in the CPU must be turned off while the ICCPMR is being
	updated. */
	portDISABLE_INTERRUPTS();
	if( portICCPMR_PRIORITY_MASK_REGISTER == ( uint32_t ) ( configMAX_API_CALL_INTERRUPT_PRIORITY << portPRIORITY_SHIFT ) )
	{
		/* Interrupts were already masked. */
		ulReturn = pdTRUE;
	}
	else
	{
		ulReturn = pdFALSE;
		portICCPMR_PRIORITY_MASK_REGISTER = ( uint32_t ) ( configMAX_API_CALL_INTERRUPT_PRIORITY << portPRIORITY_SHIFT );
		__asm volatile (	"dsb sy		\n"
							"isb sy		\n" ::: "memory" );
	}
	portENABLE_INTERRUPTS();

	return ulReturn;
}
/*-----------------------------------------------------------*/

#if( configASSERT_DEFINED == 1 )

	void vPortValidateInterruptPriority( void )
	{
		/* The following assertion will fail if a service routine (ISR) for
		an interrupt that has been assigned a priority above
		configMAX_SYSCALL_INTERRUPT_PRIORITY calls an ISR safe FreeRTOS API
		function.  ISR safe FreeRTOS API functions must *only* be called
		from interrupts that have been assigned a priority at or below
		configMAX_SYSCALL_INTERRUPT_PRIORITY.

		Numerically low interrupt priority numbers represent logically high
		interrupt priorities, therefore the priority of the interrupt must
		be set to a value equal to or numerically *higher* than
		configMAX_SYSCALL_INTERRUPT_PRIORITY.

		FreeRTOS maintains separate thread and ISR API functions to ensure
		interrupt entry is as fast and simple as possible. */
		configASSERT( portICCRPR_RUNNING_PRIORITY_REGISTER >= ( uint32_t ) ( configMAX_API_CALL_INTERRUPT_PRIORITY << portPRIORITY_SHIFT ) );

		/* Priority grouping:  The interrupt controller (GIC) allows the bits
		that define each interrupt's priority to be split between bits that
		define the interrupt's pre-emption priority bits and bits that define
		the interrupt's sub-priority.  For simplicity all bits must be defined
		to be pre-emption priority bits.  The following assertion will fail if
		this is not the case (if some bits represent a sub-priority).

		The priority grouping is configured by the GIC's binary point register
		(ICCBPR).  Writting 0 to ICCBPR will ensure it is set to its lowest
		possible value (which may be above 0). */
		configASSERT( ( portICCBPR_BINARY_POINT_REGISTER & portBINARY_POINT_BITS ) <= portMAX_BINARY_POINT_VALUE );
	}

#endif /* configASSERT_DEFINED */
/*-----------------------------------------------------------*/

/*
 * Send an SGI to yield a specific core.
 * The target_list is a bitmask of target CPUs.
 * filter_list = 0 means "use target_list" in GICv2.
 */
void vPortYieldCore( BaseType_t xCoreID )
{
	/* Send SGI0 to the target core to trigger a yield.
	 * target_list is a CPU bitmask: bit 0 = core 0, bit 1 = core 1 */
	GIC_SendSGI( ( IRQn_Type ) portYIELD_SGIn, ( uint32_t ) ( 1UL << xCoreID ), 0 );
}
/*-----------------------------------------------------------*/

/*
 * When the SMP kernel wants core N to yield (because a higher-priority
 * task became ready), it calls portYIELD_CORE(N) which sends SGI0 to
 * that core.  The SGI0 interrupt enters FreeRTOS_IRQ_Handler, which
 * dispatches through vApplicationIRQHandler -> IRQ_GetHandler(SGI0).
 *
 * This handler simply sets ullPortYieldRequired[coreID] = pdTRUE so
 * that FreeRTOS_IRQ_Handler performs a context switch on IRQ exit.
 */
void vSGIYieldHandler( void )
{
	/* Determine which core is handling this SGI. */
	uint64_t mpidr;
	BaseType_t xCoreID;

	__asm volatile ( "MRS %0, MPIDR_EL1" : "=r" ( mpidr ) ); // cpuid()
	xCoreID = ( BaseType_t ) ( mpidr & 0x3UL );

	ullPortYieldRequired[ xCoreID ] = pdTRUE;
}
/*-----------------------------------------------------------*/

/*
 * Weak default for vApplicationFPUSafeIRQHandler.
 *
 * This is called by the weak vApplicationIRQHandler in portASM.S when the
 * application does not provide a strong vApplicationIRQHandler.  If the
 * application provides vApplicationFPUSafeIRQHandler (strong), this weak
 * default is overridden by the linker.
 *
 * The weak default does nothing - this ensures a safe no-op if neither
 * handler is provided by the application (though in practice the
 * application MUST provide one of the two or no IRQs will be serviced).
 */
void __attribute__(( weak )) vApplicationFPUSafeIRQHandler( uint32_t ulICCIAR )
{
	( void ) ulICCIAR;
}

 /**************************************************************************//**
 * @file     cpu.s
 * @brief    CPU Source File for MA35D1 Device Series
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Optimized assembly for low-level CPU operations on ARM64 processors.
 */

#include <asm.h>
#include <cache.h>

.macro ma35d1_memset
	/*
	 * Clear .bss section
	*/
	ldr   x1, =__bss_start
	ldr   x2, =__bss_end
	ldr   w3, =0
bss_clear_loop:
	cmp   x1, x2
	beq   bss_clear_loop_end
	str   w3, [x1], #4
	b     bss_clear_loop
bss_clear_loop_end:
	ret.
.endm

.macro	dcache_apply_all crm
	dsb	sy
	mrs	x0, clidr_el1			// read CLIDR
	and	w3, w0, #0x07000000		// narrow to LoC
	lsr	w3, w3, #23			// left align LoC (low 4 bits)
	cbz	w3, 5f //MA35D1ne

	mov	w10, #0				// w10 = 2 * cache level
	mov	w8, #1				// w8 = constant 0b1

1:	//next_level
	add	w2, w10, w10, lsr #1		// calculate 3 * cache level
	lsr	w1, w0, w2			// extract 3-bit cache type for this level
	and	w1, w1, #0x7			// w1 = cache type
	cmp	w1, #2				// is it data or i&d?
	b.lt	4f //skip
	msr	csselr_el1, x10			// select current cache level
	isb					// sync change of csselr
	mrs	x1, ccsidr_el1			// w1 = read ccsidr
	and	w2, w1, #7			// w2 = log2(linelen_bytes) - 4
	add	w2, w2, #4			// w2 = log2(linelen_bytes)
	ubfx	w4, w1, #3, #10			// w4 = associativity - 1 (also
						// max way number)
	clz	w5, w4				// w5 = 32 - log2(ways)
						// (bit position of way in DC)
	lsl	w9, w4, w5			// w9 = max way number
						// (aligned for DC)
	lsl	w16, w8, w5			// w16 = amount to decrement (way
						// number per iteration)
2:	//next_way
	ubfx	w7, w1, #13, #15		// w7 = max set #, right aligned
	lsl	w7, w7, w2			// w7 = max set #, DC aligned
	lsl	w17, w8, w2			// w17 = amount to decrement (set
						// number per iteration)

3:	//next_set
	orr	w11, w10, w9			// w11 = combine way # & cache #
	orr	w11, w11, w7			// ... and set #
	dc	\crm, x11			// clean and/or invalidate line
	subs	w7, w7, w17			// decrement set number
	b.ge	3b //next_set
	subs	x9, x9, x16			// decrement way number
	b.ge	2b //next_way

4:	//skip
	add	w10, w10, #2			// increment 2 *cache level
	cmp	w3, w10				// Went beyond LoC?
	b.gt	1b //next_level

5:	//MA35D1ne
	dsb	sy
	isb
	ret
.endm

ENTRY(dcache_invalidate_all)
	dcache_apply_all crm=isw
ENDPROC(dcache_invalidate_all)

ENTRY(dcache_clean_all)
	dcache_apply_all crm=csw
ENDPROC(dcache_clean_all)

ENTRY(dcache_clean_invalidate_all)
	dcache_apply_all crm=cisw
ENDPROC(dcache_clean_invalidate_all)

/* This must be implemented in assembly to ensure there are no accesses to
   memory (e.g. the stack) in between disabling and flushing the cache. */
ENTRY(mmu_disable)
	str	x30, [sp, #-0x8]
	mrs	x0, sctlr_el3
	mov	x1, #~(SCTLR_C | SCTLR_M)
	and	x0, x0, x1
	msr	sctlr_el3, x0
	isb
	bl	dcache_clean_invalidate_all
	ldr	x30, [sp, #-0x8]
	ret
ENDPROC(mmu_disable)

/*
 * Bring an ARMv8 processor we just gained control of (e.g. from IROM) into a
 * known state regarding caches/SCTLR/PSTATE. Completely invalidates
 * icache/dcache, disables MMU and dcache (if active), and enables unaligned
 * accesses, icache and branch prediction (if inactive). Seeds the stack and
 * initializes SP_EL0. Clobbers R22 and R23.
 */
ENTRY(arm64_enable_int)
	msr	DAIFClr, #0xf
	dsb sy
	isb sy
	ret
ENDPROC(arm64_enable_int)

ENTRY(arm64_disable_int)
	msr	DAIFset, #0xf
	dsb sy
	isb sy
	ret
ENDPROC(arm64_disable_int)

#define SPINLOCK_LOCK       1
#define SPINLOCK_UNLOCK     0


/* void cpu_spin_lock(unsigned int *lock); */
ENTRY(cpu_spin_lock)
	mov	w2, #SPINLOCK_LOCK
	sevl
l1:	wfe
l2:	ldaxr	w1, [x0]
	cbnz	w1, l1
	stxr	w1, w2, [x0]
	cbnz	w1, l2
	ret
ENDPROC(cpu_spin_lock)

/* unsigned int cpu_spin_trylock(unsigned int *lock); */
ENTRY(cpu_spin_trylock)
	mov     x1, x0
	mov     w2, #SPINLOCK_LOCK
.loop:	ldaxr   w0, [x1]
	cbnz    w0, .cpu_spin_trylock_out
	stxr    w0, w2, [x1]
	cbnz    w0, .loop
.cpu_spin_trylock_out:
	ret
ENDPROC(cpu_spin_trylock)

/* void cpu_spin_unlock(unsigned int *lock); */
ENTRY(cpu_spin_unlock)
	stlr	wzr, [x0]
	ret
ENDPROC(cpu_spin_unlock)

ENTRY(flush_l2)
     MOV X0, #0x2            // X0 = 0x2 , target is L2 Cache
     MSR CSSELR_EL1, x0      // 0x2 for L2 Dcache.
     MRS X4, CCSIDR_EL1      // Read Cache Size ID.
     AND X1, X4, #0x7
     ADD X1, X1, #0x4        // X1 = Cache Line Size.
     LDR X3, =0x7FFF
     AND X2, X3, X4, LSR #13 // X2 = Cache Set Number �V 1.
     LDR X3, =0x3FF
     AND X3, X3, X4, LSR #3  // X3 = Cache Associativity Number �V 1.
     CLZ W4, W3              // X4 = way position in the CISW instruction.

     MOV X5, #0              // X5 = way counter way_loop.
way_loop:
     MOV X6, #0              // X6 = set counter set_loop.
set_loop:
     LSL X7, X5, X4
     ORR X7, X0, X7          // Set way.
     LSL X8, X6, X1
     ORR X7, X7, X8          // Set set.
     DC CISW, X7             // Clean and Invalidate cache line.
     ADD X6, X6, #1          // Increment set counter.
     CMP X6, X2              // Last set reached yet?
     BLE set_loop            // If not, iterate set_loop,
     ADD X5, X5, #1          // else, next way.
     CMP X5, X3              // Last way reached yet?
     BLE way_loop            // If not, iterate way_loop.

     DSB SY
ENDPROC(flush_l2)

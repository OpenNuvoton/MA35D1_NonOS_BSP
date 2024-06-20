 /**************************************************************************//**
 * @file     startup.s
 * @brief    System Startup Source File for MA35D1 Loader
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

.section ".text.boot"
.extern  IRQInterrupt
.extern  FIQInterrupt
.extern  SynchronousInterrupt
.extern  SErrorInterrupt

#define BOOT_FROM_EL3 1
#define EL1_NONSECURE 0

.global __el3_stack
.global __el0_stack
.global _vectors

.set EL3_stack,     __el3_stack
.set EL0_stack,     __el0_stack

.global start64

.macro saveregister
    stp X0,X1, [sp,#-0x10]!
    stp X2,X3, [sp,#-0x10]!
    stp X4,X5, [sp,#-0x10]!
    stp X6,X7, [sp,#-0x10]!
    stp X8,X9, [sp,#-0x10]!
    stp X10,X11, [sp,#-0x10]!
    stp X12,X13, [sp,#-0x10]!
    stp X14,X15, [sp,#-0x10]!
    stp X16,X17, [sp,#-0x10]!
    stp X18,X19, [sp,#-0x10]!
    stp X29,X30, [sp,#-0x10]!
.endm

.macro restoreregister
    ldp X29,X30, [sp], #0x10
    ldp X18,X19, [sp], #0x10
    ldp X16,X17, [sp], #0x10
    ldp X14,X15, [sp], #0x10
    ldp X12,X13, [sp], #0x10
    ldp X10,X11, [sp], #0x10
    ldp X8,X9, [sp], #0x10
    ldp X6,X7, [sp], #0x10
    ldp X4,X5, [sp], #0x10
    ldp X2,X3, [sp], #0x10
    ldp X0,X1, [sp], #0x10
.endm

start64:
    mov      x0, #0
    mov      x1, #0
    mov      x2, #0
    mov      x3, #0
    mov      x4, #0
    mov      x5, #0
    mov      x6, #0
    mov      x7, #0
    mov      x8, #0
    mov      x9, #0
    mov      x10, #0
    mov      x11, #0
    mov      x12, #0
    mov      x13, #0
    mov      x14, #0
    mov      x15, #0
    mov      x16, #0
    mov      x17, #0
    mov      x18, #0
    mov      x19, #0
    mov      x20, #0
    mov      x21, #0
    mov      x22, #0
    mov      x23, #0
    mov      x24, #0
    mov      x25, #0
    mov      x26, #0
    mov      x27, #0
    mov      x28, #0
    mov      x29, #0
    mov      x30, #0
    msr     SCTLR_EL3, x1

InitEL3:
    ldr x1, =_vectors
    msr VBAR_EL3,x1

    /* Define stack pointer for current exception level */
    ldr  x2,=EL3_stack
    mov  sp,x2

    /* Enable Trapping of SIMD/FPU register for standalone BSP */
    mov      x0, #0
    msr      CPTR_EL3, x0
    isb      sy

    /* Configure SCR_EL3 */
    mov      w1, #0                 //; Initial value of register is unknown
    orr      w1, w1, #(1 << 11)     //; Set ST bit (Secure EL1 can access CNTPS_TVAL_EL1, CNTPS_CTL_EL1 & CNTPS_CVAL_EL1)
                                    //; Set ST bit (disable trapping of timer control registers)
    orr      w1, w1, #(1 << 10)     //; Set RW bit (EL1 is AArch64, as this is the Secure world)
                                    //; Set RW bit (next lower EL in aarch64)
    orr      w1, w1, #(1 << 3)      //; Set EA bit (SError routed to EL3)
    orr      w1, w1, #(1 << 2)      //; Set FIQ bit (FIQs routed to EL3)
    orr      w1, w1, #(1 << 1)      //; Set IRQ bit (IRQs routed to EL3)
    msr      SCR_EL3, x1

    /* Enable SError Exception for asynchronous abort */
    mrs     x1,DAIF
    bic x1,x1,#(0x1<<8)
    msr DAIF,x1

    /* Disable MMU first */
    mov x1,#0x0
    msr     SCTLR_EL3, x1
    isb

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

    b    main       //jump to start

    // important, code has to be properly aligned
    .align 11
_vectors:
.set    VBAR, _vectors
.org VBAR

.org (VBAR + 0x200)
    b   SynchronousInterruptHandler

.org (VBAR + 0x280)
    b   IRQInterruptHandler

.org (VBAR + 0x300)
    b   FIQInterruptHandler

.org (VBAR + 0x380)
    b   SErrorInterruptHandler

    // synchronous
SynchronousInterruptHandler:
    saveregister
    bl       SynchronousInterrupt
    restoreregister
    eret

    // IRQ
IRQInterruptHandler:
    saveregister
    bl       IRQInterrupt
    restoreregister
    eret

    // FIQ
FIQInterruptHandler:
    saveregister
    b       FIQInterrupt
    restoreregister
    eret

    // SError
 SErrorInterruptHandler:
    saveregister
    b       SErrorInterrupt
    restoreregister
    eret

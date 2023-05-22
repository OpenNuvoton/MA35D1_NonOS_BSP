

    // important, code has to be properly aligned
    .align 11
    .global __vectors
__vectors:
.set	_VBAR, __vectors
.org _VBAR

.org (_VBAR + 0x200)
	b	SynchronousInterruptHandler

.org (_VBAR + 0x280)
	b	IRQInterruptHandler

.org (_VBAR + 0x300)
	b	FIQInterruptHandler

.org (_VBAR + 0x380)
	b	SErrorInterruptHandler

    // synchronous
SynchronousInterruptHandler:

    eret

    // IRQ
IRQInterruptHandler:
	stp		X0,X1, [sp,#-0x10]!
    // Disable interrupt
	movz	x1, #0x1000
	movk	x1, #0x5080, lsl #0x10
	ldr		w0, [x1]
	and 	w0, w0, #0xFFFFFFFC
	str		w0, [x1]
	ldp		X0,X1, [sp], #0x10
    eret

    // FIQ
FIQInterruptHandler:
	stp		X0,X1, [sp,#-0x10]!
    // Disable interrupt
	movz	x1, #0x1000
	movk	x1, #0x5080, lsl #0x10
	ldr		w0, [x1]
	and 	w0, w0, #0xFFFFFFFC
	str		w0, [x1]
	ldp		X0,X1, [sp], #0x10
    eret

    // SError
SErrorInterruptHandler:

    eret

/******************************************************************************
 * @file     gcc_arm.ld
 * @brief    GNU Linker Script for MA35D1 loader
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved. 
 ******************************************************************************/ 
_STACK_SIZE = DEFINED(_STACK_SIZE) ? _STACK_SIZE : 0x4000;
_HEAP_SIZE = DEFINED(_HEAP_SIZE) ? _HEAP_SIZE : 0x4000;

 MEMORY
{
   RAM_BASEADDR : ORIGIN = 0x28010000, LENGTH = 0x00010000
   DDR_BASEADDR : ORIGIN = 0x80400000, LENGTH = 0x40000000
}

SECTIONS
{
	.pd : {
        ./Arch/Core_A/vector_sram.o(.text*)
        ./Library/Library/pd.o(.text*)
	} > RAM_BASEADDR
    .text : {
    	KEEP(*(.text.boot))
    	__start__ = .; 
    	*(.text .text.* .gnu.linkonce.t*) 
    	} > DDR_BASEADDR
    .rodata : { 
    	*(.rodata .rodata.* .gnu.linkonce.r*) 
    	} > DDR_BASEADDR
    . = ALIGN(4096);
    PROVIDE(_data = .);
    .data : { 
    	*(.data .data.* .gnu.linkonce.d*) 
    	} > DDR_BASEADDR
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(16);
        __bss_end = .;
    } > DDR_BASEADDR
    
	.heap (NOLOAD) : {
	   . = ALIGN(64);
	   _heap = .;
	   HeapBase = .;
	   _heap_start = .;
	   . += _HEAP_SIZE;
	   _heap_end = .;
	   HeapLimit = .;
	} > DDR_BASEADDR
	
	.stack (NOLOAD) : {
	   . = ALIGN(64);
	   _el3_stack_end = .;
	   . += _STACK_SIZE;
	   __el3_stack = .;
	   . += _STACK_SIZE;
	   __el0_stack = .;
	   . += _STACK_SIZE;
	   __el3_stack_s = .;
	   . += _STACK_SIZE;
	   __el0_stack_s = .;
	} > DDR_BASEADDR

    _end = .;
    end = .;
   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
__bss_size = (__bss_end - __bss_start)>>3;

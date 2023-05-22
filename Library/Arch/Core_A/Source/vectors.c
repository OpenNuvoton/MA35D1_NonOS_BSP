/**************************************************************************//**
 * @file     vectors.c
 * @brief    Debug1 Port Setting Source File
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "NuMicro.h"

void FIQInterrupt(void)
{
	IRQHandler_t handler;
	IRQn_ID_t num=IRQ_GetActiveIRQ();

	handler=IRQ_GetHandler(num);
	if(handler!=0)
		(*handler)();

	IRQ_EndOfInterrupt(num);
}

void IRQInterrupt(void)
{
	IRQHandler_t handler;
	IRQn_ID_t num=IRQ_GetActiveIRQ();

	handler=IRQ_GetHandler(num);
	if(handler!=0)
		(*handler)();

	IRQ_EndOfInterrupt(num);
}

static char SynchronousInterruptFlag=0;
void SynchronousInterrupt(void)
{
	unsigned int ec, lv;

	lv = (raw_read_currentel() & 0xC) >> 2;
	if(lv == 3) {
		ec= (raw_read_esr_el3() & 0xFC000000) >> 26;
	} else if (lv == 2) {
		ec= (raw_read_esr_el2() & 0xFC000000) >> 26;
	} else {
		ec= (raw_read_esr_el1() & 0xFC000000) >> 26;
	}
	if(ec==0x17)
		raw_write_spsr_el3(0xC);

	if(SynchronousInterruptFlag!=0)
		return;
	sysprintf("\nSYNC(lv %x) INT - ", lv);
	sysprintf("S\n");
	if(ec == 0x0) {
		sysprintf("Unknown\n");
	} else if(ec == 0x01) {
		sysprintf("WFI/WFE\n");
	}else if(ec == 0xE) {
		sysprintf("Illegal execution state\n");
	}else if(ec == 0x15) {
		sysprintf("SVC\n");
	}else if(ec == 0x16) {
		sysprintf("HVC\n");
	}else if(ec == 0x17) {
		sysprintf("SMC\n");
	}else if(ec == 0x20 || ec == 0x21) {
		sysprintf("Instruction abort\n");
	}else if(ec == 0x22) {
		sysprintf("PC alignment fault\n");
	}else if(ec == 0x24 || ec == 0x25) {
		sysprintf("Data abort\n");
	} else if(ec == 0x26) {
		sysprintf("SP error\n");
	} else if(ec == 0x2F) {
		sysprintf("SError\n");
	} else {
		sysprintf("ec == %02x\n", ec & 0xFF);
	}
	SynchronousInterruptFlag = 1;
}

void SErrorInterrupt(void)
{

}

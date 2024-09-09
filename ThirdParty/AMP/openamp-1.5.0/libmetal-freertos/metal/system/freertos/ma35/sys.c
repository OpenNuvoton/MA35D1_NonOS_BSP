/*
 * Copyright (c) 2018, Linaro Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	freertos/template/sys.c
 * @brief	machine specific system primitives implementation.
 */

#include <metal/io.h>
#include <metal/sys.h>
#include <metal/utilities.h>
#include <stdint.h>

#include "NuMicro.h"

void sys_irq_restore_enable(unsigned int flags)
{
	metal_unused(flags);
	/* Add implementation here */
}

unsigned int sys_irq_save_disable(void)
{
	return 0;
	/* Add implementation here */
}

void sys_irq_enable(unsigned int vector)
{
	IRQ_Enable((IRQn_ID_t)vector);
}

void sys_irq_disable(unsigned int vector)
{
	IRQ_Disable((IRQn_ID_t)vector);
}

void metal_machine_cache_flush(void *addr, unsigned int len)
{
	if (addr && len)
		dcache_clean_by_mva(addr, (size_t)len);
}

void metal_machine_cache_invalidate(void *addr, unsigned int len)
{
	if (addr && len)
		dcache_clean_invalidate_by_mva(addr, (size_t)len);
}

void metal_generic_default_poll(void)
{
	asm volatile ("wfi");
}

void *metal_machine_io_mem_map(void *va, metal_phys_addr_t pa,
			       size_t size, unsigned int flags)
{
	metal_unused(pa);
	metal_unused(size);
	metal_unused(flags);

	/* Add implementation here */

	return va;
}

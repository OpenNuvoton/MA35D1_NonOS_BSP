/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    MA35D1cumentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to enMA35D1rse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <symbols.h>

#include "MA35D1.h"
#include "mmu.h"
#include "lib_helpers.h"
#include "cache.h"

#define MMU_SIZE	(512*64)
__attribute__((aligned(0x4000))) u8 _ttb[MMU_SIZE];
static u8 * _ettb = _ttb+MMU_SIZE;

/* This just caches the next free table slot (okay to MA35D1 since they fill up from
 * bottom to top and can never be freed up again). It will reset to its initial
 * value on stage transition, so we still need to check it for UNUSED_DESC. */
static uint64_t *next_free_table = (void *)_ttb;

/* Func : get_block_attr
 * Desc : Get block descriptor attributes based on the value of tag in memrange
 * region
 */
static uint64_t get_block_attr(unsigned long tag)
{
	uint64_t attr;

	attr = (tag & MA_NS)? BLOCK_NS : 0;
	attr |= (tag & MA_RO)? BLOCK_AP_RO : BLOCK_AP_RW;
	attr |= BLOCK_ACCESS;

	if (tag & MA_MEM) {
		attr |= BLOCK_SH_INNER_SHAREABLE;
		if (tag & MA_MEM_NC)
			attr |= BLOCK_INDEX_MEM_NORMAL_NC << BLOCK_INDEX_SHIFT;
		else
			attr |= BLOCK_INDEX_MEM_NORMAL << BLOCK_INDEX_SHIFT;
	} else {
		attr |= BLOCK_INDEX_MEM_DEV_NGNRNE << BLOCK_INDEX_SHIFT;
		attr |= BLOCK_XN;
	}

	return attr;
}

/* Func : setup_new_table
 * Desc : Get next free table from TTB and set it up to match old parent entry.
 */
static uint64_t *setup_new_table(uint64_t desc, size_t xlat_size)
{

	while (next_free_table[0] != UNUSED_DESC) {
		next_free_table += GRANULE_SIZE/sizeof(*next_free_table);
	}

	void *frame_base = (void *)(desc & XLAT_ADDR_MASK);
	if (desc) {
		/* Can reuse old parent entry, but may need to adjust type. */
		if (xlat_size == L3_XLAT_SIZE)
			desc |= PAGE_DESC;

		int i = 0;
		for (; i < GRANULE_SIZE/sizeof(*next_free_table); i++) {
			next_free_table[i] = desc;
			desc += xlat_size;
		}
	}

	return next_free_table;
}

/* Func: get_next_level_table
 * Desc: Check if the table entry is a valid descriptor. If not, initialize new
 * table, update the entry and return the table addr. If valid, return the addr
 */
static uint64_t *get_next_level_table(uint64_t *ptr, size_t xlat_size)
{
	uint64_t desc = *ptr;

	if ((desc & DESC_MASK) != TABLE_DESC) {
		uint64_t *new_table = setup_new_table(desc, xlat_size);
		desc = ((uint64_t)new_table) | TABLE_DESC;
		*ptr = desc;
	}
	return (uint64_t *)(desc & XLAT_ADDR_MASK);
}

/* Func : init_xlat_table
 * Desc : Given a base address and size, it identifies the indices within
 * different level XLAT tables which map the given base addr. Similar to table
 * walk, except that all invalid entries during the walk are updated
 * accordingly. On success, it returns the size of the block/page addressed by
 * the final table.
 */
static uint64_t init_xlat_table(uint64_t base_addr,
				uint64_t size,
				uint64_t tag)
{
	uint64_t l0_index = (base_addr & L0_ADDR_MASK) >> L0_ADDR_SHIFT;
	uint64_t l1_index = (base_addr & L1_ADDR_MASK) >> L1_ADDR_SHIFT;
	uint64_t l2_index = (base_addr & L2_ADDR_MASK) >> L2_ADDR_SHIFT;
	uint64_t l3_index = (base_addr & L3_ADDR_MASK) >> L3_ADDR_SHIFT;
	uint64_t *table = (uint64_t *)_ttb;
	uint64_t desc;
	uint64_t attr = get_block_attr(tag);

	/* L1 table lookup */
	if ((size >= L1_XLAT_SIZE) &&
	    IS_ALIGNED(base_addr, (1UL << L1_ADDR_SHIFT))) {
			/* If block address is aligned and size is greater than
			 * or equal to size addressed by each L1 entry, we can
			 * directly store a block desc */
			if(base_addr==0x180000000)
				base_addr=0x80000000;  //Set SDRAM(1GB) to UNCACHED_MEM area
			desc = base_addr | BLOCK_DESC | attr;
			table[l1_index] = desc;
			/* L2 lookup is not required */
			return L1_XLAT_SIZE;
	}

	/* L1 entry stores a table descriptor */
	table = get_next_level_table(&table[l1_index], L2_XLAT_SIZE);

	/* L2 table lookup */
	if ((size >= L2_XLAT_SIZE) &&
	    IS_ALIGNED(base_addr, (1UL << L2_ADDR_SHIFT))) {
		/* If block address is aligned and size is greater than
		 * or equal to size addressed by each L2 entry, we can
		 * directly store a block desc */

		/* schung : add shaMA35D1w area for non-catchable region */
		if(tag==UNCACHED_MEM)
			base_addr&=~0x100000000;

		desc = base_addr | BLOCK_DESC | attr;
		table[l2_index] = desc;
		/* L3 lookup is not required */
		return L2_XLAT_SIZE;
	}

	/* L2 entry stores a table descriptor */
	table = get_next_level_table(&table[l2_index], L3_XLAT_SIZE);

	/* L3 table lookup */
	desc = base_addr | PAGE_DESC | attr;
	table[l3_index] = desc;
	return L3_XLAT_SIZE;
}

/* Func : mmu_config_range
 * Desc : This function repeatedly calls init_xlat_table with the base
 * address. Based on size returned from init_xlat_table, base_addr is updated
 * and subsequent calls are made for initializing the xlat table until the whole
 * region is initialized.
 */
void mmu_config_range(void *start, size_t size, uint64_t tag)
{
	uint64_t base_addr = (uintptr_t)start;
	uint64_t temp_size = size;

	while (temp_size)
		temp_size -= init_xlat_table(base_addr + (size - temp_size),
					     temp_size, tag);

	/* ARMv8 MMUs snoop L1 data cache, no need to flush it. */
	dsb();
	tlbiall_el3();
	dsb();
	isb();
}

/* Func : mmu_init
 * Desc : Initialize MMU registers and page table memory region. This must be
 * called exactly ONCE PER BOOT before trying to configure any mappings.
 */
void mmu_init(void)
{
	/* Initially mark all table slots unused (first PTE == UNUSED_DESC). */
	uint64_t *table = (uint64_t *)_ttb;
	for (; _ettb - (uint8_t *)table > 0; table += GRANULE_SIZE/sizeof(*table))
		table[0] = UNUSED_DESC;

	/* Initialize the root table (L0) to be completely unmapped. */
	uint64_t *root = setup_new_table(INVALID_DESC, L1_XLAT_SIZE);

	/* Initialize TTBR */
	raw_write_ttbr0_el3((uintptr_t)root);

	/* Initialize TCR flags */
	raw_write_tcr_el3(TCR_TOSZ | TCR_IRGN0_NM_WBWAC | TCR_ORGN0_NM_WBWAC |
			  TCR_SH0_IS | TCR_TG0_4KB | TCR_PS_4GB |
			  TCR_TBI_USED);

	/* Initialize MAIR indices */
	raw_write_mair_el3(MAIR_ATTRIBUTES);
}

/* Func : mmu_save_context
 * Desc : Save mmu context (registers and ttbr base).
 */
void mmu_save_context(struct mmu_context *mmu_context)
{
	/* Back-up MAIR_ATTRIBUTES */
	mmu_context->mair = raw_read_mair_el3();

	/* Back-up TCR value */
	mmu_context->tcr = raw_read_tcr_el3();
}

/* Func : mmu_restore_context
 * Desc : Restore mmu context using input backed-up context
 */
void mmu_restore_context(const struct mmu_context *mmu_context)
{
	/* Restore TTBR */
	raw_write_ttbr0_el3((uintptr_t)_ttb);

	/* Restore MAIR indices */
	raw_write_mair_el3(mmu_context->mair);

	/* Restore TCR flags */
	raw_write_tcr_el3(mmu_context->tcr);

	/* invalidate tlb since ttbr is updated. */
	tlb_invalidate_all();
}

void mmu_enable(void)
{
	uint32_t sctlr = raw_read_sctlr_el3();
	sctlr |= SCTLR_C | SCTLR_M | SCTLR_I;
	raw_write_sctlr_el3(sctlr);
	isb();
}



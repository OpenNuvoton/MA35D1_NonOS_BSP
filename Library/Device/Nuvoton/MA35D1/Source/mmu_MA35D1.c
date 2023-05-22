 /**************************************************************************//**
 * @file     mmu_MA35D1.c
 * @brief    MMU Configuration for MA35D1 Device Series
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include <symbols.h>
#include "types.h"
#include "mmu.h"

/* Standard units. */
#define KiB (1<<10)
#define MiB (1<<20)
#define GiB (1<<30)
/* Could we ever run into this one? I hope we get this much memory! */
#define TiB (1<<40)

static void ma35d1_mmu_config(void)
{
	uintptr_t start, size;

	start = 0;
	size = 	1;
	/* device memory 0x0000_0000 - 0x3FFF_FFFF */
	mmu_config_range((void *)(start * GiB), size * GiB, DEV_MEM);

	start = 1;
	size = 	1;
	/* device memory 0x4000_0000 - 0x7FFF_FFFF */
	mmu_config_range((void *)(start * GiB), size * GiB, DEV_MEM);

	start = 2;
	size = 1;
	/* device memory 0x8000_0000 - 0xBFFF_FFFF */
	mmu_config_range((void *)(start * GiB), size * GiB, CACHED_MEM);

	start = 3;
	size = 1;
	/* device memory 0x8000_0000 - 0xFFFF_FFFF */
	mmu_config_range((void *)(start * GiB), size * GiB, DEV_MEM);

	start = 6;
	size = 1;
	/* device memory 0x1_8000_0000 - 0x1_BFFF_FFFF */
	mmu_config_range((void *)(start * GiB), size * GiB, UNCACHED_MEM);

}

void ma35d1_mmu_init(void)
{
	mmu_init();
	ma35d1_mmu_config();
	mmu_enable();
}

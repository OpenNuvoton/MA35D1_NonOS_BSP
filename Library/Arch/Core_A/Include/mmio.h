 /**************************************************************************//**
 * @file     mmio.h
 * @brief    MMIO Header File
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
 * Originally imported from linux/include/asm-arm/io.h. This file has changed
 * substantially since then.
 */

#ifndef __ARCH_MMIO_H__
#define __ARCH_MMIO_H__

#include <stdint.h>
#include <barrier.h>
#include <lib_helpers.h>

/// @cond HIDDEN_SYMBOLS

/* cast a pointer to be uint32_t */
#define ptr_to_u32(x)   ((uint32_t)((uint64_t)(x)))

/* add non-cache mask to get a 64-bits address */
#define nc_addr64(x)    (((uint64_t)(x) & 0xffffffffULL) | NON_CACHE)

/* add non-cache mask to get a pointer for non-cache access */
#define nc_ptr(x)       ((void *)nc_addr64(x))

/* translate a pointer to be 4GB safe and keep non-cache bit unchanged  */
#define ptr_nc_s(x)     ((void *)((uint64_t)(x) & (0xffffffffULL | NON_CACHE)))

/* translate a pointer to be 4GB safe and clear non-cache bit. */
#define ptr_s(x)        ((void *)((uint64_t)(x) & 0xffffffffULL))

/* translate a pointer to be 4GB safe address and keep non-cache bit unchanged  */
#define addr_nc_s(x)    ((uint64_t)ptr_nc_s(x))

/* translate a pointer to be 4GB safe address and clear non-cache bit. */
#define addr_s(x)       ((uint64_t)ptr_s(x))

/* Return 1 if pointer is NULL, 0 if pointer is not NULL.
 * Note that (NULL | NON_CACHE) is NULL.
 */
#define IS_NULL_PTR(x)  ((ptr_s(x) == NULL) ? 1 : 0)

static inline uint8_t read8(const void *addr)
{
	dmb();
	return *(volatile uint8_t *)addr;
}

static inline uint16_t read16(const void *addr)
{
	dmb();
	return *(volatile uint16_t *)addr;
}

static inline uint32_t read32(const void *addr)
{
	dmb();
	return *(volatile uint32_t *)((uint64_t)addr);
}

static inline uint64_t read64(const void *addr)
{
	dmb();
	return *(volatile uint64_t *)addr;
}

static inline void write8(void *addr, uint8_t val)
{
	dmb();
	*(volatile uint8_t *)addr = val;
	dmb();
}

static inline void write16(void *addr, uint16_t val)
{
	dmb();
	*(volatile uint16_t *)addr = val;
	dmb();
}

static inline void write32(void *addr, uint32_t val)
{
	dmb();
	*(volatile uint32_t *)addr = val;
	dmb();
}

static inline void write64(void *addr, uint64_t val)
{
	dmb();
	*(volatile uint64_t *)addr = val;
	dmb();
}

/// @endcond HIDDEN_SYMBOLS


typedef volatile uint8_t  vu8;        ///< Define 8-bit unsigned volatile data type
typedef volatile uint16_t vu16;       ///< Define 16-bit unsigned volatile data type
typedef volatile uint32_t vu32;       ///< Define 32-bit unsigned volatile data type
typedef volatile uint64_t vu64;       ///< Define 64-bit unsigned volatile data type

/**
  * @brief Get a 8-bit unsigned value from specified address
  * @param[in] addr Address to get 8-bit data from
  * @return  8-bit unsigned value stored in specified address
  */
#define M8(addr)  (*((vu8  *) (addr)))

/**
  * @brief Get a 16-bit unsigned value from specified address
  * @param[in] addr Address to get 16-bit data from
  * @return  16-bit unsigned value stored in specified address
  * @note The input address must be 16-bit aligned
  */
#define M16(addr) (*((vu16 *) (addr)))

/**
  * @brief Get a 32-bit unsigned value from specified address
  * @param[in] addr Address to get 32-bit data from
  * @return  32-bit unsigned value stored in specified address
  * @note The input address must be 32-bit aligned
  */
#define M32(addr) (*((vu32 *) ((uint64_t)addr)))

/**
  * @brief Set a 32-bit unsigned value to specified I/O port
  * @param[in] port Port address to set 32-bit data
  * @param[in] value Value to write to I/O port
  * @return  None
  * @note The output port must be 32-bit aligned
  */
#define outpw(addr,val)		write32((void *)((uint64_t)(addr)),val)
#define outp32(addr,val)	write32((void *)((uint64_t)(addr)),val)
#define writel(addr,val)	write32((void *)((uint64_t)(addr)),val)
#define outp16(addr,val)	write16((void *)((uint64_t)(addr)),val)
#define writew(addr,val)	write16((void *)((uint64_t)(addr)),val)
#define outpb(addr,val)		write8((void *)((uint64_t)(addr)),val)
#define writeb(addr,val)	write8((void *)((uint64_t)(addr)),val)

/**
  * @brief Get a 32-bit unsigned value from specified I/O port
  * @param[in] port Port address to get 32-bit data from
  * @return  32-bit unsigned value stored in specified I/O port
  * @note The input port must be 32-bit aligned
  */
#define inpb(x)		read8((const void *)((uint64_t)(x)))
#define inpw(x)		read32((const void *)((uint64_t)(x)))
#define inp32(x)	read32((const void *)((uint64_t)(x)))
#define readl(x)	read32((const void *)((uint64_t)(x)))
#define inp16(x)	read16((const void *)((uint64_t)(x)))
#define readw(x)	read16((const void *)((uint64_t)(x)))
#define readb(x)	read8((const void *)((uint64_t)(x)))

#endif /* __ARCH_MMIO_H__ */

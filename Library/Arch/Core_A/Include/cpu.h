/**************************************************************************//**
 * @file     cpu.h
 * @brief    CPU driver header file
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#ifndef __ARCH_CPU_H__
#define __ARCH_CPU_H__

#define asmlinkage

#define PD_SW_CTRL	0
#define PD_HW_CTRL	1

void cpu_spin_lock(unsigned int *lock);

void cpu_spin_unlock(unsigned int *lock);

void arm64_init_cpu(void);

void disable_n_flush_cache(int flush_l2);

void CA35_NPD(int mode);
void CA35_DPD(int mode);

static inline int cpuid(void)
{
	return raw_read_mpidr_el1() & 0x3 ? 1 : 0;
}

#endif /* __ARCH_CPU_H__ */

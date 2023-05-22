/**************************************************************************//**
 * @file     sys_arch.c
 * @brief    LwIP system architecture file
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "NuMicro.h"
#include "lwipopts.h"

#define LWIP_HWSEM_CH  0
#define LWIP_HWSEM_KEY 0x5A

static volatile uint32_t u32Jiffies = 0;

void TMR2_IRQHandler(void)
{
    u32Jiffies += 10;
    TIMER_ClearIntFlag(TIMER2);
}

/* time in ms */
uint32_t sys_now(void)
{
    return u32Jiffies;
}

static uint32_t lwip_spinlock = 0;
/* Enter critical section non-os version */
int sys_arch_protect(void)
{
#if (USE_INDEP_HWSEM == 0)
    cpu_spin_lock(&lwip_spinlock);
#else
    HWSEM_Spin_Lock(HWSEM0, LWIP_HWSEM_CH, LWIP_HWSEM_KEY);
#endif
    return 0;
}

/* Leave critical section non-os version */
void sys_arch_unprotect(int pval)
{
#if (USE_INDEP_HWSEM == 0)
    cpu_spin_unlock(&lwip_spinlock);
#else
    HWSEM_UNLOCK(HWSEM0, LWIP_HWSEM_CH, LWIP_HWSEM_KEY);
#endif
}

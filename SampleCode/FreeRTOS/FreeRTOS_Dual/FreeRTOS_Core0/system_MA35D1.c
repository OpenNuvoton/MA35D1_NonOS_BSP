/**************************************************************************//**
 * @file     system_MA35D1.c
 * @brief    Debug1 Port Setting Source File
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include "NuMicro.h"
#include "pmic.h"

#define  SYSTEM_CLOCK  800000000U
extern uint32_t start64;
extern void arm64_enable_int(void);
static const uint32_t TICK_RATE_HZ = 1000U;
uint32_t volatile msTicks0= 0; /* Counter for millisecond Interval */
uint32_t volatile msTicks1= 0; /* Counter for millisecond Interval */
uint32_t const GTIM_Clock = 12000000; //12MHz
static const uint32_t GTIM_Load = (GTIM_Clock / TICK_RATE_HZ); /* Timer load value */

static void SysTick_Handler( void )
{
    EL0_SetPhysicalCompareValue(GTIM_Load + raw_read_cntp_cval_el0());
    msTicks0++;                                     // Increment Counter
}

static void SysTick_Handler1( void )
{
    EL0_SetPhysicalCompareValue(GTIM_Load + raw_read_cntp_cval_el0());
    msTicks1++;                                     // Increment Counter
}

/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = SYSTEM_CLOCK;

/*----------------------------------------------------------------------------
  System Core Clock update function
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void)
{
    SystemCoreClock = SYSTEM_CLOCK;
}

__attribute__ ((weak)) void main1(){while(1);}

void global_timer_init(void) {

    /* Disable Generic Timer and set load value */
    EL0_SetControl(0);

    EL0_SetPhysicalCompareValue(GTIM_Load + EL0_GetCurrentPhysicalValue());

    /* Install SysTick_Handler as the interrupt function for PTIM */
    if(cpuid()==0)
        IRQ_SetHandler((IRQn_ID_t)NonSecPhysicalTimer_IRQn, SysTick_Handler);
    else
        IRQ_SetHandler((IRQn_ID_t)NonSecPhysicalTimer_IRQn, SysTick_Handler1);

    /* Determine number of implemented priority bits */
    IRQ_SetPriority ((IRQn_ID_t)NonSecPhysicalTimer_IRQn, IRQ_PRIORITY_Msk);

    /* Set lowest priority -1 */
    IRQ_SetPriority ((IRQn_ID_t)NonSecPhysicalTimer_IRQn, GIC_GetPriority((IRQn_ID_t)NonSecPhysicalTimer_IRQn)-1);

    /* Enable IRQ */
    IRQ_Enable ((IRQn_ID_t)NonSecPhysicalTimer_IRQn);

    /* Enable timer control */
    EL0_SetControl(1U);			// non-secure timer
}

/*----------------------------------------------------------------------------
  System Initialization
 *----------------------------------------------------------------------------*/
void RunCore1(uint32_t addr) {
    volatile int cnt;

    SYS->CA35WRBADR1= ptr_to_u32(addr);
    cnt=1000;
    while(cnt--);
    sev();
    cnt=1000;
    while(SYS->CA35WRBADR1!=0 && (cnt--)>0);
    SYS->CA35WRBADR1=  ptr_to_u32(addr);
    sev();
}

void SystemInit1 (void)
{
    /* Set timer clock */
    EL0_SetCounterFrequency(GTIM_Clock);

    /* Enable MMU */
    ma35d1_mmu_init();

    /* Distributor already initial by core 0 */
    GIC_CPUInterfaceInit();

    /* Enable interrupts */
    arm64_enable_int();
}

void Init_AXI_ports(void)
{
    SYS_UnlockReg();
	outp32(UMCTL2_BASE+0x490, 0x1);
	outp32(UMCTL2_BASE+0x540, 0x1);
	outp32(UMCTL2_BASE+0x5f0, 0x1);
	outp32(UMCTL2_BASE+0x6a0, 0x1);
	outp32(UMCTL2_BASE+0x750, 0x1);
	outp32(UMCTL2_BASE+0x800, 0x1);
	outp32(UMCTL2_BASE+0x8b0, 0x1);
	SYS->MISCFCR0 &= ~0x800000; /* DDR control register clock gating enable */
}

void SystemInit0 (void)
{
    SYS_UnlockReg();

	Init_AXI_ports();

    /* TODO: Enable Clock */
    CLK->SYSCLK0 |= 0x7f7f0000;

    /* Set region 0 to secure region, non-secure and m4 all can access */
    SSMCC_SetRegion0(SSMCC_SECURE_READ|SSMCC_SECURE_WRITE|SSMCC_NONSECURE_READ|
                     SSMCC_NONSECURE_WRITE|SSMCC_M4NS_READ|SSMCC_M4NS_WRITE);

    /* Set timer clock */
    EL0_SetCounterFrequency(GTIM_Clock);

    /* Enable MMU */
    ma35d1_mmu_init();

    /* Initial interrupt */
    IRQ_Initialize();

    /* Enable interrupts */
    arm64_enable_int();

    /* set PMIC 1.25V */
    ma35d1_write_pmic(PMIC_DEVICE_ADDR, PMIC_CPU_REG, PMIC_CPU_VOL_1_25);

    /* CPU boost to 800MHz */
    outpw(0x40460260, 0x00000364);   // CLK->PLL0CTL0

    SYS_LockReg();
}

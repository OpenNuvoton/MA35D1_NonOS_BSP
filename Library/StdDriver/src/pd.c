#include "MA35D1.h"

void ddr_pd(void)
{
    //Set ddrc core clock gating circuit bypass
    SYS->MISCFCR0 |= SYS_MISCFCR0_DDRCGDIS_Msk;

    //------------------------------------------------------
    // Flow to enable low power mode of DDR3/2 PHY
    //------------------------------------------------------
    //disable powerdown_en and selfref_en
    outp32(UMCTL2_BASE + 0x30, inp32(UMCTL2_BASE + 0x30)& ~(0x3));

    //enable static registers write enable
    outp32(UMCTL2_BASE + 0x328, 0x00000001);

    //set value of dfi_lp_wakeup_sr
    outp32(UMCTL2_BASE + 0x198, inp32(UMCTL2_BASE + 0x198) & ~(0x0000f000) | 0x0000f000);

    //disable static registers write enable
    outp32(UMCTL2_BASE + 0x328, 0x00000000);

    //enable static registers write enable
    outp32(UMCTL2_BASE + 0x328, 0x00000001);

    //enable dfi_lp_en_sr
    outp32(UMCTL2_BASE + 0x198, inp32(UMCTL2_BASE + 0x198) &~(0x00000100) | 0x00000100);

    //disable static registers write enable
    outp32(UMCTL2_BASE + 0x328, 0x00000000);

    //-------------------------------------------------------------------------
    //wait DDR AXI port0 idle
    while((inp32(UMCTL2_BASE + 0x3fc) & 0x00010001) != 0x00000000);

    //disable DDR AXI port0 ~ DDR AXI port6
    outp32(UMCTL2_BASE + 0x490, 0x00000000);
    outp32(UMCTL2_BASE + 0x540, 0x00000000);
    outp32(UMCTL2_BASE + 0x5f0, 0x00000000);
    outp32(UMCTL2_BASE + 0x6a0, 0x00000000);
    outp32(UMCTL2_BASE + 0x750, 0x00000000);
    outp32(UMCTL2_BASE + 0x800, 0x00000000);
    outp32(UMCTL2_BASE + 0x8b0, 0x00000000);

    //wait DDR AXI port0 ~ DDR AXI port6 idle
    while((inp32(UMCTL2_BASE + 0x3fc) & 0x003f003f) != 0x00000000);

    //enter DDR software self-refresh mode
    outp32(UMCTL2_BASE + 0x30, inp32(UMCTL2_BASE + 0x30) | 0x20);

    //wait DDR enter software self-refresh mode
    while((inp32(UMCTL2_BASE + 0x04) & 0x30) != 0x20);

    //disable DDR AXI port0 clock ~ DDR AXI port5 clock
    CLK->SYSCLK0 &= ~(0x7f000030);

    //disable DDR core clock
    CLK->APBCLK0 &= ~(0x40000000);
}

void ddr_dpd_wk(void)
{
    GIC_EnableDistributor(ENABLE_GRP0|ENABLE_GRP1);

    //enable DDR PLL clock
    SYS_UnlockReg();

    CLK->PLL0CTL0 = 0x364;
    while( (CLK->STATUS & CLK_STATUS_CAPLLSTB_Msk) != CLK_STATUS_CAPLLSTB_Msk );
    CLK->CLKSEL0 |= 1;//A35 use CPLL

    CLK->PLL[DDRPLL].CTL1 &= ~CLK_PLLnCTL1_PD_Msk;
    CLK->CLKSEL0 = 0x08000011;

    // wait PLL stable
    while( (CLK->STATUS & CLK_STATUS_DDRPLLSTB_Msk) != CLK_STATUS_DDRPLLSTB_Msk );

    //enable DDR AXI port0 clock and DDR AXI port5 clock
    CLK->SYSCLK0 |=  0x00000034;

    //Set ddrc core clock gating circuit bypass
    SYS->MISCFCR0 |= SYS_MISCFCR0_DDRCGDIS_Msk;

    //enable DDR core clock
    CLK->APBCLK0 |= CLK_APBCLK0_DDRPCKEN_Msk;

    //Wait DDR-PLL stable
    while((CLK->STATUS & CLK_STATUS_DDRPLLSTB_Msk) != CLK_STATUS_DDRPLLSTB_Msk);

    //exit DDR software self-refresh mode
    outp32(UMCTL2_BASE + 0x30, inp32(UMCTL2_BASE + 0x30) & ~(0x00000020));

    //wait DDR exit software self-refresh mode
    while((inp32(UMCTL2_BASE + 0x04) & 0x30) != 0x00);

    //enable DDR AXI port0, DDR AXI port5, and DDR AXI port6
    outp32(UMCTL2_BASE + 0x490, 0x00000001);
    outp32(UMCTL2_BASE + 0x800, 0x00000001);
    outp32(UMCTL2_BASE + 0x8b0, 0x00000001);

    //------------------------------------------------------
    // Flow to disable low power mode of DDR3/2 PHY
    //------------------------------------------------------

    //enable static registers write enable
    outp32(UMCTL2_BASE + 0x328, 0x00000001);

    //disable dfi_lp_en_sr
    outp32(UMCTL2_BASE + 0x198, inp32(UMCTL2_BASE + 0x198) & ~(0x00000100));

    //disable static registers write enable
    outp32(UMCTL2_BASE + 0x328, 0x00000000);

    //enable powerdown_en and selfref_en
    outp32(UMCTL2_BASE + 0x30, inp32(UMCTL2_BASE + 0x30) | 0x3);

    //Set ddrc core clock gating circuit enable
    SYS->MISCFCR0 &= ~SYS_MISCFCR0_DDRCGDIS_Msk;

    /* branch to execute address */
    asm volatile
    (
        "b    main \n"
    );
}

void ddr_wk(void)
{
    //enable DDR PLL clock
    SYS_UnlockReg();
    CLK->PLL[DDRPLL].CTL1 &= ~CLK_PLLnCTL1_PD_Msk;

    //enable DDR AXI port0 clock and DDR AXI port5 clock
    CLK->SYSCLK0 |=  0x00000034;

    //Set ddrc core clock gating circuit bypass
    SYS->MISCFCR0 |= SYS_MISCFCR0_DDRCGDIS_Msk;

    //enable DDR core clock
    CLK->APBCLK0 |= CLK_APBCLK0_DDRPCKEN_Msk;

    //Wait DDR-PLL stable
    while((CLK->STATUS & 0x00000100) != 0x00000100);

    //exit DDR software self-refresh mode
    outp32(UMCTL2_BASE + 0x30, inp32(UMCTL2_BASE + 0x30) & ~(0x00000020));

    //wait DDR exit software self-refresh mode
    while((inp32(UMCTL2_BASE + 0x04) & 0x30) != 0x00);

    //enable DDR AXI port0, DDR AXI port5, and DDR AXI port6
    outp32(UMCTL2_BASE + 0x490, 0x00000001);
    outp32(UMCTL2_BASE + 0x800, 0x00000001);
    outp32(UMCTL2_BASE + 0x8b0, 0x00000001);

    //------------------------------------------------------
    // Flow to disable low power mode of DDR3/2 PHY
    //------------------------------------------------------

    //enable static registers write enable
    outp32(UMCTL2_BASE + 0x328, 0x00000001);

    //disable dfi_lp_en_sr
    outp32(UMCTL2_BASE + 0x198, inp32(UMCTL2_BASE + 0x198) & ~(0x00000100));

    //disable static registers write enable
    outp32(UMCTL2_BASE + 0x328, 0x00000000);

    //enable powerdown_en and selfref_en
    outp32(UMCTL2_BASE + 0x30, inp32(UMCTL2_BASE + 0x30) | 0x3);

    //Set ddrc core clock gating circuit enable
    SYS->MISCFCR0 &= ~SYS_MISCFCR0_DDRCGDIS_Msk;
}

void ddr_hw_pd(void)
{
    int  DDR_QCH_BPPORT0;
    int  DDR_QCH_BPPORT1;
    int  DDR_QCH_BPPORT2;
    int  DDR_QCH_BPPORT3;
    int  DDR_QCH_BPPORT4;
    int  DDR_QCH_BPPORT5;
    int  DDR_QCH_BPPORT6;

    //disable DDR CG bypass
    SYS->MISCFCR0 &= ~SYS_MISCFCR0_DDRCGDIS_Msk;

    //[11:8]= pg chain time period for voltage stable
    SYS->PMUCR &= 0xfffff0ff;

    //[15:12]= pg chain timeout
    SYS->PMUCR = (SYS->PMUCR & 0xffff0fff) | (0x2<<12);

    //[31:24]=DDR time out & delay
    SYS->DDRCQCSR &= 0x00ff7f7f;

    //[16]=DDRCQBYPAS, disable ddrc qch bypass
    SYS->DDRCQCSR &= ~SYS_DDRCQCSR_DDRCQBYPAS_Msk;

    DDR_QCH_BPPORT0=1;
    DDR_QCH_BPPORT1=1;
    DDR_QCH_BPPORT2=1;
    DDR_QCH_BPPORT3=1;
    DDR_QCH_BPPORT4=1;
    DDR_QCH_BPPORT5=1;
    DDR_QCH_BPPORT6=1;

    if( (inp32(UMCTL2_BASE + 0x490) & 0x1) & (CLK->SYSCLK0 >>4) & 0x1)
        DDR_QCH_BPPORT0 = 0;//A35
    if( (inp32(UMCTL2_BASE + 0x540) & 0x1) & (CLK->SYSCLK0 >>24) & 0x1)
        DDR_QCH_BPPORT1 = 0;//GFX
    if( (inp32(UMCTL2_BASE + 0x5f0) & 0x1) & (CLK->SYSCLK0 >>26) & 0x1)
        DDR_QCH_BPPORT2 = 0;//DC
    if( (inp32(UMCTL2_BASE + 0x6a0) & 0x1) & (CLK->SYSCLK0 >>25) & 0x1)
        DDR_QCH_BPPORT3 = 0;//VC8000
    if( (inp32(UMCTL2_BASE + 0x750) & 0x1) & (CLK->SYSCLK0 >>27) | ((CLK->PWRCTL >> 28) & 0x1))
        DDR_QCH_BPPORT4 = 0;//GMAC
    if( (inp32(UMCTL2_BASE + 0x800) & 0x1) & (CLK->SYSCLK0 >>29) | ((CLK->PWRCTL >> 30) & 0x1))
        DDR_QCH_BPPORT5 = 0;//CCAP
    if( (inp32(UMCTL2_BASE + 0x8b0) & 0x1) & ((CLK->SYSCLK0 >>5) & 0x1))
        DDR_QCH_BPPORT6 = 0;//system

    SYS->DDRCQCSR = (SYS->DDRCQCSR & ~0x7F)  |
                     (DDR_QCH_BPPORT0 << 0 ) |
                     (DDR_QCH_BPPORT1 << 1 ) |
                     (DDR_QCH_BPPORT2 << 2 ) |
                     (DDR_QCH_BPPORT3 << 3 ) |
                     (DDR_QCH_BPPORT4 << 4 ) |
                     (DDR_QCH_BPPORT5 << 5 ) |
                     (DDR_QCH_BPPORT6 << 6 );

    //L2 auto-flush
    SYS->PMUCR &= ~(0x1<<4);
}

void CA35_DPD(int mode)
{
    outp32((void *)0x2803fd00,0);
    outp32((void *)0x2803fd04,0);

    // Setup stack pointer in SRAM.
    asm volatile ("mov x30, sp");

    // set stack to 0x28004000
    asm volatile ("movz x6, #0x4000");
    asm volatile ("movk x6, #0x2800, lsl #0x10");
    asm volatile ("mov sp, x6");
    asm volatile ("stp X29,X30, [sp,#-0x10]!");

    if(mode) {
        // Flush L1
        disable_n_flush_cache(0);
        ddr_hw_pd();
    } else {
        SYS->DDRCQCSR |= 0x1007f;
        // Disable L2 flush by PMU
        SYS->PMUCR |= SYS_PMUCR_AUTOL2FDIS_Msk;
        // Flush L1 and L2
        disable_n_flush_cache(1);
        ddr_pd();
    }

    // Disable clock gating
    SYS->PMUCR |= SYS_PMUCR_A35PGEN_Msk;
    // Enable Power down
    SYS->PMUCR |= SYS_PMUCR_A35PDEN_Msk;

    if(mode) {
        // Turn on auto off bits
        //CLK->PWRCTL |= 0x00E0F800;
    } else {
        SYS_UnlockReg();
        SYS->PMUIEN |= SYS_PMUIEN_PMUIEN_Msk;
        CLK->PLL[DDRPLL].CTL1 |= CLK_PLLnCTL1_PD_Msk;
    }

    // Disable interrupt
    GIC_DisableDistributor(ENABLE_GRP0|ENABLE_GRP1);

    asm volatile ("wfi");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");

    GIC_EnableDistributor(ENABLE_GRP0|ENABLE_GRP1);

    asm volatile ("ldp  X29,X30, [sp], #0x10");
    asm volatile ("mov  sp, x30");
}

void CA35_NPD(int mode)
{
    // Setup stack pointer in SRAM
    asm volatile ("mov x30, sp");

    // set stack to 0x28004000
    asm volatile ("movz x6, #0x4000");
    asm volatile ("movk x6, #0x2800, lsl #0x10");
    asm volatile ("mov sp, x6");
    asm volatile ("stp X29,X30, [sp,#-0x10]!");

    if(mode == PD_HW_CTRL) {
        ddr_hw_pd();
        // Disable L2 flush by PMU
        SYS->PMUCR |= SYS_PMUCR_AUTOL2FDIS_Msk;
    } else {
        SYS->DDRCQCSR |= 0x1007f;
        // Disable L2 flush by PMU
        SYS->PMUCR |= SYS_PMUCR_AUTOL2FDIS_Msk;
        ddr_pd();
    }
    // Disable clock gating
    SYS->PMUCR &= ~SYS_PMUCR_A35PGEN_Msk;
    // Enable Power down
    SYS->PMUCR |= SYS_PMUCR_A35PDEN_Msk;

    //Switch CPU clock to HXT
    CLK->CLKSEL0 &= ~0x3;

    // wait clk switch complete
    while((CLK->STATUS & CLK_STATUS_HXTSTB_Msk) != CLK_STATUS_HXTSTB_Msk);

    // Turn off CPLL/DPLL
    CLK->PLL0CTL0 |= CLK_PLL0CTL0_PD_Msk;
    if(mode == PD_SW_CTRL)
    {
        SYS->PMUIEN |= SYS_PMUIEN_PMUIEN_Msk;
        CLK->PLL[DDRPLL].CTL1 |= CLK_PLLnCTL1_PD_Msk;
    }

    asm volatile ("ldr  x1, =__vectors");
    asm volatile ("msr  VBAR_EL3,x1");
    asm volatile ("wfi");

    // Wake up,  CPU = 800MHz
    CLK->PLL0CTL0 = 0x364;
    // Wait CPLL stable
    while((CLK->STATUS & CLK_STATUS_CAPLLSTB_Msk)!= CLK_STATUS_CAPLLSTB_Msk);
    //A35 use CPLL
    CLK->CLKSEL0 |= 1;

    if(mode == PD_SW_CTRL) {
        CLK->PLL[DDRPLL].CTL1 &= ~CLK_PLLnCTL1_PD_Msk;
        CLK->CLKSEL0 = 0x08000011;
        // wait PLL stable
        while( (CLK->STATUS & CLK_STATUS_DDRPLLSTB_Msk) != CLK_STATUS_DDRPLLSTB_Msk );
        ddr_wk();
    }

    asm volatile ("ldr  x1, =_vectors");
    asm volatile ("msr  VBAR_EL3, x1");

    asm volatile ("ldp  X29,X30, [sp], #0x10");
    asm volatile ("mov  sp, x30");
    GIC_EnableDistributor(ENABLE_GRP0|ENABLE_GRP1);
}

#include "MA35D1.h"

void ddr_pd(void)
{
    //Set ddrc core clock gating circuit bypass
    SYS->MISCFCR0 |= SYS_MISCFCR0_DDRCGDIS_Msk;

    //------------------------------------------------------
    // Flow to enable low power mode of DDR3/2 PHY
    //------------------------------------------------------
    //disable powerdown_en and selfref_en
    outp32((u32 *)(UMCTL2_BASE + 0x30), inp32((u32 *)(UMCTL2_BASE + 0x30))&~(0x3));

    //enable static registers write enable
    outp32((u32 *)(UMCTL2_BASE + 0x328), 0x00000001);

    //set value of dfi_lp_wakeup_sr
    outp32((u32 *)(UMCTL2_BASE + 0x198), (inp32((u32 *)(UMCTL2_BASE + 0x198))) & ~(0x0000f000) | 0x0000f000);

    //disable static registers write enable
    outp32((u32 *)(UMCTL2_BASE + 0x328), 0x00000000);

    //enable static registers write enable
    outp32((u32 *)(UMCTL2_BASE + 0x328), 0x00000001);

    //enable dfi_lp_en_sr
    outp32((u32 *)(UMCTL2_BASE + 0x198), (inp32((u32 *)(UMCTL2_BASE + 0x198))) &~(0x00000100) | 0x00000100);

    //disable static registers write enable
    outp32((u32 *)(UMCTL2_BASE + 0x328), 0x00000000);

    //-------------------------------------------------------------------------
    //wait DDR AXI port0 idle
    while((inp32((u32 *)(UMCTL2_BASE + 0x3fc)) & 0x00010001) != 0x00000000);

    //disable DDR AXI port0 ~ DDR AXI port6
    outp32((u32 *)(UMCTL2_BASE + 0x490), 0x00000000);  //AXI port0
    outp32((u32 *)(UMCTL2_BASE + 0x540), 0x00000000);  //AXI port1
    outp32((u32 *)(UMCTL2_BASE + 0x5f0), 0x00000000);  //AXI port2
    outp32((u32 *)(UMCTL2_BASE + 0x6a0), 0x00000000);  //AXI port3
    outp32((u32 *)(UMCTL2_BASE + 0x750), 0x00000000);  //AXI port4
    outp32((u32 *)(UMCTL2_BASE + 0x800), 0x00000000);  //AXI port5
    outp32((u32 *)(UMCTL2_BASE + 0x8b0), 0x00000000);  //AXI port6

    //wait DDR AXI port0 ~ DDR AXI port6 idle
    while((inp32((u32 *)(UMCTL2_BASE + 0x3fc)) & 0x003f003f) != 0x00000000);

    //enter DDR software self-refresh mode
    outp32((u32 *)(UMCTL2_BASE + 0x30), inp32((u32 *)(UMCTL2_BASE + 0x30)) | 0x20);

    //wait DDR enter software self-refresh mode
    while((inp32((u32 *)(UMCTL2_BASE + 0x04)) & 0x30) != 0x20);

    //disable DDR AXI port0 clock ~ DDR AXI port5 clock
    CLK->SYSCLK0 &= ~(0x7f000030);

    //disable DDR core clock
    CLK->APBCLK0 &= ~(0x40000000);

    //disable DDR PLL clock
    SYS_UnlockReg();
    CLK->PLL[DDRPLL].CTL1 |= 0x1;
}

void ddr_wk(void)
{
    //enable DDR AXI port0 clock and DDR AXI port5 clock
    CLK->SYSCLK0 |= 0x00000034;

    //Set ddrc core clock gating circuit bypass
    SYS->MISCFCR0 |= SYS_MISCFCR0_DDRCGDIS_Msk;

    //enable DDR PLL clock
    SYS_UnlockReg();
    CLK->PLL[DDRPLL].CTL1 &= ~(0x1);

    //enable DDR core clock
    CLK->APBCLK0 |= 0x40000000;

    //Wait DDR PLL stable
    while((CLK->STATUS  & CLK_STATUS_DDRPLLSTB_Msk) != CLK_STATUS_DDRPLLSTB_Msk);

    //exit DDR software self-refresh mode
    outp32((u32 *)(UMCTL2_BASE + 0x30), inp32((u32 *)UMCTL2_BASE + 0x30) & ~(0x00000020));

    //wait DDR exit software self-refresh mode
    while((inp32((u32 *)(UMCTL2_BASE + 0x04)) & 0x30) != 0x00);

    //enable DDR AXI port0, DDR AXI port5, and DDR AXI port6
    outp32((u32 *)(UMCTL2_BASE + 0x490), 0x00000001);  //AXI port0
    outp32((u32 *)(UMCTL2_BASE + 0x800), 0x00000001);  //AXI port5
    outp32((u32 *)(UMCTL2_BASE + 0x8b0), 0x00000001);  //AXI port6

    //------------------------------------------------------
    // Flow to disable low power mode of DDR3/2 PHY
    //------------------------------------------------------

    //enable static registers write enable
    outp32((u32 *)(UMCTL2_BASE + 0x328), 0x00000001);

    //disable dfi_lp_en_sr
    outp32((u32 *)(UMCTL2_BASE + 0x198), inp32((u32 *)(UMCTL2_BASE + 0x198)) &~(0x00000100));

    //disable static registers write enable
    outp32((u32 *)(UMCTL2_BASE + 0x328), 0x00000000);

    //enable powerdown_en and selfref_en
    outp32((u32 *)(UMCTL2_BASE + 0x30), inp32((u32 *)(UMCTL2_BASE + 0x30)) | 0x3);

    //Set ddrc core clock gating circuit enable
    SYS->MISCFCR0  &= ~SYS_MISCFCR0_DDRCGDIS_Msk;
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

    //[16]=DDRCQBYPAS,
    SYS->DDRCQCSR &= ~(0x1<<16);//disable ddrc qch bypass

//-----------------------------------------------------------------------------------
    //[6:0]=AXIQBYPAS,
    DDR_QCH_BPPORT0=1;
    DDR_QCH_BPPORT1=1;
    DDR_QCH_BPPORT2=1;
    DDR_QCH_BPPORT3=1;
    DDR_QCH_BPPORT4=1;
    DDR_QCH_BPPORT5=1;
    DDR_QCH_BPPORT6=1;

    if(  (inp32((u32 *)(0x404d0000+ 0x490)) & 0x1)   &  ((inp32((u32 *)(0x40460000+0x204))>>4 ) & 0x1) )
        DDR_QCH_BPPORT0 = 0;//A35
    if(  (inp32((u32 *)(0x404d0000+ 0x540)) & 0x1)   &  ((inp32((u32 *)(0x40460000+0x204))>>24) & 0x1) )
        DDR_QCH_BPPORT1 = 0;//GFX
    if(  (inp32((u32 *)(0x404d0000+ 0x5f0)) & 0x1)   &  ((inp32((u32 *)(0x40460000+0x204))>>26) & 0x1) )
        DDR_QCH_BPPORT2 = 0;//DC
    if(  (inp32((u32 *)(0x404d0000+ 0x6a0)) & 0x1)   &  ((inp32((u32 *)(0x40460000+0x204))>>25) & 0x1) )
        DDR_QCH_BPPORT3 = 0;//VC8000
    if(  (inp32((u32 *)(0x404d0000+ 0x750)) & 0x1)   &  (((inp32((u32 *)(0x40460000+0x204))>>27) | (inp32((u32 *)(0x40460000+0x200))>>28)) & 0x1) )
        DDR_QCH_BPPORT4 = 0;//GMAC
    if(  (inp32((u32 *)(0x404d0000+ 0x800)) & 0x1)   &  (((inp32((u32 *)(0x40460000+0x204))>>29) | (inp32((u32 *)(0x40460000+0x200))>>30)) & 0x1) )
        DDR_QCH_BPPORT5 = 0;//CCAP
    if(  (inp32((u32 *)(0x404d0000+ 0x8b0)) & 0x1)   &  ((inp32((u32 *)(0x40460000+0x204))>>5 ) & 0x1) )
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
    // Setup stack pointer in SRAM
    asm volatile ("mov x30, sp");

    // set stack to 0x28004000
    asm volatile ("movz x6, #0x4000");
    asm volatile ("movk x6, #0x2800, lsl #0x10");
    asm volatile ("mov sp, x6");
    asm volatile ("stp X29,X30, [sp,#-0x10]!");

    if(mode) {
        // HW flow
        disable_n_flush_cache(0);       // Flush L1
        ddr_hw_pd();
    } else {
        // SW flow
        SYS->DDRCQCSR |= 0x100ff;
        SYS->PMUCR |= (1 << 4);         // Disable L2 flush by PMU
        disable_n_flush_cache(1);       // Flush L1 and L2
        ddr_pd();
        CLK->PLL[DDRPLL].CTL1 = 0x21;       // Power MA35D1wn DDR PLL
    }

    /* Enable clock gating */
    SYS->PMUCR |= SYS_PMUCR_A35PGEN_Msk;

    /* Enable Power down */
    SYS->PMUCR |= SYS_PMUCR_A35PDEN_Msk;

    /* PMU Interrupt Enable */
    SYS->PMUIEN |= SYS_PMUIEN_PMUIEN_Msk;

    /* Disable interrupt forwarding */
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

    /* enable interrupt */
    GIC_EnableDistributor(ENABLE_GRP0|ENABLE_GRP1);

    asm volatile ("ldp  X29,X30, [sp], #0x10");
    asm volatile ("mov  sp, x30");  // restore original stack pointer in DDR
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

    if(mode) {
        // HW flow
        ddr_hw_pd();
        /* Disable L2 flush by PMU */
        SYS->PMUCR |= SYS_PMUCR_AUTOL2FDIS_Msk;
    } else {
        // SW flow
        SYS->DDRCQCSR |= 0x100ff;
        /* Disable L2 flush by PMU */
        SYS->PMUCR |= SYS_PMUCR_AUTOL2FDIS_Msk;
        ddr_pd();
        CLK->PLL[DDRPLL].CTL1 = 0x21;      // Power MA35D1wn DDR PLL
    }

    /* Disable clock gating */
    SYS->PMUCR &= ~SYS_PMUCR_A35PGEN_Msk;

    /* Enable Power down */
    SYS->PMUCR |= SYS_PMUCR_A35PDEN_Msk;

    /* PMU Interrupt Enable  */
    SYS->PMUIEN |= SYS_PMUIEN_PMUIEN_Msk;

    asm volatile ("ldr  x1, =__vectors");
    asm volatile ("msr  VBAR_EL3,x1");
    asm volatile ("wfi");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");

    // wake up
    if(mode) {

    } else {
        CLK->PLL[DDRPLL].CTL1 = 0x20;
        // wait DDRPLL stable
        while( (CLK->STATUS & (1<<8)) != (1<<8) );
        ddr_wk();
    }

    asm volatile ("ldr  x1, =_vectors");
    asm volatile ("msr  VBAR_EL3, x1");

    asm volatile ("ldp  X29,X30, [sp], #0x10");
    asm volatile ("mov  sp, x30");  // restore original stack pointer in DDR
    GIC_EnableDistributor(ENABLE_GRP0|ENABLE_GRP1);  // enable interrupt. was disabled in vector table in SRAM.
}

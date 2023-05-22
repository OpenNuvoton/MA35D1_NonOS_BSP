#include "MA35D1.h"
#include "irq_ctrl.h"
#include "clk.h"
#include "sys.h"
#include "cpu.h"
#include "mmu.h"


#define UMCTL2_BA       0x404d0000
#define DDRPHY_BA       0x404c0000
#define TSI_SYS_BA      0x40360000
#define TSI_CLK_BA      0x40360200
#if 0
#define SDRAM_RANK0_BA  0x80000000
#define SDRAM_RANK1_BA  0xa0000000
#define SRAM0_BA        0x24000000
#define SRAM1_BA        0x28000000

#define WRHO_0_BA       0x403a0000
#endif


// Disable AXI port clock and DDR core clock
void ddr_pd(void)
{
    uint32_t i, reg;

    //disable DDR core clock gating
    SYS->MISCFCR0 |= (0x1 << 23);

    // Enable AXI clock to disable AXI port later
    // AXI 0: DDR0CKEN(CLK_SYSCLS0[4])
    // AXI 1: GFXEN(CLK_SYSCLS0[24])
    // AXI 2: DCUEN(CLK_SYSCLS0[26])
    // AXI 3: VC8KEN(CLK_SYSCLS0[25])
    // AXI 4: GMAC0EN, GMAC1EN(CLK_SYSCLS0[27], CLK_SYSCLS0[28])
    // AXI 5: CCAP0EN, CCAP1EN(CLK_SYSCLS0[29], CLK_SYSCLS0[30])
    // AXI 6: DDR6CKEN (CLK_SYSCLS0[5])
    // AXI 7: Use TSI HCLK (CLK_SYSCLS0[2])
    reg = CLK->SYSCLK0;
    CLK->SYSCLK0 |= 0x7f000034;

    //set PCTRL_1
    outp32((u32 *)(UMCTL2_BA) + 0x540,0x00000001);

    //set PCTRL_2
    outp32((u32 *)(UMCTL2_BA) + 0x5f0,0x00000001);

    //set PCTRL_3
    outp32((u32 *)(UMCTL2_BA) + 0x6a0,0x00000001);

    //set PCTRL_4
    outp32((u32 *)(UMCTL2_BA) + 0x750,0x00000001);

    //set PCTRL_5
    outp32((u32 *)(UMCTL2_BA) + 0x800,0x00000001);


    //cortex_a35_core_pwr_dwn();
    //cortex_a35_cluster_pwr_dwn();


    for(i = 0; i < 0x20; i++);

    // Disable AXI port 0~7
    outp32((u32 *)(UMCTL2_BA + 0x490 + 0)       , 0x00000000); // disable AXI port 0
    outp32((u32 *)(UMCTL2_BA + 0x490 + (0xb0*1)), 0x00000000); // disable AXI port 1
    outp32((u32 *)(UMCTL2_BA + 0x490 + (0xb0*2)), 0x00000000); // disable AXI port 2
    outp32((u32 *)(UMCTL2_BA + 0x490 + (0xb0*3)), 0x00000000); // disable AXI port 3
    outp32((u32 *)(UMCTL2_BA + 0x490 + (0xb0*4)), 0x00000000); // disable AXI port 4
    outp32((u32 *)(UMCTL2_BA + 0x490 + (0xb0*5)), 0x00000000); // disable AXI port 5
    outp32((u32 *)(UMCTL2_BA + 0x490 + (0xb0*6)), 0x00000000); // disable AXI port 6
    outp32((u32 *)(UMCTL2_BA + 0x490 + (0xb0*7)), 0x00000000); // disable AXI port 7

    // wait AXI port 0~7 idle
    while((inp32((u32 *)(UMCTL2_BA + 0x3fc)) & 0x00ff00ff) != 0x00000000);

    // Enter DDR S/W self-refresh mode // PWRCTL[5]
    outp32((u32 *)(UMCTL2_BA + 0x30), inpw((u32 *)(UMCTL2_BA + 0x30)) | (0x1 << 5));

    // wait DDR enter self-refresh mode // STAT[5:4]
    while((inp32((u32 *)(UMCTL2_BA + 0x4)) & 0x30) != 0x20);

    // restore SYSCLK0 setting
    CLK->SYSCLK0 = reg;

    // TSI unlock
    outp32((u32 *)(TSI_SYS_BA + 0x100),0x59);
    outp32((u32 *)(TSI_SYS_BA + 0x100),0x16);
    outp32((u32 *)(TSI_SYS_BA + 0x100),0x88);
    // disable AXI port 7 clock // TSI clock
    outp32((u32 *)(TSI_CLK_BA + 0x0), inp32((u32 *)(TSI_CLK_BA + 0x0)) &~ (0x4));

    // disable DDR clock // DDRPCKEN(CLK_APBCLK0[30])
    CLK->APBCLK0 &= ~(1 << 30);

    for(i = 0; i < 0x10; i++);
}


void ddr_wk(void)
{
	uint32_t reg;


	// enable AXI clock
    reg = CLK->SYSCLK0;
    CLK->SYSCLK0 |= 0x7f000034;

    // TSI unlock  -- this is for the case TSI disabled
    outp32((u32 *)(TSI_SYS_BA + 0x100),0x59);
    outp32((u32 *)(TSI_SYS_BA + 0x100),0x16);
    outp32((u32 *)(TSI_SYS_BA + 0x100),0x88);
    // enable AXI port 7 clock // TSI clock
    outp32((u32 *)(TSI_CLK_BA + 0x0), inp32((u32 *)(TSI_CLK_BA + 0x0)) | (0x4));

    //polling TSI HIRC stable
    while((inp32((u32 *)(TSI_CLK_BA + 0x50)) & 0x10) != 0x10);

    // enable DDR clock // DDRPCKEN(CLK_APBCLK0[30])
    CLK->APBCLK0 |= (0x1 << 30);

    // exit from DDR S/W self-refresh mode // PWRCTL[5]
    outp32((u32 *)(UMCTL2_BA + 0x30), inpw((u32 *)(UMCTL2_BA + 0x30)) &~ (0x1 << 5));


    // wait DDR exit self-refresh mode // STAT[5:4]
    while((inp32((u32 *)(UMCTL2_BA + 0x4)) & 0x30) != 0x00);

    //set PCTRL_0
    outp32((u32 *)(UMCTL2_BA + 0x490),0x00000001);

    //set PCTRL_1
    outp32((u32 *)(UMCTL2_BA + 0x540),0x00000001);

    //set PCTRL_2
    outp32((u32 *)(UMCTL2_BA + 0x5f0),0x00000001);

    //set PCTRL_3
    outp32((u32 *)(UMCTL2_BA + 0x6a0),0x00000001);

    //set PCTRL_4
    outp32((u32 *)(UMCTL2_BA + 0x750),0x00000001);

    //set PCTRL_5
    outp32((u32 *)(UMCTL2_BA + 0x800),0x00000001);

    //set PCTRL_6
    outp32((u32 *)(UMCTL2_BA + 0x8b0),0x00000001);

    //set PCTRL_7
    outp32((u32 *)(UMCTL2_BA + 0x960),0x00000001);

    // restore SYSCLK0 setting
    CLK->SYSCLK0 = reg;


}


// Q ch
void ddr_hw_pd(void)
{
    int  DDR_QCH_BPPORT0;
    int  DDR_QCH_BPPORT1;
    int  DDR_QCH_BPPORT2;
    int  DDR_QCH_BPPORT3;
    int  DDR_QCH_BPPORT4;
    int  DDR_QCH_BPPORT5;
    int  DDR_QCH_BPPORT6;
    int  DDR_QCH_BPPORT7;
    int  TSI_NPD;

    //disable DDR CG bypass 
    SYS->MISCFCR0 &= ~(0x1<<23);

    //[11:8]= pg chain time period for voltage stable
    SYS->PMUCR &= 0xfffff0ff;

    //[15:12]= pg chain timeout
    SYS->PMUCR = (SYS->PMUCR & 0xffff0fff) | (0x2<<12);

    //[31:24]=DDR time out & delay
    SYS->DDRCQCSR &= 0x00ffffff;

    //[16]=DDRCQBYPAS,
    SYS->DDRCQCSR &= ~(0x1<<16);//disable ddrc qch bypass

//-----------------------------------------------------------------------------------
    //[7:0]=AXIQBYPAS,
    DDR_QCH_BPPORT0=1;
    DDR_QCH_BPPORT1=1;
    DDR_QCH_BPPORT2=1;
    DDR_QCH_BPPORT3=1;
    DDR_QCH_BPPORT4=1;
    DDR_QCH_BPPORT5=1;
    DDR_QCH_BPPORT6=1;
    DDR_QCH_BPPORT7=1;
    TSI_NPD=0;


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


    SYS->DDRCQCSR = (SYS->DDRCQCSR & ~0xFF)  |
                     (DDR_QCH_BPPORT0 << 0 ) |
                     (DDR_QCH_BPPORT1 << 1 ) |
                     (DDR_QCH_BPPORT2 << 2 ) |
                     (DDR_QCH_BPPORT3 << 3 ) |
                     (DDR_QCH_BPPORT4 << 4 ) |
                     (DDR_QCH_BPPORT5 << 5 ) |
                     (DDR_QCH_BPPORT6 << 6 ) |
                     (DDR_QCH_BPPORT7 << 7 );	//disable ddr 8 ports qch bypass


    //L2 auto-flush
    SYS->PMUCR &= ~(0x1<<4);

}


void CA35_DPD(int mode)
{

	// Setup stack pointer in SRAM. In case DDR PD failed?
	asm volatile ("mov x30, sp");

	// set stack to 0x28004000
	asm	volatile ("movz x6, #0x4000");
	asm	volatile ("movk x6, #0x2800, lsl #0x10");
	asm volatile ("mov sp, x6");
	asm volatile ("stp X29,X30, [sp,#-0x10]!");

	// Disable interrupt
	GIC_DisableDistributor(ENABLE_GRP0|ENABLE_GRP1);

	if(mode) {
		// HW flow
		disable_n_flush_cache(0);		// Flush L1
		ddr_hw_pd();
	} else {
		// SW flow
		//SYS->DDRCQCSR &= ~0x100ff;		// Bypass all Q ch
		SYS->DDRCQCSR |= 0x100ff;
		SYS->PMUCR |= (1 << 4);			// Disable L2 flush by PMU
		disable_n_flush_cache(1);		// Flush L1 and L2
		ddr_pd();
		CLK->PLL[2].CTL1 = 0x21;			// Power MA35D1wn DDR PLL

	}
    //[0]=pg_eanble
	SYS->PMUCR |= (1 << 0);				// Enable clock gating

    //[16]=pd_eanble
	SYS->PMUCR |= (1 << 16);			// Enable PD

    //[16]=PMUIEN,
	SYS->PMUIEN |= (1 << 0);

    asm volatile ("wfi");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");

    GIC_EnableDistributor(ENABLE_GRP0|ENABLE_GRP1);  // enable interrupt.

    asm volatile ("ldp	X29,X30, [sp], #0x10");
    asm volatile ("mov 	sp, x30");  // restore original stack pointer in DDR

	printf("what the hell@@\n");
}


void CA35_NPD(int mode)
{

	// Setup stack pointer in SRAM
	asm volatile ("mov x30, sp");

	// set stack to 0x28004000
	asm	volatile ("movz x6, #0x4000");
	asm	volatile ("movk x6, #0x2800, lsl #0x10");
	asm volatile ("mov sp, x6");
	asm volatile ("stp X29,X30, [sp,#-0x10]!");

	if(mode) {
		// HW flow
		ddr_hw_pd();
		SYS->PMUCR |= (1 << 4);			// Disable L2 flush by PMU
	} else {
		// SW flow
		//SYS->DDRCQCSR &= ~0x100ff;		// Bypass all Q ch
		SYS->DDRCQCSR |= 0x100ff;
		SYS->PMUCR |= (1 << 4);			// Disable L2 flush by PMU
		ddr_pd();
		CLK->PLL[2].CTL1 = 0x21;  	   // Power MA35D1wn DDR PLL
	}
    //[0]=pg_eanble
	SYS->PMUCR &= ~(1 << 0);			// Disable clock gating

    //[16]=pd_eanble
	SYS->PMUCR |= (1 << 16);			// Enable Power MA35D1wn

    //[16]=PMUIEN,
	SYS->PMUIEN |= (1 << 0);


	asm volatile ("ldr	x1, =__vectors");
	asm volatile ("msr	VBAR_EL3,x1");
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
    	// MA35D1 anything?
    } else {
    	CLK->PLL[2].CTL1 = 0x20;
    	// wait PLL stable
    	while( (CLK->STATUS & (1<<8)) != (1<<8) );
    	ddr_wk();
    }

	asm volatile ("ldr	x1, =_vectors");
	asm volatile ("msr	VBAR_EL3, x1");

    //mmu_enable();

    asm volatile ("ldp	X29,X30, [sp], #0x10");
    asm volatile ("mov 	sp, x30");  // restore original stack pointer in DDR
    GIC_EnableDistributor(ENABLE_GRP0|ENABLE_GRP1);  // enable interrupt. was disabled in vector table in SRAM.
}



/*************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    This loader is designed for custom DDR situation. MA35D1 IBR will
 *           load this loader to SRAM and run. This loader will then initialize
 *           the custom DDR and then load application images from SPI NAND, NAND,
 *           or SD/eMMC to DRAM for execution.
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NuMicro.h"
#include "ddr_def.h"
#include "custom_ddr.h"
#include "loader.h"

/*--------------------------------------------------------------------------*/
/* Parameters Define                                                        */
/*--------------------------------------------------------------------------*/
//#define DDR_667MBPS
//#define DDR_800MBPS

/*--------------------------------------------------------------------------*/
/* Global Variables                                                         */
/*--------------------------------------------------------------------------*/
unsigned int (*fBLfunc)();

uint64_t volatile gStartTime = 0;
struct mmc mmcInfo;

/*--------------------------------------------------------------------------*/
/* Delay execution for given amount of ticks */
void Delay0(uint32_t ticks)  {
	uint32_t tgtTicks = msTicks0 + ticks;             // target tick count to delay
	while (msTicks0 < tgtTicks);
}

/***********************************************/
void DelayMicrosecond(unsigned int count)
{
	unsigned long volatile stime;

	stime = raw_read_cntpct_el0();
	/* wait timeout */
	while(1)
	{
		if ((raw_read_cntpct_el0() - stime) > count*12)
		{
			break;
		}
	}
}


void UART0_Init()
{
    /* Enable UART0 clock */
	CLK->APBCLK0 |= CLK_APBCLK0_UART0CKEN_Msk;
	CLK->CLKSEL2 &= ~CLK_CLKSEL2_UART0SEL_Msk;
	CLK->CLKDIV1 &= ~CLK_CLKDIV1_UART0DIV_Msk;

    /* Set multi-function pins */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Initial UART to 115200-8n1 for print message */
	UART0->INTEN = 0;
	UART0->LINE = 0x7;
	UART0->BAUD = 0x300000CE;
}

void ma35d1_ddr_setting(void)
{
	uint32_t i;
	uint64_t ddr_reg_address;
	uint32_t value;
	uint32_t u32TimeOut1 = 0, u32TimeOut2 = 0, u32TimeOut3 = 0;

	for (i=0; i<sizeof(custom_ddr)/sizeof(uint32_t); i++)
	{
		ddr_reg_address = (uint32_t)nvt_ddr_init_setting[i].base + (uint32_t)nvt_ddr_init_setting[i].offset;
		value =  *((uint32_t *)(((uintptr_t)&custom_ddr) + nvt_ddr_init_setting[i].init_flow_offset));

		*(volatile uint32_t *)(ddr_reg_address) = value;

		if (i == 84) // 0xf08 //DDRCTRL
		{
			//de-assert reset signals of DDR memory controller
			SYS->IPRST0 &= 0x8fffffff;
			while ((SYS->IPRST0 & 0x20000000) != 0x00000000);
		}

		if (i == 100) // 0x184 //DDRPHY
		{
			//polling PGSR0 (addr=4) to 0x0000000f
			while((inp32(DDRPHY_BASE + 0x010) & 0x0000000f) != 0x0000000f);
			{
				u32TimeOut1++;
			}
		}

		if (i == 104) // 0x04  // DDRPHY
		{
			//polling PGSR0 (addr=4) to 0xb0000f5f
			while((inp32(DDRPHY_BASE + 0x010) & 0xffffff5f) != 0xb0000f5f);
			{
				u32TimeOut2++;
			}

			//polling MCTL2 STAT to 0x00000001
			while((inp32(UMCTL2_BASE + 0x004) & 0x00000003) != 0x00000001);
			{
				u32TimeOut3++;
			}
		}
	}

	while((inp32(UMCTL2_BASE + 0x324) & 0x00000001) != 0x00000001);
	sysprintf(" DDR initial Finish!\n");
}


void ddr_init(void)
{
	uint32_t volatile reg_clksel0, reg_sysclk0, reg_apbclk0;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* set DDR-PLL */
#if defined(DDR_800MBPS)
    CLK->PLL[2].CTL0 = 0x0f041029;
    CLK->PLL[2].CTL1 = 0x40000050;
    CLK->PLL[2].CTL2 = 0x000048A3;

#elif defined(DDR_667MBPS)
	CLK->PLL[2].CTL0 = 0x0f041022;
	CLK->PLL[2].CTL1 = 0x947ae150;
	CLK->PLL[2].CTL2 = 0x000048A3;

#else
	CLK->PLL[2].CTL0 = 0x0F04102C;
	CLK->PLL[2].CTL1 = 0x6B851E40;
	CLK->PLL[2].CTL2 = 0x000048A3;
#endif

    /*polling CA-PLL, SYS-PLL, DDR-PLL stable */
    gStartTime = raw_read_cntpct_el0();
	while(1)
	{
		if ((CLK->STATUS & 0x100) == 0x100)
			break;
		if ((raw_read_cntpct_el0() - gStartTime) > 120000)   /* 10 ms */
		{
			printf("DDR-PLL fail, 0x%x\n", CLK->STATUS);
			break;
		}
	}

	/* backup registers */
	reg_clksel0 = CLK->CLKSEL0;
	reg_sysclk0 = CLK->SYSCLK0;
	reg_apbclk0 = CLK->APBCLK0;

	/* set SYS_CLK0, DCUltra, and GFX clock from SYS_PLL, instead of EPLL */
	CLK->CLKSEL0 = 0xd000015;
	//Set TAHBCKEN,CM4CKEN,CA35CKEN,DDR6CKEN,GFXCKEN,VC8KCKEN,DCUCKEN,GMAC0CKEN,GMAC1CKEN,CAP0CKEN,CAP1CKEN
	CLK->SYSCLK0 |= 0x7F000037;
	CLK->APBCLK0 |= 0x40000000;

	/* DDR control register clock gating disable */
	SYS->MISCFCR0 |= 0x800000;
	/* de-assert present of MCTL2 */
	SYS->IPRST0 &= 0xafffffff;
	while ((SYS->IPRST0 & 0x50000000) != 0x00000000);
	SYS->IPRST0 |= 0x20000000; /* set MCTLCRST to 1 */

	ma35d1_ddr_setting();

	outp32(UMCTL2_BASE+0x490, 0x1);
	outp32(UMCTL2_BASE+0x540, 0x1);
	outp32(UMCTL2_BASE+0x5f0, 0x1);
	outp32(UMCTL2_BASE+0x6a0, 0x1);
	outp32(UMCTL2_BASE+0x750, 0x1);
	outp32(UMCTL2_BASE+0x800, 0x1);
	outp32(UMCTL2_BASE+0x8b0, 0x1);

	SYS->MISCFCR0 &= ~0x800000; /* DDR control register clock gating enable */
	/* restore registers */
	CLK->SYSCLK0 = reg_sysclk0;
	CLK->APBCLK0 = reg_apbclk0;
	CLK->CLKSEL0 = reg_clksel0;
}

void LoadSpiNand(uint32_t offset, uint32_t size, uint32_t load_addr)
{
	uint32_t volatile page, addr = load_addr;
	uint32_t volatile PageToDownload, StartPage, StartBlock;
	int volatile BlockCount;

	StartPage = offset / SPINAND_PAGE_SIZE;
	StartBlock = StartPage / SPINAND_PAGE_PER_BLOCK;
	PageToDownload = size / SPINAND_PAGE_SIZE;
	if (size % SPINAND_PAGE_SIZE)
		PageToDownload++;
	BlockCount = PageToDownload / SPINAND_PAGE_PER_BLOCK;
	if (PageToDownload % SPINAND_PAGE_PER_BLOCK)
		BlockCount++;
	page = StartPage % SPINAND_PAGE_PER_BLOCK;

	//sysprintf("start: %d, block: %d, count: %d / %d\n", StartPage, StartBlock, PageToDownload, BlockCount);

	spiNandReset();
	gStartTime = raw_read_cntpct_el0();
	while(1)
	{
		WDT_RESET_COUNTER(WDT1);
		if ((spiNandGetStatus(0xc0) & 0x1) == 0)
			break;
		if ((raw_read_cntpct_el0() - gStartTime) > 6000000) /* 500ms */
		{
			sysprintf("Reset timeout\n");
			return;
		}
	}

	while (1)
	{
		WDT_RESET_COUNTER(WDT1);
		if (spiNandIsBlockValid(StartBlock))
		{
			while (PageToDownload > 0)
			{
				WDT_RESET_COUNTER(WDT1);
				if (spiNandRead(StartPage, SPINAND_PAGE_SIZE, (unsigned int *)(uint64_t)addr))
				{
					sysprintf("spi-nand read error!\n");
					return;
				}
				addr += SPINAND_PAGE_SIZE;
				page++;
				PageToDownload--;
				StartPage++;
				if (page >= SPINAND_PAGE_PER_BLOCK)
				{
					page = 0;
					break;
				}
			}
			if ((--BlockCount) <= 0)
				break;
		}
		StartBlock++;
	}
}

void LoadNand(uint32_t offset, uint32_t size, uint32_t load_addr)
{
	uint32_t volatile page, addr = load_addr;
	uint32_t volatile PageToDownload, StartPage, StartBlock;
	int volatile BlockCount, i;

	//sysprintf("offset: 0x%x, size: %d, addr: 0x%x\n", offset, size, load_addr);
	StartPage = offset / tNAND.uPageSize;
	StartBlock = StartPage / tNAND.uPagePerBlock;
	PageToDownload = size / tNAND.uPageSize;
	if (size % tNAND.uPageSize)
		PageToDownload++;
	BlockCount = PageToDownload / tNAND.uPagePerBlock;
	if (PageToDownload % tNAND.uPagePerBlock)
		BlockCount++;
	page = StartPage % tNAND.uPagePerBlock;

	//sysprintf("start: %d, block: %d, count: %d / %d\n", StartPage, StartBlock, PageToDownload, BlockCount);
	while (1)
	{
		WDT_RESET_COUNTER(WDT1);
		if (nfiIsBlockValid(&tNAND, StartBlock))
		{
			while (PageToDownload > 0)
			{
				WDT_RESET_COUNTER(WDT1);
                if (nfiPageRead(StartPage, (uint8_t *)(uint64_t)addr))
                	break;
				addr += tNAND.uPageSize;
				page++;
				PageToDownload--;
				StartPage++;
				if (page >= tNAND.uPagePerBlock)
				{
					page = 0;
					break;
				}
			}
			if ((--BlockCount) <= 0)
				break;
		}
		StartBlock++;
	}
}

int main()
{
	uint32_t reg_por;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Initialize UART to 115200-8n1 for print message */
    UART0_Init();

    /* Enable timer control */
    EL0_SetControl(1U);			// non-secure timer

    sysprintf("MA35D1 baremetal loader...\n");

    /* 1. initial DDR */
    WDT_RESET_COUNTER(WDT1);
	ddr_init();

    /* 2. load application from storage */
    reg_por = SYS->PWRONOTP;
    if ((reg_por & 0x1) == 0)	/* boot source from pin */
    {
    	reg_por = (reg_por & ~0xff00) | (SYS->PWRONPIN << 8);
    }

    switch (reg_por & 0xC00)
    {
    case 0x000: /* SPI boot */
    	CLK->CLKSEL4 &= ~CLK_CLKSEL4_QSPI0SEL_Msk;	/* QSPI0 from PCLK */
    	CLK->SYSCLK1 |= CLK_SYSCLK1_GPDCKEN_Msk;
    	CLK->APBCLK1 |= CLK_APBCLK1_QSPI0CKEN_Msk;
    	SYS->GPD_MFPL = 0x00005555;
		/* Set PD.1(QSPI0_CLK) high slew rate */
    	PD->SLEWCTL = 0x4;
		/* PD4, PD5 output high */
    	PD->MODE = 0x500;
    	PD->DOUT |= 0x30;

    	if (reg_por & 0x2)
    		QSPI0->CLKDIV = 3;	/* Set SPI0 clock to 50 MHz => PCLK(200)/(n+1) */
    	else
    		QSPI0->CLKDIV = 5;	/* Set SPI0 clock to 30 MHz => PCLK(200)/(n+1) */
    	QSPI0->SSCTL = 0;
    	QSPI0->CTL = 0x805;
        WDT_RESET_COUNTER(WDT1);

        if (reg_por & 0x8000)	/* SPI_NOR */
    	{
			sysprintf("SPI-NOR\n");
			spiNorReset();
			spiRead((uint32_t)APP_OFFSET_SPINOR, APP_SIZE, (uint32_t *)APP_EXE_ADDR);
    	}
    	else	/* SPI_NAND */
    	{
			sysprintf("SPI-NAND\n");
			LoadSpiNand(APP_OFFSET_SPINAND, APP_SIZE, APP_EXE_ADDR);
    	}
    	break;

    case 0x400: /* SD/eMMC boot */
		if (reg_por & 0x4000)	/* SD1 */
		{
			sysprintf("SD1\n");
			CLK->CLKSEL0 &= ~CLK_CLKSEL0_SD1SEL_Msk;	/* SYS-PLL */
			CLK->SYSCLK0 |= CLK_SYSCLK0_SDH1EN_Msk;
			SYS->GPJ_MFPL = 0x66666666;
			SYS->GPJ_MFPH = 0x00006666;
			mmcInfo.base = SDH1_BASE;
		}
		else	/* SD0 */
		{
			sysprintf("SD0\n");
			CLK->CLKSEL0 &= ~CLK_CLKSEL0_SD0SEL_Msk;	/* SYS-PLL */
			CLK->SYSCLK0 |= CLK_SYSCLK0_SDH0EN_Msk;
			SYS->GPC_MFPL = 0x66666666;
			SYS->GPC_MFPH = 0x00006666;
			mmcInfo.base = SDH0_BASE;
		}
		if (reg_por & 0x8000)
			mmcInfo.bus_width = 1;	/* 8-bit */
		else
			mmcInfo.bus_width = 0;	/* 4-bit */
		sdhInit(&mmcInfo);
		{
			uint32_t volatile start = APP_OFFSET_EMMC / 512;
			uint32_t volatile count;

			count = APP_SIZE / 512;
			if ((APP_SIZE % 512) != 0)
				count++;

			sdhReadBlocks(&mmcInfo, (void *)(uint64_t)APP_EXE_ADDR, start, count);
		}
    	break;

    case 0x800: /* NAND boot */
    	CLK->SYSCLK0 |= CLK_SYSCLK0_NANDEN_Msk;
    	SYS->GPA_MFPL = 0x66666666;
    	SYS->GPA_MFPH = 0x06666666;
        WDT_RESET_COUNTER(WDT1);

		/* Initial NAND */
        nfiOpen();

		LoadNand(APP_OFFSET_SPINAND, APP_SIZE, APP_EXE_ADDR);
		break;

    default:
    	sysprintf("Boot interface 0x%x not supported\n", reg_por);
    	while(1);
    }

    /* branch to application */
	sysprintf("finish\n\n");

    SYS->GPA_MFPL = 0;
    SYS->GPA_MFPH = 0;
    SYS->GPC_MFPL = 0;
    SYS->GPC_MFPH = 0;
    SYS->GPD_MFPL = 0;
    SYS->GPJ_MFPL = 0;
    SYS->GPJ_MFPH = 0;

    CLK->SYSCLK0 = 0x35;
    CLK->SYSCLK1 = 0x5f20;
    CLK->APBCLK0 = 0x40001000;
    CLK->APBCLK1 = 0x70000;
    CLK->APBCLK2 = 0x8;
	/* invalidate I cache */
	icialluis();

	/* wait TX FIFO empty and timeout */
	gStartTime = raw_read_cntpct_el0();
	while(1)
	{
		if ((UART0->FIFOSTS & 0x400000) == 0x400000)
			break;
		if ((raw_read_cntpct_el0() - gStartTime) > 24000)   /* 2ms */
		{
			break;
		}
	}
	/* Disable Generic Timer and set load value */
	EL0_SetControl(0);

	fBLfunc = (unsigned int (*)())(APP_EXE_ADDR);
	fBLfunc();
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/

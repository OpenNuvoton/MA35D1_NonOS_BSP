/*************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "stdio.h"
#include "string.h"
#include "NuMicro.h"
#include "ddr_def.h"
#include "custom_ddr.h"


/*--------------------------------------------------------------------------*/
/* Parameters Define                                                        */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* Global Variables                                                         */
/*--------------------------------------------------------------------------*/
#define TIME_OUT_VALUE 0x20000

/*--------------------------------------------------------------------------*/


/***********************************************/

int DDR_RW_Test(uint32_t start_addr, uint32_t size, uint32_t* data, uint32_t mod)
{
	uint32_t saddr,eaddr,midsize,rdata;
	uint32_t i;
    saddr=start_addr;
    eaddr=start_addr+size;
    midsize=size/2;

    for(i=0;i<midsize;i+=4)
    {
        *((volatile uint32_t *)((uint64_t)saddr+i))=data[(i>>2)%mod];
    }

    for(i=0;i<midsize;i+=4)
    {
        *((volatile uint32_t *)((uint64_t)eaddr-i-4))=data[(i>>2)%mod];
    }

    for(i=0;i<midsize;i+=4)
    {
        rdata=*((volatile uint32_t *)((uint64_t)saddr+i));
        if(rdata!=data[(i>>2)%mod])
        {
        	sysprintf("Error:addr=0x%08x write=0x%08x read=0x%08x\n", saddr+i, data[(i>>2)%mod], rdata);
            return -1;
        }
    }

    for(i=0;i<midsize;i+=4)
    {
        rdata=*((volatile uint32_t *)((uint64_t)eaddr-i-4));

        if(rdata!=data[(i>>2)%mod])
        {
        	sysprintf("Error:addr=0x%08x write=0x%08x read=0x%08x\n", eaddr-i-4, data[(i>>2)%mod], rdata);
            return -1;
        }
    }

    return 1;
}

void ddr_test(void)
{
    uint32_t Data[4];
    uint64_t BASEADDR = 0x80000000;
    uint64_t RAM_SIZE;
    uint32_t u23Sel_DDR_Size;

    /* Unlock protected registers */
    SYS_UnlockReg();

    sysprintf("\n Select test Size: ");
    sysprintf("\n 1.   1G ");
    sysprintf("\n 2. 512M ");
    sysprintf("\n 3. 256M ");
    sysprintf("\n 4. 128M ");
    sysprintf("\n 5.   2G ");
    sysprintf("\n");

    u23Sel_DDR_Size = getchar();

    switch(u23Sel_DDR_Size)
    {
    	case '1':
    		sysprintf("\n DDR SIZE select: 1G \n");
    		RAM_SIZE = 0x40000000; //1G
    		break;
    	case '2':
    		sysprintf("\n DDR SIZE select: 512M \n");
    		RAM_SIZE = 0x40000000/2; // 512M
    		break;
    	case '3':
    		sysprintf("\n DDR SIZE select: 256M \n");
    		RAM_SIZE =0x40000000/4; //256M
    		break;
    	case '4':
    		sysprintf("\n DDR SIZE select: 128M \n");
    		RAM_SIZE =  0x40000000/8; //128M
    		break;
    	case '5':
    		sysprintf("\n DDR SIZE select: 2G \n");
    		RAM_SIZE = 0x80000000; //2G
    		break;

    }

    sysprintf(" DDR Test Start \n\n");

	//DDR test
	Data[0]=0x00000000;Data[1]=0x55555555;Data[2]=0xaaaaaaaa;Data[3]=0xffffffff;
	if(DDR_RW_Test(BASEADDR, RAM_SIZE, Data, 4)<0)
	{
		sysprintf("\n test fail \n");
    }
	else
	{
		sysprintf("\n test pass \n");
	}

}

void Show_Status(void)
{
	uint32_t u32PGSR0_Reg = 0x0;

	u32PGSR0_Reg = inp32((void *)DDRPHY_BASE + 0x10);

	sysprintf("\n u32PGSR_Reg = 0x%x \n", u32PGSR0_Reg);

	sysprintf("\nCalibration Status: ");

	if(u32PGSR0_Reg & (0x1 << 1))
		sysprintf("\n  PLL Lock Done ");
	else
		sysprintf("\n  PLL Lock Fail ");

	if(u32PGSR0_Reg & (0x1 << 2))
		sysprintf("\n  Digital Delay Line Calibration Done ");
	else
		sysprintf("\n  Digital Delay Line Calibration Fail ");

	if(u32PGSR0_Reg & (0x1 << 3))
		sysprintf("\n  Impedance Calibration Done ");
	else
		sysprintf("\n  Impedance Calibration Fail ");

	if(u32PGSR0_Reg & (0x1 << 4))
		sysprintf("\n  DRAM Initialization Done ");
	else
		sysprintf("\n  DRAM Initialization Fail ");

	if(u32PGSR0_Reg & (0x1 << 5))
		sysprintf("\n  Write Leveling Done ");
	else
		sysprintf("\n  Write Leveling Fail ");

	if(u32PGSR0_Reg & (0x1 << 6))
		sysprintf("\n  Read DQS Gate Training Done ");
	else
		sysprintf("\n  Read DQS Gate Training Fail ");

	if(u32PGSR0_Reg & (0x1 << 7))
		sysprintf("\n  Write Leveling Adjustment Done ");
	else
		sysprintf("\n  Write Leveling Adjustment Fail ");

	if(u32PGSR0_Reg & (0x1 << 8))
		sysprintf("\n  Read Data Bit Deskew Done ");
	else
		sysprintf("\n  Read Data Bit Deskew Fail ");

	if(u32PGSR0_Reg & (0x1 << 9))
		sysprintf("\n  Write Data Bit Deskew Done ");
	else
		sysprintf("\n  Write Data Bit Deskew Fail ");

	if(u32PGSR0_Reg & (0x1 << 10))
		sysprintf("\n  Read Data Eye Training Done ");
	else
		sysprintf("\n  Read Data Eye Training Fail ");

	if(u32PGSR0_Reg & (0x1 << 11))
		sysprintf("\n  Write Data Eye Training Done ");
	else
		sysprintf("\n  Write Data Eye Training Fail ");


	sysprintf("\n\nCalibration Error Status: ");

	if(u32PGSR0_Reg & (0xff << 20))
	{
		if(u32PGSR0_Reg & (0x1 << 20))
			sysprintf("\n  Impedance Calibration Error ");

		if(u32PGSR0_Reg & (0x1 << 21))
			sysprintf("\n  Write Leveling Error ");

		if(u32PGSR0_Reg & (0x1 << 22))
			sysprintf("\n  Read DQS Gate Training Error ");

		if(u32PGSR0_Reg & (0x1 << 23))
			sysprintf("\n  Write Data Leveling Adjustment Error ");

		if(u32PGSR0_Reg & (0x1 << 24))
			sysprintf("\n  Read Data Bit Deskew Error ");

		if(u32PGSR0_Reg & (0x1 << 25))
			sysprintf("\n  Write Data Bit Deskew Error ");

		if(u32PGSR0_Reg & (0x1 << 26))
			sysprintf("\n  Read Data Eye Training Error ");

		if(u32PGSR0_Reg & (0x1 << 27))
			sysprintf("\n  Write Eye Training Error ");
	}
	else
	{
		sysprintf("\n  No Error ");
	}

}

void Show_MCTL_Register(void)
{
	static uint32_t u32Tmp_Reg = 0x0;
	static uint32_t u32Tmp_Value = 0x0;

	sysprintf("\n\nDDR MCTL Register: ");

	// INIT3
	u32Tmp_Reg = inp32((void *)UMCTL2_BASE + 0xDC);
	u32Tmp_Value = (2 * ((u32Tmp_Reg & (0x7   << 20))  >> 20) ) + ((u32Tmp_Reg & (0x1   << 18))  >> 18);
	sysprintf("\n  CL Setting ({INIT3[22:20], INIT3[18]}) : 0x%x ", u32Tmp_Value);
	sysprintf("\n  AL Setting (INTI3[4:3])   : 0x%x ", ((u32Tmp_Reg & (0x3   <<  3))  >>  3) );

	// INIT4
	u32Tmp_Reg = inp32((void *)UMCTL2_BASE + 0xE0);
	sysprintf("\n  CWL Setting (INIT4[21:19]) : 0x%x ", ((u32Tmp_Reg & (0x7   << 19))  >> 19) );

	// DFITMG0
	u32Tmp_Reg = inp32((void *)UMCTL2_BASE + 0x190);
	sysprintf("\n  dfi_t_rddata_en (DFITMG0[22:16]) : 0x%x ", ((u32Tmp_Reg & (0x7F << 16)) >> 16) );
	sysprintf("\n  dfi_tphy_wrlat  (DFITMG0[5:0])   : 0x%x ", ((u32Tmp_Reg & (0x3F <<  0)) >>  0) );

}

void Show_PHY_Register(void)
{
	static uint32_t u32Tmp_Reg = 0x0;
	static uint32_t u32Tmp_Value = 0x0;

	sysprintf("\n\nDDR PHY Register: ");

	// MR0
	u32Tmp_Reg = inp32((void *)DDRPHY_BASE + 0x54);
	u32Tmp_Value = (2 * ((u32Tmp_Reg & (0x7   << 20))  >> 20) ) + ((u32Tmp_Reg & (0x1   << 18))  >> 18);
	sysprintf("\n  CL Setting ({INIT3[22:20], INIT3[18]}) : 0x%x ", u32Tmp_Value);

	// MR1
	u32Tmp_Reg = inp32((void *)DDRPHY_BASE + 0x58);
	sysprintf("\n  AL Setting (MR1[4:3]) : 0x%x ", ((u32Tmp_Reg & (0x3   << 3))  >> 3) );

	// MR2
	u32Tmp_Reg = inp32((void *)DDRPHY_BASE + 0x5C);
	sysprintf("\n  CWL Setting (MR2[5:3]) : 0x%x ", ((u32Tmp_Reg & (0x7   << 3))  >> 3) );
	sysprintf("\n");

	// PTR0
	u32Tmp_Reg = inp32((void *)DDRPHY_BASE + 0x1C);
	sysprintf("\n  tPHYRST : 0x%x ", ((u32Tmp_Reg & (0x3F   << 0))  >> 0) );
	sysprintf("\n  tPLLGS  : 0x%x ", ((u32Tmp_Reg & (0x7FFF << 6))  >> 6) );
	sysprintf("\n  tPLLPD  : 0x%x ", ((u32Tmp_Reg & (0x7FF  << 21)) >> 21) );
	// PTR1
	u32Tmp_Reg = inp32((void *)DDRPHY_BASE + 0x20);
	sysprintf("\n  tPLLRST : 0x%x ", ((u32Tmp_Reg & (0x1FFF << 0))  >> 0) );
	sysprintf("\n  tPLLLOCK: 0x%x ", ((u32Tmp_Reg & (0xFFFF << 16)) >> 16) );
	// PTR2
	u32Tmp_Reg = inp32((void *)DDRPHY_BASE + 0x24);
	sysprintf("\n  tCALON  : 0x%x ", ((u32Tmp_Reg & (0x1F   << 0))  >> 0) );
	sysprintf("\n  tCALS   : 0x%x ", ((u32Tmp_Reg & (0x1F   << 5))  >> 5) );
	sysprintf("\n  tCALH   : 0x%x ", ((u32Tmp_Reg & (0x1F   << 10)) >> 10) );
	sysprintf("\n  tWLDLYS : 0x%x ", ((u32Tmp_Reg & (0x1F   << 15)) >> 15) );
	// PTR3
	u32Tmp_Reg = inp32((void *)DDRPHY_BASE + 0x28);
	sysprintf("\n  tDINIT0 : 0x%x ", ((u32Tmp_Reg & (0xFFFFF<< 0))  >> 0) );
	sysprintf("\n  tDINIT1 : 0x%x ", ((u32Tmp_Reg & (0x1FF  << 20)) >> 20) );
	// PTR4
	u32Tmp_Reg = inp32((void *)DDRPHY_BASE + 0x2C);
	sysprintf("\n  tDINIT2 : 0x%x ", ((u32Tmp_Reg & (0x3FFFF<< 0))  >> 0) );
	sysprintf("\n  tDINIT3 : 0x%x ", ((u32Tmp_Reg & (0x3FF  << 18)) >> 18) );
	// DTPR0
	u32Tmp_Reg = inp32((void *)DDRPHY_BASE + 0x48);
	sysprintf("\n  tRTP    : 0x%x ", ((u32Tmp_Reg & (0xF    << 0))  >> 0) );
	sysprintf("\n  tWTR    : 0x%x ", ((u32Tmp_Reg & (0xF    << 4))  >> 4) );
	sysprintf("\n  tRP     : 0x%x ", ((u32Tmp_Reg & (0xF    << 8))  >> 8) );
	sysprintf("\n  tRCD    : 0x%x ", ((u32Tmp_Reg & (0xF    << 12)) >> 12) );
	sysprintf("\n  tRAS    : 0x%x ", ((u32Tmp_Reg & (0x3F   << 16)) >> 16) );
	sysprintf("\n  tRRD    : 0x%x ", ((u32Tmp_Reg & (0xF    << 22)) >> 22) );
	sysprintf("\n  tRC     : 0x%x ", ((u32Tmp_Reg & (0x3F   << 26)) >> 26) );
	// DTPR1
	u32Tmp_Reg = inp32((void *)DDRPHY_BASE + 0x4C);
	sysprintf("\n  tMRD    : 0x%x ", ((u32Tmp_Reg & (0x3    << 0))  >> 0) );
	sysprintf("\n  tMOD    : 0x%x ", ((u32Tmp_Reg & (0x7    << 2))  >> 2) );
	sysprintf("\n  tFAW    : 0x%x ", ((u32Tmp_Reg & (0x3F   << 5))  >> 5) );
	sysprintf("\n  tRFC    : 0x%x ", ((u32Tmp_Reg & (0x1FF  << 11)) >> 11) );
	sysprintf("\n  tWLMRD  : 0x%x ", ((u32Tmp_Reg & (0x3F   << 20)) >> 20) );
	sysprintf("\n  tWLO    : 0x%x ", ((u32Tmp_Reg & (0xF    << 26)) >> 26) );
	sysprintf("\n  tAONOFFD: 0x%x ", ((u32Tmp_Reg & (0x3    << 30)) >> 30) );
	// DTPR2
	u32Tmp_Reg = inp32((void *)DDRPHY_BASE + 0x50);
	sysprintf("\n  tXS     : 0x%x ", ((u32Tmp_Reg & (0x3FF  << 0))  >> 0) );
	sysprintf("\n  tXP     : 0x%x ", ((u32Tmp_Reg & (0x1F   << 10)) >> 10) );
	sysprintf("\n  tCKE    : 0x%x ", ((u32Tmp_Reg & (0xF    << 15)) >> 15) );
	sysprintf("\n  tDLLK   : 0x%x ", ((u32Tmp_Reg & (0x3FF  << 19)) >> 19) );
	sysprintf("\n  tRTODT  : 0x%x ", ((u32Tmp_Reg & (0x1    << 29)) >> 29) );
	sysprintf("\n  tRTW    : 0x%x ", ((u32Tmp_Reg & (0x1    << 30)) >> 30) );
	sysprintf("\n  tCCD    : 0x%x ", ((u32Tmp_Reg & (0x1    << 31)) >> 31) );
	// PGCR2
	u32Tmp_Reg = inp32((void *)DDRPHY_BASE + 0x8C);
	sysprintf("\n  tREFPRD : 0x%x ", ((u32Tmp_Reg & (0x3FFFF<< 0))  >> 0) );
	// RDIMMGCR1
	u32Tmp_Reg = inp32((void *)DDRPHY_BASE + 0xB4);
	sysprintf("\n  tBCSTAB : 0x%x ", ((u32Tmp_Reg & (0xFFF  << 0))  >> 0) );
	sysprintf("\n  tBCMRD  : 0x%x ", ((u32Tmp_Reg & (0x7    << 12)) >> 12) );

	sysprintf("\n\n");
}

#define DRAW_RDQS   0
#define DRAW_WDQS   1

#define MINUS_SIGN  1
#define PLUS_SIGN   2
#define TARGET_SIGN 3

#define DXnGSR0_REG_OFFSET   0x1C4
#define DTEDR0_REG_OFFSET    0x84
#define DTEDR1_REG_OFFSET    0x88
#define DXnLCDLR1_REG_OFFSET 0x1E4
#define DXnGSR0_Lane_Num 2
uint8_t u8Data_Eye[10][256];
uint32_t u32Max_CLK_Period = 0x0;

void Draw_Data_Eye(uint32_t u32RW)
{
	uint32_t i, j;

	if(!u32RW)
		sysprintf("\n\n Read Data Eye: \n");
	else
		sysprintf("\n\n Write Data Eye: \n");

	sysprintf("\n");
	sysprintf("     ");

	for(j = 0; j < u32Max_CLK_Period; j++)
	{
		sysprintf("%d", (j/10));
	}

	sysprintf("\n");
	sysprintf("     ");

	for(j = 0; j < u32Max_CLK_Period; j++)
	{
		sysprintf("%d", (j%10));
	}

	for(i = 0; i < DXnGSR0_Lane_Num; i++)
	{
		sysprintf("\n");
		sysprintf(" 0%d  ", i);

		for(j = 0; j < u32Max_CLK_Period; j++)
		{
			switch(u8Data_Eye[i][j])
			{
				case 1: sysprintf("-"); break;
				case 2: sysprintf("+"); break;
				case 3: sysprintf("X"); break;

				default:
					sysprintf(" "); break;
			}
		}
	}

	sysprintf("\n");
}

void Show_Read_Data_Eye(void)
{
	uint32_t i, j;
	uint32_t u32Reg_DxnGSR0 = 0x0;
	uint32_t u32Reg_DTEDR1  = 0x0;
	uint32_t u32DTRBMX = 0x0, u32DTRLMX = 0x0, u32DTRLMN = 0x0, u32DTRBMN = 0x0;
	uint32_t u32Center_Data_Eye = 0x0, u32Left_Data_Eye = 0x0, u32Right_Data_Eye = 0x0;
	uint32_t u32RDQSD = 0;
	uint32_t u32TestFail = 0;

	// Check PGSR0
	if(!(inp32((void *)DDRPHY_BASE + 0x10) & (0x1 << 10)))
	{
		u32TestFail = 1;
		sysprintf("\n Read Data Eye Training Fail !!\n");
	}

	for(i = 0; i < DXnGSR0_Lane_Num; i++)
	{
		for(j = 0; j < 256; j++)
		{
			sysprintf("\r");
			u8Data_Eye[i][j] = 0; // clear struct
		}
	}

	// Step 3
	u32Reg_DTEDR1 = inp32((void *)DDRPHY_BASE + DTEDR1_REG_OFFSET );
	u32DTRBMX = ((u32Reg_DTEDR1 & (0xff << 24)) >> 24);
	u32DTRBMN = ((u32Reg_DTEDR1 & (0xff << 16)) >> 16);
	u32DTRLMX = ((u32Reg_DTEDR1 & (0xff <<  8)) >>  8);
	u32DTRLMN = (u32Reg_DTEDR1 & 0x000000ff);

	u32Center_Data_Eye = ((u32DTRBMX + u32DTRLMX + u32DTRLMN - u32DTRBMN) / 2);

	// Step 2
	for(i = 0; i < DXnGSR0_Lane_Num; i++)
	{
		u32Reg_DxnGSR0 = inp32((void *)DDRPHY_BASE + DXnGSR0_REG_OFFSET + (0x40 * i));
		u32RDQSD       = inp32((void *)DDRPHY_BASE + DXnLCDLR1_REG_OFFSET + (0x40 * i));
		u32RDQSD       = ((u32RDQSD & (0xff << 8)) >> 8);

		u32Reg_DxnGSR0 = ((u32Reg_DxnGSR0 & 0xff0000) >> 16);

		if(u32Reg_DxnGSR0 > u32Max_CLK_Period) u32Max_CLK_Period = u32Reg_DxnGSR0;

		u32Left_Data_Eye  = u32Center_Data_Eye - ((u32Reg_DxnGSR0+2)/4);
		u32Right_Data_Eye = u32Center_Data_Eye + ((u32Reg_DxnGSR0+2)/4);

		for(j = 0; j < u32Reg_DxnGSR0; j++)
		{
			sysprintf("\r");
			u8Data_Eye[i][j] = MINUS_SIGN; // "-"
		}

		if(u32TestFail == 1) continue;

		for(j = u32Left_Data_Eye; j <= u32Right_Data_Eye; j++)
		{
			sysprintf("\r");
			u8Data_Eye[i][j] = PLUS_SIGN; // "+"
		}

		// Step 4
		sysprintf("\r");
		u8Data_Eye[i][u32RDQSD] = TARGET_SIGN; // "x"
	}

	Draw_Data_Eye(DRAW_RDQS);
}

void Show_Write_Data_Eye(void)
{
	uint32_t i, j;
	uint32_t u32Reg_DxnGSR0 = 0x0;
	uint32_t u32Reg_DTEDR0  = 0x0;
	uint32_t u32DTWBMX = 0x0, u32DTWLMX = 0x0, u32DTWLMN = 0x0, u32DTWBMN = 0x0;
	uint32_t u32Center_Data_Eye = 0x0, u32Left_Data_Eye = 0x0, u32Right_Data_Eye = 0x0;
	uint32_t u32WDQD = 0;
	uint32_t u32TestFail = 0;

	// Check PGSR0
	if(!(inp32((void *)DDRPHY_BASE + 0x10) & (0x1 << 11)))
	{
		u32TestFail = 1;
		sysprintf("\n Write Data Eye Training Fail !!\n");
	}

	for(i = 0; i < DXnGSR0_Lane_Num; i++)
	{
		for(j = 0; j < 256; j++)
		{
			sysprintf("\r");
			u8Data_Eye[i][j] = 0; // clear struct
		}
	}

	// Step 3
	u32Reg_DTEDR0 = inp32((void *)DDRPHY_BASE + DTEDR0_REG_OFFSET );
	u32DTWBMX = ((u32Reg_DTEDR0 & (0xff << 24)) >> 24);
	u32DTWBMN = ((u32Reg_DTEDR0 & (0xff << 16)) >> 16);
	u32DTWLMX = ((u32Reg_DTEDR0 & (0xff <<  8)) >>  8);
	u32DTWLMN = (u32Reg_DTEDR0 & 0x000000ff);

	u32Center_Data_Eye = ((u32DTWBMX + u32DTWLMX + u32DTWLMN - u32DTWBMN) / 2);

	// Step 2
	for(i = 0; i < DXnGSR0_Lane_Num; i++)
	{
		u32Reg_DxnGSR0 = inp32((void *)DDRPHY_BASE + DXnGSR0_REG_OFFSET + (0x40 * i));
		u32WDQD        = inp32((void *)DDRPHY_BASE + DXnLCDLR1_REG_OFFSET + (0x40 * i));
		u32WDQD        = (u32WDQD & 0xff);

		u32Reg_DxnGSR0 = ((u32Reg_DxnGSR0 & 0xff0000) >> 16);

		if(u32Reg_DxnGSR0 > u32Max_CLK_Period) u32Max_CLK_Period = u32Reg_DxnGSR0;

		u32Left_Data_Eye  = u32Center_Data_Eye - ((u32Reg_DxnGSR0+2)/4);
		u32Right_Data_Eye = u32Center_Data_Eye + ((u32Reg_DxnGSR0+2)/4);

		for(j = 0; j < u32Reg_DxnGSR0; j++)
		{
			sysprintf("\r");
			u8Data_Eye[i][j] = MINUS_SIGN; // "-"
		}

		if(u32TestFail == 1) continue;

		for(j = u32Left_Data_Eye; j <= u32Right_Data_Eye; j++)
		{
			sysprintf("\r");
			u8Data_Eye[i][j] = PLUS_SIGN; // "+"
		}

		// Step 4
		sysprintf("\r");
		u8Data_Eye[i][u32WDQD] = TARGET_SIGN; // "X"
	}

	Draw_Data_Eye(DRAW_WDQS);
}

void ddr_init(void)
{
	uint32_t i, j;
	uint64_t ddr_reg_address;
	uint32_t value;
	uint32_t u32TimeOut1 = 0, u32TimeOut2 = 0, u32TimeOut3 = 0;

	/* Unlock protected registers */
	SYS_UnlockReg();

	/* DDR control register clock gating disable */
	outp32((void *)SYS_BASE+0x70, inp32((void *)SYS_BASE+0x70) | 0x800000);
	/* de-assert present of MCTL2 */
	outp32((void *)SYS_BASE+0x20, inp32((void *)SYS_BASE+0x20) & 0xafffffff);
	while((inp32((void *)SYS_BASE+0x20) & 0x50000000) != 0x00000000);
	outp32((void *)SYS_BASE+0x20, inp32((void *)SYS_BASE+0x20) | 0x20000000); /* set MCTLCRST to 1 */

	for(i = 0; i < (uint32_t)nvt_ddr_init_setting_size; i++)
	{
		ddr_reg_address = (uint32_t)nvt_ddr_init_setting[i].base + (uint32_t)nvt_ddr_init_setting[i].offset;
		value =  *((volatile uint32_t *)(((uintptr_t)&custom_ddr) + nvt_ddr_init_setting[i].init_flow_offset));

		*(volatile uint32_t *)((void *)ddr_reg_address) = value;

		if (i == 84) // 0xf08 //DDRCTRL
		{
			//de-assert reset signals of DDR memory controller
			outp32((void *)SYS_BASE+0x20,(inp32((void *)SYS_BASE+0x20) & 0x8fffffff));
			while( (inp32((void *)SYS_BASE+0x20) & 0x20000000) != 0x00000000);
		}

		if (i == 100) // 0x184 //DDRPHY
		{
			//polling PGSR0 (addr=4) to 0x0000000f
			while((inp32((void *)DDRPHY_BASE + 0x010) & 0x0000000f) != 0x0000000f)
			{
				u32TimeOut1++;

				if(u32TimeOut1 > TIME_OUT_VALUE)
				{
					sysprintf("\n TimeOut_1 !!");
					break;
				}
			}
		}

		//polling PGSR0 (addr=4) to (0xb0000fff or 0xb0000f5f)
		if (i == 104) // 0x04  // DDRPHY
		{
			// DCR // Check DDR2 or DDR3
			if ((inp32((void *)DDRPHY_BASE + 0x044) & (0x7)) == 0x2) // DDR2
			{
				while((inp32((void *)DDRPHY_BASE + 0x010) & 0xffffffff) != 0xb0000f5f)
				{
					u32TimeOut2++;
					if(u32TimeOut2 > TIME_OUT_VALUE)
					{
						sysprintf("\n TimeOut_2 !!");
						break;
					}
				}
			}
			else if ((inp32((void *)DDRPHY_BASE + 0x044) & (0x7)) == 0x3) // DDR3
			{
				// PIR[9] // Check Enable/Disable Write leveling routine
				if(inp32((void *)DDRPHY_BASE + 0x04) & (0x1 << 9)) // Enable Write leveling
				{
					while((inp32((void *)DDRPHY_BASE + 0x010) & 0xffffffff) != 0xb0000fff)
					{
						u32TimeOut2++;
						if(u32TimeOut2 > TIME_OUT_VALUE)
						{
							sysprintf("\n TimeOut_2 !!");
							break;
						}
					}
				}
				else // Disable Write leveling
				{
					while((inp32((void *)DDRPHY_BASE + 0x010) & 0xffffffff) != 0xb0000f5f)
					{
						u32TimeOut2++;
						if(u32TimeOut2 > TIME_OUT_VALUE)
						{
							sysprintf("\n TimeOut_2 !!");
							break;
						}
					}
				}
			}
			else
			{
				sysprintf("\n Unsupported DDR type !! (Only supports DDR2 or DDR3) \n");
			}

			//polling MCTL2 STAT to 0x00000001
			while((inp32((void *)UMCTL2_BASE + 0x004) & 0x00000003) != 0x00000001)
			{
				u32TimeOut3++;
				if(u32TimeOut3 > TIME_OUT_VALUE)
				{
					sysprintf("\n TimeOut_2 !!");
					break;
				}
			}
		}

		// if show printf message, ddr initial will fail
		//sysprintf("\n addr = 0x%x, value = 0x%x %d 0x%x \n", ddr_reg_address, value, i, nvt_ddr_init_setting[i].offset);
	}

	while((inp32((void *)UMCTL2_BASE + 0x324) & 0x00000001) != 0x00000001);

	outp32((void *)UMCTL2_BASE+0x490, 0x1);
	outp32((void *)UMCTL2_BASE+0x800, 0x1);
	outp32((void *)UMCTL2_BASE+0x8b0, 0x1);
	outp32((void *)SYS_BASE+0x70, inp32((void *)SYS_BASE+0x70) & ~0x800000); /* DDR control register clock gating enable */

	sysprintf("\n TimeOut = 0x%x, 0x%x, 0x%x \n", u32TimeOut1, u32TimeOut2, u32TimeOut3);

	Show_Status();
	Show_MCTL_Register();
	Show_PHY_Register();

	Show_Read_Data_Eye();
	Show_Write_Data_Eye();
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

int main()
{
    uint32_t reg_por;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Initialize UART to 115200-8n1 for print message */
    UART0_Init();

    sysprintf("MA35D1 DDR Test...\n");

    ddr_init();
    ddr_test();

    while(1);
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/

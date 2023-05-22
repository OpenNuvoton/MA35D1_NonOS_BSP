/**************************************************************************//**
 * @file     main.c
 * @brief    Use packet format (all the luma and chroma data interleaved) to
 *           store captured image from NT99141 sensor to SRAM.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include "NuMicro.h"
#include "sensor.h"
#include <stdio.h>

/*------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                     */
/*------------------------------------------------------------------------------------------*/
#define CCAP		CCAP1
#define CCAP_IRQn	CCAP1_IRQn

/*------------------------------------------------------------------------------------------*/
/* To run CCAPInterruptHandler, when CAP frame end interrupt                                */
/*------------------------------------------------------------------------------------------*/
volatile uint32_t u32FramePass = 0;
static uint32_t u32EscapeFrame = 0;

void CCAPInterruptHandler(void)
{
    u32FramePass++;
}

/*------------------------------------------------------------------------------------------*/
/*  CCAPIRQHandler                                                                          */
/*------------------------------------------------------------------------------------------*/
void CCAP_IRQHandler(void)
{
    uint32_t u32CapInt;
    u32CapInt = CCAP->INT;
    if( (u32CapInt & (CCAP_INT_VIEN_Msk | CCAP_INT_VINTF_Msk )) == (CCAP_INT_VIEN_Msk | CCAP_INT_VINTF_Msk))
    {
        CCAPInterruptHandler();
        CCAP->INT |= CCAP_INT_VINTF_Msk;        /* Clear Frame end interrupt */
        u32EscapeFrame = u32EscapeFrame+1;
    }

    if((u32CapInt & (CCAP_INT_ADDRMIEN_Msk|CCAP_INT_ADDRMINTF_Msk)) == (CCAP_INT_ADDRMIEN_Msk|CCAP_INT_ADDRMINTF_Msk))
    {
        CCAP->INT |= CCAP_INT_ADDRMINTF_Msk; /* Clear Address match interrupt */
    }

    if ((u32CapInt & (CCAP_INT_MEIEN_Msk|CCAP_INT_MEINTF_Msk)) == (CCAP_INT_MEIEN_Msk|CCAP_INT_MEINTF_Msk))
    {
        CCAP->INT |= CCAP_INT_MEINTF_Msk;    /* Clear Memory error interrupt */
    }
    CCAP->CTL = CCAP->CTL | CCAP_CTL_UPDATE;
}

void CCAP_SetFreq(uint32_t u32SensorFreq)
{
    int32_t i32Div;

    /* Unlock protected registers */
    SYS_UnlockReg();

    if(CCAP==CCAP0)
    {
		/* Enable CAP Clock */
		CLK_EnableModuleClock(CCAP0_MODULE);

		/* Reset IP */
		//SYS_ResetModule(CCAP0_RST);

		/* Select CCAP0 clock source from HXT */
		CLK_SetModuleClock(CCAP0_MODULE, CLK_CLKSEL0_CCAP0SEL_HXT, CLK_CLKDIV1_CCAP0(1));

		i32Div = 24000000/u32SensorFreq;
		CLK_SetModuleClock(CCAP0_MODULE, CLK_CLKSEL0_CCAP0SEL_HXT, CLK_CLKDIV1_CCAP0(i32Div));

		if(i32Div < 1) i32Div = 1;
		CLK_SetModuleClock(CCAP0_MODULE, CLK_CLKSEL0_CCAP0SEL_HXT, CLK_CLKDIV1_CCAP0(i32Div));
    } else {
		/* Enable CAP Clock */
		CLK_EnableModuleClock(CCAP1_MODULE);

		/* Reset IP */
		//SYS_ResetModule(CCAP0_RST);

		/* Select CCAP0 clock source from HXT */
		CLK_SetModuleClock(CCAP1_MODULE, CLK_CLKSEL0_CCAP1SEL_HXT, CLK_CLKDIV1_CCAP1(1));

		i32Div = 24000000/u32SensorFreq;
		CLK_SetModuleClock(CCAP1_MODULE, CLK_CLKSEL0_CCAP1SEL_HXT, CLK_CLKDIV1_CCAP1(i32Div));

		if(i32Div < 1) i32Div = 1;
		CLK_SetModuleClock(CCAP1_MODULE, CLK_CLKSEL0_CCAP1SEL_HXT, CLK_CLKDIV1_CCAP1(i32Div));
    }

    /* lock protected registers */
    SYS_LockReg();
}

void SYS_Init(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable UART module clock */
    CLK_EnableModuleClock(GPA_MODULE);
    CLK_EnableModuleClock(GPB_MODULE);
    CLK_EnableModuleClock(GPC_MODULE);
    CLK_EnableModuleClock(GPD_MODULE);
    CLK_EnableModuleClock(GPE_MODULE);
    CLK_EnableModuleClock(GPF_MODULE);
    CLK_EnableModuleClock(GPG_MODULE);
    CLK_EnableModuleClock(GPH_MODULE);
    CLK_EnableModuleClock(GPI_MODULE);
    CLK_EnableModuleClock(GPJ_MODULE);
    CLK_EnableModuleClock(GPK_MODULE);
    CLK_EnableModuleClock(GPL_MODULE);
    CLK_EnableModuleClock(GPM_MODULE);
    CLK_EnableModuleClock(GPN_MODULE);
    CLK_EnableModuleClock(CCAP1_MODULE);
    CLK_EnableModuleClock(CCAP0_MODULE);
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select CCAP module clock source as HXT and CCAP module clock divider as 2 */
    CLK_SetModuleClock(CCAP0_MODULE, CLK_CLKSEL0_CCAP0SEL_HXT, CLK_CLKDIV1_CCAP0(2) );
	CLK_SetModuleClock(CCAP1_MODULE, CLK_CLKSEL0_CCAP1SEL_HXT, CLK_CLKDIV1_CCAP1(2) );

    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Set GPB multi-function pins for UART0 RXD and TXD */
	SYS->GPE_MFPL &= ~(SYS_GPE_MFPH_PE15MFP_Msk | SYS_GPE_MFPH_PE14MFP_Msk);
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE15MFP_UART0_RXD | SYS_GPE_MFPH_PE14MFP_UART0_TXD);

	if(CCAP==CCAP0) {
		/* Set multi-function pins for CCAP0 */
		SYS->GPK_MFPH = (SYS->GPK_MFPH & ~SYS_GPK_MFPH_PK9MFP_Msk)  | SYS_GPK_MFPH_PK9MFP_VCAP0_SCLK;
		SYS->GPK_MFPH = (SYS->GPK_MFPH & ~SYS_GPK_MFPH_PK10MFP_Msk) | SYS_GPK_MFPH_PK10MFP_VCAP0_PIXCLK;
		SYS->GPK_MFPH = (SYS->GPK_MFPH & ~SYS_GPK_MFPH_PK11MFP_Msk) | SYS_GPK_MFPH_PK11MFP_VCAP0_HSYNC;
		SYS->GPM_MFPL = (SYS->GPM_MFPL & ~SYS_GPM_MFPL_PM0MFP_Msk)  | SYS_GPM_MFPL_PM0MFP_VCAP0_VSYNC;
		SYS->GPM_MFPL = (SYS->GPM_MFPL & ~SYS_GPM_MFPL_PM2MFP_Msk)  | SYS_GPM_MFPL_PM2MFP_VCAP0_DATA0;
		SYS->GPM_MFPL = (SYS->GPM_MFPL & ~SYS_GPM_MFPL_PM3MFP_Msk)  | SYS_GPM_MFPL_PM3MFP_VCAP0_DATA1;
		SYS->GPM_MFPL = (SYS->GPM_MFPL & ~SYS_GPM_MFPL_PM4MFP_Msk)  | SYS_GPM_MFPL_PM4MFP_VCAP0_DATA2;
		SYS->GPM_MFPL = (SYS->GPM_MFPL & ~SYS_GPM_MFPL_PM5MFP_Msk)  | SYS_GPM_MFPL_PM5MFP_VCAP0_DATA3;
		SYS->GPM_MFPL = (SYS->GPM_MFPL & ~SYS_GPM_MFPL_PM6MFP_Msk)  | SYS_GPM_MFPL_PM6MFP_VCAP0_DATA4;
		SYS->GPM_MFPL = (SYS->GPM_MFPL & ~SYS_GPM_MFPL_PM7MFP_Msk)  | SYS_GPM_MFPL_PM7MFP_VCAP0_DATA5;
		SYS->GPM_MFPH = (SYS->GPM_MFPH & ~SYS_GPM_MFPH_PM8MFP_Msk)  | SYS_GPM_MFPH_PM8MFP_VCAP0_DATA6;
		SYS->GPM_MFPH = (SYS->GPM_MFPH & ~SYS_GPM_MFPH_PM9MFP_Msk)  | SYS_GPM_MFPH_PM9MFP_VCAP0_DATA7;
		SYS->GPM_MFPH = (SYS->GPM_MFPH & ~SYS_GPM_MFPH_PM10MFP_Msk) | SYS_GPM_MFPH_PM10MFP_VCAP0_DATA8;
		SYS->GPM_MFPH = (SYS->GPM_MFPH & ~SYS_GPM_MFPH_PM11MFP_Msk) | SYS_GPM_MFPH_PM11MFP_VCAP0_DATA9;
	} else {
		/* Set multi-function pins for CCAP1 */
		SYS->GPN_MFPL = (SYS->GPN_MFPL & ~SYS_GPN_MFPL_PN0MFP_Msk)  | SYS_GPN_MFPL_PN0MFP_VCAP1_DATA0;
		SYS->GPN_MFPL = (SYS->GPN_MFPL & ~SYS_GPN_MFPL_PN1MFP_Msk)  | SYS_GPN_MFPL_PN1MFP_VCAP1_DATA1;
		SYS->GPN_MFPL = (SYS->GPN_MFPL & ~SYS_GPN_MFPL_PN2MFP_Msk)  | SYS_GPN_MFPL_PN2MFP_VCAP1_DATA2;
		SYS->GPN_MFPL = (SYS->GPN_MFPL & ~SYS_GPN_MFPL_PN3MFP_Msk)  | SYS_GPN_MFPL_PN3MFP_VCAP1_DATA3;
		SYS->GPN_MFPL = (SYS->GPN_MFPL & ~SYS_GPN_MFPL_PN4MFP_Msk)  | SYS_GPN_MFPL_PN4MFP_VCAP1_DATA4;
		SYS->GPN_MFPL = (SYS->GPN_MFPL & ~SYS_GPN_MFPL_PN5MFP_Msk)  | SYS_GPN_MFPL_PN5MFP_VCAP1_DATA5;
		SYS->GPN_MFPL = (SYS->GPN_MFPL & ~SYS_GPN_MFPL_PN6MFP_Msk)  | SYS_GPN_MFPL_PN6MFP_VCAP1_DATA6;
		SYS->GPN_MFPL = (SYS->GPN_MFPL & ~SYS_GPN_MFPL_PN7MFP_Msk)  | SYS_GPN_MFPL_PN7MFP_VCAP1_DATA7;
		SYS->GPN_MFPH = (SYS->GPN_MFPH & ~SYS_GPN_MFPH_PN8MFP_Msk)  | SYS_GPN_MFPH_PN8MFP_VCAP1_DATA8;
		SYS->GPN_MFPH = (SYS->GPN_MFPH & ~SYS_GPN_MFPH_PN9MFP_Msk)  | SYS_GPN_MFPH_PN9MFP_VCAP1_DATA9;
		SYS->GPN_MFPH = (SYS->GPN_MFPH & ~SYS_GPN_MFPH_PN10MFP_Msk) | SYS_GPN_MFPH_PN10MFP_VCAP1_SCLK;
		SYS->GPN_MFPH = (SYS->GPN_MFPH & ~SYS_GPN_MFPH_PN11MFP_Msk) | SYS_GPN_MFPH_PN11MFP_VCAP1_PIXCLK;
		SYS->GPN_MFPH = (SYS->GPN_MFPH & ~SYS_GPN_MFPH_PN12MFP_Msk) | SYS_GPN_MFPH_PN12MFP_VCAP1_HSYNC;
		SYS->GPN_MFPH = (SYS->GPN_MFPH & ~SYS_GPN_MFPH_PN13MFP_Msk) | SYS_GPN_MFPH_PN13MFP_VCAP1_VSYNC;
	}

    /* lock protected registers */
    SYS_LockReg();

}

void UART0_Init(void)
{
    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0,115200);
}

#define SENSOR_IN_WIDTH            1280
#define SENSOR_IN_HEIGHT            720
#define SYSTEM_WIDTH                160
#define SYSTEM_HEIGHT               120
uint8_t u8FrameBuffer[SYSTEM_WIDTH*SYSTEM_HEIGHT*2];

void PacketFormatDownScale(void)
{
    uint32_t u32Frame;

    /* Initialize HM1055 sensor and set HM1055 output YUV422 format  */
    if (InitHM1055_VGA_YUV422(CCAP) == 0)
    	return;

    /* Enable External CCAP Interrupt */
    IRQ_SetHandler((IRQn_ID_t)CCAP_IRQn, CCAP_IRQHandler);
	IRQ_Enable ((IRQn_ID_t)CCAP_IRQn);

    /* Enable External CAP Interrupt */
    CCAP_EnableInt(CCAP, CCAP_INT_VIEN_Msk);

    /* Set Vsync polarity, Hsync polarity, pixel clock polarity, Sensor Format and Order */
    CCAP_Open(CCAP, HM1055SensorPolarity | HM1055DataFormatAndOrder, CCAP_CTL_PKTEN );

    /* Set Cropping Window Vertical/Horizontal Starting Address and Cropping Window Size */
    CCAP_SetCroppingWindow(CCAP, 0,0,SENSOR_IN_HEIGHT,SENSOR_IN_WIDTH);

    /* Set System Memory Packet Base Address Register */
    CCAP_SetPacketBuf(CCAP, ptr_to_u32(u8FrameBuffer));

    /* Set Packet Scaling Vertical/Horizontal Factor Register */
    CCAP_SetPacketScaling(CCAP, SYSTEM_HEIGHT,SENSOR_IN_HEIGHT,SYSTEM_WIDTH,SENSOR_IN_WIDTH);

    /* Set Packet Frame Output Pixel Stride Width */
    CCAP_SetPacketStride(CCAP, SYSTEM_WIDTH);

    /* Start Image Capture Interface */
    CCAP_Start(CCAP);

    u32Frame = u32FramePass;
    while(1)
    {
        if(u32Frame != u32FramePass)
        {
            u32Frame = u32FramePass;
            sysprintf("Get frame %3d\n",u32Frame);
        }
    }

}


/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main (void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for printf */
    UART0_Init();

    /* Init Engine clock and Sensor clock */
    CCAP_SetFreq(12000000);

    /* Using Packet format to Image down scale */
    PacketFormatDownScale();

    while(1);
}






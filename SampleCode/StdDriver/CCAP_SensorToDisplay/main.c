/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Show how to display a captured sensor image on an LCD frame buffer.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "displib.h"
#include "sensor.h"

#define DDR_ADR_FRAMEBUFFER   0x88000000UL



/*------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                     */
/*------------------------------------------------------------------------------------------*/
#define CCAP		CCAP0
#define CCAP_IRQn	CCAP0_IRQn

#define SENSOR_IN_WIDTH            1280
#define SENSOR_IN_HEIGHT            720
#define SYSTEM_WIDTH               1024
#define SYSTEM_HEIGHT               600
//uint8_t u8FrameBuffer[SYSTEM_WIDTH*SYSTEM_HEIGHT*2];

//#define DISPLAY_ARGB8
#define DISPLAY_RGB565

/*------------------------------------------------------------------------------------------*/
/* To run CCAPInterruptHandler, when CAP frame end interrupt                                */
/*------------------------------------------------------------------------------------------*/
volatile uint32_t u32FramePass = 0;
static uint32_t u32EscapeFrame = 0;

/* LCD attributes 1024x600 */
DISP_LCD_INFO LcdPanelInfo =
{
    /* Panel Resolution */
    1024,
    600,
    /* DISP_LCD_TIMING */
    {
        51000000,
        1024,
        1,
        160,
        160,
        600,
        1,
        23,
        12,
        ePolarity_Positive,
        ePolarity_Positive
    },
    /* DISP_PANEL_CONF */
    {
        eDPIFmt_D24,
        ePolarity_Positive,
        ePolarity_Positive,
        ePolarity_Positive
    },
};

void SYS_Init(void)
{
    uint32_t pixelclk;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable module clock */
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

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

}

void DISP_Open(void)
{
    /* Set EPLL/2 as DISP Core Clock source */
    DISP_EnableDCUClk();

    /* Waiting EPLL ready */
    CLK_WaitClockReady(CLK_STATUS_STABLE_EPLL);

    /* Enable DISP Core Clock */
    CLK_EnableModuleClock(DCU_MODULE);

    /* Select DISP pixel clock source to VPLL */
    DISP_GeneratePixelClk(LcdPanelInfo.sLcdTiming.u32PCF);

    /* Select DISP pixel clock source */
    CLK_SetModuleClock(DCUP_MODULE, CLK_CLKSEL0_DCUSEL_EPLL_DIV2, 0);

    /* Set multi-function pins for LCD Display Controller */
    SYS->GPG_MFPH = SYS->GPG_MFPH & ~(SYS_GPG_MFPH_PG8MFP_Msk | SYS_GPG_MFPH_PG9MFP_Msk | SYS_GPG_MFPH_PG10MFP_Msk) |
                    (SYS_GPG_MFPH_PG8MFP_LCM_VSYNC | SYS_GPG_MFPH_PG9MFP_LCM_HSYNC | SYS_GPG_MFPH_PG10MFP_LCM_CLK);
    SYS->GPK_MFPL = SYS->GPK_MFPL & ~(SYS_GPK_MFPL_PK4MFP_Msk) | SYS_GPK_MFPL_PK4MFP_LCM_DEN;

    SYS->GPI_MFPH = SYS->GPI_MFPH & ~(SYS_GPI_MFPH_PI8MFP_Msk | SYS_GPI_MFPH_PI9MFP_Msk | SYS_GPI_MFPH_PI10MFP_Msk | SYS_GPI_MFPH_PI11MFP_Msk |
                     SYS_GPI_MFPH_PI12MFP_Msk | SYS_GPI_MFPH_PI13MFP_Msk | SYS_GPI_MFPH_PI14MFP_Msk | SYS_GPI_MFPH_PI15MFP_Msk);
    SYS->GPI_MFPH |= (SYS_GPI_MFPH_PI8MFP_LCM_DATA0 | SYS_GPI_MFPH_PI9MFP_LCM_DATA1 | SYS_GPI_MFPH_PI10MFP_LCM_DATA2 |
                      SYS_GPI_MFPH_PI11MFP_LCM_DATA3 | SYS_GPI_MFPH_PI12MFP_LCM_DATA4 | SYS_GPI_MFPH_PI13MFP_LCM_DATA5 | SYS_GPI_MFPH_PI14MFP_LCM_DATA6 | SYS_GPI_MFPH_PI15MFP_LCM_DATA7);
    SYS->GPH_MFPL = SYS->GPH_MFPL & ~(SYS_GPH_MFPL_PH0MFP_Msk | SYS_GPH_MFPL_PH1MFP_Msk | SYS_GPH_MFPL_PH2MFP_Msk | SYS_GPH_MFPL_PH3MFP_Msk |
                    SYS_GPH_MFPL_PH4MFP_Msk | SYS_GPH_MFPL_PH5MFP_Msk | SYS_GPH_MFPL_PH6MFP_Msk | SYS_GPH_MFPL_PH7MFP_Msk);
    SYS->GPH_MFPL |= (SYS_GPH_MFPL_PH0MFP_LCM_DATA8 | SYS_GPH_MFPL_PH1MFP_LCM_DATA9 | SYS_GPH_MFPL_PH2MFP_LCM_DATA10 | SYS_GPH_MFPL_PH3MFP_LCM_DATA11 |
                      SYS_GPH_MFPL_PH4MFP_LCM_DATA12 | SYS_GPH_MFPL_PH5MFP_LCM_DATA13 | SYS_GPH_MFPL_PH6MFP_LCM_DATA14 | SYS_GPH_MFPL_PH7MFP_LCM_DATA15);

    SYS->GPC_MFPH = SYS->GPC_MFPH & ~(SYS_GPC_MFPH_PC12MFP_Msk | SYS_GPC_MFPH_PC13MFP_Msk | SYS_GPC_MFPH_PC14MFP_Msk | SYS_GPC_MFPH_PC15MFP_Msk);
    SYS->GPC_MFPH |= (SYS_GPC_MFPH_PC12MFP_LCM_DATA16 | SYS_GPC_MFPH_PC13MFP_LCM_DATA17 | SYS_GPC_MFPH_PC14MFP_LCM_DATA18 | SYS_GPC_MFPH_PC15MFP_LCM_DATA19);
    SYS->GPH_MFPH = SYS->GPH_MFPH & ~(SYS_GPH_MFPH_PH12MFP_Msk | SYS_GPH_MFPH_PH13MFP_Msk | SYS_GPH_MFPH_PH14MFP_Msk | SYS_GPH_MFPH_PH15MFP_Msk);
    SYS->GPH_MFPH |= (SYS_GPH_MFPH_PH12MFP_LCM_DATA20 | SYS_GPH_MFPH_PH13MFP_LCM_DATA21 | SYS_GPH_MFPH_PH14MFP_LCM_DATA22 | SYS_GPH_MFPH_PH15MFP_LCM_DATA23);
}

void CCAPInterruptHandler(void)
{
/*    u32FramePass++; */
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
		//SYS_ResetModule(CCAP1_RST);

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

#define HM1055DataFormatAndOrder2 (CCAP_PAR_INDATORD_YUYV | CCAP_PAR_INFMT_YUV422 | CCAP_PAR_OUTFMT_RGB565)

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
    CCAP_Open(CCAP, HM1055SensorPolarity | HM1055DataFormatAndOrder2, CCAP_CTL_PKTEN );

    /* Set Cropping Window Vertical/Horizontal Starting Address and Cropping Window Size */
    CCAP_SetCroppingWindow(CCAP, 0,0,SENSOR_IN_HEIGHT,SENSOR_IN_WIDTH);

    /* Set System Memory Packet Base Address Register */
    CCAP_SetPacketBuf(CCAP, ptr_to_u32(DDR_ADR_FRAMEBUFFER));

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

int main(void)
{
    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    /* Open DISP IP Clock and set multi-function pins */
    DISP_Open();

    sysprintf("\n+------------------------------------------------------------------------+\n");
    sysprintf("|                This sample code show image under LCD Panel             |\n");
#ifdef DISPLAY_ARGB8
    sysprintf("|                Image format is ARGB8                                   |\n");
#endif
#ifdef DISPLAY_RGB565
    sysprintf("|                Image format is RGB565                                  |\n");
#endif
    sysprintf("+------------------------------------------------------------------------+\n");

    /* Configure display attributes of LCD panel */
    DISPLIB_LCDInit(LcdPanelInfo);

#ifdef DISPLAY_ARGB8
    /* Configure DISP Framebuffer settings  */
    DISPLIB_SetFBConfig(eFBFmt_A8R8G8B8, LcdPanelInfo.u32ResolutionWidth, LcdPanelInfo.u32ResolutionHeight, DDR_ADR_FRAMEBUFFER);
#endif
#ifdef DISPLAY_RGB565
    DISPLIB_SetFBConfig(eFBFmt_R5G6B5, LcdPanelInfo.u32ResolutionWidth, LcdPanelInfo.u32ResolutionHeight, DDR_ADR_FRAMEBUFFER);
#endif

    /* Start to display */
    DISPLIB_EnableOutput(eLayer_Video);

    /* Init Engine clock and Sensor clock */
    CCAP_SetFreq(24000000);

    /* Using Packet format to Image down scale */
    PacketFormatDownScale();

    while(1);
}

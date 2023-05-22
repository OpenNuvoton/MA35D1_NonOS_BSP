/**************************************************************************//**
 * @file     main.c
 * @brief    This sample program decode a short H264 bit stream clip with
 *           MA35D1 VC8000 H264 decoder. It enables VC8000 PP which directly
 *           output images to display memory.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "displib.h"
#include "vc8000_lib.h"

#define LCD_WIDTH       1024
#define LCD_HEIGHT      600

uint8_t  _DisplayBuff[LCD_WIDTH * LCD_HEIGHT * 4 * 4] __attribute__((aligned(32)));  /* 1024 x 600 RGB888 */
uint8_t  _VC8000Buff[0x2000000] __attribute__((aligned(32)));  /* 32 MB */

extern uint32_t  VectorDataBase_H264BS, VectorDataLimit_H264BS;

/* LCD attributes 1024x600 */
DISP_LCD_INFO LcdPanelInfo =
{
	/* Panel Resolution */
	LCD_WIDTH,
	LCD_HEIGHT,
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

void SYS_Init(void)
{
	/* Enable UART module clock */
	CLK_EnableModuleClock(UART0_MODULE);

	/* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
	CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

	/* enable VC8000 clock */
	CLK_EnableModuleClock(VDEC_MODULE);

	/* Set GPE multi-function pins for UART0 RXD and TXD */
	SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);
}

int32_t main(void)
{
	uint8_t   *file_ptr;
	uint32_t  bs_len, remain;
	struct pp_params pp;
	int   handle, ret;

	/* Unlock protected registers */
	SYS_UnlockReg();

	/* Init System, IP clock and multi-function I/O */
	SYS_Init();

	/* Init UART to 115200-8n1 for print message */
	UART_Open(UART0, 115200);

	/* Open DISP IP Clock and set multi-function pins */
	DISP_Open();

	/* Configure display attributes of LCD panel */
	DISPLIB_LCDInit(LcdPanelInfo);

	/* Configure DISP Framebuffer settings  */
	DISPLIB_SetFBConfig(eFBFmt_A8R8G8B8, LcdPanelInfo.u32ResolutionWidth,
						LcdPanelInfo.u32ResolutionHeight, ptr_to_u32(_DisplayBuff));

	/* Start to display */
	DISPLIB_EnableOutput(eLayer_Video);

	sysprintf("\n\n");
	sysprintf("+--------------------------------------------+\n");
	sysprintf("|  MA35D1 VC8000 decode H264 bit stream      |\n");
	sysprintf("+--------------------------------------------+\n");

	ret = VC8000_Init(ptr_to_u32(_VC8000Buff), sizeof(_VC8000Buff));
	if (ret < 0)
	{
		sysprintf("VC8000_Init failed! (%d)\n", ret);
		while (1);
	}

	handle = VC8000_H264_Open_Instance();
	if (handle < 0)
	{
		sysprintf("VC8000_H264_Open_Instance failed! (%d)\n", handle);
		while (1);
	}

	pp.frame_buf_w = LCD_WIDTH;
	pp.frame_buf_h = LCD_HEIGHT;
	pp.img_out_x = 0;
	pp.img_out_y = 0;
	pp.img_out_w = LCD_WIDTH;
	pp.img_out_h = LCD_HEIGHT;
	pp.img_out_fmt = VC8000_PP_F_RGB888;
	pp.rotation = VC8000_PP_ROTATION_NONE;
	pp.pp_out_dst = VC8000_PP_OUT_DST_DISPLAY;

	ret = VC8000_H264_Enable_PP(handle, &pp);
	if (ret < 0)
	{
		sysprintf("VC8000_H264_Enable_PP failed! (%d)\n", ret);
		goto err_out;
	}

	file_ptr = nc_ptr(&VectorDataBase_H264BS);
	bs_len = ptr_to_u32(&VectorDataLimit_H264BS) - ptr_to_u32(&VectorDataBase_H264BS);

	while (remain > 0)
	{
		ret = VC8000_H264_Decode_Run(handle, file_ptr, bs_len, NULL, &remain);
		if (ret != 0)
		{
			sysprintf("VC8000_H264_Decode_Run error: %d\n", ret);
			goto err_out;
		}
		file_ptr += (bs_len - remain);
		bs_len = remain;
	}
	VC8000_H264_Close_Instance(handle);
	sysprintf("\nH264 bit stream deocde done.\n");
	while (1);

err_out:
	sysprintf("ERROR CODE: %d\n", ret);
	VC8000_H264_Close_Instance(handle);
	while (1);
}

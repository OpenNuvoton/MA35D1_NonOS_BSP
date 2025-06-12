/**************************************************************************//**
 * @file     main.c
 * @brief    This sample program searches the root directory of the USB
 *           disk to play back all h264 bit stream files that are found.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "usbh_lib.h"
#include "ff.h"
#include "diskio.h"
#include "displib.h"
#include "vc8000_lib.h"

#define LCD_WIDTH         1024
#define LCD_HEIGHT        600
#define DISP_BUFF_SIZE    (LCD_WIDTH * LCD_HEIGHT * 4 * 4)  /* 1024 x 600 RGB888 */

#define jiffies           (EL0_GetCurrentPhysicalValue() / 12000)

#define FRAME_RATE_CONTROL
#define FRAME_RATE        30
#define FRAME_INTERVAL    (1000 / FRAME_RATE)

#define STREAM_BUFF_SIZE  0x100000
#define MIN_REMAIN        (256 * 1024)

uint8_t  _DisplayBuff[DISP_BUFF_SIZE] __attribute__((aligned(32)));
uint8_t  _VC8000Buff[0x2000000] __attribute__((aligned(32)));  /* 32 MB */
uint8_t  _StreamBuff[STREAM_BUFF_SIZE] __attribute__((aligned(32)));

static int _h264_handle;
static struct pp_params _pp;

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

static volatile uint64_t  _start_time = 0;
static volatile uint32_t  _mark_time;

void start_timer(void)
{
	_start_time = EL0_GetCurrentPhysicalValue();
}

uint32_t get_ticks(void)
{
	uint64_t   t_off;
	t_off = EL0_GetCurrentPhysicalValue() - _start_time;
	t_off = t_off / 12000;
	return (uint32_t)t_off;
}

/* This function is necessary for USB Host library. */
void delay_us(int usec)
{
	uint64_t   t0;
	t0  = EL0_GetCurrentPhysicalValue();
	while ((EL0_GetCurrentPhysicalValue() - t0) < (usec * 12));
}

void delay_ms(int msec)
{
	uint64_t   t0;
	t0  = EL0_GetCurrentPhysicalValue();
	while ((EL0_GetCurrentPhysicalValue() - t0) < (msec * 12000));
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

int USB_PHY_Init(void)
{
	uint64_t t0;

	/* reset USB PHY */
	SYS->USBPMISCR = (SYS->USBPMISCR & ~0x70007) | SYS_USBPMISCR_PHY0POR_Msk |
					  SYS_USBPMISCR_PHY0SUSPEND_Msk |
					  SYS_USBPMISCR_PHY1POR_Msk | SYS_USBPMISCR_PHY1SUSPEND_Msk;

	delay_us(20000);  /* delay 20 ms */

	/* enter operation mode */
	SYS->USBPMISCR = (SYS->USBPMISCR & ~0x70007) |
					 SYS_USBPMISCR_PHY0SUSPEND_Msk | SYS_USBPMISCR_PHY1SUSPEND_Msk;
	t0 = get_ticks();
	while (1) {
		delay_us(10);
		if ((SYS->USBPMISCR & SYS_USBPMISCR_PHY0HSTCKSTB_Msk) &&
			(SYS->USBPMISCR & SYS_USBPMISCR_PHY1HSTCKSTB_Msk))
			break;   /* both USB PHY0 and PHY1 clock 60MHz UTMI clock stable */

		if (get_ticks() - t0 > 500) {
			sysprintf("USB PHY reset failed. USBPMISCR = 0x%x\n", SYS->USBPMISCR);
			return -1;
		}
	}
	sysprintf("USBPMISCR = 0x%x\n", SYS->USBPMISCR);
	return 0;
}

void SYS_Init(void)
{
	/* Enable UART module clock */
	CLK_EnableModuleClock(UART0_MODULE);

	/* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
	CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

	/* Enable HSUSBH0/1 module clock */
	CLK_EnableModuleClock(HUSBH0_MODULE);
	CLK_EnableModuleClock(HUSBH1_MODULE);
	CLK_EnableModuleClock(USBD_MODULE);

	/* enable VC8000 clock */
	CLK_EnableModuleClock(VDEC_MODULE);

	/* Set GPE multi-function pins for UART0 RXD and TXD */
	SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

	/* set UHOVRCURH(SYS_MISCFCR0[12]) 1 => USBH Host over-current detect is high-active */
	/*                                 0 => USBH Host over-current detect is low-active  */
	// SYS->MISCFCR0 |= SYS_MISCFCR0_UHOVRCURH_Msk;  /* OVCR is active-high */
	SYS->MISCFCR0 &= ~SYS_MISCFCR0_UHOVRCURH_Msk;    /* OVCR is active-low  */

	/* set PL.12 as MFP9 for HSUSBH_PWREN */
	SYS->GPL_MFPH &= ~SYS_GPL_MFPH_PL12MFP_Msk;
	SYS->GPL_MFPH |= SYS_GPL_MFPH_PL12MFP_HSUSBH_PWREN;

	/* set PL.13 as MFP9 for HSUSBH_OVC */
	SYS->GPL_MFPH &= ~SYS_GPL_MFPH_PL13MFP_Msk;
	SYS->GPL_MFPH |= SYS_GPL_MFPH_PL13MFP_HSUSBH_OVC;
}

DWORD get_fattime (void)
{
	unsigned long tmr;

	tmr=0x00000;

	return tmr;
}

static int is_h264_file(char *fname)
{
	int  slen = strlen(fname);

	if ((strcasecmp(fname + slen - 4, ".264") == 0) ||
		(strcasecmp(fname + slen - 5, ".h264") == 0))
		return 1;
	return 0;
}

int do_h264_decode(char *fname, uint32_t fsize)
{
	FIL       hFile, *pFile = NULL;
	int       remain, next_frame_offs, ret;
	uint8_t   *stream_buff = nc_ptr(_StreamBuff);
	int       frame_size, r;
	int       decode_cnt;
	uint32_t  play_len;
	uint64_t  start_time, next_frame_time;
	int       last_decode_cnt = 0;
	uint64_t  fps_check_jiffy = 0;

	pFile = nc_ptr(&hFile);   /* make FIL->buff be non-cache */

	ret = f_open(pFile, fname, FA_OPEN_EXISTING | FA_READ);
	if (ret != 0)
	{
		sysprintf("Failed to open H264 file <%s>! (%d)\n", fname, ret);
		return -1;
	}

	_h264_handle = VC8000_H264_Open_Instance();
	if (_h264_handle < 0)
	{
		sysprintf("VC8000_H264_Open_Instance failed! (%d)\n", _h264_handle);
		goto err_out;
	}

	ret = VC8000_H264_Enable_PP(_h264_handle, &_pp);
	if (ret < 0)
	{
		sysprintf("VC8000_H264_Enable_PP failed! (%d)\n", ret);
		goto err_out;
	}

	ret = f_read(pFile, stream_buff, STREAM_BUFF_SIZE, &remain);
	if (ret != 0)
	{
		sysprintf("Read file failed! (%d)\n", ret);
		goto err_out;
	}

	decode_cnt = 0;
	play_len = 0;
	next_frame_offs = 0;
	start_time = jiffies;
	while (1)
	{
#ifdef FRAME_RATE_CONTROL
		/* frame rate control */
		while (jiffies - start_time < FRAME_INTERVAL * decode_cnt);
#endif

		r = 0;
		ret = VC8000_H264_Decode_Run(_h264_handle, stream_buff + next_frame_offs, remain, NULL, &r);
		if (ret != 0)
			break;

		decode_cnt++;
		next_frame_offs += (remain - r);
		play_len += (remain - r);
		remain = r;

		if (jiffies - fps_check_jiffy >= 1000)
		{
			sysprintf("[%d%c] FPS: %d\n", ((play_len/1024) * 100) / (fsize/1024), '%', decode_cnt - last_decode_cnt);
			last_decode_cnt = decode_cnt;
			fps_check_jiffy = jiffies;
		}

		// sysprintf("Decode done: %d / %d\n", remain, data_len);

		if (sysIsKbHit())
		{
			sysgetchar();
			break;
		}

		if (remain == 0 && f_eof(pFile))
			break;

		if ((remain < MIN_REMAIN) && !f_eof(pFile))
		{
			if (remain > 0)
				memcpy(stream_buff, stream_buff + next_frame_offs, remain);

			ret = f_read(pFile, stream_buff + remain, STREAM_BUFF_SIZE - remain, &r);
			if (ret != 0)
			{
				sysprintf("Read file error! (%d)\n", ret);
				goto err_out;
			}
			remain += r;
			next_frame_offs = 0;
		}
	}
	VC8000_H264_Close_Instance(_h264_handle);
	f_close(pFile);
	return 0;

err_out:
	if (pFile)
		f_close(pFile);
	if (_h264_handle != -1)
		VC8000_JPEG_Close_Instance(_h264_handle);
	return -1;
}

int h264_decode_files(char *path)
{
	FILINFO   Finfo;
	DIR       dir;
	FRESULT   res;
	int       h264_found = 0;
	char      fname[256];

	sysprintf("\n\nH264 playback on directory %s ==> \n", path);

	res = f_opendir(&dir, path);
	if (res)
	{
		sysprintf("f_opendir failed!\n");
		return -1;
	}

	while (1)
	{
		res = f_readdir(&dir, &Finfo);
		if ((res != FR_OK) || !Finfo.fname[0])
			break;

		sysprintf("%c%c%c%c%c %d/%02d/%02d %02d:%02d    %9d  %s",
			   (Finfo.fattrib & AM_DIR) ? 'D' : '-',
			   (Finfo.fattrib & AM_RDO) ? 'R' : '-',
			   (Finfo.fattrib & AM_HID) ? 'H' : '-',
			   (Finfo.fattrib & AM_SYS) ? 'S' : '-',
			   (Finfo.fattrib & AM_ARC) ? 'A' : '-',
			   (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
			   (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63, Finfo.fsize, Finfo.fname);
		sysprintf("\n");

		if (is_h264_file(Finfo.fname) && (Finfo.fattrib & AM_ARC))
		{
			h264_found = 1;
			strcpy(fname, path);
			strcat(fname, "/");
			strcat(fname, Finfo.fname);
			do_h264_decode(fname, Finfo.fsize);
			delay_us(200000);
		}
	}
	f_closedir(&dir);
	if (!h264_found)
		return -1;
	return 0;
}

int32_t main(void)
{
	TCHAR     usb_path[] = { '0', ':', 0 };
	DIR       dir;
	int       i, ret;

	/* Unlock protected registers */
	SYS_UnlockReg();

	/* Init System, IP clock and multi-function I/O */
	SYS_Init();

	/* Init UART to 115200-8n1 for print message */
	UART_Open(UART0, 115200);

	start_timer();

	if (USB_PHY_Init() != 0) {
		sysprintf("USB PHY Init failed!\n");
		while (1);
	}

	usbh_core_init();
	usbh_umas_init();
	usbh_pooling_hubs();
	f_chdrive(usb_path);               /* set default path */

	/* Open DISP IP Clock and set multi-function pins */
	DISP_Open();

	/* Configure display attributes of LCD panel */
	DISPLIB_LCDInit(LcdPanelInfo);

	/* Configure DISP Framebuffer settings  */
	DISPLIB_SetFBConfig(eFBFmt_A8R8G8B8, LcdPanelInfo.u32ResolutionWidth,
						LcdPanelInfo.u32ResolutionHeight, ptr_to_u32(_DisplayBuff));

	/* Start to display */
	DISPLIB_EnableOutput(eLayer_Video);

	sysprintf("Detecting USB disk...\n");
	while (1)
	{
		if (f_opendir(&dir, usb_path) == 0)
		{
			f_closedir(&dir);
			break;
		}
		delay_ms(200);
		usbh_pooling_hubs();
	}

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

	_h264_handle = -1;

	_pp.frame_buf_w = LCD_WIDTH;
	_pp.frame_buf_h = LCD_HEIGHT;
	_pp.img_out_x = 0;
	_pp.img_out_y = 0;
	_pp.img_out_w = LCD_WIDTH;
	_pp.img_out_h = LCD_HEIGHT;
	_pp.img_out_fmt = VC8000_PP_F_RGB888;
	_pp.rotation = VC8000_PP_ROTATION_NONE;
	_pp.pp_out_dst = VC8000_PP_OUT_DST_USER;
	_pp.pp_out_paddr =  ptr_to_u32(_DisplayBuff);
    _pp.contrast = 8;
    _pp.brightness = 0;
    _pp.saturation = 32;
    _pp.alpha = 255;
    _pp.transparency = 0;

	DISPLIB_SetFBAddr(ptr_to_u32(_DisplayBuff));

	IRQ_SetTarget(VDE_IRQn, IRQ_CPU_0);

	while (1)
		h264_decode_files(usb_path);
}

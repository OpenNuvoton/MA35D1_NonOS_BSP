/**************************************************************************//**
 * @file     main.c
 * @brief    The sample receives audio data from UAC device, and immediately send
 *           back to that UAC device.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "usbh_lib.h"
#include "usbh_uac.h"

static uint16_t  vol_max, vol_min, vol_res, vol_cur;

extern volatile int8_t   g_bMicIsMono;
extern volatile uint32_t g_UacRecCnt;          /* Counter of UAC record data             */
extern volatile uint32_t g_UacPlayCnt;         /* Counter UAC playback data              */

extern void ResetAudioLoopBack(void);
extern int audio_in_callback(UAC_DEV_T *dev, uint8_t *data, int len);
extern int audio_out_callback(UAC_DEV_T *dev, uint8_t *data, int len);

static volatile uint64_t  _start_time = 0;

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

/*
 *  This function is necessary for USB Host library.
 */
void delay_us(int usec)
{
	uint64_t   t0;
	t0  = EL0_GetCurrentPhysicalValue();
	while ((EL0_GetCurrentPhysicalValue() - t0) < (usec * 12));
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

void UART0_Init()
{
	/* Configure UART0 and set UART0 baud rate */
	UART_Open(UART0, 115200);
}

void  dump_buff_hex(uint8_t *pucBuff, int nBytes)
{
    int     nIdx, i;

    nIdx = 0;
    while (nBytes > 0)
    {
        sysprintf("0x%04X  ", nIdx);
        for (i = 0; (i < 16) && (nBytes > 0); i++)
        {
            sysprintf("%02x ", pucBuff[nIdx + i]);
            nBytes--;
        }
        nIdx += 16;
        sysprintf("\n");
    }
    sysprintf("\n");
}

void  uac_control_example(UAC_DEV_T *uac_dev)
{
    uint16_t   val16;
    uint32_t   srate[4];
    uint8_t    val8;
    uint8_t    data[8];
    int        i, ret;
    uint32_t   val32;

    vol_max = vol_min = vol_res = 0;

    sysprintf("\nGet channel information ===>\n");

    /*-------------------------------------------------------------*/
    /*  Get channel number information                             */
    /*-------------------------------------------------------------*/
    ret = usbh_uac_get_channel_number(uac_dev, UAC_SPEAKER);
    if(ret < 0)
        sysprintf("    Failed to get speaker's channel number.\n");
    else
        sysprintf("    Speaker: %d\n", ret);

    ret = usbh_uac_get_channel_number(uac_dev, UAC_MICROPHONE);
    if (ret < 0)
        sysprintf("    Failed to get microphone's channel number.\n");
    else
    {
        sysprintf("    Microphone: %d\n", ret);
        if (ret == 1)
            g_bMicIsMono = 1;
        else
            g_bMicIsMono = 0;
    }

    sysprintf("\nGet subframe bit resolution ===>\n");

    /*-------------------------------------------------------------*/
    /*  Get audio subframe bit resolution information              */
    /*-------------------------------------------------------------*/
    ret = usbh_uac_get_bit_resolution(uac_dev, UAC_SPEAKER, &val8);
    if(ret < 0)
        sysprintf("    Failed to get speaker's bit resoltion.\n");
    else
    {
        sysprintf("    Speaker audio subframe size: %d bytes\n", val8);
        sysprintf("    Speaker subframe bit resolution: %d\n", ret);
    }

    ret = usbh_uac_get_bit_resolution(uac_dev, UAC_MICROPHONE, &val8);
    if(ret < 0)
        sysprintf("    Failed to get microphone's bit resoltion.\n");
    else
    {
        sysprintf("    Microphone audio subframe size: %d bytes\n", val8);
        sysprintf("    Microphone subframe bit resolution: %d\n", ret);
    }

    sysprintf("\nGet sampling rate list ===>\n");

    /*-------------------------------------------------------------*/
    /*  Get audio subframe bit resolution information              */
    /*-------------------------------------------------------------*/
    ret = usbh_uac_get_sampling_rate(uac_dev, UAC_SPEAKER, (uint32_t *)&srate[0], 4, &val8);
    if(ret < 0)
        sysprintf("    Failed to get speaker's sampling rate.\n");
    else
    {
        if(val8 == 0)
            sysprintf("    Speaker sampling rate range: %d ~ %d Hz\n", srate[0], srate[1]);
        else
        {
            for(i = 0; i < val8; i++)
                sysprintf("    Speaker sampling rate: %d\n", srate[i]);
        }
    }

    ret = usbh_uac_get_sampling_rate(uac_dev, UAC_MICROPHONE, (uint32_t *)&srate[0], 4, &val8);
    if(ret < 0)
        sysprintf("    Failed to get microphone's sampling rate.\n");
    else
    {
        if(val8 == 0)
            sysprintf("    Microphone sampling rate range: %d ~ %d Hz\n", srate[0], srate[1]);
        else
        {
            for(i = 0; i < val8; i++)
                sysprintf("    Microphone sampling rate: %d\n", srate[i]);
        }
    }

    sysprintf("\nSpeaker mute control ===>\n");

    /*-------------------------------------------------------------*/
    /*  Get current mute value of UAC device's speaker.            */
    /*-------------------------------------------------------------*/
    if (usbh_uac_mute_control(uac_dev, UAC_SPEAKER, UAC_GET_CUR, UAC_CH_MASTER, data) == UAC_RET_OK)
    {
        sysprintf("    Speaker mute state is %d.\n", data[0]);
    }
    else
        sysprintf("    Failed to get speaker mute state!\n");

    sysprintf("\nSpeaker L(F) volume control ===>\n");

#if 0
    /*--------------------------------------------------------------------------*/
    /*  Get current volume value of UAC device's speaker left channel.          */
    /*--------------------------------------------------------------------------*/
    if (usbh_uac_vol_control(uac_dev, UAC_SPEAKER, UAC_GET_CUR, UAC_CH_LEFT_FRONT, &val16) == UAC_RET_OK)
        sysprintf("    Speaker L(F) volume is 0x%x.\n", val16);
    else
        sysprintf("    Failed to get seaker L(F) volume!\n");

    /*--------------------------------------------------------------------------*/
    /*  Get minimum volume value of UAC device's speaker left channel.          */
    /*--------------------------------------------------------------------------*/
    if(usbh_uac_vol_control(uac_dev, UAC_SPEAKER, UAC_GET_MIN, UAC_CH_LEFT_FRONT, &val16) == UAC_RET_OK)
        sysprintf("    Speaker L(F) minimum volume is 0x%x.\n", val16);
    else
        sysprintf("    Failed to get speaker L(F) minimum volume!\n");

    /*--------------------------------------------------------------------------*/
    /*  Get maximum volume value of UAC device's speaker left channel.          */
    /*--------------------------------------------------------------------------*/
    if(usbh_uac_vol_control(uac_dev, UAC_SPEAKER, UAC_GET_MAX, UAC_CH_LEFT_FRONT, &val16) == UAC_RET_OK)
        sysprintf("    Speaker L(F) maximum volume is 0x%x.\n", val16);
    else
        sysprintf("    Failed to get speaker L(F) maximum volume!\n");

    /*--------------------------------------------------------------------------*/
    /*  Get volume resolution of UAC device's speaker left channel.             */
    /*--------------------------------------------------------------------------*/
    if(usbh_uac_vol_control(uac_dev, UAC_SPEAKER, UAC_GET_RES, UAC_CH_LEFT_FRONT, &val16) == UAC_RET_OK)
        sysprintf("    Speaker L(F) volume resolution is 0x%x.\n", val16);
    else
        sysprintf("    Failed to get speaker L(F) volume resolution!\n");

    sysprintf("\nSpeaker R(F) volume control ===>\n");

    /*--------------------------------------------------------------------------*/
    /*  Get current volume value of UAC device's speaker right channel.         */
    /*--------------------------------------------------------------------------*/
    if(usbh_uac_vol_control(uac_dev, UAC_SPEAKER, UAC_GET_CUR, UAC_CH_RIGHT_FRONT, &val16) == UAC_RET_OK)
        sysprintf("    Speaker R(F) volume is 0x%x.\n", val16);
    else
        sysprintf("    Failed to get speaker R(F) volume!\n");

    /*--------------------------------------------------------------------------*/
    /*  Get minimum volume value of UAC device's speaker right channel.         */
    /*--------------------------------------------------------------------------*/
    if(usbh_uac_vol_control(uac_dev, UAC_SPEAKER, UAC_GET_MIN, UAC_CH_RIGHT_FRONT, &val16) == UAC_RET_OK)
        sysprintf("    Speaker R(F) minimum volume is 0x%x.\n", val16);
    else
        sysprintf("    Failed to get speaker R(F) minimum volume!\n");

    /*--------------------------------------------------------------------------*/
    /*  Get maximum volume value of UAC device's speaker right channel.         */
    /*--------------------------------------------------------------------------*/
    if(usbh_uac_vol_control(uac_dev, UAC_SPEAKER, UAC_GET_MAX, UAC_CH_RIGHT_FRONT, &val16) == UAC_RET_OK)
        sysprintf("    Speaker R(F) maximum volume is 0x%x.\n", val16);
    else
        sysprintf("    Failed to get speaker R(F) maximum volume!\n");

    /*--------------------------------------------------------------------------*/
    /*  Get volume resolution of UAC device's speaker right channel.            */
    /*--------------------------------------------------------------------------*/
    if(usbh_uac_vol_control(uac_dev, UAC_SPEAKER, UAC_GET_RES, UAC_CH_RIGHT_FRONT, &val16) == UAC_RET_OK)
        sysprintf("    Speaker R(F) volume resolution is 0x%x.\n", val16);
    else
        sysprintf("    Failed to get speaker R(F) volume resolution!\n");
#endif

    sysprintf("\nSpeaker master volume control ===>\n");

    /*--------------------------------------------------------------------------*/
    /*  Get minimum volume value of UAC device's speaker master channel.        */
    /*--------------------------------------------------------------------------*/
    if(usbh_uac_vol_control(uac_dev, UAC_SPEAKER, UAC_GET_MIN, UAC_CH_MASTER, &val16) == UAC_RET_OK)
        sysprintf("    Speaker minimum master volume is 0x%x.\n", val16);
    else
        sysprintf("    Failed to get speaker master minimum volume!\n");

    /*--------------------------------------------------------------------------*/
    /*  Get maximum volume value of UAC device's speaker master channel.        */
    /*--------------------------------------------------------------------------*/
    if(usbh_uac_vol_control(uac_dev, UAC_SPEAKER, UAC_GET_MAX, UAC_CH_MASTER, &val16) == UAC_RET_OK)
        sysprintf("    Speaker maximum master volume is 0x%x.\n", val16);
    else
        sysprintf("    Failed to get speaker maximum master volume!\n");

    /*--------------------------------------------------------------------------*/
    /*  Get volume resolution of UAC device's speaker master channel.           */
    /*--------------------------------------------------------------------------*/
    if(usbh_uac_vol_control(uac_dev, UAC_SPEAKER, UAC_GET_RES, UAC_CH_MASTER, &val16) == UAC_RET_OK)
        sysprintf("    Speaker master volume resolution is 0x%x.\n", val16);
    else
        sysprintf("    Failed to get speaker master volume resolution!\n");

    /*--------------------------------------------------------------------------*/
    /*  Get current volume value of UAC device's speaker master channel.        */
    /*--------------------------------------------------------------------------*/
    if(usbh_uac_vol_control(uac_dev, UAC_SPEAKER, UAC_GET_CUR, UAC_CH_MASTER, &val16) == UAC_RET_OK)
        sysprintf("    Speaker master volume is 0x%x.\n", val16);
    else
        sysprintf("    Failed to get speaker master volume!\n");

#if 0
    sysprintf("\nMixer master volume control ===>\n");

    /*-------------------------------------------------------------*/
    /*  Get current mute value of UAC device's microphone.         */
    /*-------------------------------------------------------------*/
    sysprintf("\nMicrophone mute control ===>\n");
    if(usbh_uac_mute_control(uac_dev, UAC_MICROPHONE, UAC_GET_CUR, UAC_CH_MASTER, data) == UAC_RET_OK)
        sysprintf("    Microphone mute state is %d.\n", data[0]);
    else
        sysprintf("    Failed to get microphone mute state!\n");
#endif

    sysprintf("\nMicrophone volume control ===>\n");

    /*-------------------------------------------------------------*/
    /*  Get current volume value of UAC device's microphone.       */
    /*-------------------------------------------------------------*/
    if(usbh_uac_vol_control(uac_dev, UAC_MICROPHONE, UAC_GET_CUR, UAC_CH_MASTER, &vol_cur) == UAC_RET_OK)
        sysprintf("    Microphone current volume is 0x%x.\n", vol_cur);
    else
        sysprintf("    Failed to get microphone current volume!\n");

    /*-------------------------------------------------------------*/
    /*  Get minimum volume value of UAC device's microphone.       */
    /*-------------------------------------------------------------*/
    if(usbh_uac_vol_control(uac_dev, UAC_MICROPHONE, UAC_GET_MIN, UAC_CH_MASTER, &vol_min) == UAC_RET_OK)
        sysprintf("    Microphone minimum volume is 0x%x.\n", vol_min);
    else
        sysprintf("    Failed to get microphone minimum volume!\n");

    /*-------------------------------------------------------------*/
    /*  Get maximum volume value of UAC device's microphone.       */
    /*-------------------------------------------------------------*/
    if(usbh_uac_vol_control(uac_dev, UAC_MICROPHONE, UAC_GET_MAX, UAC_CH_MASTER, &vol_max) == UAC_RET_OK)
        sysprintf("    Microphone maximum volume is 0x%x.\n", vol_max);
    else
        sysprintf("    Failed to get microphone maximum volume!\n");

    /*-------------------------------------------------------------*/
    /*  Get resolution of UAC device's microphone volume value.    */
    /*-------------------------------------------------------------*/
    if(usbh_uac_vol_control(uac_dev, UAC_MICROPHONE, UAC_GET_RES, UAC_CH_MASTER, &vol_res) == UAC_RET_OK)
        sysprintf("    Microphone volume resolution is 0x%x.\n", vol_res);
    else
        sysprintf("    Failed to get microphone volume resolution!\n");

#if 0
    /*-------------------------------------------------------------*/
    /*  Get current auto-gain setting of UAC device's microphone.  */
    /*-------------------------------------------------------------*/
    sysprintf("\nMicrophone automatic gain control ===>\n");
    if(UAC_AutoGainControl(uac_dev, UAC_MICROPHONE, UAC_GET_CUR, UAC_CH_MASTER, data) == UAC_RET_OK)
        sysprintf("    Microphone auto gain is %s.\n", data[0] ? "ON" : "OFF");
    else
        sysprintf("    Failed to get microphone auto-gain state!\n");
#endif

    sysprintf("\nSampling rate control ===>\n");

    /*-------------------------------------------------------------*/
    /*  Get current sampling rate value of UAC device's speaker.   */
    /*-------------------------------------------------------------*/
    if (usbh_uac_sampling_rate_control(uac_dev, UAC_SPEAKER, UAC_GET_CUR, &val32) == UAC_RET_OK)
        sysprintf("    Speaker's current sampling rate is %d.\n", val32);
    else
        sysprintf("    Failed to get speaker's current sampling rate!\n");

    /*-------------------------------------------------------------*/
    /*  Set new sampling rate value of UAC device's speaker.       */
    /*-------------------------------------------------------------*/
    val32 = 48000;
    if (usbh_uac_sampling_rate_control(uac_dev, UAC_SPEAKER, UAC_SET_CUR, &val32) != UAC_RET_OK)
        sysprintf("    Failed to set Speaker's current sampling rate %d.\n", val32);

    if(usbh_uac_sampling_rate_control(uac_dev, UAC_SPEAKER, UAC_GET_CUR, &val32) == UAC_RET_OK)
        sysprintf("    Speaker's current sampling rate is %d.\n", val32);
    else
        sysprintf("    Failed to get speaker's current sampling rate!\n");

    /*-------------------------------------------------------------*/
    /*  Get current sampling rate value of UAC device's microphone.*/
    /*-------------------------------------------------------------*/
    if(usbh_uac_sampling_rate_control(uac_dev, UAC_MICROPHONE, UAC_GET_CUR, &val32) == UAC_RET_OK)
        sysprintf("    Microphone's current sampling rate is %d.\n", val32);
    else
        sysprintf("    Failed to get microphone's current sampling rate!\n");

    /*-------------------------------------------------------------*/
    /*  Set new sampling rate value of UAC device's microphone.    */
    /*-------------------------------------------------------------*/
    val32 = 48000;
    if (usbh_uac_sampling_rate_control(uac_dev, UAC_MICROPHONE, UAC_SET_CUR, &val32) != UAC_RET_OK)
        sysprintf("    Failed to set microphone's current sampling rate!\n");

    if (usbh_uac_sampling_rate_control(uac_dev, UAC_MICROPHONE, UAC_GET_CUR, &val32) == UAC_RET_OK)
        sysprintf("    Microphone's current sampling rate is %d.\n", val32);
    else
        sysprintf("    Failed to get microphone's current sampling rate!\n");
}

int32_t main(void)
{
    UAC_DEV_T  *uac_dev;
    int        ch;
    uint16_t   val16;

	/* Unlock protected registers */
	SYS_UnlockReg();

	/* Init System, IP clock and multi-function I/O */
	SYS_Init();

	/* Init UART0 for sysprintf */
	UART0_Init();

	start_timer();

	if (USB_PHY_Init() != 0) {
		sysprintf("USB PHY Init failed!\n");
		while (1);
	}

    sysprintf("\n\n");
    sysprintf("+-------------------------------------------+\n");
    sysprintf("|                                           |\n");
    sysprintf("|     USB Host UAC loop back demo           |\n");
    sysprintf("|                                           |\n");
    sysprintf("+-------------------------------------------+\n");

    usbh_core_init();
    usbh_uac_init();
    usbh_memory_used();

    while(1)
    {
        if (usbh_pooling_hubs())              /* USB Host port detect polling and management */
        {
            /*
             *  Has hub port event.
             */

            uac_dev = usbh_uac_get_device_list();
            if (uac_dev == NULL)
                continue;

            if (uac_dev != NULL)                  /* should be newly connected UAC device        */
            {
                usbh_uac_open(uac_dev);

                uac_control_example(uac_dev);

                ResetAudioLoopBack();

                usbh_uac_start_audio_out(uac_dev, audio_out_callback);

                usbh_uac_start_audio_in(uac_dev, audio_in_callback);
            }
        }

        if (uac_dev == NULL)
        {
            ResetAudioLoopBack();
            if (sysIsKbHit())
            {
                ch = sysgetchar();
                usbh_memory_used();
            }
            continue;
        }

        if (sysIsKbHit())
        {
            ch = sysgetchar();

            if ((ch == '+') && (vol_cur + vol_res <= vol_max))
            {
                sysprintf("+");
                val16 = vol_cur+vol_res;
                if (usbh_uac_vol_control(uac_dev, UAC_MICROPHONE, UAC_SET_CUR, UAC_CH_MASTER, &val16) == UAC_RET_OK)
                {
                    sysprintf("    Microphone set volume 0x%x success.\n", val16);
                    vol_cur = val16;
                }
                else
                    sysprintf("    Failed to set microphone volume 0x%x!\n", val16);
            }
            else if ((ch == '-') && (vol_cur - vol_res >= vol_min))
            {
                sysprintf("-");
                val16 = vol_cur-vol_res;
                if (usbh_uac_vol_control(uac_dev, UAC_MICROPHONE, UAC_SET_CUR, UAC_CH_MASTER, &val16) == UAC_RET_OK)
                {
                    sysprintf("    Microphone set volume 0x%x success.\n", val16);
                    vol_cur = val16;
                }
                else
                    sysprintf("    Failed to set microphone volume 0x%x!\n", val16);
            }
            else if ((ch == '0') && (vol_cur - vol_res >= vol_min))
            {
                if (usbh_uac_vol_control(uac_dev, UAC_MICROPHONE, UAC_GET_CUR, UAC_CH_MASTER, &vol_cur) == UAC_RET_OK)
                    sysprintf("    Microphone current volume is 0x%x.\n", vol_cur);
                else
                    sysprintf("    Failed to get microphone current volume!\n");
            }
            else
            {
                sysprintf("IN: %d, OUT: %d\n", g_UacRecCnt, g_UacPlayCnt);
                usbh_memory_used();
            }

        }  /* end of sysIsKbHit() */
    }
}

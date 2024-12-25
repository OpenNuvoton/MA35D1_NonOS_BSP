/**************************************************************************//**
 * @file     main.c
 *
 * @brief    This example demonstrates how to use the USBH library to
 *           connect to a USB Video Class Device, capture the MJPEG bit
 *           stream, and use the VC8000 hardware JPEG decoder to decode
 *           the JPEG image and output it to display.
 *
 * @copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "displib.h"
#include "vc8000_lib.h"
#include "usbh_lib.h"
#include "usbh_uvc.h"

/*----------------------------------------------------------------------
 * Display
 */
#define LCD_WIDTH           1024
#define LCD_HEIGHT          600

#define SCALING_STEP_MAX    10

uint8_t  _DisplayBuff[LCD_WIDTH * LCD_HEIGHT * 4 * 4] __attribute__((aligned(32)));  /* 1024 x 600 RGB888 */

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

/*----------------------------------------------------------------------
 * VC8000 JPEG decoder
 */

uint8_t  _VC8000Buff[0x800000] __attribute__((aligned(32)));  /* 8 MB */


/*----------------------------------------------------------------------
 * USB UVC
 */
#define SELECT_RES_WIDTH     1280  // 1920
#define SELECT_RES_HEIGHT    720   // 1080

#define IMAGE_MAX_SIZE       (SELECT_RES_WIDTH * SELECT_RES_HEIGHT * 2)
#define IMAGE_BUFF_CNT       4

enum
{
    IMAGE_BUFF_FREE,
    IMAGE_BUFF_USB,
    IMAGE_BUFF_READY,
    IMAGE_BUFF_POST
};

struct img_buff_t
{
    uint8_t   *buff;
    int       len;
    int       state;
};

struct img_buff_t  _imgs[IMAGE_BUFF_CNT];
uint8_t  image_buff_pool[IMAGE_BUFF_CNT][IMAGE_MAX_SIZE] __attribute__((aligned(32)));
int   _idx_usb = 0, _idx_post = 0;
int   _total_frame_count = 0;


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

    /* enable VC8000 clock */
    CLK_EnableModuleClock(VDEC_MODULE);

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

void UART0_Init()
{
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

void  init_image_buffers(void)
{
    int   i;
    for (i = 0; i < IMAGE_BUFF_CNT; i++)
    {
        _imgs[i].buff = nc_ptr(&image_buff_pool[i]);
        _imgs[i].len = 0;
        _imgs[i].state = IMAGE_BUFF_FREE;
    }
    _idx_usb = 0;
    _idx_post = 0;
}

int  uvc_rx_callbak(UVC_DEV_T *vdev, uint8_t *data, int len)
{
    int  next_idx;

    // sysprintf("RX: %d\n", len);
    _total_frame_count++;

    next_idx = (_idx_usb + 1) % IMAGE_BUFF_CNT;

    if (_imgs[next_idx].state != IMAGE_BUFF_FREE)
    {
        /*
         *  Next image buffer is in used.
         *  Just drop this newly received image and reuse the same image buffer.
         */
        // sysprintf("Drop!\n");
        usbh_uvc_set_video_buffer(vdev, _imgs[_idx_usb].buff, IMAGE_MAX_SIZE);
    }
    else
    {
        _imgs[_idx_usb].state = IMAGE_BUFF_READY;   /* mark the current buffer as ready for decode/display */
        _imgs[_idx_usb].len   = len;                /* length of this newly received image   */

        /* proceed to the next image buffer */
        _idx_usb = next_idx;
        _imgs[_idx_usb].state = IMAGE_BUFF_USB;     /* mark the next image as used by USB    */

        /* assign the next image buffer to receive next image from USB */
        usbh_uvc_set_video_buffer(vdev, _imgs[_idx_usb].buff, IMAGE_MAX_SIZE);
    }
    return 0;
}

void show_menu()
{
    sysprintf("\n\n+---------------------------------------------+\n");
    sysprintf("|  Operation menu                             |\n");
    sysprintf("+---------------------------------------------+\n");
    sysprintf("|  [1] Stop video streaming                   |\n");
    sysprintf("|  [2] Start video streaming                  |\n");
    sysprintf("|  [r] Rotate                                 |\n");
    sysprintf("|  [s] Scaling                                |\n");
    sysprintf("+---------------------------------------------+\n\n");
    usbh_memory_used();
    // sysprintf("[0x%x] [0x%x] is_streaming = %d,\n",  HSUSBH->UPSCR[0], HSUSBH->UPSCR[1], cur_vdev->is_streaming);
}

UVC_DEV_T * uvc_conn_check(UVC_DEV_T *cur_vdev)
{
    UVC_DEV_T *vdev = cur_vdev;
    IMAGE_FORMAT_E  format;
    int width, height;
    int i, ret;

    if (usbh_pooling_hubs())       /* USB Host port detect polling and management */
    {
        /*
         *  Has hub port event.
         */
        vdev = usbh_uvc_get_device_list();
        if (vdev == NULL)
        {
            cur_vdev = NULL;
            sysprintf("\n[No UVC device connected]\n\n");
            return NULL;
        }

        if (cur_vdev == vdev)
        {
            sysprintf("\n\n\nWaiting for UVC device connected...\n");
            return vdev;
        }

        if (vdev->next != NULL)
        {
            sysprintf("\n\nWarning!! Multiple UVC device is not supported!!\n\n");
            sysgetchar();
            return cur_vdev;
        }

        /*----------------------------------------------------------------------------*/
        /*  New UVC device connected.                                                 */
        /*----------------------------------------------------------------------------*/
        cur_vdev = vdev;
        sysprintf("\n\n----------------------------------------------------------\n");
        sysprintf("[Video format list]\n");
        for (i = 0; ; i++)
        {
            ret = usbh_get_video_format(cur_vdev, i, &format, &width, &height);
            if (ret != 0)
                break;

            sysprintf("[%d] %s, %d x %d\n", i, (format == UVC_FORMAT_MJPEG ? "MJPEG" : "YUYV"), width, height);
        }
        sysprintf("\n\n");

        ret = usbh_set_video_format(cur_vdev, UVC_FORMAT_MJPEG, SELECT_RES_WIDTH, SELECT_RES_HEIGHT);
        if (ret != 0)
            sysprintf("usbh_set_video_format failed! - 0x%x\n", ret);

        init_image_buffers();

        /* assign the first image buffer to receive the image from USB */
        usbh_uvc_set_video_buffer(vdev, _imgs[_idx_usb].buff, IMAGE_MAX_SIZE);
        _imgs[_idx_usb].state = IMAGE_BUFF_USB;

        ret = usbh_uvc_start_streaming(cur_vdev, uvc_rx_callbak);
        if (ret != 0)
        {
            sysprintf("usbh_uvc_start_streaming failed! - %d\n", ret);
            sysprintf("Please re-connect UVC device...\n");
        }
        else
            show_menu();
    }
    return cur_vdev;
}

int main(void)
{
    UVC_DEV_T       *vdev = NULL;
    IMAGE_FORMAT_E  format;
    struct pp_params pp;
    int             i, width, height;
    int             command;
    uint32_t        t_last = 0, cnt_last = 0;
    uint64_t        t0;
    int             handle, ret;
    int             scaling_step = 0;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for sysprintf */
    UART0_Init();

    start_timer();

    sysprintf("\n\n");
    sysprintf("+-------------------------------------------+\n");
    sysprintf("|                                           |\n");
    sysprintf("|  USB Host UVC class sample demo           |\n");
    sysprintf("|                                           |\n");
    sysprintf("+-------------------------------------------+\n");

    /*---------------------------------------------------------------------*/
    /*  Display init                                                       */
    /*---------------------------------------------------------------------*/
    /* Open DISP IP Clock and set multi-function pins */
    DISP_Open();

    /* Configure display attributes of LCD panel */
    DISPLIB_LCDInit(LcdPanelInfo);

    /* Configure DISP Framebuffer settings  */
    DISPLIB_SetFBConfig(eFBFmt_A8R8G8B8, LcdPanelInfo.u32ResolutionWidth,
                        LcdPanelInfo.u32ResolutionHeight, ptr_to_u32(_DisplayBuff));

    /* Start to display */
    DISPLIB_EnableOutput(eLayer_Video);

    /*---------------------------------------------------------------------*/
    /*  VC8000 JPEG Decoder init                                           */
    /*---------------------------------------------------------------------*/
    ret = VC8000_Init(ptr_to_u32(_VC8000Buff), sizeof(_VC8000Buff));
    if (ret < 0)
    {
        sysprintf("VC8000_Init failed! (%d)\n", ret);
        while (1);
    }

    IRQ_SetTarget(VDE_IRQn, IRQ_CPU_0);

    pp.frame_buf_w = LCD_WIDTH;
    pp.frame_buf_h = LCD_HEIGHT;
    pp.img_out_x = 0;
    pp.img_out_y = 0;
    pp.img_out_w = LCD_WIDTH;
    pp.img_out_h = LCD_HEIGHT;
    pp.img_out_fmt = VC8000_PP_F_RGB888;
    pp.rotation = VC8000_PP_ROTATION_NONE;
    pp.pp_out_dst = VC8000_PP_OUT_DST_DISPLAY;

    /*---------------------------------------------------------------------*/
    /*  USBH UVC init                                                      */
    /*---------------------------------------------------------------------*/
    if (USB_PHY_Init() != 0) {
        sysprintf("USB PHY Init failed!\n");
        while (1);
    }

    usbh_core_init();
    usbh_uvc_init();
    usbh_memory_used();

    /*---------------------------------------------------------------------*/
    /*  Main Loop                                                          */
    /*---------------------------------------------------------------------*/

    t_last = get_ticks();
    cnt_last = 0;
    _total_frame_count = 0;

    while(1)
    {
        vdev = uvc_conn_check(vdev);
        if (vdev == NULL)
        {
            t_last = get_ticks();
            cnt_last = 0;
            _total_frame_count = 0;
            continue;
        }

        if (_imgs[_idx_post].state == IMAGE_BUFF_READY)
        {
            _imgs[_idx_post].state = IMAGE_BUFF_POST;

            handle = VC8000_JPEG_Open_Instance();
            if (handle < 0)
            {
                /* unlikely! */
                sysprintf("VC8000_JPEG_Open_Instance failed! (%d)\n", handle);
            }

            ret = VC8000_JPEG_Enable_PP(handle, &pp);
            if (ret < 0)
            {
                /* unlikely! */
                sysprintf("VC8000_JPEG_Enable_PP failed! (%d)\n", ret);
            }

            ret = VC8000_JPEG_Decode_Run(handle, _imgs[_idx_post].buff, _imgs[_idx_post].len, NULL);
            if (ret != 0)
                sysprintf("VC8000_JPEG_Decode_Run error: %d\n", ret);

            VC8000_JPEG_Close_Instance(handle);

            _imgs[_idx_post].state = IMAGE_BUFF_FREE;
            _idx_post = (_idx_post + 1) % IMAGE_BUFF_CNT;
        }

        if (get_ticks() - t_last > 1000)
        {
            cnt_last = _total_frame_count - cnt_last;

            sysprintf("Frame rate: %d, Total: %d \n", (cnt_last * 1000) / (get_ticks() - t_last), _total_frame_count);

            t_last = get_ticks();
            cnt_last = _total_frame_count;
        }

        if (!sysIsKbHit())
            continue;

        command = sysgetchar();

        sysprintf("\n\nInput command [%c]\n", command);

        switch (command)
        {
            case '1':
                if (!vdev->is_streaming)
                    break;
                ret = usbh_uvc_stop_streaming(vdev);
                if (ret != 0)
                    sysprintf("\nusbh_uvc_stop_streaming failed! - %d\n", ret);
                break;

            case '2':
                if (vdev->is_streaming)
                    break;
                ret = usbh_uvc_start_streaming(vdev, uvc_rx_callbak);
                if (ret != 0)
                    sysprintf("\nusbh_uvc_start_streaming failed! - %d\n", ret);
                break;

            case 'R':
            case 'r':
                switch (pp.rotation)
                {
                    case VC8000_PP_ROTATION_NONE:
                        pp.rotation = VC8000_PP_ROTATION_HOR_FLIP;
                        break;

                    case VC8000_PP_ROTATION_HOR_FLIP:
                        pp.rotation = VC8000_PP_ROTATION_VER_FLIP;
                        break;

                    case VC8000_PP_ROTATION_VER_FLIP:
                        pp.rotation = VC8000_PP_ROTATION_180;
                        break;

                    case VC8000_PP_ROTATION_180:
                        pp.rotation = VC8000_PP_ROTATION_NONE;
                        break;
                }
                sysprintf("Roration: %d\n", pp.rotation);
                break;

            case 'S':
            case 's':
                pp.img_out_x = (LCD_WIDTH * 5 / 100) * scaling_step / 2;
                pp.img_out_y = (LCD_HEIGHT * 5 / 100) * scaling_step / 2;
                pp.img_out_w = LCD_WIDTH - pp.img_out_x * 2;
                pp.img_out_h = LCD_HEIGHT - pp.img_out_y * 2;
                scaling_step = (scaling_step + 1) % SCALING_STEP_MAX;
                break;

            default:
                break;
        }
        show_menu();
    }
}

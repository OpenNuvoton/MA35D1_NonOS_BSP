/**************************************************************************//**
 * @file     main.c
 *
 * @brief    This example demonstrates how to use the USBH library to
 *           connect to a USB Video Class Device, capture the MJPEG bit
 *           stream, and use the VC8000 hardware JPEG decoder to decode
 *           the JPEG image and output it to display.
 *
 * @copyright (C) 2026 Nuvoton Technology Corp. All rights reserved.
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

#define CAM2_X_POS          512
#define CAM2_Y_POS          300

#define CAM_DISP_WIDTH      512
#define CAM_DISP_HEIGHT     300

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

uint8_t  image_buff_pool[2][IMAGE_BUFF_CNT][IMAGE_MAX_SIZE] __attribute__((aligned(32)));

struct ucam_t
{
    UVC_DEV_T *uvc;
    int new_conn;
    int idx_usb;
    int idx_post;
    int cnt_last;
    int t_last;
    int total_frame_count;
    struct pp_params pp;
    struct img_buff_t imgs[IMAGE_BUFF_CNT];
};

struct ucam_t _ucam[2];

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

void init_image_buffers(int id)
{
    struct ucam_t *u = &_ucam[id];
    int   i;

    for (i = 0; i < IMAGE_BUFF_CNT; i++)
    {
        u->imgs[i].buff = nc_ptr(&image_buff_pool[id][i]);
        u->imgs[i].len = 0;
        u->imgs[i].state = IMAGE_BUFF_FREE;
    }
    u->idx_usb = 0;
    u->idx_post = 0;
    u->cnt_last = 0;
    u->total_frame_count = 0;
}

int  uvc_rx_callbak(UVC_DEV_T *vdev, uint8_t *data, int len)
{
    struct ucam_t *u;
    int  id, next_idx;

    if (vdev->udev->hc_driver == &ehci0_driver)
        id = 0;
    else
        id = 1;

    u = &_ucam[id];

    // sysprintf("RX: %d\n", len);
    u->total_frame_count++;

    next_idx = (u->idx_usb + 1) % IMAGE_BUFF_CNT;

    if (u->imgs[next_idx].state != IMAGE_BUFF_FREE)
    {
        /*
         *  Next image buffer is in used.
         *  Just drop this newly received image and reuse the same image buffer.
         */
        // sysprintf("Drop!\n");
        usbh_uvc_set_video_buffer(vdev, u->imgs[u->idx_usb].buff, IMAGE_MAX_SIZE);
    }
    else
    {
        u->imgs[u->idx_usb].state = IMAGE_BUFF_READY;   /* mark the current buffer as ready for decode/display */
        u->imgs[u->idx_usb].len   = len;                /* length of this newly received image   */

        /* proceed to the next image buffer */
        u->idx_usb = next_idx;
        u->imgs[u->idx_usb].state = IMAGE_BUFF_USB;     /* mark the next image as used by USB    */

        /* assign the next image buffer to receive next image from USB */
        usbh_uvc_set_video_buffer(vdev, u->imgs[u->idx_usb].buff, IMAGE_MAX_SIZE);
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
    sysprintf("+---------------------------------------------+\n\n");
    usbh_memory_used();
    // sysprintf("[0x%x] [0x%x] is_streaming = %d,\n",  HSUSBH->UPSCR[0], HSUSBH->UPSCR[1], cur_vdev->is_streaming);
}

static void detect_uvc_conn_state_change(void)
{
    UVC_DEV_T *vlist;
    int is_connected[2] = { 0, 0 };
    int id;

    usbh_pooling_hubs();

    vlist = usbh_uvc_get_device_list();

    while (vlist != NULL)
    {
        if (vlist->udev->hc_driver == &ehci0_driver)
        {
            if (_ucam[0].uvc == NULL)
            {
                _ucam[0].uvc = vlist;
                _ucam[0].new_conn = 1;
            }
            is_connected[0] = 1;
        }
        else if (vlist->udev->hc_driver == &ehci1_driver)
        {
            if (_ucam[1].uvc == NULL)
            {
                _ucam[1].uvc = vlist;
                _ucam[1].new_conn = 1;
            }
            is_connected[1] = 1;
        }
        vlist = vlist->next;
    }

    for (id = 0; id < 2; id++)
    {
        if ((_ucam[id].uvc != NULL) && (is_connected[id] == 0))
        {
            _ucam[id].uvc = NULL;
            _ucam[id].new_conn = 0;
            _ucam[id].idx_usb = 0;
            _ucam[id].idx_post = 0;
            _ucam[id].cnt_last = 0;
            _ucam[id].total_frame_count = 0;
            init_image_buffers(id);
            sysprintf("\n[USB Camera %d disconnected]\n\n", id);
        }
    }
}

void uvc_conn_check(void)
{
    UVC_DEV_T *uvc;
    struct ucam_t *u;
    IMAGE_FORMAT_E format;
    int width, height;
    int i, id, ret;

    detect_uvc_conn_state_change();

    for (id = 0; id < 2; id++)
    {
        u = &_ucam[id];

        if (u->new_conn == 0)
            continue;

        u->new_conn = 0;
        uvc = u->uvc;

        /*----------------------------------------------------------------------------*/
        /*  New UVC device connected.                                                 */
        /*----------------------------------------------------------------------------*/

        sysprintf("\n\n----------------------------------------------------------\n");
        sysprintf("[%d] Video format list\n", id);
        for (i = 0; ; i++)
        {
            ret = usbh_get_video_format(uvc, i, &format, &width, &height);
            if (ret != 0)
                break;

            sysprintf("    (%d) %s, %d x %d\n", i, (format == UVC_FORMAT_MJPEG ? "MJPEG" : "YUYV"), width, height);
        }
        sysprintf("\n\n");

        ret = usbh_set_video_format(uvc, UVC_FORMAT_MJPEG, SELECT_RES_WIDTH, SELECT_RES_HEIGHT);
        if (ret != 0)
            sysprintf("usbh_set_video_format failed! - 0x%x\n", ret);

        ret = usbh_uvc_set_video_fps(uvc, 25);
        if (ret != 0)
            sysprintf("usbh_uvc_set_video_fps failed! - 0x%x\n", ret);

        init_image_buffers(id);

        /* assign the first image buffer to receive the image from USB */
        usbh_uvc_set_video_buffer(uvc, u->imgs[u->idx_usb].buff, IMAGE_MAX_SIZE);
        u->imgs[u->idx_usb].state = IMAGE_BUFF_USB;

        ret = usbh_uvc_start_streaming(uvc, uvc_rx_callbak);
        if (ret != 0)
        {
            sysprintf("USB Camera %d - usbh_uvc_start_streaming failed! - %d\n", id, ret);
            sysprintf("Please re-connect UVC device...\n");
        }
        else
            show_menu();
    }
}

int main(void)
{
    IMAGE_FORMAT_E  format;
    struct pp_params *pp;
    struct ucam_t *u;
    int       i, width, height;
    int       command;
    uint64_t  t0;
    int       id, handle, ret;

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

    pp = &_ucam[0].pp;
    pp->frame_buf_w = LCD_WIDTH;
    pp->frame_buf_h = LCD_HEIGHT;
    pp->img_out_x = 0;
    pp->img_out_y = 0;
    pp->img_out_w = CAM_DISP_WIDTH;
    pp->img_out_h = CAM_DISP_HEIGHT;
    pp->img_out_fmt = VC8000_PP_F_RGB888;
    pp->contrast = 8;
    pp->brightness = 30;
    pp->saturation = 32;
    pp->alpha = 255;
    pp->transparency = 0;
    pp->rotation = VC8000_PP_ROTATION_NONE;
    pp->pp_out_dst = VC8000_PP_OUT_DST_DISPLAY;

    memcpy(&_ucam[1].pp, pp, sizeof(struct pp_params));
    _ucam[1].pp.img_out_x = CAM2_X_POS;
    _ucam[1].pp.img_out_y = CAM2_Y_POS;


    /*---------------------------------------------------------------------*/
    /*  USBH UVC init                                                      */
    /*---------------------------------------------------------------------*/
    if (USB_PHY_Init() != 0) {
        sysprintf("USB PHY Init failed!\n");
        while (1);
    }

    _ucam[0].uvc = NULL;
    _ucam[1].uvc = NULL;

    usbh_core_init();
    usbh_uvc_init();
    usbh_pooling_hubs();
    usbh_pooling_hubs();
    usbh_memory_used();

    /*---------------------------------------------------------------------*/
    /*  Main Loop                                                          */
    /*---------------------------------------------------------------------*/

    _ucam[0].t_last = get_ticks();
    _ucam[1].t_last = get_ticks();

    while(1)
    {
        uvc_conn_check();

        if ((_ucam[0].uvc == NULL) && (_ucam[1].uvc == NULL))
            continue;

        for (id = 0; id < 2; id++)
        {
            u = &_ucam[id];

            if (u->imgs[u->idx_post].state == IMAGE_BUFF_READY)
            {
                u->imgs[u->idx_post].state = IMAGE_BUFF_POST;

                handle = VC8000_JPEG_Open_Instance();
                if (handle < 0)
                {
                    /* unlikely! */
                    sysprintf("VC8000_JPEG_Open_Instance failed! (%d)\n", handle);
                }

                ret = VC8000_JPEG_Enable_PP(handle, &u->pp);
                if (ret < 0)
                {
                    /* unlikely! */
                    sysprintf("VC8000_JPEG_Enable_PP failed! (%d)\n", ret);
                }

                ret = VC8000_JPEG_Decode_Run(handle, u->imgs[u->idx_post].buff, u->imgs[u->idx_post].len, NULL);
                if (ret != 0)
                    sysprintf("VC8000_JPEG_Decode_Run error: %d\n", ret);

                VC8000_JPEG_Close_Instance(handle);

                u->imgs[u->idx_post].state = IMAGE_BUFF_FREE;
                u->idx_post = (u->idx_post + 1) % IMAGE_BUFF_CNT;
            }

            if (get_ticks() - u->t_last > 1000)
            {
                u->cnt_last = u->total_frame_count - u->cnt_last;

                sysprintf("[%d] Frame rate: %d, Total: %d \n", id, (u->cnt_last * 1000) / (get_ticks() - u->t_last), u->total_frame_count);

                u->t_last = get_ticks();
                u->cnt_last = u->total_frame_count;
            }
        }
    }
}

/**************************************************************************//**
 * @file     video_task.c
 *
 * @brief    VC8000 VPU Decoding Task and Display Controller (DCUltra) Management.
 *
 * @details  This file implements the core video processing task on MA35D1 Core1:
 *           - Initializes the VC8000 VPU hardware for H.264 decoding.
 *           - Manages the interaction between the USB stream buffer and the VPU.
 *           - Configures the Post-Processor (PP) to output decoded frames
 *             directly to the Display Controller (DCU) via a specialized
 *             Direct-to-Display path for zero-copy efficiency.
 *           - Handles LCD panel timing, FB configuration, and FPS calculation.
 *
 * @note     - This task expects a continuous H.264 bitstream from the USB
 *             Video Sink buffer (vsink_rxq).
 *           - To maintain low latency, the VPU decodes and triggers the
 *             display hardware directly without intermediate software copies.
 *
 * @copyright (C) 2026 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include "NuMicro.h"
#include "displib.h"
#include "vc8000_lib.h"
#include "usbd_video_sink.h"
#include "FreeRTOS.h"
#include "task.h"

#define FPS_CAL_INTV        5

#define LCD_WIDTH           1024
#define LCD_HEIGHT          600

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

uint8_t _vc8k_mem_poll[VC8K_STRM_BUFSZ] __attribute__((aligned(32)));

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

void vVideo_Task(void *pvParameters)
{
    struct pp_params pp;
    uint8_t *sbuff;
    uint32_t sidx, avail, rlen, remain;
    uint32_t frame_count, last_t0;
    int handle, ret;

    ret = VC8000_Init(VC8K_BUFF_BASE, VC8K_BUFF_SIZE);
    if (ret < 0)
    {
        sysprintf("VC8000_Init failed! (%d)\n", ret);
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
    pp.contrast = 8;
    pp.brightness = 0;
    pp.saturation = 32;
    pp.alpha = 255;
    pp.transparency = 0;

    handle = VC8000_H264_Open_Instance();
    if (handle < 0)
    {
        sysprintf("VC8000_H264_Open_Instance failed! (%d)\n", handle);
        while (1);
    }

    ret = VC8000_H264_Enable_PP(handle, &pp);
    if (ret < 0)
    {
        sysprintf("VC8000_H264_Enable_PP failed! (%d)\n", ret);
        return;
    }

    /*
     *  Init VC8K stream buffer and prepare enough data
     */
    sbuff = nc_ptr(_vc8k_mem_poll);
    sidx = 0;
    while (vsink_rxq_read(&sbuff[0], VC8K_FRAME_MAX_SIZE) != VC8K_FRAME_MAX_SIZE)
    {
        // sysprintf("Wait USB data.\n");
        vTaskDelay(1);
    }
    avail = VC8K_FRAME_MAX_SIZE;
    frame_count = 0;
    last_t0 = g_tmr2_ticks;

    while (1)
    {
        ret = VC8000_H264_Decode_Run(handle, &sbuff[sidx], avail, NULL, &remain);
        if (ret != 0)
        {
            sysprintf("VC8000_H264_Decode_Run error: %d\n", ret);
            // break;
        }
        else
        {
            // sysprintf("[%d]\n", sidx /1024);
            frame_count++;

            if (g_tmr2_ticks - last_t0 >= FPS_CAL_INTV * TMR2_TICKS_PER_SEC)
            {
                sysprintf("fps: %d.%d\n", frame_count / FPS_CAL_INTV, ((frame_count * 10) / FPS_CAL_INTV) % 10);
                last_t0 = g_tmr2_ticks;
                frame_count = 0;
            }
        }

        sidx += (avail - remain);
        avail = remain;

        if (sidx + VC8K_FRAME_MAX_SIZE > VC8K_STRM_BUFSZ)
        {
            memcpy(&sbuff[0], &sbuff[sidx], avail);
            sidx = 0;
        }

        rlen = VC8K_FRAME_MAX_SIZE - avail;

        if (rlen != 0)
        {
            while (vsink_rxq_read(&sbuff[sidx + avail], rlen) != rlen)
            {
                // sysprintf("Wait USB data.\n");
                vTaskDelay(1);
            }
            avail += rlen;
        }
    }

    VC8000_H264_Close_Instance(handle);
}

int video_init(void)
{
    /* Open DISP IP Clock and set multi-function pins */
    DISP_Open();

    /* Configure display attributes of LCD panel */
    DISPLIB_LCDInit(LcdPanelInfo);

    /* Configure DISP Framebuffer settings  */
    DISPLIB_SetFBConfig(eFBFmt_A8R8G8B8, LcdPanelInfo.u32ResolutionWidth,
                        LcdPanelInfo.u32ResolutionHeight, DISP_BUFF_BASE);

    /* Start to display */
    DISPLIB_EnableOutput(eLayer_Video);

    xTaskCreate(vVideo_Task, "VIDEOTASK", configMINIMAL_STACK_SIZE, NULL,
                tskIDLE_PRIORITY + 1, NULL);
}

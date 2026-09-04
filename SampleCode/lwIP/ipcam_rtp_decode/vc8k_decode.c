/* vc8k_decode.c */

#include "vc8k_decode.h"
#include "vc8000_lib.h"
#include "rtp_ring.h"
#include "displib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "NuMicro.h"
#include <string.h>

extern DISP_LCD_INFO LcdPanelInfo;

/* VC8000 working heap (32 MB) */
static uint8_t s_vc8k_heap[0x2000000] __attribute__((aligned(32)));

extern uint8_t s_au8Primary[];
#define VC8K_FB_ADDR   ptr_to_u32(s_au8Primary)

#ifndef nc_ptr
#define nc_ptr(x)   (x)
#endif

static volatile uint32_t s_h264_frame_count = 0;
static volatile uint32_t s_h264_bytes_consumed = 0;
#ifdef HAVE_AVTP
static TickType_t s_h264_fps_start_tick;
static uint32_t s_h264_fps_frame_count;
#endif

/* Called by VC8000 library when one H.264 frame is decoded */
static void vc8k_h264_done_cb(void)
{
    s_h264_frame_count++;
#ifdef HAVE_AVTP
    s_h264_fps_frame_count++;
    TickType_t current_tick = xTaskGetTickCount();
    TickType_t elapsed_ticks = current_tick - s_h264_fps_start_tick;

    if (elapsed_ticks >= pdMS_TO_TICKS(1000U))
    {
        sysprintf("VC8000 decode FPS: %lu\n",
                  (unsigned long)((s_h264_fps_frame_count * 1000U) /
                                  (elapsed_ticks * portTICK_PERIOD_MS)));
        s_h264_fps_frame_count = 0U;
        s_h264_fps_start_tick = current_tick;
    }
#endif
}

int receive_nal_decode(int handle, rtp_ringbuf_t *rb)
{
    uint8_t *ptr;
    size_t available;
    size_t consumed;
    uint32_t remain;
    BaseType_t ret;
    int alive = 0;

    vTaskPrioritySet(NULL, tskIDLE_PRIORITY + 1UL);

    for (;;)
    {
        /* Wait until data >= watermark */
        if(rtp_ringbuf_used(rb) < TRIGGER_LEVEL_BYTES)
        {
            taskYIELD();
            continue;
        }

        // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        available = rtp_ringbuf_peek(rb, &ptr);

        /* Call decoder directly on ring buffer memory */
        ret = VC8000_H264_Decode_Run(handle,
                                     (uint8_t *)nc_ptr(ptr),
                                     available,
                                     NULL,
                                     &remain);

        if (ret != 0) {
            sysprintf("Decoder error=%d\n", ret);
            return ret;
        }

        consumed = available - remain;
        s_h264_bytes_consumed += consumed;

        /* Advance ring buffer read index by consumed bytes */
        rtp_ringbuf_consume(rb, consumed);

        if (rtp_ringbuf_used(rb) > HIGH_WATERMARK_ALERT) {
            sysprintf("RTP ring buffer high watermark alert! used=%u\n",
                      (unsigned)rtp_ringbuf_used(rb));
        }

        // if (alive++ % 30 == 0)
        //    sysprintf("decode %d, read %d\n", alive-1, rb->r);
    }

    return 0;
}


static struct pp_params pp;
void vc8k_decode_task(void *arg)
{
    rtp_ringbuf_t *ringbuf = (rtp_ringbuf_t *)arg;

    int ret;
    int handle;

    sysprintf("vc8k_decode_task started...\n");
#ifdef HAVE_AVTP
    s_h264_fps_start_tick = xTaskGetTickCount();
    s_h264_fps_frame_count = 0U;
#endif
resume:
    /* 1. Initialize VC8000 library using s_vc8k_heap in this file */
    ret = VC8000_Init(ptr_to_u32(nc_ptr(s_vc8k_heap)), sizeof(s_vc8k_heap));
    if (ret < 0)
    {
        sysprintf("VC8000_Init failed: %d\n", ret);
        vTaskDelete(NULL);
    }

    /* Install H.264 decode-done callback */
#ifdef HAVE_AVTP
    VC8000_InstallH264DecCallback(vc8k_h264_done_cb);
#endif

    /* 2. Open H.264 instance */
    handle = VC8000_H264_Open_Instance();
    if (handle < 0)
    {
        sysprintf("VC8000_H264_Open_Instance failed: %d\n", handle);
        vTaskDelete(NULL);
    }

    /* 3. Configure PP output to Display */

    memset(&pp, 0, sizeof(pp));

    int fb_w = (int)LcdPanelInfo.u32ResolutionWidth;
    int fb_h = (int)LcdPanelInfo.u32ResolutionHeight;

    if (fb_w > VC8000_PP_MAX_WIDTH)  fb_w = VC8000_PP_MAX_WIDTH;
    if (fb_h > VC8000_PP_MAX_HEIGHT) fb_h = VC8000_PP_MAX_HEIGHT;

    pp.frame_buf_w = fb_w;
    pp.frame_buf_h = fb_h;

    pp.img_out_x = 0;
    pp.img_out_y = 0;
    pp.img_out_w = fb_w;
    pp.img_out_h = fb_h;

    pp.img_out_fmt = VC8000_PP_F_RGB888;
    pp.rotation    = VC8000_PP_ROTATION_NONE;

    /* Let PP feed directly to Display (same as the original H264 demo) */
    //pp.pp_out_dst   = VC8000_PP_OUT_DST_DISPLAY;
    /* No need to set pp_out_paddr; in DISPLAY mode it is controlled by DISPLIB_SetFBConfig */
    pp.pp_out_dst   = VC8000_PP_OUT_DST_USER;
    pp.pp_out_paddr = VC8K_FB_ADDR;      // 0x88000000

    pp.contrast     = 8;
    pp.brightness   = 0;
    pp.saturation   = 32;
    pp.alpha        = 255;
    pp.transparency = 0;

    ret = VC8000_H264_Enable_PP(handle, &pp);
    if (ret < 0)
    {
        sysprintf("VC8000_H264_Enable_PP failed: %d\n", ret);
        VC8000_H264_Close_Instance(handle);
        vTaskDelete(NULL);
    }

    sysprintf("vc8k_decode_task STARTED! handle=%d, fb=0x%08X, fb_w=%d, fb_h=%d\n",
              handle, VC8K_FB_ADDR, fb_w, fb_h);

    /* Test color bars: enable temporarily to confirm display is reading from 0x88000000 */
    // vc8k_test_fill_framebuffer();
    // sysprintf("vc8k_decode_task: framebuffer test pattern filled.\n");

    /* 4. Decode loop (read bitstream from g_rtp_ring) */
    ret = receive_nal_decode(handle, ringbuf);

    sysprintf("VC8000_BitstreamDecodeLoop exit, ret=%d\n", ret);

    VC8000_H264_Close_Instance(handle);

    if(ret)
        goto resume;

    vTaskDelete(NULL);
}

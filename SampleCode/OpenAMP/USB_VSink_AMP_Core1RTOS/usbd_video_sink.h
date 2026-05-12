/***************************************************************************//**
 * @file     usbd_video_sink.h
 * @brief    MA35 USB Device Video Sink driver header
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2026 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef _USBD_VIDEO_SINK_H_
#define _USBD_VIDEO_SINK_H_

/* Define the vendor id and product id */
#define USBD_VID                    0x0416
#define USBD_PID                    0x5963

/* Define EP maximum packet size */
#define EP0_MAX_PKT_SIZE            64
#define EP0_OTHER_MAX_PKT_SIZE      64
#define EP1_MAX_PKT_SIZE            512
#define EP1_OTHER_MAX_PKT_SIZE      64

/* Define the interrupt In EP number */
#define BULK_IN_EP_NUM              0x01
#define BULK_OUT_EP_NUM             0x01

/* Define Descriptor information */
#define USBD_SELF_POWERED           0
#define USBD_REMOTE_WAKEUP          0
#define USBD_MAX_POWER              50  /* The unit is in 2mA. ex: 50 * 2mA = 100mA */

#define VSINK_BUFF_SIZE             (2 * 1024 * 1024)
#define VSINK_CHUNK_SIZE            (16 * 1024)

#define TMR2_TICKS_PER_SEC          1000

#define VC8K_BUFF_BASE              0x8A800000
#define VC8K_BUFF_SIZE              0x02000000
#define VC8K_STRM_BUFSZ             (2 * 1024 * 1024)
#define VC8K_FRAME_MAX_SIZE         (384 * 1024)
#define DISP_BUFF_BASE              0x8C800000

extern volatile uint32_t g_tmr2_ticks;
extern S_HSUSBD_INFO_T gsHSInfo;

void HSUSBD_IRQHandler(void);
void usbd_vsink_config(void);
void usbd_vsink_class_request(void);
int usbd_vsink_bulk_rx(void *buff);
uint32_t vsink_rxq_read(uint8_t *pdest, uint32_t len);
int video_init(void);

#endif /* _USBD_VIDEO_SINK_H_ */

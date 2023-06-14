/***************************************************************************//**
 * @file     hid_mousekeyboard.h
 * @brief    HSUSB HID Sample header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef __USBD_HID_H__
#define __USBD_HID_H__

/* Define the vendor id and product id */
#define USBD_VID                0x0416
#define USBD_PID                0xC145

/*!<Define HID Class Specific Request */
#define GET_REPORT              0x01
#define GET_IDLE                0x02
#define GET_PROTOCOL            0x03
#define SET_REPORT              0x09
#define SET_IDLE                0x0A
#define SET_PROTOCOL            0x0B

/*!<USB HID Interface Class protocol */
#define HID_NONE                0x00
#define HID_KEYBOARD            0x01
#define HID_MOUSE               0x02

/*!<USB HID Class Report Type */
#define HID_RPT_TYPE_INPUT      0x01
#define HID_RPT_TYPE_OUTPUT     0x02
#define HID_RPT_TYPE_FEATURE    0x03

/*!<USB HID Protocol Type */
#define HID_BOOT_PROTOCOL       0x00
#define HID_REPORT_PROTOCOL     0x01

/*-------------------------------------------------------------*/
/* Define EP maximum packet size */
#define EP0_MAX_PKT_SIZE        64
#define EP0_OTHER_MAX_PKT_SIZE  64
#define EP1_MAX_PKT_SIZE        64
#define EP1_OTHER_MAX_PKT_SIZE  64
#define EP2_MAX_PKT_SIZE        64
#define EP2_OTHER_MAX_PKT_SIZE  64

/* Define the interrupt In EP number */
#define INT_IN_EP_NUM   0x01
#define INT_IN_EP_NUM_KB    0x02

/* Define Descriptor information */
#define HID_DEFAULT_INT_IN_INTERVAL     8   /* (2^8-1) */
#define HID_DEFAULT_INT_IN_INTERVAL_KB  16  /* (2^16-1) */
#define USBD_SELF_POWERED               0
#define USBD_REMOTE_WAKEUP              0
#define USBD_MAX_POWER                  50  /* The unit is in 2mA. ex: 50 * 2mA = 100mA */

#define LEN_CONFIG_AND_SUBORDINATE      (LEN_CONFIG+LEN_INTERFACE*2+LEN_HID*2+LEN_ENDPOINT*2)


/*-------------------------------------------------------------*/

/*-------------------------------------------------------------*/
void HID_InitForHighSpeed(void);
void HID_InitForFullSpeed(void);
void HID_Init(void);
void HID_ClassRequest(void);
void HSUSBD_IRQHandler(void);
void HID_Process(void);

#endif  /* __USBD_HID_H__ */

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/

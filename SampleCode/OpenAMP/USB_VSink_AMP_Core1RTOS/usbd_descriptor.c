#include "NuMicro.h"
#include "hsusbd.h"
#include "usbd_video_sink.h"

uint8_t gu8DeviceDescriptor[] __attribute__((aligned(32))) =
{
    LEN_DEVICE,     /* bLength */
    DESC_DEVICE,    /* bDescriptorType */
    0x00, 0x02,     /* bcdUSB */
    0x00,           /* bDeviceClass */
    0x00,           /* bDeviceSubClass */
    0x00,           /* bDeviceProtocol */
    0x40,           /* bMaxPacketSize0 */
    USBD_VID & 0x00FF,
    ((USBD_VID & 0xFF00) >> 8),
    USBD_PID & 0x00FF,
    ((USBD_PID & 0xFF00) >> 8),
    0x00, 0x01,     /* bcdDevice */
    0x00,           /* iManufacture */
    0x00,           /* iProduct */
    0x00,           /* iSerialNumber */
    0x01            /* bNumConfigurations */
};

uint8_t gu8QualifierDescriptor[] __attribute__((aligned(32))) =
{
    LEN_QUALIFIER,  /* bLength */
    DESC_QUALIFIER, /* bDescriptorType */
    0x00, 0x02,     /* bcdUSB */
    0x00,           /* bDeviceClass */
    0x00,           /* bDeviceSubClass */
    0x00,           /* bDeviceProtocol */
    0x40,           /* bMaxPacketSize0 */
    0x01,           /* bNumConfigurations */
    0x00
};

uint8_t gu8ConfigDescriptor[] __attribute__((aligned(32))) =
{
    LEN_CONFIG,     /* bLength */
    DESC_CONFIG,    /* bDescriptorType */
    (LEN_CONFIG+LEN_INTERFACE+LEN_ENDPOINT*2), 0x00,
    0x01,           /* bNumInterfaces */
    0x01,           /* bConfigurationValue */
    0x00,           /* iConfiguration */
    0xC0,           /* bmAttributes */
    0x32,           /* MaxPower */

    LEN_INTERFACE,  /* bLength */
    DESC_INTERFACE, /* bDescriptorType */
    0x00,           /* bInterfaceNumber */
    0x00,           /* bAlternateSetting */
    0x02,           /* bNumEndpoints */
    0x00,           /* bInterfaceClass */
    0x00,           /* bInterfaceSubClass */
    0x00,           /* bInterfaceProtocol */
    0x00,           /* iInterface */

    LEN_ENDPOINT,   /* bLength */
    DESC_ENDPOINT,  /* bDescriptorType */
    (BULK_IN_EP_NUM | EP_INPUT),    /* bEndpointAddress */
    EP_BULK,        /* bmAttributes */
    0x00, 0x02,     /* wMaxPacketSize: 512 */
    0x01,           /* bInterval */

    LEN_ENDPOINT,   /* bLength */
    DESC_ENDPOINT,  /* bDescriptorType */
    (BULK_OUT_EP_NUM | EP_OUTPUT),  /* bEndpointAddress */
    EP_BULK,        /* bmAttributes */
    0x00, 0x02,     /* wMaxPacketSize: 512 */
    0x01            /* bInterval */
};

uint8_t gu8OtherConfigDescriptorHS[] __attribute__((aligned(32))) =
{
    LEN_CONFIG,     /* bLength */
    DESC_CONFIG,    /* bDescriptorType */
    (LEN_CONFIG+LEN_INTERFACE+LEN_ENDPOINT*2), 0x00,
    0x01,           /* bNumInterfaces */
    0x01,           /* bConfigurationValue */
    0x00,           /* iConfiguration */
    0xC0,           /* bmAttributes */
    0x32,           /* MaxPower */

    LEN_INTERFACE,  /* bLength */
    DESC_INTERFACE, /* bDescriptorType */
    0x00,           /* bInterfaceNumber */
    0x00,           /* bAlternateSetting */
    0x02,           /* bNumEndpoints */
    0x00,           /* bInterfaceClass */
    0x00,           /* bInterfaceSubClass */
    0x00,           /* bInterfaceProtocol */
    0x00,           /* iInterface */

    LEN_ENDPOINT,   /* bLength */
    DESC_ENDPOINT,  /* bDescriptorType */
    (BULK_IN_EP_NUM | EP_INPUT),    /* bEndpointAddress */
    EP_BULK,        /* bmAttributes */
    0x00, 0x02,     /* wMaxPacketSize: 512 */
    0x01,           /* bInterval */

    LEN_ENDPOINT,   /* bLength */
    DESC_ENDPOINT,  /* bDescriptorType */
    (BULK_OUT_EP_NUM | EP_OUTPUT),  /* bEndpointAddress */
    EP_BULK,        /* bmAttributes */
    0x00, 0x02,     /* wMaxPacketSize: 512 */
    0x01            /* bInterval */
};

uint8_t gu8ConfigDescriptorFS[] __attribute__((aligned(32))) =
{
	0x09,           /* bLength */
	0x02,           /* bDescriptorType */
	0x20, 0x00,     /* wTotalLength */
	0x01,           /* bNumInterfaces */
	0x01,           /* bConfigurationValue */
	0x00,           /* iConfiguration */
	0xC0,           /* bmAttributes */
	0x32,           /* MaxPower */

	0x09,           /* bLength */
	0x04,           /* bDescriptorType */
	0x00,           /* bInterfaceNumber */
	0x00,           /* bAlternateSetting */
	0x02,           /* bNumEndpoints */
	0x00,           /* bInterfaceClass */
	0x00,           /* bInterfaceSubClass */
	0x00,           /* bInterfaceProtocol */
	0x00,           /* iInterface */

	0x07,           /* bLength */
	0x05,           /* bDescriptorType */
	0x81,           /* bEndpointAddress */
	0x02,           /* bmAttributes */
	0x40, 0x00,     /* wMaxPacketSize */
	0x01,           /* bInterval */

	0x07,           /* bLength */
	0x05,           /* bDescriptorType */
	0x01,           /* bEndpointAddress */
	0x02,           /* bmAttributes */
	0x40, 0x00,     /* wMaxPacketSize */
	0x01            /* bInterval */
};

uint8_t gu8OtherConfigDescriptorFS[] __attribute__((aligned(32))) =
{
	0x09,           /* bLength */
	0x02,           /* bDescriptorType */
	0x20, 0x00,     /* wTotalLength */
	0x01,           /* bNumInterfaces */
	0x01,           /* bConfigurationValue */
	0x00,           /* iConfiguration */
	0xC0,           /* bmAttributes */
	0x32,           /* MaxPower */

	0x09,           /* bLength */
	0x04,           /* bDescriptorType */
	0x00,           /* bInterfaceNumber */
	0x00,           /* bAlternateSetting */
	0x02,           /* bNumEndpoints */
	0x00,           /* bInterfaceClass */
	0x00,           /* bInterfaceSubClass */
	0x00,           /* bInterfaceProtocol */
	0x00,           /* iInterface */

	0x07,           /* bLength */
	0x05,           /* bDescriptorType */
	0x81,           /* bEndpointAddress */
	0x02,           /* bmAttributes */
	0x40, 0x00,     /* wMaxPacketSize */
	0x01,           /* bInterval */

	0x07,           /* bLength */
	0x05,           /* bDescriptorType */
	0x01,           /* bEndpointAddress */
	0x02,           /* bmAttributes */
	0x40, 0x00,     /* wMaxPacketSize */
	0x01            /* bInterval */
};

uint8_t gu8StringLang[4] __attribute__((aligned(32))) =
{
    4,              /* bLength */
    DESC_STRING,    /* bDescriptorType */
    0x09, 0x04
};

uint8_t gu8VendorStringDesc[] __attribute__((aligned(32))) =
{
    16,
    DESC_STRING,
    'N', 0, 'u', 0, 'v', 0, 'o', 0, 't', 0, 'o', 0, 'n', 0
};

uint8_t gu8ProductStringDesc[] __attribute__((aligned(32))) =
{
    22,             /* bLength          */
    DESC_STRING,    /* bDescriptorType  */
    'U', 0, 'S', 0, 'B', 0, ' ', 0, 'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0
};

uint8_t gu8StringSerial[] __attribute__((aligned(32))) =
{
    26,             // bLength
    DESC_STRING,    // bDescriptorType
    'A', 0, '0', 0, '0', 0, '0', 0, '2', 0, '0', 0, '1', 0, '4', 0, '1', 0, '1', 0, '0', 0, '4', 0
};

uint8_t *gpu8UsbString[4] =
{
    gu8StringLang,
    gu8VendorStringDesc,
    gu8ProductStringDesc,
    gu8StringSerial,
};

uint8_t *gu8UsbHidReport[3] =
{
    NULL,
    NULL,
    NULL,
};

uint32_t gu32UsbHidReportLen[3] =
{
    0,
    0,
    0,
};

uint32_t gu32ConfigHidDescIdx[3] =
{
    0,
    0,
    0
};

S_HSUSBD_INFO_T gsHSInfo =
{
    gu8DeviceDescriptor,
    gu8ConfigDescriptor,
    gpu8UsbString,
    gu8QualifierDescriptor,
    gu8ConfigDescriptorFS,
    gu8OtherConfigDescriptorHS,
    gu8OtherConfigDescriptorFS,
    gu8UsbHidReport,
    gu32UsbHidReportLen,
    gu32ConfigHidDescIdx,
};

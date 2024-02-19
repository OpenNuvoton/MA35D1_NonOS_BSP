/* USBD */

/* DWC2_UDC_OTG_GRSTCTL */
#define AHB_MASTER_IDLE		(1u<<31)
#define CORE_SOFT_RESET		(0x1<<0)

/* DWC2_UDC_OTG_GAHBCFG */
#define MODE_SLAVE			(0<<5)
#define MODE_DMA			(1<<5)
#define BURST_SINGLE		(0<<1)
#define BURST_INCR			(1<<1)
#define BURST_INCR4			(3<<1)
#define BURST_INCR8			(5<<1)
#define BURST_INCR16		(7<<1)
#define GBL_INT_UNMASK		(1<<0)
#define GBL_INT_MASK		(0<<0)

/* DWC2_UDC_OTG_GINTSTS/DWC2_UDC_OTG_GINTMSK core interrupt register */
#define INT_RESUME				(1u<<31)
#define INT_OUT_EP				(0x1<<19)
#define INT_IN_EP				(0x1<<18)
#define INT_ENUMMA35D1NE			(0x1<<13)
#define INT_RESET				(0x1<<12)
#define INT_SUSPEND				(0x1<<11)
#define INT_EARLY_SUSPEND		(0x1<<10)
#define INT_SOF					(0x1<<3)
#define INT_GOUTNakEff			(0x01<<7)
#define INT_GINNakEff			(0x01<<6)

/* DWC2_UDC_OTG_DCTL device control register */
#define NORMAL_OPERATION	(0x1<<0)
#define SOFT_DISCONNECT		(0x1<<1)
#define DCTL_TSTCTL_MASK		(0x7 << 4)

/* DWC2_UDC_OTG_DIEPINTn/MA35D1EPINTn device IN/OUT endpoint interrupt register */
#define BNAMSK							(0x1<<9)
#define BACK2BACK_SETUP_RECEIVED		(0x1<<6)
#define INTKNEPMIS						(0x1<<5)
#define INTKN_TXFEMP					(0x1<<4)
#define CTRL_OUT_EP_SETUP_PHASE_MA35D1NE	(0x1<<3)
#define AHB_ERROR						(0x1<<2)
#define EPDISBLD						(0x1<<1)
#define TRANSFER_MA35D1NE					(0x1<<0)

/* Core Reset Register (GRSTCTL) */
#define TX_FIFO_FLUSH                   (0x1 << 5)
#define RX_FIFO_FLUSH                   (0x1 << 4)
#define TX_FIFO_NUMBER(x)               (x << 6)
#define TX_FIFO_FLUSH_ALL               TX_FIFO_NUMBER(0x10)

/* Masks definitions */
#define GINTMSK_INIT	(INT_OUT_EP | INT_IN_EP | INT_RESUME | INT_ENUMMA35D1NE | INT_RESET | INT_SUSPEND)
#define MA35D1EPMSK_INIT	(BNAMSK | CTRL_OUT_EP_SETUP_PHASE_MA35D1NE | AHB_ERROR | TRANSFER_MA35D1NE)
#define DIEPMSK_INIT	(BNAMSK | AHB_ERROR | TRANSFER_MA35D1NE)
#define GAHBCFG_INIT	(MODE_DMA | BURST_INCR4 | GBL_INT_UNMASK)

/* DWC2_UDC_OTG_DAINT device all endpoint interrupt register */
#define DAINT_OUT_BIT			(16)
#define DAINT_MASK				(0xFFFF)

/* Device Endpoint-N Control Register (DIEPCTLn/MA35D1EPCTLn) */
#define DIEPCTL_TX_FIFO_NUM(x)		(x << 22)
#define DIEPCTL_TX_FIFO_NUM_MASK	(~DIEPCTL_TX_FIFO_NUM(0xF))

/* Device ALL Endpoints Interrupt Register (DAINT) */
#define DAINT_IN_EP_INT(x)		(x << 0)
#define DAINT_OUT_EP_INT(x)		(x << 16)


/* DWC2_UDC_OTG_DIEPCTL0/MA35D1EPCTL0 device
   control IN/OUT endpoint 0 control register */
#define DEPCTL_EPENA			(0x1<<31)
#define DEPCTL_EPDIS			(0x1<<30)
#define DEPCTL_SETD1PID			(0x1<<29)
#define DEPCTL_SETD0PID			(0x1<<28)
#define DEPCTL_SNAK				(0x1<<27)
#define DEPCTL_CNAK				(0x1<<26)
#define DEPCTL_STALL			(0x1<<21)
#define DEPCTL_TYPE_BIT			(18)
#define DEPCTL_TYPE_MASK		(0x3<<18)
#define DEPCTL_CTRL_TYPE		(0x0<<18)
#define DEPCTL_ISO_TYPE			(0x1<<18)
#define DEPCTL_BULK_TYPE		(0x2<<18)
#define DEPCTL_INTR_TYPE		(0x3<<18)
#define DEPCTL_USBACTEP			(0x1<<15)
#define DEPCTL_NEXT_EP_BIT		(11)
#define DEPCTL_MPS_BIT			(0)
#define DEPCTL_MPS_MASK			(0x7FF)

#define DEPCTL0_MPS_64			(0x0<<0)
#define DEPCTL0_MPS_32			(0x1<<0)
#define DEPCTL0_MPS_16			(0x2<<0)
#define DEPCTL0_MPS_8			(0x3<<0)
#define DEPCTL_MPS_BULK_512		(512<<0)
#define DEPCTL_MPS_INT_MPS_16	(16<<0)

/* Enumeration speed */
#define USB_HIGH_30_60MHZ		(0x0<<1)
#define USB_FULL_30_60MHZ		(0x1<<1)

/* ep0 state */
#define WAIT_FOR_SETUP          0
#define DATA_STATE_XMIT         1
#define DATA_STATE_NEED_ZLP     2
#define WAIT_FOR_OUT_STATUS     3
#define DATA_STATE_RECV         4
#define WAIT_FOR_COMPLETE		5
#define WAIT_FOR_OUT_COMPLETE	6
#define WAIT_FOR_IN_COMPLETE	7
#define WAIT_FOR_NULL_COMPLETE	8


/* ************************************************************************* */
/* IO
 */

/* USB 2.0 defines three speeds, here's how Linux identifies them */

enum usb_device_speed {
	USB_SPEED_UNKNOWN = 0,			/* enumerating */
	USB_SPEED_LOW, USB_SPEED_FULL,	/* usb 1.1 */
	USB_SPEED_HIGH,				/* usb 2.0 */
};

enum ep_type {
	ep_control, ep_bulk_in, ep_bulk_out, ep_interrupt_in
};

/**
 * struct dwc2_dma_desc - DMA descriptor structure,
 * used for both host and gadget modes
 *
 * @status: DMA descriptor status quadlet
 * @buf:    DMA descriptor data buffer pointer
 *
 * DMA Descriptor structure contains two quadlets:
 * Status quadlet and Data buffer pointer.
 */

typedef union dev_dma_desc_sts
{
	unsigned int d32;
	struct
	{
		unsigned bytes:16;	// Received number of bytes
		unsigned reserved16_22:7;
		unsigned mtrf:1;	// Multiple Transfer - only for OUT EPs
		unsigned sr:1;		// Setup Packet received - only for OUT EPs
		unsigned ioc:1;		// Interrupt On Complete.
		unsigned sp:1;		// Short Packet
		unsigned l:1;		// Last
		unsigned sts:2;		// Receive Status
		unsigned bs:2;		// Buffer Status
	} b;

} dev_dma_desc_sts_t;

struct dwc2_dma_desc {
	dev_dma_desc_sts_t status;
	unsigned int buf;
} __packed;

struct dwc2_ep {

	unsigned int maxpacket;
	int len;

	unsigned char ready;
	unsigned char bEndpointAddress;
	unsigned char bmAttributes;

	enum ep_type ep_type;
	int fifo_num;

	unsigned int desc_list_dma;
	struct dwc2_dma_desc *desc_list;
	unsigned char desc_count;

	unsigned int next_desc;
	unsigned int compl_desc;
};


struct dwc2_udc {
	enum usb_device_speed	speed;
	int ep0state;
	struct dwc2_ep ep[3];

	unsigned char state;
	unsigned int volatile txlen;
};

/*---------------------- USB Device Controller -------------------------*/

/* Device Logical IN Endpoint-Specific Registers */
struct dwc2_dev_in_endp
{
	volatile unsigned int diepctl;
	volatile const unsigned int res1;
	volatile unsigned int diepint;
	volatile const unsigned int res2[2];
	volatile unsigned int diepdma;
	volatile unsigned int dtxfsts;
	volatile unsigned int diepdmab;
};

/* Device Logical OUT Endpoint-Specific Registers */
struct dwc2_dev_out_endp
{
	volatile unsigned int MA35D1epctl;
	volatile const unsigned int res1;
	volatile unsigned int MA35D1epint;
	volatile const unsigned int res2[2];
	volatile unsigned int MA35D1epdma;
	volatile const unsigned int res3;
	volatile unsigned int MA35D1epdmab;
};

/* USB2.0 OTG Controller register */
struct dwc2_usbotg_reg
{
	/* Core Global Registers */
	volatile unsigned int gotgctl; /* OTG Control & Status */
	volatile unsigned int gotgint; /* OTG Interrupt */
	volatile unsigned int gahbcfg; /* Core AHB Configuration */
	volatile unsigned int gusbcfg; /* Core USB Configuration */
	volatile unsigned int grstctl; /* Core Reset */
	volatile unsigned int gintsts; /* Core Interrupt */
	volatile unsigned int gintmsk; /* Core Interrupt Mask */
	volatile unsigned int grxstsr; /* Receive Status Debug Read/Status Read */
	volatile unsigned int grxstsp; /* Receive Status Debug Pop/Status Pop */
	volatile unsigned int grxfsiz; /* Receive FIFO Size */
	volatile unsigned int gnptxfsiz; /* Non-Periodic Transmit FIFO Size */
	volatile const unsigned int res0[5]; /* 0x2c ~ 0x3c */
	volatile unsigned int gsnpsid; /* 0x040 Synopsys ID Register */
	volatile unsigned int ghwcfg1; /* User HW Config1 */
	volatile unsigned int ghwcfg2; /* User HW Config2 */
	volatile unsigned int ghwcfg3; /* User HW Config3 */
	volatile unsigned int ghwcfg4; /* User HW Config4 */
	volatile unsigned int glpmcfg; /* LPM Config */
	volatile const unsigned int res1[1]; /* 0x58 */
	volatile unsigned int gdfifocfg; /* Global DFIFO Configuration  */
	volatile const unsigned int res2[1]; /* 0x60 */
	volatile unsigned int grefclk; /* ref_clk Control */
	volatile unsigned int gintmsk2; /* Interrupt Mask Register 2 */
	volatile unsigned int gintsts2; /* Interrupt Register 2 */
	volatile const unsigned int res3[37]; /* 0x70 ~ 0x100 */
	volatile unsigned int dieptxf[8]; /* Device Periodic Transmit FIFO size register */
	volatile const unsigned int res4[439]; /* 0x124 ~ 0x7fc */
	/* Device Configuration */
	volatile unsigned int dcfg; /* Device Config */
	volatile unsigned int dctl; /* Device Control */
	volatile unsigned int dsts; /* Device Status */
	volatile const unsigned int res5[1]; /* 0x80c */
	volatile unsigned int diepmsk; /* Device IN Endpoint Common Interrupt Mask */
	volatile unsigned int MA35D1epmsk; /* Device OUT Endpoint Common Interrupt Mask */
	volatile unsigned int daint; /* Device All Endpoints Interrupt */
	volatile unsigned int daintmsk; /* Device All Endpoints Interrupt Mask */
	volatile const unsigned int res6[4]; /* 0x820 ~ 0x82c */
	volatile unsigned int dthrctl; /* Device Threshold Control Register */
	volatile unsigned int diepempmsk; /* Device IN Endpoint FIFO Empty Interrupt Mask Register */
	volatile const unsigned int res7[50]; /* 0x838 ~ 0x8fc */
	struct dwc2_dev_in_endp in_endp[9]; /* 0x900 */
	unsigned int res8[56]; /* 0xa20 ~ 0xafc */
	struct dwc2_dev_out_endp out_endp[9];	/* 0xb00 */
};

#define USBD    ((volatile struct dwc2_usbotg_reg *)USBD_BASE)

/****************************************************/
/* control request */
struct usb_ctrlrequest {
	unsigned char bRequestType;
	unsigned char bRequest;
	unsigned short wValue;
	unsigned short wIndex;
	unsigned short wLength;
} __attribute__ ((packed));

struct usb_ctrlrequest setup_pkt;
static struct usb_ctrlrequest *usb_ctrl;
static int reset_available = 1;
static int test_mode = 0;

/*
 * USB types, the second of three bRequestType fields
 */
#define USB_TYPE_MASK			(0x03 << 5)
#define USB_TYPE_STANDARD		(0x00 << 5)
#define USB_TYPE_CLASS			(0x01 << 5)
#define USB_TYPE_VENMA35D1R			(0x02 << 5)
#define USB_TYPE_RESERVED		(0x03 << 5)

/*
 * Standard requests, for the bRequest field of a SETUP packet.
 *
 * These are qualified by the bRequestType field, so that for example
 * TYPE_CLASS or TYPE_VENMA35D1R specific feature flags could be retrieved
 * by a GET_STATUS request.
 */
#define USB_REQ_GET_STATUS			0x00
#define USB_REQ_CLEAR_FEATURE		0x01
#define USB_REQ_SET_FEATURE			0x03
#define USB_REQ_SET_ADDRESS			0x05
#define USB_REQ_GET_DESCRIPTOR		0x06
#define USB_REQ_SET_DESCRIPTOR		0x07
#define USB_REQ_GET_CONFIGURATION	0x08
#define USB_REQ_SET_CONFIGURATION	0x09
#define USB_REQ_GET_INTERFACE		0x0A
#define USB_REQ_SET_INTERFACE		0x0B

#define USB_DT_DEVICE				0x01
#define USB_DT_CONFIG				0x02
#define USB_DT_STRING				0x03
#define USB_DT_INTERFACE			0x04
#define USB_DT_ENDPOINT				0x05
#define USB_DT_DEVICE_QUALIFIER		0x06
#define USB_DT_OTHER_SPEED_CONFIG	0x07

/*
 * USB recipients, the third of three bRequestType fields
 */
#define USB_RECIP_MASK			0x1f
#define USB_RECIP_DEVICE		0x00
#define USB_RECIP_INTERFACE		0x01
#define USB_RECIP_ENDPOINT		0x02

/* USB Descriptor Type */
#define DESC_DEVICE         0x01
#define DESC_CONFIG         0x02
#define DESC_STRING         0x03
#define DESC_INTERFACE      0x04
#define DESC_ENDPOINT       0x05
#define DESC_QUALIFIER      0x06
#define DESC_OTHERSPEED     0x07
#define DESC_IFPOWER        0x08
#define DESC_OTG            0x09

/* USB HID Descriptor Type */
#define DESC_HID            0x21
#define DESC_HID_RPT        0x22

/* USB Descriptor Length */
#define LEN_DEVICE          18
#define LEN_QUALIFIER       10
#define LEN_CONFIG          9
#define LEN_INTERFACE       9
#define LEN_ENDPOINT        7
#define LEN_OTG             5
#define LEN_HID             9


extern struct dwc2_udc udcInfo;
void MA35D1wnloadFW(void);
void Delay(uint32_t ticks);
void usbInit();
void dwc2_udc_irq();
int usb_recv(uint8_t *buf, uint32_t len);
int usb_send(uint8_t *buf, uint32_t len);
void SendAck(uint32_t status);



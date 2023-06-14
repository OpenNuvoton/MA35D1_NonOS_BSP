/**************************************************************************//**
 * @file     hsusbd.h
 *
 * @brief    HSUSBD driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __HSUSBD_H__
#define __HSUSBD_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup HSUSBD_Driver HSUSBD Driver
  @{
*/

/** @addtogroup HSUSBD_EXPORTED_CONSTANTS HSUSBD Exported Constants
  @{
*/
/// @cond HIDDEN_SYMBOLS
#define HSUSBD_MAX_EP     12ul

#define Maximum(a,b)    (a)>(b) ? (a) : (b)
#define Minimum(a,b)    (((a)<(b)) ? (a) : (b))


#define EP0     0ul       /*!< Control Endpoint  \hideinitializer */
#define EP1     1ul       /*!< Endpoint 1  \hideinitializer */
#define EP2     2ul       /*!< Endpoint 2  \hideinitializer */
#define EP3     3ul       /*!< Endpoint 3  \hideinitializer */
#define EP4     4ul       /*!< Endpoint 4  \hideinitializer */
#define EP5     5ul       /*!< Endpoint 5  \hideinitializer */
#define EP6     6ul       /*!< Endpoint 6  \hideinitializer */
#define EP7     7ul       /*!< Endpoint 7  \hideinitializer */
#define EP8     8ul       /*!< Endpoint 8  \hideinitializer */

/* USB Request Type */
#define REQ_STANDARD        0x00ul
#define REQ_CLASS           0x20ul
#define REQ_VENDOR          0x40ul

/* USB Standard Request */
#define GET_STATUS          0x00ul
#define CLEAR_FEATURE       0x01ul
#define SET_FEATURE         0x03ul
#define SET_ADDRESS         0x05ul
#define GET_DESCRIPTOR      0x06ul
#define SET_DESCRIPTOR      0x07ul
#define GET_CONFIGURATION   0x08ul
#define SET_CONFIGURATION   0x09ul
#define GET_INTERFACE       0x0Aul
#define SET_INTERFACE       0x0Bul
#define SYNC_FRAME          0x0Cul

/* USB Descriptor Type */
#define DESC_DEVICE         0x01ul
#define DESC_CONFIG         0x02ul
#define DESC_STRING         0x03ul
#define DESC_INTERFACE      0x04ul
#define DESC_ENDPOINT       0x05ul
#define DESC_QUALIFIER      0x06ul
#define DESC_OTHERSPEED     0x07ul
#define DESC_IFPOWER        0x08ul
#define DESC_OTG            0x09ul
#define DESC_BOS            0x0Ful
#define DESC_CAPABILITY     0x10ul

/* USB Device Capability Type */
#define CAP_WIRELESS        0x01ul
#define CAP_USB20_EXT       0x02ul

/* USB HID Descriptor Type */
#define DESC_HID            0x21ul
#define DESC_HID_RPT        0x22ul

/* USB Descriptor Length */
#define LEN_DEVICE          18ul
#define LEN_QUALIFIER       10ul
#define LEN_CONFIG          9ul
#define LEN_INTERFACE       9ul
#define LEN_ENDPOINT        7ul
#define LEN_OTG             5ul
#define LEN_BOS             5ul
#define LEN_HID             9ul
#define LEN_CCID            0x36ul
#define LEN_BOSCAP          7ul

/* USB Endpoint Type */
#define EP_ISO              0x01
#define EP_BULK             0x02
#define EP_INT              0x03

#define EP_INPUT            0x80
#define EP_OUTPUT           0x00

/* USB Feature Selector */
#define FEATURE_DEVICE_REMOTE_WAKEUP    0x01ul
#define FEATURE_ENDPOINT_HALT           0x00ul

typedef enum ep0_state
{
	WAIT_FOR_SETUP = 0,
	DATA_STATE_XMIT,
	DATA_STATE_NEED_ZLP,
	WAIT_FOR_OUT_STATUS,
	DATA_STATE_RECV,
	WAIT_FOR_COMPLETE,
	WAIT_FOR_OUT_COMPLETE,
	WAIT_FOR_IN_COMPLETE,
	WAIT_FOR_NULL_COMPLETE

} E_EP0_STATE;

/// @endcond HIDDEN_SYMBOLS

/* AHB Configuration Register (GAHBCFG) */
#define HSUSBD_GAHBCFG_BURST_SINGLE    (0x0 << 1)
#define HSUSBD_GAHBCFG_BURST_INCR      (0x1 << 1)
#define HSUSBD_GAHBCFG_BURST_INCR4     (0x3 << 1)
#define HSUSBD_GAHBCFG_BURST_INCR8     (0x5 << 1)
#define HSUSBD_GAHBCFG_BURST_INCR16    (0x7 << 1)

/* Device Endpoint Type (DOEPCTL/DIEPCTL) */
#define HSUSBD_DEPCTL_SETD1PID     (0x1 << 29)
#define HSUSBD_DEPCTL_SETD0PID     (0x1 << 28)
#define HSUSBD_DEPCTL_TYPE_CTRL    (0x0 << 18)
#define HSUSBD_DEPCTL_TYPE_ISO     (0x1 << 18)
#define HSUSBD_DEPCTL_TYPE_BULK    (0x2 << 18)
#define HSUSBD_DEPCTL_TYPE_INTR    (0x3 << 18)

/* Device Endpoint-N Interrupt Mask Register (DAINTMSK) */
#define HSUSBD_EP0_IN_EN    HSUSBD_DAINTMSK_InEpMsk0_Msk
#define HSUSBD_EP1_IN_EN    HSUSBD_DAINTMSK_InEpMsk1_Msk
#define HSUSBD_EP2_IN_EN    HSUSBD_DAINTMSK_InEpMsk2_Msk
#define HSUSBD_EP3_IN_EN    HSUSBD_DAINTMSK_InEpMsk3_Msk
#define HSUSBD_EP4_IN_EN    HSUSBD_DAINTMSK_InEpMsk4_Msk
#define HSUSBD_EP5_IN_EN    HSUSBD_DAINTMSK_InEpMsk5_Msk
#define HSUSBD_EP6_IN_EN    HSUSBD_DAINTMSK_InEpMsk6_Msk
#define HSUSBD_EP7_IN_EN    HSUSBD_DAINTMSK_InEpMsk7_Msk
#define HSUSBD_EP8_IN_EN    HSUSBD_DAINTMSK_InEpMsk8_Msk

#define HSUSBD_EP0_OUT_EN    HSUSBD_DAINTMSK_OutEPMsk0_Msk
#define HSUSBD_EP1_OUT_EN    HSUSBD_DAINTMSK_OutEPMsk1_Msk
#define HSUSBD_EP2_OUT_EN    HSUSBD_DAINTMSK_OutEPMsk2_Msk
#define HSUSBD_EP3_OUT_EN    HSUSBD_DAINTMSK_OutEPMsk3_Msk
#define HSUSBD_EP4_OUT_EN    HSUSBD_DAINTMSK_OutEPMsk4_Msk
#define HSUSBD_EP5_OUT_EN    HSUSBD_DAINTMSK_OutEPMsk5_Msk
#define HSUSBD_EP6_OUT_EN    HSUSBD_DAINTMSK_OutEPMsk6_Msk
#define HSUSBD_EP7_OUT_EN    HSUSBD_DAINTMSK_OutEPMsk7_Msk
#define HSUSBD_EP8_OUT_EN    HSUSBD_DAINTMSK_OutEPMsk8_Msk

/* Device Endpoint-N Control Register (DIEPCTLn/DOEPCTLn) */
#define HSUSBD_DIEPCTL_TX_FIFO_NUM(x)		(x << HSUSBD_DIEPCTL_TxFNum_Pos)

/* Reset Register (GRSTCTL) */
#define HSUSBD_GRSTCTL_TX_FIFO_NUM(x)		(x << HSUSBD_GRSTCTL_TxFNum_Pos)

/*!@}*/ /* end of group HSUSBD_EXPORTED_CONSTANTS */

/** @addtogroup HSUSBD_EXPORTED_STRUCT HSUSBD Exported Struct
  @{
*/

/**
 * struct dwc2_dma_desc - DMA descriptor structure
 *
 * @status: DMA descriptor status quadlet
 * @buf:    DMA descriptor data buffer pointer
 *
 * DMA Descriptor structure contains two quadlets:
 * Status quadlet and Data buffer pointer.
 */

typedef union HSUSBD_DMA_DESC_STS
{
	unsigned int d32;
	struct
	{
		unsigned bytes:16;		/*!<Received number of bytes */
		unsigned reserved16_22:7;
		unsigned mtrf:1;		/*!<Multiple Transfer - only for OUT EPs */
		unsigned sr:1;			/*!<Setup Packet received - only for OUT EPs */
		unsigned ioc:1;			/*!<Interrupt On Complete. */
		unsigned sp:1;			/*!<Short Packet */
		unsigned l:1;			/*!<Last */
		unsigned sts:2;			/*!<Receive Status */
		unsigned bs:2;			/*!<Buffer Status */
	} b;

} HSUSBD_DMA_DESC_STS_T;

typedef struct HSUSBD_DMA_DESC
{
	HSUSBD_DMA_DESC_STS_T status;
	uint32_t buf;

} S_HSUSBD_DMA_DESC_T;


/* control request */
typedef struct HSUSBD_CMD_STRUCT
{
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;

} S_HSUSBD_CMD_T; /*!<USB Setup Packet Structure */


typedef struct s_hsusbd_info
{
    uint8_t *gu8DevDesc;            /*!< Device descriptor */
    uint8_t *gu8ConfigDesc;         /*!< Config descriptor */
    uint8_t **gu8StringDesc;        /*!< Pointer for USB String Descriptor pointers */
    uint8_t *gu8QualDesc;           /*!< Qualifier descriptor */
    uint8_t *gu8FullConfigDesc;     /*!< Full Speed Config descriptor */
    uint8_t *gu8HSOtherConfigDesc;  /*!< Other Speed Config descriptor */
    uint8_t *gu8FSOtherConfigDesc;  /*!< Other Speed Config descriptor */
    uint8_t **gu8HidReportDesc;     /*!< Pointer for HID Report descriptor */
    uint32_t *gu32HidReportSize;    /*!< Pointer for HID Report descriptor Size */
    uint32_t *gu32ConfigHidDescIdx; /*!< Pointer for HID Descriptor start index */

} S_HSUSBD_INFO_T; /*!<USB Information Structure */


/*!@}*/ /* end of group HSUSBD_EXPORTED_STRUCT */

/// @cond HIDDEN_SYMBOLS
extern uint8_t volatile g_u8ResetAvailable;
extern uint8_t volatile g_hsusbd_Configured;
extern S_HSUSBD_INFO_T gsHSInfo;
extern E_EP0_STATE gEp0State;
extern S_HSUSBD_CMD_T gUsbCmd;
/// @endcond HIDDEN_SYMBOLS

/** @addtogroup HSUSBD_EXPORTED_FUNCTIONS HSUSBD Exported Functions
  @{
*/

#define HSUSBD_ENABLE_EP_INT(intr)        (HSUSBD->DAINTMSK = (intr)) /*!<Enable EP Interrupt  \hideinitializer */
#define HSUSBD_ENABLE_OUT_EP_INT(intr)    (HSUSBD->DOEPMSK = (intr)) /*!<Enable Out EP Interrupt  \hideinitializer */
#define HSUSBD_ENABLE_IN_EP_INT(intr)     (HSUSBD->DIEPMSK = (intr)) /*!<Enable In EP Interrupt  \hideinitializer */

#define HSUSBD_IS_ATTACHED()              ((uint32_t)(SYS->MISCISR & SYS_MISCISR_VBUSSTS_Msk)) /*!<Check cable connect state  \hideinitializer */
#define HSUSBD_SET_ADDR(addr)             (HSUSBD->DCFG = (HSUSBD->DCFG & ~(HSUSBD_DCFG_DevAddr_Msk)) | (addr << HSUSBD_DCFG_DevAddr_Pos)) /*!<Set USB address  \hideinitializer */
#define HSUSBD_GET_ADDR()                 ((uint32_t)((HSUSBD->DCFG & HSUSBD_DCFG_DevAddr_Msk) >> HSUSBD_DCFG_DevAddr_Pos) /*!<Get USB address  \hideinitializer */
#define HSUSBD_SET_SE0()                  ((uint32_t)(HSUSBD->DCTL |= HSUSBD_DCTL_SftDiscon_Msk)) /*!<Enable SE0, device in disconnected state  \hideinitializer */
#define HSUSBD_CLR_SE0()                  ((uint32_t)(HSUSBD->DCTL &= ~(HSUSBD_DCTL_SftDiscon_Msk))) /*!<Disable SE0, device connect to host  \hideinitializer */

#define HSUSBD_SET_DMA_BURST(burst)       (HSUSBD->GAHBCFG = (HSUSBD->GAHBCFG & ~(HSUSBD_GAHBCFG_HBstLen_Msk)) | burst) /*!<Set DMA burst length  \hideinitializer */


/**
  * @brief  HSUSBD_memcpy, Copy bytes hardware limitation
  * @param[in]  u8Dst   Destination pointer.
  * @param[in]  u8Src   Source pointer.
  * @param[in]  u32Size Copy size.
  */
__STATIC_INLINE void HSUSBD_MemCopy(uint8_t u8Dst[], uint8_t u8Src[], uint32_t u32Size)
{
    uint32_t i = 0ul;

    while (u32Size--)
    {
        u8Dst[i] = u8Src[i];
        i++;
    }
}

/**
  * @brief  HSUSBD_SetEp0MaxPktSize
  * @param[in]  size  EP0 Max Packet Size
  */
__STATIC_INLINE void HSUSBD_SetEp0MaxPktSize(uint32_t size)
{
	switch (size)
	{
	case 64:
		HSUSBD->IEP[0].DIEPCTL &= ~(HSUSBD_DIEPCTL0_MPS_Msk);
		HSUSBD->OEP[0].DOEPCTL &= ~(HSUSBD_DOEPCTL0_MPS_Msk);
		break;

	case 32:
		HSUSBD->IEP[0].DIEPCTL &= ~(HSUSBD_DIEPCTL0_MPS_Msk);
		HSUSBD->IEP[0].DIEPCTL |= 0x01;
		HSUSBD->OEP[0].DOEPCTL &= ~(HSUSBD_DOEPCTL0_MPS_Msk);
		HSUSBD->OEP[0].DOEPCTL |= 0x01;
		break;

	case 16:
		HSUSBD->IEP[0].DIEPCTL &= ~(HSUSBD_DIEPCTL0_MPS_Msk);
		HSUSBD->IEP[0].DIEPCTL |= 0x02;
		HSUSBD->OEP[0].DOEPCTL &= ~(HSUSBD_DOEPCTL0_MPS_Msk);
		HSUSBD->OEP[0].DOEPCTL |= 0x02;
		break;

	case 8:
		HSUSBD->IEP[0].DIEPCTL &= ~(HSUSBD_DIEPCTL0_MPS_Msk);
		HSUSBD->IEP[0].DIEPCTL |= 0x03;
		HSUSBD->OEP[0].DOEPCTL &= ~(HSUSBD_DOEPCTL0_MPS_Msk);
		HSUSBD->OEP[0].DOEPCTL |= 0x03;
		break;

	default:
		sysprintf("Invalid EP0 Max Packet Size!\n");
	}
}

/**
  * @brief  HSUSBD_SetEpMaxPktSize
  * @param[in]  epno  endpoint number
  * @param[in]  epdir EP_INPUT or EP_OUTPUT
  * @param[in]  size  Max Packet Size
  */
__STATIC_INLINE void HSUSBD_SetEpMaxPktSize(uint32_t epno, uint32_t epdir, uint32_t size)
{
	if (epdir == EP_INPUT)
	{
		HSUSBD->IEP[epno].DIEPCTL &= ~(HSUSBD_DIEPCTL_MPS_Msk);
		HSUSBD->IEP[epno].DIEPCTL |= size;
	}
	else
	{
		HSUSBD->OEP[epno].DOEPCTL &= ~(HSUSBD_DOEPCTL_MPS_Msk);
		HSUSBD->OEP[epno].DOEPCTL |= size;
	}
}


/*-------------------------------------------------------------------------------------------*/
typedef void (*HSUSBD_VENDOR_REQ)(void); /*!<USB Vendor request callback function */
typedef void (*HSUSBD_CLASS_REQ)(void); /*!<USB Class request callback function */
typedef void (*HSUSBD_SET_INTERFACE_REQ)(uint32_t u32AltInterface); /*!<USB Standard request "Set Interface" callback function */

void HSUSBD_Open(S_HSUSBD_INFO_T *param, HSUSBD_CLASS_REQ pfnClassReq, HSUSBD_SET_INTERFACE_REQ pfnSetInterface);
void HSUSBD_Start(void);
void HSUSBD_EP0_Configuration(void);
void HSUSBD_PreSetup(void);
void HSUSBD_EP0_CompleteTx(void);
void HSUSBD_EP0_CompleteRx(void);
void HSUSBD_EP0_SendZero(void);
void HSUSBD_SetStall(uint32_t u32EpNum);
void HSUSBD_ClearStall(uint32_t u32EpNum);
uint32_t HSUSBD_GetStall(uint32_t u32EpNum);
void HSUSBD_ProcessSetupPacket(void);
void HSUSBD_StandardRequest(void);
void HSUSBD_SetVendorRequest(HSUSBD_VENDOR_REQ pfnVendorReq);
void HSUSBD_StartDMA(uint32_t ep, S_HSUSBD_DMA_DESC_T *desc, uint8_t buf[], uint32_t len);
void HSUSBD_PrepareCtrlIn(uint8_t pu8Buf[], uint32_t u32Size);
void HSUSBD_CtrlOut(uint8_t pu8Buf[], uint32_t u32Size);


/*!@}*/ /* end of group HSUSBD_EXPORTED_FUNCTIONS */

/*!@}*/ /* end of group HSUSBD_Driver */

/*!@}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /*__HSUSBD_H__ */

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/

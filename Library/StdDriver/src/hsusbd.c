/**************************************************************************//**
 * @file     hsusbd.c
 *
 * @brief    HSUSBD driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup HSUSBD_Driver HSUSBD Driver
  @{
*/


/** @addtogroup HSUSBD_EXPORTED_FUNCTIONS HSUSBD Exported Functions
  @{
*/
/*--------------------------------------------------------------------------*/
/// @cond HIDDEN_SYMBOLS
/* Global variables for Control Pipe */
S_HSUSBD_CMD_T gUsbCmd;
S_HSUSBD_INFO_T *g_hsusbd_sInfo;

HSUSBD_VENDOR_REQ g_hsusbd_pfnVendorRequest = NULL;
HSUSBD_CLASS_REQ g_hsusbd_pfnClassRequest = NULL;
HSUSBD_SET_INTERFACE_REQ g_hsusbd_pfnSetInterface = NULL;

static uint32_t g_hsusbd_UsbConfig = 0ul;
static uint32_t g_hsusbd_UsbAltInterface = 0ul;

static uint8_t g_hsusbd_EnableTestMode = 0ul;
static uint8_t g_hsusbd_TestSelector = 0ul;
uint8_t volatile g_hsusbd_RemoteWakeupEn = 0ul; /*!< Remote wake up function enable flag */
uint8_t volatile g_u8ResetAvailable = 1ul;	 /*!< Reset available flag */
uint8_t volatile g_hsusbd_Configured = 0ul;

#ifdef __ICCARM__
#pragma data_alignment=4
static uint8_t g_hsusbd_buf[4];
S_HSUSBD_DMA_DESC_T ddma_ep0_in[3];
S_HSUSBD_DMA_DESC_T ddma_ep0_out[3];
uint8_t ep0_buf_in[3][64];
uint8_t ep0_buf_out[3][64];
#else
static uint8_t g_hsusbd_buf[4] __attribute__((aligned(4)));
S_HSUSBD_DMA_DESC_T ddma_ep0_in[3];
S_HSUSBD_DMA_DESC_T ddma_ep0_out[3];
uint8_t ep0_buf_in[3][64] __attribute__((aligned(4)));
uint8_t ep0_buf_out[3][64] __attribute__((aligned(4)));
#endif

E_EP0_STATE gEp0State;

extern void Delay0(uint32_t ticks);

/// @endcond HIDDEN_SYMBOLS

/* HSUSBD Low Level Initial */
static void HSUSBD_LL_Init(void)
{
	int volatile i;
	uint32_t dflt_gusbcfg;

	/* Soft-reset OTG Core */
	HSUSBD->GRSTCTL = HSUSBD_GRSTCTL_CSftRst_Msk;
	dflt_gusbcfg =
		0 << 15		/* PHY Low Power Clock select */
		| 0x9 << 10	/* Turn around time */
		| 0x7 << 0;	/* HS/FS Timeout */
	HSUSBD->GUSBCFG = dflt_gusbcfg;

	/* Put the OTG device core in the disconnected state */
	HSUSBD_SET_SE0();
	Delay0(20);

	/* Unmask the core interrupts */
	HSUSBD->GINTMSK = (HSUSBD_GINTMSK_OEPIntMsk_Msk | HSUSBD_GINTMSK_IEPIntMsk_Msk | HSUSBD_GINTMSK_WkUpIntMsk_Msk |
			HSUSBD_GINTMSK_USBSuspMsk_Msk | HSUSBD_GINTMSK_USBRstMsk_Msk | HSUSBD_GINTMSK_EnumDoneMsk_Msk);

	/* Set NAK bit of EP0 */
	HSUSBD->OEP[0].DOEPCTL = (HSUSBD_DOEPCTL_EPDis_Msk | HSUSBD_DOEPCTL_SNAK_Msk);

	HSUSBD->IEP[0].DIEPCTL = (HSUSBD_DIEPCTL_EPDis_Msk | HSUSBD_DIEPCTL_SNAK_Msk);
	HSUSBD->IEP[0].DIEPCTL &= ~(HSUSBD_DIEPCTL_TxFNum_Msk);
	HSUSBD->IEP[0].DIEPCTL |= HSUSBD_DIEPCTL_TX_FIFO_NUM(0);

	/* Set RX FIFO Size (in 32-bit words) */
	HSUSBD->GRXFSIZ = 0x100;

	/* Set Non Periodic TX FIFO Size */
	HSUSBD->GNPTXFSIZ = 0x00200100;

	/* retrieve the number of IN Endpoints (excluding ep0) */
	HSUSBD->DIEPTXF1 = 0x01000120;
	HSUSBD->DIEPTXF2 = 0x01000220;
	HSUSBD->DIEPTXF3 = 0x01000320;
	HSUSBD->DIEPTXF4 = 0x01000420;
	HSUSBD->DIEPTXF5 = 0x01000520;
	HSUSBD->DIEPTXF6 = 0x01000620;
	HSUSBD->DIEPTXF7 = 0x01000720;
	HSUSBD->DIEPTXF8 = 0x01000820;

	/* Flush the RX FIFO */
	HSUSBD->GRSTCTL = HSUSBD_GRSTCTL_RxFFlsh_Msk;
	while (HSUSBD->GRSTCTL & HSUSBD_GRSTCTL_RxFFlsh_Msk) {}

	/* Flush all the TX FIFO's */
	HSUSBD->GRSTCTL = HSUSBD_GRSTCTL_TX_FIFO_NUM(0x10);
	HSUSBD->GRSTCTL = (HSUSBD_GRSTCTL_TX_FIFO_NUM(0x10) | HSUSBD_GRSTCTL_TxFFlsh_Msk);
	while (HSUSBD->GRSTCTL & HSUSBD_GRSTCTL_TxFFlsh_Msk) {}

	/* clear global NAKs */
	HSUSBD->DCTL |= (HSUSBD_DCTL_CGNPInNak_Msk | HSUSBD_DCTL_CGOUTNak_Msk);

	/* enable DDMA */
	HSUSBD->DCFG = HSUSBD_DCFG_DescDMA_Msk;
	HSUSBD->GAHBCFG = (HSUSBD_GAHBCFG_DMAEn_Msk | HSUSBD_GAHBCFG_GlblIntrMsk_Msk | HSUSBD_GAHBCFG_BURST_INCR4);
}

/**
 * @brief       HSUSBD Initial
 *
 * @param[in]   param               Descriptor
 * @param[in]   pfnClassReq         Class Request Callback Function
 * @param[in]   pfnSetInterface     SetInterface Request Callback Function
 *
 * @details     This function is used to initial HSUSBD.
 */
void HSUSBD_Open(S_HSUSBD_INFO_T *param, HSUSBD_CLASS_REQ pfnClassReq, HSUSBD_SET_INTERFACE_REQ pfnSetInterface)
{
    g_hsusbd_sInfo = param;
    g_hsusbd_pfnClassRequest = pfnClassReq;
    g_hsusbd_pfnSetInterface = pfnSetInterface;

    /* Initial USB PHY */
	SYS->USBPMISCR &= ~(SYS_USBPMISCR_PHY0POR_Msk | SYS_USBPMISCR_PHY0SUSPEND_Msk | SYS_USBPMISCR_PHY0COMN_Msk);
	SYS->USBPMISCR |= SYS_USBPMISCR_PHY0SUSPEND_Msk;
    /* wait PHY clock ready */
	while((SYS->USBPMISCR & SYS_USBPMISCR_PHY0DEVCKSTB_Msk) != SYS_USBPMISCR_PHY0DEVCKSTB_Msk);

	HSUSBD_LL_Init();
}

/**
 * @brief       HSUSBD Start
 *
 * @details     This function is used to start transfer
 */
void HSUSBD_Start(void)
{
    HSUSBD_CLR_SE0();
}


/**
 * @brief       HSUSBD EP0 Configuration
 *
 * @details     This function is used to configure EP0
 */
void HSUSBD_EP0_Configuration(void)
{
    HSUSBD->OEP[0].DOEPCTL = (HSUSBD_DOEPCTL_EPDis_Msk | HSUSBD_DOEPCTL_CNAK_Msk | HSUSBD_DEPCTL_TYPE_CTRL);

    dcache_clean_invalidate_by_mva(ddma_ep0_in, sizeof(ddma_ep0_in));
    dcache_clean_invalidate_by_mva(ddma_ep0_out, sizeof(ddma_ep0_out));
    ddma_ep0_in[0].buf = nc_addr64(ep0_buf_in[0]);
	ddma_ep0_in[1].buf = nc_addr64(ep0_buf_in[1]);
	ddma_ep0_out[0].buf = nc_addr64(ep0_buf_out[0]);
	ddma_ep0_out[1].buf = nc_addr64(ep0_buf_out[1]);
	dcache_clean_invalidate_by_mva(ddma_ep0_in, sizeof(ddma_ep0_in));
	dcache_clean_invalidate_by_mva(ddma_ep0_out, sizeof(ddma_ep0_out));
}

/**
 * @brief       Process Setup Packet
 *
 * @details     This function is used to process Setup packet.
 */
void HSUSBD_ProcessSetupPacket(void)
{
	int i;
	uint8_t *ptr = (uint8_t *)&gUsbCmd;

	for (i=0; i<8; i++)
		*(ptr+i) = *(uint8_t *)(nc_ptr(ddma_ep0_out[0].buf)+i);

	sysprintf("bmRequestType = 0x%x(%s), bRequest = 0x%x, wLength = 0x%x, wValue = 0x%x, wIndex= 0x%x\n",
			gUsbCmd.bmRequestType, (gUsbCmd.bmRequestType & 0x80) ? "IN" : "OUT", gUsbCmd.bRequest,
					gUsbCmd.wLength, gUsbCmd.wValue, gUsbCmd.wIndex);

    /* USB device request in setup packet: offset 0, D[6..5]: 0=Standard, 1=Class, 2=Vendor, 3=Reserved */
    switch (gUsbCmd.bmRequestType & 0x60ul)
    {
    case REQ_STANDARD:
    {
        HSUSBD_StandardRequest();
        break;
    }
    case REQ_CLASS:
    {
        if (g_hsusbd_pfnClassRequest != NULL)
        {
            g_hsusbd_pfnClassRequest();
        }
        break;
    }
    case REQ_VENDOR:
    {
        if (g_hsusbd_pfnVendorRequest != NULL)
        {
            g_hsusbd_pfnVendorRequest();
        }
        break;
    }
    default:
    {
        /* Setup error, stall the device */
    	HSUSBD_SetStall(0);
        break;
    }
    }
}

/**
 * @brief       Get Descriptor request
 *
 * @details     This function is used to process GetDescriptor request.
 */
void HSUSBD_GetDescriptor(void)
{
    uint32_t u32Len;
    int val = 0;

    u32Len = gUsbCmd.wLength;

    switch ((gUsbCmd.wValue & 0xff00ul) >> 8)
    {
    /* Get Device Descriptor */
    case DESC_DEVICE:
    {
        u32Len = Minimum(u32Len, LEN_DEVICE);
        HSUSBD_PrepareCtrlIn((uint8_t *)g_hsusbd_sInfo->gu8DevDesc, u32Len);
        break;
    }
    /* Get Configuration Descriptor */
    case DESC_CONFIG:
    {
        uint32_t u32TotalLen;
		if ((HSUSBD->DSTS & HSUSBD_DSTS_EnumSpd_Msk) == 0)
        {
            u32TotalLen = g_hsusbd_sInfo->gu8ConfigDesc[3];
            u32TotalLen = g_hsusbd_sInfo->gu8ConfigDesc[2] + (u32TotalLen << 8);
            u32Len = Minimum(u32Len, u32TotalLen);

            HSUSBD_PrepareCtrlIn((uint8_t *)g_hsusbd_sInfo->gu8ConfigDesc, u32Len);
        }
        else
        {
            u32TotalLen = g_hsusbd_sInfo->gu8FullConfigDesc[3];
            u32TotalLen = g_hsusbd_sInfo->gu8FullConfigDesc[2] + (u32TotalLen << 8);
            u32Len = Minimum(u32Len, u32TotalLen);

            HSUSBD_PrepareCtrlIn((uint8_t *)g_hsusbd_sInfo->gu8FullConfigDesc, u32Len);
        }

        break;
    }
    /* Get Qualifier Descriptor */
    case DESC_QUALIFIER:
    {
        u32Len = Minimum(u32Len, LEN_QUALIFIER);
        HSUSBD_PrepareCtrlIn((uint8_t *)g_hsusbd_sInfo->gu8QualDesc, u32Len);
        break;
    }
    /* Get Other Speed Descriptor - Full speed */
    case DESC_OTHERSPEED:
    {
        uint32_t u32TotalLen;
		if ((HSUSBD->DSTS & HSUSBD_DSTS_EnumSpd_Msk) == 0)
        {
            u32TotalLen = g_hsusbd_sInfo->gu8HSOtherConfigDesc[3];
            u32TotalLen = g_hsusbd_sInfo->gu8HSOtherConfigDesc[2] + (u32TotalLen << 8);
            u32Len = Minimum(u32Len, u32TotalLen);

            HSUSBD_PrepareCtrlIn((uint8_t *)g_hsusbd_sInfo->gu8HSOtherConfigDesc, u32Len);
        }
        else
        {
            u32TotalLen = g_hsusbd_sInfo->gu8FSOtherConfigDesc[3];
            u32TotalLen = g_hsusbd_sInfo->gu8FSOtherConfigDesc[2] + (u32TotalLen << 8);
            u32Len = Minimum(u32Len, u32TotalLen);

            HSUSBD_PrepareCtrlIn((uint8_t *)g_hsusbd_sInfo->gu8FSOtherConfigDesc, u32Len);
        }

        break;
    }
    /* Get HID Descriptor */
    case DESC_HID:
    {
        uint32_t u32ConfigDescOffset;   /* u32ConfigDescOffset is configuration descriptor offset (HID descriptor start index) */
        u32Len = Minimum(u32Len, LEN_HID);
        u32ConfigDescOffset = g_hsusbd_sInfo->gu32ConfigHidDescIdx[gUsbCmd.wIndex & 0xfful];
        HSUSBD_PrepareCtrlIn((uint8_t *)&g_hsusbd_sInfo->gu8ConfigDesc[u32ConfigDescOffset], u32Len);
        break;
    }
    /* Get Report Descriptor */
    case DESC_HID_RPT:
    {
        u32Len = Minimum(u32Len, g_hsusbd_sInfo->gu32HidReportSize[gUsbCmd.wIndex & 0xfful]);
        HSUSBD_PrepareCtrlIn((uint8_t *)g_hsusbd_sInfo->gu8HidReportDesc[gUsbCmd.wIndex & 0xfful], u32Len);
        break;
    }
    /* Get String Descriptor */
    case DESC_STRING:
    {
        if ((gUsbCmd.wValue & 0xfful) < 8ul)
        {
            u32Len = Minimum(u32Len, g_hsusbd_sInfo->gu8StringDesc[gUsbCmd.wValue & 0xfful][0]);
            HSUSBD_PrepareCtrlIn((uint8_t *)g_hsusbd_sInfo->gu8StringDesc[gUsbCmd.wValue & 0xfful], u32Len);
        }
        else
        {
        	HSUSBD_SetStall(EP0);
        }
        break;
    }
    default:
        /* Not support. Reply STALL. */
    	HSUSBD_SetStall(EP0);
    }
}


/**
 * @brief       Process USB standard request
 *
 * @details     This function is used to process USB Standard Request.
 */
void HSUSBD_StandardRequest(void)
{
	uint8_t *ptr;

	if ((gUsbCmd.bmRequestType & 0x80ul) == 0x80ul)   /* request data transfer direction */
    {
        /* Device to host */
        switch (gUsbCmd.bRequest)
        {
        case GET_CONFIGURATION:
        {
            /* Return current configuration setting */
            HSUSBD_PrepareCtrlIn((uint8_t *)&g_hsusbd_UsbConfig, 1ul);
            break;
        }
        case GET_DESCRIPTOR:
        {
            HSUSBD_GetDescriptor();
            break;
        }
        case GET_INTERFACE:
        {
            /* Return current interface setting */
            HSUSBD_PrepareCtrlIn((uint8_t *)&g_hsusbd_UsbAltInterface, 1ul);
            break;
        }
        case GET_STATUS:
        {
            /* Device */
            if (gUsbCmd.bmRequestType == 0x80ul)
            {
                uint8_t u8Tmp;

                u8Tmp = (uint8_t)0ul;
                if ((g_hsusbd_sInfo->gu8ConfigDesc[7] & 0x40ul) == 0x40ul) {
                    u8Tmp |= (uint8_t)1ul; /* Self-Powered/Bus-Powered.*/
                }
                if ((g_hsusbd_sInfo->gu8ConfigDesc[7] & 0x20ul) == 0x20ul) {
                    u8Tmp |= (uint8_t)(g_hsusbd_RemoteWakeupEn << 1ul); /* Remote wake up */
                }
                g_hsusbd_buf[0] = u8Tmp;
            }
            /* Interface */
            else if (gUsbCmd.bmRequestType == 0x81ul)
            {
                g_hsusbd_buf[0] = (uint8_t)0ul;
            }
            /* Endpoint */
            else if (gUsbCmd.bmRequestType == 0x82ul)
            {
                uint8_t ep = (uint8_t)(gUsbCmd.wIndex & 0xFul);
                g_hsusbd_buf[0] = (uint8_t)HSUSBD_GetStall((uint32_t)gUsbCmd.wIndex);
            }
            g_hsusbd_buf[1] = (uint8_t)0ul;
            HSUSBD_PrepareCtrlIn(g_hsusbd_buf, 2ul);
            break;
        }
        default:
        {
            /* Setup error, stall the device */
        	HSUSBD_SetStall(EP0);
        }
        }
    }
    else
    {
        /* Host to device */
        switch (gUsbCmd.bRequest)
        {
        case CLEAR_FEATURE:
        {
            if ((gUsbCmd.wValue & 0xfful) == FEATURE_ENDPOINT_HALT)
            {
                uint32_t epNum;

                epNum = (uint32_t)(gUsbCmd.wIndex & 0xFul);
                if (epNum == 0)
                {
                	HSUSBD_SetStall(gUsbCmd.wIndex);
                	return;
                }
                HSUSBD_ClearStall(gUsbCmd.wIndex);
            }
            else if ((gUsbCmd.wValue & 0xfful) == FEATURE_DEVICE_REMOTE_WAKEUP)
            {
                g_hsusbd_RemoteWakeupEn = (uint8_t)0;
            }
        	HSUSBD_EP0_SendZero();
            break;
        }
        case SET_ADDRESS:
        {
        	if (gUsbCmd.bmRequestType != 0)	/* standard request, RECIP: device */
        		break;
        	HSUSBD_SET_ADDR(gUsbCmd.wValue);
        	HSUSBD_EP0_SendZero();
            break;
        }
        case SET_CONFIGURATION:
        {
        	ptr = (uint8_t *)((uint64_t)(&g_hsusbd_UsbConfig)|NON_CACHE);
        	if (gUsbCmd.bmRequestType == 0)	/* RECIP: device */
        		g_u8ResetAvailable = 1;
        	*ptr = g_hsusbd_UsbConfig = (uint8_t)gUsbCmd.wValue;
            g_hsusbd_Configured = (uint8_t)1ul;
        	HSUSBD_EP0_SendZero();
            break;
        }
        case SET_FEATURE:
        {
            if (gUsbCmd.wValue == FEATURE_ENDPOINT_HALT)
            {
                uint32_t idx;
                idx = (uint32_t)(gUsbCmd.wIndex & 0xFul);
                HSUSBD_SetStall(gUsbCmd.wIndex);
                if (idx == 0)
                	return;
            }
            if ((gUsbCmd.wValue & 0x3ul) == 2ul)    /* TEST_MODE */
            {
                g_hsusbd_EnableTestMode = (uint8_t)1ul;
                g_hsusbd_TestSelector = (uint8_t)(gUsbCmd.wIndex >> 8);
            }
            if ((gUsbCmd.wValue & FEATURE_DEVICE_REMOTE_WAKEUP) == FEATURE_DEVICE_REMOTE_WAKEUP)
            {
                g_hsusbd_RemoteWakeupEn = (uint8_t)1ul;
            }
        	HSUSBD_EP0_SendZero();
            break;
        }
        case SET_INTERFACE:
        {
        	ptr = (uint8_t *)((uint64_t)(&g_hsusbd_UsbAltInterface)|NON_CACHE);

        	if (gUsbCmd.bmRequestType == 0x1)	/* RECIP: interface */
        		g_u8ResetAvailable = 1;

        	*ptr = g_hsusbd_UsbAltInterface = (uint8_t)gUsbCmd.wValue;
            if (g_hsusbd_pfnSetInterface != NULL)
            {
                g_hsusbd_pfnSetInterface((uint32_t)g_hsusbd_UsbAltInterface);
            }
        	HSUSBD_EP0_SendZero();
            break;
        }
        default:
        {
            /* Setup error, stall the device */
        	HSUSBD_SetStall(EP0);
        }
        }
    }
}


/**
 * @brief       Prepare Setup descriptor
 *
 * @details     This function is used to prepare Setup descriptor
 */
void HSUSBD_PreSetup(void)
{
    dcache_clean_invalidate_by_mva(ddma_ep0_out, sizeof(ddma_ep0_out));
	ddma_ep0_out[0].buf = nc_addr64(&ep0_buf_out[0]);
	ddma_ep0_out[0].status.d32 = 0;
	ddma_ep0_out[0].status.b.bytes = 8;
	ddma_ep0_out[0].status.b.bs = 0;
	ddma_ep0_out[0].status.b.l = 1;
	ddma_ep0_out[0].status.b.ioc = 1;
    dcache_clean_invalidate_by_mva(ddma_ep0_out, sizeof(ddma_ep0_out));
	HSUSBD->OEP[EP0].DOEPDMA = (uint64_t)&ddma_ep0_out[0];
	HSUSBD->OEP[EP0].DOEPCTL |= (HSUSBD_DOEPCTL_EPEna_Msk | HSUSBD_DOEPCTL_CNAK_Msk);
}


/**
 * @brief       Prepare Complete Out Packet
 *
 * @details     This function is used to prepare complete out packet
 */
void HSUSBD_EP0_CompleteOut(void)
{
    dcache_clean_invalidate_by_mva(ddma_ep0_out, sizeof(ddma_ep0_out));
	ddma_ep0_out[1].buf = nc_addr64(&ep0_buf_out[1]);
	ddma_ep0_out[1].status.d32 = 0x0a000000;
    dcache_clean_invalidate_by_mva(ddma_ep0_out, sizeof(ddma_ep0_out));
	HSUSBD->OEP[EP0].DOEPDMA = (uint64_t)&ddma_ep0_out[1];
	HSUSBD->OEP[EP0].DOEPCTL |= (HSUSBD_DOEPCTL_EPEna_Msk | HSUSBD_DOEPCTL_CNAK_Msk);
}



/**
 * @brief       EP0 Complete TX
 *
 * @details     This function is used to process complete TX state
 */
void HSUSBD_EP0_CompleteTx(void)
{
	if (gEp0State == WAIT_FOR_NULL_COMPLETE)
	{
		gEp0State = WAIT_FOR_OUT_COMPLETE;
		HSUSBD_EP0_CompleteOut();
		return;
	}

	if (gEp0State == DATA_STATE_XMIT)
	{
		/* Flush the endpoint's TX FIFO */
		HSUSBD->GRSTCTL = HSUSBD_GRSTCTL_TX_FIFO_NUM(EP0);
		HSUSBD->GRSTCTL = (HSUSBD_GRSTCTL_TX_FIFO_NUM(EP0) | HSUSBD_GRSTCTL_TxFFlsh_Msk);
		while (HSUSBD->GRSTCTL & HSUSBD_GRSTCTL_TxFFlsh_Msk) {}

		/* Write the FIFO number to be used for this endpoint */
		HSUSBD->IEP[EP0].DIEPCTL &= ~(HSUSBD_DIEPCTL_TxFNum_Msk);
		HSUSBD->IEP[EP0].DIEPCTL |= HSUSBD_DIEPCTL_TX_FIFO_NUM(EP0);

		/* Clear reserved (Next EP) bits */
		HSUSBD->IEP[EP0].DIEPCTL &= ~(0xf << 11);
		HSUSBD->IEP[EP0].DIEPCTL |= (HSUSBD_DIEPCTL_EPEna_Msk | HSUSBD_DIEPCTL_CNAK_Msk);

		gEp0State = WAIT_FOR_COMPLETE;
	}
	else if (gEp0State == WAIT_FOR_IN_COMPLETE)
	{
		/* test mode */
		if (g_hsusbd_EnableTestMode)
		{
			HSUSBD->DCTL &= ~(HSUSBD_DCTL_TstCtl_Msk);
			HSUSBD->DCTL |= (g_hsusbd_TestSelector << HSUSBD_DCTL_TstCtl_Pos);
		}
		gEp0State = WAIT_FOR_SETUP;
	}
	else if (gEp0State == WAIT_FOR_COMPLETE)
	{
		gEp0State = WAIT_FOR_OUT_COMPLETE;
		HSUSBD_EP0_CompleteOut();
	}
}


/**
 * @brief       EP0 Send zero packet
 *
 * @details     This function is used to send zero length packet
 */
void HSUSBD_EP0_SendZero(void)
{
	/* prepare zero packet */
    dcache_clean_invalidate_by_mva(ddma_ep0_in, sizeof(ddma_ep0_in));
	ddma_ep0_in[1].buf = nc_addr64(ep0_buf_in);
	ddma_ep0_in[1].status.d32 = 0x0a000000;
    dcache_clean_invalidate_by_mva(ddma_ep0_in, sizeof(ddma_ep0_in));
	HSUSBD->IEP[EP0].DIEPDMA = (uint64_t)&ddma_ep0_in[1];
	/* Clear reserved (Next EP) bits */
	HSUSBD->IEP[EP0].DIEPCTL &= ~(0xf << 11);	/* NOTE: can't remove this line */
	HSUSBD->IEP[EP0].DIEPCTL |= (HSUSBD_DIEPCTL_EPEna_Msk | HSUSBD_DIEPCTL_CNAK_Msk);
	gEp0State = WAIT_FOR_IN_COMPLETE;
}


/**
 * @brief       EP0 Complete RX
 *
 * @details     This function is used to process complete RX state
 */
void HSUSBD_EP0_CompleteRx(void)
{
	if (gEp0State == DATA_STATE_RECV)
	{
		HSUSBD_EP0_SendZero();
	}
}


/**
 * @brief       Set USB endpoint stall state
 *
 * @param[in]   u32Value         USB command index
 *
 * @details     Set USB endpoint stall state, endpoint will return STALL token.
 */
void HSUSBD_SetStall(uint32_t u32Value)
{
	uint32_t u32EpNum = u32Value & 0xf;

    if ((u32Value & 0xf0) == 0x80)
    {
		/* set the disable and stall bits */
		if (HSUSBD->IEP[u32EpNum].DIEPCTL & HSUSBD_DIEPCTL_EPEna_Msk)
			HSUSBD->IEP[u32EpNum].DIEPCTL |= HSUSBD_DIEPCTL_EPDis_Msk;

		HSUSBD->IEP[u32EpNum].DIEPCTL |= HSUSBD_DIEPCTL_Stall_Msk;
    }
    else if (u32Value == 0x00)
    {
		HSUSBD->IEP[u32EpNum].DIEPCTL |= HSUSBD_DIEPCTL_Stall_Msk;
    	HSUSBD->OEP[u32EpNum].DOEPCTL |= HSUSBD_DOEPCTL_Stall_Msk;
    }
    else
    	HSUSBD->OEP[u32EpNum].DOEPCTL |= HSUSBD_DOEPCTL_Stall_Msk;

    if (u32EpNum == 0ul)
    {
    	/*
    	 * The application can only set this bit, and the core clears it,
    	 * when a SETUP token is received for this endpoint
    	 */
    	gEp0State = WAIT_FOR_SETUP;
    	HSUSBD_PreSetup();
    }
}


/**
 * @brief       Clear USB endpoint stall state
 *
 * @param[in]   u32Value         USB command index
 *
 * @details     Clear USB endpoint stall state, endpoint will return ACK/NAK token.
 */
void HSUSBD_ClearStall(uint32_t u32Value)
{
	uint32_t u32EpNum = u32Value & 0xf;

    if ((u32Value & 0xf0) == 0x80)
    	HSUSBD->IEP[u32EpNum].DIEPCTL = ((HSUSBD->IEP[u32EpNum].DIEPCTL & ~(HSUSBD_DIEPCTL_Stall_Msk)) | HSUSBD_DEPCTL_SETD0PID);
    else
    	HSUSBD->OEP[u32EpNum].DOEPCTL = ((HSUSBD->OEP[u32EpNum].DOEPCTL & ~(HSUSBD_DOEPCTL_Stall_Msk)) | HSUSBD_DEPCTL_SETD0PID);
}


/**
 * @brief       Get USB endpoint stall state
 *
 * @param[in]   u32Value         USB command index
 * @retval      0: USB endpoint is not stalled.
 * @retval      1: USB endpoint is stalled.
 *
 * @details     Get USB endpoint stall state.
 */
uint32_t HSUSBD_GetStall(uint32_t u32Value)
{
	uint32_t u32EpNum = u32Value & 0xf;
	uint32_t val = 0;

    if ((u32Value & 0xf0) == 0x80)
    	val = (HSUSBD->IEP[u32EpNum].DIEPCTL & HSUSBD_DIEPCTL_Stall_Msk)? 1 : 0;
    else
    	val = (HSUSBD->OEP[u32EpNum].DOEPCTL & HSUSBD_DOEPCTL_Stall_Msk)? 1 : 0;

	return val;
}


/**
 * @brief       Prepare Control IN transaction
 *
 * @param[in]   pu8Buf      Control IN data pointer
 * @param[in]   u32Size     IN transfer size
 *
 * @details     This function is used to prepare Control IN transfer
 */
void HSUSBD_PrepareCtrlIn(uint8_t pu8Buf[], uint32_t u32Size)
{
    dcache_clean_invalidate_by_mva(ddma_ep0_in, sizeof(ddma_ep0_in));
	ddma_ep0_in[0].buf = nc_addr64(pu8Buf);
	ddma_ep0_in[0].status.d32 = 0x0a000000 | u32Size;
    dcache_clean_invalidate_by_mva(ddma_ep0_in, sizeof(ddma_ep0_in));

	gEp0State = WAIT_FOR_COMPLETE;

	HSUSBD->IEP[EP0].DIEPDMA = (uint64_t)&ddma_ep0_in[0];
	HSUSBD->IEP[EP0].DIEPCTL |= (HSUSBD_DIEPCTL_EPEna_Msk | HSUSBD_DIEPCTL_CNAK_Msk);
}

/**
 * @brief       Control OUT transaction
 *
 * @param[in]   pu8Buf      Control IN data pointer
 * @param[in]   u32Size     IN transfer size
 *
 * @details     This function is used to prepare Control IN transfer
 */
void HSUSBD_CtrlOut(uint8_t pu8Buf[], uint32_t u32Size)
{
    dcache_clean_invalidate_by_mva(ddma_ep0_out, sizeof(ddma_ep0_out));
    ddma_ep0_out[1].buf = nc_addr64(pu8Buf);
    ddma_ep0_out[1].status.d32 = 0x0a000000 | u32Size;
    dcache_clean_invalidate_by_mva(ddma_ep0_out, sizeof(ddma_ep0_out));

    gEp0State = WAIT_FOR_COMPLETE;

    HSUSBD->OEP[EP0].DOEPDMA = (uint64_t)&ddma_ep0_out[1];
	HSUSBD->OEP[EP0].DOEPCTL |= (HSUSBD_DOEPCTL_EPEna_Msk | HSUSBD_DOEPCTL_CNAK_Msk);
}




/**
 * @brief       HSUSBD Set Vendor Request
 *
 * @param[in]   pfnVendorReq         Vendor Request Callback Function
 *
 * @details     This function is used to set HSUSBD vendor request callback function
 */
void HSUSBD_SetVendorRequest(HSUSBD_VENDOR_REQ pfnVendorReq)
{
    g_hsusbd_pfnVendorRequest = pfnVendorReq;
}


/**
 * @brief       HSUSBD Start next DMA descriptor
 *
 * @param[in]   ep        endpoint number
 * @param[in]   desc      dma descriptor
 * @param[in]   buf       transfer buffer
 * @param[in]   len       transfer length
 *
 * @details     This function is used to set HSUSBD vendor request callback function
 */
void HSUSBD_StartDMA(uint32_t ep, S_HSUSBD_DMA_DESC_T *desc, uint8_t buf[], uint32_t len)
{
    dcache_clean_invalidate_by_mva(desc, sizeof(S_HSUSBD_DMA_DESC_T));
	desc->buf = nc_addr64(buf);
	desc->status.d32 = 0x0a000000 | len;
    dcache_clean_invalidate_by_mva(desc, sizeof(S_HSUSBD_DMA_DESC_T));
	HSUSBD->IEP[ep].DIEPDMA = (uint64_t)desc;
	HSUSBD->IEP[ep].DIEPCTL |= (HSUSBD_DIEPCTL_EPEna_Msk | HSUSBD_DIEPCTL_CNAK_Msk);
}

/*!@}*/ /* end of group HSUSBD_EXPORTED_FUNCTIONS */

/*!@}*/ /* end of group HSUSBD_Driver */

/*!@}*/ /* end of group Standard_Driver */

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/

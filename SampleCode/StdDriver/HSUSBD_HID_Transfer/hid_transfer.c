/**************************************************************************//**
 * @file     hid_transfer.c
 * @version  V1.00
 * @brief    HSUSBD HID class Sample file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

/*!<Includes */
#include <string.h>
#include "NuMicro.h"
#include "hid_transfer.h"

/******************************************************************************/
/* Global parameters                                                          */
/******************************************************************************/
static uint32_t volatile g_u8ReportProtocol = HID_REPORT_PROTOCOL;

S_HSUSBD_DMA_DESC_T ddma_ep1_in[2];
uint8_t ep1_buf_in[512];
S_HSUSBD_DMA_DESC_T ddma_ep1_out[2];
uint8_t ep1_buf_out[512];

uint32_t g_u32Ep1MaxPacketSize;

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/
void HSUSBD_IRQHandler(void)
{
	__IO uint32_t intr_status, gotgint;
	__IO uint32_t usb_status, gintmsk;
	__IO uint32_t ep_intr, ep_intr_status;

	intr_status = HSUSBD->GINTSTS;
	gintmsk = HSUSBD->GINTMSK;

	if (!(intr_status & gintmsk))
	{
		return;
	}

	if (intr_status & HSUSBD_GINTSTS_EnumDone_Msk)
	{
		//sysprintf("Speed Detection interrupt\n");

		HSUSBD->GINTSTS = HSUSBD_GINTSTS_EnumDone_Msk;
		if ((HSUSBD->DSTS & HSUSBD_DSTS_EnumSpd_Msk) == 0)
		{
			sysprintf("High Speed Detection: 0x%x\n", HSUSBD->DSTS & HSUSBD_DSTS_EnumSpd_Msk);
		    HSUSBD_SetEp0MaxPktSize(EP0_MAX_PKT_SIZE);
			HSUSBD_SetEpMaxPktSize(EP1, EP_INPUT, EP1_MAX_PKT_SIZE);
		}
		else
		{
			sysprintf("Full Speed Detection: 0x%x\n", HSUSBD->DSTS & HSUSBD_DSTS_EnumSpd_Msk);
		    HSUSBD_SetEp0MaxPktSize(EP0_OTHER_MAX_PKT_SIZE);
			HSUSBD_SetEpMaxPktSize(EP1, EP_INPUT, EP1_OTHER_MAX_PKT_SIZE);
		}
	}

	if (intr_status & HSUSBD_GINTSTS_ErlySusp_Msk)
	{
		//sysprintf("Early suspend interrupt\n");
		HSUSBD->GINTSTS = HSUSBD_GINTSTS_ErlySusp_Msk;
	}

	if (intr_status & HSUSBD_GINTSTS_USBSusp_Msk)
	{
		//sysprintf("Suspend interrupt: (DSTS): 0x%x\n", HSUSBD->DSTS);
		HSUSBD->GINTSTS = HSUSBD_GINTSTS_USBSusp_Msk;
	}

	if (intr_status & HSUSBD_GINTSTS_WkUpInt_Msk)
	{
		//sysprintf("Resume interrupt\n");
		HSUSBD->GINTSTS = HSUSBD_GINTSTS_WkUpInt_Msk;
	}

	if (intr_status & HSUSBD_GINTSTS_USBRst_Msk)
	{
		sysprintf("\n\nReset interrupt - (SYS_MISCISR):0x%x\n", SYS->MISCISR);
		HSUSBD->GINTSTS = HSUSBD_GINTSTS_USBRst_Msk;

		/************************************/
		/* check GMISC VBUS detect */
		if (SYS->MISCISR & SYS_MISCISR_VBUSSTS_Msk)
		{
			if (g_u8ResetAvailable)
			{
				sysprintf("got reset (%d)!!\n",g_u8ResetAvailable);
	            g_hsusbd_Configured = 0;
				HSUSBD_SET_ADDR(0);
				gEp0State = WAIT_FOR_SETUP;
				g_u8ResetAvailable = 0;
				HSUSBD_PreSetup();
			}
			else
				g_u8ResetAvailable = 1;
		}
		else
		{
			g_u8ResetAvailable = 1;
			sysprintf("RESET handling skipped\n");
		}
		/************************************/
	}

	if (intr_status & HSUSBD_GINTSTS_IEPInt_Msk)
	{
		ep_intr = HSUSBD->DAINT & 0x1ff;	/* get In endpoint interrupt */

		if (ep_intr & HSUSBD_DAINT_InEpInt0_Msk)	/* EP0 */
		{
			ep_intr_status = HSUSBD->IEP[EP0].DIEPINT;
			HSUSBD->IEP[EP0].DIEPINT = ep_intr_status;
			if (ep_intr_status & HSUSBD_DIEPINT_XferCompl_Msk)
			{
				HSUSBD_EP0_CompleteTx();

				if (gEp0State == WAIT_FOR_IN_COMPLETE)
					gEp0State = WAIT_FOR_SETUP;

				if (gEp0State == WAIT_FOR_SETUP)
					HSUSBD_PreSetup();
			}
		}
		if (ep_intr & HSUSBD_DAINT_InEpInt1_Msk)	/* EP1 */
		{
			ep_intr_status = HSUSBD->IEP[EP1].DIEPINT;
			HSUSBD->IEP[EP1].DIEPINT = ep_intr_status;
			if (ep_intr_status & HSUSBD_DIEPINT_XferCompl_Msk)
			{
				HID_SetInReport();
			}
		}
		if (ep_intr & HSUSBD_DAINT_InEpInt2_Msk)	/* EP2 */
		{
			ep_intr_status = HSUSBD->IEP[EP2].DIEPINT;
			HSUSBD->IEP[EP2].DIEPINT = ep_intr_status;
		}
		if (ep_intr & HSUSBD_DAINT_InEpInt3_Msk)	/* EP3 */
		{
			ep_intr_status = HSUSBD->IEP[EP3].DIEPINT;
			HSUSBD->IEP[EP3].DIEPINT = ep_intr_status;
		}
	}

	if (intr_status & HSUSBD_GINTSTS_OEPInt_Msk)
	{
		ep_intr = HSUSBD->DAINT & 0x1ff0000;	/* get Out endpoint interrupt */

		if (ep_intr & HSUSBD_DAINT_OutEPInt0_Msk)	/* EP0 */
		{
			ep_intr_status = HSUSBD->OEP[EP0].DOEPINT;
			HSUSBD->OEP[EP0].DOEPINT = ep_intr_status;

			if (ep_intr_status & HSUSBD_DOEPINT_SetUp_Msk)
			{
				if (gEp0State == WAIT_FOR_SETUP)
					HSUSBD_ProcessSetupPacket();
			}
			if (ep_intr_status & HSUSBD_DOEPINT_XferCompl_Msk)
			{
				if (gEp0State != WAIT_FOR_OUT_COMPLETE)
					HSUSBD_EP0_CompleteRx();
				else
				{
					gEp0State = WAIT_FOR_SETUP;
					HSUSBD_PreSetup();
				}
			}
		}
		if (ep_intr & HSUSBD_DAINT_OutEPInt1_Msk)	/* EP1 */
		{
			ep_intr_status = HSUSBD->OEP[EP1].DOEPINT;
			HSUSBD->OEP[EP1].DOEPINT = ep_intr_status;
			if (ep_intr_status & HSUSBD_DOEPINT_XferCompl_Msk)
			{
				HID_GetOutReport((uint8_t *)nc_addr64(ep1_buf_out), ddma_ep1_out[0].status.b.bytes);
			}
		}
		if (ep_intr & HSUSBD_DAINT_OutEPInt2_Msk)	/* EP2 */
		{
			ep_intr_status = HSUSBD->OEP[EP2].DOEPINT;
			HSUSBD->OEP[EP2].DOEPINT = ep_intr_status;
		}
		if (ep_intr & HSUSBD_DAINT_OutEPInt3_Msk)	/* EP3 */
		{
			ep_intr_status = HSUSBD->OEP[EP3].DOEPINT;
			HSUSBD->OEP[EP3].DOEPINT = ep_intr_status;
		}
	}
}

/*--------------------------------------------------------------------------*/
/**
  * @brief  HSUSBD Endpoint Configuration.
  * @param  None.
  */
void HID_InitForHighSpeed(void)
{
	/* EP1 ==> Interrupt Out endpoint, address 1 */
	HSUSBD->OEP[1].DOEPCTL = (HSUSBD_DOEPCTL_EPDis_Msk | HSUSBD_DOEPCTL_SNAK_Msk | HSUSBD_DEPCTL_TYPE_INTR);
	HSUSBD_SetEpMaxPktSize(EP1, EP_OUTPUT, EP1_MAX_PKT_SIZE);

	/* EP1 ==> Interrupt IN endpoint, address 1 */
	HSUSBD->IEP[1].DIEPCTL = (HSUSBD_DIEPCTL_EPDis_Msk | HSUSBD_DIEPCTL_SNAK_Msk | HSUSBD_DEPCTL_TYPE_INTR);
	HSUSBD->IEP[1].DIEPCTL &= ~(HSUSBD_DIEPCTL_TxFNum_Msk);
	HSUSBD->IEP[1].DIEPCTL |= HSUSBD_DIEPCTL_TX_FIFO_NUM(1);
	HSUSBD_SetEpMaxPktSize(EP1, EP_INPUT, EP1_MAX_PKT_SIZE);

	g_u32Ep1MaxPacketSize = EP1_MAX_PKT_SIZE;
}

void HID_InitForFullSpeed(void)
{
	/* EP1 ==> Interrupt Out endpoint, address 1 */
	HSUSBD->OEP[1].DOEPCTL = (HSUSBD_DOEPCTL_EPDis_Msk | HSUSBD_DOEPCTL_SNAK_Msk | HSUSBD_DEPCTL_TYPE_INTR);
	HSUSBD_SetEpMaxPktSize(EP1, EP_OUTPUT, EP1_OTHER_MAX_PKT_SIZE);

	/* EP1 ==> Interrupt IN endpoint, address 1 */
	HSUSBD->IEP[1].DIEPCTL = (HSUSBD_DIEPCTL_EPDis_Msk | HSUSBD_DIEPCTL_SNAK_Msk | HSUSBD_DEPCTL_TYPE_INTR);
	HSUSBD->IEP[1].DIEPCTL &= ~(HSUSBD_DIEPCTL_TxFNum_Msk);
	HSUSBD->IEP[1].DIEPCTL |= HSUSBD_DIEPCTL_TX_FIFO_NUM(1);
	HSUSBD_SetEpMaxPktSize(EP1, EP_INPUT, EP1_OTHER_MAX_PKT_SIZE);

    g_u32Ep1MaxPacketSize = EP1_OTHER_MAX_PKT_SIZE;
}

void HID_Init(void)
{
    /* Configure USB controller */
    /* Enable endpoint interrupt - EP0 in, EP0 out, EP1 in */
	HSUSBD_ENABLE_EP_INT(HSUSBD_DAINTMSK_InEpMsk0_Msk | HSUSBD_DAINTMSK_OutEPMsk0_Msk | HSUSBD_DAINTMSK_InEpMsk1_Msk | HSUSBD_DAINTMSK_OutEPMsk1_Msk);

	/* Unmask device OUT EP common interrupts */
	HSUSBD_ENABLE_OUT_EP_INT(HSUSBD_DOEPMSK_XferComplMsk_Msk | HSUSBD_DOEPMSK_AHBErrMsk_Msk | HSUSBD_DOEPMSK_SetUPMsk_Msk | HSUSBD_DOEPMSK_BnaOutIntrMsk_Msk);
	/* Unmask device IN EP common interrupts */
	HSUSBD_ENABLE_IN_EP_INT(HSUSBD_DOEPMSK_XferComplMsk_Msk | HSUSBD_DOEPMSK_AHBErrMsk_Msk | HSUSBD_DOEPMSK_BnaOutIntrMsk_Msk);

    /* Reset Address to 0 */
    HSUSBD_SET_ADDR(0);

    /*****************************************************/
    /* Control endpoint */
    HSUSBD_EP0_Configuration();
    HSUSBD_SetEp0MaxPktSize(EP0_MAX_PKT_SIZE);

    /*****************************************************/
    HID_InitForHighSpeed();
}

void HID_ClassRequest(void)
{
    static uint32_t u8Report = 0;
    static uint32_t u8Idle = 0;
	uint8_t *ptr;

    if (gUsbCmd.bmRequestType & 0x80) { // Device to host
        
        switch (gUsbCmd.bRequest) {
        case GET_REPORT:
            HSUSBD_PrepareCtrlIn((uint8_t *)&u8Report, 1ul);
            break;
        case GET_IDLE:
            HSUSBD_PrepareCtrlIn((uint8_t *)&u8Idle, 1ul);
            break;
        case GET_PROTOCOL:
            if (gUsbCmd.wIndex == 0x00) {
                HSUSBD_PrepareCtrlIn((uint8_t *)&g_u8ReportProtocol, 1ul);
                break;
            } else {
                // Stall
                /* Setup error, stall the device */
            	HSUSBD_SetStall(EP0);
                break;
            }
        default:
            // Stall
            /* Setup error, stall the device */
        	HSUSBD_SetStall(EP0);
            break;
        }
    } else {    // Host to device
        switch (gUsbCmd.bRequest) {
        case SET_REPORT:
            if (((gUsbCmd.wValue >> 8) & 0xff) == 3) {  /* Request Type = Feature */
            	HSUSBD_EP0_SendZero();
            } else {
                // Stall
                /* Setup error, stall the device */
            	HSUSBD_SetStall(EP0);
            }
            break;

        case SET_IDLE:
        	ptr = (uint8_t *)((uint64_t)(&u8Idle)|NON_CACHE);
        	*ptr = u8Idle = (gUsbCmd.wValue >> 8) & 0xff;
        	HSUSBD_EP0_SendZero();

        	/* setup EP1 */
        	HSUSBD->IEP[EP1].DIEPCTL |= (HSUSBD_DIEPCTL_USBActEP_Msk | HSUSBD_DEPCTL_SETD0PID);
            dcache_clean_invalidate_by_mva(ddma_ep1_in, sizeof(ddma_ep1_in));
	        ddma_ep1_in[0].buf = nc_addr64(ep1_buf_in);
	    	ddma_ep1_in[0].status.d32 = 0x0a000000;
	        dcache_clean_invalidate_by_mva(ddma_ep1_in, sizeof(ddma_ep1_in));
	    	HSUSBD->IEP[EP1].DIEPDMA = (uint64_t)&ddma_ep1_in[0];
	    	HSUSBD->IEP[EP1].DIEPCTL |= (HSUSBD_DIEPCTL_EPEna_Msk | HSUSBD_DIEPCTL_CNAK_Msk);
        	break;

        case SET_PROTOCOL:
        	ptr = (uint8_t *)((uint64_t)(&g_u8ReportProtocol)|NON_CACHE);

            if (gUsbCmd.wIndex == 0x00) {
                if (gUsbCmd.wValue == HID_BOOT_PROTOCOL) {
                	*ptr = g_u8ReportProtocol = HID_BOOT_PROTOCOL;
                } else {
                	*ptr = g_u8ReportProtocol = HID_REPORT_PROTOCOL;
                }
            	HSUSBD_EP0_SendZero();
            } else {
                // Stall
                /* Setup error, stall the device */
            	HSUSBD_SetStall(EP0);
                break;
            }
            break;
        default:
            // Stall
            /* Setup error, stall the device */
        	HSUSBD_SetStall(EP0);
        }
    }
}


void HID_VendorRequest(void)
{
    if (gUsbCmd.bmRequestType & 0x80)   /* request data transfer direction */
    {
        // Device to host
        switch (gUsbCmd.bRequest)
        {
        default:
        {
            /* Setup error, stall the device */
        	HSUSBD_SetStall(EP0);
            break;
        }
        }
    }
    else
    {
        // Host to device
        switch (gUsbCmd.bRequest)
        {
        default:
        {
            // Stall
            /* Setup error, stall the device */
        	HSUSBD_SetStall(EP0);
        }
        }
    }
}

/***************************************************************/
#define HID_CMD_SIGNATURE   0x43444948

/* HID Transfer Commands */
#define HID_CMD_NONE     0x00
#define HID_CMD_ERASE    0x71
#define HID_CMD_READ     0xD2
#define HID_CMD_WRITE    0xC3
#define HID_CMD_TEST     0xB4

#define PAGE_SIZE        512

#ifdef __ICCARM__
typedef __packed struct
{
    uint8_t u8Cmd;
    uint8_t u8Size;
    uint32_t u32Arg1;
    uint32_t u32Arg2;
    uint32_t u32Signature;
    uint32_t u32Checksum;
} CMD_T;

#else
typedef struct __attribute__((__packed__))
{
    uint8_t u8Cmd;
    uint8_t u8Size;
    uint32_t u32Arg1;
    uint32_t u32Arg2;
    uint32_t u32Signature;
    uint32_t u32Checksum;
}
CMD_T;
#endif

CMD_T gCmd;

#ifdef __ICCARM__
#pragma data_alignment=4
static uint8_t  g_u8PageBuff[PAGE_SIZE] = {0};    /* Page buffer to upload/download through HID report */
static uint32_t g_u32BytesInPageBuf = 0;          /* The bytes of data in g_u8PageBuff */
#else
static uint8_t  g_u8PageBuff[PAGE_SIZE] __attribute__((aligned(4))) = {0};    /* Page buffer to upload/download through HID report */
static uint32_t g_u32BytesInPageBuf __attribute__((aligned(4))) = 0;          /* The bytes of data in g_u8PageBuff */
#endif


int32_t HID_CmdEraseSectors(CMD_T *pCmd)
{
    uint32_t u32StartSector;
    uint32_t u32Sectors;
    uint32_t i;

    u32StartSector = pCmd->u32Arg1;
    u32Sectors = pCmd->u32Arg2;

    sysprintf("Erase command - Sector: %d   Sector Cnt: %d\n", u32StartSector, u32Sectors);

    for(i=0; i<u32Sectors; i++)
    {
        //sysprintf("Erase sector - %d\n",u32StartSector + i);
        /* TODO: To erase the sector of storage */

    }
    /* To note the command has been done */
    pCmd->u8Cmd = HID_CMD_NONE;

    return 0;
}


int32_t HID_CmdReadPages(CMD_T *pCmd)
{
    uint32_t u32StartPage;
    uint32_t u32Pages;
    uint32_t len;
    int32_t i;

    u32StartPage = pCmd->u32Arg1;
    u32Pages     = pCmd->u32Arg2;

    sysprintf("Read command - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);

    if(u32Pages)
    {
        /* Update data to page buffer to upload */
        /* TODO: We need to update the page data if got a page read command. (0xFF is used in this sample code) */
    	dcache_clean_invalidate_by_mva(g_u8PageBuff, PAGE_SIZE);
        for(i=0; i<PAGE_SIZE; i++)
            g_u8PageBuff[i] = 0xFF;
    	dcache_clean_invalidate_by_mva(g_u8PageBuff, PAGE_SIZE);
        g_u32BytesInPageBuf = PAGE_SIZE;

        /* The signature word is used as page counter */
        pCmd->u32Signature = 1;

        /* Trigger HID IN */
        len = Minimum(g_u32Ep1MaxPacketSize, g_u32BytesInPageBuf);
    	dcache_clean_invalidate_by_mva(ddma_ep1_in, sizeof(S_HSUSBD_DMA_DESC_T));
    	ddma_ep1_in[1].buf = nc_addr64(g_u8PageBuff);
    	ddma_ep1_in[1].status.d32 = 0x0;
    	ddma_ep1_in[1].status.b.bytes = len;
    	ddma_ep1_in[1].status.b.bs = 0;
    	ddma_ep1_in[1].status.b.l = 1;
    	ddma_ep1_in[1].status.b.ioc = 1;
    	dcache_clean_invalidate_by_mva(ddma_ep1_in, sizeof(S_HSUSBD_DMA_DESC_T));
    	HSUSBD->IEP[1].DIEPDMA = (uint64_t)&ddma_ep1_in[1];
    	HSUSBD->IEP[1].DIEPCTL |= (HSUSBD_DIEPCTL_EPEna_Msk | HSUSBD_DIEPCTL_CNAK_Msk);
        g_u32BytesInPageBuf -= len;
    }

    return 0;
}


int32_t HID_CmdWritePages(CMD_T *pCmd)
{
    uint32_t u32StartPage;
    uint32_t u32Pages;

    u32StartPage = pCmd->u32Arg1;
    u32Pages     = pCmd->u32Arg2;

    sysprintf("Write command - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);
    g_u32BytesInPageBuf = 0;

    /* The signature is used to page counter */
    pCmd->u32Signature = 0;
    return 0;
}


int32_t gi32CmdTestCnt = 0;
int32_t HID_CmdTest(CMD_T *pCmd)
{
    int32_t i;
    uint8_t *pu8;

    pu8 = (uint8_t *)pCmd;
    sysprintf("Get test command #%d (%d bytes)\n", gi32CmdTestCnt++, pCmd->u8Size);
    for(i=0; i<pCmd->u8Size; i++)
    {
        if((i&0xF) == 0)
        {
            sysprintf("\n");
        }
        sysprintf(" %02x", pu8[i]);
    }

    sysprintf("\n");


    /* To note the command has been done */
    pCmd->u8Cmd = HID_CMD_NONE;

    return 0;
}


uint32_t CalCheckSum(uint8_t *buf, uint32_t size)
{
    uint32_t sum;
    int32_t i;

    i = 0;
    sum = 0;
    while(size--)
    {
        sum+=buf[i++];
    }

    return sum;

}


int32_t ProcessCommand(uint8_t *pu8Buffer, uint32_t u32BufferLen)
{
    uint32_t u32sum;

    HSUSBD_MemCopy((uint8_t *)&gCmd, pu8Buffer, u32BufferLen);

    /* Check size */
    if((gCmd.u8Size > sizeof(gCmd)) || (gCmd.u8Size > u32BufferLen))
        return -1;

    /* Check signature */
    if(gCmd.u32Signature != HID_CMD_SIGNATURE)
        return -1;

    /* Calculate checksum & check it*/
    u32sum = CalCheckSum((uint8_t *)&gCmd, gCmd.u8Size);
    if(u32sum != gCmd.u32Checksum)
        return -1;

    switch(gCmd.u8Cmd)
    {
    case HID_CMD_ERASE:
    {
        HID_CmdEraseSectors(&gCmd);
        break;
    }
    case HID_CMD_READ:
    {
        HID_CmdReadPages(&gCmd);
        break;
    }
    case HID_CMD_WRITE:
    {
        HID_CmdWritePages(&gCmd);
        break;
    }
    case HID_CMD_TEST:
    {
        HID_CmdTest(&gCmd);
        break;
    }
    default:
        return -1;
    }

	/* setup Out EP1 */
	HSUSBD->OEP[EP1].DOEPCTL |= (HSUSBD_DOEPCTL_USBActEP_Msk | HSUSBD_DEPCTL_SETD0PID);
	dcache_clean_invalidate_by_mva(ddma_ep1_out, sizeof(S_HSUSBD_DMA_DESC_T));
	ddma_ep1_out[0].buf = nc_addr64(ep1_buf_out);
	ddma_ep1_out[0].status.d32 = 0x0;
	ddma_ep1_out[0].status.b.bytes = g_u32Ep1MaxPacketSize;
	ddma_ep1_out[0].status.b.bs = 0;
	ddma_ep1_out[0].status.b.l = 1;
	ddma_ep1_out[0].status.b.ioc = 1;
	dcache_clean_invalidate_by_mva(ddma_ep1_out, sizeof(S_HSUSBD_DMA_DESC_T));
	HSUSBD->OEP[1].DOEPDMA = (uint64_t)&ddma_ep1_out[0];
	HSUSBD->OEP[1].DOEPCTL |= (HSUSBD_DOEPCTL_EPEna_Msk | HSUSBD_DOEPCTL_CNAK_Msk);
	sysprintf("3. 0x%08x\n", ddma_ep1_out[0].status);

	return 0;
}


void HID_GetOutReport(uint8_t *pu8EpBuf, uint32_t u32Size)
{
    uint32_t volatile i;
    uint8_t  u8Cmd;
    uint32_t u32StartPage;
    uint32_t u32Pages;
    uint32_t u32PageCnt;

    /* Get command information */
    u8Cmd        = gCmd.u8Cmd;
    u32StartPage = gCmd.u32Arg1;
    u32Pages     = gCmd.u32Arg2;
    u32PageCnt   = gCmd.u32Signature; /* The signature word is used to count pages */

    /* Check if it is in the data phase of write command */
    if ((u8Cmd == HID_CMD_WRITE) && (u32PageCnt < u32Pages))
    {
        /* Process the data phase of write command */

        /* Get data from HID OUT */
        HSUSBD_MemCopy(&g_u8PageBuff[g_u32BytesInPageBuf], pu8EpBuf, u32Size);
        g_u32BytesInPageBuf += u32Size;

        /* The HOST must make sure the data is PAGE_SIZE alignment */
        if (g_u32BytesInPageBuf >= PAGE_SIZE)
        {
            sysprintf("Writing page %d\n", u32StartPage + u32PageCnt);
            /* TODO: We should program received data to storage here */
            u32PageCnt++;

            /* Write command complete! */
            if (u32PageCnt >= u32Pages)
            {
                u8Cmd = HID_CMD_NONE;

                sysprintf("Write command complete.\n");
            }

            g_u32BytesInPageBuf = 0;

            if (u32PageCnt < u32Pages)
            {
				/* setup Out EP1 */
				dcache_clean_invalidate_by_mva(ddma_ep1_out, sizeof(S_HSUSBD_DMA_DESC_T));
				ddma_ep1_out[0].buf = nc_addr64(&g_u8PageBuff[g_u32BytesInPageBuf]);
				ddma_ep1_out[0].status.d32 = 0x0;
				ddma_ep1_out[0].status.b.bytes = u32Size;
				ddma_ep1_out[0].status.b.bs = 0;
				ddma_ep1_out[0].status.b.l = 1;
				ddma_ep1_out[0].status.b.ioc = 1;
				dcache_clean_invalidate_by_mva(ddma_ep1_out, sizeof(S_HSUSBD_DMA_DESC_T));
				HSUSBD->OEP[1].DOEPDMA = (uint64_t)&ddma_ep1_out[0];
				HSUSBD->OEP[1].DOEPCTL |= (HSUSBD_DOEPCTL_EPEna_Msk | HSUSBD_DOEPCTL_CNAK_Msk);
				sysprintf("4. 0x%08x\n", ddma_ep1_out[0].status);
            }
            else
            {
            	/* setup Out EP1 */
            	HSUSBD->OEP[EP1].DOEPCTL |= (HSUSBD_DOEPCTL_USBActEP_Msk | HSUSBD_DEPCTL_SETD0PID);
            	dcache_clean_invalidate_by_mva(ddma_ep1_out, sizeof(S_HSUSBD_DMA_DESC_T));
            	ddma_ep1_out[0].buf = nc_addr64(ep1_buf_out);
            	ddma_ep1_out[0].status.d32 = 0x0;
            	ddma_ep1_out[0].status.b.bytes = g_u32Ep1MaxPacketSize;
            	ddma_ep1_out[0].status.b.bs = 0;
            	ddma_ep1_out[0].status.b.l = 1;
            	ddma_ep1_out[0].status.b.ioc = 1;
            	dcache_clean_invalidate_by_mva(ddma_ep1_out, sizeof(S_HSUSBD_DMA_DESC_T));
            	HSUSBD->OEP[1].DOEPDMA = (uint64_t)&ddma_ep1_out[0];
            	HSUSBD->OEP[1].DOEPCTL |= (HSUSBD_DOEPCTL_EPEna_Msk | HSUSBD_DOEPCTL_CNAK_Msk);
            	sysprintf("5. 0x%08x\n", ddma_ep1_out[0].status);
            }
        }

        /* Update command status */
        gCmd.u8Cmd        = u8Cmd;
        gCmd.u32Signature = u32PageCnt;
    }
    else
    {
        /* Check and process the command packet */
        if(ProcessCommand(pu8EpBuf, u32Size))
        {
            sysprintf("Unknown HID command!\n");
        }
    }
}

void HID_SetInReport(void)
{
    uint32_t u32StartPage;
    uint32_t u32TotalPages;
    uint32_t u32PageCnt;
    uint32_t len;
    int32_t i;
    uint8_t u8Cmd;

    u8Cmd         = gCmd.u8Cmd;
    u32StartPage  = gCmd.u32Arg1;
    u32TotalPages = gCmd.u32Arg2;
    u32PageCnt    = gCmd.u32Signature;

    if (u8Cmd == HID_CMD_NONE)
    {
    	/* setup Out EP1 */
    	HSUSBD->OEP[EP1].DOEPCTL |= (HSUSBD_DOEPCTL_USBActEP_Msk | HSUSBD_DEPCTL_SETD0PID);
    	dcache_clean_invalidate_by_mva(ddma_ep1_out, sizeof(S_HSUSBD_DMA_DESC_T));
    	ddma_ep1_out[0].buf = nc_addr64(ep1_buf_out);
    	ddma_ep1_out[0].status.d32 = 0x0;
    	ddma_ep1_out[0].status.b.bytes = g_u32Ep1MaxPacketSize;
    	ddma_ep1_out[0].status.b.bs = 0;
    	ddma_ep1_out[0].status.b.l = 1;
    	ddma_ep1_out[0].status.b.ioc = 1;
    	dcache_clean_invalidate_by_mva(ddma_ep1_out, sizeof(S_HSUSBD_DMA_DESC_T));
    	HSUSBD->OEP[1].DOEPDMA = (uint64_t)&ddma_ep1_out[0];
    	HSUSBD->OEP[1].DOEPCTL |= (HSUSBD_DOEPCTL_EPEna_Msk | HSUSBD_DOEPCTL_CNAK_Msk);
    	sysprintf("1. 0x%08x\n", ddma_ep1_out[0].status);
    }
    /* Check if it is in data phase of read command */
    if (u8Cmd == HID_CMD_READ)
    {
        /* Process the data phase of read command */
        if ((u32PageCnt >= u32TotalPages) && (g_u32BytesInPageBuf == 0))
        {
            /* The data transfer is complete. */
            u8Cmd = HID_CMD_NONE;
            sysprintf("Read command complete!\n");
        }
        else
        {
            if (g_u32BytesInPageBuf == 0)
            {
                /* The previous page has sent out. Read new page to page buffer */
                /* TODO: We should update new page data here. (0xFF is used in this sample code) */
                sysprintf("Reading page %d\n", u32StartPage + u32PageCnt);
                //for (i=0; i<PAGE_SIZE; i++)
                //    g_u8PageBuff[i] = 0xFF;

                g_u32BytesInPageBuf = PAGE_SIZE;

                /* Update the page counter */
                u32PageCnt++;
            }

            /* Prepare the data for next HID IN transfer */
            len = Minimum(g_u32Ep1MaxPacketSize, g_u32BytesInPageBuf);

        	dcache_clean_invalidate_by_mva(ddma_ep1_in, sizeof(S_HSUSBD_DMA_DESC_T));
        	ddma_ep1_in[1].buf = nc_addr64(&g_u8PageBuff[PAGE_SIZE - g_u32BytesInPageBuf]);
        	ddma_ep1_in[1].status.d32 = 0x0;
        	ddma_ep1_in[1].status.b.bytes = len;
        	ddma_ep1_in[1].status.b.bs = 0;
        	ddma_ep1_in[1].status.b.l = 1;
        	ddma_ep1_in[1].status.b.ioc = 1;
        	dcache_clean_invalidate_by_mva(ddma_ep1_in, sizeof(S_HSUSBD_DMA_DESC_T));
        	HSUSBD->IEP[1].DIEPDMA = (uint64_t)&ddma_ep1_in[1];
        	HSUSBD->IEP[1].DIEPCTL |= (HSUSBD_DIEPCTL_EPEna_Msk | HSUSBD_DIEPCTL_CNAK_Msk);

        	g_u32BytesInPageBuf -= len;
        }
    }

    gCmd.u8Cmd        = u8Cmd;
    gCmd.u32Signature = u32PageCnt;

}




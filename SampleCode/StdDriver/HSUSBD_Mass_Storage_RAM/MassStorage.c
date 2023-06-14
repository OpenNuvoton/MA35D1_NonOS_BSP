/***************************************************************************//**
 * @file     MassStorage.c
 * @brief    HSUSBD Mass Storage class Sample file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

/*!<Includes */
#include <string.h>
#include "NuMicro.h"
#include "hsusbd.h"
#include "massstorage.h"

/******************************************************************************/
/* Global parameters                                                          */
/******************************************************************************/
int32_t g_TotalSectors = 0;
S_HSUSBD_DMA_DESC_T ddma_ep1_in[4];
S_HSUSBD_DMA_DESC_T ddma_ep1_out[4];

/* USB flow control variables */
uint8_t g_u8BulkState = BULK_NORMAL;
uint8_t g_u8Prevent = 0;
uint8_t volatile g_u8MscStart = 0;
uint8_t g_au8SenseKey[4];
uint8_t volatile g_u8EP1InReady = 0, g_u8EP1OutReady = 0;

uint32_t g_u32MSCMaxLun = 0;
uint32_t g_u32LbaAddress;
uint32_t g_u32MassBase, g_u32StorageBase;

uint32_t g_u32EpMaxPacketSize;

/* CBW/CSW variables */
struct CBW g_sCBW;
struct CSW g_sCSW;

/*--------------------------------------------------------------------------*/
uint8_t g_au8InquiryID[36] =
{
    0x00,                   /* Peripheral Device Type */
    0x80,                   /* RMB */
    0x00,                   /* ISO/ECMA, ANSI Version */
    0x00,                   /* Response Data Format */
    0x1F, 0x00, 0x00, 0x00, /* Additional Length */

    /* Vendor Identification */
    'N', 'u', 'v', 'o', 't', 'o', 'n', ' ',

    /* Product Identification */
    'U', 'S', 'B', ' ', 'M', 'a', 's', 's', ' ', 'S', 't', 'o', 'r', 'a', 'g', 'e',

    /* Product Revision */
    '1', '.', '0', '0'
};

// code = 5Ah, Mode Sense
static uint8_t g_au8ModePage_01[12] =
{
    0x01, 0x0A, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00
};

static uint8_t g_au8ModePage_05[32] =
{
    0x05, 0x1E, 0x13, 0x88, 0x08, 0x20, 0x02, 0x00,
    0x01, 0xF4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x05, 0x1E, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x68, 0x00, 0x00
};

static uint8_t g_au8ModePage_1B[12] =
{
    0x1B, 0x0A, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

static uint8_t g_au8ModePage_1C[8] =
{
    0x1C, 0x06, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00
};

static uint8_t g_au8ModePage[24] =
{
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x00, 0x1C, 0x0A, 0x80, 0x03,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
};

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
				//sysprintf("got reset (%d)!!\n",g_u8ResetAvailable);
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
			//sysprintf("RESET handling skipped\n");
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
			    g_u8EP1InReady = 1;
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
			    g_u8EP1OutReady = 1;
			    if (g_u8BulkState == BULK_OUT)
			    	g_u8BulkState = BULK_CBW;
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

/**
  * @brief  HSUSBD Endpoint Configuration.
  * @param  None.
  */
void MSC_InitForHighSpeed(void)
{
    /* EP1 ==> Bulk Out endpoint, address 1 */
	HSUSBD->OEP[1].DOEPCTL = (HSUSBD_DOEPCTL_EPDis_Msk | HSUSBD_DOEPCTL_SNAK_Msk | HSUSBD_DEPCTL_TYPE_BULK);
	HSUSBD_SetEpMaxPktSize(EP1, EP_OUTPUT, EP1_MAX_PKT_SIZE);

	/* EP1 ==> Bulk In endpoint, address 1 */
	HSUSBD->IEP[1].DIEPCTL = (HSUSBD_DIEPCTL_EPDis_Msk | HSUSBD_DIEPCTL_SNAK_Msk | HSUSBD_DEPCTL_TYPE_BULK);
	HSUSBD->IEP[1].DIEPCTL &= ~(HSUSBD_DIEPCTL_TxFNum_Msk);
	HSUSBD->IEP[1].DIEPCTL |= HSUSBD_DIEPCTL_TX_FIFO_NUM(1);
	HSUSBD_SetEpMaxPktSize(EP1, EP_INPUT, EP1_MAX_PKT_SIZE);
}

void MSC_InitForFullSpeed(void)
{
    /* EP1 ==> Bulk Out endpoint, address 1 */
	HSUSBD->OEP[1].DOEPCTL = (HSUSBD_DOEPCTL_EPDis_Msk | HSUSBD_DOEPCTL_SNAK_Msk | HSUSBD_DEPCTL_TYPE_BULK);
	HSUSBD_SetEpMaxPktSize(EP1, EP_OUTPUT, EP1_OTHER_MAX_PKT_SIZE);

	/* EP1 ==> Bulk In endpoint, address 1 */
	HSUSBD->IEP[1].DIEPCTL = (HSUSBD_DIEPCTL_EPDis_Msk | HSUSBD_DIEPCTL_SNAK_Msk | HSUSBD_DEPCTL_TYPE_BULK);
	HSUSBD->IEP[1].DIEPCTL &= ~(HSUSBD_DIEPCTL_TxFNum_Msk);
	HSUSBD->IEP[1].DIEPCTL |= HSUSBD_DIEPCTL_TX_FIFO_NUM(1);
	HSUSBD_SetEpMaxPktSize(EP1, EP_INPUT, EP1_OTHER_MAX_PKT_SIZE);
}

void MSC_Init(void)
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
    MSC_InitForHighSpeed();

    g_sCSW.dCSWSignature = CSW_SIGNATURE;
    /* when doing FDBENCH, the disk size should >= 5MB */
    //g_TotalSectors = 10240;
    g_TotalSectors = 60;
    g_u32MassBase = 0x80600000;
    g_u32StorageBase = 0x80610000;
}

void MSC_ClassRequest(void)
{
    g_u8MscStart = 1;
    if (gUsbCmd.bmRequestType & 0x80)   /* request data transfer direction */
    {
        // Device to host
        switch (gUsbCmd.bRequest)
        {
        case GET_MAX_LUN:
        {
            // Return current configuration setting
            HSUSBD_PrepareCtrlIn((uint8_t *)&g_u32MSCMaxLun, 1);
            g_u8MscStart = 1;

        	/* Active EP1 */
        	HSUSBD->IEP[EP1].DIEPCTL |= (HSUSBD_DIEPCTL_USBActEP_Msk | HSUSBD_DEPCTL_SETD0PID);
        	HSUSBD->OEP[EP1].DOEPCTL |= (HSUSBD_DOEPCTL_USBActEP_Msk | HSUSBD_DEPCTL_SETD0PID);
            break;
        }
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
        case BULK_ONLY_MASS_STORAGE_RESET:
        {
        	HSUSBD_EP0_SendZero();
            break;
        }
        default:
        {
            // Stall
            /* Setup error, stall the device */
        	HSUSBD_SetStall(EP0);
            break;
        }
        }
    }
}


void MSC_RequestSense(void)
{
    memset((uint8_t *)nc_ptr(g_u32MassBase), 0, 18);
    if (g_u8Prevent)
    {
        g_u8Prevent = 0;
        *(uint8_t *)nc_ptr(g_u32MassBase) = 0x70;
    }
    else
        *(uint8_t *)nc_ptr(g_u32MassBase) = 0xf0;

    *(uint8_t *)nc_ptr(g_u32MassBase + 2) = g_au8SenseKey[0];
    *(uint8_t *)nc_ptr(g_u32MassBase + 7) = 0x0a;
    *(uint8_t *)nc_ptr(g_u32MassBase + 12) = g_au8SenseKey[1];
    *(uint8_t *)nc_ptr(g_u32MassBase + 13) = g_au8SenseKey[2];
    MSC_BulkIn((uint8_t *)(uint64_t)g_u32MassBase, g_sCBW.dCBWDataTransferLength);

    g_au8SenseKey[0] = 0;
    g_au8SenseKey[1] = 0;
    g_au8SenseKey[2] = 0;
}

void MSC_ReadFormatCapacity(void)
{
    memset((uint8_t *)nc_ptr(g_u32MassBase), 0, 36);

    *((uint8_t *)nc_ptr(g_u32MassBase+3)) = 0x10;
    *((uint8_t *)nc_ptr(g_u32MassBase+4)) = *((uint8_t *)&g_TotalSectors+3);
    *((uint8_t *)nc_ptr(g_u32MassBase+5)) = *((uint8_t *)&g_TotalSectors+2);
    *((uint8_t *)nc_ptr(g_u32MassBase+6)) = *((uint8_t *)&g_TotalSectors+1);
    *((uint8_t *)nc_ptr(g_u32MassBase+7)) = *((uint8_t *)&g_TotalSectors+0);
    *((uint8_t *)nc_ptr(g_u32MassBase+8)) = 0x02;
    *((uint8_t *)nc_ptr(g_u32MassBase+10)) = 0x02;
    *((uint8_t *)nc_ptr(g_u32MassBase+12)) = *((uint8_t *)&g_TotalSectors+3);
    *((uint8_t *)nc_ptr(g_u32MassBase+13)) = *((uint8_t *)&g_TotalSectors+2);
    *((uint8_t *)nc_ptr(g_u32MassBase+14)) = *((uint8_t *)&g_TotalSectors+1);
    *((uint8_t *)nc_ptr(g_u32MassBase+15)) = *((uint8_t *)&g_TotalSectors+0);
    *((uint8_t *)nc_ptr(g_u32MassBase+18)) = 0x02;

    MSC_BulkIn((uint8_t *)(uint64_t)g_u32MassBase, g_sCBW.dCBWDataTransferLength);
}

void MSC_ReadCapacity(void)
{
    uint32_t tmp;

    memset((uint8_t *)nc_ptr(g_u32MassBase), 0, 36);

    tmp = g_TotalSectors - 1;
    *((uint8_t *)nc_ptr(g_u32MassBase+0)) = *((uint8_t *)&tmp+3);
    *((uint8_t *)nc_ptr(g_u32MassBase+1)) = *((uint8_t *)&tmp+2);
    *((uint8_t *)nc_ptr(g_u32MassBase+2)) = *((uint8_t *)&tmp+1);
    *((uint8_t *)nc_ptr(g_u32MassBase+3)) = *((uint8_t *)&tmp+0);
    *((uint8_t *)nc_ptr(g_u32MassBase+6)) = 0x02;

    MSC_BulkIn((uint8_t *)(uint64_t)g_u32MassBase, g_sCBW.dCBWDataTransferLength);
}

void MSC_ModeSense10(void)
{
    uint8_t i,j;
    uint8_t NumHead,NumSector;
    uint16_t NumCyl=0;

    /* Clear the command buffer */
    *((uint32_t *)(uint64_t)g_u32MassBase) = 0;
    *((uint32_t *)(uint64_t)g_u32MassBase + 1) = 0;

    switch (g_sCBW.au8Data[0])
    {
    case 0x01:
        *((uint8_t *)(uint64_t)g_u32MassBase) = 19;
        i = 8;
        for (j = 0; j<12; j++, i++)
            *((uint8_t *)nc_ptr(g_u32MassBase+i)) = g_au8ModePage_01[j];
        break;

    case 0x05:
        *((uint8_t *)(uint64_t)g_u32MassBase) = 39;
        i = 8;
        for (j = 0; j<32; j++, i++)
            *((uint8_t *)nc_ptr(g_u32MassBase+i)) = g_au8ModePage_05[j];

        NumHead = 2;
        NumSector = 64;
        NumCyl = g_TotalSectors / 128;

        *((uint8_t *)nc_ptr(g_u32MassBase+12)) = NumHead;
        *((uint8_t *)nc_ptr(g_u32MassBase+13)) = NumSector;
        *((uint8_t *)nc_ptr(g_u32MassBase+16)) = (uint8_t)(NumCyl >> 8);
        *((uint8_t *)nc_ptr(g_u32MassBase+17)) = (uint8_t)(NumCyl & 0x00ff);
        break;

    case 0x1B:
        *((uint8_t *)(uint64_t)g_u32MassBase) = 19;
        i = 8;
        for (j = 0; j<12; j++, i++)
            *((uint8_t *)nc_ptr(g_u32MassBase+i)) = g_au8ModePage_1B[j];
        break;

    case 0x1C:
        *((uint8_t *)(uint64_t)g_u32MassBase) = 15;
        i = 8;
        for (j = 0; j<8; j++, i++)
            *((uint8_t *)nc_ptr(g_u32MassBase+i)) = g_au8ModePage_1C[j];
        break;

    case 0x3F:
        *((uint8_t *)(uint64_t)g_u32MassBase) = 0x47;
        i = 8;
        for (j = 0; j<12; j++, i++)
            *((uint8_t *)nc_ptr(g_u32MassBase+i)) = g_au8ModePage_01[j];
        for (j = 0; j<32; j++, i++)
            *((uint8_t *)nc_ptr(g_u32MassBase+i)) = g_au8ModePage_05[j];
        for (j = 0; j<12; j++, i++)
            *((uint8_t *)nc_ptr(g_u32MassBase+i)) = g_au8ModePage_1B[j];
        for (j = 0; j<8; j++, i++)
            *((uint8_t *)nc_ptr(g_u32MassBase+i)) = g_au8ModePage_1C[j];

        NumHead = 2;
        NumSector = 64;
        NumCyl = g_TotalSectors / 128;

        *((uint8_t *)nc_ptr(g_u32MassBase+24)) = NumHead;
        *((uint8_t *)nc_ptr(g_u32MassBase+25)) = NumSector;
        *((uint8_t *)nc_ptr(g_u32MassBase+28)) = (uint8_t)(NumCyl >> 8);
        *((uint8_t *)nc_ptr(g_u32MassBase+29)) = (uint8_t)(NumCyl & 0x00ff);
        break;

    default:
        g_au8SenseKey[0] = 0x05;
        g_au8SenseKey[1] = 0x24;
        g_au8SenseKey[2] = 0x00;
    }
    MSC_BulkIn((uint8_t *)(uint64_t)g_u32MassBase, g_sCBW.dCBWDataTransferLength);
}

void MSC_ModeSense6(void)
{
    uint8_t i;

    for (i = 0; i<4; i++)
        *((uint8_t *)nc_ptr(g_u32MassBase+i)) = g_au8ModePage[i];

    MSC_BulkIn((uint8_t *)(uint64_t)g_u32MassBase, g_sCBW.dCBWDataTransferLength);
}

void MSC_BulkOut(uint8_t *u8Buf, uint32_t u32Len)
{
	dcache_clean_invalidate_by_mva(ddma_ep1_out, sizeof(S_HSUSBD_DMA_DESC_T));
	ddma_ep1_out[0].buf = nc_addr64(u8Buf);
	ddma_ep1_out[0].status.d32 = 0x0;
	ddma_ep1_out[0].status.b.bytes = u32Len;
	ddma_ep1_out[0].status.b.bs = 0;
	ddma_ep1_out[0].status.b.l = 1;
	ddma_ep1_out[0].status.b.ioc = 1;
	dcache_clean_invalidate_by_mva(ddma_ep1_out, sizeof(S_HSUSBD_DMA_DESC_T));
    g_u8EP1OutReady = 0;
	HSUSBD->OEP[1].DOEPDMA = (uint64_t)&ddma_ep1_out[0];
	HSUSBD->OEP[1].DOEPCTL |= (HSUSBD_DOEPCTL_EPEna_Msk | HSUSBD_DOEPCTL_CNAK_Msk);
	while(1)
	{
		if (g_u8EP1OutReady)
		{
			break;
		}
	}
}

void MSC_BulkIn(uint8_t *u8Buf, uint32_t u32Len)
{
	dcache_clean_invalidate_by_mva(ddma_ep1_in, sizeof(S_HSUSBD_DMA_DESC_T));
	ddma_ep1_in[1].buf = nc_addr64(u8Buf);
	ddma_ep1_in[1].status.d32 = 0x0;
	ddma_ep1_in[1].status.b.bytes = u32Len;
	ddma_ep1_in[1].status.b.bs = 0;
	ddma_ep1_in[1].status.b.l = 1;
	ddma_ep1_in[1].status.b.ioc = 1;
	dcache_clean_invalidate_by_mva(ddma_ep1_in, sizeof(S_HSUSBD_DMA_DESC_T));
    g_u8EP1InReady = 0;
	HSUSBD->IEP[1].DIEPDMA = (uint64_t)&ddma_ep1_in[1];
	HSUSBD->IEP[1].DIEPCTL |= (HSUSBD_DIEPCTL_EPEna_Msk | HSUSBD_DIEPCTL_CNAK_Msk);

	while(1)
	{
		if (g_u8EP1InReady)
		{
			break;
		}
	}
}


void MSC_ReceiveCBW(uint8_t *u8Buf)
{
	dcache_clean_invalidate_by_mva(ddma_ep1_out, sizeof(S_HSUSBD_DMA_DESC_T));
	ddma_ep1_out[0].buf = nc_addr64(u8Buf);
	ddma_ep1_out[0].status.d32 = 0x0;
	ddma_ep1_out[0].status.b.bytes = 512;
	ddma_ep1_out[0].status.b.bs = 0;
	ddma_ep1_out[0].status.b.l = 1;
	ddma_ep1_out[0].status.b.ioc = 1;
	dcache_clean_invalidate_by_mva(ddma_ep1_out, sizeof(S_HSUSBD_DMA_DESC_T));
    g_u8EP1OutReady = 0;
	HSUSBD->OEP[1].DOEPDMA = (uint64_t)&ddma_ep1_out[0];
	HSUSBD->OEP[1].DOEPCTL |= (HSUSBD_DOEPCTL_EPEna_Msk | HSUSBD_DOEPCTL_CNAK_Msk);

	while(1)
	{
		if (g_u8EP1OutReady)
		{
			break;
		}
	}
}

void MSC_ProcessCmd(void)
{
    uint32_t i;

    if (g_u8BulkState == BULK_NORMAL)
    {
        g_u8BulkState = BULK_OUT;
        MSC_ReceiveCBW((uint8_t *)(uint64_t)g_u32MassBase);
    }

    if (g_u8BulkState == BULK_CBW)
    {
        /* Check Signature of CBW */
        if ((*(uint32_t *)nc_addr64(g_u32MassBase) != CBW_SIGNATURE))
        {
        	sysprintf("<0x%x>\n", *(uint32_t *)nc_addr64(g_u32MassBase));
            g_u8BulkState = BULK_NORMAL;
            return;
        }

    	//sysprintf("[0x%x]\n", *(uint32_t *)nc_addr64(g_u32MassBase));
        /* Get the CBW */
        for (i = 0; i < 31; i++)
            *((uint8_t *) (&g_sCBW.dCBWSignature) + i) = *(uint8_t *)nc_addr64(g_u32MassBase + i);

        /* Prepare to echo the tag from CBW to CSW */
        g_sCSW.dCSWTag = g_sCBW.dCBWTag;

        /* Parse Op-Code of CBW */
        switch (g_sCBW.u8OPCode)
        {
        case UFI_READ_10:
        {
            /* Get LBA address */
            g_u32LbaAddress = get_be32(&g_sCBW.au8Data[0]) * USBD_SECTOR_SIZE;
            //MSC_ReadMedia(g_u32LbaAddress, g_sCBW.dCBWDataTransferLength, (uint8_t *)g_u32StorageBase);

            MSC_BulkIn((uint8_t *)(uint64_t)(g_u32StorageBase+g_u32LbaAddress), g_sCBW.dCBWDataTransferLength);
            MSC_AckCmd(0);
            break;
        }
        case UFI_WRITE_10:
        {
            g_u32LbaAddress = get_be32(&g_sCBW.au8Data[0]) * USBD_SECTOR_SIZE;
            MSC_BulkOut((uint8_t *)(uint64_t)(g_u32StorageBase+g_u32LbaAddress), g_sCBW.dCBWDataTransferLength);
            MSC_AckCmd(0);
            break;
        }
        case UFI_PREVENT_ALLOW_MEDIUM_REMOVAL:
        {
            if (g_sCBW.au8Data[2] & 0x01)
            {
                g_au8SenseKey[0] = 0x05;  //INVALID COMMAND
                g_au8SenseKey[1] = 0x24;
                g_au8SenseKey[2] = 0;
                g_u8Prevent = 1;
            }
            else
                g_u8Prevent = 0;
            MSC_AckCmd(0);
            break;
        }
        case UFI_VERIFY_10:
        case UFI_START_STOP:
        case UFI_TEST_UNIT_READY:
        {
            MSC_AckCmd(0);
            break;
        }
        case UFI_REQUEST_SENSE:
        {
            MSC_RequestSense();
            MSC_AckCmd(0);
            break;
        }
        case UFI_READ_FORMAT_CAPACITY:
        {
            MSC_ReadFormatCapacity();
            MSC_AckCmd(0);
            break;
        }
        case UFI_READ_CAPACITY:
        {
            MSC_ReadCapacity();
            MSC_AckCmd(0);
            break;
        }
        case UFI_MODE_SELECT_10:
        {
            MSC_BulkOut((uint8_t *)(uint64_t)g_u32StorageBase, g_sCBW.dCBWDataTransferLength);
            MSC_AckCmd(0);
            break;
        }
        case UFI_MODE_SENSE_10:
        {
            MSC_ModeSense10();
            MSC_AckCmd(0);
            break;
        }
        case UFI_MODE_SENSE_6:
        {
            MSC_ModeSense6();
            MSC_AckCmd(0);
            break;
        }
        case UFI_INQUIRY:
        {
            MSC_BulkIn((uint8_t *)(uint64_t)g_au8InquiryID, g_sCBW.dCBWDataTransferLength);
            MSC_AckCmd(0);
            break;
        }
        default:
        {
            /* Unsupported command */
            g_au8SenseKey[0] = 0x05;
            g_au8SenseKey[1] = 0x20;
            g_au8SenseKey[2] = 0x00;

            /* If CBW request for data phase, just return zero packet to end data phase */
            if (g_sCBW.dCBWDataTransferLength > 0)
                MSC_AckCmd(g_sCBW.dCBWDataTransferLength);
            else
                MSC_AckCmd(0);
        }
        }
    }
}

void MSC_AckCmd(uint32_t u32Residue)
{
    g_sCSW.dCSWDataResidue = u32Residue;
    g_sCSW.bCSWStatus = g_u8Prevent;
    MSC_BulkIn((uint8_t *)(uint64_t)&g_sCSW, 13);
    g_u8BulkState = BULK_NORMAL;
}

void MSC_ReadMedia(uint32_t addr, uint32_t size, uint8_t *buffer)
{
}

void MSC_WriteMedia(uint32_t addr, uint32_t size, uint8_t *buffer)
{
}


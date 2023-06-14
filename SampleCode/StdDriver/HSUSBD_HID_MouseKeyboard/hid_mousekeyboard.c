/**************************************************************************//**
 * @file     hid_mouse.c
 * @version  V1.00
 * @brief    HSUSBD HID class Sample file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

/*!<Includes */
#include <string.h>
#include "NuMicro.h"
#include "hid_mousekeyboard.h"


/******************************************************************************/
/* Global parameters                                                          */
/******************************************************************************/
signed char mouse_table[] = {-16, -16, -16, 0, 16, 16, 16, 0};
uint8_t mouse_idx = 0;
uint8_t move_len, mouse_mode = 1;
uint8_t volatile g_u8EP1Ready = 0;
uint8_t volatile g_u8EP2Ready = 0;

static uint32_t volatile g_u8ReportProtocol = HID_REPORT_PROTOCOL;

S_HSUSBD_DMA_DESC_T ddma_ep1_in[2];
uint8_t ep1_buf_in[64];
S_HSUSBD_DMA_DESC_T ddma_ep2_in[2];
uint8_t ep2_buf_in[64];

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
			    g_u8EP1Ready = 1;
			}
		}
		if (ep_intr & HSUSBD_DAINT_InEpInt2_Msk)	/* EP2 */
		{
			ep_intr_status = HSUSBD->IEP[EP2].DIEPINT;
			HSUSBD->IEP[EP2].DIEPINT = ep_intr_status;
			if (ep_intr_status & HSUSBD_DIEPINT_XferCompl_Msk)
			{
			    g_u8EP2Ready = 1;
			}
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
    /* EP1 ==> Interrupt IN endpoint, address 1 */
	HSUSBD->IEP[1].DIEPCTL = (HSUSBD_DIEPCTL_EPDis_Msk | HSUSBD_DIEPCTL_SNAK_Msk | HSUSBD_DEPCTL_TYPE_INTR);
	HSUSBD->IEP[1].DIEPCTL &= ~(HSUSBD_DIEPCTL_TxFNum_Msk);
	HSUSBD->IEP[1].DIEPCTL |= HSUSBD_DIEPCTL_TX_FIFO_NUM(1);
	HSUSBD_SetEpMaxPktSize(EP1, EP_INPUT, EP1_MAX_PKT_SIZE);

	/* EP2 ==> Interrupt IN endpoint, address 2 */
	HSUSBD->IEP[2].DIEPCTL = (HSUSBD_DIEPCTL_EPDis_Msk | HSUSBD_DIEPCTL_SNAK_Msk | HSUSBD_DEPCTL_TYPE_INTR);
	HSUSBD->IEP[2].DIEPCTL &= ~(HSUSBD_DIEPCTL_TxFNum_Msk);
	HSUSBD->IEP[2].DIEPCTL |= HSUSBD_DIEPCTL_TX_FIFO_NUM(2);
	HSUSBD_SetEpMaxPktSize(EP2, EP_INPUT, EP2_MAX_PKT_SIZE);
}

void HID_InitForFullSpeed(void)
{
    /* EP1 ==> Interrupt IN endpoint, address 1 */
	HSUSBD->IEP[1].DIEPCTL = (HSUSBD_DIEPCTL_EPDis_Msk | HSUSBD_DIEPCTL_SNAK_Msk | HSUSBD_DEPCTL_TYPE_INTR);
	HSUSBD->IEP[1].DIEPCTL &= ~(HSUSBD_DIEPCTL_TxFNum_Msk);
	HSUSBD->IEP[1].DIEPCTL |= HSUSBD_DIEPCTL_TX_FIFO_NUM(1);
	HSUSBD_SetEpMaxPktSize(EP1, EP_INPUT, EP1_OTHER_MAX_PKT_SIZE);

    /* EP2 ==> Interrupt IN endpoint, address 2 */
	HSUSBD->IEP[2].DIEPCTL = (HSUSBD_DIEPCTL_EPDis_Msk | HSUSBD_DIEPCTL_SNAK_Msk | HSUSBD_DEPCTL_TYPE_INTR);
	HSUSBD->IEP[2].DIEPCTL &= ~(HSUSBD_DIEPCTL_TxFNum_Msk);
	HSUSBD->IEP[2].DIEPCTL |= HSUSBD_DIEPCTL_TX_FIFO_NUM(2);
	HSUSBD_SetEpMaxPktSize(EP2, EP_INPUT, EP2_OTHER_MAX_PKT_SIZE);
}

void HID_Init(void)
{
    /* Configure USB controller */
    /* Enable endpoint interrupt - EP0 in, EP0 out, EP1 in, EP2 in */
	HSUSBD_ENABLE_EP_INT(HSUSBD_DAINTMSK_InEpMsk0_Msk | HSUSBD_DAINTMSK_OutEPMsk0_Msk | HSUSBD_DAINTMSK_InEpMsk1_Msk | HSUSBD_DAINTMSK_InEpMsk2_Msk);

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
            HSUSBD_PrepareCtrlIn((uint8_t *)&g_u8ReportProtocol, 1ul);
            break;
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

	    	/* setup EP2 */
        	HSUSBD->IEP[EP2].DIEPCTL |= (HSUSBD_DIEPCTL_USBActEP_Msk | HSUSBD_DEPCTL_SETD0PID);
            dcache_clean_invalidate_by_mva(ddma_ep2_in, sizeof(ddma_ep2_in));
	        ddma_ep2_in[0].buf = nc_addr64(ep2_buf_in);
	    	ddma_ep2_in[0].status.d32 = 0x0a000000;
	        dcache_clean_invalidate_by_mva(ddma_ep2_in, sizeof(ddma_ep2_in));
	    	HSUSBD->IEP[EP2].DIEPDMA = (uint64_t)&ddma_ep2_in[0];
	    	HSUSBD->IEP[EP2].DIEPCTL |= (HSUSBD_DIEPCTL_EPEna_Msk | HSUSBD_DIEPCTL_CNAK_Msk);
	        /* start to IN data */
	        g_u8EP1Ready = 1;
	        g_u8EP2Ready = 1;
            break;

        case SET_PROTOCOL:
        	ptr = (uint8_t *)((uint64_t)(&g_u8ReportProtocol)|NON_CACHE);

            if (gUsbCmd.wValue == HID_BOOT_PROTOCOL) {
             	*ptr = g_u8ReportProtocol = HID_BOOT_PROTOCOL;
            } else {
              	*ptr = g_u8ReportProtocol = HID_REPORT_PROTOCOL;
            }
           	HSUSBD_EP0_SendZero();
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

void HID_UpdateMouseData(void)
{
    int volatile i;

    if (g_u8EP1Ready)
    {
        /* Update new report data */
        ep1_buf_in[0] = 0x00;
        ep1_buf_in[1] = 0x00;
        ep1_buf_in[2] = 0x00;
        ep1_buf_in[3] = 0x00;
        g_u8EP1Ready = 0;
        HSUSBD_StartDMA(EP1, &ddma_ep1_in[0], ep1_buf_in, 4);
    }
}

void HID_UpdateKeyboardData(void)
{
    int volatile i;

    if (g_u8EP2Ready)
    {
        /* Update new report data */
        for(i = 0; i < 8; i++)
        	ep2_buf_in[i] = 0;
        g_u8EP2Ready = 0;
        HSUSBD_StartDMA(EP2, &ddma_ep2_in[0], ep2_buf_in, 8);
    }
}

void HID_Process(void)
{
    HID_UpdateMouseData();
    HID_UpdateKeyboardData();
}


/**************************************************************************//**
 * @file     usbd_video_sink.c
 *
 * @brief    High-Speed USB (HSUSBD) Video Sink Data Transport Layer.
 *
 * @details  This file manages the USB transport logic for the video pipeline:
 *           - Implements HSUSBD interrupt handling (GINTSTS) and enumeration.
 *           - Configures Endpoint 1 (EP1) as a Bulk-OUT and Bulk-IN interface
 *             for high-throughput H.264 bitstream reception.
 *           - Utilizes Dedicated DMA (DDMA) with descriptor chains to offload
 *             CPU during large data transfers.
 *           - Provides abstraction APIs (usbd_vsink_bulk_rx/tx) for the upper-layer
 *             video decoding task to fetch raw bitstream data.
 *
 * @note     - The implementation uses a Bulk-based transport (often used in UVC
 *             uncompressed or custom H.264 streaming) rather than Isochronous.
 *           - Ensure the DMA descriptors (ddma_ep1_out) are aligned to 32 bytes
 *             for hardware compatibility.
 *           - Current inquiry data identifies as "Nuvoton USB Mass Storage",
 *             which serves as the base framework for the custom video sink.
 *
 * @copyright (C) 2026 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
/*!<Includes */
#include <string.h>
#include "NuMicro.h"
#include "hsusbd.h"
#include "usbd_video_sink.h"

/******************************************************************************/
/* Global parameters                                                          */
/******************************************************************************/
int32_t g_TotalSectors = 0;
S_HSUSBD_DMA_DESC_T ddma_ep1_in[4]  __attribute__((aligned(32)));
S_HSUSBD_DMA_DESC_T ddma_ep1_out[4] __attribute__((aligned(32)));

/* USB flow control variables */
uint8_t volatile g_u8EP1InReady = 0, g_u8EP1OutReady = 0;

extern uint32_t g_u32EpStallLock;

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
            //sysprintf("High Speed Detection: 0x%x\n", HSUSBD->DSTS & HSUSBD_DSTS_EnumSpd_Msk);
            HSUSBD_SetEp0MaxPktSize(EP0_MAX_PKT_SIZE);
            HSUSBD_SetEpMaxPktSize(EP1, EP_INPUT, EP1_MAX_PKT_SIZE);
        }
        else
        {
            //sysprintf("Full Speed Detection: 0x%x\n", HSUSBD->DSTS & HSUSBD_DSTS_EnumSpd_Msk);
            HSUSBD_SetEp0MaxPktSize(EP0_OTHER_MAX_PKT_SIZE);
            HSUSBD_SetEpMaxPktSize(EP1, EP_INPUT, EP1_OTHER_MAX_PKT_SIZE);
        }
        /* Active EP1 */
        HSUSBD->IEP[EP1].DIEPCTL |= (HSUSBD_DIEPCTL_USBActEP_Msk | HSUSBD_DEPCTL_SETD0PID);
        HSUSBD->OEP[EP1].DOEPCTL |= (HSUSBD_DOEPCTL_USBActEP_Msk | HSUSBD_DEPCTL_SETD0PID);
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
        // sysprintf("\n\nReset interrupt - (SYS_MISCISR):0x%x\n", SYS->MISCISR);
        fflush(stdout);
        HSUSBD->GINTSTS = HSUSBD_GINTSTS_USBRst_Msk;

        /************************************/
        /* check GMISC VBUS detect */
        if (SYS->MISCISR & SYS_MISCISR_VBUSSTS_Msk)
        {
            if (g_u8ResetAvailable)
            {
                // sysprintf("got reset (%d)!!\n", g_u8ResetAvailable);
                fflush(stdout);

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
            // sysprintf("RESET handling skipped\n");

        }
        /************************************/
    }

    if (intr_status & HSUSBD_GINTSTS_IEPInt_Msk)
    {
        ep_intr = HSUSBD->DAINT & 0x1ff;    /* get In endpoint interrupt */

        if (ep_intr & HSUSBD_DAINT_InEpInt0_Msk)    /* EP0 */
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
        if (ep_intr & HSUSBD_DAINT_InEpInt1_Msk)    /* EP1 */
        {
            ep_intr_status = HSUSBD->IEP[EP1].DIEPINT;
            HSUSBD->IEP[EP1].DIEPINT = ep_intr_status;
            if (ep_intr_status & HSUSBD_DIEPINT_XferCompl_Msk)
            {
                g_u8EP1InReady = 1;
            }
        }
        if (ep_intr & HSUSBD_DAINT_InEpInt2_Msk)    /* EP2 */
        {
            ep_intr_status = HSUSBD->IEP[EP2].DIEPINT;
            HSUSBD->IEP[EP2].DIEPINT = ep_intr_status;
        }
        if (ep_intr & HSUSBD_DAINT_InEpInt3_Msk)    /* EP3 */
        {
            ep_intr_status = HSUSBD->IEP[EP3].DIEPINT;
            HSUSBD->IEP[EP3].DIEPINT = ep_intr_status;
        }
    }

    if (intr_status & HSUSBD_GINTSTS_OEPInt_Msk)
    {
        ep_intr = HSUSBD->DAINT & 0x1ff0000;    /* get Out endpoint interrupt */

        if (ep_intr & HSUSBD_DAINT_OutEPInt0_Msk)   /* EP0 */
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
        if (ep_intr & HSUSBD_DAINT_OutEPInt1_Msk)   /* EP1 */
        {
            ep_intr_status = HSUSBD->OEP[EP1].DOEPINT;
            HSUSBD->OEP[EP1].DOEPINT = ep_intr_status;

            if (ep_intr_status & HSUSBD_DOEPINT_XferCompl_Msk)
            {
                g_u8EP1OutReady = 1;
            }
        }
        if (ep_intr & HSUSBD_DAINT_OutEPInt2_Msk)   /* EP2 */
        {
            ep_intr_status = HSUSBD->OEP[EP2].DOEPINT;
            HSUSBD->OEP[EP2].DOEPINT = ep_intr_status;
        }
        if (ep_intr & HSUSBD_DAINT_OutEPInt3_Msk)   /* EP3 */
        {
            ep_intr_status = HSUSBD->OEP[EP3].DOEPINT;
            HSUSBD->OEP[EP3].DOEPINT = ep_intr_status;
        }
    }
}

void usbd_vsink_config(void)
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
    /* EP1 ==> Bulk Out endpoint, address 1 */
    HSUSBD->OEP[1].DOEPCTL = (HSUSBD_DOEPCTL_EPDis_Msk | HSUSBD_DOEPCTL_SNAK_Msk | HSUSBD_DEPCTL_TYPE_BULK);
    HSUSBD_SetEpMaxPktSize(EP1, EP_OUTPUT, EP1_MAX_PKT_SIZE);

    /* EP1 ==> Bulk In endpoint, address 1 */
    HSUSBD->IEP[1].DIEPCTL = (HSUSBD_DIEPCTL_EPDis_Msk | HSUSBD_DIEPCTL_SNAK_Msk | HSUSBD_DEPCTL_TYPE_BULK);
    HSUSBD->IEP[1].DIEPCTL &= ~(HSUSBD_DIEPCTL_TxFNum_Msk);
    HSUSBD->IEP[1].DIEPCTL |= HSUSBD_DIEPCTL_TX_FIFO_NUM(1);
    HSUSBD_SetEpMaxPktSize(EP1, EP_INPUT, EP1_MAX_PKT_SIZE);
}

void usbd_vsink_class_request(void)
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
            // Stall
            /* Setup error, stall the device */
            HSUSBD_SetStall(EP0);
            break;
        }
    }
}

int usbd_vsink_bulk_tx(void *buff, uint32_t len)
{
    S_HSUSBD_DMA_DESC_T *desc;

    desc = nc_ptr(&ddma_ep1_in[0]);

    desc->buf = ptr_to_u32(buff);
    desc->status.d32 = 0x0;
    desc->status.b.bytes = len;
    desc->status.b.bs = 0;
    desc->status.b.l = 1;
    desc->status.b.ioc = 1;

    g_u8EP1InReady = 0;
    HSUSBD->IEP[1].DIEPDMA = ptr_to_u32(desc);
    HSUSBD->IEP[1].DIEPCTL |= (HSUSBD_DIEPCTL_EPEna_Msk | HSUSBD_DIEPCTL_CNAK_Msk);

    while(1)
    {
        if (g_u8EP1InReady)
            return 0;
    }
}

int usbd_vsink_bulk_rx(void *buff)
{
    S_HSUSBD_DMA_DESC_T *desc;
    uint32_t t0;

    desc = nc_ptr(&ddma_ep1_out[0]);

    desc->buf = ptr_to_u32(buff);
    desc->status.d32 = 0;
    desc->status.d32 = 0x0;
    desc->status.b.bytes = VSINK_CHUNK_SIZE;
    desc->status.b.bs = 0x0;
    desc->status.b.l = 1;
    desc->status.b.ioc = 1;
    __DSB();

    g_u8EP1OutReady = 0;
    HSUSBD->OEP[1].DOEPDMA = ptr_to_u32(desc);
    HSUSBD->OEP[1].DOEPCTL |= (HSUSBD_DOEPCTL_EPEna_Msk | HSUSBD_DOEPCTL_CNAK_Msk);

    while (HSUSBD_IS_ATTACHED() && g_hsusbd_Configured)
    {
        if (g_u8EP1OutReady)
            return 0;
    }
    return -1;
}

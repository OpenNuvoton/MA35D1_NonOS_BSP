/**************************************************************************//**
 * @file    ohci_0.c
 * @brief   USB Host library USBH0 OHCI driver wrapper
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usbh_lib.h"

/// @cond HIDDEN_SYMBOLS

#define _ohci    		_ohci0
#define ohci_driver		ohci0_driver

#define OHCI_IRQHandler   OHCI0_IRQHandler

#include "ohci.c"

HC_DRV_T  ohci0_driver =
{
    ohci_init,               /* init               */
    ohci_shutdown,           /* shutdown           */
    ohci_suspend,            /* suspend            */
    ohci_resume,             /* resume             */
    ohci_ctrl_xfer,          /* ctrl_xfer          */
    ohci_bulk_xfer,          /* bulk_xfer          */
    ohci_int_xfer,           /* int_xfer           */
    ohci_iso_xfer,           /* iso_xfer           */
    ohci_quit_xfer,          /* quit_xfer          */
    ohci_rh_port_reset,      /* rthub_port_reset   */
    ohci_rh_polling,         /* rthub_polling      */
};
/// @endcond HIDDEN_SYMBOLS

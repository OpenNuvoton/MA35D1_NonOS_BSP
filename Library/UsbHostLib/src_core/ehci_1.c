/**************************************************************************//**
 * @file    ehci_1.c
 * @brief   USB Host library HSUSBH1 EHCI driver wrapper
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usbh_lib.h"

/// @cond HIDDEN_SYMBOLS

#define _ehci     		_ehci1
#define ehci_driver		ehci1_driver

#define EHCI_IRQHandler   EHCI1_IRQHandler

#include "ehci.c"
#include "ehci_iso.c"

HC_DRV_T  ehci1_driver =
{
    ehci_init,               /* init               */
    ehci_shutdown,           /* shutdown           */
    ehci_suspend,            /* suspend            */
    ehci_resume,             /* resume             */
    ehci_ctrl_xfer,          /* ctrl_xfer          */
    ehci_bulk_xfer,          /* bulk_xfer          */
    ehci_int_xfer,           /* int_xfer           */
    ehci_iso_xfer,           /* iso_xfer           */
    ehci_quit_xfer,          /* quit_xfer          */
    ehci_rh_port_reset,      /* rthub_port_reset   */
    ehci_rh_polling,         /* rthub_polling      */
};
/// @endcond HIDDEN_SYMBOLS

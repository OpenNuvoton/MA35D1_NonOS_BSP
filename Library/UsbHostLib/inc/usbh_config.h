/**************************************************************************//**
 * @file     usbh_config.h
 * @brief    This header file defines the configuration of USB Host library.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright(C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#ifndef  _USBH_CONFIG_H_
#define  _USBH_CONFIG_H_

#include "NuMicro.h"
#include "hsusbh_reg.h"
#include "usbh_reg.h"

/*----------------------------------------------------------------------------------------*/
/*   Hardware settings                                                                    */
/*----------------------------------------------------------------------------------------*/

/// @cond HIDDEN_SYMBOLS

static __inline void ENABLE_OHCI_IRQ(void)
{
	IRQ_Enable(USBH0_IRQn);
	IRQ_Enable(USBH1_IRQn);
}
static __inline void DISABLE_OHCI_IRQ(void)
{
	IRQ_Disable(USBH0_IRQn);
	IRQ_Disable(USBH1_IRQn);
}
static __inline void ENABLE_EHCI_IRQ(void)
{
	IRQ_Enable(HSUSBH0_IRQn);
	IRQ_Enable(HSUSBH1_IRQn);
}
static __inline void DISABLE_EHCI_IRQ(void)
{
	IRQ_Disable(HSUSBH0_IRQn);
	IRQ_Disable(HSUSBH1_IRQn);
}

#define ENABLE_EHCI0
#define ENABLE_EHCI1
#define ENABLE_OHCI0
#define ENABLE_OHCI1

#define EHCI_PORT_CNT          1          /*!< Number of EHCI roothub ports               */
#define OHCI_PORT_CNT          1          /*!< Number of OHCI roothub ports               */
#define OHCI_PER_PORT_POWER               /*!< OHCI root hub per port powered             */

#define OHCI_ISO_DELAY         4          /*!< preserved number frames while scheduling 
                                               OHCI isochronous transfer                  */

#define EHCI_ISO_DELAY         2          /*!< preserved number of frames while 
                                               scheduling EHCI isochronous transfer       */

#define EHCI_ISO_RCLM_RANGE    32         /*!< When inspecting activated iTD/siTD, 
                                               unconditionally reclaim iTD/isTD scheduled
                                               in just elapsed EHCI_ISO_RCLM_RANGE ms.    */

/// @endcond HIDDEN_SYMBOLS

#define MAX_DESC_BUFF_SIZE     4096       /*!< To hold the configuration descriptor, USB 
                                               core will allocate a buffer with this size
                                               for each connected device. USB core does 
                                               not release it until device disconnected.  */

/*----------------------------------------------------------------------------------------*/
/*   Memory allocation settings                                                           */
/*----------------------------------------------------------------------------------------*/
#define MAX_UDEV_DRIVER        8       /*!< Maximum number of registered drivers                      */
#define MAX_ALT_PER_IFACE      8       /*!< maximum number of alternative interfaces per interface    */
#define MAX_EP_PER_IFACE       6       /*!< maximum number of endpoints per interface                 */
#define MAX_HUB_DEVICE         8       /*!< Maximum number of hub devices                             */

/* Host controller hardware transfer descriptors memory pool. ED/TD/ITD of OHCI and QH/QTD of EHCI
   are all allocated from this pool. Allocated unit size is determined by MEM_POOL_UNIT_SIZE.
   May allocate one or more units depend on hardware descriptor type.                                 */

#define HW_MEM_UNIT_SIZE       128     /*!< Fixed hard coding setting. Do not change it! (itd > 64)   */
#define HW_MEM_UNIT_NUM        512     /*!< Increase this or heap size if memory allocate failed.     */

/* USB transfer memory pool. For non-cache DMA memory allocation.                                     */

#define DMA_MEM_UNIT_SIZE      1024    /*!< A fixed hard coding setting. Do not change it!            */
#define DMA_MEM_UNIT_NUM       128     /*!< Increase this or heap size if memory allocate failed.     */

/// @cond HIDDEN_SYMBOLS

/*----------------------------------------------------------------------------------------*/
/*   Re-defined staff for various compiler                                                */
/*----------------------------------------------------------------------------------------*/
#ifdef __ICCARM__
#define   __inline    inline
#endif

/*----------------------------------------------------------------------------------------*/
/*   Debug settings                                                                       */
/*----------------------------------------------------------------------------------------*/
#define ENABLE_ERROR_MSG                    /* enable debug messages                      */
#define ENABLE_DEBUG_MSG                    /* enable debug messages                      */
//#define ENABLE_VERBOSE_DEBUG              /* verbos debug messages                      */
#define DUMP_DESCRIPTOR                     /* dump descriptors                           */

#ifdef ENABLE_ERROR_MSG
#define USB_error            sysprintf
#else
#define USB_error(...)
#endif

#ifdef ENABLE_DEBUG_MSG
#define USB_debug            sysprintf
#ifdef ENABLE_VERBOSE_DEBUG
#define USB_vdebug           sysprintf
#else
#define USB_vdebug(...)
#endif
#else
#define USB_debug(...)
#define USB_vdebug(...)
#endif

/// @endcond HIDDEN_SYMBOLS

#endif  /* _USBH_CONFIG_H_ */



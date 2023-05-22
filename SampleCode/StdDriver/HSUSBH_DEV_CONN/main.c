/**************************************************************************//**
 * @file     main.c
 *
 * @brief    This sample uses connect/disconnect callback to get aware of
 *           device connect and disconnect events. It also shows device
 *           information represented in UDEV_T.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "usbh_lib.h"

static volatile uint64_t  _start_time = 0;

void start_timer(void)
{
	_start_time = EL0_GetCurrentPhysicalValue();
}

uint32_t get_ticks(void)
{
	uint64_t   t_off;
	t_off = EL0_GetCurrentPhysicalValue() - _start_time;
	t_off = t_off / 12000;
	return (uint32_t)t_off;
}

/*
 *  This function is necessary for USB Host library.
 */
void delay_us(int usec)
{
	uint64_t   t0;
	t0  = EL0_GetCurrentPhysicalValue();
	while ((EL0_GetCurrentPhysicalValue() - t0) < (usec * 12));
}

void SYS_Init(void)
{
	/* Enable UART module clock */
	CLK_EnableModuleClock(UART0_MODULE);

	/* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
	CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

	/* Enable HSUSBH0/1 module clock */
	CLK_EnableModuleClock(HUSBH0_MODULE);
	CLK_EnableModuleClock(HUSBH1_MODULE);
	CLK_EnableModuleClock(USBD_MODULE);

	/* Set GPE multi-function pins for UART0 RXD and TXD */
	SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

	/* set UHOVRCURH(SYS_MISCFCR0[12]) 1 => USBH Host over-current detect is high-active */
	/*                                 0 => USBH Host over-current detect is low-active  */
	// SYS->MISCFCR0 |= SYS_MISCFCR0_UHOVRCURH_Msk;  /* OVCR is active-high */
	SYS->MISCFCR0 &= ~SYS_MISCFCR0_UHOVRCURH_Msk;    /* OVCR is active-low  */

	/* set PL.12 as MFP9 for HSUSBH_PWREN */
	SYS->GPL_MFPH &= ~SYS_GPL_MFPH_PL12MFP_Msk;
	SYS->GPL_MFPH |= SYS_GPL_MFPH_PL12MFP_HSUSBH_PWREN;

	/* set PL.13 as MFP9 for HSUSBH_OVC */
	SYS->GPL_MFPH &= ~SYS_GPL_MFPH_PL13MFP_Msk;
	SYS->GPL_MFPH |= SYS_GPL_MFPH_PL13MFP_HSUSBH_OVC;
}

int USB_PHY_Init(void)
{
	uint64_t t0;

	/* reset USB PHY */
	SYS->USBPMISCR = (SYS->USBPMISCR & ~0x70007) | SYS_USBPMISCR_PHY0POR_Msk |
					  SYS_USBPMISCR_PHY0SUSPEND_Msk |
					  SYS_USBPMISCR_PHY1POR_Msk | SYS_USBPMISCR_PHY1SUSPEND_Msk;

	delay_us(20000);  /* delay 20 ms */

	/* enter operation mode */
	SYS->USBPMISCR = (SYS->USBPMISCR & ~0x70007) |
					 SYS_USBPMISCR_PHY0SUSPEND_Msk | SYS_USBPMISCR_PHY1SUSPEND_Msk;
	t0 = get_ticks();
	while (1) {
		delay_us(10);
		if ((SYS->USBPMISCR & SYS_USBPMISCR_PHY0HSTCKSTB_Msk) &&
			(SYS->USBPMISCR & SYS_USBPMISCR_PHY1HSTCKSTB_Msk))
			break;   /* both USB PHY0 and PHY1 clock 60MHz UTMI clock stable */

		if (get_ticks() - t0 > 500) {
			sysprintf("USB PHY reset failed. USBPMISCR = 0x%x\n", SYS->USBPMISCR);
			return -1;
		}
	}
	sysprintf("USBPMISCR = 0x%x\n", SYS->USBPMISCR);
	return 0;
}

void UART0_Init()
{
	/* Configure UART0 and set UART0 baud rate */
	UART_Open(UART0, 115200);
}

/*
 *  USB device connect callback function.
 *  User invokes usbh_pooling_hubs() to let USB core able to scan and handle events of
 *  HSUSBH port, USBH port, and USB hub device ports. Once a new device connected, it
 *  will be detected and enumerated in the call to usbh_pooling_hubs(). This callback
 *  will be invoked from USB core once a newly connected device was successfully enumerated.
 */
void  connect_func(struct udev_t *udev, int param)
{
	struct hub_dev_t *parent;
	int    i;

	parent = udev->parent;

	sysprintf("Device [0x%x,0x%x] was connected.\n",
		   udev->descriptor.idVendor, udev->descriptor.idProduct);
	sysprintf("    Speed:    %s-speed\n", (udev->speed == SPEED_HIGH) ? "high" : ((udev->speed == SPEED_FULL) ? "full" : "low"));
	sysprintf("    Location: ");

	if (parent == NULL)
	{
		if (udev->port_num == 1)
			sysprintf("USB 2.0 port\n");
		else
			sysprintf("USB 1.1 port\n");
	}
	else
	{
		if (parent->pos_id[0] == '1')
			sysprintf("USB 2.0 port");
		else
			sysprintf("USB 1.1 port");

		for (i = 1; parent->pos_id[i] != 0; i++)
		{
			sysprintf(" => Hub port %c", parent->pos_id[i]);
		}

		sysprintf(" => Hub port %d\n", udev->port_num);

		sysprintf("\n");
	}
	sysprintf("\n");
}


/*
 *  USB device disconnect callback function.
 *  User invokes usbh_pooling_hubs() to let USB core able to scan and handle events of
 *  HSUSBH port, USBH port, and USB hub device ports. Once a device was disconnected, it
 *  will be detected and removed in the call to usbh_pooling_hubs(). This callback
 *  will be invoked from USB core prior to remove that device.
 */
void  disconnect_func(struct udev_t *udev, int param)
{
	sysprintf("Device [0x%x,0x%x] was connected.\n",
		   udev->descriptor.idVendor, udev->descriptor.idProduct);
}

int32_t main(void)
{
	/* Unlock protected registers */
	SYS_UnlockReg();

	/* Init System, IP clock and multi-function I/O */
	SYS_Init();

	/* Init UART0 for sysprintf */
	UART0_Init();

	start_timer();

	if (USB_PHY_Init() != 0) {
		sysprintf("USB PHY Init failed!\n");
		while (1);
	}

	sysprintf("\n\n");
	sysprintf("+-----------------------------------------------+\n");
	sysprintf("|                                               |\n");
	sysprintf("|  USB Host Connect/Disconnect callback demo    |\n");
	sysprintf("|                                               |\n");
	sysprintf("+-----------------------------------------------+\n");

	usbh_core_init();

	usbh_install_conn_callback(connect_func, disconnect_func);

	while (1)
	{
		if (usbh_pooling_hubs())             /* USB Host port detect polling and management */
		{
			usbh_memory_used();              /* print out USB memory allocating information */
		}
	}
}

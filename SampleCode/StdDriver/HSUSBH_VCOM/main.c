/**************************************************************************//**
 * @file     main.c
 * @brief    Use USB Host core driver and CDC driver. This sample demonstrates how
 *           to connect a CDC class VCOM device.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "usbh_lib.h"
#include "usbh_cdc.h"

char Line[64];             /* Console input buffer */

static volatile int  g_rx_ready = 0;

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

void  dump_buff_hex(uint8_t *pucBuff, int nBytes)
{
	int     nIdx, i;

	nIdx = 0;
	while (nBytes > 0)
	{
		sysprintf("0x%04X  ", nIdx);
		for (i = 0; (i < 16) && (nBytes > 0); i++)
		{
			sysprintf("%02x ", pucBuff[nIdx + i]);
			nBytes--;
		}
		nIdx += 16;
		sysprintf("\n");
	}
	sysprintf("\n");
}

void  vcom_status_callback(CDC_DEV_T *cdev, uint8_t *rdata, int data_len)
{
	int  i;
	sysprintf("[VCOM STS] ");
	for(i = 0; i < data_len; i++)
		sysprintf("0x%02x ", rdata[i]);
	sysprintf("\n");
}

void  vcom_rx_callback(CDC_DEV_T *cdev, uint8_t *rdata, int data_len)
{
	int  i;

	sysprintf("[VCOM DATA %d] ", data_len);
	for (i = 0; i < data_len; i++)
	{
		sysprintf("0x%02x ", rdata[i]);
		sysprintf("%c", rdata[i]);
	}
	sysprintf("\n");

	g_rx_ready = 1;
}

void show_line_coding(LINE_CODING_T *lc)
{
	sysprintf("[CDC device line coding]\n");
	sysprintf("====================================\n");
	sysprintf("Baud rate:  %d bps\n", lc->baud);
	sysprintf("Parity:     ");
	switch (lc->parity)
	{
	case 0:
		sysprintf("None\n");
		break;
	case 1:
		sysprintf("Odd\n");
		break;
	case 2:
		sysprintf("Even\n");
		break;
	case 3:
		sysprintf("Mark\n");
		break;
	case 4:
		sysprintf("Space\n");
		break;
	default:
		sysprintf("Invalid!\n");
		break;
	}
	sysprintf("Data Bits:  ");
	switch (lc->data_bits)
	{
	case 5 :
	case 6 :
	case 7 :
	case 8 :
	case 16:
		sysprintf("%d\n", lc->data_bits);
		break;
	default:
		sysprintf("Invalid!\n");
		break;
	}
	sysprintf("Stop Bits:  %s\n\n", (lc->stop_bits == 0) ? "1" : ((lc->stop_bits == 1) ? "1.5" : "2"));
}

int  init_cdc_device(CDC_DEV_T *cdev)
{
	int     ret;
	LINE_CODING_T  line_code;

	sysprintf("\n\n==================================\n");
	sysprintf("  Init CDC device : 0x%x\n", ptr_to_u32(cdev));
	sysprintf("  VID: 0x%x, PID: 0x%x\n\n", cdev->udev->descriptor.idVendor, cdev->udev->descriptor.idProduct);

	ret = usbh_cdc_get_line_coding(cdev, &line_code);
	if (ret < 0)
	{
		sysprintf("Get Line Coding command failed: %d\n", ret);
	}
	else
		show_line_coding(&line_code);

	line_code.baud = 115200;
	line_code.parity = 0;
	line_code.data_bits = 8;
	line_code.stop_bits = 0;

	ret = usbh_cdc_set_line_coding(cdev, &line_code);
	if (ret < 0)
	{
		sysprintf("Set Line Coding command failed: %d\n", ret);
	}

	ret = usbh_cdc_get_line_coding(cdev, &line_code);
	if (ret < 0)
	{
		sysprintf("Get Line Coding command failed: %d\n", ret);
	}
	else
	{
		sysprintf("New line coding =>\n");
		show_line_coding(&line_code);
	}

	usbh_cdc_set_control_line_state(cdev, 1, 1);

	sysprintf("usbh_cdc_start_polling_status...\n");
	usbh_cdc_start_polling_status(cdev, vcom_status_callback);

	sysprintf("usbh_cdc_start_to_receive_data...\n");
	usbh_cdc_start_to_receive_data(cdev, vcom_rx_callback);

	return 0;
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int32_t main(void)
{
	CDC_DEV_T   *cdev;
	int         ret;

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

	sysprintf("+---------------------------------------------------------+\n");
	sysprintf("|           M480 USB Host VCOM sample program             |\n");
	sysprintf("+---------------------------------------------------------+\n");
	sysprintf("|  (NOTE: This sample supports only one CDC device, but   |\n");
	sysprintf("|         driver supports multiple CDC devices. If you    |\n");
	sysprintf("|         want to support multiple CDC devices, you       |\n");
	sysprintf("|         have to modify this sample.                     |\n");
	sysprintf("+---------------------------------------------------------+\n");

	usbh_core_init();
	usbh_cdc_init();
	usbh_memory_used();

	while(1)
	{
		if (usbh_pooling_hubs())             /* USB Host port detect polling and management */
		{
			usbh_memory_used();              /* print out USB memory allocating information */

			cdev = usbh_cdc_get_device_list();
			if (cdev == NULL)
				continue;

			while (cdev != NULL)
			{
				init_cdc_device(cdev);

				if (cdev != NULL)
					cdev = cdev->next;
			}
		}

		cdev = usbh_cdc_get_device_list();
		if (cdev == NULL)
			continue;

		if (g_rx_ready)
		{
			g_rx_ready = 0;

			if (cdev->rx_busy == 0)
				usbh_cdc_start_to_receive_data(cdev, vcom_rx_callback);
		}

		/*
		 *  Check user input and send to CDC device immediately
		 *  (You can also modify it send multiple characters at one time.)
		 */
		if (sysIsKbHit() == 0)
		{
			Line[0] = sysgetchar();
			ret = usbh_cdc_send_data(cdev, (uint8_t *)Line, 1);
			if (ret != 0)
				sysprintf("\n!! Send data failed, 0x%x!\n", ret);
		}
	}
}

/**************************************************************************//**
 * @file     main.c
 * @brief    Use USB Host core driver and HID driver. This sample demonstrates how
 *           to support mouse and keyboard input.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "usbh_lib.h"
#include "usbh_uac.h"

HID_DEV_T   *g_hid_list[CONFIG_HID_MAX_DEV];

extern void keycode_process(KEYBOARD_EVENT_T *kbd);

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

int  is_a_new_hid_device(HID_DEV_T *hdev)
{
    int    i;
    for (i = 0; i < CONFIG_HID_MAX_DEV; i++)
    {
        if ((g_hid_list[i] != NULL) && (g_hid_list[i] == hdev) &&
                (g_hid_list[i]->uid == hdev->uid))
            return 0;
    }
    return 1;
}

void update_hid_device_list(HID_DEV_T *hdev)
{
    int  i = 0;
    memset(g_hid_list, 0, sizeof(g_hid_list));
    while ((i < CONFIG_HID_MAX_DEV) && (hdev != NULL))
    {
        g_hid_list[i++] = hdev;
        hdev = hdev->next;
    }
}

void  int_read_callback(HID_DEV_T *hdev, uint16_t ep_addr, int status, uint8_t *rdata, uint32_t data_len)
{
    /* This callback is in interrupt context! */
    /*
     *  USB host HID driver notify user the transfer status via <status> parameter. If the
     *  If <status> is 0, the USB transfer is fine. If <status> is not zero, this interrupt in
     *  transfer failed and HID driver will stop this pipe. It can be caused by USB transfer error
     *  or device disconnected.
     */
    if (status < 0)
    {
        sysprintf("Interrupt in transfer failed! status: %d\n", status);
        return;
    }
    sysprintf("Device [0x%x,0x%x] ep 0x%x, %d bytes received =>\n",
           hdev->idVendor, hdev->idProduct, ep_addr, data_len);
    dump_buff_hex(rdata, data_len);
}

int  init_hid_device(HID_DEV_T *hdev)
{
    int     ret;

    sysprintf("\n\n==================================\n");
    sysprintf("  Init HID device : 0x%x\n", ptr_to_u32(hdev));
    sysprintf("  VID: 0x%x, PID: 0x%x\n\n", hdev->idVendor, hdev->idProduct);

    sysprintf("  bSubClassCode: 0x%x, bProtocolCode: 0x%x\n\n", hdev->bSubClassCode, hdev->bProtocolCode);
    //ret = usbh_hid_set_idle(hdev, 0, 0);
    //if (ret != HID_RET_OK)
    //  sysprintf("HID Set_Idle command failed! %d\n", ret);

    sysprintf("\nUSBH_HidStartIntReadPipe...\n");
    ret = usbh_hid_start_int_read(hdev, 0, int_read_callback);
    if (ret != HID_RET_OK)
        sysprintf("usbh_hid_start_int_read failed! %d\n", ret);
    else
        sysprintf("Interrupt in transfer started...\n");
    return 0;
}

/*
 *  Mouse callback function
 */
void mouse_callback(struct usbhid_dev *hdev, MOUSE_EVENT_T *mouse)
{
    /* This callback is in interrupt context! */

    sysprintf("X: %d, Y: %d, W: %d, button: 0x%x\n", mouse->X, mouse->Y, mouse->wheel, mouse->button_map);
}

/*
 *  Keyboard callback function
 */
void keyboard_callback(struct usbhid_dev *hdev, KEYBOARD_EVENT_T *kbd)
{
    /* This callback is in interrupt context! */

    //int   i;
    //sysprintf("[0x%x] ", kbd->modifier);
    //for (i = 0; i < kbd->key_cnt; i++)
    //  sysprintf("%x ", kbd->keycode[i]);
    //sysprintf("(0x%x)\n", kbd->lock_state);
    keycode_process(kbd);
}

int32_t main(void)
{
    HID_DEV_T    *hdev, *hdev_list;

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
    sysprintf("+-------------------------------------------+\n");
    sysprintf("|                                           |\n");
    sysprintf("|     USB Host HID class sample demo        |\n");
    sysprintf("|                                           |\n");
    sysprintf("+-------------------------------------------+\n");

    usbh_core_init();
    usbh_hid_init();
    usbh_memory_used();

    usbh_hid_regitser_mouse_callback(mouse_callback);
    usbh_hid_regitser_keyboard_callback(keyboard_callback);

    memset(g_hid_list, 0, sizeof(g_hid_list));

    while (1)
    {
        if (usbh_pooling_hubs())             /* USB Host port detect polling and management */
        {
            usbh_memory_used();              /* print out USB memory allocating information */

            sysprintf("\n Has hub events.\n");
            hdev_list = usbh_hid_get_device_list();
            hdev = hdev_list;
            while (hdev != NULL)
            {
                if (is_a_new_hid_device(hdev))
                {
                    init_hid_device(hdev);
                }
                hdev = hdev->next;
            }

            update_hid_device_list(hdev_list);
            usbh_memory_used();
        }
        if (sysIsKbHit())
        {
            sysgetchar();
            usbh_memory_used();
        }
    }
}

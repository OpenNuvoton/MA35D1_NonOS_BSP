/*************************************************************************//**
 * @file     ma35d1_mac.h
 * @brief    Packet processor header file for MA35D1
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright(C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __MA35D1_MAC_H__
#define __MA35D1_MAC_H__

#include "NuMicro.h"
#include <string.h>
#include <stdio.h>

/******************************************************************************
 * User configuration
 * NuMaker SOM MA35D1 :
 *  - GMACINTF0 + RGMII_1G
 *  - GMACINTF1 + RGMII_1G
 * Numaker IOT MA35D1 :
 *  - GMACINTF0 + RGMII_1G
 *  - GMACINTF1 + RMII_100M
 ******************************************************************************/
#define GMAC_INTF    GMACINTF0

#define GMAC_MODE    RGMII_1G // mii mode supported by local PHY

#define DEFAULT_MAC0_ADDRESS {0x00, 0x11, 0x22, 0x33, 0x44, 0x55}
#define DEFAULT_MAC1_ADDRESS {0x00, 0x11, 0x22, 0x33, 0x44, 0x66}

/******************************************************************************
 * Functions
 ******************************************************************************/
s32 GMAC_setup_tx_desc_queue(GMACdevice *gmacdev, u32 no_of_desc, u32 desc_mode);
s32 GMAC_setup_rx_desc_queue(GMACdevice *gmacdev, u32 no_of_desc, u32 desc_mode);
s32 GMAC_set_mode(GMACdevice *gmacdev);
s32 GMAC_open(int intf, int mode);
s32 GMAC_register_interrupt(int intf);
void GMAC_giveup_rx_desc_queue(GMACdevice *gmacdev, u32 desc_mode);
void GMAC_giveup_tx_desc_queue(GMACdevice *gmacdev, u32 desc_mode);
s32 GMAC_close(int intf);
s32 GMAC_xmit_frames(struct sk_buff *skb, int intf, u32 offload_needed, u32 ts);
void GMAC_handle_transmit_over(int intf);
uint32_t GMAC_handle_received_data(int intf, struct sk_buff *prskb);
static void GMAC_powerup_mac(GMACdevice *gmacdev);
static void GMAC_powerdown_mac(GMACdevice *gmacdev);
uint32_t GMAC_int_handler0(struct sk_buff *prskb);
uint32_t GMAC_int_handler1(struct sk_buff *prskb);

extern GMACdevice GMACdev[];
extern u8 mac_addr0[];
extern u8 mac_addr1[];
extern struct sk_buff txbuf[];
extern struct sk_buff rxbuf[];

#endif /* __MA35D1_MAC_H__ */

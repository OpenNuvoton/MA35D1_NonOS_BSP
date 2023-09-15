/**************************************************************************//**
 * @file     ethernetif.h
 * @brief    Ethernet interface header
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "lwip/netif.h"

err_t ethernetif_init0(struct netif *netif);
err_t ethernetif_init1(struct netif *netif);
void ethernetif_input0(uint32_t packetCnt);
void ethernetif_input1(uint32_t packetCnt);
void GMAC0_IRQHandler(void);
void GMAC1_IRQHandler(void);
int32_t GMAC0_TransmitPkt(struct sk_buff *ptskb, uint8_t *pbuf, uint32_t len);
int32_t GMAC1_TransmitPkt(struct sk_buff *ptskb, uint8_t *pbuf, uint32_t len);

#endif

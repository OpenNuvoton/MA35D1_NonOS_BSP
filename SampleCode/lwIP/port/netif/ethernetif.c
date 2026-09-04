/*************************************************************************//**
 * @file     ethernetif.c
 * @brief    lwIP ethernet interface porting for MA35D1 GMAC0/1.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright(C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "netif/ethernetif.h"
#include "string.h"
#include "lwipopts.h"
#ifdef HAVE_AVTP
#include "rtp_h264.h"
#endif

/* Define those to better describe your network interface. */
#define IFNAME  'e'
#define IFNAME0 '0'
#define IFNAME1 '1'

struct netif *_netif0;
struct netif *_netif1;

/* Fired by GMAC Rx interrupt. This is greedy so just keep medium priority */
#define GMAC_LWIP_RX_PRIORITY   (tskIDLE_PRIORITY + 1)
#define GMAC_LWIP_RX_STACKSIZE  (1024)

static struct sk_buff rxskbuf[GMAC_CNT][RECEIVE_DESC_SIZE];

extern u8_t mac_addr0[6];
extern u8_t mac_addr1[6];
extern struct sk_buff txbuf[GMAC_CNT][TRANSMIT_DESC_SIZE];

static TaskHandle_t post_rx_task[GMAC_CNT] = {NULL, NULL};

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif
{
    struct eth_addr *ethaddr;
    /* Add whatever per-interface state that is needed here. */
};

#ifdef HAVE_AVTP
#define ETHERTYPE_AVTP 0x22F0
#define AVTP_RX_PRINT_INTERVAL 90000000U
#define AVTP_HEADER_SIZE 24U
#define AVTP_CVF_H264_HEADER_SIZE 4U
static uint32_t avtp_rx_packet_count;
static uint32_t avtp_rx_byte_count;
static uint32_t avtp_rx_valid_count;
static uint32_t avtp_rx_invalid_count;
static uint32_t avtp_rx_marker_count;
static uint32_t avtp_rx_h264_packet_count;
static uint32_t avtp_rx_h264_byte_count;
static uint32_t avtp_rx_ring_error_count;
static const uint8_t avtp_expected_stream_id[8] =
    {0x45, 0x4D, 0x4F, 0x53, 0x89, 0xAB, 0xCD, 0xEF};
#endif

void notify_rx_task(int intf)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(xTaskGetSchedulerState() != taskSCHEDULER_RUNNING)
        return;

    /* Guard: ethernetif_initX() enables the IRQ inside low_level_initX()
     * BEFORE the RX task is created via xTaskCreate().  If a GMAC interrupt
     * fires in that narrow window, post_rx_task[intf] is still NULL.
     * Passing NULL to vTaskNotifyGiveFromISR() would assert/crash, so
     * silently drop the notification here; the task will drain any pending
     * descriptors on its very first wakeup. */
    if (post_rx_task[intf] == NULL)
        return;

    vTaskNotifyGiveFromISR(post_rx_task[intf], &xHigherPriorityTaskWoken);
    /* Force context switch immediately (risky for scheduler) */
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void GMAC0_IRQHandler(void)
{
    GMAC_int_handler0();
}

void gmac0_lwip_rx(void *arg)
{
    struct sk_buff *rskb = rxskbuf[GMACINTF0];
    uint32_t packetCnt;

    for (;;)
    {
        /* Block until IRQ notifies */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        packetCnt = GMAC_handle_received_data(GMACINTF0, rskb);

        ethernetif_input0(packetCnt);
    }
}

void GMAC1_IRQHandler(void)
{
    GMAC_int_handler1();
}

void gmac1_lwip_rx(void *arg)
{
    struct sk_buff *rskb = rxskbuf[GMACINTF1];
    uint32_t packetCnt;

    for (;;)
    {
        /* Block until IRQ notifies */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        packetCnt = GMAC_handle_received_data(GMACINTF1, rskb);

        ethernetif_input1(packetCnt);
    }
}

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_init0(struct netif *netif)
{
    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
    memcpy(netif->hwaddr, mac_addr0, netif->hwaddr_len);

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
#ifdef LWIP_IGMP
    netif->flags |= NETIF_FLAG_IGMP;
#endif

    GMAC_open(GMACINTF0, GMAC_MODE);
    /* we will call interrupt safe API, the priority must be at or below configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY */
    IRQ_SetPriority((IRQn_ID_t)GMAC0_IRQn, (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1) << portPRIORITY_SHIFT);
    IRQ_SetHandler(GMAC0_IRQn, GMAC0_IRQHandler);
    IRQ_SetTarget(GMAC0_IRQn, 0x1 << cpuid());
    IRQ_Enable(GMAC0_IRQn);
}

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_init1(struct netif *netif)
{
    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
    memcpy(netif->hwaddr, mac_addr1, netif->hwaddr_len);

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
#ifdef LWIP_IGMP
    netif->flags |= NETIF_FLAG_IGMP;
#endif

    GMAC_open(GMACINTF1, GMAC_MODE);
    /* we will call interrupt safe API, the priority must be at or below configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY */
    IRQ_SetPriority((IRQn_ID_t)GMAC1_IRQn, (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1) << portPRIORITY_SHIFT);
    IRQ_SetHandler(GMAC1_IRQn, GMAC1_IRQHandler);
    IRQ_SetTarget(GMAC1_IRQn, 0x1 << cpuid());
    IRQ_Enable(GMAC1_IRQn);
}

int32_t GMAC0_TransmitPkt(struct sk_buff *ptskb, uint8_t *pbuf, uint32_t len)
{
    struct sk_buff *tskb;

#if (LWIP_USING_HW_CHECKSUM == 1)
    u32 offload_needed = 1;
#else
    u32 offload_needed = 0;
#endif

    if(ptskb == NULL)
    {
        tskb = (struct sk_buff *)((uint64_t)&txbuf[GMACINTF0][0] | NON_CACHE);

        tskb->len = len;
        memcpy((uint8_t *)((u64)(tskb->data)), pbuf, len);
        return GMAC_xmit_frames(tskb, GMACINTF0, offload_needed, 0);
    }
    else
    {
        ptskb->len = len;
        return GMAC_xmit_frames(ptskb, GMACINTF0, offload_needed, 0);
    }
}

int32_t GMAC1_TransmitPkt(struct sk_buff *ptskb, uint8_t *pbuf, uint32_t len)
{
    struct sk_buff *tskb;

#if (LWIP_USING_HW_CHECKSUM == 1)
    u32 offload_needed = 1;
#else
    u32 offload_needed = 0;
#endif

    if(ptskb == NULL)
    {
        tskb = (struct sk_buff *)((uint64_t)&txbuf[GMACINTF1][0] | NON_CACHE);

        tskb->len = len;
        memcpy((uint8_t *)((u64)(tskb->data)), pbuf, len);
        return GMAC_xmit_frames(tskb, GMACINTF1, offload_needed, 0);
    }
    else
    {
        ptskb->len = len;
        return GMAC_xmit_frames(ptskb, GMACINTF1, offload_needed, 0);
    }
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
static err_t
low_level_output0(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;
    u8_t *buf = NULL;
    u16_t len = 0;

    /* Select the TX buffer that corresponds to the next free descriptor.
     * txbuf[GMACINTF0][tx_idx] is only used by descriptor tx_idx, so
     * it cannot alias any buffer the DMA is currently reading. */
    u32 tx_idx = GMACdev[GMACINTF0].TxNext;

    /* If the ring is full (all TRANSMIT_DESC_SIZE descriptors are
     * DMA-owned) descriptor tx_idx is still in use.  Writing to its buffer
     * would corrupt the outgoing frame.  Return ERR_MEM so lwIP can retry
     * via the TCP retransmit timer.  In normal operation (TCP window <<
     * TRANSMIT_DESC_SIZE) this branch is never taken. */
    if (*(volatile u32 *)&GMACdev[GMACINTF0].BusyTxDesc >= TRANSMIT_DESC_SIZE) {
        LINK_STATS_INC(link.drop);
        return ERR_MEM;
    }

    struct sk_buff *tskb = (struct sk_buff *)((uint64_t)&txbuf[GMACINTF0][tx_idx] | NON_CACHE);

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    buf = (u8_t*)tskb->data;
    for(q = p; q != NULL; q = q->next)
    {
        memcpy((u8_t*)&buf[len], q->payload, q->len);
        len = len + q->len;
    }
    GMAC0_TransmitPkt(tskb, NULL, len);

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.xmit);

    return ERR_OK;
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
static err_t
low_level_output1(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;
    u8_t *buf = NULL;
    u16_t len = 0;

    /* Same per-descriptor buffer scheme as low_level_output0. */
    u32 tx_idx = GMACdev[GMACINTF1].TxNext;

    if (*(volatile u32 *)&GMACdev[GMACINTF1].BusyTxDesc >= TRANSMIT_DESC_SIZE) {
        LINK_STATS_INC(link.drop);
        return ERR_MEM;
    }

    struct sk_buff *tskb = (struct sk_buff *)((uint64_t)&txbuf[GMACINTF1][tx_idx] | NON_CACHE);

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    buf = (u8_t*)tskb->data;
    for(q = p; q != NULL; q = q->next)
    {
        memcpy((u8_t*)&buf[len], q->payload, q->len);
        len = len + q->len;
    }
    GMAC1_TransmitPkt(tskb, NULL, len);

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.xmit);

    return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif, u16_t len, u8_t *buf)
{
    struct pbuf *p, *q;
    u16_t offset;

#if ETH_PAD_SIZE
    len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

    if (p != NULL)
    {
#if ETH_PAD_SIZE
        pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

        /* Copy data from the DMA RX buffer into the pbuf's own memory.
         *
         * GMAC_get_rx_qptr() re-arms the DMA descriptor
         * (sets rxdesc->status = DescOwnByDma) immediately after reading
         * it.  This means the hardware can write a NEW incoming packet
         * into the same buffer before tcpip_thread gets to process this
         * pbuf.  A zero-copy pointer (p->payload = buf) would expose that
         * race and corrupt TCP/IP headers.  Copy here to be safe. */
        offset = 0;
        for (q = p; q != NULL; q = q->next)
        {
            memcpy(q->payload, buf + offset, q->len);
            offset += q->len;
        }

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

        LINK_STATS_INC(link.recv);
    }
    else
    {
        // do nothing. drop the packet
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
    }

    return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void
ethernetif_input0(uint32_t packetCnt)
{
    struct eth_hdr *ethhdr;
    struct pbuf *p;
    u16_t i;

    for(i = 0; i < packetCnt; i++) {
        /* move received packet into a new pbuf */
#if (LWIP_USING_HW_CHECKSUM == 1)
        p = low_level_input(_netif0, rxskbuf[GMACINTF0][i].len, rxskbuf[GMACINTF0][i].pData);
#else
        p = low_level_input(_netif0, rxskbuf[GMACINTF0][i].len + 4, rxskbuf[GMACINTF0][i].pData);
#endif
        /* no packet could be read, silently ignore this */
        if (p == NULL) continue;

        /* points to packet payload, which starts with an Ethernet header */
        ethhdr = p->payload;

#ifdef HAVE_AVTP
        uint16_t ether_type = htons(ethhdr->type);

        if (ether_type == ETHERTYPE_AVTP)
        {
            uint32_t packet_length = rxskbuf[GMACINTF0][i].len;
            uint32_t ethernet_header_length = sizeof(struct eth_hdr);
            uint32_t avtp_length = 0U;
            uint8_t *avtp = (uint8_t *)ethhdr;
            uint32_t packet_info;
            uint32_t stream_data_length;
            uint32_t h264_payload_length = 0U;
            uint8_t marker = 0U;
            uint8_t nal_type = 0U;
            uint8_t *h264_payload = NULL;
            int avtp_valid = 1;

            avtp_rx_packet_count++;
            avtp_rx_byte_count += packet_length;

            if (packet_length < ethernet_header_length + AVTP_HEADER_SIZE)
            {
                avtp_valid = 0;
            }
            else
            {
                avtp_length = packet_length - ethernet_header_length;
                avtp = (uint8_t *)ethhdr + ethernet_header_length;
                packet_info = ((uint32_t)avtp[20] << 24) |
                              ((uint32_t)avtp[21] << 16) |
                              ((uint32_t)avtp[22] << 8) |
                              avtp[23];
                stream_data_length = (packet_info >> 16) & 0xFFFFU;
                marker = (uint8_t)((packet_info >> 12) & 0x01U);

                if (((avtp[0] & 0x7FU) != 0x03U) ||
                    (memcmp(&avtp[4], avtp_expected_stream_id, 8) != 0) ||
                    (avtp[16] != 0x02U) ||
                    (avtp[17] != 0x01U) ||
                    (stream_data_length < AVTP_CVF_H264_HEADER_SIZE) ||
                    (stream_data_length > avtp_length - AVTP_HEADER_SIZE))
                {
                    avtp_valid = 0;
                }
                else
                {
                    h264_payload = avtp + AVTP_HEADER_SIZE +
                                   AVTP_CVF_H264_HEADER_SIZE;
                    h264_payload_length = stream_data_length -
                                          AVTP_CVF_H264_HEADER_SIZE;
                    avtp_rx_h264_packet_count++;
                    avtp_rx_h264_byte_count += h264_payload_length;
                    avtp_rx_marker_count += marker;
                    if (h264_payload_length > 0U)
                        nal_type = h264_payload[0] & 0x1FU;

                    if ((g_avtp_ringbuf == NULL) ||
                        (h264_cvf_to_annexb(g_avtp_ringbuf,
                                            h264_payload,
                                            h264_payload_length,
                                            avtp[2]) != 0))
                    {
                        avtp_rx_ring_error_count++;
                    }
                }
            }

            if (avtp_valid)
            {
                avtp_rx_valid_count++;
            }
            else
            {
                avtp_rx_invalid_count++;
            }

            if (avtp_rx_packet_count == 1U && avtp_valid)
            {
                sysprintf("AVTP first: len=%lu subtype=%u seq=%u sid=%02X%02X%02X%02X%02X%02X%02X%02X data=%lu h264=%lu fmt=%u/%u marker=%u nal=%u\n",
                          (unsigned long)packet_length,
                          avtp[0] & 0x7FU, avtp[2],
                          avtp[4], avtp[5], avtp[6], avtp[7],
                          avtp[8], avtp[9], avtp[10], avtp[11],
                          (unsigned long)stream_data_length,
                          (unsigned long)h264_payload_length,
                          avtp[16], avtp[17], marker, nal_type);
            }
            else if ((avtp_rx_packet_count % AVTP_RX_PRINT_INTERVAL) == 0U)
            {
                sysprintf("AVTP stats: packets=%lu valid=%lu invalid=%lu bytes=%lu h264_packets=%lu h264=%lu frames=%lu ring_err=%lu\n",
                          (unsigned long)avtp_rx_packet_count,
                          (unsigned long)avtp_rx_valid_count,
                          (unsigned long)avtp_rx_invalid_count,
                          (unsigned long)avtp_rx_byte_count,
                          (unsigned long)avtp_rx_h264_packet_count,
                          (unsigned long)avtp_rx_h264_byte_count,
                          (unsigned long)avtp_rx_marker_count,
                          (unsigned long)avtp_rx_ring_error_count);
            }

            pbuf_free(p);
            continue;
        }
#else
        uint16_t ether_type = htons(ethhdr->type);
#endif

        switch (ether_type)
        {
        /* IP or ARP packet? */
        case ETHTYPE_IP:
        case ETHTYPE_ARP:
    #if PPPOE_SUPPORT
        /* PPPoE packet? */
        case ETHTYPE_PPPOEDISC:
        case ETHTYPE_PPPOE:
    #endif /* PPPOE_SUPPORT */
            /* full packet send to tcpip_thread to process */
            if (_netif0->input(p, _netif0)!=ERR_OK)
            {
                LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
                pbuf_free(p);
                p = NULL;
            }
            break;

        default:
            pbuf_free(p);
            p = NULL;
            break;
        }
    }
}


/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void
ethernetif_input1(uint32_t packetCnt)
{
    struct eth_hdr *ethhdr;
    struct pbuf *p;
    u16_t i;

    for(i = 0; i < packetCnt; i++) {
        /* move received packet into a new pbuf */
#if (LWIP_USING_HW_CHECKSUM == 1)
        p = low_level_input(_netif1, rxskbuf[GMACINTF1][i].len, rxskbuf[GMACINTF1][i].pData);
#else
        p = low_level_input(_netif1, rxskbuf[GMACINTF1][i].len + 4, rxskbuf[GMACINTF1][i].pData);
#endif
        /* no packet could be read, silently ignore this */
        if (p == NULL) continue;

        /* points to packet payload, which starts with an Ethernet header */
        ethhdr = p->payload;

        switch (htons(ethhdr->type))
        {
        /* IP or ARP packet? */
        case ETHTYPE_IP:
        case ETHTYPE_ARP:
    #if PPPOE_SUPPORT
        /* PPPoE packet? */
        case ETHTYPE_PPPOEDISC:
        case ETHTYPE_PPPOE:
    #endif /* PPPOE_SUPPORT */
            /* full packet send to tcpip_thread to process */
            if (_netif1->input(p, _netif1)!=ERR_OK)
            {
                LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
                pbuf_free(p);
                p = NULL;
            }
            break;

        default:
            pbuf_free(p);
            p = NULL;
            break;
        }
    }
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init0(struct netif *netif)
{
    struct ethernetif *ethernetif;

    LWIP_ASSERT("netif != NULL", (netif != NULL));

    _netif0 = netif;

    ethernetif = mem_malloc(sizeof(struct ethernetif));
    if (ethernetif == NULL) {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
        return ERR_MEM;
    }

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "ma35d1";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    netif->state = ethernetif;
    netif->name[0] = IFNAME;
    netif->name[1] = IFNAME0;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = low_level_output0;

    ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

    /* initialize the hardware */
    low_level_init0(netif);

    xTaskCreate(gmac0_lwip_rx,
            "gmac0-lwip-rx",
            GMAC_LWIP_RX_STACKSIZE,
            NULL,
            GMAC_LWIP_RX_PRIORITY,
            &post_rx_task[GMACINTF0]);

#if ( configNUMBER_OF_CORES > 1 ) && ( configUSE_CORE_AFFINITY == 1 )
    vTaskCoreAffinitySet(post_rx_task[GMACINTF0], 0x01);
#endif

    return ERR_OK;
}


/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init1(struct netif *netif)
{
    struct ethernetif *ethernetif;

    LWIP_ASSERT("netif != NULL", (netif != NULL));

    _netif1 = netif;

    ethernetif = mem_malloc(sizeof(struct ethernetif));
    if (ethernetif == NULL) {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
        return ERR_MEM;
    }

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "ma35d1";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    netif->state = ethernetif;
    netif->name[0] = IFNAME;
    netif->name[1] = IFNAME1;

    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = low_level_output1;

    ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

    /* initialize the hardware */
    low_level_init1(netif);

    xTaskCreate(gmac1_lwip_rx,
            "gmac1-lwip-rx",
            GMAC_LWIP_RX_STACKSIZE,
            NULL,
            GMAC_LWIP_RX_PRIORITY,
            &post_rx_task[GMACINTF1]);

#if ( configNUMBER_OF_CORES > 1 ) && ( configUSE_CORE_AFFINITY == 1 )
    vTaskCoreAffinitySet(post_rx_task[GMACINTF1], 0x01);
#endif

    return ERR_OK;
}

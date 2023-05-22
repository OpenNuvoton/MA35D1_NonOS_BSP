/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

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
#include "string.h"
#include "lwipopts.h"

/* Define those to better describe your network interface. */
#define IFNAME  'e'
#define IFNAME0 '0'
#define IFNAME1 '1'

struct netif *_netif0;
struct netif *_netif1;

extern u8_t mac_addr0[6];
extern u8_t mac_addr1[6];
extern struct sk_buff txbuf[GMAC_CNT];
extern struct sk_buff rxbuf[GMAC_CNT];

extern uint32_t queue_try_put(struct pbuf *p);

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

uint32_t GMAC0_ReceivePkt(struct sk_buff *prskb)
{
    prskb->rdy = 0;
    prskb->len = 0;

    GMAC_int_handler0();

    return prskb->len;
}

void GMAC0_IRQHandler(void)
{
    struct sk_buff *rskb = &rxbuf[GMACINTF0];

    if(GMAC0_ReceivePkt(rskb) != 0)
    {
        /* Allocate pbuf from pool (avoid using heap in interrupts) */
        struct pbuf* p = pbuf_alloc(PBUF_RAW, rskb->len, PBUF_POOL);

        if(p != NULL)
        {
            /* Copy ethernet frame into pbuf */
            pbuf_take(p, rskb->data, rskb->len);

            /* Put in a queue which is processed in main loop */
            if(!queue_try_put(p))
            {
                /* queue is full -> packet loss */
                pbuf_free(p);
            }
        }
    }
}

uint32_t GMAC1_ReceivePkt(struct sk_buff *prskb)
{
    prskb->rdy = 0;
    prskb->len = 0;

    GMAC_int_handler1();

    return prskb->len;
}

void GMAC1_IRQHandler(void)
{
    struct sk_buff *rskb = &rxbuf[GMACINTF1];

    if(GMAC1_ReceivePkt(rskb) != 0)
    {
        /* Allocate pbuf from pool (avoid using heap in interrupts) */
        struct pbuf* p = pbuf_alloc(PBUF_RAW, rskb->len, PBUF_POOL);

        if(p != NULL)
        {
            /* Copy ethernet frame into pbuf */
            pbuf_take(p, rskb->data, rskb->len);

            /* Put in a queue which is processed in main loop */
            if(!queue_try_put(p))
            {
                /* queue is full -> packet loss */
                pbuf_free(p);
            }
        }
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
    IRQ_SetHandler(GMAC0_IRQn, GMAC0_IRQHandler);
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
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;
#ifdef LWIP_IGMP
    netif->flags |= NETIF_FLAG_IGMP;
#endif

    GMAC_open(GMACINTF1, GMAC_MODE);
    IRQ_SetHandler(GMAC1_IRQn, GMAC1_IRQHandler);
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
        tskb = (struct sk_buff *)((uint64_t)&txbuf[GMACINTF0] | NON_CACHE);

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
        tskb = (struct sk_buff *)((uint64_t)&txbuf[GMACINTF1] | NON_CACHE);

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
    struct sk_buff *tskb = (struct sk_buff *)((uint64_t)&txbuf[GMACINTF0] | NON_CACHE);

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    if((p != NULL) && (p->tot_len != 0))
    {
        buf = (uint8_t *)tskb->data;
        len = pbuf_copy_partial(p, buf, p->tot_len, 0);
        dcache_clean_invalidate_by_mva(buf, (size_t)p->tot_len);

        GMAC0_TransmitPkt(tskb, NULL, len);
    }

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
    struct sk_buff *tskb = (struct sk_buff *)((uint64_t)&txbuf[GMACINTF1] | NON_CACHE);

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    if((p != NULL) && (p->tot_len != 0))
    {
        buf = (uint8_t *)tskb->data;
        len = pbuf_copy_partial(p, buf, p->tot_len, 0);
        dcache_clean_invalidate_by_mva(buf, (size_t)p->tot_len);

        GMAC1_TransmitPkt(tskb, NULL, len);
    }

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

        len = 0;
        /* We iterate over the pbuf chain until we have read the entire
        * packet into the pbuf. */
        for(q = p; q != NULL; q = q->next)
        {
            memcpy((u8_t*)q->payload, (u8_t*)&buf[len], q->len);
            len = len + q->len;
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
ethernetif_input0(u16_t len, u8_t *buf)
{
    struct eth_hdr *ethhdr;
    struct pbuf *p;

    /* move received packet into a new pbuf */
    p = low_level_input(NULL, len, buf);
    /* no packet could be read, silently ignore this */
    if (p == NULL) return;

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
ethernetif_input1(u16_t len, u8_t *buf)
{
    struct eth_hdr *ethhdr;
    struct pbuf *p;

    /* move received packet into a new pbuf */
    p = low_level_input(NULL, len, buf);
    /* no packet could be read, silently ignore this */
    if (p == NULL) return;

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
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "nvt";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    netif->name[0] = IFNAME;
    netif->name[1] = IFNAME0;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = low_level_output0;

    /* initialize the hardware */
    low_level_init0(netif);

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
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "nvt";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    netif->name[0] = IFNAME;
    netif->name[1] = IFNAME1;

    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = low_level_output1;

    /* initialize the hardware */
    low_level_init1(netif);

    return ERR_OK;
}

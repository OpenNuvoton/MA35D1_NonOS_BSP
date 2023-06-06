/**************************************************************************//**
 * @file     main.c
 *
 * @brief    A LwIP iperf sample on MA35D1
 *           Modify configuration according to target MAC in ma35d1_mac.h
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include "NuMicro.h"
#include "lwipopts.h"

#include "lwip/tcpip.h"
#include "netif/ethernetif.h"
#include "lwip/apps/lwiperf.h"
#include "lwip/etharp.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/init.h"

extern void TMR2_IRQHandler(void);
extern struct pbuf *queue_try_get(void);

void UART0_Init()
{
    /* Enable UART0 clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Set multi-function pins */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);
}

/**
 * @brief Set GMAC clock source EPLL, init DDR and SSMCC for GMAC
 */
void SYS_Init()
{
    // Configure EPLL = 500MHz
    CLK->PLL[EPLL].CTL0 = (6 << CLK_PLLnCTL0_INDIV_Pos) | (250 << CLK_PLLnCTL0_FBDIV_Pos); // M=6, N=250
    CLK->PLL[EPLL].CTL1 = 2 << CLK_PLLnCTL1_OUTDIV_Pos; // EPLL divide by 2 and enable
    CLK_WaitClockReady(CLK_STATUS_STABLE_EPLL);

    // DDR Init
    outp32(UMCTL2_BASE + 0x6a0, 0x01);

#if (USE_INDEP_HWSEM == 1)
    // Enable HWSEM clock
    CLK_EnableModuleClock(HWS_MODULE);

    // Reset HWSEM
    SYS->IPRST0 = SYS_IPRST0_HWSEMRST_Msk;
    SYS->IPRST0 = 0;
#endif
}

/**
 * @brief Init timer for LwIP
 * Timer2 & Timer3 is assigned to LwIP
 */
void TMR_Init()
{
    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HXT, 0);
    TIMER_Open(TIMER2, TIMER_PERIODIC_MODE, 100); // 10ms
    TIMER_EnableInt(TIMER2);
    IRQ_SetHandler((IRQn_ID_t)TMR2_IRQn, TMR2_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR2_IRQn);
    // Start Timer 2
    TIMER_Start(TIMER2);

    CLK_EnableModuleClock(TMR3_MODULE);
    CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_HXT, 0);
    TIMER_Open(TIMER3, TIMER_PERIODIC_MODE, 1); // 1s
    // Start Timer 3
    TIMER_Start(TIMER3);
}

void netif_Init(struct netif *netif, int intf)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    IP4_ADDR(&gw, 192, 168, 1, 1);
    IP4_ADDR(&ipaddr, 192, 168, 1, 2);
    IP4_ADDR(&netmask, 255, 255, 255, 0);

    sysprintf("Local IP: 192.168.1.2\n");

    lwip_init();

    if(intf == GMACINTF0) {
        netif_add(netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init0, netif_input);
        netif->name[0] = 'e';
        netif->name[1] = '0';
    }
    else {
        netif_add(netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init1, netif_input);
        netif->name[0] = 'e';
        netif->name[1] = '1';
    }

    netif_set_default(netif);
    netif_set_up(netif);
    netif_set_link_up(netif);
}

/* main function */
int main(void)
{
    struct netif netif;
    GMACdevice *gmacdev = &GMACdev[GMAC_INTF];

    SYS_UnlockReg();

    SYS_Init();

    UART0_Init();

    TMR_Init();

    global_timer_init();

    sysprintf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    sysprintf("MA35D1 GMAC Lwiperf Sample Code\n");
    sysprintf("\n-----------------------------------------------------------\n");
	sysprintf("Server listening on 5001");
	sysprintf("\n-----------------------------------------------------------\n");

    netif_Init(&netif, GMAC_INTF);

    lwiperf_start_tcp_server_default(NULL, NULL);

    while(1)
    {
        struct pbuf* p;

        /* Check mii link status per second */
        if(TIMER3->INTSTS != 0)
        {
            TIMER3->INTSTS = TIMER3->INTSTS;
            /* Only enable under the circumstance cable may be plug/unplug */
            GMAC_link_monitor(gmacdev, GMAC_MODE);
        }

        /* Check for received frames, feed them to lwIP */
        IRQ_Disable(GMAC0_IRQn | GMAC1_IRQn);
        p = queue_try_get();
        IRQ_Enable(GMAC0_IRQn | GMAC1_IRQn);

        if(p != NULL)
        {
            if(netif.input(p, &netif) != ERR_OK)
            {
                pbuf_free(p);
            }
        }

        /* Cyclic lwIP timers check */
        sys_check_timeouts();
    }
}

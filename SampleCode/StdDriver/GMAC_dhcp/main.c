/**************************************************************************//**
 * @file     main.c
 *
 * @brief    This Ethernet sample tends to get a DHCP lease from DHCP server.
 *           Modify configuration according to target MAC in ma35d1_mac.h
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include "NuMicro.h"
#include "net.h"

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

    // TODO : fix ssmcc.h, ssmcc_reg.h
    // SSMCC Init
    TZC0->GATE_KEEPER = 0xF; //TZC_GATE_KEEPER_open_request_Msk
    TZC0->REGION_ATTRIBUTES_0 = 0xC0000000; //TZC_REGION_ATTRIBUTES_s_rd_en_Msk | TZC_REGION_ATTRIBUTES_s_wr_en_Msk
    TZC0->REGION_ID_ACCESS_0 = 0x00010001; //(1 << TZC_REGION_ID_ACCESS_nsaid_rd_en_Pos) | (1 << TZC_REGION_ID_ACCESS_nsaid_wr_en_Pos)
    TZC2->GATE_KEEPER = 0x7; // 0x7 << TZC_GATE_KEEPER_open_request_Pos
    TZC2->REGION_ATTRIBUTES_0 = 0xC0000000;
    TZC2->REGION_ID_ACCESS_0 = 0x00030003; //(3 << TZC_REGION_ID_ACCESS_nsaid_rd_en_Pos) | (3 << TZC_REGION_ID_ACCESS_nsaid_wr_en_Pos)

    // DDR Init
    outp32(UMCTL2_BASE + 0x490, 0x01);
    outp32(UMCTL2_BASE + 0x540, 0x01);
    outp32(UMCTL2_BASE + 0x5f0, 0x01);
    outp32(UMCTL2_BASE + 0x6a0, 0x01);
    outp32(UMCTL2_BASE + 0x750, 0x01);
    outp32(UMCTL2_BASE + 0x800, 0x01);
    outp32(UMCTL2_BASE + 0x8b0, 0x01);
    outp32(UMCTL2_BASE + 0x960, 0x01);
}

void IRQ_Init(int intf)
{
    if(intf == GMACINTF0) {
        IRQ_SetHandler(GMAC0_IRQn, GMAC0_IRQHandler);
        IRQ_Enable(GMAC0_IRQn);
    }
    else {
        IRQ_SetHandler(GMAC1_IRQn, GMAC1_IRQHandler);
        IRQ_Enable(GMAC1_IRQn);
    }
}

/* main function */
int main(void)
{
    SYS_UnlockReg();

    SYS_Init();

    UART0_Init();

    global_timer_init();

    sysprintf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    sysprintf("+-----------------------------------+\n");
    sysprintf("|   MA35D1 GMAC Tx/Rx Sample Code   |\n");
    sysprintf("|       - Get IP from DHCP server   |\n");
    sysprintf("+-----------------------------------+\n\n");

    /* Initial MA35D1 GMAC module */
    if(GMAC_open(GMAC_INTF, GMAC_MODE))
        sysprintf("Link not found\n");

    IRQ_Init(GMAC_INTF);

    if(dhcp_start(GMAC_INTF) < 0)
    {
        // Cannot get a DHCP lease
        sysprintf("\nDHCP failed......\n");
    }

    while(1) {}
}

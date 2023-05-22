/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate ts_calibration and ts_test from tslib.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "ma35d1_touch.h"
#include "displib.h"

extern void TMR2_IRQHandler(void);
extern void TMR3_IRQHandler(void);
extern void ADC0_IRQHandler(void);

/* LCD attributes 1024x600 */
DISP_LCD_INFO LcdPanelInfo =
{
    /* Panel Resolution */
    1024,
    600,
    /* DISP_LCD_TIMING */
    {
        51000000,
        1024,
        1,
        160,
        160,
        600,
        1,
        23,
        12,
        ePolarity_Positive,
        ePolarity_Positive
    },
    /* DISP_PANEL_CONF */
    {
        eDPIFmt_D24,
        ePolarity_Positive,
        ePolarity_Positive,
        ePolarity_Positive
    },
};

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable IP clock */
    CLK_EnableModuleClock(ADC_MODULE);
    CLK_EnableModuleClock(GPB_MODULE);
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(TMR2_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(ADC_MODULE, 0, CLK_CLKDIV4_ADC(450));  // Set ADC clock rate to 2MHz
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HXT, 0);
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set multi-function pins for UART RXD, TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* Set PB.12~15 to input mode */
    PB->MODE &= ~(GPIO_MODE_MODE12_Msk | GPIO_MODE_MODE13_Msk | GPIO_MODE_MODE14_Msk | GPIO_MODE_MODE15_Msk);

    /* Set multi-function pin ADC channel 4~7 input*/
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk | SYS_GPB_MFPH_PB14MFP_Msk | SYS_GPB_MFPH_PB15MFP_Msk)) |
                    SYS_GPB_MFPH_PB12MFP_ADC0_CH4 | SYS_GPB_MFPH_PB13MFP_ADC0_CH5 | SYS_GPB_MFPH_PB14MFP_ADC0_CH6 | SYS_GPB_MFPH_PB15MFP_ADC0_CH7;

    /* Disable digital input path to prevent leakage */
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT12);
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT13);
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT14);
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT15);

    /* To update the variable SystemCoreClock */
    SystemCoreClockUpdate();
}

void DISP_Open(void)
{
    /* Set EPLL/2 as DISP Core Clock source */
    DISP_EnableDCUClk();

    /* Waiting EPLL ready */
    CLK_WaitClockReady(CLK_STATUS_STABLE_EPLL);

    /* Enable DISP Core Clock */
    CLK_EnableModuleClock(DCU_MODULE);

    /* Select DISP pixel clock source to VPLL */
    DISP_GeneratePixelClk(LcdPanelInfo.sLcdTiming.u32PCF);

    /* Select DISP pixel clock source */
    CLK_SetModuleClock(DCUP_MODULE, CLK_CLKSEL0_DCUSEL_EPLL_DIV2, 0);

    /* Set multi-function pins for LCD Display Controller */
    SYS->GPG_MFPH = SYS->GPG_MFPH & ~(SYS_GPG_MFPH_PG8MFP_Msk | SYS_GPG_MFPH_PG9MFP_Msk | SYS_GPG_MFPH_PG10MFP_Msk) |
                    (SYS_GPG_MFPH_PG8MFP_LCM_VSYNC | SYS_GPG_MFPH_PG9MFP_LCM_HSYNC | SYS_GPG_MFPH_PG10MFP_LCM_CLK);
    SYS->GPK_MFPL = SYS->GPK_MFPL & ~(SYS_GPK_MFPL_PK4MFP_Msk) | SYS_GPK_MFPL_PK4MFP_LCM_DEN;

    SYS->GPI_MFPH = SYS->GPI_MFPH & ~(SYS_GPI_MFPH_PI8MFP_Msk | SYS_GPI_MFPH_PI9MFP_Msk | SYS_GPI_MFPH_PI10MFP_Msk | SYS_GPI_MFPH_PI11MFP_Msk |
                                      SYS_GPI_MFPH_PI12MFP_Msk | SYS_GPI_MFPH_PI13MFP_Msk | SYS_GPI_MFPH_PI14MFP_Msk | SYS_GPI_MFPH_PI15MFP_Msk);
    SYS->GPI_MFPH |= (SYS_GPI_MFPH_PI8MFP_LCM_DATA0 | SYS_GPI_MFPH_PI9MFP_LCM_DATA1 | SYS_GPI_MFPH_PI10MFP_LCM_DATA2 |
                      SYS_GPI_MFPH_PI11MFP_LCM_DATA3 | SYS_GPI_MFPH_PI12MFP_LCM_DATA4 | SYS_GPI_MFPH_PI13MFP_LCM_DATA5 | SYS_GPI_MFPH_PI14MFP_LCM_DATA6 | SYS_GPI_MFPH_PI15MFP_LCM_DATA7);
    SYS->GPH_MFPL = SYS->GPH_MFPL & ~(SYS_GPH_MFPL_PH0MFP_Msk | SYS_GPH_MFPL_PH1MFP_Msk | SYS_GPH_MFPL_PH2MFP_Msk | SYS_GPH_MFPL_PH3MFP_Msk |
                                      SYS_GPH_MFPL_PH4MFP_Msk | SYS_GPH_MFPL_PH5MFP_Msk | SYS_GPH_MFPL_PH6MFP_Msk | SYS_GPH_MFPL_PH7MFP_Msk);
    SYS->GPH_MFPL |= (SYS_GPH_MFPL_PH0MFP_LCM_DATA8 | SYS_GPH_MFPL_PH1MFP_LCM_DATA9 | SYS_GPH_MFPL_PH2MFP_LCM_DATA10 | SYS_GPH_MFPL_PH3MFP_LCM_DATA11 |
                      SYS_GPH_MFPL_PH4MFP_LCM_DATA12 | SYS_GPH_MFPL_PH5MFP_LCM_DATA13 | SYS_GPH_MFPL_PH6MFP_LCM_DATA14 | SYS_GPH_MFPL_PH7MFP_LCM_DATA15);

    SYS->GPC_MFPH = SYS->GPC_MFPH & ~(SYS_GPC_MFPH_PC12MFP_Msk | SYS_GPC_MFPH_PC13MFP_Msk | SYS_GPC_MFPH_PC14MFP_Msk | SYS_GPC_MFPH_PC15MFP_Msk);
    SYS->GPC_MFPH |= (SYS_GPC_MFPH_PC12MFP_LCM_DATA16 | SYS_GPC_MFPH_PC13MFP_LCM_DATA17 | SYS_GPC_MFPH_PC14MFP_LCM_DATA18 | SYS_GPC_MFPH_PC15MFP_LCM_DATA19);
    SYS->GPH_MFPH = SYS->GPH_MFPH & ~(SYS_GPH_MFPH_PH12MFP_Msk | SYS_GPH_MFPH_PH13MFP_Msk | SYS_GPH_MFPH_PH14MFP_Msk | SYS_GPH_MFPH_PH15MFP_Msk);
    SYS->GPH_MFPH |= (SYS_GPH_MFPH_PH12MFP_LCM_DATA20 | SYS_GPH_MFPH_PH13MFP_LCM_DATA21 | SYS_GPH_MFPH_PH14MFP_LCM_DATA22 | SYS_GPH_MFPH_PH15MFP_LCM_DATA23);

    outp32((void *)0x404d05f0, 0x1);
    outp32((void *)0x404d0000+0x328, 0x1);
    sysprintf("\n  >>>>> AXI Port Priority Finish: 0x%x, 0x%x, 0x%x, 0x%x \n", inp32(0x404d0000 + 0x564),inp32(0x404d0000 + 0x4b4), inp32(0x404d0000 + 0x614), inp32(0x404d0000 + 0x404));
    outp32((void *)0x404d0000+0x564, inp32(0x404d0000 + 0x564) | (0x1 << 5));
    outp32((void *)0x404d0000+0x568, inp32(0x404d0000 + 0x568) | (0x1 << 5));
    outp32((void *)0x404d0000+0x4b4, inp32(0x404d0000 + 0x4b4) | (0x8 << 5));
    outp32((void *)0x404d0000+0x4b8, inp32(0x404d0000 + 0x4b8) | (0x8 << 5));
    outp32((void *)0x404d0000+0x614, inp32(0x404d0000 + 0x614) | (0x10 << 5));
    outp32((void *)0x404d0000+0x618, inp32(0x404d0000 + 0x618) | (0x10 << 5));
    outp32((void *)0x404d0000+0x404, inp32(0x404d0000 + 0x404) | (0x1f << 5));
    outp32((void *)0x404d0000+0x408, inp32(0x404d0000 + 0x408) | (0x1f << 5));
    outp32((void *)0x404d0000+0x328, 0x0);
    while((inp32(0x404d0000 + 0x324) & 0x1) != 0x00000001);
}

/**
 * @brief Init timer for tslib
 * Timer2 & Timer3 is assigned
 */
void TMR_Init()
{
    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HXT, 0);
    TIMER_Open(TIMER2, TIMER_PERIODIC_MODE, 200); // 200kHz for sampling
    TIMER_EnableInt(TIMER2);
    IRQ_SetHandler((IRQn_ID_t)TMR2_IRQn, TMR2_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)TMR2_IRQn);
}

int32_t main (void)
{
    uint8_t u8Cmd;
    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    TMR_Init();

    global_timer_init();

    /* Open DISP IP Clock and set multi-function pins */
    DISP_Open();

    sysprintf("\nThis sample code demonstrate 4-wire touch panel convert function and prints the result on UART\n");

    // Enable channel 0
    ADC_Open(ADC0, ADC_INPUT_MODE_4WIRE_TOUCH, ADC_HIGH_SPEED_MODE, ADC_CH_0_MASK);

    IRQ_SetHandler((IRQn_ID_t)ADC0_IRQn, ADC0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)ADC0_IRQn);

    DISPLIB_LCDInit(LcdPanelInfo);

    DISPLIB_SetFBConfig(eFBFmt_A8R8G8B8, LcdPanelInfo.u32ResolutionWidth, LcdPanelInfo.u32ResolutionHeight, DDR_ADR_FRAMEBUFFER);

    while(1) {
        sysprintf("\n+-------------------------------------------------------------------+\n");
        sysprintf("|           This sample code demonstrate 4-wire touch function      |\n");
        sysprintf("|           Command 'c' for calibration                             |\n");
        sysprintf("|           Command 't' for test                                    |\n");
        sysprintf("+-------------------------------------------------------------------+\n");

        u8Cmd = sysgetchar();

        switch(u8Cmd) {
        case 'C':
        case 'c':
            sysprintf("Start calibration...\n");
            ts_mode = 1;
            ts_calibrate();
            ts_mode = 0;
            break;
        case 'T':
        case 't':
            sysprintf("Start test...\n");
            ts_mode = 2;
            ts_test();
            ts_mode = 0;
            break;
        default:
            break;
        }
    }
}

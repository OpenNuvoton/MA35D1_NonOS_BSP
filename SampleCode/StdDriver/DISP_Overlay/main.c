/**************************************************************************//**
 * @file     main.c
 *
 * @brief    Demonstrate LCD Overlay display.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "displib.h"

#define DDR_ADR_FRAMEBUFFER   0x88000000UL
#define DDR_ADR_OVERLAY       0x89000000UL

uint8_t DISP_Frame[]=
{
#include "framebuffer_image_1024x600.dat"
};

uint8_t DISP_Overlay[]=
{
#include "overlay_image_640x480.dat"
};

/* LCD attributes 1024x600 */
DISP_LCD_INFO LcdPanelInfo =
{
    /* Panel Resolution */
    .u32ResolutionWidth = 1024,
    .u32ResolutionHeight= 600,
    /* DISP_LCD_TIMING */
    {
        .u32PCF = 51000000,
        .u32HA  = 1024,
        .u32HSL = 1,
        .u32HFP = 160,
        .u32HBP = 160,
        .u32VA  = 600,
        .u32VSL = 1,
        .u32VFP = 23,
        .u32VBP = 12,
        .eHSPP =  ePolarity_Positive,
        .eVSPP =  ePolarity_Positive
    },
    /* DISP_PANEL_CONF */
    {
        .eDpiFmt = eDPIFmt_D24,
        .eDEP = ePolarity_Positive,
        .eDP = ePolarity_Positive,
        .eCP = ePolarity_Positive
    },
};

DISP_OVERLAY_INFO OverlayInfo =
{
        .eOverlayFmt = eFBFmt_A8R8G8B8,
        .u32OverlayWidth = 640,
        .u32OverlayHeight = 480,
        .rect_tlx = 160,
        .rect_tly = 50,
        .rect_brx = 800,
        .rect_bry = 530,
};

DISP_OVERLAY_ALPHABLEND OverlayAlphaBlendConfig = {
    .u8Enable = 1,
    .BlendingMode = DC_BLEND_MODE_SRC_OVER,
    .srcGlobalAlphaMode = eGloAM_NORMAL,
    .dstGlobalAlphaMode = eGloAM_NORMAL,
    .srcGlobalAlphaValue = 0x80000000,
    .dstGlobalAlphaValue= 0x80000000,
};

DISP_OVERLAY_COLORKEY OverlayColorKeyConfig = {
    .colorKey = 0xFF,
    .colorKeyHigh = 0xFF,
    .transparency = eKEY,
};

static const char *szAlphaBlending[] =
{
    "DC_BLEND_MODE_CLEAR",
    "DC_BLEND_MODE_SRC",
    "DC_BLEND_MODE_DST",
    "DC_BLEND_MODE_SRC_OVER",
    "DC_BLEND_MODE_DST_OVER",
    "DC_BLEND_MODE_SRC_IN",
    "DC_BLEND_MODE_DST_IN",
    "DC_BLEND_MODE_SRC_OUT"
};

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

void delay_us(int usec)
{
    uint64_t   t0;
    t0  = EL0_GetCurrentPhysicalValue();
    while ((EL0_GetCurrentPhysicalValue() - t0) < (usec * 12));
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(1));

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

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
}

void AlphaBlendingForOverlay(void)
{
    E_DC_BLEND_MODE eAlpha;

    if(OverlayAlphaBlendConfig.u8Enable)
    {
        for (eAlpha = DC_BLEND_MODE_CLEAR; eAlpha <= DC_BLEND_MODE_SRC_OUT; eAlpha++)
        {
            sysprintf("Alpha Blending Operation Mode [%d]: %s\n", eAlpha, szAlphaBlending[eAlpha]);

            /* Set supporting alpha blending effect */
            OverlayAlphaBlendConfig.BlendingMode = eAlpha;
            DISPLIB_SetOverlayBlendOpMode(OverlayAlphaBlendConfig);
            delay_us(1000000);
            delay_us(1000000);
            delay_us(1000000);
        }
    }
    else
    {
        sysprintf("Disable Alpha Blending.\n");
        DISPLIB_DisableOverlayAlphaBlend();
    }
}

void ColorKeyForOverlay(void)
{
    DISPLIB_EnableOverlayAlphaBlend();
    OverlayAlphaBlendConfig.BlendingMode = DC_BLEND_MODE_SRC_OVER;
    DISPLIB_SetOverlayBlendOpMode(OverlayAlphaBlendConfig);
    DISPLIB_SetOverlayGlobalColor(OverlayAlphaBlendConfig);

    /* Transparency of the overlay */
    DISPLIB_SetTransparencyMode(eLayer_Overlay, OverlayColorKeyConfig.transparency);
    DISPLIB_SetOverlayColorKeyValue(OverlayColorKeyConfig);
}

int main(void)
{
    uint8_t u8Char;

    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    /* Open DISP IP Clock and set multi-function pins */
    DISP_Open();

    /* Prepare DISP Framebuffer/Overlay image */
    memcpy((void *)(nc_ptr(DDR_ADR_FRAMEBUFFER)), (const void *)(nc_ptr(DISP_Frame)), sizeof(DISP_Frame));
    memcpy((void *)(nc_ptr(DDR_ADR_OVERLAY)), (const void *)(nc_ptr(DISP_Overlay)), sizeof(DISP_Overlay));

    /* Configure display attributes of LCD panel */
    DISPLIB_LCDInit(LcdPanelInfo);

    /* Configure DISP Framebuffer settings  */
    DISPLIB_SetFBConfig(eFBFmt_A8R8G8B8, LcdPanelInfo.u32ResolutionWidth, LcdPanelInfo.u32ResolutionHeight, DDR_ADR_FRAMEBUFFER);

    /* Framebuffer start to display */
    DISPLIB_EnableOutput(eLayer_Video);

    /* Configure DISP Overlay Setting */
    DISPLIB_SetOverlayConfig(OverlayInfo, DDR_ADR_OVERLAY);
    DISPLIB_SetOverlayRect(OverlayInfo);
    DISPLIB_DisableOutput(eLayer_Overlay);

    do {
        sysprintf("\n+-----------------------------------------------------------------+\n");
        sysprintf("|     This sample code show image under LCD Panel                 |\n");
        sysprintf("|     AlphaBlending and ColorKey operation for Overlay.           |\n");
        sysprintf("+-----------------------------------------------------------------+\n");
        sysprintf("| [1] Alpha Blending for Overlay                                  |\n");
        sysprintf("| [2] ColorKey for Overlay                                        |\n");
        sysprintf("| [3] Disable Overlay Output                                      |\n");
        sysprintf("+-----------------------------------------------------------------+\n");
        sysprintf("Choose item: ");
        u8Char = sysgetchar();
        sysprintf("[%c]\n\n", u8Char);

        DISPLIB_EnableOutput(eLayer_Overlay);

       switch(u8Char) {
            case '1':
                AlphaBlendingForOverlay();
                break;

            case '2':
                ColorKeyForOverlay();
                break;

            case '3':
                DISPLIB_DisableOutput(eLayer_Overlay);
                break;
       }
    } while(1);
}

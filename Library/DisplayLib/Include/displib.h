/**************************************************************************//**
 * @file     displib.h
 * @brief    Display library exported header file.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef __DISPLIB_H__
#define __DISPLIB_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NuMicro.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup LIBRARY Library
  @{
*/

/** @addtogroup DISP_Library Display Library
  @{
*/

/** @addtogroup DISP_EXPORTED_CONSTANTS Display Exported Constants
  @{
*/
typedef enum
{
    eDispLcd_1024x600   = 0,
    eDispLcd_1920x1080  = 1,
    eDispLcd_Cnt
} E_DISP_LCD;


typedef enum
{
    ePolarity_Disable  = -1,   /*!< Pins disable */
    ePolarity_Positive = 0,    /*!< Pins are active high */
    ePolarity_Negative = 1     /*!< Pins are active low */
} E_POLARITY;

typedef enum
{
    eFmtSwizzle_ARGB = 0,      /*!< Pixel format data orders ARGB */
    eFmtSwizzle_RGBA = 1,      /*!< Pixel format data orders RGBA */
    eFmtSwizzle_ABGR = 2,      /*!< Pixel format data orders ABGR */
    eFmtSwizzle_BGRA = 3       /*!< Pixel format data orders BGRA */
} E_PIXEL_FMT_SWIZZLE;

typedef enum
{
    eDPIFmt_D16CFG1,      /*!< DPI interface data format D16CFG1 */
    eDPIFmt_D16CFG2,      /*!< DPI interface data format D16CFG2 */
    eDPIFmt_D16CFG3,      /*!< DPI interface data format D16CFG3 */
    eDPIFmt_D18CFG1,      /*!< DPI interface data format D18CFG1 */
    eDPIFmt_D18CFG2,      /*!< DPI interface data format D18CFG2 */
    eDPIFmt_D24           /*!< DPI interface data format D24 */
} E_DPI_DATA_FMT;

typedef enum
{
    eLayer_Video   = 0,   /*!< Framebuffer layer */
    eLayer_Overlay = 1,   /*!< Overlay layer */
    eLayer_Cnt
} E_DISP_LAYER;

typedef enum
{
    eYUV_709_BT709   = 1,   /*!< YUV standard 709_BT709 */
    eYUV_2020_BT2020 = 3,   /*!< YUV standard 2020_BT2020 */
} E_YUV_STANDARD;

typedef enum
{
    eFBFmt_X4R4G4B4      = 0,   /*!< X4R4G4B4 format */
    eFBFmt_A4R4G4B4      = 1,   /*!< A4R4G4B4 format */
    eFBFmt_X1R5G5B5      = 2,   /*!< X1R5G5B5 format */
    eFBFmt_A1R5G5B5      = 3,   /*!< A1R5G5B5 format */
    eFBFmt_R5G6B5        = 4,   /*!< R5G6B5 format */
    eFBFmt_X8R8G8B8      = 5,   /*!< X8R8G8B8 format */
    eFBFmt_A8R8G8B8      = 6,   /*!< A8R8G8B8 format */
    eFBFmt_YUY2          = 7,   /*!< YUY2 format */
    eFBFmt_UYVY          = 8,   /*!< UYVY format */
    eFBFmt_INDEX8        = 9,   /*!< INDEX8 format */
    eFBFmt_MONOCHROME    = 10,  /*!< MONOCHROME format */
    eFBFmt_YV12          = 15,  /*!< YV12 format */
    eFBFmt_A8            = 16,  /*!< A8 format */
    eFBFmt_NV12          = 17,  /*!< NV12 format */
    eFBFmt_NV16          = 18,  /*!< NV16 format */
    eFBFmt_RG16          = 19,  /*!< RG16 format */
    eFBFmt_R8            = 20,  /*!< R8 format */
    eFBFmt_NV12_10BIT    = 21,  /*!< NV12_10BIT format */
    eFBFmt_A2R10G10B10   = 22,  /*!< A2R10G10B10 format */
    eFBFmt_NV16_10BIT    = 23,  /*!< NV16_10BIT format */
    eFBFmt_INDEX1        = 24,  /*!< INDEX1 format */
    eFBFmt_INDEX2        = 25,  /*!< INDEX2 format */
    eFBFmt_INDEX4        = 26,  /*!< INDEX4 format */
    eFBFmt_P010          = 27,  /*!< P010 format */
    eFBFmt_NV12_10BIT_L1 = 28,  /*!< NV12_10BIT_L1 format */
    eFBFmt_NV16_10BIT_L1 = 29   /*!< NV16_10BIT_L1 format */
} E_FB_FMT;

typedef enum
{
    eOPAQUE,     /*!< OPAQUE effects */
    eMASK,       /*!< MASK effect */
    eKEY         /*!< OPAQUE effect */
} E_TRANSPARENCY_MODE;

typedef enum
{
    DC_BLEND_MODE_CLEAR,
    DC_BLEND_MODE_SRC,
    DC_BLEND_MODE_DST,
    DC_BLEND_MODE_SRC_OVER,
    DC_BLEND_MODE_DST_OVER,
    DC_BLEND_MODE_SRC_IN,
    DC_BLEND_MODE_DST_IN,
    DC_BLEND_MODE_SRC_OUT
} E_DC_BLEND_MODE;

typedef enum
{
    eGloAM_NORMAL,
    eGloAM_GLOBAL,
} E_GLOBAL_ALPHA_MODE;

typedef enum
{
    eBM_ZERO,
    eBM_ONE,
    eBM_NORMAL,
    eBM_INVERSED,
    eBM_COLOR,
    eBM_COLOR_INVERSED,
    eBM_SATURATED_ALPHA,
    eBM_SATURATED_DEST_ALPHA
} E_BLENDING_MODE;

typedef void (VSYNC_CB)(void);    /*!< interrupt in callback function \hideinitializer */

/* Error code */
#define DISPLIB_SUCCESS                           0x00000000UL  /*!< Display successful without error   */
#define DISPLIB_ERR_SETTIMING                     0x00000001UL  /*!< Display set timing error   */
#define DISPLIB_ERR_SETOVERLAYRECT                0x00000002UL  /*!< Display Overlay set rect(tlx, tly, brx, bry) error   */

/*! @}*/ /* end of group DISPLIB_EXPORTED_CONSTANTS */

/** @addtogroup DISPLIB_EXPORTED_STRUCTURES Display Exported Structures
  @{
*/
typedef struct {

/*
    Set display panel timing.
    htotal: u32HA + u32HBP + u32HFP + u32HSL
    vtotal: u32VA + u32VBP + u32VFP + u32VSL
    Panel Pixel Clock frequency: htotal * vtotal * fps
*/
    uint32_t u32PCF;    /*!< Panel Pixel Clock Frequency in Hz   */

    uint32_t u32HA;     /*!< Horizontal Active, Horizontal panel resolution   */
    uint32_t u32HSL;    /*!< Horizontal Sync Length, panel timing   */
    uint32_t u32HFP;    /*!< Horizontal Front Porch, panel timing   */
    uint32_t u32HBP;    /*!< Horizontal Back Porch, panel timing   */
    uint32_t u32VA;     /*!< Vertical Active, Vertical panel resolution in pixels   */
    uint32_t u32VSL;    /*!< Vertical Sync Length, panel timing   */
    uint32_t u32VFP;    /*!< Vertical Front Porch, panel timing   */
    uint32_t u32VBP;    /*!< Vertical Back Porch, panel timing   */

    E_POLARITY eHSPP;   /*!< Polarity of the horizontal sync pulse   */
    E_POLARITY eVSPP;   /*!< VSync Pulse Polarity   */

} DISP_LCD_TIMING;

typedef struct
{
    E_DPI_DATA_FMT eDpiFmt; /*!< DPI Data Format   */
    E_POLARITY     eDEP;    /*!< DE Polarity       */
    E_POLARITY     eDP;     /*!< DATA Polarity     */
    E_POLARITY     eCP;     /*!< CLOCK Polarity    */
} DISP_PANEL_CONF;

typedef struct
{
    uint32_t u32ResolutionWidth;  /*!< Panel Width    */
    uint32_t u32ResolutionHeight; /*!< Panel Height   */
    DISP_LCD_TIMING sLcdTiming;   /*!< Panel timings for some registers   */
    DISP_PANEL_CONF sPanelConf;   /*!< Panel Configure information   */
} DISP_LCD_INFO;

typedef struct
{
    E_FB_FMT eOverlayFmt;          /*!< Overlay format                   */
    uint32_t u32OverlayWidth;      /*!< Overlay width                    */
    uint32_t u32OverlayHeight;     /*!< Overlay Height                   */
    uint32_t rect_tlx;             /*!< Left boundary of overlay window  */
    uint32_t rect_tly;             /*!< Top boundary of overlay window   */
    uint32_t rect_brx;             /*!< Right boundary of overlay        */
    uint32_t rect_bry;             /*!< Bottom boundary of overlay       */
} DISP_OVERLAY_INFO;

typedef struct
{
    uint8_t u8Enable;                /*!< Overlay format                   */
    uint32_t BlendingMode;           /*!< Overlay Blending Mode            */
    uint32_t srcGlobalAlphaMode;     /*!< Overlay Source Alpha Mode        */
    uint32_t dstGlobalAlphaMode;     /*!< Overlay Destination Alpha Mode   */
    uint32_t srcGlobalAlphaValue;    /*!< Overlay Source Global Color value used in alpha blending process       */
    uint32_t dstGlobalAlphaValue;    /*!< Overlay Destination Global Color value used in alpha blending process  */
} DISP_OVERLAY_ALPHABLEND;

typedef struct
{
    uint32_t colorKey;               /*!< Start of color key range for overlay  */
    uint32_t colorKeyHigh;           /*!< End of color key range for overlay    */
    uint32_t transparency;           /*!< Transparency mode of the overlay      */
} DISP_OVERLAY_COLORKEY;

/*! @}*/ /* end of group DISPLIB_EXPORTED_STRUCTURES */

/** @addtogroup DISP_EXPORTED_FUNCTIONS Display Exported Functions
  @{
*/

/*------------------------------------------------------------------*/
/*  Display Library APIs                                            */
/*------------------------------------------------------------------*/
uint32_t DISPLIB_GetFrameCounter(void);
uint32_t DISPLIB_LCDTIMING_GetFPS(const DISP_LCD_TIMING DispLCDTiming);
uint32_t DISPLIB_GetBitPerPixel(E_FB_FMT eFbFmt);

/**
  * @brief    Configure display Timings of video interface.
  *
  * @param[in]  LCDTiming   Base address of the memory block
  *
  * @retval   0     Init OK
  * @retval   < 0   Failed. Refer to error code definitions.
  */
uint32_t DISPLIB_SetTiming(DISP_LCD_TIMING LCDTiming);
void DISPLIB_SetPanelConf(DISP_PANEL_CONF PanelConf);
int DISPLIB_EnableOutput(E_DISP_LAYER eLayer);
int DISPLIB_DisableOutput(E_DISP_LAYER eLayer);
int DISPLIB_SetTransparencyMode(E_DISP_LAYER eLayer, E_TRANSPARENCY_MODE eTM);
void DISPLIB_EnableOverlayAlphaBlend(void);
void DISPLIB_DisableOverlayAlphaBlend(void);
int DISPLIB_SetOverlayBlendOpMode(DISP_OVERLAY_ALPHABLEND OverlayAlphaBlend);
void DISPLIB_SetOverlayGlobalColor(DISP_OVERLAY_ALPHABLEND OverlayAlphaBlend);
void DISPLIB_SetOverlayColorKeyValue(DISP_OVERLAY_COLORKEY OverlayColorKey);
void DISPLIB_SetFrameBufferBGColor(uint32_t u32BGColor);

/**
  * @brief    Set FrameBuffer Color Key Value
  * @param[in]  u32ColorKeyLow   Base address of the memory block
  * @param[in]  u32ColorKeyHigh   Size of memory block.
  */
void DISPLIB_SetFrameBufferColorKeyValue(uint32_t u32ColorKeyLow, uint32_t u32ColorKeyHigh);

/**
  * @brief    Set Framebuffer Start Address
  * @param[in]  u32DMAFBStartAddr   Framebuffer Start Address
  * @retval   0     Init OK
  * @retval   < 0   Failed. Refer to error code definitions.
  */
int DISPLIB_SetFBAddr(uint32_t u32DMAFBStartAddr);

/**
  * @brief    Set Framebuffer format
  * @param[in]  eFbFmt      The format of the framebuffer
  * @param[in]  u32Pitch    Number of bytes from start of one line to next line
  */
void DISPLIB_SetFBFmt(E_FB_FMT eFbFmt, uint32_t u32Pitch);
int32_t DISPLIB_SetFBConfig(E_FB_FMT eFbFmt, uint32_t u32ResWidth, uint32_t u32ResHeight, uint32_t u32DMAFBStartAddr);

/**
  * @brief    Set Overlay Start Address
  * @param[in]  u32DMAFBStartAddr   Overlay Start Address
  * @retval   0     Init OK
  * @retval   < 0   Failed. Refer to error code definitions.
  */
int DISPLIB_SetOverlayAddr(uint32_t u32DMAFBStartAddr);
void DISPLIB_SetOverlaySize(DISP_OVERLAY_INFO OverlayInfo);
void DISPLIB_SetOverlayRect(DISP_OVERLAY_INFO OverlayInfo);
int32_t DISPLIB_SetOverlayConfig(DISP_OVERLAY_INFO OverlayInfo, uint32_t u32DMAFBStartAddr);

/**
  * @brief    Install Display VSync callback function which will be called
  *           from Display interrupt.
  * @param[in]  func   The Display VSync callback function pointer.
  * @retval   0     Success
  * @retval   < 0   Failed. Refer to error code definitions.
  */
int32_t DISPLIB_InstallVSyncCallback(VSYNC_CB *func);
int32_t DISPLIB_LCDInit(const DISP_LCD_INFO LCDInfo);

/**
  * @brief    Disable LCD Display Controller.
  */
int32_t DISPLIB_LCDDeinit(void);


/**
  * @brief    Assign the highest AXI port priority to Display
  */
void DISPLIB_DDR_AXIPort_Priority(void);

/**
  * @brief    Initialize VC8000 library. It requires a large block
  *           of memory for H264 reference buffer. For 1080P H264
  *           decode, at least 32 MB memory is required, which for
  *           720P H264 decode, at least 26 MB memory is required.
  * @param[in]  buf_base   Base address of the memory block
  * @param[in]  buf_size   Size of memory block.
  * @retval   0     Init OK
  * @retval   < 0   Failed. Refer to error code definitions.
  */
int32_t DISPLIB_HDMIinit(void);
int32_t DISPLIB_EDPinit(void);
int32_t DISPLIB_MIPIinit(void);

/*! @}*/ /* end of group DISP_EXPORTED_FUNCTIONS */

/*! @}*/ /* end of group DISP_Library */

/*! @}*/ /* end of group LIBRARY */

#ifdef __cplusplus
}
#endif

#endif  /* __DISPLIB_H__ */

/**************************************************************************//**
 * @file     vc8000_lib.h
 * @brief    VC8000 H264/JPEG decoder library header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef  _VC8000_LIB_H_
#define  _VC8000_LIB_H_

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup LIBRARY Library
  @{
*/

/** @addtogroup VC8000_Library VC8000 Library
  @{
*/

/** @addtogroup VC8000_EXPORTED_CONSTANTS VC8000 Exported Constants
  @{
*/

#define VC8000_PP_MAX_WIDTH           1920   /*!< Maximum width of PP output */
#define VC8000_PP_MAX_HEIGHT          1080   /*!< Maximum width of PP output */

#define VC8000_OK                     0      /*!< No error */
#define VC8000_ERR_LIB_SERIOUS        -1     /*!< A serious error. Some internal data corrupted */
#define VC8000_ERR_MEMORY_OUT         -10    /*!< Out of memory */
#define VC8000_ERR_NO_FREE_INSTANCE   -11    /*!< There are no free instances */
#define VC8000_ERR_INVALID_INSTANCE   -12    /*!< Is not a valid instance */
#define VC8000_ERR_NOT_SUPPORTED      -21    /*!< Function not supported */
#define VC8000_ERR_INVALID_PARAM      -31    /*!< Invalid parameter */
#define VC8000_ERR_OUT_REQUIRED       -32    /*!< Output buffer is required */
#define VC8000_ERR_DEC_INIT           -100   /*!< H264/JPEG decoder initialization failed */
#define VC8000_ERR_TIMEOUT            -101   /*!< H264/JPEG decode timeout */
#define VC8000_ERR_DECODE             -102   /*!< H264/JPEG decode error */
#define VC8000_ERR_BUS                -103   /*!< system bus error */
#define VC8000_ERR_BIT_STREAM         -104   /*!< Invalid or unsupported bit stream  */
#define VC8000_ERR_JPEG_YUV_ADDR      -151   /*!< JPEG PP is not enabled and YUV address required */
#define VC8000_ERR_PP_INIT            -201   /*!< PP config failed */

#define VC8000_PP_F_RGB888            1 /*!< color format use RGB888 */
#define VC8000_PP_F_RGB565            2 /*!< color format use RGB565 */
#define VC8000_PP_F_NV12              3 /*!< color format use NV12 (YUV420) */
#define VC8000_PP_F_YUV422            4 /*!< color format use YUV422 */

#define VC8000_PP_OUT_DST_DISPLAY     0 /*!< PP output directly to display buffer */
#define VC8000_PP_OUT_DST_OVERLAY     1 /*!< PP output directly to overlay buffer */
#define VC8000_PP_OUT_DST_USER        2 /*!< PP output to user specified buffer */

#define VC8000_PP_ROTATION_NONE       0 /*!< PP output image no rotate */
#define VC8000_PP_ROTATION_RIGHT_90   1 /*!< PP output image rotate right 90 degree */
#define VC8000_PP_ROTATION_LEFT_90    2 /*!< PP output image rotate left 90 degree */
#define VC8000_PP_ROTATION_HOR_FLIP   3 /*!< PP output image horizontal flip */
#define VC8000_PP_ROTATION_VER_FLIP   4 /*!< PP output image vertical flip */
#define VC8000_PP_ROTATION_180        5 /*!< PP output image rotate 180 degree */

/*! @}*/ /* end of group VC8000_EXPORTED_CONSTANTS */

/** @addtogroup VC8000_EXPORTED_STRUCTURES VC8000 Exported Structures
  @{
*/
struct pp_params {
    int      frame_buf_w;      /*!< width of frame buffer */
    int      frame_buf_h;      /*!< height of frame buffer */
    int      img_out_x;        /*!< image original point(x,y) on frame buffer */
    int      img_out_y;        /*!< image original point(x,y) on frame buffer */
    int      img_out_w;        /*!< image output width on frame buffer */
    int      img_out_h;        /*!< image output height on frame buffer */
    int      img_out_fmt;      /*!< image output format */
    int      rotation;         /*!< select image output rotation */
    int      pp_out_dst;       /*!< PP output destination */
    uint32_t pp_out_paddr;     /*!< PP output physical address */
    int      contrast;         /*!< contrast setting for PP RGB output; suggest 8 */
    int      brightness;       /*!< brightness setting for PP RGB output */
    int      saturation;       /*!< brightness setting for PP RGB output; suggest 32 */
    int      alpha;            /*!< alpha setting for PP RGB output; suggest 255 */
    int      transparency;     /*!< brightness setting for PP RGB output */
};                             /*!< VC8000 post-processing parameters */

typedef void (VC8K_H264Dec_CB)(void);    /*!< VC8000 H264 decode done callback function \hideinitializer */

/*! @}*/ /* end of group VC8000_EXPORTED_STRUCTURES */

/** @addtogroup VC8000_EXPORTED_FUNCTIONS VC8000 Exported Functions
  @{
*/

/*------------------------------------------------------------------*/
/*  VC8000 Library APIs                                           */
/*------------------------------------------------------------------*/

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
int VC8000_Init(uint32_t buf_base, uint32_t buf_size);

/**
  * @brief    Request for a VC8000 H264 decode instance from VC8000 library.
  * @return   It will successfully create an instance for VC8000 H264
  *           decoding if the return handle value is greater than 0.
  *           If the handle is < 0, it indicates an error code.
  */
int VC8000_H264_Open_Instance(void);

/**
  * @brief    Enable post-processing for the H264 decode instance.
  * @param[in]  handle  Handle of the VC8000 H264 decode instance
  * @param[in]  pp   PP output parameters
  * @retval   0     Success
  * @retval   < 0   Failed. Refer to error code definitions.
  */
int VC8000_H264_Enable_PP(int handle, struct pp_params *pp);

/**
  * @brief    Update post-processing for the H264 decode instance.
  * @param[in]  handle  Handle of the VC8000 H264 decode instance
  * @param[in]  pp   PP output parameters
  * @retval   0     Success
  * @retval   < 0   Failed. Refer to error code definitions.
  */
int VC8000_H264_Update_PP(int handle, struct pp_params *pp);

/**
  * @brief    Perform H264 decode. It will decode one H264 frame and return
  *           the length of remaining bit stream data.
  * @param[in]  handle  Handle of the VC8000 H264 decode instance
  * @param[in]  in       Input H264 bit stream
  * @param[in]  in_size  Leghth of the input H264 bit stream
  * @param[out] out      Decode output buffer.
  *                      If PP enabled and 'out' is NULL, the VC8000
  *                      PP will output to the address specified in
  *                      the PP parameter configured by VC8000_H264_Enable_PP().
  * @param[out] remain   The remaining byte count of input bit stream.
  * @retval   0     Success
  * @retval   < 0   Failed. Refer to error code definitions.
  */
int VC8000_H264_Decode_Run(int handle, uint8_t *in, uint32_t in_size, uint8_t *out, uint32_t *remain);

/**
  * @brief    Close a VC8000 H264 decode instance.
  * @param[in]  handle   Handle of the VC8000 H264 decode instance to be closed.
  * @retval   0   Success
  * @retval   < 0   Failed. Refer to error code definitions.
  */
int VC8000_H264_Close_Instance(int handle);

/**
  * @brief    Request for a VC8000 JPEG decode instance from VC8000 library.
  * @return   It will successfully create an instance for VC8000 JPEG
  *           decoding if the return handle value is greater than 0.
  *           If the handle is < 0, it indicates an error code.
  */
int VC8000_JPEG_Open_Instance(void);

/**
  * @brief    Enable post-processing for the JPEG decode instance.
  * @param[in]  handle  Handle of the VC8000 JPEG decode instance
  * @param[in]  pp   PP output parameters
  * @retval   0     Success
  * @retval   < 0   Failed. Refer to error code definitions.
  */
int VC8000_JPEG_Enable_PP(int handle, struct pp_params *pp);

/**
  * @brief    Set the YUV plane base address of JPEG decode output.
  * @param[in]  handle  Handle of the VC8000 JPEG decode instance
  * @param[in]  y_addr   Base address of JPEG decode output Y plane
  * @param[in]  u_addr   Base address of JPEG decode output U plane
  * @param[in]  v_addr   Base address of JPEG decode output V plane
  * @retval   0     Success
  * @retval   < 0   Failed. Refer to error code definitions.
  */
int VC8000_JPEG_Set_YUV_Address(int handle, uint32_t y_addr, uint32_t u_addr, uint32_t v_addr);

/**
  * @brief    Perform JPEG decode. It will decode one JPEG image.
  * @param[in]  handle  Handle of the VC8000 JPEG decode instance
  * @param[in]  in       Input JPEG image
  * @param[in]  in_size  Size of the input JPEG image in bytes
  * @param[out] out      Decode output buffer.
  *                      If PP enabled and 'out' is NULL, the VC8000
  *                      PP will output to the address specified in
  *                      the PP parameter configured by VC8000_JPEG_Enable_PP().
  * @retval   0     Success
  * @retval   < 0   Failed. Refer to error code definitions.
  */
int VC8000_JPEG_Decode_Run(int handle, uint8_t *in, uint32_t in_size, uint8_t *out);

/**
  * @brief    Close a VC8000 JPEG decode instance.
  * @param[in]  handle   Handle of the VC8000 JPEG decode instance to be closed.
  * @retval   0   Success
  * @retval   < 0   Failed. Refer to error code definitions.
  */
int VC8000_JPEG_Close_Instance(int handle);

/**
  * @brief    Install VC8000 H264 decode done callback function which will be called
  *           on each H264 frame decode done.
  * @param[in]  func   The VC8000 H264 decode done callback function pointer.
  * @retval   0     Success
  * @retval   < 0   Failed. Refer to error code definitions.
  */
int32_t VC8000_InstallH264DecCallback(VC8K_H264Dec_CB *func);

/**
  * @brief    Disable VC8000 H264 decode done callback function.
  * @retval   0     Success
  * @retval   < 0   Failed. Refer to error code definitions.
  */
int32_t VC8000_DisableH264DecCallback(void);


/*! @}*/ /* end of group VC8000_EXPORTED_FUNCTIONS */

/*! @}*/ /* end of group VC8000_Library */

/*! @}*/ /* end of group LIBRARY */

#ifdef __cplusplus
}
#endif

#endif  /* _VC8000_LIB_H_ */

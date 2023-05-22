/**************************************************************************//**
 * @file     adc.h
 * @brief    ADC driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright(C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup ADC_Driver ADC Driver
  @{
*/

/** @addtogroup ADC_EXPORTED_CONSTANTS ADC Exported Constants
  @{
*/

#define ADC_CH_0_MASK           (1UL << 0)          /*!< ADC channel 0 mask \hideinitializer */
#define ADC_CH_1_MASK           (1UL << 1)          /*!< ADC channel 1 mask \hideinitializer */
#define ADC_CH_2_MASK           (1UL << 2)          /*!< ADC channel 2 mask \hideinitializer */
#define ADC_CH_3_MASK           (1UL << 3)          /*!< ADC channel 3 mask \hideinitializer */
#define ADC_CH_4_MASK           (1UL << 4)          /*!< ADC channel 4 mask \hideinitializer */
#define ADC_CH_5_MASK           (1UL << 5)          /*!< ADC channel 5 mask \hideinitializer */
#define ADC_CH_6_MASK           (1UL << 6)          /*!< ADC channel 6 mask \hideinitializer */
#define ADC_CH_7_MASK           (1UL << 7)          /*!< ADC channel 7 mask \hideinitializer */
#define ADC_CH_NUM              8                   /*!< Total Channel number \hideinitializer */
#define ADC_HIGH_SPEED_MODE     ADC_CONF_SPEED_Msk  /*!< ADC working in high speed mode (3.2MHz <= ECLK <= 16MHz) \hideinitializer */
#define ADC_NORMAL_SPEED_MODE   0                   /*!< ADC working in normal speed mode (ECLK < 3.2MHz) \hideinitializer */
#define ADC_REFSEL_VREF         0                   /*!< ADC reference voltage source selection set to VREF \hideinitializer */
#define ADC_REFSEL_AVDD         (3UL << ADC_CONF_REFSEL_Pos)  /*!< ADC reference voltage source selection set to AVDD \hideinitializer */

#define ADC_INPUT_MODE_NORMAL_CONV  0   /*!< ADC works in normal conversion mode \hideinitializer */
#define ADC_INPUT_MODE_4WIRE_TOUCH  1   /*!< ADC works in 4-wire touch screen mode \hideinitializer */
#define ADC_INPUT_MODE_5WIRE_TOUCH  2   /*!< ADC works in 5-wire touch screen mode \hideinitializer */

/*! @}*/ /* end of group ADC_EXPORTED_CONSTANTS */

/** @addtogroup ADC_EXPORTED_FUNCTIONS ADC Exported Functions
  @{
*/

/**
  * @brief Get the latest ADC conversion data
  * @param[in] adc Base address of ADC module
  * @param[in] u32ChNum Currently not used
  * @return  Latest ADC conversion data
  * \hideinitializer
  */
#define ADC_GET_CONVERSION_DATA(adc, u32ChNum) ((adc)->DATA)

/**
  * @brief Get the latest ADC conversion X data
  * @param[in] adc Base address of ADC module
  * @return  Latest ADC conversion X data
  * \hideinitializer
  */
#define ADC_GET_CONVERSION_XDATA(adc) ((adc)->XYDATA & ADC_XYDATA_XDATA_Msk)

/**
  * @brief Get the latest ADC conversion Y data
  * @param[in] adc Base address of ADC module
  * @return  Latest ADC conversion Y data
  * \hideinitializer
  */
#define ADC_GET_CONVERSION_YDATA(adc) ((adc)->XYDATA >> ADC_XYDATA_YDATA_Pos)

/**
  * @brief Get the latest ADC conversion Z1 data
  * @param[in] adc Base address of ADC module
  * @return  Latest ADC conversion Z1 data
  * \hideinitializer
  */
#define ADC_GET_CONVERSION_Z1DATA(adc) ((adc)->ZDATA & ADC_ZDATA_Z1DATA_Msk)

/**
  * @brief Get the latest ADC conversion Z2 data
  * @param[in] adc Base address of ADC module
  * @return  Latest ADC conversion Z2 data
  * \hideinitializer
  */
#define ADC_GET_CONVERSION_Z2DATA(adc) ((adc)->ZDATA >> ADC_ZDATA_Z2DATA_Pos)

/**
  * @brief Return the user-specified interrupt flags
  * @param[in] adc Base address of ADC module
  * @param[in] u32Mask Could be \ref ADC_IER_MIEN_Msk
  * @return  User specified interrupt flags
  * \hideinitializer
  */
#define ADC_GET_INT_FLAG(adc, u32Mask) ((adc)->ISR & (u32Mask))

/**
  * @brief This macro clear the selected interrupt status bits
  * @param[in] adc Base address of ADC module
  * @param[in] u32Mask Could be \ref ADC_IER_MIEN_Msk
  * @return  None
  * \hideinitializer
  */
#define ADC_CLR_INT_FLAG(adc, u32Mask) ((adc)->ISR = (u32Mask))


/**
  * @brief Power down ADC module
  * @param[in] adc Base address of ADC module
  * @return None
  * \hideinitializer
  */
#define ADC_POWER_DOWN(adc) ((adc)->CTL &= ~ADC_CTL_ADEN_Msk)

/**
  * @brief Power on ADC module
  * @param[in] adc Base address of ADC module
  * @return None
  * \hideinitializer
  */
#define ADC_POWER_ON(adc) ((adc)->CTL |= ADC_CTL_ADEN_Msk)


/**
  * @brief Set ADC input channel. Enabled channel will be converted while ADC starts.
  * @param[in] adc Base address of ADC module
  * @param[in] u32Mask  Channel enable bit. Each bit corresponds to a input channel. Bit 0 is channel 0, bit 1 is channel 1...
  * @return None
  * @note ADC can only convert 1 channel at a time. If more than 1 channels are enabled, only channel
  *       with smallest number will be convert.
  * \hideinitializer
  */
#define ADC_SET_INPUT_CHANNEL(adc, u32Mask) do {uint32_t u32Ch = 0, i;\
                                                for(i = 0; i < ADC_CH_NUM; i++) {\
                                                    if((u32Mask) & (1 << i)) {\
                                                        u32Ch = i;\
                                                        break;\
                                                    }\
                                                }\
                                                (adc)->CONF = ((adc)->CONF & ~ADC_CONF_CHSEL_Msk) | (u32Ch << ADC_CONF_CHSEL_Pos);\
                                            }while(0)

/**
  * @brief Start the A/D conversion.
  * @param[in] adc Base address of ADC module
  * @return None
  * \hideinitializer
  */
#define ADC_START_CONV(adc) ((adc)->CTL |= ADC_CTL_MST_Msk)

/**
  * @brief Set the reference voltage selection.
  * @param[in] adc Base address of ADC module
  * @param[in] u32Ref The reference voltage selection. Valid values are:
  *                 - \ref ADC_REFSEL_VREF
  *                 - \ref ADC_REFSEL_AVDD
  * @return None
  * \hideinitializer
  */
#define ADC_SET_REF_VOLTAGE(adc, u32Ref) ((adc)->CONF = ((adc)->CONF & ~ADC_CONF_REFSEL_Msk) | (u32Ref))

/**
  * @brief Set ADC to convert X/Y coordinate
  * @param[in] adc Base address of ADC module
  * @return None
  * \hideinitializer
  */
#define ADC_CONVERT_XY_MODE(adc) do {(adc)->CTL &= ~ADC_CTL_PEDEEN_Msk;\
                                     (adc)->CONF |= ADC_CONF_TEN_Msk | ADC_CONF_ZEN_Msk;} while(0)

/**
  * @brief Set ADC to detect pen down event
  * @param[in] adc Base address of ADC module
  * @return None
  * \hideinitializer
  */
#define ADC_DETECT_PD_MODE(adc) do {(adc)->CONF &= ~(ADC_CONF_TEN_Msk | ADC_CONF_ZEN_Msk);\
                                    (adc)->CTL |= ADC_CTL_PEDEEN_Msk;} while(0)

void ADC_Open(ADC_T *adc,
              uint32_t u32InputMode,
              uint32_t u32OpMode,
              uint32_t u32ChMask);
void ADC_Close(ADC_T *adc);
void ADC_EnableInt(ADC_T *adc, uint32_t u32Mask);
void ADC_DisableInt(ADC_T *adc, uint32_t u32Mask);

/*! @}*/ /* end of group ADC_EXPORTED_FUNCTIONS */

/*! @}*/ /* end of group ADC_Driver */

/*! @}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */


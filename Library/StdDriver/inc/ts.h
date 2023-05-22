/**************************************************************************//**
 * @file     ts.h
 * @brief    Temperature sensor driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __TS_H__
#define __TS_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup TS_Driver TS Driver
  @{
*/

/** @addtogroup TS_EXPORTED_CONSTANTS TS Exported Constants
  @{
*/

/*---------------------------------------------------------------------------------------------------------*/
/*  Temperature Sensor Calibration Constant Definitions                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define TS_CAL_A                    (27435)
#define TS_CAL_B                    (9333)

/*! @}*/ /* end of group TS_EXPORTED_CONSTANTS */

/** @addtogroup TS_EXPORTED_FUNCTIONS TS Exported Functions
  @{
*/

/**
  * @brief      Clear TS Data Vaild Flag
  *
  * @details    Write 1 to DATAVALID bit of SYS_TSENSRFCR register to clear the data valid flag.
  * \hideinitializer
  */
#define TS_CLEAR_DATAVALID_FLAG()          (TS->TSENSRFCR |= TS_CTL_DATAVALID_Msk)

/**
  * @brief      Get TS Data Vaild Flag
  *
  * @retval     0    Temperature sensor data is invalid.
  * @retval     1    Temperature sensor data is valid.
  *
  * @details    This macro indicates the temperature sensor data is valid or not.
  * \hideinitializer
  */
#define TS_GET_DATAVALID_FLAG()            ((TS->TSENSRFCR & TS_CTL_DATAVALID_Msk)? 1UL : 0UL)

/**
  * @brief      Get Temperature Value
  *
  * @return     Temperature value.
  *
  * @details    Calculate the temperature from ADC conversion data of internal temperature sensor.
  * \hideinitializer
  */
#define TS_GET_TEMPERATURE()               ((((TS->TSENSRFCR & TS_CTL_TSENSRDATA_Msk) >> TS_CTL_TSENSRDATA_Pos)\
		                                       * TS_CAL_A / 4096 - TS_CAL_B) / 100)

void TS_Open(void);
void TS_Close(void);

/*! @}*/ /* end of group TS_EXPORTED_FUNCTIONS */

/*! @}*/ /* end of group TS_Driver */

/*! @}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __TS_H__ */


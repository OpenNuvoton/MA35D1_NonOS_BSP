/**************************************************************************//**
 * @file     ts.c
 * @brief    Temperature sensor driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NuMicro.h"


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup TS_Driver TS Driver
  @{
*/

/** @addtogroup TS_EXPORTED_FUNCTIONS TS Exported Functions
  @{
*/

/**
  * @brief      Begin measuring the temperature.
  *
  * @details    This function sets Temperature Sensor in normal mode.
  */
void TS_Open(void)
{
	TS->TSENSRFCR &= ~TS_CTL_PD_Msk;

	return;
}

/**
  * @brief      Stop measuring the temperature.
  *
  * @details    This function sets Temperature Sensor in power-down mode.
  */
void TS_Close(void)
{
	TS->TSENSRFCR |= TS_CTL_PD_Msk;

	return;
}

/*! @}*/ /* end of group TS_EXPORTED_FUNCTIONS */

/*! @}*/ /* end of group TS_Driver */

/*! @}*/ /* end of group Standard_Driver */


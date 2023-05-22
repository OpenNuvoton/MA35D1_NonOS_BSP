/**************************************************************************//**
 * @file     ts_reg.h
 * @brief    Temperature sensor register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __TS_REG_H__
#define __TS_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/** @addtogroup REGISTER Control Register
  @{
*/

/** @addtogroup TS Temperature Sensor (TS)
    Memory Mapped Structure for TS Controller
  @{
*/

typedef struct
{

/**
 * @var TS_T::TSENSRFCR
 * Offset: 0x00  TS Control Register
 * ---------------------------------------------------------------------------------------------------
 * |Bits    |Field      |Descriptions
 * | :----: | :----:    | :---- |
 * |[7:0]   |TSENSRREF0 |Temperature Sensor Reference Value 0
 * |        |           |TSENSRREF0 keeps 8-bit value measured at 25C for temperature conversion formula variable A calibration.
 * |[15:8]  |TSENSRREF1 |Temperature Sensor Reference Value 1
 * |        |           |TSENSRREF1 keeps 8-bit value measured at 25C for temperature conversion formula variable B calibration.
 * |[27:16] |TSENSRDATA |Temperature Sensor Data
 * |        |           |TSENSRDATA keeps 12-bit value measured by temperature sensor.
 * |[28]    |PD         |Temperature Sensor Power Down
 * |        |           |0 = Temperature sensor data is in normal operation.
 * |        |           |1 = Temperature sensor data is in power down.
 * |[29]    |REFUDEN    |Temperature Sensor Reference Data Update Enable Bit
 * |        |           |0 = Write to update TSENSRREF0 and TSENSRREF1 is Disabled.
 * |        |           |1 = Write to update TSENSRREF0 and TSENSRREF1 is Enabled.
 * |[30]    |TSTCON     |Temperature Sensor Test Mode Control
 * |        |           |0 = Temperature sensor test mode Disabled.
 * |        |           |1 = Temperature sensor test mode Enabled.
 * |[31]    |DATAVALID  |Temperature Sensor Data Valid
 * |        |           |0 = Temperature sensor data in TSENSRDATA is not valid.
 * |        |           |1 = Temperature sensor data in TSENSRDATA is valid.
 */
    __IO uint32_t TSENSRFCR;             /*!< [0x0000] Temperature Sensor Function Control Register                                     */
} TS_T;

/** @addtogroup TS_CONST TS Bit Field Definition
    Constant Definitions for TS Controller
  @{
*/

#define TS_CTL_TSENSRREF0_Pos           (0)                                               /*!< TS_T::TSENSRFCR: TSENSRREF0 Position     */
#define TS_CTL_TSENSRREF0_Msk           (0xFFul << TS_CTL_TSENSRREF0_Pos)                 /*!< TS_T::TSENSRFCR: TSENSRREF0 Mask         */

#define TS_CTL_TSENSRREF1_Pos           (8)                                               /*!< TS_T::TSENSRFCR: TSENSRREF1 Position     */
#define TS_CTL_TSENSRREF1_Msk           (0xFFul << TS_CTL_TSENSRREF1_Pos)                 /*!< TS_T::TSENSRFCR: TSENSRREF1 Mask         */

#define TS_CTL_TSENSRDATA_Pos           (16)                                              /*!< TS_T::TSENSRFCR: TSENSRDATA Position     */
#define TS_CTL_TSENSRDATA_Msk           (0xFFFul << TS_CTL_TSENSRDATA_Pos)                /*!< TS_T::TSENSRFCR: TSENSRDATA Mask         */

#define TS_CTL_PD_Pos                   (28)                                              /*!< TS_T::TSENSRFCR: PD Position             */
#define TS_CTL_PD_Msk                   (0x1ul << TS_CTL_PD_Pos)                          /*!< TS_T::TSENSRFCR: PD Mask                 */

#define TS_CTL_REFUDEN_Pos              (29)                                              /*!< TS_T::TSENSRFCR: REFUDEN Position        */
#define TS_CTL_REFUDEN_Msk              (0x1ul << TS_CTL_REFUDEN_Pos)                     /*!< TS_T::TSENSRFCR: REFUDEN Mask            */

#define TS_CTL_TSTCON_Pos               (30)                                              /*!< TS_T::TSENSRFCR: TSTCON Position         */
#define TS_CTL_TSTCON_Msk               (0x1ul << TS_CTL_TSTCON_Pos)                      /*!< TS_T::TSENSRFCR: TSTCON Mask             */

#define TS_CTL_DATAVALID_Pos            (31)                                              /*!< TS_T::TSENSRFCR: DATAVALID Position      */
#define TS_CTL_DATAVALID_Msk            (0x1ul << TS_CTL_DATAVALID_Pos)                   /*!< TS_T::TSENSRFCR: DATAVALID Mask          */


/**@}*/ /* TS_CONST */
/**@}*/ /* end of TS register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __TS_REG_H__ */

/**************************************************************************//**
 * @file     pmic.h
 * @brief    MA35D1 Control PMIC header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __PMIC_H__
#define __PMIC_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup PMIC_Driver PMIC Driver
  @{
*/

/** @addtogroup PMIC_EXPORTED_CONSTANTS PMIC Exported Constants
  @{
*/

#define PMIC_CUSTOMER	0 /*!< User Defined PMIC IC Suppliers */
#define PMIC_DIALOG	    1 /*!< DIALOG PMIC IC */
#define PMIC_IP6103	    2 /*!< IP6103 PMIC IC */
#define PMIC_APW7704F	3 /*!< APW7704F PMIC IC */

#define MA35D1_PMIC PMIC_DIALOG /*!< PMIC IC Suppliers */

#if (MA35D1_PMIC == PMIC_IP6103)
#define PMIC_DEVICE_ADDR  0x60  /*!< PMIC Device Address */
#define PMIC_CPU_REG      0x21  /*!< PMIC set CPU voltage register  */
#define PMIC_CPU_VOL_1_00 0x24  /*!< PMIC set CPU voltage to 1.00V  */
#define PMIC_CPU_VOL_1_10 0x28  /*!< PMIC set CPU voltage to 1.10V  */
#define PMIC_CPU_VOL_1_15 0x2C  /*!< PMIC set CPU voltage to 1.15V  */
#define PMIC_CPU_VOL_1_18 0x2E  /*!< PMIC set CPU voltage to 1.18V  */
#define PMIC_CPU_VOL_1_19 0x2F  /*!< PMIC set CPU voltage to 1.19V  */
#define PMIC_CPU_VOL_1_20 0x30  /*!< PMIC set CPU voltage to 1.20V  */
#define PMIC_CPU_VOL_1_21 0x31  /*!< PMIC set CPU voltage to 1.21V  */
#define PMIC_CPU_VOL_1_22 0x32  /*!< PMIC set CPU voltage to 1.22V  */
#define PMIC_CPU_VOL_1_24 0x33  /*!< PMIC set CPU voltage to 1.24V  */
#define PMIC_CPU_VOL_1_25 0x34  /*!< PMIC set CPU voltage to 1.25V  */
#define PMIC_CPU_VOL_1_26 0x35  /*!< PMIC set CPU voltage to 1.26V  */
#define PMIC_CPU_VOL_1_27 0x36  /*!< PMIC set CPU voltage to 1.27V  */
#define PMIC_CPU_VOL_1_29 0x37  /*!< PMIC set CPU voltage to 1.29V  */
#define PMIC_CPU_VOL_1_30 0x38  /*!< PMIC set CPU voltage to 1.30V  */
#define PMIC_CPU_VOL_1_32 0x3A  /*!< PMIC set CPU voltage to 1.32V  */
#define PMIC_CPU_VOL_1_34 0x3C  /*!< PMIC set CPU voltage to 1.34V  */
#define PMIC_CPU_VOL_1_36 0x3E  /*!< PMIC set CPU voltage to 1.36V  */

#define PMIC_SD_REG       0x42  /*!< PMIC set SD voltage register */
#define PMIC_SD_VOL_1_80  0x2C  /*!< PMIC set SD voltage to 1.8V  */
#define PMIC_SD_VOL_3_30  0x68  /*!< PMIC set SD voltage to 3.3V  */

#define PMIC_LDO_REG       0x41  /*!< PMIC set LDO register           */

#define PMIC_LDO4_REG      0x4A  /*!< PMIC set LDO4 voltage register  */
#define PMIC_LDO4_VOL_1_00 0x0C  /*!< PMIC set LDO4 voltage to 1.00V  */
#define PMIC_LDO4_VOL_1_20 0x14  /*!< PMIC set LDO4 voltage to 1.20V  */
#define PMIC_LDO4_VOL_1_80 0x2C  /*!< PMIC set LDO4 voltage to 1.80V  */
#define PMIC_LDO4_VOL_2_50 0x48  /*!< PMIC set LDO4 voltage to 2.50V  */
#define PMIC_LDO4_VOL_3_30 0x68  /*!< PMIC set LDO4 voltage to 3.30V  */

#define PMIC_LDO5_REG      0x4C  /*!< PMIC set LDO5 voltage register  */
#define PMIC_LDO5_VOL_1_00 0x0C  /*!< PMIC set LDO5 voltage to 1.00V  */
#define PMIC_LDO5_VOL_1_20 0x14  /*!< PMIC set LDO5 voltage to 1.20V  */
#define PMIC_LDO5_VOL_1_80 0x2C  /*!< PMIC set LDO5 voltage to 1.80V  */
#define PMIC_LDO5_VOL_2_50 0x48  /*!< PMIC set LDO5 voltage to 2.50V  */
#define PMIC_LDO5_VOL_3_30 0x68  /*!< PMIC set LDO5 voltage to 3.30V  */

#define PMIC_LDO6_REG      0x4E  /*!< PMIC set LDO6 voltage register  */
#define PMIC_LDO6_VOL_1_00 0x0C  /*!< PMIC set LDO6 voltage to 1.00V  */
#define PMIC_LDO6_VOL_1_20 0x14  /*!< PMIC set LDO6 voltage to 1.20V  */
#define PMIC_LDO6_VOL_1_80 0x2C  /*!< PMIC set LDO6 voltage to 1.80V  */
#define PMIC_LDO6_VOL_2_50 0x48  /*!< PMIC set LDO6 voltage to 2.50V  */
#define PMIC_LDO6_VOL_3_30 0x68  /*!< PMIC set LDO6 voltage to 3.30V  */
#endif

#if (MA35D1_PMIC == PMIC_DIALOG)
#define PMIC_DEVICE_ADDR  0xB0  /*!< PMIC Device Address */
#define PMIC_CPU_REG      0xA4  /*!< PMIC set CPU voltage register  */
#define PMIC_CPU_VOL_1_00 0x46  /*!< PMIC set CPU voltage to 1.00V  */
#define PMIC_CPU_VOL_1_10 0x50  /*!< PMIC set CPU voltage to 1.10V  */
#define PMIC_CPU_VOL_1_15 0x55  /*!< PMIC set CPU voltage to 1.15V  */
#define PMIC_CPU_VOL_1_18 0x58  /*!< PMIC set CPU voltage to 1.18V  */
#define PMIC_CPU_VOL_1_19 0x59  /*!< PMIC set CPU voltage to 1.19V  */
#define PMIC_CPU_VOL_1_20 0x5A  /*!< PMIC set CPU voltage to 1.20V  */
#define PMIC_CPU_VOL_1_21 0x5B  /*!< PMIC set CPU voltage to 1.21V  */
#define PMIC_CPU_VOL_1_22 0x5C  /*!< PMIC set CPU voltage to 1.22V  */
#define PMIC_CPU_VOL_1_23 0x5D  /*!< PMIC set CPU voltage to 1.23V  */
#define PMIC_CPU_VOL_1_24 0x5E  /*!< PMIC set CPU voltage to 1.24V  */
#define PMIC_CPU_VOL_1_25 0x5F  /*!< PMIC set CPU voltage to 1.25V  */
#define PMIC_CPU_VOL_1_26 0x60  /*!< PMIC set CPU voltage to 1.26V  */
#define PMIC_CPU_VOL_1_27 0x61  /*!< PMIC set CPU voltage to 1.27V  */
#define PMIC_CPU_VOL_1_28 0x62  /*!< PMIC set CPU voltage to 1.28V  */
#define PMIC_CPU_VOL_1_29 0x63  /*!< PMIC set CPU voltage to 1.29V  */
#define PMIC_CPU_VOL_1_30 0x64  /*!< PMIC set CPU voltage to 1.30V  */

#define PMIC_SD_REG       0x32  /*!< PMIC set SD voltage register */
#define PMIC_SD_VOL_1_80  0x80  /*!< PMIC set SD voltage to 1.8V  */
#define PMIC_SD_VOL_3_30  0x00  /*!< PMIC set SD voltage to 3.3V  */
#endif

#if (MA35D1_PMIC == PMIC_APW7704F)
#define PMIC_DEVICE_ADDR (0x25 << 1) /*!< PMIC Device Address */
#define PMIC_CPU_REG      0x07  /*!< PMIC set CPU voltage register  */
#define PMIC_CPU_VOL_1_17 0x32  /*!< PMIC set CPU voltage to 1.17V  */
#define PMIC_CPU_VOL_1_20 0x33  /*!< PMIC set CPU voltage to 1.20V  */
#define PMIC_CPU_VOL_1_23 0x34  /*!< PMIC set CPU voltage to 1.23V  */
#define PMIC_CPU_VOL_1_26 0x35  /*!< PMIC set CPU voltage to 1.26V  */
#define PMIC_CPU_VOL_1_29 0x36  /*!< PMIC set CPU voltage to 1.29V  */
#endif

/// @cond HIDDEN_SYMBOLS
/*-----------------------------------------------------------------------------
 * I2C Register's Definition
 *---------------------------------------------------------------------------*/
#define    REG_I2C0_CTL      (I2C0_BASE+0x000)   /*!< I2C Control Register */
#define    REG_I2C0_ADDR0    (I2C0_BASE+0x000)   /*!< I2C Slave Address Register0 */
#define    REG_I2C0_DAT      (I2C0_BASE+0x008)   /*!< I2C Data Register */
#define    REG_I2C0_STATUS   (I2C0_BASE+0x00C)   /*!< I2C Status Register */
#define    REG_I2C0_CLKDIV   (I2C0_BASE+0x010)   /*!< I2C Clock Divided Register */
#define    REG_I2C0_TOCTL    (I2C0_BASE+0x014)   /*!< I2C Time-out Control Register */

/*-----------------------------------------*/
/* global interface variables declarations */
/*-----------------------------------------*/
#define I2C_CTL_STA_SI     0x28UL /*!< I2C_CTL setting for I2C control bits. It would set STA and SI bits     */
#define I2C_CTL_STA_SI_AA  0x2CUL /*!< I2C_CTL setting for I2C control bits. It would set STA, SI and AA bits */
#define I2C_CTL_STO_SI     0x18UL /*!< I2C_CTL setting for I2C control bits. It would set STO and SI bits     */
#define I2C_CTL_STO_SI_AA  0x1CUL /*!< I2C_CTL setting for I2C control bits. It would set STO, SI and AA bits */
#define I2C_CTL_SI         0x08UL /*!< I2C_CTL setting for I2C control bits. It would set SI bit              */
#define I2C_CTL_SI_AA      0x0CUL /*!< I2C_CTL setting for I2C control bits. It would set SI and AA bits      */
#define I2C_CTL_STA        0x20UL /*!< I2C_CTL setting for I2C control bits. It would set STA bit             */
#define I2C_CTL_STO        0x10UL /*!< I2C_CTL setting for I2C control bits. It would set STO bit             */
#define I2C_CTL_AA         0x04UL /*!< I2C_CTL setting for I2C control bits. It would set AA bit              */
#define I2C_CTL_ALL        0x3CUL /*!< I2C_CTL setting for I2C control bits. It would set ALL bit             */

#define I2C_CTL_ENABLE     0x40UL /*!< I2C_CTL setting for I2C control bits. It would set I2C Enable bit      */
/// @endcond HIDDEN_SYMBOLS

/*****************************************************************************
 * PMIC
 *****************************************************************************/

static volatile int pmicIsInit = 0; /*!< \hideinitializer */
static volatile unsigned long pmic_clk; /*!< \hideinitializer */

/*! @}*/ /* end of group PMIC_EXPORTED_CONSTANTS */

/** @addtogroup PMIC_EXPORTED_FUNCTIONS PMIC Exported Functions
  @{
*/

int ma35d1_write_pmic(unsigned char u32DevAddr, unsigned int u32RegAddr, unsigned int value);
int ma35d1_read_pmic(unsigned char u32DevAddr, unsigned int u32RegAddr, unsigned int* u32Data);

/*! @}*/ /* end of group PMIC_EXPORTED_FUNCTIONS */

/*! @}*/ /* end of group PMIC_Driver */

/*! @}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __PMIC_H__ */

/*** (C) COPYRIGHT 2022 Nuvoton Technology Corp. ***/

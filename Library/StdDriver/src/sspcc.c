/**************************************************************************//**
 * @file     sspcc.c
 * @brief    SSPCC driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup SSPCC_Driver SSPCC Driver
  @{
*/

/** @addtogroup SSPCC_EXPORTED_FUNCTIONS SSPCC Exported Functions
  @{
*/

/// @cond HIDDEN_SYMBOLS

#define REG_PSSET0          0x00
#define REG_PSSET1          0x04
#define REG_PSSET2          0x08
#define REG_PSSET3          0x0C
#define REG_PSSET4          0x10
#define REG_PSSET5          0x14
#define REG_PSSET6          0x18
#define REG_PSSET7          0x1C
#define REG_PSSET8          0x20
#define REG_PSSET9          0x24
#define REG_PSSET10         0x28
#define REG_PSSET11         0x2C
#define REG_EBISSET         0x50
#define REG_IOASSET         0x60

struct sspcc_tbl
{
	uint32_t  reg_offs;
	int       bit_offs;
	uint32_t  attrs;
};

struct sspcc_tbl sspcc_table[] =
{
	/* SSPCC_ID_PDMA0   */ { REG_PSSET0,  16,  ATTR_TZS | ATTR_TZNS },
	/* SSPCC_ID_PDMA1   */ { REG_PSSET0,  18,  ATTR_TZS | ATTR_TZNS },
	/* SSPCC_ID_PDMA2   */ { REG_PSSET0,  20,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_PDMA3   */ { REG_PSSET0,  22,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_EBI     */ { REG_PSSET1,   0,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_SDH0    */ { REG_PSSET1,  16,  ATTR_TZS | ATTR_TZNS },
	/* SSPCC_ID_SDH1    */ { REG_PSSET1,  18,  ATTR_TZS | ATTR_TZNS },
	/* SSPCC_ID_NANDC   */ { REG_PSSET1,  20,  ATTR_TZS | ATTR_TZNS },
	/* SSPCC_ID_USBD    */ { REG_PSSET2,   0,  ATTR_TZS | ATTR_TZNS },
	/* SSPCC_ID_CAN0    */ { REG_PSSET3,  24,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_CAN1    */ { REG_PSSET3,  26,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_CAN2    */ { REG_PSSET3,  28,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_CAN3    */ { REG_PSSET3,  30,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_ADC0    */ { REG_PSSET4,   4,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_EADC0   */ { REG_PSSET4,   6,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_WDT1    */ { REG_PSSET4,   8,  ATTR_TZS | ATTR_TZNS },
	/* SSPCC_ID_I2S0    */ { REG_PSSET4,  16,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_I2S1    */ { REG_PSSET4,  18,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_KPI     */ { REG_PSSET4,  20,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_DDRPHY  */ { REG_PSSET4,  24,  ATTR_TZS | ATTR_TZNS },
	/* SSPCC_ID_MCTL    */ { REG_PSSET4,  26,  ATTR_TZS | ATTR_TZNS },
	/* SSPCC_ID_TMR01   */ { REG_PSSET5,   0,  ATTR_TZS | ATTR_TZNS },
	/* SSPCC_ID_TMR23   */ { REG_PSSET5,   2,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_TMR45   */ { REG_PSSET5,   4,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_TMR67   */ { REG_PSSET5,   6,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_TMR89   */ { REG_PSSET5,   8,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_TMR1011 */ { REG_PSSET5,  10,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_EPWM0   */ { REG_PSSET5,  16,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_EPWM1   */ { REG_PSSET5,  18,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_EPWM2   */ { REG_PSSET5,  20,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_SPI0    */ { REG_PSSET6,   0,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_SPI1    */ { REG_PSSET6,   2,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_SPI2    */ { REG_PSSET6,   4,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_SPI3    */ { REG_PSSET6,   6,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_QSPI0   */ { REG_PSSET6,  16,  ATTR_TZS | ATTR_TZNS },
	/* SSPCC_ID_QSPI1   */ { REG_PSSET6,  18,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART0   */ { REG_PSSET7,   0,  ATTR_TZS | ATTR_TZNS },
	/* SSPCC_ID_UART1   */ { REG_PSSET7,   2,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART2   */ { REG_PSSET7,   4,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART3   */ { REG_PSSET7,   6,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART4   */ { REG_PSSET7,   8,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART5   */ { REG_PSSET7,  10,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART6   */ { REG_PSSET7,  12,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART7   */ { REG_PSSET7,  14,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART8   */ { REG_PSSET7,  16,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART9   */ { REG_PSSET7,  18,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART10  */ { REG_PSSET7,  20,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART11  */ { REG_PSSET7,  22,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART12  */ { REG_PSSET7,  24,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART13  */ { REG_PSSET7,  26,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART14  */ { REG_PSSET7,  28,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART15  */ { REG_PSSET7,  30,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_I2C0    */ { REG_PSSET8,   0,  ATTR_TZS | ATTR_TZNS },
	/* SSPCC_ID_I2C1    */ { REG_PSSET8,   2,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_I2C2    */ { REG_PSSET8,   4,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_I2C3    */ { REG_PSSET8,   6,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_I2C4    */ { REG_PSSET8,   8,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_I2C5    */ { REG_PSSET8,  10,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_UART16  */ { REG_PSSET8,  16,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_SC0     */ { REG_PSSET9,   0,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_SC1     */ { REG_PSSET9,   2,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_QEI0    */ { REG_PSSET11,  0,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_QEI1    */ { REG_PSSET11,  2,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_QEI2    */ { REG_PSSET11,  4,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_ECAP0   */ { REG_PSSET11,  8,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_ECAP1   */ { REG_PSSET11, 10,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_ECAP2   */ { REG_PSSET11, 12,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_EBIBK0  */ { REG_EBISSET,  0,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_EBIBK1  */ { REG_EBISSET,  2,  ATTR_TZNS | ATTR_SUBM },
	/* SSPCC_ID_EBIBK2  */ { REG_EBISSET,  4,  ATTR_TZNS | ATTR_SUBM },
};

static uint32_t attr_to_regval(E_SSPCC_ATTR attr)
{
	if (attr == ATTR_TZS)
		return 0;
	if (attr == ATTR_SUBM)
		return 3;
	return 1; /* ATTR_TZNS */
}

static E_SSPCC_ATTR regval_to_attr(uint32_t val)
{
	if (val == 0)
		return ATTR_TZS;
	if (val == 3)
		return ATTR_SUBM;
	if (val == 1)
		return ATTR_TZNS;
	return ATTR_INVALID;
}

/// @endcond HIDDEN_SYMBOLS

/**
 * @brief       Configure secure attribute of the specified IP.
 *
 * @param[in]   id     SSPCC Peripheral ID.
 * @param[in]   attr   Security attribute.
 *
 * @retval      0      Success
 * @retval      -1     Invalid ID
 * @retval      -2     Invalid attribute
 * @retval      -3     Failed to write SSPCC register.
 */
int SSPCC_SetSecureAttribute(E_SSPCC_ID id, E_SSPCC_ATTR attr)
{
	uint32_t  reg_val;

	if ((id < 0) || (id >= SSPCC_ID_LIMIT))
		return -1;

	if (!(sspcc_table[id].attrs & attr))
		return -2;

	reg_val = inpw(SSPCC_BASE + sspcc_table[id].reg_offs);
	reg_val = (reg_val & ~(0x3 << sspcc_table[id].bit_offs)) |
			  (attr_to_regval(attr) << sspcc_table[id].bit_offs);

	outpw(SSPCC_BASE + sspcc_table[id].reg_offs, reg_val);

	if (inpw(SSPCC_BASE + sspcc_table[id].reg_offs) != reg_val)
		return -3;

	return 0;
}

/**
 * @brief       Get secure attribute of the specified IP.
 *
 * @param[in]   id     SSPCC Peripheral ID.
 * @param[in]   attr   The current secure attribute of the specified IP.
 *
 * @retval      0      Success
 * @retval      -1     Invalid ID
 */
int SSPCC_GetSecureAttribute(E_SSPCC_ID id, E_SSPCC_ATTR *attr)
{
	uint32_t  reg_val;

	if ((id < 0) || (id >= SSPCC_ID_LIMIT))
		return -1;

	reg_val = inpw(SSPCC_BASE + sspcc_table[id].reg_offs);

	return regval_to_attr((reg_val >> sspcc_table[id].bit_offs) & 0x3);
}

/**
 * @brief       Set secure attribute of specified GPIO pin
 *
 * @param[in]   id     SSPCC ID of specified GPIO port.
 * @param[in]   pin    pin number
 * @param[in]   attr   Security attribute.
 *
 * @retval      0      Success
 * @retval      -1     Invalid ID
 * @retval      -2     Invalid attribute or pin
 * @retval      -3     Failed to write SSPCC register.
 */
int SSPCC_SetGpioSecureAttribute(E_SSPCC_GPIO_ID id, uint32_t pin, E_SSPCC_ATTR attr)
{
	uint32_t  reg_val;

	if ((SSPCC_ID_GPIOA < 0) || (id > SSPCC_ID_GPION))
		return -1;

	if (!((ATTR_TZS | ATTR_TZNS | ATTR_SUBM) & attr))
		return -2;

	if (pin > 15)
		return -2;

	reg_val = inpw(SSPCC_BASE + REG_IOASSET + id * 4);
	reg_val = (reg_val & ~(0x3 << (pin * 2))) |
			  (attr_to_regval(attr) << (pin * 2));

	outpw(SSPCC_BASE + REG_IOASSET + id * 4, reg_val);

	if (inpw(SSPCC_BASE + REG_IOASSET + id * 4) != reg_val)
		return -3;

	return 0;
}

/**
 * @brief       Get secure attribute of specified GPIO pin
 *
 * @param[in]   id     SSPCC ID of specified GPIO port.
 * @param[in]   pin    pin number
 * @param[in]   attr   The current security attribute of specified GPIO pin.
 *
 * @retval      0      Success
 * @retval      -1     Invalid ID
 * @retval      -2     Invalid pin
 *
 * @details     This function is used to set specified IO realm.
 */
int SSPCC_GetGpioSecureAttribute(E_SSPCC_GPIO_ID id, uint32_t pin, E_SSPCC_ATTR *attr)
{
	uint32_t  reg_val;

	if ((SSPCC_ID_GPIOA < 0) || (id > SSPCC_ID_GPION))
		return -1;

	if (pin > 15)
		return -2;

	reg_val = inpw(SSPCC_BASE + REG_IOASSET + id * 4);

	return regval_to_attr((reg_val >> (pin * 2)) & 0x3);
}

/*@}*/ /* end of group SSPCC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group SSPCC_Driver */

/*@}*/ /* end of group Standard_Driver */


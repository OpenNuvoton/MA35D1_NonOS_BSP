/**************************************************************************//**
 * @file     sspcc.h
 * @brief    SSPCC driver header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright(C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __SSPCC_H__
#define __SSPCC_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup SSPCC_Driver SSPCC Driver
  @{
*/

/** @addtogroup SSPCC_EXPORTED_CONSTANTS SSPCC Exported Constants
  @{
*/

/*! SSPCC security attribute */
typedef enum
{
	ATTR_TZS           = 0x01, /*!< Attribute secure-only \hideinitializer */
	ATTR_TZNS          = 0x02, /*!< Attribute non-secure \hideinitializer */
	ATTR_SUBM          = 0x04, /*!< Attribute SubM \hideinitializer */
	ATTR_INVALID       = 0x03  /*!< Invalid attribute \hideinitializer */
} E_SSPCC_ATTR;

/*! SSPCC ID for peripheral controllers */
typedef enum
{
	SSPCC_ID_PDMA0     = 0,   /*!< SSPCC ID of PDMA0 \hideinitializer */
	SSPCC_ID_PDMA1     = 1,   /*!< SSPCC ID of PDMA1 \hideinitializer */
	SSPCC_ID_PDMA2     = 2,   /*!< SSPCC ID of PDMA2 \hideinitializer */
	SSPCC_ID_PDMA3     = 3,   /*!< SSPCC ID of PDMA3 \hideinitializer */
	SSPCC_ID_EBI       = 4,   /*!< SSPCC ID of EBI \hideinitializer */
	SSPCC_ID_SDH0      = 5,   /*!< SSPCC ID of SDH0 \hideinitializer */
	SSPCC_ID_SDH1      = 6,   /*!< SSPCC ID of SDH1 \hideinitializer */
	SSPCC_ID_NANDC     = 7,   /*!< SSPCC ID of NAND \hideinitializer */
	SSPCC_ID_USBD      = 8,   /*!< SSPCC ID of USBD \hideinitializer */
	SSPCC_ID_CAN0      = 9,   /*!< SSPCC ID of CAN0 \hideinitializer */
	SSPCC_ID_CAN1      = 10,  /*!< SSPCC ID of CAN1 \hideinitializer */
	SSPCC_ID_CAN2      = 11,  /*!< SSPCC ID of CAN2 \hideinitializer */
	SSPCC_ID_CAN3      = 12,  /*!< SSPCC ID of CAN3 \hideinitializer */
	SSPCC_ID_ADC0      = 13,  /*!< SSPCC ID of ADC0 \hideinitializer */
	SSPCC_ID_EADC0     = 14,  /*!< SSPCC ID of EADC0 \hideinitializer */
	SSPCC_ID_WDT1      = 15,  /*!< SSPCC ID of WDT1 \hideinitializer */
	SSPCC_ID_I2S0      = 16,  /*!< SSPCC ID of I2S0 \hideinitializer */
	SSPCC_ID_I2S1      = 17,  /*!< SSPCC ID of I2S1 \hideinitializer */
	SSPCC_ID_KPI       = 18,  /*!< SSPCC ID of KPI \hideinitializer */
	SSPCC_ID_DDRPHY    = 19,  /*!< SSPCC ID of DDRPHY \hideinitializer */
	SSPCC_ID_MCTL      = 20,  /*!< SSPCC ID of MCTL \hideinitializer */
	SSPCC_ID_TMR01     = 21,  /*!< SSPCC ID of TMR01 \hideinitializer */
	SSPCC_ID_TMR23     = 22,  /*!< SSPCC ID of TMR23 \hideinitializer */
	SSPCC_ID_TMR45     = 23,  /*!< SSPCC ID of TMR45 \hideinitializer */
	SSPCC_ID_TMR67     = 24,  /*!< SSPCC ID of TMR67 \hideinitializer */
	SSPCC_ID_TMR89     = 25,  /*!< SSPCC ID of TMR89 \hideinitializer */
	SSPCC_ID_TMR1011   = 26,  /*!< SSPCC ID of TMR1011 \hideinitializer */
	SSPCC_ID_EPWM0     = 27,  /*!< SSPCC ID of EPWM0 \hideinitializer */
	SSPCC_ID_EPWM1     = 28,  /*!< SSPCC ID of EPWM1 \hideinitializer */
	SSPCC_ID_EPWM2     = 29,  /*!< SSPCC ID of EPWM2 \hideinitializer */
	SSPCC_ID_SPI0      = 30,  /*!< SSPCC ID of SPI0 \hideinitializer */
	SSPCC_ID_SPI1      = 31,  /*!< SSPCC ID of SPI1 \hideinitializer */
	SSPCC_ID_SPI2      = 32,  /*!< SSPCC ID of SPI2 \hideinitializer */
	SSPCC_ID_SPI3      = 33,  /*!< SSPCC ID of SPI3 \hideinitializer */
	SSPCC_ID_QSPI0     = 34,  /*!< SSPCC ID of QSPI0 \hideinitializer */
	SSPCC_ID_QSPI1     = 35,  /*!< SSPCC ID of QSPI1 \hideinitializer */
	SSPCC_ID_UART0     = 36,  /*!< SSPCC ID of UART0 \hideinitializer */
	SSPCC_ID_UART1     = 37,  /*!< SSPCC ID of UART1 \hideinitializer */
	SSPCC_ID_UART2     = 38,  /*!< SSPCC ID of UART2 \hideinitializer */
	SSPCC_ID_UART3     = 39,  /*!< SSPCC ID of UART3 \hideinitializer */
	SSPCC_ID_UART4     = 40,  /*!< SSPCC ID of UART4 \hideinitializer */
	SSPCC_ID_UART5     = 41,  /*!< SSPCC ID of UART5 \hideinitializer */
	SSPCC_ID_UART6     = 42,  /*!< SSPCC ID of UART6 \hideinitializer */
	SSPCC_ID_UART7     = 43,  /*!< SSPCC ID of UART7 \hideinitializer */
	SSPCC_ID_UART8     = 44,  /*!< SSPCC ID of UART8 \hideinitializer */
	SSPCC_ID_UART9     = 45,  /*!< SSPCC ID of UART9 \hideinitializer */
	SSPCC_ID_UART10    = 46,  /*!< SSPCC ID of UART10 \hideinitializer */
	SSPCC_ID_UART11    = 47,  /*!< SSPCC ID of UART11 \hideinitializer */
	SSPCC_ID_UART12    = 48,  /*!< SSPCC ID of UART12 \hideinitializer */
	SSPCC_ID_UART13    = 49,  /*!< SSPCC ID of UART13 \hideinitializer */
	SSPCC_ID_UART14    = 50,  /*!< SSPCC ID of UART14 \hideinitializer */
	SSPCC_ID_UART15    = 51,  /*!< SSPCC ID of UART15 \hideinitializer */
	SSPCC_ID_I2C0      = 52,  /*!< SSPCC ID of I2C0 \hideinitializer */
	SSPCC_ID_I2C1      = 53,  /*!< SSPCC ID of I2C1 \hideinitializer */
	SSPCC_ID_I2C2      = 54,  /*!< SSPCC ID of I2C2 \hideinitializer */
	SSPCC_ID_I2C3      = 55,  /*!< SSPCC ID of I2C3 \hideinitializer */
	SSPCC_ID_I2C4      = 56,  /*!< SSPCC ID of I2C4 \hideinitializer */
	SSPCC_ID_I2C5      = 57,  /*!< SSPCC ID of I2C5 \hideinitializer */
	SSPCC_ID_UART16    = 58,  /*!< SSPCC ID of UART16 \hideinitializer */
	SSPCC_ID_SC0       = 59,  /*!< SSPCC ID of SC0 \hideinitializer */
	SSPCC_ID_SC1       = 60,  /*!< SSPCC ID of SC1 \hideinitializer */
	SSPCC_ID_QEI0      = 61,  /*!< SSPCC ID of QEI0 \hideinitializer */
	SSPCC_ID_QEI1      = 62,  /*!< SSPCC ID of QEI1 \hideinitializer */
	SSPCC_ID_QEI2      = 63,  /*!< SSPCC ID of QEI2 \hideinitializer */
	SSPCC_ID_ECAP0     = 64,  /*!< SSPCC ID of ECAP0 \hideinitializer */
	SSPCC_ID_ECAP1     = 65,  /*!< SSPCC ID of ECAP1 \hideinitializer */
	SSPCC_ID_ECAP2     = 66,  /*!< SSPCC ID of ECAP2 \hideinitializer */
	SSPCC_ID_EBIBK0    = 67,  /*!< SSPCC ID of EBI BANK0 \hideinitializer */
	SSPCC_ID_EBIBK1    = 68,  /*!< SSPCC ID of EBI BANK1 \hideinitializer */
	SSPCC_ID_EBIBK2    = 69,  /*!< SSPCC ID of EBI BANK2 \hideinitializer */
	SSPCC_ID_LIMIT     = 70
} E_SSPCC_ID;

/*! SSPCC ID for peripheral controllers */
typedef enum
{
	SSPCC_ID_GPIOA     =  0,  /*!< SSPCC GPIO ID of GPIOA \hideinitializer */
	SSPCC_ID_GPIOB     =  1,  /*!< SSPCC GPIO ID of GPIOB \hideinitializer */
	SSPCC_ID_GPIOC     =  2,  /*!< SSPCC GPIO ID of GPIOC \hideinitializer */
	SSPCC_ID_GPIOD     =  3,  /*!< SSPCC GPIO ID of GPIOD \hideinitializer */
	SSPCC_ID_GPIOE     =  4,  /*!< SSPCC GPIO ID of GPIOE \hideinitializer */
	SSPCC_ID_GPIOF     =  5,  /*!< SSPCC GPIO ID of GPIOF \hideinitializer */
	SSPCC_ID_GPIOG     =  6,  /*!< SSPCC GPIO ID of GPIOG \hideinitializer */
	SSPCC_ID_GPIOH     =  7,  /*!< SSPCC GPIO ID of GPIOH \hideinitializer */
	SSPCC_ID_GPIOI     =  8,  /*!< SSPCC GPIO ID of GPIOI \hideinitializer */
	SSPCC_ID_GPIOJ     =  9,  /*!< SSPCC GPIO ID of GPIOJ \hideinitializer */
	SSPCC_ID_GPIOK     = 10,  /*!< SSPCC GPIO ID of GPIOK \hideinitializer */
	SSPCC_ID_GPIOL     = 11,  /*!< SSPCC GPIO ID of GPIOL \hideinitializer */
	SSPCC_ID_GPIOM     = 12,  /*!< SSPCC GPIO ID of GPIOM \hideinitializer */
	SSPCC_ID_GPION     = 13,  /*!< SSPCC GPIO ID of GPION \hideinitializer */
} E_SSPCC_GPIO_ID;

/*! @}*/ /* end of group SSPCC_EXPORTED_CONSTANTS */

/** @addtogroup SSPCC_EXPORTED_FUNCTIONS SSPCC Exported Functions
  @{
*/

int SSPCC_SetSecureAttribute(E_SSPCC_ID id, E_SSPCC_ATTR attr);
int SSPCC_GetSecureAttribute(E_SSPCC_ID id, E_SSPCC_ATTR *attr);
int SSPCC_SetGpioSecureAttribute(E_SSPCC_GPIO_ID id, uint32_t pin, E_SSPCC_ATTR attr);
int SSPCC_GetGpioSecureAttribute(E_SSPCC_GPIO_ID id, uint32_t pin, E_SSPCC_ATTR *attr);

/*! @}*/ /* end of group SSPCC_EXPORTED_FUNCTIONS */

/*! @}*/ /* end of group SSPCC_Driver */

/*! @}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __SSPCC_H__ */


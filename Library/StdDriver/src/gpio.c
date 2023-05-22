/**************************************************************************//**
 * @file     gpio.c
 * @brief    GPIO driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup GPIO_Driver GPIO Driver
  @{
*/

/** @addtogroup GPIO_EXPORTED_FUNCTIONS GPIO Exported Functions
  @{
*/

/**
 * @brief       Set GPIO operation mode
 *
 * @param[in]   port        GPIO port. It could be PA, PB, PC, PD, PE, PF, PG or PH.
 * @param[in]   u32PinMask  The single or multiple pins of specified GPIO port.
 *                          It could be BIT0 ~ BIT15 for PA, PB, PC, PD, PF and PH GPIO port.
 *                          It could be BIT0 ~ BIT13 for PE GPIO port.
 *                          It could be BIT0 ~ BIT11 for PG GPIO port.
 * @param[in]   u32Mode     Operation mode.  It could be \n
 *                          GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_OPEN_DRAIN, GPIO_MODE_QUASI.
 *
 * @return      None
 *
 * @details     This function is used to set specified GPIO operation mode.
 */
void GPIO_SetMode(GPIO_T *port, uint32_t u32PinMask, uint32_t u32Mode)
{
    uint32_t i;

    for(i = 0ul; i < GPIO_PIN_MAX; i++)
    {
        if((u32PinMask & (1ul << i))==(1ul << i))
        {
            port->MODE = (port->MODE & ~(0x3ul << (i << 1))) | (u32Mode << (i << 1));
        }
    }
}

/**
 * @brief       Enable GPIO interrupt
 *
 * @param[in]   port        GPIO port. It could be PA, PB, PC, PD, PE, PF, PG or PH.
 * @param[in]   u32Pin      The pin of specified GPIO port.
 *                          It could be 0 ~ 15 for PA, PB, PC, PD, PF and PH GPIO port.
 *                          It could be 0 ~ 13 for PE GPIO port.
 *                          It could be 0 ~ 11 for PG GPIO port.
 * @param[in]   u32IntAttribs   The interrupt attribute of specified GPIO pin. It could be \n
 *                              GPIO_INT_RISING, GPIO_INT_FALLING, GPIO_INT_BOTH_EDGE, GPIO_INT_HIGH, GPIO_INT_LOW.
 *
 * @return      None
 *
 * @details     This function is used to enable specified GPIO pin interrupt.
 */
void GPIO_EnableInt(GPIO_T *port, uint32_t u32Pin, uint32_t u32IntAttribs)
{
    port->INTTYPE = (port->INTTYPE&~(1ul<<u32Pin)) | (((u32IntAttribs >> 24) & 0xFFUL) << u32Pin);
    port->INTEN = (port->INTEN&~(0x00010001ul<<u32Pin)) | ((u32IntAttribs & 0xFFFFFFUL) << u32Pin);
}


/**
 * @brief       Disable GPIO interrupt
 *
 * @param[in]   port        GPIO port. It could be PA, PB, PC, PD, PE, PF, PG or PH.
 * @param[in]   u32Pin      The pin of specified GPIO port.
 *                          It could be 0 ~ 15 for PA, PB, PC, PD, PF and PH GPIO port.
 *                          It could be 0 ~ 13 for PE GPIO port.
 *                          It could be 0 ~ 11 for PG GPIO port.
 *
 * @return      None
 *
 * @details     This function is used to disable specified GPIO pin interrupt.
 */
void GPIO_DisableInt(GPIO_T *port, uint32_t u32Pin)
{
    port->INTTYPE &= ~(1UL << u32Pin);
    port->INTEN &= ~((0x00010001UL) << u32Pin);
}

/**
 * @brief       Set GPIO slew rate control
 *
 * @param[in]   port        GPIO port. It could be \ref PA, \ref PB, ... or \ref PH
 * @param[in]   u32Pin      The pin of specified GPIO port. It could be 0 ~ 15.
 * @param[in]   u32Mode     Slew rate mode. \ref GPIO_SLEWCTL_NORMAL
 *                                          \ref GPIO_SLEWCTL_HIGH
 *
 * @return      None
 *
 * @details     This function is used to set specified GPIO operation mode.
 */
void GPIO_SetSlewCtl(GPIO_T *port, uint32_t u32Pin, uint32_t u32Mode)
{
	port->SLEWCTL = (port->SLEWCTL & ~(0x3ul << (u32Pin << 1))) | (u32Mode << (u32Pin << 1));
}

/**
 * @brief       Set GPIO drive strength control
 *
 * @param[in]   port        GPIO port. It could be \ref PA, \ref PB, ... or \ref PH
 * @param[in]   u32Pin      The pin of specified GPIO port. It could be 0 ~ 15.
 * @param[in]   u32Mode     The drive strength of specified GPIO pin. It could be 0 ~ 7.
 *
 * @return      None
 *
 * @details     This function is used to set specified GPIO operation mode.
 */
void GPIO_SetDriveStrength(GPIO_T *port, uint32_t u32Pin, uint32_t u32Mode)
{
	if(u32Pin<=7)
		port->DSL = (port->DSL & ~(0x7ul << (u32Pin << 2))) | (u32Mode << (u32Pin << 2));
	else
		port->DSH = (port->DSH & ~(0x7ul << ((u32Pin-7) << 2))) | (u32Mode << ((u32Pin-7) << 2));
}

/**
 * @brief       Set GPIO Power mode control
 *
 * @param[in]   port        GPIO port. It could be \ref PA, \ref PB, ... or \ref PH
 * @param[in]   u32Pin      The pin of specified GPIO port. It could be 0 ~ 15.
 * @param[in]   u32Mode     Power mode. \ref GPIO_SPW_1_8_V
 *                                      \ref GPIO_SPW_3_3_V
 *
 * @return      None
 *
 * @details     This function is used to set specified GPIO power mode.
 */
void GPIO_SetSPW(GPIO_T *port, uint32_t u32Pin, uint32_t u32Mode)
{
	port->SPW = (port->SPW & ~(0x1ul << u32Pin)) | (u32Mode << u32Pin);
}


/**
 * @brief       Set GPIO Pull-up and Pull-down control
 *
 * @param[in]   port          GPIO port. It could be \ref PA, \ref PB, ... or \ref PH
 * @param[in]   u32Pin    The pin of specified GPIO port. It could be 0 ~ 15.
 * @param[in]   u32Mode       The pin mode of specified GPIO pin. It could be
 *                                \ref GPIO_PUSEL_DISABLE
 *                                \ref GPIO_PUSEL_PULL_UP
 *                                \ref GPIO_PUSEL_PULL_DOWN
 *
 * @return      None
 *
 * @details     Set the pin mode of specified GPIO pin.
 */
void GPIO_SetPullCtl(GPIO_T *port, uint32_t u32Pin, uint32_t u32Mode)
{
	port->PUSEL = (port->PUSEL & ~(0x3ul << (u32Pin << 1))) | (u32Mode << (u32Pin << 1));
}

/*@}*/ /* end of group GPIO_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group GPIO_Driver */

/*@}*/ /* end of group Standard_Driver */


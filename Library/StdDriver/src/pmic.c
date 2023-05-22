/**************************************************************************//**
 * @file     pmic.c
 * @version  V1.00
 * @brief    MA35D1 Control PMIC source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2022 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "MA35D1.h"
#include "pmic.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup PMIC_Driver PMIC Driver
  @{
*/

/** @addtogroup PMIC_EXPORTED_FUNCTIONS PMIC Exported Functions
  @{
*/

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

#define RETRY_COUNT 3
/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
unsigned char g_u8PMIC_DeviceAddr = 0xB0;
unsigned char g_u8PMIC_PageNum;
unsigned char g_u8PMIC_RegAddr;
unsigned char g_u8PMIC_WriteData;

volatile unsigned char g_u8PMIC_RxData;
volatile unsigned char g_u8PMIC_DataLen;
volatile unsigned char g_u8PMIC_EndFlag = 0;

/**
  * @brief    I2C Rx Callback Function.
  * @param[in]  u32Status    I2C status.
  *
  * @details  The function is the I2C master mode rx callback function.
  */
void PMIC_I2C_MasterRx(unsigned int u32Status)
{
    if (u32Status == 0x08)
    {   /* START has been transmitted and prepare SLA+W */
        outp32((void *)REG_I2C0_DAT, g_u8PMIC_DeviceAddr|(g_u8PMIC_PageNum << 1));
        outp32((void *)REG_I2C0_CTL, (inp32((void *)REG_I2C0_CTL)& ~0x3c) | I2C_CTL_SI);
    }
    else if (u32Status == 0x18)
    {   /* SLA+W has been transmitted and ACK has been received */
        outp32((void *)REG_I2C0_DAT, g_u8PMIC_RegAddr);
        outp32((void *)REG_I2C0_CTL, (inp32((void *)REG_I2C0_CTL)& ~0x3c) | I2C_CTL_SI);
    }
    else if (u32Status == 0x20)
    {   /* SLA+W has been transmitted and NACK has been received */
        outp32((void *)REG_I2C0_CTL, (inp32((void *)REG_I2C0_CTL)& ~0x3c)|(I2C_CTL_STA|I2C_CTL_STO|I2C_CTL_SI));
    }
    else if (u32Status == 0x28)
    {   /* DATA has been transmitted and ACK has been received */
        outp32((void *)REG_I2C0_CTL, (inp32((void *)REG_I2C0_CTL)& ~0x3c)|(I2C_CTL_STA|I2C_CTL_SI)); // repeat start
    }
    else if (u32Status == 0x10)
    {   /* Repeat START has been transmitted and prepare SLA+R */
        outp32((void *)REG_I2C0_DAT, (g_u8PMIC_DeviceAddr|(g_u8PMIC_PageNum << 1))|0x01);
        outp32((void *)REG_I2C0_CTL, (inp32((void *)REG_I2C0_CTL)& ~0x3c) | I2C_CTL_SI);
    }
    else if (u32Status == 0x40)
    {   /* SLA+R has been transmitted and ACK has been received */
        outp32((void *)REG_I2C0_CTL, (inp32((void *)REG_I2C0_CTL)& ~0x3c) | I2C_CTL_SI);
    }
    else if (u32Status == 0x58)
    {   /* DATA has been received and NACK has been returned */
        g_u8PMIC_RxData = inp32((void *)REG_I2C0_DAT);
        outp32((void *)REG_I2C0_CTL, (inp32((void *)REG_I2C0_CTL)& ~0x3c)|(I2C_CTL_STO|I2C_CTL_SI));
        g_u8PMIC_EndFlag = 1;
    }
    else
    {
        //sysprintf("Status 0x%x is NOT processed\n", u32Status);
    }
}

/**
  * @brief    I2C Tx Callback Function.
  * @param[in]  u32Status    I2C status.
  *
  * @details  The function is the I2C master mode tx callback function.
  */
void PMIC_I2C_MasterTx(unsigned int u32Status)
{
    if (u32Status == 0x08)
    {   /* START has been transmitted */
        outp32((void *)REG_I2C0_DAT, g_u8PMIC_DeviceAddr|(g_u8PMIC_PageNum << 1));  /* Write SLA+W to Register I2CDAT */
        outp32((void *)REG_I2C0_CTL, (inp32((void *)REG_I2C0_CTL)& ~0x3c) | I2C_CTL_SI);
    }
    else if (u32Status == 0x18)
    {   /* SLA+W has been transmitted and ACK has been received */
        outp32((void *)REG_I2C0_DAT, g_u8PMIC_RegAddr);
        outp32((void *)REG_I2C0_CTL, (inp32((void *)REG_I2C0_CTL)& ~0x3c) | I2C_CTL_SI);
    }
    else if (u32Status == 0x20)
    {   /* SLA+W has been transmitted and NACK has been received */
        outp32((void *)REG_I2C0_CTL, (inp32((void *)REG_I2C0_CTL)& ~0x3c)|(I2C_CTL_STA|I2C_CTL_STO|I2C_CTL_SI));
    }
    else if (u32Status == 0x28)
    {   /* DATA has been transmitted and ACK has been received */
        if ((g_u8PMIC_DataLen == 0) && (g_u8PMIC_EndFlag == 0))
        {
            outp32((void *)REG_I2C0_DAT, g_u8PMIC_WriteData);
            outp32((void *)REG_I2C0_CTL, (inp32((void *)REG_I2C0_CTL)& ~0x3c) | I2C_CTL_SI);
            g_u8PMIC_DataLen++;
        }
        else
        {
            g_u8PMIC_DataLen = 0;
            outp32((void *)REG_I2C0_CTL, (inp32((void *)REG_I2C0_CTL)& ~0x3c)|(I2C_CTL_STO|I2C_CTL_SI));
            g_u8PMIC_EndFlag = 1;
        }
    }
    else
    {
        //sysprintf("Status 0x%x is NOT processed\n", u32Status);
    }
}

/**
  * @brief      Send data via I2C.
  * @param[in]  u32Addr    I2C protocol address.
  * @param[in]  u32Data    Data sent via i2c.
  *
  * @return   TRUE : I2C sends data successfully
  *           FALSE : I2C failed to send data
  *
  * @details  The function is to send data through I2C.
  */
unsigned int ma35d1_write_i2c_data(unsigned int u32Addr, unsigned int u32Data)
{
    unsigned int I2C_TIME_OUT_COUNT = 0x20000;
    unsigned int u32Status;
    unsigned int u32time_out = 0;

    g_u8PMIC_PageNum = 0;

    g_u8PMIC_RegAddr     = u32Addr;
    g_u8PMIC_WriteData   = u32Data;
    g_u8PMIC_EndFlag     = 0x0;

    outp32((void *)REG_I2C0_CTL, (inp32((void *)REG_I2C0_CTL)& ~0x3c) | I2C_CTL_STA);

    while(1)
    {
        if (inp32((void *)REG_I2C0_CTL) & I2C_CTL_SI)
        {
            u32time_out = 0;
            u32Status = inp32((void *)REG_I2C0_STATUS);
            PMIC_I2C_MasterTx(u32Status);
        }

        if (g_u8PMIC_EndFlag)
        {
            break;
        }
        u32time_out++;
        if (u32time_out > I2C_TIME_OUT_COUNT)
        {
            sysprintf("i2c Write Time Out!\n");
            return FALSE;
        }
    }
    return TRUE;
}

/**
  * @brief      Read data via I2C.
  * @param[in]  u32Addr    I2C protocol address.
  * @param[in]  u32Data    Data read via i2c.
  *
  * @return   TRUE : I2C sends data successfully
  *           FALSE : I2C failed to read data
  *
  * @details  The function is to read data through I2C.
  */
unsigned int ma35d1_read_i2c_data(unsigned int u32Addr, unsigned int* u32Data)
{
    unsigned int I2C_TIME_OUT_COUNT = 0x200000;
    unsigned int u32Status;
    unsigned int u32time_out = 0;

    g_u8PMIC_PageNum = 0;

    g_u8PMIC_RegAddr     = u32Addr;
    g_u8PMIC_EndFlag     = 0x0;

    outp32((void *)REG_I2C0_CTL, (inp32((void *)REG_I2C0_CTL)& ~0x3c) | I2C_CTL_STA);

    while(1)
    {
        if (inp32((void *)REG_I2C0_CTL) & I2C_CTL_SI)
        {
            u32time_out = 0;
            u32Status = inp32((void *)REG_I2C0_STATUS);
            PMIC_I2C_MasterRx(u32Status);
        }

        if (g_u8PMIC_EndFlag)
        {
            break;
        }

        u32time_out++;
        if (u32time_out > I2C_TIME_OUT_COUNT)
        {
            sysprintf("i2c Read Time Out!\n");
            return FALSE;
        }
    }

    *u32Data = g_u8PMIC_RxData;
    return TRUE;
}

/**
  * @brief      Read data from PMIC.
  * @param[in]  u32Addr    PMIC register address.
  * @param[in]  u32Data    Data read from PMIC registers.
  *
  * @return   TRUE : Read data from PMIC successfully
  *           FALSE : Failed to read data from PMIC
  *
  * @details  The function is to read data from PMIC.
  */
unsigned int ma35d1_read_pmic_data(unsigned int u32Addr, unsigned int* u32Data)
{
    unsigned int j = RETRY_COUNT;

    while(j-- > 0)
    {
        if (ma35d1_read_i2c_data(u32Addr, u32Data) == 2)
        {
            break;
        }
    }

    if (j <= 0)
    {
        sysprintf("\n READ Fail! \n");
        return FALSE;
    }

    return TRUE;
}

/**
  * @brief      Write data to PMIC.
  * @param[in]  u32Addr    PMIC register address.
  * @param[in]  u32Data    Data to send to PMIC registers.
  *
  * @return   TRUE : Write data to PMIC successfully
  *           FALSE : Failed to write data to PMIC
  *
  * @details  The function is to write data to PMIC.
  */
unsigned int ma35d1_write_pmic_data(unsigned int u32Addr, unsigned int u32Data)
{
    unsigned int j = RETRY_COUNT;

    while (j-- > 0)
    {
        if (ma35d1_write_i2c_data(u32Addr, u32Data) == 1)
        {
            break;
        }
    }

    if (j <= 0)
    {
        sysprintf("\n WRITE Fail! \n");
        return FALSE;
    }

    return TRUE;
}

/// @endcond HIDDEN_SYMBOLS

/**
  * @brief      Initial I2C module.
  * @param[in]  sys_clk    I2C clock source frequency.
  *
  * @details  The I2C initial function.
  */
void ma35d1_i2c0_init(unsigned int sys_clk)
{
    unsigned long clk_rate;
    unsigned int u32Div, speed;

    outp32((void *)0X40460208, inp32((void *)0X40460208) | (0x3fff << 16)); // enable GPIO clock
    outp32((void *)0X40460210, inp32((void *)0X40460210) | (0x1 << 0));  // I2C0 CLK

    outp32((void *)0x40460098, ((inp32((void *)0x40460098) & ~0x0f000000) | (0x6<<24))); // PD.6 I2C0_SDA
    outp32((void *)0x40460098, ((inp32((void *)0x40460098) & ~0xf0000000) | (0x6<<28))); // PD.7 I2C0_CLK
    outp32((void *)0x400400F0, 0x5 << 12); // pull high

    /* i2c_clk = 100KHz */
    clk_rate = sys_clk;
    speed = 100*1000;

    /* assume speed above 1000 are Hz-specified */
    if (speed > 1000) speed = speed/1000;
    if (speed > 400) speed = 400;

    u32Div = (unsigned int)(((clk_rate * 10U) / (speed * 4U) + 5U) / 10U - 1U);

    outp32((void *)REG_I2C0_CLKDIV, u32Div);
    outp32((void *)REG_I2C0_CTL, inp32((void *)REG_I2C0_CTL) | I2C_CTL_ENABLE); /* i2c enable */
}

/**
  * @brief      Set customer PMIC.
  * @param[in]  u32DevAddr    Customer PMIC device address
  * @param[in]  u32RegAddr    The PMIC register address
  * @param[in]  value         The PMIC register value
  *
  * @return   TRUE : Write data to PMIC successfully
  *           FALSE : Failed to write data to PMIC
  *
  * @details  Set customer PMIC register.
  */
int ma35d1_write_pmic(unsigned char u32DevAddr, unsigned int u32RegAddr, unsigned int value)
{
    int ret = 0;

    if(pmicIsInit==0)
    {
        pmic_clk = 180000000;
        ma35d1_i2c0_init(pmic_clk);
        pmicIsInit=1;
    }

    g_u8PMIC_DeviceAddr = u32DevAddr;
    ret = ma35d1_write_pmic_data(u32RegAddr, value);

    return ret;
}

/**
  * @brief      Read customer PMIC register value.
  * @param[in]  u32DevAddr    Customer PMIC device address
  * @param[in]  u32RegAddr    The PMIC register address
  * @param[in]  u32Data       The PMIC register value
  *
  * @return   TRUE : Read data from PMIC successfully
  *           FALSE : Failed to read data from PMIC
  *
  * @details  Get customer PMIC register data.
  */
int ma35d1_read_pmic(unsigned char u32DevAddr, unsigned int u32RegAddr, unsigned int* u32Data)
{
    int ret = 0;

    if(pmicIsInit==0)
    {
        pmic_clk = 180000000;
        ma35d1_i2c0_init(pmic_clk);
        pmicIsInit=1;
    }

    g_u8PMIC_DeviceAddr = u32DevAddr;
    ret = ma35d1_read_pmic_data(u32RegAddr, u32Data);
    return ret;
}


/*! @}*/ /* end of group PMIC_EXPORTED_FUNCTIONS */

/*! @}*/ /* end of group PMIC_Driver */

/*! @}*/ /* end of group Standard_Driver */


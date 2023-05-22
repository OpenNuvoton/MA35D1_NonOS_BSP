/**************************************************************************//**
 * @file     main.c
 *
 * @brief    I2C Driver Sample Code
 *           This is a I2C master mode demo and need to be tested with a slave device.
 *
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_u8DeviceAddr;
uint8_t g_au8TxData[3];
volatile uint8_t g_u8RxData;
volatile uint8_t g_u8DataLen;
volatile uint8_t g_u8EndFlag = 0;

typedef void (*I2C_FUNC)(uint32_t u32Status);

static volatile I2C_FUNC s_I2C1HandlerFn = NULL;

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C1_IRQHandler(void)
{
    uint32_t u32Status;

    u32Status = I2C_GET_STATUS(I2C1);

    if (I2C_GET_TIMEOUT_FLAG(I2C1))
    {
        /* Clear I2C0 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C1);
    }
    else
    {
        if (s_I2C1HandlerFn != NULL)
            s_I2C1HandlerFn(u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Rx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterRx(uint32_t u32Status)
{
    if (u32Status == 0x08)                      /* START has been transmitted and prepare SLA+W */
    {
        I2C_SET_DATA(I2C1, (g_u8DeviceAddr << 1)); /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI);
    }
    else if (u32Status == 0x18)                 /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI);
    }
    else if (u32Status == 0x20)                 /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_STA | I2C_CTL_STO | I2C_CTL_SI);
    }
    else if (u32Status == 0x28)                 /* DATA has been transmitted and ACK has been received */
    {
        if (g_u8DataLen != 2)
        {
            I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
            I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI);
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C1, I2C_CTL_STA | I2C_CTL_SI);
        }
    }
    else if (u32Status == 0x10)                 /* Repeat START has been transmitted and prepare SLA+R */
    {
        I2C_SET_DATA(I2C1, (g_u8DeviceAddr << 1) | 0x01);  /* Write SLA+R to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI);
    }
    else if (u32Status == 0x40)                 /* SLA+R has been transmitted and ACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI);
    }
    else if (u32Status == 0x58)                 /* DATA has been received and NACK has been returned */
    {
        g_u8RxData = I2C_GET_DATA(I2C1);
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_STO | I2C_CTL_SI);
        g_u8EndFlag = 1;
    }
    else
    {
        /* TO DO */
        sysprintf("Status 0x%x is NOT processed\n", u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Tx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterTx(uint32_t u32Status)
{
    if (u32Status == 0x08)                      /* START has been transmitted */
    {
        I2C_SET_DATA(I2C1, g_u8DeviceAddr << 1);  /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI);
    }
    else if (u32Status == 0x18)                 /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI);
    }
    else if (u32Status == 0x20)                 /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_STA | I2C_CTL_STO | I2C_CTL_SI);
    }
    else if (u32Status == 0x28)                 /* DATA has been transmitted and ACK has been received */
    {
        if (g_u8DataLen != 3)
        {
            I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
            I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI);
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C1, I2C_CTL_STO | I2C_CTL_SI);
            g_u8EndFlag = 1;
        }
    }
    else
    {
        /* TO DO */
        sysprintf("Status 0x%x is NOT processed\n", u32Status);
    }
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HXT */
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;

    /* Waiting clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Enable IP clock */
    CLK_EnableModuleClock(I2C1_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_HXT, CLK_CLKDIV1_UART0(2));
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(GPB_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);
    /* Set multi-function pins for I2C1 */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB11MFP_Msk);
    SYS->GPB_MFPH |= SYS_GPB_MFPH_PB10MFP_I2C1_SDA | SYS_GPB_MFPH_PB11MFP_I2C1_SCL;

    /* I2C pin enable schmitt trigger */
    PB->SMTEN |= GPIO_SMTEN_SMTEN10_Msk | GPIO_SMTEN_SMTEN11_Msk;

    /* Lock protected registers */
    SYS_LockReg();
}

void I2C1_Init(void)
{
    /* Open I2C1 and set clock to 100k */
    I2C_Open(I2C1, 100000);

    /* Get I2C1 Bus Clock */
    sysprintf("I2C clock %d Hz\n", I2C_GetBusClockFreq(I2C1));

    /* Set I2C1 4 Slave Addresses */
    I2C_SetSlaveAddr(I2C1, 0, 0x15, I2C_GCMODE_DISABLE);   /* Slave Address : 0x15 */
    I2C_SetSlaveAddr(I2C1, 1, 0x35, I2C_GCMODE_DISABLE);   /* Slave Address : 0x35 */
    I2C_SetSlaveAddr(I2C1, 2, 0x55, I2C_GCMODE_DISABLE);   /* Slave Address : 0x55 */
    I2C_SetSlaveAddr(I2C1, 3, 0x75, I2C_GCMODE_DISABLE);   /* Slave Address : 0x75 */

    I2C_EnableInt(I2C1);
    IRQ_SetHandler((IRQn_ID_t)I2C1_IRQn, I2C1_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)I2C1_IRQn);
}

int32_t Read_Write_SLAVE(uint8_t slvaddr)
{
    uint32_t i;

    g_u8DeviceAddr = slvaddr;

    for (i = 0; i < 0x100; i++)
    {
        g_au8TxData[0] = (uint8_t)((i & 0xFF00) >> 8);
        g_au8TxData[1] = (uint8_t)(i & 0x00FF);
        g_au8TxData[2] = (uint8_t)(g_au8TxData[1] + 3);

        g_u8DataLen = 0;
        g_u8EndFlag = 0;

        /* I2C function to write data to slave */
        s_I2C1HandlerFn = (I2C_FUNC)I2C_MasterTx;

        /* I2C as master sends START signal */
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_STA);

        /* Wait I2C Tx Finish */
        while (g_u8EndFlag == 0);
        g_u8EndFlag = 0;

        /* I2C function to read data from slave */
        s_I2C1HandlerFn = (I2C_FUNC)I2C_MasterRx;

        g_u8DataLen = 0;
        g_u8DeviceAddr = slvaddr;

        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_STA);

        /* Wait I2C Rx Finish */
        while (g_u8EndFlag == 0);

        /* Compare data */
        if (g_u8RxData != g_au8TxData[2])
        {
            sysprintf("I2C Byte Write/Read Failed, Data 0x%x\n", g_u8RxData);
            return -1;
        }
    }
    sysprintf("Master Access Slave (0x%X) Test OK\n", slvaddr);
    return 0;
}

int32_t main (void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    /*
        This sample code sets I2C bus clock to 100kHz. Then, Master accesses Slave with Byte Write
        and Byte Read operations, and check if the read data is equal to the programmed data.
    */

    sysprintf("+-------------------------------------------------------+\n");
    sysprintf("|       I2C Driver Sample Code(Master) for access Slave |\n");
    sysprintf("+-------------------------------------------------------+\n");

    /* Init I2C1 */
    I2C1_Init();

    /* Access Slave with no address mask */
    sysprintf("\n");
    sysprintf(" == No Mask Address ==\n");
    Read_Write_SLAVE(0x15);
    Read_Write_SLAVE(0x35);
    Read_Write_SLAVE(0x55);
    Read_Write_SLAVE(0x75);
    sysprintf("SLAVE Address test OK.\n");

    /* Access Slave with address mask */
    sysprintf("\n");
    sysprintf(" == Mask Address ==\n");
    Read_Write_SLAVE(0x15 & ~0x01);
    Read_Write_SLAVE(0x35 & ~0x04);
    Read_Write_SLAVE(0x55 & ~0x01);
    Read_Write_SLAVE(0x75 & ~0x04);
    sysprintf("SLAVE Address Mask test OK.\n");

    while(1);
}

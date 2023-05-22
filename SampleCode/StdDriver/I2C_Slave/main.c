/**************************************************************************//**
 * @file     main.c
 *
 * @brief    I2C Driver Sample Code
 *           This is a I2C slave mode demo and need to be tested with a master device.
 *
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"

uint32_t slave_buff_addr;
uint8_t g_u8SlvData[256];
uint8_t g_au8RxData[3];
/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_u8DeviceAddr;
uint8_t g_au8TxData[3];
uint8_t g_u8RxData;
uint8_t g_u8DataLen;

typedef void (*I2C_FUNC)(uint32_t u32Status);

static I2C_FUNC s_I2C1HandlerFn = NULL;


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
/*  I2C TRx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_SlaveTRx(uint32_t u32Status)
{
    if (u32Status == 0x60)                      /* Own SLA+W has been receive; ACK has been return */
    {
        g_u8DataLen = 0;
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI | I2C_CTL_AA);
    }
    else if (u32Status == 0x80)                 /* Previously address with own SLA address
                                                   Data has been received; ACK has been returned*/
    {
        g_au8RxData[g_u8DataLen] = I2C_GET_DATA(I2C1);;
        g_u8DataLen++;

        if (g_u8DataLen == 2)
        {
            slave_buff_addr = (g_au8RxData[0] << 8) + g_au8RxData[1];
        }
        if (g_u8DataLen == 3)
        {
            g_u8SlvData[slave_buff_addr] = g_au8RxData[2];
            g_u8DataLen = 0;
        }
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI | I2C_CTL_AA);
    }
    else if(u32Status == 0xA8)                  /* Own SLA+R has been receive; ACK has been return */
    {

        I2C_SET_DATA(I2C1, g_u8SlvData[slave_buff_addr]);
        slave_buff_addr++;
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI | I2C_CTL_AA);
    }
    else if (u32Status == 0xC0)                 /* Data byte or last data in I2CDAT has been transmitted
                                                   Not ACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI | I2C_CTL_AA);
    }
    else if (u32Status == 0x88)                 /* Previously addressed with own SLA address; NOT ACK has
                                                   been returned */
    {
        g_u8DataLen = 0;
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI | I2C_CTL_AA);
    }
    else if (u32Status == 0xA0)                 /* A STOP or repeated START has been received while still
                                                   addressed as Slave/Receiver*/
    {
        g_u8DataLen = 0;
        I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI | I2C_CTL_AA);
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

    I2C_SetSlaveAddrMask(I2C1, 0, 0x01);
    I2C_SetSlaveAddrMask(I2C1, 1, 0x04);
    I2C_SetSlaveAddrMask(I2C1, 2, 0x01);
    I2C_SetSlaveAddrMask(I2C1, 3, 0x04);

    I2C_EnableInt(I2C1);
    IRQ_SetHandler((IRQn_ID_t)I2C1_IRQn, I2C1_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)I2C1_IRQn);
}


int32_t main (void)
{
    uint32_t i;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    /*
        This sample code sets I2C bus clock to 100kHz. Then, Master accesses Slave with Byte Write
        and Byte Read operations, and check if the read data is equal to the programmed data.
    */

    sysprintf("+-------------------------------------------------------+\n");
    sysprintf("|               I2C Driver Sample Code(Slave)           |\n");
    sysprintf("+-------------------------------------------------------+\n");

    /* Init I2C1 */
    I2C1_Init();

    /* I2C enter no address SLV mode */
    I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI | I2C_CTL_AA);

    for (i = 0; i < 0x100; i++)
    {
        g_u8SlvData[i] = 0;
    }

    /* I2C function to Slave receive/transmit data */
    s_I2C1HandlerFn=I2C_SlaveTRx;

    sysprintf("\n");
    sysprintf("I2C Slave Mode is Running.\n");

    while(1);
}

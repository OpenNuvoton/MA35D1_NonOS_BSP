/**************************************************************************//**
 * @file     main.c
 *
 * @brief    This is an I2S demo using NAU88C22/88L25 audio codec, and used to play
 *           back the input from line-in or MIC interface.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "config.h"

#define NAU8822     1

uint32_t PcmBuff[BUFF_LEN] = {0};
uint32_t volatile u32BuffPos = 0;

#if NAU8822

/*---------------------------------------------------------------------------------------------------------*/
/*  Write 9-bit data to 7-bit address register of NAU8822 with I2C2                                        */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_WriteNAU8822(uint8_t u8addr, uint16_t u16data)
{

    I2C_START(I2C2);
    I2C_WAIT_READY(I2C2);

    I2C_SET_DATA(I2C2, 0x1A<<1);
    I2C_SET_CONTROL_REG(I2C2, I2C_CTL_SI);
    I2C_WAIT_READY(I2C2);

    I2C_SET_DATA(I2C2, (uint8_t)((u8addr << 1) | (u16data >> 8)));
    I2C_SET_CONTROL_REG(I2C2, I2C_CTL_SI);
    I2C_WAIT_READY(I2C2);

    I2C_SET_DATA(I2C2, (uint8_t)(u16data & 0x00FF));
    I2C_SET_CONTROL_REG(I2C2, I2C_CTL_SI);
    I2C_WAIT_READY(I2C2);

    I2C_STOP(I2C2);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  NAU8822 Settings with I2C interface                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void NAU8822_Setup()
{
    sysprintf("\nConfigure NAU88C22 ...");

    I2C_WriteNAU8822(0,  0x000);   /* Reset all registers */
    //CLK_SysTickDelay(10000);

    I2C_WriteNAU8822(1,  0x03F);
    I2C_WriteNAU8822(2,  0x1BF);   /* Enable L/R Headphone, ADC Mix/Boost, ADC */
    I2C_WriteNAU8822(3,  0x07F);   /* Enable L/R main mixer, DAC */
    I2C_WriteNAU8822(4,  0x010);   /* 16-bit word length, I2S format, Stereo */
    I2C_WriteNAU8822(5,  0x000);   /* Companding control and loop back mode (all disable) */
    I2C_WriteNAU8822(6,  0x1AD);   /* Divide by 6, 16K */
    I2C_WriteNAU8822(7,  0x006);   /* 16K for internal filter coefficients */
    I2C_WriteNAU8822(10, 0x008);   /* DAC soft mute is disabled, DAC oversampling rate is 128x */
    I2C_WriteNAU8822(14, 0x108);   /* ADC HP filter is disabled, ADC oversampling rate is 128x */
    I2C_WriteNAU8822(15, 0x1EF);   /* ADC left digital volume control */
    I2C_WriteNAU8822(16, 0x1EF);   /* ADC right digital volume control */
    I2C_WriteNAU8822(44, 0x033);   /* LMICN/LMICP is connected to PGA */
    I2C_WriteNAU8822(50, 0x001);   /* Left DAC connected to LMIX */
    I2C_WriteNAU8822(51, 0x001);   /* Right DAC connected to RMIX */

    sysprintf("[OK]\n");
}

#else   // NAU88L25

uint8_t I2cWrite_MultiByteforNAU88L25(uint8_t chipadd,uint16_t subaddr, const uint8_t *p,uint32_t len)
{
    /* Send START */
    I2C_START(I2C2);
    I2C_WAIT_READY(I2C2);

    /* Send device address */
    I2C_SET_DATA(I2C2, chipadd);
    I2C_SET_CONTROL_REG(I2C2, I2C_CTL_SI);
    I2C_WAIT_READY(I2C2);

    /* Send register number and MSB of data */
    I2C_SET_DATA(I2C2, (uint8_t)(subaddr>>8));
    I2C_SET_CONTROL_REG(I2C2, I2C_CTL_SI);
    I2C_WAIT_READY(I2C2);

    /* Send register number and MSB of data */
    I2C_SET_DATA(I2C2, (uint8_t)(subaddr));
    I2C_SET_CONTROL_REG(I2C2, I2C_CTL_SI);
    I2C_WAIT_READY(I2C2);

    /* Send data */
    I2C_SET_DATA(I2C2, p[0]);
    I2C_SET_CONTROL_REG(I2C2, I2C_CTL_SI);
    I2C_WAIT_READY(I2C2);

    /* Send data */
    I2C_SET_DATA(I2C2, p[1]);
    I2C_SET_CONTROL_REG(I2C2, I2C_CTL_SI);
    I2C_WAIT_READY(I2C2);

    /* Send STOP */
    I2C_STOP(I2C2);

    return  0;
}

uint8_t I2C_WriteNAU88L25(uint16_t addr,uint16_t dat)
{
    uint8_t Tx_Data0[2];

    Tx_Data0[0] = (uint8_t)(dat >> 8);
    Tx_Data0[1] = (uint8_t)(dat & 0x00FF);

    return ( I2cWrite_MultiByteforNAU88L25(0x1A << 1,addr,&Tx_Data0[0],2) );
}

void NAU88L25_Reset(void)
{
    I2C_WriteNAU88L25(0,  0x1);
    I2C_WriteNAU88L25(0,  0);   // Reset all registers
    CLK_SysTickDelay(10000);

    sysprintf("NAU88L25 Software Reset.\n");
}

void NAU88L25_Setup(void)
{
    I2C_WriteNAU88L25(0x0003,  0x8053);
    I2C_WriteNAU88L25(0x0004,  0x0001);
    I2C_WriteNAU88L25(0x0005,  0x3126);
    I2C_WriteNAU88L25(0x0006,  0x0008);
    I2C_WriteNAU88L25(0x0007,  0x0010);
    I2C_WriteNAU88L25(0x0008,  0xC000);
    I2C_WriteNAU88L25(0x0009,  0x6000);
    I2C_WriteNAU88L25(0x000A,  0xF13C);
    I2C_WriteNAU88L25(0x000C,  0x0048);
    I2C_WriteNAU88L25(0x000D,  0x0000);
    I2C_WriteNAU88L25(0x000F,  0x0000);
    I2C_WriteNAU88L25(0x0010,  0x0000);
    I2C_WriteNAU88L25(0x0011,  0x0000);
    I2C_WriteNAU88L25(0x0012,  0xFFFF);
    I2C_WriteNAU88L25(0x0013,  0x0015);
    I2C_WriteNAU88L25(0x0014,  0x0110);
    I2C_WriteNAU88L25(0x0015,  0x0000);
    I2C_WriteNAU88L25(0x0016,  0x0000);
    I2C_WriteNAU88L25(0x0017,  0x0000);
    I2C_WriteNAU88L25(0x0018,  0x0000);
    I2C_WriteNAU88L25(0x0019,  0x0000);
    I2C_WriteNAU88L25(0x001A,  0x0000);
    I2C_WriteNAU88L25(0x001B,  0x0000);
    I2C_WriteNAU88L25(0x001C,  0x0002);
    I2C_WriteNAU88L25(0x001D,  0x301a);   //301A:Master, BCLK_DIV=12.288M/8=1.536M, LRC_DIV=1.536M/32=48K
    I2C_WriteNAU88L25(0x001E,  0x0000);
    I2C_WriteNAU88L25(0x001F,  0x0000);
    I2C_WriteNAU88L25(0x0020,  0x0000);
    I2C_WriteNAU88L25(0x0021,  0x0000);
    I2C_WriteNAU88L25(0x0022,  0x0000);
    I2C_WriteNAU88L25(0x0023,  0x0000);
    I2C_WriteNAU88L25(0x0024,  0x0000);
    I2C_WriteNAU88L25(0x0025,  0x0000);
    I2C_WriteNAU88L25(0x0026,  0x0000);
    I2C_WriteNAU88L25(0x0027,  0x0000);
    I2C_WriteNAU88L25(0x0028,  0x0000);
    I2C_WriteNAU88L25(0x0029,  0x0000);
    I2C_WriteNAU88L25(0x002A,  0x0000);
    I2C_WriteNAU88L25(0x002B,  0x0012);
    I2C_WriteNAU88L25(0x002C,  0x0082);
    I2C_WriteNAU88L25(0x002D,  0x0000);
    I2C_WriteNAU88L25(0x0030,  0x00CF);
    I2C_WriteNAU88L25(0x0031,  0x0000);
    I2C_WriteNAU88L25(0x0032,  0x0000);
    I2C_WriteNAU88L25(0x0033,  0x009E);
    I2C_WriteNAU88L25(0x0034,  0x029E);
    I2C_WriteNAU88L25(0x0038,  0x1486);
    I2C_WriteNAU88L25(0x0039,  0x0F12);
    I2C_WriteNAU88L25(0x003A,  0x25FF);
    I2C_WriteNAU88L25(0x003B,  0x3457);
    I2C_WriteNAU88L25(0x0045,  0x1486);
    I2C_WriteNAU88L25(0x0046,  0x0F12);
    I2C_WriteNAU88L25(0x0047,  0x25F9);
    I2C_WriteNAU88L25(0x0048,  0x3457);
    I2C_WriteNAU88L25(0x004C,  0x0000);
    I2C_WriteNAU88L25(0x004D,  0x0000);
    I2C_WriteNAU88L25(0x004E,  0x0000);
    I2C_WriteNAU88L25(0x0050,  0x2007);
    I2C_WriteNAU88L25(0x0051,  0x0000);
    I2C_WriteNAU88L25(0x0053,  0xC201);
    I2C_WriteNAU88L25(0x0054,  0x0C95);
    I2C_WriteNAU88L25(0x0055,  0x0000);
    I2C_WriteNAU88L25(0x0058,  0x1A14);
    I2C_WriteNAU88L25(0x0059,  0x00FF);
    I2C_WriteNAU88L25(0x0066,  0x0060);
    I2C_WriteNAU88L25(0x0068,  0xC300);
    I2C_WriteNAU88L25(0x0069,  0x0000);
    I2C_WriteNAU88L25(0x006A,  0x0083);
    I2C_WriteNAU88L25(0x0071,  0x0011);
    I2C_WriteNAU88L25(0x0072,  0x0260);
    I2C_WriteNAU88L25(0x0073,  0x332C);
    I2C_WriteNAU88L25(0x0074,  0x4502);
    I2C_WriteNAU88L25(0x0076,  0x3140);
    I2C_WriteNAU88L25(0x0077,  0x0000);
    I2C_WriteNAU88L25(0x007F,  0x553F);
    I2C_WriteNAU88L25(0x0080,  0x0420);
    I2C_WriteNAU88L25(0x0001,  0x07D4);

    sysprintf("NAU88L25 Configured done.\n");
}
#endif

void SYS_Init(void)
{
    /* Set APLL to 250 MHz */
    CLK_SetPLLClockFreq(APLL, PLL_OPMODE_INTEGER, FREQ_PLLSRC, 250000000);

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

    /* Enable I2S0 module clock */
    CLK_EnableModuleClock(I2S0_MODULE);

    /* Select APLL as the clock source of I2S0 */
    CLK_SetModuleClock(I2S0_MODULE, CLK_CLKSEL4_I2S0SEL_APLL, MODULE_NoMsk);

    /* Enable I2C2 module clock */
    CLK_EnableModuleClock(I2C2_MODULE);

    /* Enable GPIOD module clock */
    CLK_EnableModuleClock(GPD_MODULE);

    /* Set GPE multi-function pins for UART0 RXD and TXD */
    SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
    SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);

    /* PB8: I2C2_SDA; PB9: I2C2_SCL */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~(SYS_GPB_MFPH_PB8MFP_Msk|SYS_GPB_MFPH_PB9MFP_Msk))) | SYS_GPB_MFPH_PB8MFP_I2C2_SDA | SYS_GPB_MFPH_PB9MFP_I2C2_SCL;

    /* PK.12(I2S0_LRCK),PK.13(I2S0_BCLK),PK.14(I2S0_DI),PK.15(I2S0_DO), PN.15(I2S0_MCLK) */
    SYS->GPK_MFPH &= ~(SYS_GPK_MFPH_PK12MFP_Msk | SYS_GPK_MFPH_PK13MFP_Msk | SYS_GPK_MFPH_PK14MFP_Msk | SYS_GPK_MFPH_PK15MFP_Msk);
    SYS->GPK_MFPH |= (SYS_GPK_MFPH_PK12MFP_I2S0_LRCK | SYS_GPK_MFPH_PK13MFP_I2S0_BCLK | SYS_GPK_MFPH_PK14MFP_I2S0_DI | SYS_GPK_MFPH_PK15MFP_I2S0_DO);
    /* PN.15(I2S0_MCLK) */
    SYS->GPN_MFPH = (SYS->GPN_MFPH & (~SYS_GPN_MFPH_PN15MFP_Msk)) | SYS_GPN_MFPH_PN15MFP_I2S0_MCLK;
}

void I2C2_Init(void)
{
    /* Open I2C2 and set clock to 100k */
    I2C_Open(I2C2, 100000);

    /* Get I2C2 Bus Clock */
    sysprintf("I2C clock %d Hz\n", I2C_GetBusClockFreq(I2C2));

}

extern void I2S0_IRQHandler(void);
int32_t main (void)
{
    uint32_t u32startFlag = 1;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    sysprintf("+------------------------------------------------------------------------+\n");
    sysprintf("|                   I2S Driver Sample Code with NAU88C22                 |\n");
    sysprintf("+------------------------------------------------------------------------+\n");
    sysprintf("  NOTE: This sample code needs to work with NAU88C22.\n");

    /* Init I2C2 to access Codec */
    I2C2_Init();

    // Plug-In DET
    //SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA4MFP_Msk));
    //GPIO_SetMode(PA, BIT4, GPIO_MODE_OUTPUT);
    //PA4 = 1;

#if (!NAU8822)
    /* Reset NAU88L25 codec */
    NAU88L25_Reset();
#endif

    /* Open I2S0 interface and set to slave mode, stereo channel, I2S format */
    I2S_Open(I2S0, I2S_MODE_SLAVE, 16000, I2S_DATABIT_16, I2S_DISABLE_MONO, I2S_FORMAT_I2S);
    IRQ_SetHandler((IRQn_ID_t)I2S0_IRQn, I2S0_IRQHandler);
    IRQ_Enable ((IRQn_ID_t)I2S0_IRQn);

    /* Set PD.13 low to enable phone jack on DEV board. */
    /* PD.13(AUDIO_JKEN : keep output low) */
    SYS->GPD_MFPH = (SYS->GPD_MFPH & ~(SYS_GPD_MFPH_PD13MFP_Msk));
    GPIO_SetMode(PD, BIT13, GPIO_MODE_OUTPUT);
    PD13 = 0;

    // select source from APLL(250MHz)
    CLK_SetModuleClock(I2S0_MODULE, CLK_CLKSEL4_I2S0SEL_APLL, 0);

    /* Set MCLK and enable MCLK */
    I2S_EnableMCLK(I2S0, 12000000);

#if NAU8822
    /* Initialize NAU8822 codec */
    NAU8822_Setup();
#else
    I2S0->CTL0 |= I2S_CTL0_ORDER_Msk;
    /* Initialize NAU88L25 codec */
    CLK_SysTickDelay(20000);
    NAU88L25_Setup();
#endif

    /* Enable Rx threshold level interrupt */
    I2S_EnableInt(I2S0, I2S_IEN_RXTHIEN_Msk);

    /* Enable I2S Rx function to receive data */
    I2S_ENABLE_RX(I2S0);

    while(1)
    {
        if (u32startFlag)
        {
            /* Enable I2S Tx function to send data when data in the buffer is more than half of buffer size */
            if (u32BuffPos >= BUFF_LEN/2)
            {
                I2S_EnableInt(I2S0, I2S_IEN_TXTHIEN_Msk);
                I2S_ENABLE_TX(I2S0);
                u32startFlag = 0;
            }
        }
    }
}



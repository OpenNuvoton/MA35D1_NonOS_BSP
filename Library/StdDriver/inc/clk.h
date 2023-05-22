/**************************************************************************//**
 * @file     CLK.h
 * @brief    CLK Driver Header File
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright(C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __CLK_H__
#define __CLK_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup CLK_Driver CLK Driver
  @{
*/

/** @addtogroup CLK_EXPORTED_CONSTANTS CLK Exported Constants
  @{
*/

#define FREQ_180MHZ                 180000000UL  /*!< 180 MHz \hideinitializer */


/* Advanced PLL Specification limits */
#define FREQ_1MHZ                   1000000UL    /*!< 1 MHz \hideinitializer */
#define FREQ_10MHZ                  10000000UL   /*!< 10 MHz \hideinitializer */
#define FREQ_40MHZ                  40000000UL   /*!< 40 MHz \hideinitializer */
#define FREQ_200MHZ                 200000000UL  /*!< 200 MHz \hideinitializer */
#define FREQ_600MHZ                 600000000UL  /*!< 600 MHz \hideinitializer */
#define FREQ_1000MHZ                1000000000UL /*!< 1000 MHz \hideinitializer */
#define FREQ_2400MHZ                2400000000UL /*!< 2400 MHz \hideinitializer */
#define FREQ_85700KHZ               85700000UL   /*!< 85.7 MHz \hideinitializer */

#define FREQ_PLLSRC                 24000000UL   /*!< Fref Input Frequency is 24 MHz \hideinitializer */

#define ADVPLL_SPREAD_RANGE         194          /*!< Spread Range 1.94% \hideinitializer */
#define ADVPLL_MODULATION_FREQ      50000        /*!< Modulation Frequency 50kHz \hideinitializer */

/*! PLL Clock Index */
typedef enum
{
	CAPLL = 0,       /*!< PLL0 is a CA-PLL. */
	SYSPLL = 1,      /*!< PLL1 is a SYS-PLL. */
	DDRPLL = 2,      /*!< PLL2 is a DDR-PLL. */
	APLL = 3,        /*!< PLL3 is a APLL. */
	EPLL = 4,        /*!< PLL4 is a EPLL. */
	VPLL = 5         /*!< PLL5 is a VPLL. */
} E_PLL_INDEX;

/*! Advanced PLL Operation Mode */
typedef enum
{
	PLL_OPMODE_INTEGER = 0,        /*!< Integer Operation Mode. */
	PLL_OPMODE_FRACTIONAL = 1,     /*!< Fractional Operation Mode. */
	PLL_OPMODE_SPREAD_SPECTRUM= 2  /*!< Spread Spectrum Operation Mode. */
} E_ADVPLL_OPMODE;

/*---------------------------------------------------------------------------------------------------------*/
/*  PWRCTL clock source constant definitions.                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_PWRCTL_ENABLE_HXT          (CLK_PWRCTL_HXTEN_Msk)                       /*!< HXT Clock Enable \hideinitializer */
#define CLK_PWRCTL_ENABLE_LXT          (CLK_PWRCTL_LXTEN_Msk)                       /*!< LXT Clock Enable \hideinitializer */
#define CLK_PWRCTL_ENABLE_HIRC         (CLK_PWRCTL_HIRCSTB_Msk)                     /*!< HIRC Clock Enable \hideinitializer */
#define CLK_PWRCTL_ENABLE_LIRC         (CLK_PWRCTL_LIRCSTB_Msk)                     /*!< LIRC Clock Enable \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  STATUS constant definitions.                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_STATUS_STABLE_HXT          (CLK_STATUS_HXTSTB_Msk)                      /*!< HXT Clock Source Stable Flag \hideinitializer */
#define CLK_STATUS_STABLE_LXT          (CLK_STATUS_LXTSTB_Msk)                      /*!< LXT Clock Source Stable Flag \hideinitializer */
#define CLK_STATUS_STABLE_SYSPLL       (CLK_STATUS_SYSPLLSTB_Msk)                   /*!< SYSPLL Clock Source Stable Flag \hideinitializer */
#define CLK_STATUS_STABLE_LIRC         (CLK_STATUS_LIRCSTB_Msk)                     /*!< LIRC Clock Source Stable Flag \hideinitializer */
#define CLK_STATUS_STABLE_HIRC         (CLK_STATUS_HIRCSTB_Msk)                     /*!< HIRC Clock Source Stable Flag \hideinitializer */
#define CLK_STATUS_STABLE_CAPLL        (CLK_STATUS_CAPLLSTB_Msk)                    /*!< CAPLL Clock Source Stable Flag \hideinitializer */
#define CLK_STATUS_STABLE_DDRPLL       (CLK_STATUS_DDRPLLSTB_Msk)                   /*!< DDRPLL Clock Source Stable Flag \hideinitializer */
#define CLK_STATUS_STABLE_EPLL         (CLK_STATUS_EPLLSTB_Msk)                     /*!< EPLL Clock Source Stable Flag \hideinitializer */
#define CLK_STATUS_STABLE_APLL         (CLK_STATUS_APLLSTB_Msk)                     /*!< APLL Clock Source Stable Flag \hideinitializer */
#define CLK_STATUS_STABLE_VPLL         (CLK_STATUS_VPLLSTB_Msk)                     /*!< VPLL Clock Source Stable Flag \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  CLKSEL0 constant definitions.  (Write-protection)                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKSEL0_CA35CKSEL_HXT          (0x0UL<<CLK_CLKSEL0_CA35CKSEL_Pos)       /*!< Select CA35CK clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL0_CA35CKSEL_CAPLL        (0x1UL<<CLK_CLKSEL0_CA35CKSEL_Pos)       /*!< Select CA35CK clock source from CA-PLL \hideinitializer */
#define CLK_CLKSEL0_CA35CKSEL_DDRPLL       (0x2UL<<CLK_CLKSEL0_CA35CKSEL_Pos)       /*!< Select CA35CK clock source from DDR-PLL \hideinitializer */
#define CLK_CLKSEL0_SYSCK0SEL_EPLL_DIV2    (0x0UL<<CLK_CLKSEL0_SYSCK0SEL_Pos)       /*!< Select SYSCK0 clock source from EPLL/2 \hideinitializer */
#define CLK_CLKSEL0_SYSCK0SEL_SYSPLL       (0x1UL<<CLK_CLKSEL0_SYSCK0SEL_Pos)       /*!< Select SYSCK0 clock source from SYS-PLL \hideinitializer */
#define CLK_CLKSEL0_LVRDBSEL_LIRC          (0x0UL<<CLK_CLKSEL0_LVRDBSEL_Pos)        /*!< Select LVRDB clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL0_LVRDBSEL_HIRC          (0x1UL<<CLK_CLKSEL0_LVRDBSEL_Pos)        /*!< Select LVRDB clock source from high speed oscillator \hideinitializer */
#define CLK_CLKSEL0_SYSCK1SEL_HXT          (0x0UL<<CLK_CLKSEL0_SYSCK1SEL_Pos)       /*!< Select SYSCK1 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL0_SYSCK1SEL_SYSPLL       (0x1UL<<CLK_CLKSEL0_SYSCK1SEL_Pos)       /*!< Select SYSCK1 clock source from SYS-PLL \hideinitializer */
#define CLK_CLKSEL0_RTPSTSEL_HXT           (0x0UL<<CLK_CLKSEL0_RTPSTSEL_Pos)        /*!< Select RTPST clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL0_RTPSTSEL_LXT           (0x1UL<<CLK_CLKSEL0_RTPSTSEL_Pos)        /*!< Select RTPST clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL0_RTPSTSEL_HXT_DIV2      (0x2UL<<CLK_CLKSEL0_RTPSTSEL_Pos)        /*!< Select RTPST clock source from high speed crystal/2 \hideinitializer */
#define CLK_CLKSEL0_RTPSTSEL_SYSCLK1_DIV2  (0x3UL<<CLK_CLKSEL0_RTPSTSEL_Pos)        /*!< Select RTPST clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL0_RTPSTSEL_HIRC          (0x4UL<<CLK_CLKSEL0_RTPSTSEL_Pos)        /*!< Select RTPST clock source from high speed oscillator \hideinitializer */
#define CLK_CLKSEL0_CCAP0SEL_HXT           (0x0UL<<CLK_CLKSEL0_CCAP0SEL_Pos)        /*!< Select CCAP0 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL0_CCAP0SEL_VPLL          (0x1UL<<CLK_CLKSEL0_CCAP0SEL_Pos)        /*!< Select CCAP0 clock source from VPLL \hideinitializer */
#define CLK_CLKSEL0_CCAP0SEL_APLL          (0x2UL<<CLK_CLKSEL0_CCAP0SEL_Pos)        /*!< Select CCAP0 clock source from APLL \hideinitializer */
#define CLK_CLKSEL0_CCAP0SEL_SYSPLL        (0x3UL<<CLK_CLKSEL0_CCAP0SEL_Pos)        /*!< Select CCAP0 clock source from SYS-PLL \hideinitializer */
#define CLK_CLKSEL0_CCAP1SEL_HXT           (0x0UL<<CLK_CLKSEL0_CCAP1SEL_Pos)        /*!< Select CCAP1 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL0_CCAP1SEL_VPLL          (0x1UL<<CLK_CLKSEL0_CCAP1SEL_Pos)        /*!< Select CCAP1 clock source from VPLL \hideinitializer */
#define CLK_CLKSEL0_CCAP1SEL_APLL          (0x2UL<<CLK_CLKSEL0_CCAP1SEL_Pos)        /*!< Select CCAP1 clock source from APLL \hideinitializer */
#define CLK_CLKSEL0_CCAP1SEL_SYSPLL        (0x3UL<<CLK_CLKSEL0_CCAP1SEL_Pos)        /*!< Select CCAP1 clock source from SYS-PLL \hideinitializer */
#define CLK_CLKSEL0_SD0SEL_SYSPLL          (0x0UL<<CLK_CLKSEL0_SD0SEL_Pos)          /*!< Select SD0 clock source from SYS-PLL \hideinitializer */
#define CLK_CLKSEL0_SD0SEL_APLL            (0x1UL<<CLK_CLKSEL0_SD0SEL_Pos)          /*!< Select SD0 clock source from APLL \hideinitializer */
#define CLK_CLKSEL0_SD1SEL_SYSPLL          (0x0UL<<CLK_CLKSEL0_SD1SEL_Pos)          /*!< Select SD1 clock source from SYS-PLL \hideinitializer */
#define CLK_CLKSEL0_SD1SEL_APLL            (0x1UL<<CLK_CLKSEL0_SD1SEL_Pos)          /*!< Select SD1 clock source from APLL \hideinitializer */
#define CLK_CLKSEL0_DCUSEL_EPLL_DIV2       (0x0UL<<CLK_CLKSEL0_DCUSEL_Pos)          /*!< Select DCU clock source from EPLL/2 \hideinitializer */
#define CLK_CLKSEL0_DCUSEL_SYSPLL          (0x1UL<<CLK_CLKSEL0_DCUSEL_Pos)          /*!< Select DCU clock source from SYS-PLL \hideinitializer */
#define CLK_CLKSEL0_GFXSEL_EPLL            (0x0UL<<CLK_CLKSEL0_GFXSEL_Pos)          /*!< Select GFX clock source from EPLL \hideinitializer */
#define CLK_CLKSEL0_GFXSEL_SYSPLL          (0x1UL<<CLK_CLKSEL0_GFXSEL_Pos)          /*!< Select GFX clock source from SYS-PLL \hideinitializer */
#define CLK_CLKSEL0_DBGSEL_HIRC            (0x0UL<<CLK_CLKSEL0_DBGSEL_Pos)          /*!< Select DBG clock source from high speed oscillator \hideinitializer */
#define CLK_CLKSEL0_DBGSEL_SYSPLL          (0x1UL<<CLK_CLKSEL0_DBGSEL_Pos)          /*!< Select DBG clock source from SYS-PLL \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  CLKSEL1 constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKSEL1_TMR0SEL_HXT            (0x0UL<<CLK_CLKSEL1_TMR0SEL_Pos)         /*!< Select TMR0 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR0SEL_LXT            (0x1UL<<CLK_CLKSEL1_TMR0SEL_Pos)         /*!< Select TMR0 clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR0SEL_PCLK0          (0x2UL<<CLK_CLKSEL1_TMR0SEL_Pos)         /*!< Select TMR0 clock source from PCLK0 \hideinitializer */
#define CLK_CLKSEL1_TMR0SEL_EXT            (0x3UL<<CLK_CLKSEL1_TMR0SEL_Pos)         /*!< Select TMR0 clock source from external trigger \hideinitializer */
#define CLK_CLKSEL1_TMR0SEL_LIRC           (0x5UL<<CLK_CLKSEL1_TMR0SEL_Pos)         /*!< Select TMR0 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL1_TMR0SEL_HIRC           (0x7UL<<CLK_CLKSEL1_TMR0SEL_Pos)         /*!< Select TMR0 clock source from high speed oscillator \hideinitializer */
#define CLK_CLKSEL1_TMR1SEL_HXT            (0x0UL<<CLK_CLKSEL1_TMR1SEL_Pos)         /*!< Select TMR1 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR1SEL_LXT            (0x1UL<<CLK_CLKSEL1_TMR1SEL_Pos)         /*!< Select TMR1 clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR1SEL_PCLK0          (0x2UL<<CLK_CLKSEL1_TMR1SEL_Pos)         /*!< Select TMR1 clock source from PCLK0 \hideinitializer */
#define CLK_CLKSEL1_TMR1SEL_EXT            (0x3UL<<CLK_CLKSEL1_TMR1SEL_Pos)         /*!< Select TMR1 clock source from external trigger \hideinitializer */
#define CLK_CLKSEL1_TMR1SEL_LIRC           (0x5UL<<CLK_CLKSEL1_TMR1SEL_Pos)         /*!< Select TMR1 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL1_TMR1SEL_HIRC           (0x7UL<<CLK_CLKSEL1_TMR1SEL_Pos)         /*!< Select TMR1 clock source from high speed oscillator \hideinitializer */
#define CLK_CLKSEL1_TMR2SEL_HXT            (0x0UL<<CLK_CLKSEL1_TMR2SEL_Pos)         /*!< Select TMR2 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR2SEL_LXT            (0x1UL<<CLK_CLKSEL1_TMR2SEL_Pos)         /*!< Select TMR2 clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR2SEL_PCLK1          (0x2UL<<CLK_CLKSEL1_TMR2SEL_Pos)         /*!< Select TMR2 clock source from PCLK1 \hideinitializer */
#define CLK_CLKSEL1_TMR2SEL_EXT            (0x3UL<<CLK_CLKSEL1_TMR2SEL_Pos)         /*!< Select TMR2 clock source from external trigger \hideinitializer */
#define CLK_CLKSEL1_TMR2SEL_LIRC           (0x5UL<<CLK_CLKSEL1_TMR2SEL_Pos)         /*!< Select TMR2 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL1_TMR2SEL_HIRC           (0x7UL<<CLK_CLKSEL1_TMR2SEL_Pos)         /*!< Select TMR2 clock source from high speed oscillator \hideinitializer */
#define CLK_CLKSEL1_TMR3SEL_HXT            (0x0UL<<CLK_CLKSEL1_TMR3SEL_Pos)         /*!< Select TMR3 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR3SEL_LXT            (0x1UL<<CLK_CLKSEL1_TMR3SEL_Pos)         /*!< Select TMR3 clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR3SEL_PCLK1          (0x2UL<<CLK_CLKSEL1_TMR3SEL_Pos)         /*!< Select TMR3 clock source from PCLK1 \hideinitializer */
#define CLK_CLKSEL1_TMR3SEL_EXT            (0x3UL<<CLK_CLKSEL1_TMR3SEL_Pos)         /*!< Select TMR3 clock source from external trigger \hideinitializer */
#define CLK_CLKSEL1_TMR3SEL_LIRC           (0x5UL<<CLK_CLKSEL1_TMR3SEL_Pos)         /*!< Select TMR3 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL1_TMR3SEL_HIRC           (0x7UL<<CLK_CLKSEL1_TMR3SEL_Pos)         /*!< Select TMR3 clock source from high speed oscillator \hideinitializer */
#define CLK_CLKSEL1_TMR4SEL_HXT            (0x0UL<<CLK_CLKSEL1_TMR4SEL_Pos)         /*!< Select TMR4 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR4SEL_LXT            (0x1UL<<CLK_CLKSEL1_TMR4SEL_Pos)         /*!< Select TMR4 clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR4SEL_PCLK2          (0x2UL<<CLK_CLKSEL1_TMR4SEL_Pos)         /*!< Select TMR4 clock source from PCLK2 \hideinitializer */
#define CLK_CLKSEL1_TMR4SEL_EXT            (0x3UL<<CLK_CLKSEL1_TMR4SEL_Pos)         /*!< Select TMR4 clock source from external trigger \hideinitializer */
#define CLK_CLKSEL1_TMR4SEL_LIRC           (0x5UL<<CLK_CLKSEL1_TMR4SEL_Pos)         /*!< Select TMR4 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL1_TMR4SEL_HIRC           (0x7UL<<CLK_CLKSEL1_TMR4SEL_Pos)         /*!< Select TMR4 clock source from high speed oscillator \hideinitializer */
#define CLK_CLKSEL1_TMR5SEL_HXT            (0x0UL<<CLK_CLKSEL1_TMR5SEL_Pos)         /*!< Select TMR5 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR5SEL_LXT            (0x1UL<<CLK_CLKSEL1_TMR5SEL_Pos)         /*!< Select TMR5 clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR5SEL_PCLK2          (0x2UL<<CLK_CLKSEL1_TMR5SEL_Pos)         /*!< Select TMR5 clock source from PCLK2 \hideinitializer */
#define CLK_CLKSEL1_TMR5SEL_EXT            (0x3UL<<CLK_CLKSEL1_TMR5SEL_Pos)         /*!< Select TMR5 clock source from external trigger \hideinitializer */
#define CLK_CLKSEL1_TMR5SEL_LIRC           (0x5UL<<CLK_CLKSEL1_TMR5SEL_Pos)         /*!< Select TMR5 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL1_TMR5SEL_HIRC           (0x7UL<<CLK_CLKSEL1_TMR5SEL_Pos)         /*!< Select TMR5 clock source from high speed oscillator \hideinitializer */
#define CLK_CLKSEL1_TMR6SEL_HXT            (0x0UL<<CLK_CLKSEL1_TMR6SEL_Pos)         /*!< Select TMR6 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR6SEL_LXT            (0x1UL<<CLK_CLKSEL1_TMR6SEL_Pos)         /*!< Select TMR6 clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR6SEL_PCLK0          (0x2UL<<CLK_CLKSEL1_TMR6SEL_Pos)         /*!< Select TMR6 clock source from PCLK0 \hideinitializer */
#define CLK_CLKSEL1_TMR6SEL_EXT            (0x3UL<<CLK_CLKSEL1_TMR6SEL_Pos)         /*!< Select TMR6 clock source from external trigger \hideinitializer */
#define CLK_CLKSEL1_TMR6SEL_LIRC           (0x5UL<<CLK_CLKSEL1_TMR6SEL_Pos)         /*!< Select TMR6 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL1_TMR6SEL_HIRC           (0x7UL<<CLK_CLKSEL1_TMR6SEL_Pos)         /*!< Select TMR6 clock source from high speed oscillator \hideinitializer */
#define CLK_CLKSEL1_TMR7SEL_HXT            (0x0UL<<CLK_CLKSEL1_TMR7SEL_Pos)         /*!< Select TMR7 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR7SEL_LXT            (0x1UL<<CLK_CLKSEL1_TMR7SEL_Pos)         /*!< Select TMR7 clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL1_TMR7SEL_PCLK0          (0x2UL<<CLK_CLKSEL1_TMR7SEL_Pos)         /*!< Select TMR7 clock source from PCLK0 \hideinitializer */
#define CLK_CLKSEL1_TMR7SEL_EXT            (0x3UL<<CLK_CLKSEL1_TMR7SEL_Pos)         /*!< Select TMR7 clock source from external trigger \hideinitializer */
#define CLK_CLKSEL1_TMR7SEL_LIRC           (0x5UL<<CLK_CLKSEL1_TMR7SEL_Pos)         /*!< Select TMR7 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL1_TMR7SEL_HIRC           (0x7UL<<CLK_CLKSEL1_TMR7SEL_Pos)         /*!< Select TMR7 clock source from high speed oscillator \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  CLKSEL2 constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKSEL2_TMR8SEL_HXT            (0x0UL<<CLK_CLKSEL2_TMR8SEL_Pos)         /*!< Select TMR8 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL2_TMR8SEL_LXT            (0x1UL<<CLK_CLKSEL2_TMR8SEL_Pos)         /*!< Select TMR8 clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL2_TMR8SEL_PCLK1          (0x2UL<<CLK_CLKSEL2_TMR8SEL_Pos)         /*!< Select TMR8 clock source from PCLK1 \hideinitializer */
#define CLK_CLKSEL2_TMR8SEL_EXT            (0x3UL<<CLK_CLKSEL2_TMR8SEL_Pos)         /*!< Select TMR8 clock source from external trigger \hideinitializer */
#define CLK_CLKSEL2_TMR8SEL_LIRC           (0x5UL<<CLK_CLKSEL2_TMR8SEL_Pos)         /*!< Select TMR8 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL2_TMR8SEL_HIRC           (0x7UL<<CLK_CLKSEL2_TMR8SEL_Pos)         /*!< Select TMR8 clock source from high speed oscillator \hideinitializer */
#define CLK_CLKSEL2_TMR9SEL_HXT            (0x0UL<<CLK_CLKSEL2_TMR9SEL_Pos)         /*!< Select TMR9 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL2_TMR9SEL_LXT            (0x1UL<<CLK_CLKSEL2_TMR9SEL_Pos)         /*!< Select TMR9 clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL2_TMR9SEL_PCLK1          (0x2UL<<CLK_CLKSEL2_TMR9SEL_Pos)         /*!< Select TMR9 clock source from PCLK1 \hideinitializer */
#define CLK_CLKSEL2_TMR9SEL_EXT            (0x3UL<<CLK_CLKSEL2_TMR9SEL_Pos)         /*!< Select TMR9 clock source from external trigger \hideinitializer */
#define CLK_CLKSEL2_TMR9SEL_LIRC           (0x5UL<<CLK_CLKSEL2_TMR9SEL_Pos)         /*!< Select TMR9 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL2_TMR9SEL_HIRC           (0x7UL<<CLK_CLKSEL2_TMR9SEL_Pos)         /*!< Select TMR9 clock source from high speed oscillator \hideinitializer */
#define CLK_CLKSEL2_TMR10SEL_HXT           (0x0UL<<CLK_CLKSEL2_TMR10SEL_Pos)        /*!< Select TMR10 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL2_TMR10SEL_LXT           (0x1UL<<CLK_CLKSEL2_TMR10SEL_Pos)        /*!< Select TMR10 clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL2_TMR10SEL_PCLK2         (0x2UL<<CLK_CLKSEL2_TMR10SEL_Pos)        /*!< Select TMR10 clock source from PCLK2 \hideinitializer */
#define CLK_CLKSEL2_TMR10SEL_EXT           (0x3UL<<CLK_CLKSEL2_TMR10SEL_Pos)        /*!< Select TMR10 clock source from external trigger \hideinitializer */
#define CLK_CLKSEL2_TMR10SEL_LIRC          (0x5UL<<CLK_CLKSEL2_TMR10SEL_Pos)        /*!< Select TMR10 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL2_TMR10SEL_HIRC          (0x7UL<<CLK_CLKSEL2_TMR10SEL_Pos)        /*!< Select TMR10 clock source from high speed oscillator \hideinitializer */
#define CLK_CLKSEL2_TMR11SEL_HXT           (0x0UL<<CLK_CLKSEL2_TMR11SEL_Pos)        /*!< Select TMR11 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL2_TMR11SEL_LXT           (0x1UL<<CLK_CLKSEL2_TMR11SEL_Pos)        /*!< Select TMR11 clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL2_TMR11SEL_PCLK2         (0x2UL<<CLK_CLKSEL2_TMR11SEL_Pos)        /*!< Select TMR11 clock source from PCLK2 \hideinitializer */
#define CLK_CLKSEL2_TMR11SEL_EXT           (0x3UL<<CLK_CLKSEL2_TMR11SEL_Pos)        /*!< Select TMR11 clock source from external trigger \hideinitializer */
#define CLK_CLKSEL2_TMR11SEL_LIRC          (0x5UL<<CLK_CLKSEL2_TMR11SEL_Pos)        /*!< Select TMR11 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL2_TMR11SEL_HIRC          (0x7UL<<CLK_CLKSEL2_TMR11SEL_Pos)        /*!< Select TMR11 clock source from high speed oscillator \hideinitializer */
#define CLK_CLKSEL2_UART0SEL_HXT           (0x0UL<<CLK_CLKSEL2_UART0SEL_Pos)        /*!< Select UART0 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2  (0x1UL<<CLK_CLKSEL2_UART0SEL_Pos)        /*!< Select UART0 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL2_UART1SEL_HXT           (0x0UL<<CLK_CLKSEL2_UART1SEL_Pos)        /*!< Select UART1 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL2_UART1SEL_SYSCLK1_DIV2  (0x1UL<<CLK_CLKSEL2_UART1SEL_Pos)        /*!< Select UART1 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL2_UART2SEL_HXT           (0x0UL<<CLK_CLKSEL2_UART2SEL_Pos)        /*!< Select UART2 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL2_UART2SEL_SYSCLK1_DIV2  (0x1UL<<CLK_CLKSEL2_UART2SEL_Pos)        /*!< Select UART2 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL2_UART3SEL_HXT           (0x0UL<<CLK_CLKSEL2_UART3SEL_Pos)        /*!< Select UART3 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL2_UART3SEL_SYSCLK1_DIV2  (0x1UL<<CLK_CLKSEL2_UART3SEL_Pos)        /*!< Select UART3 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL2_UART4SEL_HXT           (0x0UL<<CLK_CLKSEL2_UART4SEL_Pos)        /*!< Select UART4 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL2_UART4SEL_SYSCLK1_DIV2  (0x1UL<<CLK_CLKSEL2_UART4SEL_Pos)        /*!< Select UART4 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL2_UART5SEL_HXT           (0x0UL<<CLK_CLKSEL2_UART5SEL_Pos)        /*!< Select UART5 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL2_UART5SEL_SYSCLK1_DIV2  (0x1UL<<CLK_CLKSEL2_UART5SEL_Pos)        /*!< Select UART5 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL2_UART6SEL_HXT           (0x0UL<<CLK_CLKSEL2_UART6SEL_Pos)        /*!< Select UART6 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL2_UART6SEL_SYSCLK1_DIV2  (0x1UL<<CLK_CLKSEL2_UART6SEL_Pos)        /*!< Select UART6 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL2_UART7SEL_HXT           (0x0UL<<CLK_CLKSEL2_UART7SEL_Pos)        /*!< Select UART7 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL2_UART7SEL_SYSCLK1_DIV2  (0x1UL<<CLK_CLKSEL2_UART7SEL_Pos)        /*!< Select UART7 clock source from SYSCLK1/2 \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  CLKSEL3 constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKSEL3_UART8SEL_HXT           (0x0UL<<CLK_CLKSEL3_UART8SEL_Pos)        /*!< Select UART8 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL3_UART8SEL_SYSCLK1_DIV2  (0x1UL<<CLK_CLKSEL3_UART8SEL_Pos)        /*!< Select UART8 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL3_UART9SEL_HXT           (0x0UL<<CLK_CLKSEL3_UART9SEL_Pos)        /*!< Select UART9 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL3_UART9SEL_SYSCLK1_DIV2  (0x1UL<<CLK_CLKSEL3_UART9SEL_Pos)        /*!< Select UART9 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL3_UART10SEL_HXT          (0x0UL<<CLK_CLKSEL3_UART10SEL_Pos)       /*!< Select UART10 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL3_UART10SEL_SYSCLK1_DIV2 (0x1UL<<CLK_CLKSEL3_UART10SEL_Pos)       /*!< Select UART10 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL3_UART11SEL_HXT          (0x0UL<<CLK_CLKSEL3_UART11SEL_Pos)       /*!< Select UART11 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL3_UART11SEL_SYSCLK1_DIV2 (0x1UL<<CLK_CLKSEL3_UART11SEL_Pos)       /*!< Select UART11 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL3_UART12SEL_HXT          (0x0UL<<CLK_CLKSEL3_UART12SEL_Pos)       /*!< Select UART12 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL3_UART12SEL_SYSCLK1_DIV2 (0x1UL<<CLK_CLKSEL3_UART12SEL_Pos)       /*!< Select UART12 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL3_UART13SEL_HXT          (0x0UL<<CLK_CLKSEL3_UART13SEL_Pos)       /*!< Select UART13 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL3_UART13SEL_SYSCLK1_DIV2 (0x1UL<<CLK_CLKSEL3_UART13SEL_Pos)       /*!< Select UART13 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL3_UART14SEL_HXT          (0x0UL<<CLK_CLKSEL3_UART14SEL_Pos)       /*!< Select UART14 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL3_UART14SEL_SYSCLK1_DIV2 (0x1UL<<CLK_CLKSEL3_UART14SEL_Pos)       /*!< Select UART14 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL3_UART15SEL_HXT          (0x0UL<<CLK_CLKSEL3_UART15SEL_Pos)       /*!< Select UART15 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL3_UART15SEL_SYSCLK1_DIV2 (0x1UL<<CLK_CLKSEL3_UART15SEL_Pos)       /*!< Select UART15 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL3_UART16SEL_HXT          (0x0UL<<CLK_CLKSEL3_UART16SEL_Pos)       /*!< Select UART16 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL3_UART16SEL_SYSCLK1_DIV2 (0x1UL<<CLK_CLKSEL3_UART16SEL_Pos)       /*!< Select UART16 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL3_WDT0SEL_LXT            (0x1UL<<CLK_CLKSEL3_WDT0SEL_Pos)         /*!< Select WDT0 clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL3_WDT0SEL_PCLK3_DIV4096  (0x2UL<<CLK_CLKSEL3_WDT0SEL_Pos)         /*!< Select WDT0 clock source from PCLK3/4096 \hideinitializer */
#define CLK_CLKSEL3_WDT0SEL_LIRC           (0x3UL<<CLK_CLKSEL3_WDT0SEL_Pos)         /*!< Select WDT0 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL3_WWDT0SEL_PCLK3_DIV4096 (0x2UL<<CLK_CLKSEL3_WWDT0SEL_Pos)        /*!< Select WWDT0 clock source from PCLK3/4096 \hideinitializer */
#define CLK_CLKSEL3_WWDT0SEL_LIRC          (0x3UL<<CLK_CLKSEL3_WWDT0SEL_Pos)        /*!< Select WWDT0 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL3_WDT1SEL_LXT            (0x1UL<<CLK_CLKSEL3_WDT1SEL_Pos)         /*!< Select WDT1 clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL3_WDT1SEL_PCLK3_DIV4096  (0x2UL<<CLK_CLKSEL3_WDT1SEL_Pos)         /*!< Select WDT1 clock source from PCLK3/4096 \hideinitializer */
#define CLK_CLKSEL3_WDT1SEL_LIRC           (0x3UL<<CLK_CLKSEL3_WDT1SEL_Pos)         /*!< Select WDT1 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL3_WWDT1SEL_PCLK3_DIV4096 (0x2UL<<CLK_CLKSEL3_WWDT1SEL_Pos)        /*!< Select WWDT1 clock source from PCLK3/4096 \hideinitializer */
#define CLK_CLKSEL3_WWDT1SEL_LIRC          (0x3UL<<CLK_CLKSEL3_WWDT1SEL_Pos)        /*!< Select WWDT1 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL3_WDT2SEL_LXT            (0x1UL<<CLK_CLKSEL3_WDT2SEL_Pos)         /*!< Select WDT2 clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL3_WDT2SEL_PCLK4_DIV4096  (0x2UL<<CLK_CLKSEL3_WDT2SEL_Pos)         /*!< Select WDT2 clock source from PCLK4/4096 \hideinitializer */
#define CLK_CLKSEL3_WDT2SEL_LIRC           (0x3UL<<CLK_CLKSEL3_WDT2SEL_Pos)         /*!< Select WDT2 clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL3_WWDT2SEL_PCLK4_DIV4096 (0x2UL<<CLK_CLKSEL3_WWDT2SEL_Pos)        /*!< Select WWDT2 clock source from PCLK4/4096 \hideinitializer */
#define CLK_CLKSEL3_WWDT2SEL_LIRC          (0x3UL<<CLK_CLKSEL3_WWDT2SEL_Pos)        /*!< Select WWDT2 clock source from low speed oscillator \hideinitializer */


/*---------------------------------------------------------------------------------------------------------*/
/*  CLKSEL4 constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKSEL4_SPI0SEL_PCLK1          (0x0UL<<CLK_CLKSEL4_SPI0SEL_Pos)         /*!< Select SPI0 clock source from PCLK1 \hideinitializer */
#define CLK_CLKSEL4_SPI0SEL_APLL           (0x1UL<<CLK_CLKSEL4_SPI0SEL_Pos)         /*!< Select SPI0 clock source from APLL \hideinitializer */
#define CLK_CLKSEL4_SPI1SEL_PCLK2          (0x0UL<<CLK_CLKSEL4_SPI1SEL_Pos)         /*!< Select SPI1 clock source from PCLK2 \hideinitializer */
#define CLK_CLKSEL4_SPI1SEL_APLL           (0x1UL<<CLK_CLKSEL4_SPI1SEL_Pos)         /*!< Select SPI1 clock source from APLL \hideinitializer */
#define CLK_CLKSEL4_SPI2SEL_PCLK1          (0x0UL<<CLK_CLKSEL4_SPI2SEL_Pos)         /*!< Select SPI2 clock source from PCLK1 \hideinitializer */
#define CLK_CLKSEL4_SPI2SEL_APLL           (0x1UL<<CLK_CLKSEL4_SPI2SEL_Pos)         /*!< Select SPI2 clock source from APLL \hideinitializer */
#define CLK_CLKSEL4_SPI3SEL_PCLK2          (0x0UL<<CLK_CLKSEL4_SPI3SEL_Pos)         /*!< Select SPI3 clock source from PCLK2 \hideinitializer */
#define CLK_CLKSEL4_SPI3SEL_APLL           (0x1UL<<CLK_CLKSEL4_SPI3SEL_Pos)         /*!< Select SPI3 clock source from APLL \hideinitializer */
#define CLK_CLKSEL4_QSPI0SEL_PCLK0         (0x0UL<<CLK_CLKSEL4_QSPI0SEL_Pos)        /*!< Select QSPI0 clock source from PCLK0 \hideinitializer */
#define CLK_CLKSEL4_QSPI0SEL_APLL          (0x1UL<<CLK_CLKSEL4_QSPI0SEL_Pos)        /*!< Select QSPI0 clock source from APLL \hideinitializer */
#define CLK_CLKSEL4_QSPI1SEL_PCLK0         (0x0UL<<CLK_CLKSEL4_QSPI1SEL_Pos)        /*!< Select QSPI1 clock source from PCLK0 \hideinitializer */
#define CLK_CLKSEL4_QSPI1SEL_APLL          (0x1UL<<CLK_CLKSEL4_QSPI1SEL_Pos)        /*!< Select QSPI1 clock source from APLL \hideinitializer */
#define CLK_CLKSEL4_I2S0SEL_APLL           (0x0UL<<CLK_CLKSEL4_I2S0SEL_Pos)         /*!< Select I2S0 clock source from APLL \hideinitializer */
#define CLK_CLKSEL4_I2S0SEL_SYSCLK1_DIV2   (0x1UL<<CLK_CLKSEL4_I2S0SEL_Pos)         /*!< Select I2S0 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL4_I2S1SEL_APLL           (0x0UL<<CLK_CLKSEL4_I2S1SEL_Pos)         /*!< Select I2S1 clock source from APLL \hideinitializer */
#define CLK_CLKSEL4_I2S1SEL_SYSCLK1_DIV2   (0x1UL<<CLK_CLKSEL4_I2S1SEL_Pos)         /*!< Select I2S1 clock source from SYSCLK1/2 \hideinitializer */
#define CLK_CLKSEL4_CANFD0SEL_APLL         (0x0UL<<CLK_CLKSEL4_CANFD0SEL_Pos)       /*!< Select CANFD0 clock source from APLL \hideinitializer */
#define CLK_CLKSEL4_CANFD0SEL_VPLL         (0x1UL<<CLK_CLKSEL4_CANFD0SEL_Pos)       /*!< Select CANFD0 clock source from VPLL \hideinitializer */
#define CLK_CLKSEL4_CANFD1SEL_APLL         (0x0UL<<CLK_CLKSEL4_CANFD1SEL_Pos)       /*!< Select CANFD1 clock source from APLL \hideinitializer */
#define CLK_CLKSEL4_CANFD1SEL_VPLL         (0x1UL<<CLK_CLKSEL4_CANFD1SEL_Pos)       /*!< Select CANFD1 clock source from VPLL \hideinitializer */
#define CLK_CLKSEL4_CANFD2SEL_APLL         (0x0UL<<CLK_CLKSEL4_CANFD2SEL_Pos)       /*!< Select CANFD2 clock source from APLL \hideinitializer */
#define CLK_CLKSEL4_CANFD2SEL_VPLL         (0x1UL<<CLK_CLKSEL4_CANFD2SEL_Pos)       /*!< Select CANFD2 clock source from VPLL \hideinitializer */
#define CLK_CLKSEL4_CANFD3SEL_APLL         (0x0UL<<CLK_CLKSEL4_CANFD3SEL_Pos)       /*!< Select CANFD3 clock source from APLL \hideinitializer */
#define CLK_CLKSEL4_CANFD3SEL_VPLL         (0x1UL<<CLK_CLKSEL4_CANFD3SEL_Pos)       /*!< Select CANFD3 clock source from VPLL \hideinitializer */
#define CLK_CLKSEL4_CKOSEL_HXT             (0x0UL<<CLK_CLKSEL4_CKOSEL_Pos)          /*!< Select CKO clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL4_CKOSEL_LXT             (0x1UL<<CLK_CLKSEL4_CKOSEL_Pos)          /*!< Select CKO clock source from low speed crystal \hideinitializer */
#define CLK_CLKSEL4_CKOSEL_HIRC            (0x2UL<<CLK_CLKSEL4_CKOSEL_Pos)          /*!< Select CKO clock source from high speed oscillator \hideinitializer */
#define CLK_CLKSEL4_CKOSEL_LIRC            (0x3UL<<CLK_CLKSEL4_CKOSEL_Pos)          /*!< Select CKO clock source from low speed oscillator \hideinitializer */
#define CLK_CLKSEL4_CKOSEL_CAPLL_DIV4      (0x4UL<<CLK_CLKSEL4_CKOSEL_Pos)          /*!< Select CKO clock source from CA-PLL/4 \hideinitializer */
#define CLK_CLKSEL4_CKOSEL_SYSPLL          (0x5UL<<CLK_CLKSEL4_CKOSEL_Pos)          /*!< Select CKO clock source from SYS-PLL \hideinitializer */
#define CLK_CLKSEL4_CKOSEL_DDRPLL          (0x6UL<<CLK_CLKSEL4_CKOSEL_Pos)          /*!< Select CKO clock source from DDR-PLL \hideinitializer */
#define CLK_CLKSEL4_CKOSEL_EPLL_DIV2       (0x7UL<<CLK_CLKSEL4_CKOSEL_Pos)          /*!< Select CKO clock source from EPLL/2 \hideinitializer */
#define CLK_CLKSEL4_CKOSEL_APLL            (0x8UL<<CLK_CLKSEL4_CKOSEL_Pos)          /*!< Select CKO clock source from APLL \hideinitializer */
#define CLK_CLKSEL4_CKOSEL_VPLL            (0x9UL<<CLK_CLKSEL4_CKOSEL_Pos)          /*!< Select CKO clock source from VPLL \hideinitializer */
#define CLK_CLKSEL4_SC0SEL_HXT             (0x0UL<<CLK_CLKSEL4_SC0SEL_Pos)          /*!< Select SC0 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL4_SC0SEL_PCLK4           (0x1UL<<CLK_CLKSEL4_SC0SEL_Pos)          /*!< Select SC0 clock source from PCLK4 \hideinitializer */
#define CLK_CLKSEL4_SC1SEL_HXT             (0x0UL<<CLK_CLKSEL4_SC1SEL_Pos)          /*!< Select SC1 clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL4_SC1SEL_PCLK4           (0x1UL<<CLK_CLKSEL4_SC1SEL_Pos)          /*!< Select SC1 clock source from PCLK4 \hideinitializer */
#define CLK_CLKSEL4_KPISEL_HXT             (0x0UL<<CLK_CLKSEL4_KPISEL_Pos)          /*!< Select KPI clock source from high speed crystal \hideinitializer */
#define CLK_CLKSEL4_KPISEL_LXT             (0x1UL<<CLK_CLKSEL4_KPISEL_Pos)          /*!< Select KPI clock source from low speed crystal \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  CLKDIV0 constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKDIV0_CANFD0(x)              (((x) - 1UL) << CLK_CLKDIV0_CANFD0DIV_Pos) /*!< CLKDIV0 Setting for CANFD0 clock divider. It could be 1~8 \hideinitializer */
#define CLK_CLKDIV0_CANFD1(x)              (((x) - 1UL) << CLK_CLKDIV0_CANFD1DIV_Pos) /*!< CLKDIV0 Setting for CANFD1 clock divider. It could be 1~8 \hideinitializer */
#define CLK_CLKDIV0_CANFD2(x)              (((x) - 1UL) << CLK_CLKDIV0_CANFD2DIV_Pos) /*!< CLKDIV0 Setting for CANFD2 clock divider. It could be 1~8 \hideinitializer */
#define CLK_CLKDIV0_CANFD3(x)              (((x) - 1UL) << CLK_CLKDIV0_CANFD3DIV_Pos) /*!< CLKDIV0 Setting for CANFD3 clock divider. It could be 1~8 \hideinitializer */
#define CLK_CLKDIV0_GMAC1(x)               (((x) - 1UL) << CLK_CLKDIV0_GMAC1DIV_Pos)  /*!< CLKDIV0 Setting for GMAC1 clock divider. It could be 1~4 \hideinitializer */
#define CLK_CLKDIV0_GMAC0(x)               (((x) - 1UL) << CLK_CLKDIV0_GMAC0DIV_Pos)  /*!< CLKDIV0 Setting for GMAC0 clock divider. It could be 1~4 \hideinitializer */
#define CLK_CLKDIV0_ACLK0(x)               (((x) - 1UL) << CLK_CLKDIV0_ACLK0DIV_Pos)  /*!< CLKDIV0 Setting for ACLK0 clock divider. It could be 1~2 \hideinitializer */
#define CLK_CLKDIV0_DCUP(x)                (((x) - 1UL) << CLK_CLKDIV0_DCUPDIV_Pos)   /*!< CLKDIV0 Setting for DCUP clock divider. It could be 1~8 \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  CLKDIV1 constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKDIV1_CCAP0(x)               (((x) - 1UL) << CLK_CLKDIV1_CCAP0DIV_Pos)  /*!< CLKDIV1 Setting for CCAP0 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV1_CCAP1(x)               (((x) - 1UL) << CLK_CLKDIV1_CCAP1DIV_Pos)  /*!< CLKDIV1 Setting for CCAP1 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV1_UART0(x)               (((x) - 1UL) << CLK_CLKDIV1_UART0DIV_Pos)  /*!< CLKDIV1 Setting for UART0 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV1_UART1(x)               (((x) - 1UL) << CLK_CLKDIV1_UART1DIV_Pos)  /*!< CLKDIV1 Setting for UART1 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV1_UART2(x)               (((x) - 1UL) << CLK_CLKDIV1_UART2DIV_Pos)  /*!< CLKDIV1 Setting for UART2 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV1_UART3(x)               (((x) - 1UL) << CLK_CLKDIV1_UART3DIV_Pos)  /*!< CLKDIV1 Setting for UART3 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV1_SC0(x)                 (((x) - 1UL) << CLK_CLKDIV1_SC0DIV_Pos)    /*!< CLKDIV1 Setting for SC0 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV1_SC1(x)                 (((x) - 1UL) << CLK_CLKDIV1_SC1DIV_Pos)    /*!< CLKDIV1 Setting for SC1 clock divider. It could be 1~16 \hideinitializer */


/*---------------------------------------------------------------------------------------------------------*/
/*  CLKDIV2 constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKDIV2_UART4(x)               (((x) - 1UL) << CLK_CLKDIV2_UART4DIV_Pos)  /*!< CLKDIV2 Setting for UART4 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV2_UART5(x)               (((x) - 1UL) << CLK_CLKDIV2_UART5DIV_Pos)  /*!< CLKDIV2 Setting for UART5 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV2_UART6(x)               (((x) - 1UL) << CLK_CLKDIV2_UART6DIV_Pos)  /*!< CLKDIV2 Setting for UART6 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV2_UART7(x)               (((x) - 1UL) << CLK_CLKDIV2_UART7DIV_Pos)  /*!< CLKDIV2 Setting for UART7 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV2_UART8(x)               (((x) - 1UL) << CLK_CLKDIV2_UART8DIV_Pos)  /*!< CLKDIV2 Setting for UART8 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV2_UART9(x)               (((x) - 1UL) << CLK_CLKDIV2_UART9DIV_Pos)  /*!< CLKDIV2 Setting for UART9 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV2_UART10(x)              (((x) - 1UL) << CLK_CLKDIV2_UART10DIV_Pos) /*!< CLKDIV2 Setting for UART10 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV2_UART11(x)              (((x) - 1UL) << CLK_CLKDIV2_UART11DIV_Pos) /*!< CLKDIV2 Setting for UART11 clock divider. It could be 1~16 \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  CLKDIV3 constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKDIV3_UART12(x)              (((x) - 1UL) << CLK_CLKDIV3_UART12DIV_Pos) /*!< CLKDIV3 Setting for UART12 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV3_UART13(x)              (((x) - 1UL) << CLK_CLKDIV3_UART13DIV_Pos) /*!< CLKDIV3 Setting for UART13 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV3_UART14(x)              (((x) - 1UL) << CLK_CLKDIV3_UART14DIV_Pos) /*!< CLKDIV3 Setting for UART14 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV3_UART15(x)              (((x) - 1UL) << CLK_CLKDIV3_UART15DIV_Pos) /*!< CLKDIV3 Setting for UART15 clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV3_UART16(x)              (((x) - 1UL) << CLK_CLKDIV3_UART16DIV_Pos) /*!< CLKDIV3 Setting for UART16 clock divider. It could be 1~16 \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  CLKDIV4 constant definitions.                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_CLKDIV4_ADC(x)                 (((x) - 1UL) << CLK_CLKDIV4_ADCDIV_Pos)    /*!< CLKDIV4 Setting for ADC clock divider. It could be 1~131072 \hideinitializer */
#define CLK_CLKDIV4_EADC(x)                (((x) - 1UL) << CLK_CLKDIV4_EADCDIV_Pos)   /*!< CLKDIV4 Setting for EADC clock divider. It could be 1~16 \hideinitializer */
#define CLK_CLKDIV4_KPI(x)                 (((x) - 1UL) << CLK_CLKDIV4_KPIDIV_Pos)    /*!< CLKDIV4 Setting for KPI clock divider. It could be 1~256 \hideinitializer */

/*---------------------------------------------------------------------------------------------------------*/
/*  MODULE constant definitions.                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

/* APBCLK(31:29)|CLKSEL(28:26)|CLKSEL_Msk(25:22) |CLKSEL_Pos(21:17)|CLKDIV(16:14)|CLKDIV_Msk(13:10)|CLKDIV_Pos(9:5)|IP_EN_Pos(4:0) */

#define MODULE_APBCLK(x)        (((x) >>29) & 0x7UL)    /*!< Calculate SYSCLK/APBCLK offset on MODULE index, 0x0:SYSCLK0, 0x1:SYSCLK1, 0x2:APBCLK0, 0x3:APBCLK1, 0x4:APBCLK2 \hideinitializer */
#define MODULE_CLKSEL(x)        (((x) >>26) & 0x7UL)    /*!< Calculate CLKSEL offset on MODULE index, 0x0:CLKSEL0, 0x1:CLKSEL1, 0x2:CLKSEL2, 0x3:CLKSEL3, 0x4:CLKSEL4 \hideinitializer */
#define MODULE_CLKSEL_Msk(x)    (((x) >>22) & 0xfUL)    /*!< Calculate CLKSEL mask offset on MODULE index \hideinitializer */
#define MODULE_CLKSEL_Pos(x)    (((x) >>17) & 0x1fUL)   /*!< Calculate CLKSEL position offset on MODULE index \hideinitializer */
#define MODULE_CLKDIV(x)        (((x) >>14) & 0x7UL)    /*!< Calculate APBCLK CLKDIV on MODULE index, 0x0:CLKDIV0, 0x1:CLKDIV1, 0x2:CLKDIV2, 0x3:CLKDIV3, 0x4:CLKDIV4 \hideinitializer */
#define MODULE_CLKDIV_Msk(x)    (((x) >>10) & 0xfUL)    /*!< Calculate CLKDIV mask offset on MODULE index \hideinitializer */
#define MODULE_CLKDIV_Pos(x)    (((x) >>5 ) & 0x1fUL)   /*!< Calculate CLKDIV position offset on MODULE index \hideinitializer */
#define MODULE_IP_EN_Pos(x)     (((x) >>0 ) & 0x1fUL)   /*!< Calculate APBCLK offset on MODULE index \hideinitializer */
#define MODULE_NoMsk            0x0UL                   /*!< Not mask on MODULE index \hideinitializer */
#define NA                      MODULE_NoMsk            /*!< Not Available \hideinitializer */

#define MODULE_APBCLK_ENC(x)        (((x) & 0x07UL) << 29)   /*!< MODULE index, 0x0:SYSCLK0, 0x1:SYSCLK1, 0x2:APBCLK0, 0x3:APBCLK1, 0x4:APBCLK2 \hideinitializer */
#define MODULE_CLKSEL_ENC(x)        (((x) & 0x07UL) << 26)   /*!< CLKSEL offset on MODULE index, 0x0:CLKSEL0, 0x1:CLKSEL1, 0x2:CLKSEL2, 0x3:CLKSEL3, 0x4:CLKSEL4 \hideinitializer */
#define MODULE_CLKSEL_Msk_ENC(x)    (((x) & 0x0fUL) << 22)   /*!< CLKSEL mask offset on MODULE index \hideinitializer */
#define MODULE_CLKSEL_Pos_ENC(x)    (((x) & 0x1fUL) << 17)   /*!< CLKSEL position offset on MODULE index \hideinitializer */
#define MODULE_CLKDIV_ENC(x)        (((x) & 0x07UL) << 14)   /*!< APBCLK CLKDIV on MODULE index, 0x0:CLKDIV0, 0x1:CLKDIV1, 0x2:CLKDIV2, 0x3:CLKDIV3, 0x4:CLKDIV4 \hideinitializer */
#define MODULE_CLKDIV_Msk_ENC(x)    (((x) & 0x0fUL) << 10)   /*!< CLKDIV mask offset on MODULE index \hideinitializer */
#define MODULE_CLKDIV_Pos_ENC(x)    (((x) & 0x1fUL) << 5 )   /*!< CLKDIV position offset on MODULE index \hideinitializer */
#define MODULE_IP_EN_Pos_ENC(x)     (((x) & 0x1fUL) << 0 )   /*!< AHBCLK/APBCLK offset on MODULE index \hideinitializer */

#define PDMA0_MODULE     ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(0UL<<0))  /*!< PDMA0 Module \hideinitializer */
#define PDMA1_MODULE     ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(1UL<<0))  /*!< PDMA1 Module \hideinitializer */
#define PDMA2_MODULE     ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(2UL<<0))  /*!< PDMA2 Module \hideinitializer */
#define PDMA3_MODULE     ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(3UL<<0))  /*!< PDMA3 Module \hideinitializer */
#define WH0_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(4UL<<0))  /*!< WH0 Module \hideinitializer */
#define WH1_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(5UL<<0))  /*!< WH1 Module \hideinitializer */
#define HWS_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(6UL<<0))  /*!< HWS Module \hideinitializer */
#define EBI_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(7UL<<0))  /*!< EBI Module \hideinitializer */
#define SRAM0_MODULE     ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(8UL<<0))  /*!< SRAM0 Module \hideinitializer */
#define SRAM1_MODULE     ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(9UL<<0))  /*!< SRAM1 Module \hideinitializer */
#define ROM_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(10UL<<0)) /*!< ROM Module \hideinitializer */
#define TRA_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(11UL<<0)) /*!< TRA Module \hideinitializer */
#define DBG_MODULE       ((1UL<<29)|(0UL<<26)         |(0x1UL<<22)       |(27UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(12UL<<0)) /*!< DBG Module \hideinitializer */
#define CLKO_MODULE      ((1UL<<29)|(4UL<<26)         |(0xfUL<<22)       |(24UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(13UL<<0)) /*!< CLKO Module \hideinitializer */
#define GTMR_MODULE      ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(14UL<<0)) /*!< GTMR Module \hideinitializer */
#define GPA_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(16UL<<0)) /*!< GPA Module \hideinitializer */
#define GPB_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(17UL<<0)) /*!< GPB Module \hideinitializer */
#define GPC_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(18UL<<0)) /*!< GPC Module \hideinitializer */
#define GPD_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(19UL<<0)) /*!< GPD Module \hideinitializer */
#define GPE_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(20UL<<0)) /*!< GPE Module \hideinitializer */
#define GPF_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(21UL<<0)) /*!< GPF Module \hideinitializer */
#define GPG_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(22UL<<0)) /*!< GPG Module \hideinitializer */
#define GPH_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(23UL<<0)) /*!< GPH Module \hideinitializer */
#define GPI_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(24UL<<0)) /*!< GPI Module \hideinitializer */
#define GPJ_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(25UL<<0)) /*!< GPJ Module \hideinitializer */
#define GPK_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(26UL<<0)) /*!< GPK Module \hideinitializer */
#define GPL_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(27UL<<0)) /*!< GPL Module \hideinitializer */
#define GPM_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(28UL<<0)) /*!< GPM Module \hideinitializer */
#define GPN_MODULE       ((1UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(29UL<<0)) /*!< GPN Module \hideinitializer */
#define CA35_MODULE      ((0UL<<29 |(0UL<<26)         |(0x3UL<<22)       |(0UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(0UL<<0))  /*!< CA35 Module \hideinitializer */
#define SYSCK0_MODULE    ((0UL<<29)|(0UL<<26)         |(0x1UL<<22)       |(2UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(0UL<<0))  /*!< SYSCK0 Module \hideinitializer */
#define SYSCK1_MODULE    ((0UL<<29)|(0UL<<26)         |(0x1UL<<22)       |(4UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(0UL<<0))  /*!< SYSCK1 Module \hideinitializer */
#define RTPST_MODULE     ((0UL<<29)|(0UL<<26)         |(0x7UL<<22)       |(8UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(1UL<<0))  /*!< RTPST Module \hideinitializer */
#define LVRDB_MODULE     ((0UL<<29)|(0UL<<26)         |(0x1UL<<22)       |(3UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(3UL<<0))  /*!< LVRDB Module \hideinitializer */
#define DDR0_MODULE      ((0UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(4UL<<0))  /*!< DDR0 Module \hideinitializer */
#define DDR6_MODULE      ((0UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(5UL<<0))  /*!< DDR6 Module \hideinitializer */
#define CANFD0_MODULE    ((0UL<<29)|(4UL<<26)         |(0x1UL<<22)       |(16UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(8UL<<0))  /*!< CANFD0 Module \hideinitializer */
#define CANFD1_MODULE    ((0UL<<29)|(4UL<<26)         |(0x1UL<<22)       |(17UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(9UL<<0))  /*!< CANFD1 Module \hideinitializer */
#define CANFD2_MODULE    ((0UL<<29)|(4UL<<26)         |(0x1UL<<22)       |(18UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(10UL<<0)) /*!< CANFD2 Module \hideinitializer */
#define CANFD3_MODULE    ((0UL<<29)|(4UL<<26)         |(0x1UL<<22)       |(19UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(11UL<<0)) /*!< CANFD3 Module \hideinitializer */
#define SD0_MODULE       ((0UL<<29)|(0UL<<26)         |(0x3UL<<22)       |(16UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(16UL<<0)) /*!< SD0 Module \hideinitializer */
#define SD1_MODULE       ((0UL<<29)|(0UL<<26)         |(0x3UL<<22)       |(18UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(17UL<<0)) /*!< SD1 Module \hideinitializer */
#define NAND_MODULE      ((0UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(18UL<<0)) /*!< NAND Module \hideinitializer */
#define USBD_MODULE      ((0UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(19UL<<0)) /*!< USBD Module \hideinitializer */
#define USBH_MODULE      ((0UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(20UL<<0)) /*!< USBH Module \hideinitializer */
#define HUSBH0_MODULE    ((0UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(21UL<<0)) /*!< HUSBH0 Module \hideinitializer */
#define HUSBH1_MODULE    ((0UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(22UL<<0)) /*!< HUSBH1 Module \hideinitializer */
#define GFX_MODULE       ((0UL<<29)|(0UL<<26)         |(0x1UL<<22)       |(26UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(24UL<<0)) /*!< GFX Module \hideinitializer */
#define VDEC_MODULE      ((0UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(25UL<<0)) /*!< VDEC Module \hideinitializer */
#define DCU_MODULE       ((0UL<<29)|(0UL<<26)         |(0x1UL<<22)       |(24UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(26UL<<0)) /*!< DCU Module \hideinitializer */
#define DCUP_MODULE      ((0UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(0UL<<14)         |(0x7UL<<10)       |(16UL<<5)        |(26UL<<0)) /*!< DCUP Module \hideinitializer */
#define GMAC0_MODULE     ((0UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(0UL<<14)         |(0x3UL<<10)       |(28UL<<5)        |(27UL<<0)) /*!< GMAC0 Module \hideinitializer */
#define GMAC1_MODULE     ((0UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(0UL<<14)         |(0x3UL<<10)       |(30UL<<5)        |(28UL<<0)) /*!< GMAC1 Module \hideinitializer */
#define CCAP0_MODULE     ((0UL<<29)|(0UL<<26)         |(0x3UL<<22)       |(12UL<<17)        |(1UL<<14)         |(0xFUL<<10)       |(8UL<<5)         |(29UL<<0)) /*!< CCAP0 Module \hideinitializer */
#define CCAP1_MODULE     ((0UL<<29)|(0UL<<26)         |(0x3UL<<22)       |(14UL<<17)        |(1UL<<14)         |(0xFUL<<10)       |(12UL<<5)        |(30UL<<0)) /*!< CCAP1 Module \hideinitializer */
#define TMR0_MODULE      ((2UL<<29)|(1UL<<26)         |(0x7UL<<22)       |(0UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(0UL<<0))  /*!< TMR0 Module \hideinitializer */
#define TMR1_MODULE      ((2UL<<29)|(1UL<<26)         |(0x7UL<<22)       |(4UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(1UL<<0))  /*!< TMR1 Module \hideinitializer */
#define TMR2_MODULE      ((2UL<<29)|(1UL<<26)         |(0x7UL<<22)       |(8UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(2UL<<0))  /*!< TMR2 Module \hideinitializer */
#define TMR3_MODULE      ((2UL<<29)|(1UL<<26)         |(0x7UL<<22)       |(12UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(3UL<<0))  /*!< TMR3 Module \hideinitializer */
#define TMR4_MODULE      ((2UL<<29)|(1UL<<26)         |(0x7UL<<22)       |(16UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(4UL<<0))  /*!< TMR4 Module \hideinitializer */
#define TMR5_MODULE      ((2UL<<29)|(1UL<<26)         |(0x7UL<<22)       |(20UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(5UL<<0))  /*!< TMR5 Module \hideinitializer */
#define TMR6_MODULE      ((2UL<<29)|(1UL<<26)         |(0x7UL<<22)       |(24UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(6UL<<0))  /*!< TMR6 Module \hideinitializer */
#define TMR7_MODULE      ((2UL<<29)|(1UL<<26)         |(0x7UL<<22)       |(28UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(7UL<<0))  /*!< TMR7 Module \hideinitializer */
#define TMR8_MODULE      ((2UL<<29)|(2UL<<26)         |(0x7UL<<22)       |(0UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(8UL<<0))  /*!< TMR8 Module \hideinitializer */
#define TMR9_MODULE      ((2UL<<29)|(2UL<<26)         |(0x7UL<<22)       |(4UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(9UL<<0))  /*!< TMR9 Module \hideinitializer */
#define TMR10_MODULE     ((2UL<<29)|(2UL<<26)         |(0x7UL<<22)       |(8UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(10UL<<0)) /*!< TMR10 Module \hideinitializer */
#define TMR11_MODULE     ((2UL<<29)|(2UL<<26)         |(0x7UL<<22)       |(12UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(11UL<<0)) /*!< TMR11 Module \hideinitializer */
#define UART0_MODULE     ((2UL<<29)|(2UL<<26)         |(0x3UL<<22)       |(16UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(12UL<<0)) /*!< UART0 Module \hideinitializer */
#define UART1_MODULE     ((2UL<<29)|(2UL<<26)         |(0x3UL<<22)       |(18UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(13UL<<0)) /*!< UART1 Module \hideinitializer */
#define UART2_MODULE     ((2UL<<29)|(2UL<<26)         |(0x3UL<<22)       |(20UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(14UL<<0)) /*!< UART2 Module \hideinitializer */
#define UART3_MODULE     ((2UL<<29)|(2UL<<26)         |(0x3UL<<22)       |(22UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(15UL<<0)) /*!< UART3 Module \hideinitializer */
#define UART4_MODULE     ((2UL<<29)|(2UL<<26)         |(0x3UL<<22)       |(24UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(16UL<<0)) /*!< UART4 Module \hideinitializer */
#define UART5_MODULE     ((2UL<<29)|(2UL<<26)         |(0x3UL<<22)       |(26UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(17UL<<0)) /*!< UART5 Module \hideinitializer */
#define UART6_MODULE     ((2UL<<29)|(2UL<<26)         |(0x3UL<<22)       |(28UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(18UL<<0)) /*!< UART6 Module \hideinitializer */
#define UART7_MODULE     ((2UL<<29)|(2UL<<26)         |(0x3UL<<22)       |(30UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(19UL<<0)) /*!< UART7 Module \hideinitializer */
#define UART8_MODULE     ((2UL<<29)|(3UL<<26)         |(0x3UL<<22)       |(0UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(20UL<<0)) /*!< UART8 Module \hideinitializer */
#define UART9_MODULE     ((2UL<<29)|(3UL<<26)         |(0x3UL<<22)       |(2UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(21UL<<0)) /*!< UART9 Module \hideinitializer */
#define UART10_MODULE    ((2UL<<29)|(3UL<<26)         |(0x3UL<<22)       |(4UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(22UL<<0)) /*!< UART10 Module \hideinitializer */
#define UART11_MODULE    ((2UL<<29)|(3UL<<26)         |(0x3UL<<22)       |(6UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(23UL<<0)) /*!< UART11 Module \hideinitializer */
#define UART12_MODULE    ((2UL<<29)|(3UL<<26)         |(0x3UL<<22)       |(8UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(24UL<<0)) /*!< UART12 Module \hideinitializer */
#define UART13_MODULE    ((2UL<<29)|(3UL<<26)         |(0x3UL<<22)       |(10UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(25UL<<0)) /*!< UART13 Module \hideinitializer */
#define UART14_MODULE    ((2UL<<29)|(3UL<<26)         |(0x3UL<<22)       |(12UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(26UL<<0)) /*!< UART14 Module \hideinitializer */
#define UART15_MODULE    ((2UL<<29)|(3UL<<26)         |(0x3UL<<22)       |(14UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(27UL<<0)) /*!< UART15 Module \hideinitializer */
#define UART16_MODULE    ((2UL<<29)|(3UL<<26)         |(0x3UL<<22)       |(16UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(28UL<<0)) /*!< UART16 Module \hideinitializer */
#define RTC_MODULE       ((2UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(29UL<<0)) /*!< RTC Module \hideinitializer */
#define DDRP_MODULE      ((2UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(30UL<<0)) /*!< DDRP Module \hideinitializer */
#define KPI_MODULE       ((2UL<<29)|(4UL<<26)         |(1UL<<22)         |(30UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(31UL<<0)) /*!< KPI Module \hideinitializer */
#define I2C0_MODULE      ((3UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(0UL<<0))  /*!< I2C0 Module \hideinitializer */
#define I2C1_MODULE      ((3UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(1UL<<0))  /*!< I2C1 Module \hideinitializer */
#define I2C2_MODULE      ((3UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(2UL<<0))  /*!< I2C2 Module \hideinitializer */
#define I2C3_MODULE      ((3UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(3UL<<0))  /*!< I2C3 Module \hideinitializer */
#define I2C4_MODULE      ((3UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(4UL<<0))  /*!< I2C4 Module \hideinitializer */
#define I2C5_MODULE      ((3UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(5UL<<0))  /*!< I2C5 Module \hideinitializer */
#define QSPI0_MODULE     ((3UL<<29)|(4UL<<26)         |(3UL<<22)         |(8UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(6UL<<0))  /*!< QSPI0 Module \hideinitializer */
#define QSPI1_MODULE     ((3UL<<29)|(4UL<<26)         |(3UL<<22)         |(10UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(7UL<<0))  /*!< QSPI1 Module \hideinitializer */
#define SC0_MODULE       ((3UL<<29)|(4UL<<26)         |(1UL<<22)         |(28UL<<17)        |(1UL<<14)         |(0xFUL<<10)       |(0UL<<5)         |(12UL<<0)) /*!< SC0 Module \hideinitializer */
#define SC1_MODULE       ((3UL<<29)|(4UL<<26)         |(1UL<<22)         |(29UL<<17)        |(1UL<<14)         |(0xFUL<<10)       |(4UL<<5)         |(13UL<<0)) /*!< SC1 Module \hideinitializer */
#define WDT0_MODULE      ((3UL<<29)|(3UL<<26)         |(3UL<<22)         |(20UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(16UL<<0)) /*!< WDT0 Module \hideinitializer */
#define WDT1_MODULE      ((3UL<<29)|(3UL<<26)         |(3UL<<22)         |(24UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(17UL<<0)) /*!< WDT1 Module \hideinitializer */
#define WDT2_MODULE      ((3UL<<29)|(3UL<<26)         |(3UL<<22)         |(28UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(18UL<<0)) /*!< WDT2 Module \hideinitializer */
#define WWDT0_MODULE     ((3UL<<29)|(3UL<<26)         |(3UL<<22)         |(22UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(16UL<<0)) /*!< WWDT0 Module \hideinitializer */
#define WWDT1_MODULE     ((3UL<<29)|(3UL<<26)         |(3UL<<22)         |(26UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(17UL<<0)) /*!< WWDT1 Module \hideinitializer */
#define WWDT2_MODULE     ((3UL<<29)|(3UL<<26)         |(3UL<<22)         |(30UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(18UL<<0)) /*!< WWDT2 Module \hideinitializer */
#define EPWM0_MODULE     ((3UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(24UL<<0)) /*!< EPWM0 Module \hideinitializer */
#define EPWM1_MODULE     ((3UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(25UL<<0)) /*!< EPWM1 Module \hideinitializer */
#define EPWM2_MODULE     ((3UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(26UL<<0)) /*!< EPWM2 Module \hideinitializer */
#define I2S0_MODULE      ((4UL<<29)|(4UL<<26)         |(3UL<<22)         |(12UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(0UL<<0))  /*!< I2S0 Module \hideinitializer */
#define I2S1_MODULE      ((4UL<<29)|(4UL<<26)         |(3UL<<22)         |(14UL<<17)        |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(1UL<<0))  /*!< I2S1 Module \hideinitializer */
#define SSMCC_MODULE     ((4UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(2UL<<0))  /*!< SSMCC Module \hideinitializer */
#define SSPCC_MODULE     ((4UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(3UL<<0))  /*!< SSPCC Module \hideinitializer */
#define SPI0_MODULE      ((4UL<<29)|(4UL<<26)         |(3UL<<22)         |(0UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(4UL<<0))  /*!< SPI0 Module \hideinitializer */
#define SPI1_MODULE      ((4UL<<29)|(4UL<<26)         |(3UL<<22)         |(2UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(5UL<<0))  /*!< SPI1 Module \hideinitializer */
#define SPI2_MODULE      ((4UL<<29)|(4UL<<26)         |(3UL<<22)         |(4UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(6UL<<0))  /*!< SPI2 Module \hideinitializer */
#define SPI3_MODULE      ((4UL<<29)|(4UL<<26)         |(3UL<<22)         |(6UL<<17)         |(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(7UL<<0))  /*!< SPI3 Module \hideinitializer */
#define ECAP0_MODULE     ((4UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(8UL<<0))  /*!< ECAP0 Module \hideinitializer */
#define ECAP1_MODULE     ((4UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(9UL<<0))  /*!< ECAP1 Module \hideinitializer */
#define ECAP2_MODULE     ((4UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(10UL<<0)) /*!< ECAP2 Module \hideinitializer */
#define QEI0_MODULE      ((4UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(12UL<<0)) /*!< QEI0 Module \hideinitializer */
#define QEI1_MODULE      ((4UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(13UL<<0)) /*!< QEI1 Module \hideinitializer */
#define QEI2_MODULE      ((4UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(14UL<<0)) /*!< QEI2 Module \hideinitializer */
#define ADC_MODULE       ((4UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(24UL<<0)) /*!< ADCModule \hideinitializer */
#define EADC_MODULE      ((4UL<<29)|(MODULE_NoMsk<<26)|(MODULE_NoMsk<<22)|(MODULE_NoMsk<<17)|(MODULE_NoMsk<<14)|(MODULE_NoMsk<<10)|(MODULE_NoMsk<<5)|(25UL<<0)) /*!< EADC Module \hideinitializer */

/*! @}*/ /* end of group CLK_EXPORTED_CONSTANTS */

/** @addtogroup CLK_EXPORTED_FUNCTIONS CLK Exported Functions
  @{
*/

void CLK_DisableCKO(void);
void CLK_EnableCKO(uint32_t u32ClkSrc, uint32_t u32ClkDiv, uint32_t u32ClkDivBy1En);
uint32_t CLK_GetHXTFreq(void);
uint32_t CLK_GetLXTFreq(void);
uint32_t CLK_GetSYSCLK0Freq(void);
uint32_t CLK_GetSYSCLK1Freq(void);
uint32_t CLK_GetPCLK0Freq(void);
uint32_t CLK_GetPCLK3Freq(void);
void CLK_SetModuleClock(uint32_t u32ModuleIdx, uint32_t u32ClkSrc, uint32_t u32ClkDiv);
void CLK_SetSysTickClockSrc(uint32_t u32ClkSrc);
void CLK_EnableXtalRC(uint32_t u32ClkMask);
void CLK_DisableXtalRC(uint32_t u32ClkMask);
void CLK_EnableModuleClock(uint32_t u32ModuleIdx);
void CLK_DisableModuleClock(uint32_t u32ModuleIdx);
uint32_t CLK_GetPLLOpMode(uint32_t u32PllIdx);
uint32_t CLK_WaitClockReady(uint32_t u32ClkMask);
#ifndef __aarch64__
void CLK_EnableSysTick(uint32_t u32ClkSrc, uint32_t u32Count);
void CLK_DisableSysTick(void);
#endif
uint64_t CLK_SetPLLClockFreq(uint32_t u32PllIdx, uint32_t u32OpMode, uint64_t u64PllSrcClk, uint64_t u64PllFreq);
uint32_t CLK_GetPLLClockFreq(uint32_t u32PllIdx);
void CLK_DisableAdvPLL(uint32_t u32PllIdx);

/// @cond HIDDEN_SYMBOLS
static uint32_t CLK_GetCAPLLClockFreq(void);
static uint32_t CLK_GetAdvPLLClockFreq(uint32_t u32PllIdx);
static uint64_t CLK_CalPLLFreq_Mode0(uint64_t u64PllSrcClk, uint64_t u64PllFreq, uint32_t *u32Reg);
static uint64_t CLK_SetAdvPLLFreq(uint32_t u32PllIdx, uint32_t u32OpMode, uint64_t u64PllSrcClk, uint64_t u64PllFreq);
static uint32_t CLK_GetModuleClockSource(uint32_t u32ModuleIdx);
static uint32_t CLK_GetModuleClockDivider(uint32_t u32ModuleIdx);
/// @endcond HIDDEN_SYMBOLS


/*! @}*/ /* end of group CLK_EXPORTED_FUNCTIONS */

/*! @}*/ /* end of group CLK_Driver */

/*! @}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __CLK_H__ */


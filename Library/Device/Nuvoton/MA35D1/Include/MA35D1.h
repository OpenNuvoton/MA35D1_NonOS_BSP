 /**************************************************************************//**
 * @file     MA35D1.h
 * @brief    Nuvoton ARMv8-based MA35D1 Peripheral Access Layer Header File
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
  \mainpage NuMicro MA35D1 Driver Reference Guide
  *
  * <b>Introduction</b>
  *
  * This user manual describes the usage of MA35D1 device driver
  *
  * <b>Disclaimer</b>
  *
  * The Software is furnished "AS IS", without warranty as to performance or results, and
  * the entire risk as to performance or results is assumed by YOU. Nuvoton disclaims all
  * warranties, express, implied or otherwise, with regard to the Software, its use, or
  * operation, including without limitation any and all warranties of merchantability, fitness
  * for a particular purpose, and non-infringement of intellectual property rights.
  *
  * <b>Important Notice</b>
  *
  * Nuvoton Products are neither intended nor warranted for usage in systems or equipment,
  * any malfunction or failure of which may cause loss of human life, bodily injury or severe
  * property damage. Such applications are deemed, "Insecure Usage".
  *
  * Insecure usage includes, but is not limited to: equipment for surgical implementation,
  * atomic energy control instruments, airplane or spaceship instruments, the control or
  * operation of dynamic, brake or safety systems designed for vehicular use, traffic signal
  * instruments, all types of safety devices, and other applications intended to support or
  * sustain life.
  *
  * All Insecure Usage shall be made at customer's risk, and in the event that third parties
  * lay claims to Nuvoton as a result of customer's Insecure Usage, customer shall indemnify
  * the damages and liabilities thus incurred by Nuvoton.
  *
  * Please note that all data and specifications are subject to change without notice. All the
  * trademarks of products and companies mentioned in this datasheet belong to their respective
  * owners.
  *
  * <b>Copyright Notice</b>
  *
  * Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
  */

#ifndef __MA35D1_H__
#define __MA35D1_H__

#ifdef __cplusplus
extern "C" {
#endif


#define EnSecondaryCore
#define NON_CACHE	(0x100000000ULL)

/* -------------------------  Interrupt Number Definition  ------------------------ */

/** Device specific Interrupt IDs */
typedef enum IRQn
{
/******  SGI Interrupts Numbers ************************************************/
      SGI0_IRQn            =  0,        /*!< Software Generated Interrupt 0 */
      SGI1_IRQn            =  1,        /*!< Software Generated Interrupt 1 */
      SGI2_IRQn            =  2,        /*!< Software Generated Interrupt 2 */
      SGI3_IRQn            =  3,        /*!< Software Generated Interrupt 3 */
      SGI4_IRQn            =  4,        /*!< Software Generated Interrupt 4 */
      SGI5_IRQn            =  5,        /*!< Software Generated Interrupt 5 */
      SGI6_IRQn            =  6,        /*!< Software Generated Interrupt 6 */
      SGI7_IRQn            =  7,        /*!< Software Generated Interrupt 7 */
      SGI8_IRQn            =  8,        /*!< Software Generated Interrupt 8 */
      SGI9_IRQn            =  9,        /*!< Software Generated Interrupt 9 */
      SGI10_IRQn           = 10,        /*!< Software Generated Interrupt 10 */
      SGI11_IRQn           = 11,        /*!< Software Generated Interrupt 11 */
      SGI12_IRQn           = 12,        /*!< Software Generated Interrupt 12 */
      SGI13_IRQn           = 13,        /*!< Software Generated Interrupt 13 */
      SGI14_IRQn           = 14,        /*!< Software Generated Interrupt 14 */
      SGI15_IRQn           = 15,        /*!< Software Generated Interrupt 15 */

/******  Cortex-35 Processor Exceptions Numbers ****************************************/
	  VirtualMachine_IRQn     	= 25,
	  HypervisorTimer_IRQn      = 26,
	  VirtualTimer_IRQn    		= 27,
	  LegacyFIQ_IRQn     		= 28,
	  SecPhysicalTimer_IRQn     = 29,
	  NonSecPhysicalTimer_IRQn  = 30,
	  LegacyIRQ_IRQn     		= 31,

/******  Platform Exceptions Numbers ***************************************************/
      LVD_IRQn          =   32,         /*!< Low Voltage detection Interrupt            */
      A35PMU_IRQn       =   33,         /*!< A35 PMU Interrupt                          */
      HWSEM0_IRQn       =   34,         /*!< Hardware Semaphore 0 Interrupt             */
      CKFAIL_IRQn       =   35,         /*!< Clock failed Interrupt                     */
	  WRHO0_IRQn        =   36,         /*!< Wormhole Interrupt                         */
      RTC_IRQn          =   37,         /*!< Real Time Clock Interrupt                  */
      TAMPER_IRQn       =   38,         /*!< Tamper detection Interrupt                 */
      WDT0_IRQn         =   39,         /*!< WatchMA35D1g timer 0 Interrupt             */
	  WWDT0_IRQn        =   40,         /*!< WinMA35D1w WatchMA35D1g timer 0 Interrupt  */
      EINT0_IRQn        =   41,         /*!< External Input 0 Interrupt                 */
      EINT1_IRQn        =   42,         /*!< External Input 1 Interrupt                 */
      EINT2_IRQn        =   43,         /*!< External Input 2 Interrupt                 */
      EINT3_IRQn        =   44,         /*!< External Input 3 Interrupt                 */
      I2C0_IRQn         =   45,         /*!< I2C 0 Interrupt                            */
      GPA_IRQn          =   46,         /*!< GPIO Port A Interrupt                      */
      GPB_IRQn          =   47,         /*!< GPIO Port B Interrupt                      */
      GPC_IRQn          =   48,         /*!< GPIO Port C Interrupt                      */
      GPD_IRQn          =   49,         /*!< GPIO Port D Interrupt                      */
      PDMA0_IRQn        =   50,         /*!< Peripheral DMA 0 Interrupt                 */
      PDMA1_IRQn        =   51,         /*!< Peripheral DMA 1 Interrupt                 */
      DISP_IRQn         =   52,         /*!< Display Controller (DCUltra) Interrupt     */
      CCAP0_IRQn        =   53,         /*!< CCAP 0 Interrupt                           */
      CCAP1_IRQn        =   54,         /*!< CCAP 1 Interrupt                           */
      GMAC0_IRQn        =   55,         /*!< GMAC0 RX Interrupt                         */
      GMAC1_IRQn        =   56,         /*!< GMAC1 RX Interrupt                         */
      SSMCC_IRQn        =   57,         /*!< GMAC0 TX Interrupt                         */
      SSPCC_IRQn        =   58,         /*!< GMAC1 TX Interrupt                         */
      GFX_IRQn          =   59,         /*!< GFX GC520L Interrupt (Graphic Engine)      */
      VDE_IRQn          =   60,         /*!< Video Decoder (VC8000) Interrupt           */
      WRHO1_IRQn        =   61,         /*!< WRHO 1 Interrupt                           */
      SDH0_IRQn         =   62,         /*!< SDH 0 Interrupt                            */
      SDH1_IRQn         =   63,         /*!< SDH 1 Interrupt                            */
      HSUSBD_IRQn       =   64,         /*!< USB 2.0 High-Speed Device Interrupt        */
      HSUSBH0_IRQn      =   65,         /*!< USB 2.0 High-Speed Host (EHCI) Interrupt   */
      HSUSBH1_IRQn      =   66,         /*!< USB 2.0 High-Speed Host (EHCI) Interrupt   */
      USBH0_IRQn        =   67,         /*!< USB 1.1 Host (OHCI) 0 Interrupt (Synopsys) */
      USBH1_IRQn        =   68,         /*!< USB 1.1 Host (OHCI) 0 Interrupt (Synopsys) */
      NAND_IRQn         =   70,         /*!< NAND Controller Interrupt                  */
      WDT1_IRQn         =   75,         /*!< WatchMA35D1g timer 1 Interrupt             */
      WWDT1_IRQn        =   76,         /*!< WinMA35D1w WatchMA35D1g timer 1 Interrupt  */
      PDMA2_IRQn        =   77,         /*!< Peripheral DMA 2 Interrupt                 */
      PDMA3_IRQn        =   78,         /*!< Peripheral DMA 3 Interrupt                 */
      TMR0_IRQn         =   79,         /*!< Timer 0 Interrupt                          */
      TMR1_IRQn         =   80,         /*!< Timer 1 Interrupt                          */
      TMR2_IRQn         =   81,         /*!< Timer 2 Interrupt                          */
      TMR3_IRQn         =   82,         /*!< Timer 3 Interrupt                          */
      BRAKE0_IRQn       =   83,         /*!< BRAKE0 Interrupt                           */
      EPWM0P0_IRQn      =   84,         /*!< EPWM0P0 Interrupt                          */
      EPWM0P1_IRQn      =   85,         /*!< EPWM0P1 Interrupt                          */
      EPWM0P2_IRQn      =   86,         /*!< EPWM0P2 Interrupt                          */
      QEI0_IRQn         =   87,         /*!< QEI0 Interrupt                             */
      ECAP0_IRQn        =   88,         /*!< ECAP0 Interrupt                            */
      QSPI0_IRQn        =   89,         /*!< QSPI0 Interrupt                            */
      QSPI1_IRQn        =   90,         /*!< QSPI1 Interrupt                            */
      UART0_IRQn        =   91,         /*!< UART 0 Interrupt                           */
      UART1_IRQn        =   92,         /*!< UART 1 Interrupt                           */
      UART2_IRQn        =   93,         /*!< UART 2 Interrupt                           */
      UART3_IRQn        =   94,         /*!< UART 3 Interrupt                           */
      UART4_IRQn        =   95,         /*!< UART 4 Interrupt                           */
      UART5_IRQn        =   96,         /*!< UART 5 Interrupt                           */
      EADC00_IRQn       =   97,         /*!< EADC00 Interrupt                           */
      EADC01_IRQn       =   98,         /*!< EADC01 Interrupt                           */
      EADC02_IRQn       =   99,         /*!< EADC02 Interrupt                           */
      EADC03_IRQn       =   100,        /*!< EADC03 Interrupt                           */
      I2C1_IRQn         =   101,        /*!< I2C 1 Interrupt                            */
      I2S0_IRQn         =   102,        /*!< I2S 0 Interrupt                            */
      CANFD00_IRQn      =   103,        /*!< CAN-FD 00 Interrupt                        */
      SC0_IRQn          =   104,        /*!< Smart Card 0 Interrupt                     */
      GPE_IRQn          =   105,        /*!< GPIO Port E Interrupt                      */
      GPF_IRQn          =   106,        /*!< GPIO Port F Interrupt                      */
      GPG_IRQn          =   107,        /*!< GPIO Port G Interrupt                      */
      GPH_IRQn          =   108,        /*!< GPIO Port H Interrupt                      */
      GPI_IRQn          =   109,        /*!< GPIO Port I Interrupt                      */
      GPJ_IRQn          =   110,        /*!< GPIO Port J Interrupt                      */
      KPI_IRQn          =   111,        /*!< KPI Interrupt                              */
      ADC0_IRQn         =   112,        /*!< ADC 0 (Touch Panel ADC) Interrupt          */
      TMR4_IRQn         =   113,        /*!< Timer 4 Interrupt                          */
      TMR5_IRQn         =   114,        /*!< Timer 5 Interrupt                          */
      BRAKE1_IRQn       =   115,        /*!< BRAKE1 Interrupt                           */
      EPWM1P0_IRQn      =   116,        /*!< EPWM1P0 Interrupt                          */
      EPWM1P1_IRQn      =   117,        /*!< EPWM1P1 Interrupt                          */
      EPWM1P2_IRQn      =   118,        /*!< EPWM1P2 Interrupt                          */
      QEI1_IRQn         =   119,        /*!< QEI1 Interrupt                             */
      ECAP1_IRQn        =   120,        /*!< ECAP1 Interrupt                            */
      SPI0_IRQn         =   121,        /*!< SPI0 Interrupt                             */
      SPI1_IRQn         =   122,        /*!< SPI1 Interrupt                             */
      UART6_IRQn        =   123,        /*!< UART 6 Interrupt                           */
      UART7_IRQn        =   124,        /*!< UART 7 Interrupt                           */
      UART8_IRQn        =   125,        /*!< UART 8 Interrupt                           */
      UART9_IRQn        =   126,        /*!< UART 9 Interrupt                           */
      UART10_IRQn       =   127,        /*!< UART 10 Interrupt                          */
      UART11_IRQn       =   128,        /*!< UART 11 Interrupt                          */
      I2C2_IRQn         =   129,        /*!< I2C 2 Interrupt                            */
      I2C3_IRQn         =   130,        /*!< I2C 3 Interrupt                            */
      I2S1_IRQn         =   131,        /*!< I2S 1 Interrupt                            */
      CANFD10_IRQn      =   132,        /*!< CAN-FD 10 Interrupt                        */
      SC1_IRQn          =   133,        /*!< Smart Card 1 Interrupt                     */
      GPK_IRQn          =   134,        /*!< GPIO Port K Interrupt                      */
      GPL_IRQn          =   135,        /*!< GPIO Port L Interrupt                      */
      GPM_IRQn          =   136,        /*!< GPIO Port M Interrupt                      */
      GPN_IRQn          =   137,        /*!< GPIO Port N Interrupt                      */
      TMR6_IRQn         =   138,        /*!< Timer 6 Interrupt                          */
      TMR7_IRQn         =   139,        /*!< Timer 7 Interrupt                          */
      TMR8_IRQn         =   140,        /*!< Timer 8 Interrupt                          */
      TMR9_IRQn         =   141,        /*!< Timer 9 Interrupt                          */
      BRAKE2_IRQn       =   142,        /*!< BRAKE2 Interrupt                           */
      EPWM2P0_IRQn      =   143,        /*!< EPWM2P0 Interrupt                          */
      EPWM2P1_IRQn      =   144,        /*!< EPWM2P1 Interrupt                          */
      EPWM2P2_IRQn      =   145,        /*!< EPWM2P2 Interrupt                          */
      QEI2_IRQn         =   146,        /*!< QEI2 Interrupt                             */
      ECAP2_IRQn        =   147,        /*!< ECAP2 Interrupt                            */
      SPI2_IRQn         =   148,        /*!< SPI2 Interrupt                             */
      SPI3_IRQn         =   149,        /*!< SPI3 Interrupt                             */
      UART12_IRQn       =   150,        /*!< UART 12 Interrupt                          */
      UART13_IRQn       =   151,        /*!< UART 13 Interrupt                          */
      UART14_IRQn       =   152,        /*!< UART 14 Interrupt                          */
      UART15_IRQn       =   153,        /*!< UART 15 Interrupt                          */
      UART16_IRQn       =   154,        /*!< UART 16 Interrupt                          */
      I2C4_IRQn         =   155,        /*!< I2C 4 Interrupt                            */
      I2C5_IRQn         =   156,        /*!< I2C 5 Interrupt                            */
      CANFD20_IRQn      =   157,        /*!< CAN-FD 20 Interrupt                        */
      CANFD30_IRQn      =   158,        /*!< CAN-FD 30 Interrupt                        */
      TMR10_IRQn        =   159,        /*!< Timer 10 Interrupt                         */
      TMR11_IRQn        =   160,        /*!< Timer 11 Interrupt                         */
      CANFD01_IRQn      =   163,        /*!< CAN-FD 01 Interrupt                        */
      CANFD11_IRQn      =   164,        /*!< CAN-FD 11 Interrupt                        */
      CANFD21_IRQn      =   165,        /*!< CAN-FD 21 Interrupt                        */
      CANFD31_IRQn      =   166,        /*!< CAN-FD 31 Interrupt                        */
      PWRWU_IRQn        =   167,        /*!< Power Down Wake-up interrupt               */
      DDRPOISION_IRQn   =   168,        /*!< DDR Out of range Interrupt                 */
} IRQn_Type;

/** @addtogroup PERIPHERAL_MEM_MAP Peripheral Memory Base
  Memory Mapped Structure for Peripherals
  @{
 */

/* Peripheral and RAM base address */
#define GIC_DISTRIBUTOR_BASE    (0x50801000UL)
#define GIC_INTERFACE_BASE      (0x50802000UL)

/* Peripheral and SRAM base address */
#define SRAM0_BASE				(0x24000000ul)		/*!< SRAM0 Base Address       */
#define SRAM1_BASE				(0x28000000ul)		/*!< SRAM1 Base Address       */
#define DDR_BASE				(0x80000000ul)      /*!< DDR Base Address         */

/*!< AHB peripherals */
#define SYS_BASE                (0x40460000UL)
#define TS_BASE                 (0x40460104UL)
#define CLK_BASE                (0x40460200UL)
#define NMI_BASE                (0x40460300UL)
#define GPIOA_BASE              (0x40040000UL)
#define GPIOB_BASE              (0x40040040UL)
#define GPIOC_BASE              (0x40040080UL)
#define GPIOD_BASE              (0x400400C0UL)
#define GPIOE_BASE              (0x40040100UL)
#define GPIOF_BASE              (0x40040140UL)
#define GPIOG_BASE              (0x40040180UL)
#define GPIOH_BASE              (0x400401C0UL)
#define GPIOI_BASE              (0x40040200UL)
#define GPIOJ_BASE              (0x40040240UL)
#define GPIOK_BASE              (0x40040280UL)
#define GPIOL_BASE              (0x400402C0UL)
#define GPIOM_BASE              (0x40040300UL)
#define GPION_BASE              (0x40040340UL)
#define GPIO_PIN_DATA_BASE      (0x40040800UL)
#define PDMA0_BASE              (0x40080000UL)
#define PDMA1_BASE              (0x40090000UL)
#define PDMA2_BASE              (0x400A0000UL)
#define PDMA3_BASE              (0x400B0000UL)
#define EBI_BASE                (0x40100000UL)
#define GMAC0_BASE              (0x40120000UL)
#define GMAC1_BASE              (0x40130000UL)
#define HSUSBH0_BASE            (0x40140000UL)
#define USBH0_BASE              (0x40150000UL)
#define SDH0_BASE               (0x40180000UL)
#define SDH1_BASE               (0x40190000UL)
#define NAND_BASE               (0x401A0000UL)
#define HSUSBH1_BASE            (0x401C0000UL)
#define USBH1_BASE              (0x401D0000UL)
#define HSUSBD_BASE             (0x40200000UL)
#define CCAP0_BASE              (0x40240000UL)
#define CCAP1_BASE              (0x40250000UL)
#define DISP_BASE               (0x40260000UL)
#define DISP_MPU_BASE           (0x40261C40UL)
#define GFX_BASE                (0x40280000UL)
#define VDEC_BASE               (0x40290000UL)
#define HWSEM_BASE              (0x40380000UL)
#define WRHO0_BASE              (0x403A0000UL)
#define WRHO1_BASE              (0x503B0000UL)
#define CANFD0_BASE             (0x403C0000UL)
#define CANFD1_BASE             (0x403D0000UL)
#define CANFD2_BASE             (0x403E0000UL)
#define CANFD3_BASE             (0x403F0000UL)
#define WDT0_BASE               (0x40400000UL)
#define WWDT0_BASE              (0x40400100UL)
#define RTC_BASE                (0x40410000UL)
#define ADC0_BASE               (0x40420000UL)
#define EADC0_BASE              (0x40430000UL)
#define WDT1_BASE               (0x40440000UL)
#define WWDT1_BASE              (0x40440100UL)
#define I2S0_BASE               (0x40480000UL)
#define I2S1_BASE               (0x40490000UL)
#define KPI_BASE                (0x404A0000UL)
#define DDRPHY_BASE             (0x404C0000UL)
#define UMCTL2_BASE             (0x404D0000UL)
#define SSMCC_BASE              (0x404E0000UL)
#define SSPCC_BASE              (0x404F0000UL)
#define TMR01_BASE              (0x40500000UL)
#define TMR23_BASE              (0x40510000UL)
#define TMR45_BASE              (0x40520000UL)
#define TMR67_BASE              (0x40530000UL)
#define TMR89_BASE              (0x40540000UL)
#define TMR1011_BASE            (0x40550000UL)
#define TIMER0_BASE             (0x40500000UL)
#define TIMER1_BASE             (0x40500100UL)
#define TIMER2_BASE             (0x40510000UL)
#define TIMER3_BASE             (0x40510100UL)
#define TIMER4_BASE             (0x40520000UL)
#define TIMER5_BASE             (0x40520100UL)
#define TIMER6_BASE             (0x40530000UL)
#define TIMER7_BASE             (0x40530100UL)
#define TIMER8_BASE             (0x40540000UL)
#define TIMER9_BASE             (0x40540100UL)
#define TIMER10_BASE            (0x40550000UL)
#define TIMER11_BASE            (0x40550100UL)
#define EPWM0_BASE              (0x40580000UL)
#define EPWM1_BASE              (0x40590000UL)
#define EPWM2_BASE              (0x405A0000UL)
#define SPI0_BASE               (0x40600000UL)
#define SPI1_BASE               (0x40610000UL)
#define SPI2_BASE               (0x40620000UL)
#define SPI3_BASE               (0x40630000UL)
#define QSPI0_BASE              (0x40680000UL)
#define QSPI1_BASE              (0x40690000UL)
#define UART0_BASE              (0x40700000UL)
#define UART1_BASE              (0x40710000UL)
#define UART2_BASE              (0x40720000UL)
#define UART3_BASE              (0x40730000UL)
#define UART4_BASE              (0x40740000UL)
#define UART5_BASE              (0x40750000UL)
#define UART6_BASE              (0x40760000UL)
#define UART7_BASE              (0x40770000UL)
#define UART8_BASE              (0x40780000UL)
#define UART9_BASE              (0x40790000UL)
#define UART10_BASE             (0x407A0000UL)
#define UART11_BASE             (0x407B0000UL)
#define UART12_BASE             (0x407C0000UL)
#define UART13_BASE             (0x407D0000UL)
#define UART14_BASE             (0x407E0000UL)
#define UART15_BASE             (0x407F0000UL)
#define I2C0_BASE               (0x40800000UL)
#define I2C1_BASE               (0x40810000UL)
#define I2C2_BASE               (0x40820000UL)
#define I2C3_BASE               (0x40830000UL)
#define I2C4_BASE               (0x40840000UL)
#define I2C5_BASE               (0x40850000UL)
#define UART16_BASE             (0x40880000UL)
#define SC0_BASE                (0x40900000UL)
#define SC1_BASE                (0x40910000UL)
#define QEI0_BASE               (0x40B00000UL)
#define QEI1_BASE               (0x40B10000UL)
#define QEI2_BASE               (0x40B20000UL)
#define ECAP0_BASE              (0x40B40000UL)
#define ECAP1_BASE              (0x40B50000UL)
#define ECAP2_BASE              (0x40B60000UL)
#define SYSDBG_BASE             (0x60800000UL)
#define A35DBG_BASE             (0x60C00000UL)
#define RPTDBG_BASE             (0xE0000000UL)
/*@}*/ /* end of group PERIPHERAL_MEM_MAP */

/** @addtogroup PERIPHERAL_DECLARATION Peripheral Pointer
  The Declaration of Peripherals
  @{
 */

#define SYS                     ((SYS_T *)   SYS_BASE)
#define CLK                     ((CLK_T *)   CLK_BASE)
#define PA                      ((GPIO_T *)  GPIOA_BASE)
#define PB                      ((GPIO_T *)  GPIOB_BASE)
#define PC                      ((GPIO_T *)  GPIOC_BASE)
#define PD                      ((GPIO_T *)  GPIOD_BASE)
#define PE                      ((GPIO_T *)  GPIOE_BASE)
#define PF                      ((GPIO_T *)  GPIOF_BASE)
#define PG                      ((GPIO_T *)  GPIOG_BASE)
#define PH                      ((GPIO_T *)  GPIOH_BASE)
#define PI                      ((GPIO_T *)  GPIOI_BASE)
#define PJ                      ((GPIO_T *)  GPIOJ_BASE)
#define PK                      ((GPIO_T *)  GPIOK_BASE)
#define PL                      ((GPIO_T *)  GPIOL_BASE)
#define PM                      ((GPIO_T *)  GPIOM_BASE)
#define PN                      ((GPIO_T *)  GPION_BASE)
#define PDMA0                   ((PDMA_T *)  PDMA0_BASE)
#define PDMA1                   ((PDMA_T *)  PDMA1_BASE)
#define PDMA2                   ((PDMA_T *)  PDMA2_BASE)
#define PDMA3                   ((PDMA_T *)  PDMA3_BASE)
#define EBI                     ((EBI_T *)   EBI_BASE)
#define HWSEM0                  ((HWSEM_T *) HWSEM_BASE)
#define WHC0                    ((WHC_T *)   WRHO0_BASE)
#define WHC1                    ((WHC_T *)   WRHO1_BASE)
#define MCAN0                   ((MCAN_T *)  MCAN0_BASE)
#define MCAN1                   ((MCAN_T *)  MCAN1_BASE)
#define MCAN2                   ((MCAN_T *)  MCAN2_BASE)
#define MCAN3                   ((MCAN_T *)  MCAN3_BASE)
#define RTC                     ((RTC_T *)   RTC_BASE)
#define ADC0                    ((ADC_T *)   ADC0_BASE)
#define EADC0                   ((EADC_T *)  EADC0_BASE)
#define I2S0                    ((I2S_T *)   I2S0_BASE)
#define I2S1                    ((I2S_T *)   I2S1_BASE)
#define KPI                     ((KPI_T *)   KPI_BASE)
#define TIMER0                  ((TIMER_T *) TIMER0_BASE)
#define TIMER1                  ((TIMER_T *) TIMER1_BASE)
#define TIMER2                  ((TIMER_T *) TIMER2_BASE)
#define TIMER3                  ((TIMER_T *) TIMER3_BASE)
#define TIMER4                  ((TIMER_T *) TIMER4_BASE)
#define TIMER5                  ((TIMER_T *) TIMER5_BASE)
#define TIMER6                  ((TIMER_T *) TIMER6_BASE)
#define TIMER7                  ((TIMER_T *) TIMER7_BASE)
#define TIMER8                  ((TIMER_T *) TIMER8_BASE)
#define TIMER9                  ((TIMER_T *) TIMER9_BASE)
#define TIMER10                 ((TIMER_T *) TIMER10_BASE)
#define TIMER11                 ((TIMER_T *) TIMER11_BASE)
#define EPWM0                   ((EPWM_T *)  EPWM0_BASE)
#define EPWM1                   ((EPWM_T *)  EPWM1_BASE)
#define EPWM2                   ((EPWM_T *)  EPWM2_BASE)
#define SPI0                    ((SPI_T *)   SPI0_BASE)
#define SPI1                    ((SPI_T *)   SPI1_BASE)
#define SPI2                    ((SPI_T *)   SPI2_BASE)
#define SPI3                    ((SPI_T *)   SPI3_BASE)
#define QSPI0                   ((QSPI_T *)  QSPI0_BASE)
#define QSPI1                   ((QSPI_T *)  QSPI1_BASE)
#define UART0                   ((UART_T *)  UART0_BASE)
#define UART1                   ((UART_T *)  UART1_BASE)
#define UART2                   ((UART_T *)  UART2_BASE)
#define UART3                   ((UART_T *)  UART3_BASE)
#define UART4                   ((UART_T *)  UART4_BASE)
#define UART5                   ((UART_T *)  UART5_BASE)
#define UART6                   ((UART_T *)  UART6_BASE)
#define UART7                   ((UART_T *)  UART7_BASE)
#define UART8                   ((UART_T *)  UART8_BASE)
#define UART9                   ((UART_T *)  UART9_BASE)
#define UART10                  ((UART_T *)  UART10_BASE)
#define UART11                  ((UART_T *)  UART11_BASE)
#define UART12                  ((UART_T *)  UART12_BASE)
#define UART13                  ((UART_T *)  UART13_BASE)
#define UART14                  ((UART_T *)  UART14_BASE)
#define UART15                  ((UART_T *)  UART15_BASE)
#define UART16                  ((UART_T *)  UART16_BASE)
#define I2C1                    ((I2C_T *)   I2C1_BASE)
#define I2C2                    ((I2C_T *)   I2C2_BASE)
#define I2C3                    ((I2C_T *)   I2C3_BASE)
#define I2C4                    ((I2C_T *)   I2C4_BASE)
#define I2C5                    ((I2C_T *)   I2C5_BASE)
#define SC0                     ((SC_T *)    SC0_BASE)
#define SC1                     ((SC_T *)    SC1_BASE)
#define WDT0                    ((WDT_T *)   WDT0_BASE)
#define WWDT0                   ((WWDT_T *)  WWDT0_BASE)
#define WDT1                    ((WDT_T *)   WDT1_BASE)
#define WWDT1                   ((WWDT_T *)  WWDT1_BASE)
#define QEI0                    ((QEI_T *)   QEI0_BASE)
#define QEI1                    ((QEI_T *)   QEI1_BASE)
#define QEI2                    ((QEI_T *)   QEI2_BASE)
#define ECAP0                   ((ECAP_T *)  ECAP0_BASE)
#define ECAP1                   ((ECAP_T *)  ECAP1_BASE)
#define ECAP2                   ((ECAP_T *)  ECAP2_BASE)
#define CANFD0                  ((CANFD_T*)  CANFD0_BASE)
#define CANFD1                  ((CANFD_T*)  CANFD1_BASE)
#define CANFD2                  ((CANFD_T*)  CANFD2_BASE)
#define CANFD3                  ((CANFD_T*)  CANFD3_BASE)
#define DISP					((DISP_T*)   DISP_BASE)
#define SDH0					((SDH_T*)    SDH0_BASE)
#define SDH1					((SDH_T*)    SDH1_BASE)
#define CCAP0					((CCAP_T*)   CCAP0_BASE)
#define CCAP1					((CCAP_T*)   CCAP1_BASE)
#define TS                      ((TS_T *)    TS_BASE)
#define HSUSBD					((HSUSBD_T*) HSUSBD_BASE)
#define NFI						((NFI_T *)   NAND_BASE)
#define GMAC0                   ((GMAC_T *)  GMAC0_BASE)
#define GMAC1                   ((GMAC_T *)  GMAC1_BASE)
#define SSPCC                   ((SSPCC_T *) SSPCC_BASE)

/*@}*/ /* end of group ERIPHERAL_DECLARATION */


/* --------  Configuration of the Cortex-A9 Processor and Core Peripherals  ------- */
#define __CA_REV        0x0000U    /*!< Core revision r0p0                          */
#define __CORTEX_A          35U    /*!< Cortex-A35 Core                             */
#define __FPU_PRESENT        1U    /* FPU present                                   */
#define __GIC_PRESENT        1U    /* GIC present                                   */
#define __TIM_PRESENT        1U    /* TIM present                                   */
#define __L2C_PRESENT        0U    /* L2C present                                   */

#include "core_ca.h"		/* Cortex-A35 processor and core peripherals	*/
#include "system_ma35d1.h"	/* System include file							*/
#include <stdint.h>

#include "types.h"
#include "vectors.h"
#include "cpu.h"
#include "irq_ctrl.h"
#include "mmio.h"
#include "mmu.h"

extern uint32_t volatile msTicks0, msTicks1;
extern void global_timer_init(void);
extern void sysprintf(char * pcStr,...);
extern char sysgetchar(void);
extern void sysputchar(char ch);
extern char sysgetchar(void);
extern int  sysIsKbHit(void);
/******************************************************************************/
/*                            Register definitions                            */
/******************************************************************************/
#include "sys_reg.h"
#include "clk_reg.h"
#include "uart_reg.h"

#include "whc_reg.h"
#include "hwsem_reg.h"
#include "wdt_reg.h"
#include "wwdt_reg.h"
#include "adc_reg.h"
#include "eadc_reg.h"
#include "sc_reg.h"
#include "gpio_reg.h"
#include "ecap_reg.h"
#include "qei_reg.h"
#include "i2c_reg.h"
#include "i2s_reg.h"
#include "pdma_reg.h"
#include "qspi_reg.h"
#include "spi_reg.h"
#include "rtc_reg.h"
#include "timer_reg.h"
#include "ebi_reg.h"
#include "epwm_reg.h"
#include "kpi_reg.h"
#include "canfd_reg.h"
#include "disp_reg.h"
#include "ccap_reg.h"
#include "ts_reg.h"
#include "hsusbd_reg.h"
#include "gmac_reg.h"
#include "sspcc_reg.h"
#include "nfi_reg.h"
#include "sdh_reg.h"

#define IS_ALIGNED(x,a)         (((x) & ((typeof(x))(a)-1UL)) == 0)

/******************************************************************************/
/*                Legacy Constants                                            */
/******************************************************************************/
/** @addtogroup Legacy_Constants Legacy Constants
  Legacy Constants
  @{
*/

#ifndef NULL
#define NULL           (0)      ///< NULL pointer
#endif

//#define TRUE           (1UL)      ///< Boolean true, define to use in API parameters or return value
//#define FALSE          (0UL)      ///< Boolean false, define to use in API parameters or return value

#define ENABLE         (1UL)      ///< Enable, define to use in API parameters
#define DISABLE        (0UL)      ///< Disable, define to use in API parameters

/* Define one bit mask */
#define BIT0     (0x00000001UL)       ///< Bit 0 mask of an 32 bit integer
#define BIT1     (0x00000002UL)       ///< Bit 1 mask of an 32 bit integer
#define BIT2     (0x00000004UL)       ///< Bit 2 mask of an 32 bit integer
#define BIT3     (0x00000008UL)       ///< Bit 3 mask of an 32 bit integer
#define BIT4     (0x00000010UL)       ///< Bit 4 mask of an 32 bit integer
#define BIT5     (0x00000020UL)       ///< Bit 5 mask of an 32 bit integer
#define BIT6     (0x00000040UL)       ///< Bit 6 mask of an 32 bit integer
#define BIT7     (0x00000080UL)       ///< Bit 7 mask of an 32 bit integer
#define BIT8     (0x00000100UL)       ///< Bit 8 mask of an 32 bit integer
#define BIT9     (0x00000200UL)       ///< Bit 9 mask of an 32 bit integer
#define BIT10    (0x00000400UL)       ///< Bit 10 mask of an 32 bit integer
#define BIT11    (0x00000800UL)       ///< Bit 11 mask of an 32 bit integer
#define BIT12    (0x00001000UL)       ///< Bit 12 mask of an 32 bit integer
#define BIT13    (0x00002000UL)       ///< Bit 13 mask of an 32 bit integer
#define BIT14    (0x00004000UL)       ///< Bit 14 mask of an 32 bit integer
#define BIT15    (0x00008000UL)       ///< Bit 15 mask of an 32 bit integer
#define BIT16    (0x00010000UL)       ///< Bit 16 mask of an 32 bit integer
#define BIT17    (0x00020000UL)       ///< Bit 17 mask of an 32 bit integer
#define BIT18    (0x00040000UL)       ///< Bit 18 mask of an 32 bit integer
#define BIT19    (0x00080000UL)       ///< Bit 19 mask of an 32 bit integer
#define BIT20    (0x00100000UL)       ///< Bit 20 mask of an 32 bit integer
#define BIT21    (0x00200000UL)       ///< Bit 21 mask of an 32 bit integer
#define BIT22    (0x00400000UL)       ///< Bit 22 mask of an 32 bit integer
#define BIT23    (0x00800000UL)       ///< Bit 23 mask of an 32 bit integer
#define BIT24    (0x01000000UL)       ///< Bit 24 mask of an 32 bit integer
#define BIT25    (0x02000000UL)       ///< Bit 25 mask of an 32 bit integer
#define BIT26    (0x04000000UL)       ///< Bit 26 mask of an 32 bit integer
#define BIT27    (0x08000000UL)       ///< Bit 27 mask of an 32 bit integer
#define BIT28    (0x10000000UL)       ///< Bit 28 mask of an 32 bit integer
#define BIT29    (0x20000000UL)       ///< Bit 29 mask of an 32 bit integer
#define BIT30    (0x40000000UL)       ///< Bit 30 mask of an 32 bit integer
#define BIT31    (0x80000000UL)       ///< Bit 31 mask of an 32 bit integer

/* Byte Mask Definitions */
#define BYTE0_Msk              (0x000000FFUL)         ///< Mask to get bit0~bit7 from a 32 bit integer
#define BYTE1_Msk              (0x0000FF00UL)         ///< Mask to get bit8~bit15 from a 32 bit integer
#define BYTE2_Msk              (0x00FF0000UL)         ///< Mask to get bit16~bit23 from a 32 bit integer
#define BYTE3_Msk              (0xFF000000UL)         ///< Mask to get bit24~bit31 from a 32 bit integer

#define GET_BYTE0(u32Param)    (((u32Param) & BYTE0_Msk)      )  /*!< Extract Byte 0 (Bit  0~ 7) from parameter u32Param */
#define GET_BYTE1(u32Param)    (((u32Param) & BYTE1_Msk) >>  8)  /*!< Extract Byte 1 (Bit  8~15) from parameter u32Param */
#define GET_BYTE2(u32Param)    (((u32Param) & BYTE2_Msk) >> 16)  /*!< Extract Byte 2 (Bit 16~23) from parameter u32Param */
#define GET_BYTE3(u32Param)    (((u32Param) & BYTE3_Msk) >> 24)  /*!< Extract Byte 3 (Bit 24~31) from parameter u32Param */

/*@}*/ /* end of group Legacy_Constants */

/******************************************************************************/
/*                         Peripheral header files                            */
/******************************************************************************/
#include "sys.h"
#include "clk.h"
#include "uart.h"
#include "hwsem.h"
#include "whc.h"
#include "gpio.h"
#include "ecap.h"
#include "qei.h"
#include "timer.h"
#include "timer_pwm.h"
#include "pdma.h"
#include "i2c.h"
#include "i2s.h"
#include "epwm.h"
#include "eadc.h"
#include "adc.h"
#include "wdt.h"
#include "wwdt.h"
#include "ebi.h"
#include "scuart.h"
#include "sc.h"
#include "spi.h"
#include "qspi.h"
#include "rtc.h"
#include "kpi.h"
#include "canfd.h"
#include "ssmcc.h"
#include "sspcc.h"
#include "disp.h"
#include "ccap.h"
#include "ts.h"
#include "hsusbd.h"
#include "gmac.h"
#include "sdh.h"
#include "nfi.h"


#ifdef __cplusplus
}
#endif




#endif  // __MA35D1_H__

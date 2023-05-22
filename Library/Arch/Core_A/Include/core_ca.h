/**************************************************************************//**
 * @file     core_ca.h
 * @brief    CMSIS Cortex-A Core Peripheral Access Layer Header File
 * @version  V1.0.2
 * @date     12. November 2018
 ******************************************************************************/
/*
 * Copyright (c) 2009-2018 ARM Limited. All rights reservefd.
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

#include <stdio.h>
#include <stdlib.h>
#include "lib_helpers.h"


#if   defined ( __ICCARM__ )
  #pragma system_include         /* treat file as system include file for MISRA check */
#elif defined (__clang__)
  #pragma clang system_header   /* treat file as system include file */
#endif

#ifndef __CORE_CA_H_GENERIC
#define __CORE_CA_H_GENERIC

#ifdef __cplusplus
 extern "C" {
#endif

/*******************************************************************************
 *                 CMSIS definitions
 ******************************************************************************/

/*  CMSIS CA definitions */
#define __CA_CMSIS_VERSION_MAIN  (1U)                                      /*!< \brief [31:16] CMSIS-Core(A) main version   */
#define __CA_CMSIS_VERSION_SUB   (1U)                                      /*!< \brief [15:0]  CMSIS-Core(A) sub version    */
#define __CA_CMSIS_VERSION       ((__CA_CMSIS_VERSION_MAIN << 16U) | \
                                   __CA_CMSIS_VERSION_SUB          )       /*!< \brief CMSIS-Core(A) version number         */

#if defined ( __CC_ARM )
  #if defined __TARGET_FPU_VFP
    #if (__FPU_PRESENT == 1)
      #define __FPU_USED       1U
    #else
      #warning "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
      #define __FPU_USED       0U
    #endif
  #else
    #define __FPU_USED         0U
  #endif

#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #if defined __ARM_FP
    #if defined (__FPU_PRESENT) && (__FPU_PRESENT == 1U)
      #define __FPU_USED       1U
    #else
      #warning "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
      #define __FPU_USED       0U
    #endif
  #else
    #define __FPU_USED         0U
  #endif

#elif defined ( __ICCARM__ )
  #if defined __ARMVFP__
    #if (__FPU_PRESENT == 1)
      #define __FPU_USED       1U
    #else
      #warning "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
      #define __FPU_USED       0U
    #endif
  #else
    #define __FPU_USED         0U
  #endif

#elif defined ( __TMS470__ )
  #if defined __TI_VFP_SUPPORT__
    #if (__FPU_PRESENT == 1)
      #define __FPU_USED       1U
    #else
      #warning "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
      #define __FPU_USED       0U
    #endif
  #else
    #define __FPU_USED         0U
  #endif

#elif defined ( __GNUC__ )
  #if defined (__VFP_FP__) && !defined(__SOFTFP__)
    #if (__FPU_PRESENT == 1)
      #define __FPU_USED       1U
    #else
      #warning "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
      #define __FPU_USED       0U
    #endif
  #else
    #define __FPU_USED         0U
  #endif

#elif defined ( __TASKING__ )
  #if defined __FPU_VFP__
    #if (__FPU_PRESENT == 1)
      #define __FPU_USED       1U
    #else
      #error "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
      #define __FPU_USED       0U
    #endif
  #else
    #define __FPU_USED         0U
  #endif
#endif

#include "cmsis_compiler.h"               /* CMSIS compiler specific defines */

#ifdef __cplusplus
}
#endif

#endif /* __CORE_CA_H_GENERIC */

#ifndef __CMSIS_GENERIC

#ifndef __CORE_CA_H_DEPENDANT
#define __CORE_CA_H_DEPENDANT

#ifdef __cplusplus
 extern "C" {
#endif

 /* check device defines and use defaults */
#if defined __CHECK_DEVICE_DEFINES
  #ifndef __CA_REV
    #define __CA_REV              0x0000U
    #warning "__CA_REV not defined in device header file; using default!"
  #endif
  
  #ifndef __FPU_PRESENT
    #define __FPU_PRESENT             0U
    #warning "__FPU_PRESENT not defined in device header file; using default!"
  #endif
    
  #ifndef __GIC_PRESENT
    #define __GIC_PRESENT             1U
    #warning "__GIC_PRESENT not defined in device header file; using default!"
  #endif
  
  #ifndef __TIM_PRESENT
    #define __TIM_PRESENT             1U
    #warning "__TIM_PRESENT not defined in device header file; using default!"
  #endif
  
  #ifndef __L2C_PRESENT
    #define __L2C_PRESENT             0U
    #warning "__L2C_PRESENT not defined in device header file; using default!"
  #endif
#endif

/* IO definitions (access restrictions to peripheral registers) */
#ifdef __cplusplus
  #define   __I     volatile             /*!< \brief Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< \brief Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< \brief Defines 'write only' permissions */
#define     __IO    volatile             /*!< \brief Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /*!< \brief Defines 'read only' structure member permissions */
#define     __OM     volatile            /*!< \brief Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*!< \brief Defines 'read / write' structure member permissions */
#define RESERVED(N, T) T RESERVED##N;    // placeholder struct members used for "reserved" areas


 enum {
 	ENABLE_GRP0 = 0x1 << 0,
 	ENABLE_GRP1 = 0x1 << 1,
 	FIQ_BYP_DIS_GRP0 = 0x1 << 5,
 	IRQ_BYP_DIS_GRP0 = 0x1 << 6,
 	FIQ_BYP_DIS_GRP1 = 0x1 << 7,
 	IRQ_BYP_DIS_GRP1 = 0x1 << 8,
 };


/** \brief  Structure type to access the Generic Interrupt Controller Distributor (GICD)
*/
typedef struct
{
  __IOM uint32_t CTLR;                 /*!< \brief  Offset: 0x000 (R/W) Distributor Control Register */
  __IM  uint32_t TYPER;                /*!< \brief  Offset: 0x004 (R/ ) Interrupt Controller Type Register */
  __IM  uint32_t IIDR;                 /*!< \brief  Offset: 0x008 (R/ ) Distributor Implementer Identification Register */
        RESERVED(0, uint32_t)
  __IOM uint32_t STATUSR;              /*!< \brief  Offset: 0x010 (R/W) Error Reporting Status Register, optional */
        RESERVED(1[11], uint32_t)
  __OM  uint32_t SETSPI_NSR;           /*!< \brief  Offset: 0x040 ( /W) Set SPI Register */
        RESERVED(2, uint32_t)
  __OM  uint32_t CLRSPI_NSR;           /*!< \brief  Offset: 0x048 ( /W) Clear SPI Register */
        RESERVED(3, uint32_t)
  __OM  uint32_t SETSPI_SR;            /*!< \brief  Offset: 0x050 ( /W) Set SPI, Secure Register */
        RESERVED(4, uint32_t)
  __OM  uint32_t CLRSPI_SR;            /*!< \brief  Offset: 0x058 ( /W) Clear SPI, Secure Register */
        RESERVED(5[9], uint32_t)
  __IOM uint32_t IGROUPR[32];          /*!< \brief  Offset: 0x080 (R/W) Interrupt Group Registers */
  __IOM uint32_t ISENABLER[32];        /*!< \brief  Offset: 0x100 (R/W) Interrupt Set-Enable Registers */
  __IOM uint32_t ICENABLER[32];        /*!< \brief  Offset: 0x180 (R/W) Interrupt Clear-Enable Registers */
  __IOM uint32_t ISPENDR[32];          /*!< \brief  Offset: 0x200 (R/W) Interrupt Set-Pending Registers */
  __IOM uint32_t ICPENDR[32];          /*!< \brief  Offset: 0x280 (R/W) Interrupt Clear-Pending Registers */
  __IOM uint32_t ISACTIVER[32];        /*!< \brief  Offset: 0x300 (R/W) Interrupt Set-Active Registers */
  __IOM uint32_t ICACTIVER[32];        /*!< \brief  Offset: 0x380 (R/W) Interrupt Clear-Active Registers */
  __IOM uint32_t IPRIORITYR[255];      /*!< \brief  Offset: 0x400 (R/W) Interrupt Priority Registers */
        RESERVED(6, uint32_t)
  __IOM uint32_t  ITARGETSR[255];      /*!< \brief  Offset: 0x800 (R/W) Interrupt Targets Registers */
        RESERVED(7, uint32_t)
  __IOM uint32_t ICFGR[64];            /*!< \brief  Offset: 0xC00 (R/W) Interrupt Configuration Registers */
  __IOM uint32_t IGRPMODR[32];         /*!< \brief  Offset: 0xD00 (R/W) Interrupt Group Modifier Registers */
        RESERVED(8[32], uint32_t)
  __IOM uint32_t NSACR[64];            /*!< \brief  Offset: 0xE00 (R/W) Non-secure Access Control Registers */
  __OM  uint32_t SGIR;                 /*!< \brief  Offset: 0xF00 ( /W) Software Generated Interrupt Register */
        RESERVED(9[3], uint32_t)
  __IOM uint32_t CPENDSGIR[4];         /*!< \brief  Offset: 0xF10 (R/W) SGI Clear-Pending Registers */
  __IOM uint32_t SPENDSGIR[4];         /*!< \brief  Offset: 0xF20 (R/W) SGI Set-Pending Registers */
        RESERVED(10[5236], uint32_t)
  __IOM uint64_t IROUTER[988];         /*!< \brief  Offset: 0x6100(R/W) Interrupt Routing Registers */
}  GICDistributor_Type;

#define GICDistributor      ((GICDistributor_Type      *)     GIC_DISTRIBUTOR_BASE ) /*!< \brief GIC Distributor register set access pointer */

/** \brief  Structure type to access the Generic Interrupt Controller Interface (GICC)
*/
typedef struct
{
  __IOM uint32_t CTLR;                 /*!< \brief  Offset: 0x000 (R/W) CPU Interface Control Register */
  __IOM uint32_t PMR;                  /*!< \brief  Offset: 0x004 (R/W) Interrupt Priority Mask Register */
  __IOM uint32_t BPR;                  /*!< \brief  Offset: 0x008 (R/W) Binary Point Register */
  __IM  uint32_t IAR;                  /*!< \brief  Offset: 0x00C (R/ ) Interrupt Acknowledge Register */
  __OM  uint32_t EOIR;                 /*!< \brief  Offset: 0x010 ( /W) End Of Interrupt Register */
  __IM  uint32_t RPR;                  /*!< \brief  Offset: 0x014 (R/ ) Running Priority Register */
  __IM  uint32_t HPPIR;                /*!< \brief  Offset: 0x018 (R/ ) Highest Priority Pending Interrupt Register */
  __IOM uint32_t ABPR;                 /*!< \brief  Offset: 0x01C (R/W) Aliased Binary Point Register */
  __IM  uint32_t AIAR;                 /*!< \brief  Offset: 0x020 (R/ ) Aliased Interrupt Acknowledge Register */
  __OM  uint32_t AEOIR;                /*!< \brief  Offset: 0x024 ( /W) Aliased End Of Interrupt Register */
  __IM  uint32_t AHPPIR;               /*!< \brief  Offset: 0x028 (R/ ) Aliased Highest Priority Pending Interrupt Register */
  __IOM uint32_t STATUSR;              /*!< \brief  Offset: 0x02C (R/W) Error Reporting Status Register, optional */
        RESERVED(1[40], uint32_t)
  __IOM uint32_t APR[4];               /*!< \brief  Offset: 0x0D0 (R/W) Active Priority Register */
  __IOM uint32_t NSAPR[4];             /*!< \brief  Offset: 0x0E0 (R/W) Non-secure Active Priority Register */
        RESERVED(2[3], uint32_t)
  __IM  uint32_t IIDR;                 /*!< \brief  Offset: 0x0FC (R/ ) CPU Interface Identification Register */
        RESERVED(3[960], uint32_t)
  __OM  uint32_t DIR;                  /*!< \brief  Offset: 0x1000( /W) Deactivate Interrupt Register */
}  GICInterface_Type;

#define GICInterface        ((GICInterface_Type        *)     GIC_INTERFACE_BASE )   /*!< \brief GIC Interface register set access pointer */


 /*******************************************************************************
  *                Hardware Abstraction Layer
   Core Function Interface contains:
   - L1 Cache Functions
   - L2C-310 Cache Controller Functions 
   - PL1 Timer Functions
   - GIC Functions
   - MMU Functions
  ******************************************************************************/
 
/* ##########################  L1 Cache functions  ################################# */
#include "cache.h"

/* ##########################  GIC functions  ###################################### */
  
/** \brief  Enable the interrupt distributor using the GIC's CTLR register.
 * \param [in] enable  Enable itnerrupt as bellow :
 *					ENABLE_GRP0: Enable Group 0 interrupts
 *                  ENABLE_GRP1: Enable Non-secure Group 1 interrupt
 *          		FIQ_BYP_DIS_GRP0:
 *            		IRQ_BYP_DIS_GRP0:
 *             		FIQ_BYP_DIS_GRP1:
 *             		IRQ_BYP_DIS_GRP1:
*/
__STATIC_INLINE void GIC_EnableDistributor(uint32_t enable)
{
  GICDistributor->CTLR |= enable;
}

/** \brief Disable the interrupt distributor using the GIC's CTLR register.
 * \param [in] enable  Disable itnerrupt as bellow :
 *					ENABLE_GRP0: Disable Group 0 interrupts
 *                  ENABLE_GRP1: Disable Non-secure Group 1 interrupt
 *          		FIQ_BYP_DIS_GRP0:
 *            		IRQ_BYP_DIS_GRP0:
 *             		FIQ_BYP_DIS_GRP1:
 *             		IRQ_BYP_DIS_GRP1:
*/
__STATIC_INLINE void GIC_DisableDistributor(uint32_t disable)
{
  GICDistributor->CTLR &=~disable;
}

/** \brief Read the GIC's TYPER register.
* \return GICDistributor_Type::TYPER
*/
__STATIC_INLINE uint32_t GIC_DistributorInfo(void)
{
  return (GICDistributor->TYPER);
}

/** \brief Reads the GIC's IIDR register.
* \return GICDistributor_Type::IIDR
*/
__STATIC_INLINE uint32_t GIC_DistributorImplementer(void)
{
  return (GICDistributor->IIDR);
}

/** \brief Sets the GIC's ITARGETSR register for the given interrupt.
* \param [in] IRQn Interrupt to be configured.
* \param [in] cpu_target CPU interfaces to assign this interrupt to. as bellow
*               IRQ_CPU_0: CPU0
*               IRQ_CPU_1: CPU1
*/
__STATIC_INLINE void GIC_SetTarget(IRQn_Type IRQn, uint32_t cpu_target)
{
  uint32_t mask = GICDistributor->ITARGETSR[IRQn / 4U] & ~(0xFFUL << ((IRQn % 4U) * 8U));
  GICDistributor->ITARGETSR[IRQn / 4U] = mask | ((cpu_target & 0xFFUL) << ((IRQn % 4U) * 8U));
}

/** \brief Read the GIC's ITARGETSR register.
* \param [in] IRQn Interrupt to acquire the configuration for.
* \return GICDistributor_Type::ITARGETSR
*/
__STATIC_INLINE uint32_t GIC_GetTarget(IRQn_Type IRQn)
{
  return (GICDistributor->ITARGETSR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & 0xFFUL;
}

/** \brief Enable the CPU's interrupt interface.
*/
__STATIC_INLINE void GIC_EnableInterface(void)
{
  GICInterface->CTLR |= 1U; //enable interface
}

/** \brief Disable the CPU's interrupt interface.
*/
__STATIC_INLINE void GIC_DisableInterface(void)
{
  GICInterface->CTLR &=~1U; //disable distributor
}

/** \brief Read the CPU's IAR register.
* \return GICInterface_Type::IAR
*/
__STATIC_INLINE IRQn_Type GIC_AcknowledgePending(void)
{
 if(raw_read_currentel()==0xC)
	 return (IRQn_Type)(GICInterface->IAR);
 else
	 return (IRQn_Type)(GICInterface->AIAR);
}

/** \brief Writes the given interrupt number to the CPU's EOIR register.
* \param [in] IRQn The interrupt to be signaled as finished.
*/
__STATIC_INLINE void GIC_EndInterrupt(IRQn_Type IRQn)
{
	if(raw_read_currentel()==0xC)
		GICInterface->EOIR = IRQn;
	else
		GICInterface->AEOIR = IRQn;
}

/** \brief Enables the given interrupt using GIC's ISENABLER register.
* \param [in] IRQn The interrupt to be enabled.
*/
__STATIC_INLINE void GIC_EnableIRQ(IRQn_Type IRQn)
{
  GICDistributor->ISENABLER[IRQn / 32U] = 1U << (IRQn % 32U);
}

/** \brief Get interrupt enable status using GIC's ISENABLER register.
* \param [in] IRQn The interrupt to be queried.
* \return 0 - interrupt is not enabled, 1 - interrupt is enabled.
*/
__STATIC_INLINE uint32_t GIC_GetEnableIRQ(IRQn_Type IRQn)
{
  return (GICDistributor->ISENABLER[IRQn / 32U] >> (IRQn % 32U)) & 1UL;
}

/** \brief Disables the given interrupt using GIC's ICENABLER register.
* \param [in] IRQn The interrupt to be disabled.
*/
__STATIC_INLINE void GIC_DisableIRQ(IRQn_Type IRQn)
{
  GICDistributor->ICENABLER[IRQn / 32U] = 1U << (IRQn % 32U);
}

/** \brief Get interrupt pending status from GIC's ISPENDR register.
* \param [in] IRQn The interrupt to be queried.
* \return 0 - interrupt is not pending, 1 - interrupt is pendig.
*/
__STATIC_INLINE uint32_t GIC_GetPendingIRQ(IRQn_Type IRQn)
{
  uint32_t pend;

  if (IRQn >= 16U) {
    pend = (GICDistributor->ISPENDR[IRQn / 32U] >> (IRQn % 32U)) & 1UL;
  } else {
    // INTID 0-15 Software Generated Interrupt
    pend = (GICDistributor->SPENDSGIR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & 0xFFUL;
    // No CPU identification offered
    if (pend != 0U) {
      pend = 1U;
    } else {
      pend = 0U;
    }
  }

  return (pend);
}

/** \brief Sets the given interrupt as pending using GIC's ISPENDR register.
* \param [in] IRQn The interrupt to be enabled.
*/
__STATIC_INLINE void GIC_SetPendingIRQ(IRQn_Type IRQn)
{
  if (IRQn >= 16U) {
    GICDistributor->ISPENDR[IRQn / 32U] = 1U << (IRQn % 32U);
  } else {
    // INTID 0-15 Software Generated Interrupt
    GICDistributor->SPENDSGIR[IRQn / 4U] = 1U << ((IRQn % 4U) * 8U);
  }
}

/** \brief Clears the given interrupt from being pending using GIC's ICPENDR register.
* \param [in] IRQn The interrupt to be enabled.
*/
__STATIC_INLINE void GIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  if (IRQn >= 16U) {
    GICDistributor->ICPENDR[IRQn / 32U] = 1U << (IRQn % 32U);
  } else {
    // INTID 0-15 Software Generated Interrupt
    GICDistributor->CPENDSGIR[IRQn / 4U] = 1U << ((IRQn % 4U) * 8U);
  }
}

/** \brief Sets the interrupt configuration using GIC's ICFGR register.
* \param [in] IRQn The interrupt to be configured.
* \param [in] int_config Int_config field value. Bit 0: Reserved (0 - N-N model, 1 - 1-N model for some GIC before v1)
*                                           Bit 1: 0 - level sensitive, 1 - edge triggered
*/
__STATIC_INLINE void GIC_SetConfiguration(IRQn_Type IRQn, uint32_t int_config)
{
  uint32_t icfgr = GICDistributor->ICFGR[IRQn / 16U];
  uint32_t shift = (IRQn % 16U) << 1U;

  icfgr &= (~(3U         << shift));
  icfgr |= (  int_config << shift);

  GICDistributor->ICFGR[IRQn / 16U] = icfgr;
}

/** \brief Get the interrupt configuration from the GIC's ICFGR register.
* \param [in] IRQn Interrupt to acquire the configuration for.
* \return Int_config field value. Bit 0: Reserved (0 - N-N model, 1 - 1-N model for some GIC before v1)
*                                 Bit 1: 0 - level sensitive, 1 - edge triggered
*/
__STATIC_INLINE uint32_t GIC_GetConfiguration(IRQn_Type IRQn)
{
  return (GICDistributor->ICFGR[IRQn / 16U] >> ((IRQn % 16U) >> 1U));
}

/** \brief Set the priority for the given interrupt in the GIC's IPRIORITYR register.
* \param [in] IRQn The interrupt to be configured.
* \param [in] priority The priority for the interrupt, lower values denote higher priorities.
*/
__STATIC_INLINE void GIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  uint32_t mask = GICDistributor->IPRIORITYR[IRQn / 4U] & ~(0xFFUL << ((IRQn % 4U) * 8U));
  GICDistributor->IPRIORITYR[IRQn / 4U] = mask | ((priority & 0xFFUL) << ((IRQn % 4U) * 8U));
}

/** \brief Read the current interrupt priority from GIC's IPRIORITYR register.
* \param [in] IRQn The interrupt to be queried.
*/
__STATIC_INLINE uint32_t GIC_GetPriority(IRQn_Type IRQn)
{
  return (GICDistributor->IPRIORITYR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & 0xFFUL;
}

/** \brief Set the interrupt priority mask using CPU's PMR register.
* \param [in] priority Priority mask to be set.
*/
__STATIC_INLINE void GIC_SetInterfacePriorityMask(uint32_t priority)
{
  GICInterface->PMR = priority & 0xFFUL; //set priority mask
}

/** \brief Read the current interrupt priority mask from CPU's PMR register.
* \result GICInterface_Type::PMR
*/
__STATIC_INLINE uint32_t GIC_GetInterfacePriorityMask(void)
{
  return GICInterface->PMR;
}

/** \brief Configures the group priority and subpriority split point using CPU's BPR register.
* \param [in] binary_point Amount of bits used as subpriority.
*/
__STATIC_INLINE void GIC_SetBinaryPoint(uint32_t binary_point)
{
  GICInterface->BPR = binary_point & 7U; //set binary point
}

/** \brief Read the current group priority and subpriority split point from CPU's BPR register.
* \return GICInterface_Type::BPR
*/
__STATIC_INLINE uint32_t GIC_GetBinaryPoint(void)
{
  return GICInterface->BPR;
}

/** \brief Get the status for a given interrupt.
* \param [in] IRQn The interrupt to get status for.
* \return 0 - not pending/active, 1 - pending, 2 - active, 3 - pending and active
*/
__STATIC_INLINE uint32_t GIC_GetIRQStatus(IRQn_Type IRQn)
{
  uint32_t pending, active;

  active = ((GICDistributor->ISACTIVER[IRQn / 32U])  >> (IRQn % 32U)) & 1UL;
  pending = ((GICDistributor->ISPENDR[IRQn / 32U]) >> (IRQn % 32U)) & 1UL;

  return ((active<<1U) | pending);
}

/** \brief Generate a software interrupt using GIC's SGIR register.
* \param [in] IRQn Software interrupt to be generated.
* \param [in] target_list List of CPUs the software interrupt should be forwarded to.
* \param [in] filter_list Filter to be applied to determine interrupt receivers.
*/
__STATIC_INLINE void GIC_SendSGI(IRQn_Type IRQn, uint32_t target_list, uint32_t filter_list)
{
  GICDistributor->SGIR = ((filter_list & 3U) << 24U) | ((target_list & 0xFFUL) << 16U) | (IRQn & 0x0FUL);
}

/** \brief Get the interrupt number of the highest interrupt pending from CPU's HPPIR register.
* \return GICInterface_Type::HPPIR
*/
__STATIC_INLINE uint32_t GIC_GetHighPendingIRQ(void) 
{ 
  return GICInterface->HPPIR; 
}

/** \brief Provides information about the implementer and revision of the CPU interface.
* \return GICInterface_Type::IIDR
*/
__STATIC_INLINE uint32_t GIC_GetInterfaceId(void)
{ 
  return GICInterface->IIDR; 
}

/** \brief Set the interrupt group from the GIC's IGROUPR register.
* \param [in] IRQn The interrupt to be queried.
* \param [in] group Interrupt group number: 0 - Group 0, 1 - Group 1
*/
__STATIC_INLINE void GIC_SetGroup(IRQn_Type IRQn, uint32_t group)
{
  uint32_t igroupr = GICDistributor->IGROUPR[IRQn / 32U];
  uint32_t shift   = (IRQn % 32U);

  igroupr &= (~(1U          << shift));
  igroupr |= ( (group & 1U) << shift);

  GICDistributor->IGROUPR[IRQn / 32U] = igroupr;
}
#define GIC_SetSecurity         GIC_SetGroup

/** \brief Get the interrupt group from the GIC's IGROUPR register.
* \param [in] IRQn The interrupt to be queried.
* \return 0 - Group 0, 1 - Group 1
*/
__STATIC_INLINE uint32_t GIC_GetGroup(IRQn_Type IRQn)
{
  return (GICDistributor->IGROUPR[IRQn / 32U] >> (IRQn % 32U)) & 1UL;
}
#define GIC_GetSecurity         GIC_GetGroup

/** \brief Initialize the interrupt distributor.
*/
__STATIC_INLINE void GIC_DistInit(void)
{
  uint32_t i;
  uint32_t num_irq = 0U;
  uint32_t priority_field;

  //A reset sets all bits in the IGROUPRs corresponding to the SPIs to 0,
  //configuring all of the interrupts as Secure.

  //Disable interrupt forwarding
  GIC_DisableDistributor(ENABLE_GRP0|ENABLE_GRP1);
  //Get the maximum number of interrupts that the GIC supports
  num_irq = 32U * ((GIC_DistributorInfo() & 0x1FU) + 1U);

  /* Priority level is implementation defined.
   To determine the number of priority bits implemented write 0xFF to an IPRIORITYR
   priority field and read back the value stored.*/
  GIC_SetPriority((IRQn_Type)0U, 0xFFU);
  priority_field = GIC_GetPriority((IRQn_Type)0U);

  for (i = 32U; i < num_irq; i++)
  {
      //Disable the SPI interrupt
      GIC_DisableIRQ((IRQn_Type)i);
      //Set level-sensitive (and N-N model)
      GIC_SetConfiguration((IRQn_Type)i, 0U);
      //Set priority
      GIC_SetPriority((IRQn_Type)i, priority_field/2U);
      //Set target
      GIC_SetTarget((IRQn_Type)i, 3U);
  }
  //Enable distributor
  GIC_EnableDistributor(ENABLE_GRP0|ENABLE_GRP1);
}

/** \brief Initialize the CPU's interrupt interface
*/
__STATIC_INLINE void GIC_CPUInterfaceInit(void)
{
  uint32_t i;
  uint32_t priority_field;

  //A reset sets all bits in the IGROUPRs corresponding to the SPIs to 0,
  //configuring all of the interrupts as Secure.

  //Disable interrupt forwarding
  GIC_DisableInterface();

  /* Priority level is implementation defined.
   To determine the number of priority bits implemented write 0xFF to an IPRIORITYR
   priority field and read back the value stored.*/
  GIC_SetPriority((IRQn_Type)0U, 0xFFU);
  priority_field = GIC_GetPriority((IRQn_Type)0U);

  //SGI and PPI
  for (i = 0U; i < 32U; i++)
  {
    if(i > 15U) {
      //Set level-sensitive (and N-N model) for PPI
      GIC_SetConfiguration((IRQn_Type)i, 0U);
    }
    //Disable SGI and PPI interrupts
    GIC_DisableIRQ((IRQn_Type)i);
    //Set priority
    GIC_SetPriority((IRQn_Type)i, priority_field/2U);
  }
  //Enable interface
  GIC_EnableInterface();
  //Set binary point to 0
  GIC_SetBinaryPoint(0U);
  //Set priority mask
  GIC_SetInterfacePriorityMask(0xFFU);

  GICInterface->CTLR |= (1 << 1); //schung, enable group 1
  //GICInterface->CTLR |= (1 << 3); //schung, Set group 0 to FIQ

}

/** \brief Initialize and enable the GIC
*/
__STATIC_INLINE void GIC_Enable(void)
{
  GIC_DistInit();
  GIC_CPUInterfaceInit(); //per CPU
}

/* ##########################  Generic Timer functions  ############################ */

/** \brief Physical Timer Control register */
typedef union
{
  struct
  {
    uint32_t ENABLE:1;      /*!< \brief bit: 0      Enables the timer. */
    uint32_t IMASK:1;       /*!< \brief bit: 1      Timer output signal mask bit. */
    uint32_t ISTATUS:1;     /*!< \brief bit: 2      The status of the timer. */
    RESERVED(0:29, uint32_t)
  } b;                      /*!< \brief Structure used for bit  access */
  uint32_t w;               /*!< \brief Type      used for word access */
} CNTP_CTL_Type;

/** \brief Configures the frequency the timer shall run at.
* \param [in] value The timer frequency in Hz.
*/
__STATIC_INLINE void EL0_SetCounterFrequency(uint32_t value)
{
	raw_write_cntfrq_el0(value);
	__ISB();
}

/** \brief Sets the reset value of the timer.
* \param [in] value The value the timer is loaded with.
*/
__STATIC_INLINE void EL0_SetLoadValue(uint32_t value)
{
	raw_write_cntp_tval_el0(value);
	__ISB();
}

/** \brief Get the current counter value.
* \return Current counter value.
*/
__STATIC_INLINE uint32_t EL0_GetCurrentValue(void)
{
	uint32_t result;
	result = raw_read_cntp_tval_el0();
	return result;
}

/** \brief Get the current physical counter value.
* \return Current physical counter value.
*/
__STATIC_INLINE uint64_t EL0_GetCurrentPhysicalValue(void)
{
	uint64_t result;
	result = raw_read_cntpct_el0();
	return result;
}

/** \brief Set the physical compare value.
* \param [in] value New physical timer compare value.
*/
__STATIC_INLINE void EL0_SetPhysicalCompareValue(uint64_t value)
{
	raw_write_cntp_cval_el0(value);
	__ISB();
}

/** \brief Get the physical compare value.
* \return Physical compare value.
*/
__STATIC_INLINE uint64_t EL0_GetPhysicalCompareValue(void)
{
	uint64_t result;
	result = raw_read_cntp_cval_el0();
	return result;
}

/** \brief Configure the timer by setting the control value.
* \param [in] value New timer control value.
*/
__STATIC_INLINE void EL0_SetControl(uint32_t value)
{
	raw_write_cntp_ctl_el0(value);
	__ISB();
}

/** \brief Get the control value.
* \return Control value.
*/
__STATIC_INLINE uint32_t EL0_GetControl(void)
{
	uint32_t result;
	result = raw_read_cntp_ctl_el0();
	return result;
}

/* ##########################  MMU functions  ###################################### */
#include "mmu.h"



#ifdef __cplusplus
}
#endif

#endif /* __CORE_CA_H_DEPENDANT */

#endif /* __CMSIS_GENERIC */

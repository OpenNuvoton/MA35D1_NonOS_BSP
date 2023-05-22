/**************************************************************************//**
 * @file     otp.h
 * @version  V1.00
 * $Revision: 1 $
 * @brief    MA35D1 series OTP driver header file
 *
 * @note
 * Copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __OTP_H__
#define __OTP_H__

#ifdef __cplusplus
extern "C"
{
#endif


/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup OTP_Driver OTP Driver
  @{
*/

/** @addtogroup OTP_EXPORTED_CONSTANTS OTP Exported Constants
  @{
*/


#define OTP     ((volatile OTP_T *)  OTP_BASE)
#define NS_OTP  ((volatile NS_OTP_T *) NS_OTP_BASE)

/*---------------------- One Time Programming Memory Controller -------------------------*/
/**
    @addtogroup OTP One Time Programming Memory Controller(OTP)
    Memory Mapped Structure for OTP Controller
@{ */


typedef struct
{
    __IO uint32_t OTP_CTL;            /* Offset: 0x00  OTP Control Register                                              */
    __IO uint32_t OTP_STS;            /* Offset: 0x04  OTP Status Register                                               */
    __IO uint32_t OTP_ADDR;           /* Offset: 0x08  OTP Address Register                                              */
    __IO uint32_t OTP_DATA;           /* Offset: 0x0C  OTP DATA Register                                                 */
    __I  uint32_t OTP_USMSTS;         /* Offset: 0x10  OTP Unused Spare Memory Status Register                           */

} OTP_T;

typedef struct
{
    __IO uint32_t NS_OTP_CTL;         /* Offset: 0x800  Non-secure OTP Control Register                                  */
    __IO uint32_t NS_OTP_STS;         /* Offset: 0x804  Non-secure OTP Status Register                                   */
    __IO uint32_t NS_OTP_ADDR;        /* Offset: 0x808  Non-secure OTP Address Register                                  */
    __IO uint32_t NS_OTP_DATA;        /* Offset: 0x80C  Non-secure OTP DATA Register                                     */
    __I  uint32_t NS_OTP_USMSTS;      /* Offset: 0x810  Non-secure OTP Unused Spare Memory Status Register               */

} NS_OTP_T;

/**
    @addtogroup OTP_CONST OTP Bit Field Definition
    Constant Definitions for OTP Controller
@{ */

#define OTP_CTL_START_Pos               (0)                                               /*!< OTP_T::CTL: START Position               */
#define OTP_CTL_START_Msk               (0x1ul << OTP_CTL_START_Pos)                      /*!< OTP_T::CTL: START Mask                   */

#define OTP_CTL_CMD_Pos                 (4)                                               /*!< OTP_T::CTL: CMD Position                 */
#define OTP_CTL_CMD_Msk                 (0x7ul << OTP_CTL_CMD_Pos)                        /*!< OTP_T::CTL: CMD Mask                     */

#define OTP_STS_BUSY_Pos                (0)                                               /*!< OTP_T::STS: BUSY Position                */
#define OTP_STS_BUSY_Msk                (0x1ul << OTP_STS_BUSY_Pos)                       /*!< OTP_T::STS: BUSY Mask                    */

#define OTP_STS_PFF_Pos                 (1)                                               /*!< OTP_T::STS: PFF Position                 */
#define OTP_STS_PFF_Msk                 (0x1ul << OTP_STS_PFF_Pos)                        /*!< OTP_T::STS: PFF Mask                     */

#define OTP_STS_ADDRFF_Pos              (2)                                               /*!< OTP_T::STS: ADDRFF Position              */
#define OTP_STS_ADDRFF_Msk              (0x1ul << OTP_STS_ADDRFF_Pos)                     /*!< OTP_T::STS: ADDRFF Mask                  */

#define OTP_STS_FTMFF_Pos               (3)                                               /*!< OTP_T::STS: FTMFF Position               */
#define OTP_STS_FTMFF_Msk               (0x1ul << OTP_STS_FTMFF_Pos)                      /*!< OTP_T::STS: FTMFF Mask                   */

#define OTP_STS_CMDFF_Pos               (4)                                               /*!< OTP_T::STS: CMDFF Position               */
#define OTP_STS_CMDFF_Msk               (0x1ul << OTP_STS_CMDFF_Pos)                      /*!< OTP_T::STS: CMDFF Mask                   */

#define OTP_STS_TFF_Pos                 (7)                                               /*!< OTP_T::STS: TFF Position                 */
#define OTP_STS_TFF_Msk                 (0x1ul << OTP_STS_TFF_Pos)                        /*!< OTP_T::STS: TFF Mask                     */

#define OTP_ADDR_ADDR_Pos               (0)                                               /*!< OTP_T::ADDR: ADDR Position               */
#define OTP_ADDR_ADDR_Msk               (0xFFFul << OTP_ADDR_ADDR_Pos)                    /*!< OTP_T::ADDR: ADDR Mask                   */

#define OTP_DATA_DATA_Pos               (0)                                               /*!< OTP_T::DATA: DATA Position               */
#define OTP_DATA_DATA_Msk               (0xFFFFFFFFul << OTP_DATA_DATA_Pos)               /*!< OTP_T::DATA: DATA Mask                   */

#define OTP_USMSTS_BLK0_Pos             (0)                                               /*!< OTP_T::USMSTS: BLK0 Position             */
#define OTP_USMSTS_BLK0_Msk             (0x3ul << OTP_USMSTS_BLK0_Pos)                    /*!< OTP_T::USMSTS: BLK0 Mask                 */

#define OTP_USMSTS_BLK1_Pos             (2)                                               /*!< OTP_T::USMSTS: BLK1 Position             */
#define OTP_USMSTS_BLK1_Msk             (0x3ul << OTP_USMSTS_BLK1_Pos)                    /*!< OTP_T::USMSTS: BLK1 Mask                 */

#define OTP_USMSTS_BLK2_Pos             (4)                                               /*!< OTP_T::USMSTS: BLK2 Position             */
#define OTP_USMSTS_BLK2_Msk             (0x3ul << OTP_USMSTS_BLK2_Pos)                    /*!< OTP_T::USMSTS: BLK2 Mask                 */

#define OTP_USMSTS_BLK3_Pos             (6)                                               /*!< OTP_T::USMSTS: BLK3 Position             */
#define OTP_USMSTS_BLK3_Msk             (0x3ul << OTP_USMSTS_BLK3_Pos)                    /*!< OTP_T::USMSTS: BLK3 Mask                 */

#define OTP_USMSTS_BLK4_Pos             (8)                                               /*!< OTP_T::USMSTS: BLK4 Position             */
#define OTP_USMSTS_BLK4_Msk             (0x3ul << OTP_USMSTS_BLK4_Pos)                    /*!< OTP_T::USMSTS: BLK4 Mask                 */

#define OTP_USMSTS_BLK5_Pos             (10)                                              /*!< OTP_T::USMSTS: BLK5 Position             */
#define OTP_USMSTS_BLK5_Msk             (0x3ul << OTP_USMSTS_BLK5_Pos)                    /*!< OTP_T::USMSTS: BLK5 Mask                 */

#define OTP_USMSTS_BLK6_Pos             (12)                                              /*!< OTP_T::USMSTS: BLK6 Position             */
#define OTP_USMSTS_BLK6_Msk             (0x3ul << OTP_USMSTS_BLK6_Pos)                    /*!< OTP_T::USMSTS: BLK6 Mask                 */

#define OTP_USMSTS_BLK7_Pos             (14)                                              /*!< OTP_T::USMSTS: BLK7 Position             */
#define OTP_USMSTS_BLK7_Msk             (0x3ul << OTP_USMSTS_BLK7_Pos)                    /*!< OTP_T::USMSTS: BLK7 Mask                 */

#define OTP_USMSTS_BLK8_Pos             (16)                                              /*!< OTP_T::USMSTS: BLK8 Position             */
#define OTP_USMSTS_BLK8_Msk             (0x3ul << OTP_USMSTS_BLK8_Pos)                    /*!< OTP_T::USMSTS: BLK8 Mask                 */

#define OTP_USMSTS_BLK9_Pos             (18)                                              /*!< OTP_T::USMSTS: BLK9 Position             */
#define OTP_USMSTS_BLK9_Msk             (0x3ul << OTP_USMSTS_BLK9_Pos)                    /*!< OTP_T::USMSTS: BLK9 Mask                 */

#define OTP_USMSTS_BLK10_Pos            (20)                                              /*!< OTP_T::USMSTS: BLK10 Position            */
#define OTP_USMSTS_BLK10_Msk            (0x3ul << OTP_USMSTS_BLK10_Pos)                   /*!< OTP_T::USMSTS: BLK10 Mask                */

#define OTP_USMSTS_BLK11_Pos            (22)                                              /*!< OTP_T::USMSTS: BLK11 Position            */
#define OTP_USMSTS_BLK11_Msk            (0x3ul << OTP_USMSTS_BLK11_Pos)                   /*!< OTP_T::USMSTS: BLK11 Mask                */

#define OTP_USMSTS_BLK12_Pos            (24)                                              /*!< OTP_T::USMSTS: BLK12 Position            */
#define OTP_USMSTS_BLK12_Msk            (0x3ul << OTP_USMSTS_BLK12_Pos)                   /*!< OTP_T::USMSTS: BLK12 Mask                */

#define OTP_USMSTS_BLK13_Pos            (26)                                              /*!< OTP_T::USMSTS: BLK13 Position            */
#define OTP_USMSTS_BLK13_Msk            (0x3ul << OTP_USMSTS_BLK13_Pos)                   /*!< OTP_T::USMSTS: BLK13 Mask                */

#define OTP_USMSTS_BLK14_Pos            (28)                                              /*!< OTP_T::USMSTS: BLK14 Position            */
#define OTP_USMSTS_BLK14_Msk            (0x3ul << OTP_USMSTS_BLK14_Pos)                   /*!< OTP_T::USMSTS: BLK14 Mask                */

#define OTP_USMSTS_BLK15_Pos            (30)                                              /*!< OTP_T::USMSTS: BLK15 Position            */
#define OTP_USMSTS_BLK15_Msk            (0x3ul << OTP_USMSTS_BLK15_Pos)                   /*!< OTP_T::USMSTS: BLK15 Mask                */

#define NS_OTP_CTL_START_Pos            (0)                                               /*!< OTP_T::NS_OTP_CTL: START Position        */
#define NS_OTP_CTL_START_Msk            (0x1ul << NS_OTP_CTL_START_Pos)                   /*!< OTP_T::NS_OTP_CTL: START Mask            */

#define NS_OTP_CTL_CMD_Pos              (4)                                               /*!< OTP_T::NS_OTP_CTL: CMD Position          */
#define NS_OTP_CTL_CMD_Msk              (0x7ul << NS_OTP_CTL_CMD_Pos)                     /*!< OTP_T::NS_OTP_CTL: CMD Mask              */

#define NS_OTP_STS_BUSY_Pos             (0)                                               /*!< OTP_T::NS_OTP_STS: BUSY Position         */
#define NS_OTP_STS_BUSY_Msk             (0x1ul << NS_OTP_STS_BUSY_Pos)                    /*!< OTP_T::NS_OTP_STS: BUSY Mask             */

#define NS_OTP_STS_PFF_Pos              (1)                                               /*!< OTP_T::NS_OTP_STS: PFF Position          */
#define NS_OTP_STS_PFF_Msk              (0x1ul << NS_OTP_STS_PFF_Pos)                     /*!< OTP_T::NS_OTP_STS: PFF Mask              */

#define NS_OTP_STS_ADDRFF_Pos           (2)                                               /*!< OTP_T::NS_OTP_STS: ADDRFF Position       */
#define NS_OTP_STS_ADDRFF_Msk           (0x1ul << NS_OTP_STS_ADDRFF_Pos)                  /*!< OTP_T::NS_OTP_STS: ADDRFF Mask           */

#define NS_OTP_STS_FTMFF_Pos            (3)                                               /*!< OTP_T::NS_OTP_STS: FTMFF Position        */
#define NS_OTP_STS_FTMFF_Msk            (0x1ul << NS_OTP_STS_FTMFF_Pos)                   /*!< OTP_T::NS_OTP_STS: FTMFF Mask            */

#define NS_OTP_STS_CMDFF_Pos            (4)                                               /*!< OTP_T::NS_OTP_STS: CMDFF Position        */
#define NS_OTP_STS_CMDFF_Msk            (0x1ul << NS_OTP_STS_CMDFF_Pos)                   /*!< OTP_T::NS_OTP_STS: CMDFF Mask            */

#define NS_OTP_ADDR_ADDR_Pos            (0)                                               /*!< OTP_T::NS_OTP_ADDR: ADDR Position        */
#define NS_OTP_ADDR_ADDR_Msk            (0xFFFul << NS_OTP_ADDR_ADDR_Pos)                 /*!< OTP_T::NS_OTP_ADDR: ADDR Mask            */

#define NS_OTP_DATA_DATA_Pos            (0)                                               /*!< OTP_T::NS_OTP_DATA: DATA Position        */
#define NS_OTP_DATA_DATA_Msk            (0xFFFFFFFFul << NS_OTP_DATA_DATA_Pos)            /*!< OTP_T::NS_OTP_DATA: DATA Mask            */

#define NS_OTP_USMSTS_BLK3_Pos          (6)                                               /*!< OTP_T::NS_OTP_USMSTS: BLK3 Position      */
#define NS_OTP_USMSTS_BLK3_Msk          (0x3ul << NS_OTP_USMSTS_BLK3_Pos)                 /*!< OTP_T::NS_OTP_USMSTS: BLK3 Mask          */

#define NS_OTP_USMSTS_BLK4_Pos          (8)                                               /*!< OTP_T::NS_OTP_USMSTS: BLK4 Position      */
#define NS_OTP_USMSTS_BLK4_Msk          (0x3ul << NS_OTP_USMSTS_BLK4_Pos)                 /*!< OTP_T::NS_OTP_USMSTS: BLK4 Mask          */

#define NS_OTP_USMSTS_BLK6_Pos          (12)                                              /*!< OTP_T::NS_OTP_USMSTS: BLK6 Position      */
#define NS_OTP_USMSTS_BLK6_Msk          (0x3ul << NS_OTP_USMSTS_BLK6_Pos)                 /*!< OTP_T::NS_OTP_USMSTS: BLK6 Mask          */

/**@}*/ /* OTP_CONST */
/**@}*/ /* end of OTP register group */

/*---------------------------------------------------------------------------------------------------------*/
/*  OTPCMD constant definitions                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define OTP_CMD_READ          (0x00UL << OTP_CTL_CMD_Pos)        /*!< Command: Read  flash word            \hideinitializer */
#define OTP_CMD_PROGRAM       (0x01UL << OTP_CTL_CMD_Pos)        /*!< Command: Write flash word            \hideinitializer */
#define OTP_CMD_READ_ONLY     (0x02UL << OTP_CTL_CMD_Pos)        /*!< Command: Read Only Lock              \hideinitializer */
#define OTP_CMD_FTM           (0x03UL << OTP_CTL_CMD_Pos)        /*!< Command: Fault Tolerance Mechanism   \hideinitializer */
#define OTP_CMD_READ_CHKER    (0x07UL << OTP_CTL_CMD_Pos)        /*!< Command: Fault Tolerance Mechanism   \hideinitializer */
#define OTP_CMD_READ_CID      (0x0BUL << OTP_CTL_CMD_Pos)        /*!< Command: Fault Tolerance Mechanism   \hideinitializer */
#define OTP_CMD_READ_DID      (0x0CUL << OTP_CTL_CMD_Pos)        /*!< Command: Fault Tolerance Mechanism   \hideinitializer */
#define OTP_CMD_READ_UID      (0x0DUL << OTP_CTL_CMD_Pos)        /*!< Command: Fault Tolerance Mechanism   \hideinitializer */
//#define OTP_CMD_READ_STATUS   (0x0FUL << OTP_CTL_CMD_Pos)        /*!< Command: Read STATUS                  \hideinitializer */

#define ENTRY_MODE_RommapMode     0x0
#define ENTRY_MODE_UserMode       0x1
#define ENTRY_MODE_ReaMA35D1nlyMode   0x2


/*--------------------------------------------------------------------------------------
 *  Block Number | OTP Memory Block      |	OTP Logic Address Range	| Access Source
 *       1	       Poweron Setting      	0x100~0x103	               CPU_S, TWICP, WRITER
 *       2	       DPM Setting	            0x104~0x107	               CPU_S, TWICP, WRITER
 *       	       PLM Setting	            0x108~0x10B	               CPU_S, TWICP, WRITER
 *       3	       MAC0 Address	            0x10C~0x113	               CPU_S, CPU_NS TWICP, WRITER
 *       4	       MAC1 Address	            0x114~0x11B	               CPU_S, CPU_NS TWICP, WRITER
 *       5	       Deployed Password        0x11C~0x11F	               CPU_S, TWICP, WRITER
 *       6	       Secure Region	        0x120~0x177	               CPU_S, TWICP, WRITER
 *       7	       Non-secure Region	    0x178~0x1CF	               CPU_S, CPU_NS TWICP, WRITER 
 */

//BLK 0
#define BLK0_ChipConig_Start     0x0
#define BLK0_ChipConig_End       0x23

#define OTP_ErrChipConig_Start   0x24
#define OTP_ErrChipConig_End     0x28

//BLK 1
#define BLK1_PWRON_SettingAddr_Start  0x100
#define BLK1_PWRON_SettingAddr_End    0x103

//BLK 2
#define BLK2_DPMSettingAddr_Start     0x104
//#define BLK2_DPMSettingAddr_End       0x107

//#define BLK2_PLMSettingAddr_Start     0x108
//#define BLK2_PLMSettingAddr_End       0x10B
#define BLK2_DPMSettingAddr_End       0x10B

//BLK 3
#define BLK3_MAC0Addr_Start           0x10C
#define BLK3_MAC0Addr_End             0x113

//BLK 4
#define BLK4_MAC1Addr_Start           0x114
#define BLK4_MAC1Addr_End             0x11B

//BLK 5
#define BLK5_Deployed_PWD_Start       0x11C
#define BLK5_Deployed_PWD_End         0x11F

//BLK 6
#define BLK6_SecureRegionAddr_Start   0x120
#define BLK6_SecureRegionAddr_End     0x177

//BLK 7
#define BLK7_NSRegionAddr_Start       0x178
#define BLK7_NSRegionAddr_End         0x1CF

// OTP Memory end , Final BLK
#define FINALBLK_Addr_Start           0x1D0
#define FINALBLK_Addr_End             0x438

#define CHIP_CONFIG        0x00
#define PWRON_CONFIG       0x01
#define DPM_PLM_SETTING    0x02
#define MAC0_ADDR          0x03
#define MAC1_ADDR          0x04
#define DEPLOYED_PWD       0x05
#define SECURE_REGION      0x06
#define NON_SECURE_REGION  0x07
#define OTP_OP_REG_MAX     0x08

#define OTP_STS_NAME_BIT0    "BUSY"
#define OTP_STS_NAME_BIT1    "PFF"
#define OTP_STS_NAME_BIT2    "ADDRFF"
#define OTP_STS_NAME_BIT3    "FTMFF"
#define OTP_STS_NAME_BIT4    "CMDFF"
#define OTP_STS_IDX_BIT0     0x00
#define OTP_STS_IDX_BIT1     0x01
#define OTP_STS_IDX_BIT2     0x02
#define OTP_STS_IDX_BIT3     0x03
#define OTP_STS_IDX_BIT4     0x04
#define OTP_STS_IDX_LMT      0x05


int OTP_Program(uint32_t u32Addr, uint32_t u32Data);
int OTP_Read(uint32_t u32Addr, uint32_t *u32Data);
int OTP_Read_Lock(uint32_t u32Addr, uint32_t u32Password);
int OTP_Erase(uint32_t u32BlkAddr, uint32_t u32Password);
int OTP_Read_Checker(uint32_t *u32Data);
int OTP_Read_CID(uint32_t *u32Data);
int OTP_Read_DID(uint32_t *u32Data);
int OTP_Read_UID(uint32_t *u32Data);



/*@}*/ /* end of group OTP_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group OTP_Driver */

/*@}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif //__OTP_H__

/*** (C) COPYRIGHT 2019 Nuvoton Technology Corp. ***/

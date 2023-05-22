/**************************************************************************//**
 * @file     ssmcc.h
 * @version  V1.00
 * @brief    MA35D1 series ssmcc driver header file
 *
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __SSMCC_H__
#define __SSMCC_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    __IO uint32_t SCWP;					/*!< [0x0000] Security Configuration Write Protect Register  */

} SSMCC_T;

typedef struct
{
    __I  uint32_t FAIL_ADDRESS_LOW;		/*!< [0x0000] */
    __I  uint32_t FAIL_ADDRESS_HIGH;	/*!< [0x0004] */
    __I  uint32_t FAIL_CONTROL;			/*!< [0x0008] */
    __I  uint32_t FAIL_ID;				/*!< [0x000C] */

} FAIL_STS_T;

typedef struct
{
    __IO uint32_t REGION_BASE_LOW;		/*!< [0x0000] */
    __IO uint32_t REGION_BASE_HIGH;		/*!< [0x0004] */
    __IO uint32_t REGION_TOP_LOW;		/*!< [0x0008] */
    __IO uint32_t REGION_TOP_HIGH;		/*!< [0x000C] */
    __IO uint32_t REGION_ATTRIBUTES;	/*!< [0x0010] */
    __IO uint32_t REGION_ID_ACCESS;		/*!< [0x0014] */
    __I  uint32_t RESERVED0[2];			/*!< [0x0018 ~ 0x1C] */

} REGION_T;

typedef struct
{
    __I  uint32_t BUILD_CONFIG;			/*!< [0x0000] */
    __IO uint32_t ACTION;				/*!< [0x0004] */
    __IO uint32_t GATE_KEEPER;			/*!< [0x0008] */
    __IO uint32_t SPECULATION_CTRL;		/*!< [0x000C] */
    __I  uint32_t INT_STATUS;			/*!< [0x0010] */
    __O  uint32_t INT_CLEAR;			/*!< [0x0014] */
    __I  uint32_t RESERVED0[2];			/*!< [0x0018 ~ 0x1C] */
    FAIL_STS_T    FAIL_STS[4];
    __I  uint32_t RESERVED1[40];		/*!< [0x0060 ~ 0xFC] */

    __I  uint32_t REGION_BASE_LOW_0;	/*!< [0x0100] */
    __I  uint32_t REGION_BASE_HIGH_0;	/*!< [0x0104] */
    __I  uint32_t REGION_TOP_LOW_0;		/*!< [0x0108] */
    __I  uint32_t REGION_TOP_HIGH_0;	/*!< [0x010C] */
    __IO uint32_t REGION_ATTRIBUTES_0;	/*!< [0x0110] */
    __IO uint32_t REGION_ID_ACCESS_0;	/*!< [0x0114] */
    __I  uint32_t RESERVED2[2];			/*!< [0x0118 ~ 0x11C] */

    REGION_T      REGION[8];

} TZC_T;

/**
    @addtogroup SSMCC_CONST SSMCC Bit Field Definition
    Constant Definitions for SSMCC Controller
@{ */


/**@}*/ /* WDT_CONST */
/**@}*/ /* end of WDT register group */
/**@}*/ /* end of REGISTER group */

#define SSMCC_BASE		(0x404E0000UL)
#define TZC0_BASE		(0x404E1000UL)
#define TZC1_BASE		(0x404E2000UL)
#define TZC2_BASE		(0x404E3000UL)


#define SSMCC		((SSMCC_T *) SSMCC_BASE)
#define TZC0		((TZC_T *)   TZC0_BASE)
#define TZC1		((TZC_T *)   TZC1_BASE)
#define TZC2		((TZC_T *)   TZC2_BASE)

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup SSMCC_Driver SSMCC Driver
  @{
*/

/** @addtogroup SSMCC_EXPORTED_CONSTANTS SSMCC Exported Constants
  @{
*/
#define SSMCC_REGION_1      0UL     /*!< region 1                \hideinitializer */
#define SSMCC_REGION_2      1UL     /*!< region 2                \hideinitializer */
#define SSMCC_REGION_3      2UL     /*!< region 3                \hideinitializer */
#define SSMCC_REGION_4      3UL     /*!< region 4                \hideinitializer */
#define SSMCC_REGION_5      4UL     /*!< region 5                \hideinitializer */
#define SSMCC_REGION_6      5UL     /*!< region 6                \hideinitializer */
#define SSMCC_REGION_7      6UL     /*!< region 7                \hideinitializer */
#define SSMCC_REGION_8      7UL     /*!< region 8                \hideinitializer */


#define SSMCC_SECURE_READ        0x40000000UL     /*!< set region to secure read                \hideinitializer */
#define SSMCC_SECURE_WRITE       0x80000000UL     /*!< set region to secure write               \hideinitializer */
#define SSMCC_NONSECURE_READ     0x00000001UL     /*!< set region to non-secure read            \hideinitializer */
#define SSMCC_NONSECURE_WRITE    0x00010000UL     /*!< set region to non-secure write           \hideinitializer */
#define SSMCC_M4NS_READ          0x00000002UL     /*!< set region to m4 non-secure read         \hideinitializer */
#define SSMCC_M4NS_WRITE         0x00020000UL     /*!< set region to m4 non-secure write        \hideinitializer */

typedef struct ssmcc_param
{
	uint32_t region_no;
	uint32_t base_address;
	uint32_t size;
	uint32_t attribute;

}  SSMCC_PARAM_T;



/*@}*/ /* end of group SSMCC_EXPORTED_CONSTANTS */

/** @addtogroup SSMCC_EXPORTED_FUNCTIONS SSMCC Exported Functions
  @{
*/
void SSMCC_SetRegion0(uint32_t attrib);


/*@}*/ /* end of group SSMCC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group SSMCC_Driver */

/*@}*/ /* end of group Standard_Driver */


#ifdef __cplusplus
}
#endif

#endif /* __SSMCC_H__ */

/*** (C) COPYRIGHT 2020 Nuvoton Technology Corp. ***/

/**************************************************************************//**
 * @file     keystore.h
 * @version  V1.00
 * @brief    Key Store Driver Header
 *
 * @note
 * Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __KEYSTORE_H__
#define __KEYSTORE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup KS_Driver Key Store Driver
  @{
*/

/** @addtogroup KEY_STORE_EXPORTED_CONSTANTS Key Store Exported Constants
  @{
*/

#define KS_MAX_KEY_SIZE     (4096) /*!< Maximum Key Store SRAM key size */
#define KS_MAX_OTP_KEY_SIZE (256)  /*!< Maximum Key Store OTP key size */

#define KS_SRAM_KEY_CNT     (32)  /*!< Maximum SRAM key count of Key Store */
#define KS_OTP_KEY_CNT      (9)   /*!< Maximum OTP key count of Key Store */

#define KS_TOMETAKEY(x)     (((x) << KS_METADATA_NUMBER_Pos) & KS_METADATA_NUMBER_Msk) /*!< translate key number to meta data */
#define KS_TOKEYIDX(x)      (((x) & KS_METADATA_NUMBER_Msk) >> KS_METADATA_NUMBER_Pos) /*!< get key number from meta data */

typedef enum KSMEM /*!< Key Store memory type enumerator */
{
	KS_SRAM = 0x0,          /*!< Volatile Memory                                */
	KS_OTP  = 0x2           /*!< One-Time Programming Memory                    */
} KS_MEM_Type;

#define KS_OWNER_AES                (0ul) /*!< Key is used for AES key */
#define KS_OWNER_HMAC               (1ul) /*!< Key is used for HMAC key */
#define KS_OWNER_RSA_EXP            (2ul) /*!< Key is used for RSA exponential */
#define KS_OWNER_RSA_MID            (3ul) /*!< Key is used for RSA middle calculation */
#define KS_OWNER_ECC                (4ul) /*!< Key is used for ECC */
#define KS_OWNER_CPU                (5ul) /*!< Key can be read by CPU */

#define KS_METADATA_OWNER_Pos       (16)                            /*!< METADATA: TYPE Position */
#define KS_META_AES                 (0ul << KS_METADATA_OWNER_Pos)  /*!< AES Access Only     */
#define KS_META_HMAC                (1ul << KS_METADATA_OWNER_Pos)  /*!< HMAC Access Only    */
#define KS_META_RSA_EXP             (2ul << KS_METADATA_OWNER_Pos)  /*!< RSA_EXP Access Only */
#define KS_META_RSA_MID             (3ul << KS_METADATA_OWNER_Pos)  /*!< RSA_MID Access Only */
#define KS_META_ECC                 (4ul << KS_METADATA_OWNER_Pos)  /*!< ECC Access Only     */
#define KS_META_CPU                 (5ul << KS_METADATA_OWNER_Pos)  /*!< CPU Access Only     */

#define KS_METADATA_SIZE_Pos        (8)                             /*!< METADATA: SIZE Position */
#define KS_META_128                 ( 0ul << KS_METADATA_SIZE_Pos)  /*!< Key size 128 bits    */
#define KS_META_163                 ( 1ul << KS_METADATA_SIZE_Pos)  /*!< Key size 163 bits    */
#define KS_META_192                 ( 2ul << KS_METADATA_SIZE_Pos)  /*!< Key size 192 bits    */
#define KS_META_224                 ( 3ul << KS_METADATA_SIZE_Pos)  /*!< Key size 224 bits    */
#define KS_META_233                 ( 4ul << KS_METADATA_SIZE_Pos)  /*!< Key size 233 bits    */
#define KS_META_255                 ( 5ul << KS_METADATA_SIZE_Pos)  /*!< Key size 255 bits    */
#define KS_META_256                 ( 6ul << KS_METADATA_SIZE_Pos)  /*!< Key size 256 bits    */
#define KS_META_283                 ( 7ul << KS_METADATA_SIZE_Pos)  /*!< Key size 283 bits    */
#define KS_META_384                 ( 8ul << KS_METADATA_SIZE_Pos)  /*!< Key size 384 bits    */
#define KS_META_409                 ( 9ul << KS_METADATA_SIZE_Pos)  /*!< Key size 409 bits    */
#define KS_META_512                 (10ul << KS_METADATA_SIZE_Pos)  /*!< Key size 512 bits    */
#define KS_META_521                 (11ul << KS_METADATA_SIZE_Pos)  /*!< Key size 521 bits    */
#define KS_META_571                 (12ul << KS_METADATA_SIZE_Pos)  /*!< Key size 571 bits    */
#define KS_META_1024                (16ul << KS_METADATA_SIZE_Pos)  /*!< Key size 1024 bits   */
#define KS_META_1536                (17ul << KS_METADATA_SIZE_Pos)  /*!< Key size 1024 bits   */
#define KS_META_2048                (18ul << KS_METADATA_SIZE_Pos)  /*!< Key size 2048 bits   */
#define KS_META_3072                (19ul << KS_METADATA_SIZE_Pos)  /*!< Key size 1024 bits   */
#define KS_META_4096                (20ul << KS_METADATA_SIZE_Pos)  /*!< Key size 4096 bits   */

#define KS_METADATA_READABLE_Pos    (2)                     /*!< METADATA: READABLE Position  */
#define KS_META_READABLE            (1ul << KS_METADATA_READABLE_Pos)  /*!< Allow the key to be read by software */

#define KS_METADATA_PRIV_Pos        (1)                             /*!< METADATA: PRIV Position */
#define KS_META_PRIV                (1ul << KS_METADATA_PRIV_Pos)   /*!< Privilege key        */
#define KS_META_NONPRIV             (0ul << KS_METADATA_PRIV_Pos)   /*!< Non-privilege key    */

#define KS_METADATA_SEC_Pos         (0)                             /*!< METADATA: SEC Position */
#define KS_META_SECURE              (1ul << KS_METADATA_SEC_Pos)    /*!< Secure key            */
#define KS_META_NONSECURE           (0ul << KS_METADATA_SEC_Pos)    /*!< Non-secure key        */


/*! @}*/ /* end of group KEY_STORE_EXPORTED_CONSTANTS */

/*! @}*/ /* end of group KS_Driver */

/*! @}*/ /* end of group Standard_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __KEYSTORE_H__ */


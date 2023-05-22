/**************************************************************************//**
 * @file     tsi_cmd.h
 * @brief    MA35D1 TSI driver header file
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#ifndef __TSI_CMD_H__
#define __TSI_CMD_H__

#include "crypto.h"
#include "keystore.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup TSI_Driver TSI Driver
  @{
*/

/** @addtogroup TSI_EXPORTED_CONSTANTS TSI Exported Constants
  @{
*/

/*------------------------------------------------------------------*/
/*  TSI command class code                                          */
/*------------------------------------------------------------------*/
#define C_CODE_TSI_CTRL         0x01   /*!< TSI control command class */
#define C_CODE_TRNG             0x02   /*!< TRNG command class */
#define C_CODE_PRNG             0x03   /*!< PRNG command class */
#define C_CODE_AES              0x04   /*!< AES command class */
#define C_CODE_SHA              0x05   /*!< HMAC/SHA command class */
#define C_CODE_ECC              0x06   /*!< ECC command class */
#define C_CODE_RSA              0x07   /*!< RSA command class */
#define C_CODE_KS               0x08   /*!< Key Store command class */
#define C_CODE_OTP              0x09   /*!< OTP command class */

/*------------------------------------------------------------------*/
/*  TSI commands                                                    */
/*------------------------------------------------------------------*/
#define CMD_TSI_SYNC            0x0101 /*!< Synchronize TSI to initial state */
#define CMD_TSI_GET_VERSION     0x0102 /*!< Get TSI IBR version */
#define CMD_TSI_SET_CLOCK       0x0110 /*!< Set TSI PLL clock. (SYS-PLL) */
#define CMD_TSI_RESET           0x0111 /*!< Force TSI to perform a chip reset immediately */
#define CMD_TSI_CONFIG_UART     0x0130 /*!< Configure TSI UART */
#define CMD_TSI_LOAD_EX_FUNC    0x0140 /*!< Load a patch image to TSI */
#define CMD_TSI_MONITOR_SET     0x0142 /*!< Setup a TSI monitor area */
#define CMD_TSI_MONITOR_STATUS  0x0143 /*!< Get TSI monitor area status */
#define CMD_TSI_MONITOR_CLOSE   0x0144 /*!< Close a TSI monitor process */
#define CMD_TSI_OPEN_SESSION    0x0150 /*!< Open an AES or SHA session */
#define CMD_TSI_CLOSE_SESSION   0x0151 /*!< Close an AES or SHA session */
#define CMD_TRNG_INIT           0x0201 /*!< Initialize TRNG */
#define CMD_TRNG_GEN_RANDOM     0x0205 /*!< Get a set fo random numbers from TRNG */
#define CMD_PRNG_RESEED         0x0301 /*!< Make PRNG re-seed */
#define CMD_PRNG_GEN_RANDOM     0x0305 /*!< Get 64-bits random number from PRNG */
#define CMD_PRNG_GEN_RAN_MASS   0x0315 /*!< Get mass random numbers from PRNG */
#define CMD_PRNG_GEN_KS_SRAM    0x0320 /*!< Generate a random key to Key Store SRAM */
#define CMD_AES_SET_MODE        0x0401 /*!< Configure AES encrypt/decrypt mode */
#define CMD_AES_SET_IV          0x0402 /*!< Set the AES initial vector */
#define CMD_AES_SET_KEY         0x0403 /*!< Set the AES Keys */
#define CMD_AES_RUN             0x0405 /*!< Start AES encrypt/decrypt */
#define CMD_AES_GCM_RUN         0x0406 /*!< Start AES GCM mode encrypt/decrypt */
#define CMD_AES_CCM_RUN         0x0408 /*!< Start AES CCM mode encrypt/decrypt */
#define CMD_AES_ACCESS_FEEDBACK 0x0409 /*!< Read or write AES intermediate feedback data. */
#define CMD_SHA_START           0x0501 /*!< Start to process the first block of a SHA session */
#define CMD_SHA_UPDATE          0x0502 /*!< Update SHA data */
#define CMD_SHA_FINISH          0x0503 /*!< Update the last block of data and get result digest */
#define CMD_SHA_ALL_AT_ONCE     0x0505 /*!< Run SHA all at once */
#define CMD_ECC_GEN_PUB_KEY     0x0601 /*!< Generate an ECC public key */
#define CMD_ECC_GEN_SIG         0x0602 /*!< Generate an ECC signature */
#define CMD_ECC_VERIFY_SIG      0x0603 /*!< Verify if an ECC signature valid or not */
#define CMD_ECC_MULTIPLY        0x0604 /*!< Execute ECC point multiplication */
#define CMD_RSA_EXP_MOD         0x0702 /*!< Execute RSA exponent modulus */
#define CMD_KS_WRITE_SRAM_KEY   0x0A01 /*!< Write key to key store SRAM */
#define CMD_KS_WRITE_OTP_KEY    0x0A02 /*!< Write key to key store OTP */
#define CMD_KS_READ_KEY         0x0A05 /*!< Read key from key store */
#define CMD_KS_REVOKE_KEY       0x0A08 /*!< Revoke a key in key store */
#define CMD_KS_ERASE_KEY        0x0A10 /*!< Erase a key from key store */
#define CMD_KS_ERASE_ALL        0x0A11 /*!< Erase all keys from Key Store SRAM */
#define CMD_KS_REMAIN_SIZE      0x0A15 /*!< Get remain size of Key Store SRAM */
#define CMD_KS_GET_STATUS       0x0A20 /*!< Get status of Key Store */

/*------------------------------------------------------------------*/
/*  TSI extened commands supported by patch image                   */
/*------------------------------------------------------------------*/
#define	CMD_EXT_OTP_READ        0xF910 /*!< Read OTP */
#define	CMD_EXT_RSA_EXP_MOD     0xFE51 /*!< Execute RSA exponent modulus */

/*------------------------------------------------------------------*/
/*  TSI command ACK status                                          */
/*------------------------------------------------------------------*/
#define ST_SUCCESS              0x00   /*!< successful */
#define ST_WAIT_TSI_SYNC        0x01   /*!< Waiting host to send TSI_SYNC command */
#define ST_UNKNOWN_CMD          0x02   /*!< Unknown command */
#define ST_NO_TSI_IMAGE         0x03   /*!< TSI image is required */
#define ST_CMD_QUEUE_FULL       0x04   /*!< TSI command queue is full */
#define ST_TIME_OUT             0x10   /*!< Command request was not completed in time */
#define ST_INVALID_PARAM        0x11   /*!< Invalid parameter */
#define ST_NO_AVAIL_SESSION     0x12   /*!< No available AES/SHA sessions */
#define ST_INVALID_SESSION_ID   0x13   /*!< Not an opened AES/SHA session ID */
#define ST_INVALID_OPERATION    0x14   /*!< Invalid operation */
#define ST_HW_NOT_READY         0x31   /*!< Hrdawre engine in not ready */
#define ST_HW_ERROR             0x32   /*!< Hardware engine report error */
#define ST_HW_BUSY              0x33   /*!< Hardware engine is busy on other request */
#define ST_HW_TIME_OUT          0x34   /*!< Hardware engine time-out */
#define ST_BUS_ERROR            0x35   /*!< Memory address can not access */
#define ST_KS_ERROR             0x40   /*!< Key Store key error */
#define ST_MON_FULL             0x41   /*!< All monitors are used. */
#define ST_MON_ID_ERR           0x42   /*!< Invalid monitor ID */
#define ST_MON_CHECK_FAIL       0x43   /*!< Monitor found memory area modified */
#define ST_IMG_SIZE             0x45   /*!< Patch image size over limitation */
#define ST_IMG_VERIFY           0x46   /*!< Patch image size authentication failed */
#define ST_ECC_UNKNOWN_CURVE    0x61   /*!< Unknown ECC curve */
#define ST_ECC_INVALID_PRIV_KEY 0x62   /*!< ECC private key in not valid */
#define ST_SIG_VERIFY_ERROR     0x70   /*!< ECC/RSA signature verification failed */
#define ST_KS_READ_PROTECT      0x80   /*!< Key Store key entry cannot read by CPU */
#define ST_KS_FULL              0x81   /*!< Key Store full */
#define ST_WHC_TX_BUSY          0xd1   /*!< All TX channel of Wormhole are busy */
#define ST_CMD_ACK_TIME_OUT     0xd2   /*!< TSI MA35D1es not ack command in time limit */

/// @cond HIDDEN_SYMBOLS

/*!< TSI command request */
typedef struct tsi_cmd_t
{
	uint32_t	cmd[4];      /*!< TSI command words */
	uint32_t	ack[4];      /*!< TSI ack status words */
	int         tx_channel;  /*!< WHC TX channel used to send this command */
	uint32_t    tx_jiffy;    /*!< the jiffy of command being send */
	int         session_id;  /*!< session ID; for AES and SHA only */
	uint32_t    addr_param;  /*!< parameter block address */
	uint32_t    caddr_src;   /*!< current data source address */
	uint32_t    caddr_dst;   /*!< current data destination address */
	uint32_t    remain_len;  /*!< remaining data length */
}  TSI_REQ_T;

#define TC_GET_CLASS_CODE(r)	((((r)->cmd[0])>>24)&0xff)
#define TC_GET_SUB_CODE(r) 		((((r)->cmd[0])>>16)&0xff)
#define TC_GET_COMMAND(r)		((((r)->cmd[0])>>16)&0xffff)
#define TC_GET_SESSION_ID(r)	(((r)->cmd[0])&0xff)
#define TC_SET_CLASS_CODE(r,c)	(r)->cmd[0] = (((r)->cmd[0])&0x00ffffff)|((c&0xff)<<24)
#define TC_SET_SUB_CODE(r,c)	(r)->cmd[0] = (((r)->cmd[0])&0xff00ffff)|((c&0xff)<<16)
#define TC_SET_SESSION_ID(r,s)	(r)->cmd[0] = (((r)->cmd[0])&0xffffff00)|(s&0xff)

#define TA_GET_CLASS_CODE(a)	((((a)->ack[0])>>24)&0xff)
#define TA_GET_SESSION_ID(a)	(((a)->ack[0])&0xff)
#define TA_GET_STATUS(a)		((((a)->ack[0])>>8)&0xff)

/* Command/ACK characteristic is composed of class code, sub-code, and session ID */
#define TCK_CHR_MASK            (0xffff00ff)

#define TSI_CMD_WORD0(cc,sc,sid)    ((cc<<24)|(sc<<16)|sid)

#define CMD_TIME_OUT_1S             1000 /*!< general time-out 1 seconds */
#define CMD_TIME_OUT_2S             2000 /*!< general time-out 2 seconds */
#define CMD_TIME_OUT_3S             3000 /*!< general time-out 3 seconds */
#define CMD_TIME_OUT_5S             5000 /*!< general time-out 5 seconds */

#define CRYPTO_PRNG_KSCTL_ECDH_Pos  (19)                                    /*!< CRYPTO_T::PRNG_KSCTL: ECDH Position    */
#define CRYPTO_PRNG_KSCTL_ECDH_Msk  (0x1ul << CRYPTO_PRNG_KSCTL_ECDH_Pos)   /*!< CRYPTO_T::PRNG_KSCTL: ECDH Mask        */
#define CRYPTO_PRNG_KSCTL_ECDSA_Pos (20)                                    /*!< CRYPTO_T::PRNG_KSCTL: ECDSA Position   */
#define CRYPTO_PRNG_KSCTL_ECDSA_Msk (0x1ul << CRYPTO_PRNG_KSCTL_ECDSA_Pos)  /*!< CRYPTO_T::PRNG_KSCTL: ECDSA Mask       */
#define CRYPTO_PRNG_KSCTL_OWNER_Pos (24)                                    /*!< CRYPTO_T::PRNG_KSCTL: OWNER Position   */
#define CRYPTO_PRNG_KSCTL_OWNER_Msk (0x7ul << CRYPTO_PRNG_KSCTL_OWNER_Pos)  /*!< CRYPTO_T::PRNG_KSCTL: OWNER Mask       */
#define KS_METADATA_SIZE_Pos        (8)                                     /*!< KS_T::METADATA: SIZE Position          */
#define KS_METADATA_SIZE_Msk        (0x1ful << KS_METADATA_SIZE_Pos)        /*!< KS_T::METADATA: SIZE Mask              */

/// @endcond HIDDEN_SYMBOLS

#define SEL_KEY_FROM_REG            0x0 /*!< AES/HMAC key from register */
#define SEL_KEY_FROM_KS_SRAM        0x1 /*!< AES/HMAC key from Key Store SRAM */
#define SEL_KEY_FROM_KS_OTP         0x5 /*!< AES/HMAC key from Key Store OTP */

#define ECC_KEY_SEL_TRNG            0x0 /*!< ECC key from TRNG */
#define ECC_KEY_SEL_KS_OTP          0x1 /*!< ECC key from Key Store OTP */
#define ECC_KEY_SEL_KS_SRAM         0x2 /*!< ECC key from Key Store SRAM */
#define ECC_KEY_SEL_USER            0x3 /*!< ECC key from register */

#define RSA_KEY_SEL_KS_OTP          0x1 /*!< RSA key from Key Store OTP */
#define RSA_KEY_SEL_KS_SRAM         0x2 /*!< RSA key from Key Store SRAM */
#define RSA_KEY_SEL_USER            0x3 /*!< RSA key from Key register */

/*-------------------------------------------------------------------------------------------------*/
/*  Functions                                                                                      */
/*-------------------------------------------------------------------------------------------------*/
void TSI_Print_Error(int code);
int TSI_Init(void);
int TSI_Sync(void);
int TSI_Get_Version(uint32_t *ver_code);
int TSI_Reset(void);
int TSI_Config_UART(uint32_t line, uint32_t baud);
int TSI_Set_Clock(uint32_t pllctl);
int TSI_Load_Image(uint32_t base, uint32_t size);
int TSI_Monitor_Set(uint32_t base, uint32_t size, int interval, int *id);
int TSI_Monitor_Status(int id);
int TSI_Monitor_Close(int id);
int TSI_Open_Session(int class_code, int *session_id);
int TSI_Close_Session(int class_code, int session_id);
int TSI_TRNG_Init(int method, uint32_t pb_addr);
int TSI_TRNG_Gen_Random(uint32_t wcnt, uint32_t dest_addr);
int TSI_PRNG_ReSeed(int seed_src, uint32_t seed);
int TSI_PRNG_Gen_Random(uint32_t *rnd_w0, uint32_t *rnd_w1);
int TSI_PRNG_Gen_Random_Mass(uint32_t wcnt, uint32_t dest_addr);
int TSI_PRNG_GenTo_KS_SRAM(uint32_t owner, int is_ecdsa, int is_ecdh, uint32_t keysz, int *key_num);
int TSI_AES_Set_Mode(int sid, int kinswap, int koutswap, int inswap, int outswap, int sm4en,
                     int encrypt, int mode, int keysz, int ks, int ks_num);
int TSI_AES_Set_IV(int sid, uint32_t iv_addr);
int TSI_AES_Set_Key(int sid, int keysz, uint32_t key_addr);
int TSI_AES_Run(int sid, int is_last, int data_cnt, uint32_t src_addr, uint32_t dest_addr);
int TSI_AES_GCM_Run(int sid, int is_last, int data_cnt, uint32_t param_addr);
int TSI_Access_Feedback(int sid, int rw, int wcnt, uint32_t fdbck_addr);
int TSI_SHA_Start(int sid, int inswap, int outswap, int mode_sel, int hmac,
                     int mode, int keylen, int ks, int ks_num);
int TSI_SHA_Update(int sid, int data_cnt, uint32_t src_addr);
int TSI_SHA_Finish(int sid, int wcnt, int data_cnt, uint32_t src_addr, uint32_t dest_addr);
int TSI_SHA_All_At_Once(int inswap, int outswap, int mode_sel, int mode, int wcnt, int data_cnt, uint32_t src_addr, uint32_t dest_addr);
int TSI_ECC_GenPublicKey(E_ECC_CURVE curve_id, int is_ecdh, int psel, int d_knum, uint32_t priv_key, uint32_t pub_key);
int TSI_ECC_GenSignature(E_ECC_CURVE curve_id, int rsel, int psel, int key_idx, uint32_t param_addr, uint32_t sig_addr);
int TSI_ECC_VerifySignature(E_ECC_CURVE curve_id, int psel, int x_knum, int y_knum, uint32_t param_addr);
int TSI_ECC_Multiply(E_ECC_CURVE curve_id, int type, int msel, int sps, int m_knum, int x_knum, int y_knum, uint32_t param_addr, uint32_t dest_addr);
int TSI_RSA_Exp_Mod(int rsa_len, int crt, int esel, int e_knum, uint32_t param_addr, uint32_t dest_addr);

int TSI_KS_Write_SRAM(uint32_t u32Meta, uint32_t au32Key[], int *iKeyNum);
int TSI_KS_Write_OTP(int KeyNum, uint32_t u32Meta, uint32_t au32Key[]);
int TSI_KS_Read(KS_MEM_Type eType, int32_t i32KeyIdx, uint32_t au32Key[], uint32_t u32WordCnt);
int TSI_KS_RevokeKey(KS_MEM_Type eType, int32_t i32KeyIdx);
int TSI_KS_EraseKey(KS_MEM_Type eType, int32_t i32KeyIdx);
int TSI_KS_EraseAll(void);
int TSI_KS_GetRemainSize(int *remain_size);
int TSI_KS_GetStatus(uint32_t *ks_sts, uint32_t *ks_otpsts, uint32_t *ks_metadata);

int TSI_OTP_Read(uint32_t u32Addr, uint32_t *u32Data);

/*! @}*/ /* end of group TSI_EXPORTED_FUNCTIONS */

/*! @}*/ /* end of group TSI_Driver */

/*! @}*/ /* end of group Standard_Driver */

#endif  /* __TSI_CMD_H__ */

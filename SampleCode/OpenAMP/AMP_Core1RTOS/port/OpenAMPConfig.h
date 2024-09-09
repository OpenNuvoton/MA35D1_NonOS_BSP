/*************************************************************************//**
 * @file     OpenAMPConfig.h
 * @version  V1.00
 * @brief    Configurable parameters for IPI & shared memory
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

/* Configurable parameters */
#define SHARED_RSC_TABLE       ( 0x8A000000UL )
#define SHARED_MEM_PA          ( SHARED_RSC_TABLE + SHARED_BUF_OFFSET )
#define RING_TX                ( SHARED_MEM_PA )
#define RING_TX_SIZE           ( 0x8000 )
#define RING_RX                ( RING_TX + RING_TX_SIZE )
#define RING_RX_SIZE           ( 0x8000 )
#define SHARED_MEM_SIZE        ( RING_TX_SIZE + RING_RX_SIZE )
#define NO_NAME_SERVICE        ( 32 ) /* Number of char supported by ns (must be aligned with word) */

#define RXIPI_BASE             ( TIMER9 )
#define RXIPI_IRQ_NUM          (IRQn_ID_t)TMR9_IRQn
#define TXIPI_BASE             ( TIMER8 )
#define TXIPI_IRQ_NUM          (IRQn_ID_t)TMR8_IRQn

/**
 * @brief Check if remote is ready
 * 
 * @return int 1 from ready
 */
int ma35_rpmsg_remote_ready(void);

/**
 * @brief Create tx endpoint
 * 
 * @param ept  rpmsg endpoint
 * @param rdev rpmsg device
 * @param name name of endpoint
 * @param size request tx size
 * @return int actual request tx size 
 */
int ma35_rpmsg_create_txept(struct rpmsg_endpoint *ept, struct rpmsg_device *rdev, const char *name, int size);

/**
 * @brief Create rx endpoint
 * 
 * @param ept  rpmsg endpoint
 * @param rdev rpmsg device
 * @param name name of endpoint
 * @param cb   user rx callback function
 * @return int 0 for success
 */
int ma35_rpmsg_create_rxept(struct rpmsg_endpoint *ept, struct rpmsg_device *rdev, const char *name, rpmsg_ept_cb cb);

/**
 * @brief Destroy rpmsg endpoint
 * 
 * @param ept rpmsg endpoint
 */
int ma35_rpmsg_destroy_ept(struct rpmsg_endpoint *ept);

/**
 * @brief Receive data by rx endpoint
 *        poll for head or remote close
 *        this function calls user callback if data is ready
 * @param ept 
 * @return "true" for success
 *         "RPMSG_ERR_PERM" if remote endpoint closed
 *         - 1. Do nothing and try reconnecting 2. Destroy and exit
 *         "RPMSG_ERR_NO_BUFF" if rx buffer is full
 *         - Warning: Increase receiver freq. or decrease sender freq.
 */
int ma35_rpmsg_poll(struct rpmsg_endpoint *ept);

/**
 * @brief Send data by tx endpoint
 * 
 * @param ept  rpmsg endpoint
 * @param data data to send
 * @param len  data length
 * @return "positive" data length sent
 *         "RPMSG_ERR_PERM" if remote endpoint closed
 *         - 1. Do nothing and try reconnecting 2. Destroy and exit
 *         "RPMSG_ERR_NO_BUFF" if Tx channel is blocking
 *         "RPMSG_ERR_INIT" if remote endpoint is not ready
 */
int ma35_rpmsg_send(struct rpmsg_endpoint *ept, const void *data, int len);

/**
 * @brief 
 * 
 * @param avail get available buffer size
 * @param total get total buffer size
 * @return int  0 for success
 */
int ma35_rpmsg_get_buffer_size(int *avail, int *total);

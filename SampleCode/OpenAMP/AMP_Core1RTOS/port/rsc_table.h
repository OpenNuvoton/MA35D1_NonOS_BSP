/*************************************************************************//**
 * @file     rsc_table.h
 * @version  V1.00
 * @brief    Resource table fot amp
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

/*
 * This file populates resource table for BM remote
 * for use by the Linux host
 */

#ifndef RSC_TABLE_H_
#define RSC_TABLE_H_

#include <stddef.h>
#include <openamp/open_amp.h>
#include "OpenAMPConfig.h"

#if defined __cplusplus
extern "C" {
#endif

#define EPT_TYPE_TX            0x01
#define EPT_TYPE_RX            0x10

#define VRING_DESC_CMD_HEAD    0x1 // write
#define VRING_DESC_CMD_RELOAD  0x2
#define VRING_DESC_CMD_CLAIM   0x10 // scan, no need notification
#define VRING_DESC_CMD_CLOSE   0x20 // remote close it

#define VRING_DESC_STS_READING 0x1
#define VRING_DESC_STS_RELOAD  0x2
#define VRING_DESC_STS_ACK     0x4
#define VRING_DESC_STS_UNKNOWN 0x8
#define VRING_DESC_STS_BIND    0x10
#define VRING_DESC_STS_CLOSE   0x20
#define VRING_DESC_STS_ERR     0x80

#define IPI_CMD_REQUEST        0x10
#define IPI_CMD_REPLY          0x11
#define IPI_CMD_MODIFY         0x20
#define IPI_CMD_UPDATE         0x22

#define RXBUF_QUEUE_SIZE       ( 7 ) /* If the heap size is insufficient, try reducing the queue size */
#define RXBUF_POOL_SIZE        ( RXBUF_QUEUE_SIZE + 1 )
#define BINDING_SLEEP_MS       ( 500 )
#define QUEUE_WAIT_TICK        ( 0 ) // waiting time if blocked
#define VRING_SIZE             8 /* Number of desc supported by this vring (must be power of two) */

/* Not configurable */
#define KICK_DEV_ID            0
#define NO_RESOURCE_ENTRIES    8
#define VRING_ALIGN            0x80
#define SHARED_BUF_OFFSET      sizeof(struct remote_resource_table) /* Reserved region for resource table */

/* Unused */
#define KICK_DEV_NAME          "poll_dev"
#define KICK_BUS_NAME          "generic"
#define POLL_BASE_ADDR         TMR89_BASE
#define POLL_STOP              0x1U

/* A35 memory attributes */
#define DEVICE_SHARED		   0x00000001U /* device, shareable */
#define DEVICE_NONSHARED	   0x00000010U /* device, non shareable */
#define NORM_NSHARED_NCACHE	   0x00000008U /* Non cacheable  non shareable */
#define NORM_SHARED_NCACHE	   0x0000000CU /* Non cacheable shareable */
#define	PRIV_RW_USER_RO		   (2U << 8U)
#define	PRIV_RW_USER_RW		   (3U << 8U)

struct rsc_table_desc {
	uint8_t  CMD;
	uint8_t  STS;
	uint16_t len;
	uint32_t buf_offset; // offset from tx or rx base
	uint32_t nxt_offset; // offset from shmem_base
	//void    *next;        // continue to read next desc
	char     ns[NO_NAME_SERVICE];
}__attribute__((packed, aligned(4)));

/* Resource table for the given remote, 256 bytes aligned */
struct remote_resource_table {
	unsigned int version;
	unsigned int num;
	unsigned int reserved[2];
	unsigned int offset[NO_RESOURCE_ENTRIES];
	/* rpmsg vdev entry */
	struct fw_rsc_vdev rpmsg_vdev;
	struct fw_rsc_vdev_vring rpmsg_vring0;
	struct fw_rsc_vdev_vring rpmsg_vring1;
	/* priv */
	struct rsc_table_desc desc_vring0[VRING_SIZE];
	struct rsc_table_desc desc_vring1[VRING_SIZE];
}__attribute__((packed, aligned(0x100)));

void *get_resource_table (int rsc_id, int *len);

#if defined __cplusplus
}
#endif

#endif /* RSC_TABLE_H_ */

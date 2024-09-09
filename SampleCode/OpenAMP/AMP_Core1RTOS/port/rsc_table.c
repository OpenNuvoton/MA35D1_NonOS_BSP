/*************************************************************************//**
 * @file     rsc_table.c
 * @version  V1.00
 * @brief    Demonstrate OpenAMP share memory control.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

/* Linux must have reserved-memory node for this resource_table */

#include <openamp/open_amp.h>
#include "rsc_table.h"

/* Place resource table in special ELF section */
#if 0
#define __section_t(S)          __attribute__((__section__(#S)))
#define __resource              __section_t(.resource_table)
#else
#define __resource
#endif

/* VirtIO rpmsg device id */
#define NUM_TABLE_ENTRIES           1
#define VIRTIO_ID_RPMSG_            7 // must be unique
#define RPMSG_VDEV_DFEATURES        (1 << VIRTIO_RPMSG_F_NS)
#define NUM_VRINGS                  0x02

#ifndef RING_TX
  #error "RING_TX is not defined"
#endif

#ifndef RING_RX
  #error "RING_RX is not defined"
#endif

struct remote_resource_table __resource resources = {
	/* Version */
	1,

	/* Number of table entries */
	NUM_TABLE_ENTRIES,

	/* reserved fields */
	{0, 0,},

	/* Offsets of rsc entries */
	{
        offsetof(struct remote_resource_table, rpmsg_vdev),
		offsetof(struct remote_resource_table, rpmsg_vring0),
		offsetof(struct remote_resource_table, rpmsg_vring1),
		offsetof(struct remote_resource_table, desc_vring0),
		offsetof(struct remote_resource_table, desc_vring1),
	},

	/* Virtio device entry */
	{
        RSC_VDEV, VIRTIO_ID_RPMSG_, 0, RPMSG_VDEV_DFEATURES, 0, 0, 0,
        NUM_VRINGS, {0, 0},
	},

	/* Vring rsc entry - part of vdev rsc entry */
	{RING_TX - SHARED_RSC_TABLE, VRING_ALIGN, VRING_SIZE, 1, RING_TX_SIZE},
	{RING_RX - SHARED_RSC_TABLE, VRING_ALIGN, VRING_SIZE, 2, RING_RX_SIZE},
};

void *get_resource_table (int rsc_id, int *len)
{
	(void) rsc_id;
	*len = sizeof(resources);
	return &resources;
}

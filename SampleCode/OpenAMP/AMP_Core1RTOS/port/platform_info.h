/*************************************************************************//**
 * @file     platform_info.h
 * @version  V1.00
 * @brief    Standard APIs used by OpenAMP
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#ifndef PLATFORM_INFO_H
#define PLATFORM_INFO_H

#include <openamp/remoteproc.h>
#include <openamp/virtio.h>
#include <openamp/rpmsg.h>

#include "FreeRTOS.h"
#include "queue.h"

#if defined __cplusplus
extern "C" {
#endif

typedef struct Node {
    void *rpmsg_ept;
    struct Node* next;
} Node;

struct remoteproc_priv {
	/* Unused, reserved for IPI */
	const char *kick_dev_name;
	const char *kick_dev_bus_name;
	struct metal_device *kick_dev; // link to kick_device
	struct metal_io_region *kick_io; // metal_io_region of above
	// general
	int      ready; // check remote driver is ready, aka first frame
	uint32_t desc_num;
	uint32_t desc_rxbuf; // size of buf managed by 1 desc
	uint32_t desc_txbuf;
	u64      shmem_base;
	u64      shmem_tx_base;
	u64      shmem_rx_base;
	// tx
	uint8_t *buf_flag; // manage tx buffer usage
	// rx
	void   **kick_ept; // save pointer to each rx ept by id
	Node    *head_ept; // head of ns task list
};

struct rxbuf_queue_t {
	void *rxbuf;
	int len;
	uint8_t cmd;
	uint8_t id;
};

struct rpmsg_endpoint_priv {
	struct rpmsg_endpoint *ept_parent; // save its parent
	int ept_type;
	unsigned int available_len;
	Node* tasknode;
	// tx
	unsigned int no_desc;
	unsigned int id; // desc id of the following
	void *pDesc; // pointer to tx desc link
	metal_mutex_t lock;
	int remote_binded;
	// rx
    void *bind_desc; // pointer to rx desc
	unsigned int bind_id; // index of kickept
	// atomic_int kicked; // abort
	//void *rxbuf; // buffer copied from shared memory
	QueueHandle_t xQueue;
	uint8_t cmd; // save CMD
	void *rxns;
	void **rxpool; // pre-allocated buffer pool
	int poolid;
};

struct rpmsg_endpoint_info{
	char name[32]; // Tx: name of local ept; Rx: name of remote ept to bind with
	u32 type;      // EPT_TYPE_TX or EPT_TYPE_RX
	u32 size;      // Tx: request data length in byte; Rx: reserved
};

struct amp_endpoint {
	struct rpmsg_endpoint_info eptinfo;
	rpmsg_ept_cb cb;           // Rx endpoint callback
	TaskFunction_t task_fn;    // User designed task
	struct rpmsg_endpoint ept;
	TaskHandle_t taskHandle;
	void *ept_priv;            // Reserved for passing cookies
};

extern struct remoteproc_priv rproc_priv;

/**
 * platform_init - initialize the platform
 *
 * It will initialize the platform.
 *
 * @argc: number of arguments
 * @argv: array of the input arguments
 * @platform: pointer to store the platform data pointer
 *
 * return 0 for success or negative value for failure
 */
int platform_init(int argc, char *argv[], void **platform);

/**
 * platform_create_rpmsg_vdev - create rpmsg vdev
 *
 * It will create rpmsg virtio device, and returns the rpmsg virtio
 * device pointer.
 *
 * @platform: pointer to the private data
 * @vdev_index: index of the virtio device, there can more than one vdev
 *              on the platform.
 * @role: virtio driver or virtio device of the vdev
 * @rst_cb: virtio device reset callback
 * @ns_bind_cb: rpmsg name service bind callback
 *
 * return pointer to the rpmsg virtio device
 */
struct rpmsg_device *
platform_create_rpmsg_vdev(void *platform, unsigned int vdev_index,
			   unsigned int role,
			   void (*rst_cb)(struct virtio_device *vdev),
			   rpmsg_ns_bind_cb ns_bind_cb);

/**
 * platform_release_rpmsg_vdev - release rpmsg virtio device
 *
 * @rpdev: pointer to the rpmsg device
 */
void platform_release_rpmsg_vdev(struct rpmsg_device *rpdev, void *platform);

/**
 * platform_cleanup - clean up the platform resource
 *
 * @platform: pointer to the platform
 */
void platform_cleanup(void *platform);

#if defined __cplusplus
}
#endif

#endif /* PLATFORM_INFO_H */

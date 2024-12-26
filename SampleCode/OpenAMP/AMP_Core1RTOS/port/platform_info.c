/*************************************************************************//**
 * @file     platform_info.c
 * @version  V1.00
 * @brief    Standard APIs used by OpenAMP
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <metal/atomic.h>
#include <metal/assert.h>
#include <metal/device.h>
#include <metal/sys.h>
#include <metal/irq.h>
#include <metal/utilities.h>
#include <openamp/rpmsg_virtio.h>
#include <errno.h>
#include "platform_info.h"
#include "rsc_table.h"
#include "NuMicro.h"

int init_system(void);
void cleanup_system(void);
extern int ma35_rpmsg_init_vdev(struct rpmsg_virtio_device *rvdev,
		    struct virtio_device *vdev,
		    rpmsg_ns_bind_cb ns_bind_cb,
		    struct metal_io_region *shm_io,
		    struct rpmsg_virtio_shm_pool *shpool);

/* Polling information used by remoteproc operations.
 * Unused, reserved for IPI
 */
static metal_phys_addr_t poll_phys_addr = POLL_BASE_ADDR;
struct metal_device kick_device = {
	.name = "poll_dev",
	.bus = NULL,
	.num_regions = 1,
	.regions = {
		{
			.virt = (void *)POLL_BASE_ADDR,
			.physmap = &poll_phys_addr,
			.size = 0x1000,
			.page_shift = -1UL,
			.page_mask = -1UL,
			.mem_flags = DEVICE_NONSHARED | PRIV_RW_USER_RW,
			.ops = {NULL},
		}
	},
	.node = {NULL},
	.irq_num = 1,
	.irq_info = (void *)TMR8_IRQn,
};

struct remoteproc_priv rproc_priv = {
	.kick_dev_name = KICK_DEV_NAME,
	.kick_dev_bus_name = KICK_BUS_NAME,
	.desc_num = VRING_SIZE,
	.desc_rxbuf = RING_RX_SIZE / VRING_SIZE,
	.desc_txbuf = RING_TX_SIZE / VRING_SIZE,
	.shmem_base = SHARED_RSC_TABLE,
};

/* processor operations from core1 to core0. It defines notification
 * operation and remote processor management operations.
 */
extern const struct remoteproc_ops ma35_rproc_ops;

/* prime rproc parameter */
static struct remoteproc rproc_prime;

/* resource table in shared memory */
const volatile void *resource_table_shmem = (void *)SHARED_RSC_TABLE;

/* RPMsg virtio shared buffer pool */
static struct rpmsg_virtio_shm_pool shpool;

/* Create rpmsg (no role, act as both server & client) */
struct  rpmsg_device *
platform_create_rpmsg_vdev(void *platform, unsigned int vdev_index,
			   unsigned int role,
			   void (*rst_cb)(struct virtio_device *vdev),
			   rpmsg_ns_bind_cb ns_bind_cb)
{
	struct remoteproc *rproc = platform;
	struct rpmsg_virtio_device *rpmsg_vdev;
	struct virtio_device *vdev;
	void *shbuf;
	struct metal_io_region *shbuf_io;
	int ret;

	rpmsg_vdev = metal_allocate_memory(sizeof(*rpmsg_vdev));

	if (!rpmsg_vdev)
		return NULL;
	shbuf_io = remoteproc_get_io_with_pa(rproc, SHARED_MEM_PA);
	if (!shbuf_io)
		goto err1;
	shbuf = metal_io_phys_to_virt(shbuf_io,
				      SHARED_MEM_PA /*+ SHARED_BUF_OFFSET*/);

	role = VIRTIO_DEV_DRIVER;
	vdev = remoteproc_create_virtio(rproc, vdev_index, role, rst_cb);
	if (!vdev) {
		sysprintf("Failed remoteproc_create_virtio\r\n");
		goto err1;
	}

	sysprintf("Initializing rpmsg shared buffer pool... ");
	/* Only RPMsg virtio driver needs to initialize the shared buffers pool */
	rpmsg_virtio_init_shm_pool(&shpool, shbuf, SHARED_MEM_SIZE);
	sysprintf("finished.\nResource table base: 0x%x.\nMinimum reserved memory size: 0x%x.\n",
			SHARED_RSC_TABLE, (SHARED_BUF_OFFSET + SHARED_MEM_SIZE));

	sysprintf("Initializing rpmsg IPI device... ");
	/* RPMsg virtio device can set shared buffers pool argument to NULL */
	ret = ma35_rpmsg_init_vdev(rpmsg_vdev, vdev, ns_bind_cb,
			       shbuf_io,
			       &shpool);
	if (ret) {
		sysprintf("\nFailed rpmsg_init_vdev\r\n");
		goto err2;
	}
	sysprintf("finished.\r\n");

	return rpmsg_virtio_get_rpmsg_device(rpmsg_vdev);
err2:
	remoteproc_remove_virtio(rproc, vdev);
err1:
	metal_free_memory(rpmsg_vdev);
	return NULL;
}

void platform_release_rpmsg_vdev(struct rpmsg_device *rpdev, void *platform)
{
	struct rpmsg_virtio_device *rpvdev;
	struct remoteproc *rproc;

	rpvdev = metal_container_of(rpdev, struct rpmsg_virtio_device, rdev);
	rproc = platform;

	rpmsg_deinit_vdev(rpvdev);
	remoteproc_remove_virtio(rproc, rpvdev->vdev);
}

/* Create remoteproc & kick device */
static struct remoteproc *
platform_create_rproc(int proc_index, int rsc_index)
{
	void *rsc_table;
	int rsc_size;
	int ret;
	metal_phys_addr_t pa;

	(void) proc_index;
	rsc_table = get_resource_table(rsc_index, &rsc_size);
	/* Copy resource table from local to shared momory */
	memcpy((void *)resource_table_shmem, rsc_table, rsc_size);
	rsc_table = (void *)resource_table_shmem;

	/* Unused, reserved for IPI */
	if (metal_register_generic_device(&kick_device))
		return NULL;

	/* Initialize remoteproc instance */
	if (!remoteproc_init(&rproc_prime, &ma35_rproc_ops, &rproc_priv))
		return NULL;

	/* mmap resource table */
	pa = (metal_phys_addr_t)rsc_table;
	(void *)remoteproc_mmap(&rproc_prime, &pa,
				NULL, rsc_size,
				NORM_NSHARED_NCACHE|PRIV_RW_USER_RW,
				&rproc_prime.rsc_io);

	/* mmap shared memory */
	pa = SHARED_MEM_PA;
	(void *)remoteproc_mmap(&rproc_prime, &pa,
				NULL, SHARED_MEM_SIZE,
				NORM_NSHARED_NCACHE|PRIV_RW_USER_RW,
				NULL);

	/* parse resource table to remoteproc */
	ret = remoteproc_set_rsc_table(&rproc_prime, rsc_table, rsc_size);
	if (ret) {
		sysprintf("Failed to initialize remoteproc\r\n");
		remoteproc_remove(&rproc_prime);
		return NULL;
	}
	sysprintf("Initialize remoteproc successfully.\r\n");

	return &rproc_prime;
}

/* Init openamp */
int platform_init(int argc, char *argv[], void **platform)
{
	unsigned long proc_id = 0;
	unsigned long rsc_id = 0;
	struct remoteproc *rproc;

	if (!platform) {
		sysprintf("Failed to initialize platform,"
			   "NULL pointer to store platform data.\r\n");
		return -EINVAL;
	}
	/* Initialize HW system components */
	init_system();

	if (argc >= 2) {
		proc_id = strtoul(argv[1], NULL, 0);
	}

	if (argc >= 3) {
		rsc_id = strtoul(argv[2], NULL, 0);
	}

	rproc = platform_create_rproc(proc_id, rsc_id);
	if (!rproc) {
		sysprintf("Failed to create remoteproc device.\r\n");
		return -EINVAL;
	}
	*platform = rproc;
	return 0;
}

void platform_cleanup(void *platform)
{
	struct remoteproc *rproc = platform;

	if (rproc)
		remoteproc_remove(rproc);
	cleanup_system();
}

/* Init libmetal */
int init_system(void)
{
	int ret;
    struct metal_init_params metal_param = METAL_INIT_DEFAULTS;

    ret = metal_init(&metal_param);
    if (ret) {
        sysprintf("%s: libmetal init failed\n", __func__);
    }

	return ret;
}

void cleanup_system()
{
	metal_finish();

    /* close endpoint */
}

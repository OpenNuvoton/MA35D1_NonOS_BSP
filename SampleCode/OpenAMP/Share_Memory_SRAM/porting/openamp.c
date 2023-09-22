/****************************************************************************
 * @file     openamp.c
 *
 * @brief    OpenAMP applications.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
 
#include "openamp.h"
#include "rsc_table.h"
#include "metal/sys.h"
#include "metal/device.h"

/* Private define ------------------------------------------------------------*/

#define SHM_DEVICE_NAME         "MA35D1_SHM"

/* Globals */
extern uint32_t WHC0_TX_Flag;

static struct metal_io_region *shm_io;
static struct metal_io_region *rsc_io;
static struct shared_resource_table *rsc_table;
static struct rpmsg_virtio_shm_pool shpool;
struct rpmsg_virtio_device rvdev;

static metal_phys_addr_t shm_physmap;

struct metal_device shm_device = {
    .name = SHM_DEVICE_NAME,
    .num_regions = 2,
    .regions = {
        {.virt = NULL}, /* shared memory */
        {.virt = NULL}, /* rsc_table memory */
    },
    .node = { NULL },
    .irq_num = 0,
    .irq_info = NULL
};

static int OPENAMP_shmem_init(int RPMsgRole)
{
    int status = 0;
    struct metal_device *device;
    struct metal_init_params metal_params = METAL_INIT_DEFAULTS;
    void* rsc_tab_addr;
    int rsc_size;

    metal_init(&metal_params);

    status = metal_register_generic_device(&shm_device);
    if (status != 0)
    {
        return status;
    }

    status = metal_device_open("generic", SHM_DEVICE_NAME, &device);
    if (status != 0) 
    {
        return status;
    }

    shm_physmap = SHM_START_ADDRESS;
    metal_io_init(&device->regions[0], (void *)SHM_START_ADDRESS, &shm_physmap, SHM_SIZE, (unsigned int)(-1), 0, NULL);

    shm_io = metal_device_io_region(device, 0);
    if (shm_io == NULL)
    {
        return -1;
    }

    /* Initialize resources table variables */
    resource_table_init(RPMsgRole, &rsc_tab_addr, &rsc_size);
    rsc_table = (struct shared_resource_table *)rsc_tab_addr;
    if (!rsc_table)
    {
      return -1;
    }

    metal_io_init(&device->regions[1], rsc_table, (metal_phys_addr_t *)rsc_table, rsc_size,(unsigned int)(-1), 0, NULL);

    rsc_io = metal_device_io_region(device, 1);
    if (rsc_io == NULL)
    {
      return -1;
    }

    return 0;
}

int MA35D1_OpenAMP_Init(int RPMsgRole, rpmsg_ns_bind_cb ns_bind_cb)
{
    struct fw_rsc_vdev_vring *vring_rsc;
    struct virtio_device *vdev;
    int status = 0;

    Mbox_Init();

    /* Libmetal Initilalization */
    status = OPENAMP_shmem_init(RPMsgRole);
    if (status)
    {
      return status;
    }

    vdev = rproc_virtio_create_vdev(RPMsgRole, VDEV_ID, &rsc_table->vdev, rsc_io, NULL, Mbox_Notify, NULL);
    if (vdev == NULL)
    {
      return -1;
    }

    rproc_virtio_wait_remote_ready(vdev);
    vring_rsc = &rsc_table->vring0;
    status = rproc_virtio_init_vring(vdev, 0, vring_rsc->notifyid, (void *) ((uint64_t)vring_rsc->da), shm_io,
                                     vring_rsc->num, vring_rsc->align);
    if (status != 0)
    {
        return status;
    }
    vring_rsc = &rsc_table->vring1;
    status = rproc_virtio_init_vring(vdev, 1, vring_rsc->notifyid,
                                     (void *) ((uint64_t)vring_rsc->da), shm_io,
                                     vring_rsc->num, vring_rsc->align);
    if (status != 0)
    {
        return status;
    }

    rpmsg_virtio_init_shm_pool(&shpool, (void *)VRING_BUF_ADDR, (size_t)SHM_SIZE);

    return 0;
}

void OPENAMP_DeInit(void)
{
    rpmsg_deinit_vdev(&rvdev);

    metal_finish();
}

void OPENAMP_init_ept(struct rpmsg_endpoint *ept)
{
    rpmsg_init_ept(ept, "", RPMSG_ADDR_ANY, RPMSG_ADDR_ANY, NULL, NULL);
}

int OPENAMP_create_endpoint(struct rpmsg_endpoint *ept, const char *name,
                            uint32_t dest, rpmsg_ept_cb cb,
                            rpmsg_ns_unbind_cb unbind_cb)
{
    return rpmsg_create_ept(ept, &rvdev.rdev, name, RPMSG_ADDR_ANY, dest, cb, unbind_cb);
}

void OPENAMP_check_for_message(struct rpmsg_endpoint *ept)
{
    Mbox_Poll(ept);
}

int OPENAMP_send_data(struct rpmsg_endpoint *ept, const void *data, int len)
{
    uint32_t u32TX_Length = len;
    uint32_t au32TxBuf[WHC_BUFFER_LEN];
    uint32_t i;

    if (len > SHM_TX_RX_SIZE)
        u32TX_Length = SHM_TX_RX_SIZE;

    WHC0_TX_Flag = TX_NO_ACK;

    for (i = 0; i < len; i++)
    {
        *((uint8_t *) SHM_TX_START_ADDRESS + i) = *((uint8_t *) data + i);
    }

    au32TxBuf[0] = COMMAND_SEND_MSG_TO_M4;
    au32TxBuf[1] = u32TX_Length;
    WHC_Send(WHC0, mbox_ch, au32TxBuf);
    while ((WHC0->TXSTS & 0xf) != 0xf);

    return (int)u32TX_Length;
}

int OPENAMP_check_TxAck(struct rpmsg_endpoint *ept)
{
    if (WHC0_TX_Flag == TX_ACK)
        return 1;
    else
        return 0;
}


/****************************************************************************
 * @file     rsc_table.c
 *
 * @brief    Ressource table.
 *
 * @copyright (C) 2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/

#include <stddef.h> /* needed  for offsetof definition*/
#include "rsc_table.h"
#include "openamp/open_amp.h"

/* Place resource table in special ELF section */
#if defined(__ICCARM__)
//#pragma section =".resource_table"
#elif defined(__GNUC__) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
#define __section_t(S)          __attribute__((__section__(#S)))
#define __resource              __section_t(.resource_table)
#endif

#define RPMSG_IPU_C0_FEATURES       1
#define VRING_COUNT                 2

/* VirtIO rpmsg device id */
#define VIRTIO_ID_RPMSG_            7

extern char system_log_buf[];

#if defined(__ICCARM__)
__root struct shared_resource_table resource_table @ ".resource_table" = {
    .version = 1,
    .num = 2,
    .reserved = {0, 0},
    .offset = {
        offsetof(struct shared_resource_table, vdev),
    },

    /* Virtio device entry */
    .vdev= {
        RSC_VDEV, VIRTIO_ID_RPMSG_, 0, RPMSG_IPU_C0_FEATURES, 0, 0, 0,
        VRING_COUNT, {0, 0},
    },

    /* Vring rsc entry - part of vdev rsc entry */
    .vring0 = {VRING_TX_STR_ADDR, VRING_ALIGNMENT, VRING_NUM_BUF, VRING0_ID, 0},
    .vring1 = {VRING_RX_STR_ADDR, VRING_ALIGNMENT, VRING_NUM_BUF, VRING1_ID, 0},
};

#elif defined(__GNUC__) || defined(__ARMCC_VERSION)

struct shared_resource_table __resource __attribute__((used)) resource_table = {

    .version = 1,
    .num = 2,
    .reserved = {0, 0},
    .offset = {
        offsetof(struct shared_resource_table, vdev),
    },
    /* Virtio device entry */
    .vdev= {
        RSC_VDEV, VIRTIO_ID_RPMSG_, 0, RPMSG_IPU_C0_FEATURES, 0, 0, 0,
        VRING_COUNT, {0, 0},
    },

    /* Vring rsc entry - part of vdev rsc entry */
    .vring0 = {(uint32_t)VRING_TX_STR_ADDR, (uint32_t)VRING_ALIGNMENT, (uint32_t)VRING_NUM_BUF, (uint32_t)VRING0_ID, 0},
    .vring1 = {(uint32_t)VRING_RX_STR_ADDR, (uint32_t)VRING_ALIGNMENT, (uint32_t)VRING_NUM_BUF, (uint32_t)VRING1_ID, 0},
};
#endif


void resource_table_init(int RPMsgRole, void **table_ptr, int *length)
{
    (void)RPMsgRole;
    *length = sizeof(resource_table);
    *table_ptr = &resource_table;
}

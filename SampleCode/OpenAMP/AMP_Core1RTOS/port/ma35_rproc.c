/*************************************************************************//**
 * @file     ma35_rproc.c
 * @version  V1.00
 * @brief    amp driver
 *           IPI :
 * 				   Core0 -> Core1 : RXIPI
 * 				   Core1 -> Core0 : TXIPI
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <metal/atomic.h>
#include <metal/assert.h>
#include <metal/device.h>
#include <metal/irq.h>
#include <metal/utilities.h>
#include <openamp/rpmsg_virtio.h>
#include <openamp/virtqueue.h>
#include "platform_info.h"
#include "rsc_table.h"
#include "NuMicro.h"

extern void *resource_table_shmem;

void RxIPI_IRQHandler(void);
static int ma35_rpmsg_receive(struct rpmsg_endpoint_priv *ept_priv);
static int check_tx_bind_ready(struct rpmsg_endpoint_priv *ept_priv);
static int check_rx_bind_ready(struct rpmsg_endpoint_priv *ept_priv);
static int ma35_rpmsg_reconnect_ept(struct rpmsg_endpoint_priv *ept_priv);
static int ma35_desc_init(struct remoteproc_priv *priv);

/**
 * @brief 
 * remoteproc device is claimed in platform_info.c
 * @param rproc 
 * @param ops 
 * @param arg 
 * @return struct remoteproc* 
 */
static struct remoteproc *
ma35_rproc_init(struct remoteproc *rproc,
			const struct remoteproc_ops *ops, void *arg)
{
	struct remoteproc_priv *prproc = arg;
	struct metal_device *kick_dev;
	int ret;

	if (!rproc || !prproc || !ops)
		return NULL;

	/* Unused, reserved for IPI */
	ret = metal_device_open(prproc->kick_dev_bus_name,
				prproc->kick_dev_name,
				&kick_dev);
	if (ret) {
		sysprintf("failed to open polling device: %d.\r\n", ret);
		return NULL;
	}
	rproc->priv = prproc;
	prproc->kick_dev = kick_dev;
	prproc->kick_io = metal_device_io_region(kick_dev, 0);
	if (!prproc->kick_io)
		goto err1;
	/* End of IPI init */

	rproc->ops = ops;

	/* Init HW here to support IPI */
	IRQ_SetHandler((IRQn_ID_t)RXIPI_IRQ_NUM, RxIPI_IRQHandler);
	IRQ_SetTarget(RXIPI_IRQ_NUM, IRQ_CPU_1);
	IRQ_Enable((IRQn_ID_t)RXIPI_IRQ_NUM);

	/* Rx handler is registered by remote */
	IRQ_SetTarget(TXIPI_IRQ_NUM, IRQ_CPU_0);

	return rproc;
err1:
	metal_device_close(kick_dev);
	return NULL;
}

static void ma35_rproc_remove(struct remoteproc *rproc)
{
	struct remoteproc_priv *prproc;
	struct metal_device *dev;

	if (!rproc)
		return;
	prproc = rproc->priv;

	(void)dev;
	TIMER_Close(RXIPI_BASE);
	TIMER_DisableInt(RXIPI_BASE);
	IRQ_Disable((IRQn_ID_t)TMR9_IRQn);

	TIMER_Close(TXIPI_BASE);
	TIMER_DisableInt(TXIPI_BASE);
	IRQ_Disable((IRQn_ID_t)TMR8_IRQn);

	metal_device_close(prproc->kick_dev);
}

static void *
ma35_rproc_mmap(struct remoteproc *rproc, metal_phys_addr_t *pa,
			metal_phys_addr_t *da, size_t size,
			unsigned int attribute, struct metal_io_region **io)
{
	struct remoteproc_mem *mem;
	metal_phys_addr_t lpa, lda;
	struct metal_io_region *tmpio;

	lpa = *pa;
	lda = *da;

	if (lpa == METAL_BAD_PHYS && lda == METAL_BAD_PHYS)
		return NULL;
	if (lpa == METAL_BAD_PHYS)
		lpa = lda;
	if (lda == METAL_BAD_PHYS)
		lda = lpa;

	if (!attribute)
		attribute = NORM_SHARED_NCACHE | PRIV_RW_USER_RW;
	mem = metal_allocate_memory(sizeof(*mem));
	if (!mem)
		return NULL;
	tmpio = metal_allocate_memory(sizeof(*tmpio));
	if (!tmpio) {
		metal_free_memory(mem);
		return NULL;
	}
	remoteproc_init_mem(mem, NULL, lpa, lda, size, tmpio);
	/* va is the same as pa in this platform */
	metal_io_init(tmpio, (void *)lpa, &mem->pa, size,
		      sizeof(metal_phys_addr_t) << 3, attribute, NULL);
	remoteproc_add_mem(rproc, mem);
	*pa = lpa;
	*da = lda;
	if (io)
		*io = tmpio;
	return metal_io_phys_to_virt(tmpio, mem->pa);
}

void RxIPI_IRQHandler(void)
{
	int i, rsc_size;
	struct remote_resource_table *rsc_table = resource_table_shmem;
	struct rpmsg_endpoint_priv *ept_priv;
	struct rsc_table_desc *desc;

	if(rsc_table->reserved[1] == IPI_CMD_REQUEST)
	{
		if(!ma35_rpmsg_remote_ready())
		{
			/* Get remap first */
			rsc_table = get_resource_table(0, &rsc_size);
			/* Copy resource table from local to shared momory */
			memcpy((void *)resource_table_shmem, rsc_table, rsc_size);
			rsc_table = (void *)resource_table_shmem;
			ma35_desc_init(NULL);
		}
		rsc_table->reserved[1] = 0;
		rsc_table->reserved[0] = IPI_CMD_REPLY;
		TIMER_Start(TXIPI_BASE);
		if(TIMER_GetIntFlag(RXIPI_BASE) == 1)
			TIMER_ClearIntFlag(RXIPI_BASE);
		return;
	}

	// scan desc
	for (i = 0; i < rproc_priv.desc_num; i++)
	{
		if(rsc_table->desc_vring1[i].CMD & (VRING_DESC_CMD_HEAD | VRING_DESC_CMD_CLOSE))
		{
			ept_priv = (struct rpmsg_endpoint_priv *)rproc_priv.kick_ept[i];
			if(!check_rx_bind_ready(ept_priv))
			{
				desc = ept_priv->bind_desc;
				ept_priv->cmd = desc->CMD;
				desc->CMD = 0;
				desc->STS = VRING_DESC_STS_READING;
				if(ept_priv->cmd & VRING_DESC_CMD_HEAD)
				{
					if(ma35_rpmsg_receive(ept_priv) != RPMSG_SUCCESS)
						continue;
				}
				else if(ept_priv->cmd & VRING_DESC_CMD_CLOSE)
				{
					ma35_rpmsg_reconnect_ept(ept_priv);
				}
			}
		}
	}

    if(TIMER_GetIntFlag(RXIPI_BASE) == 1)
    	TIMER_ClearIntFlag(RXIPI_BASE);
}

// Use timer to generate INT to remote
static int ma35_notify_remote(struct metal_io_region *io_resion, uint32_t id)
{
	(void *)io_resion;
	(void)id;

	/* check remote ready before send notification */
	if(GIC_GetEnableIRQ(TMR8_IRQn))
	{
		TIMER_Start(TXIPI_BASE);
		return 0;
	}
	else
	{
		//sysprintf("Notification fail : Remote driver not ready yet.\n");
		return -EAGAIN;
	}
}

static int ma35_rproc_notify(struct remoteproc *rproc, uint32_t id)
{
	struct remoteproc_priv *prproc;

	(void)id;
	if (!rproc)
		return -1;
	prproc = rproc->priv;

	ma35_notify_remote(prproc->kick_io, KICK_DEV_ID);

	return 0;
}

const struct remoteproc_ops ma35_rproc_ops = {
	.init = ma35_rproc_init,
	.remove = ma35_rproc_remove,
	.mmap = ma35_rproc_mmap,
	.notify = ma35_rproc_notify,
	.start = NULL,
	.stop = NULL,
	.shutdown = NULL,
	.get_mem = NULL,
};

static Node* create_node(struct rpmsg_endpoint *ept) {
    Node* new_node = (Node*)metal_allocate_memory(sizeof(Node));
    if (!new_node) {
        sysprintf("Memory allocation failed\n");
        return NULL;
    }
    new_node->rpmsg_ept = ept;
    new_node->next = NULL;
    return new_node;
}

static void add_node(Node* head, struct rpmsg_endpoint *ept) {
	struct rpmsg_endpoint_priv *ept_priv = ept->priv;
    Node* new_node = create_node(ept);
    if (!new_node) {
        return;
    }

    Node* temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new_node;
	ept_priv->tasknode = new_node;
}

static void remove_node(Node* head, struct rpmsg_endpoint *ept) {
	struct rpmsg_endpoint_priv *ept_priv = ept->priv;
    Node* temp = head;
    Node* prev = NULL;

    while (temp->next != NULL && temp->next->rpmsg_ept != ept) {
        prev = temp;
        temp = temp->next;
    }

    if (temp->next == NULL) {
        return; // Node not found
    }

    Node* node_to_remove = temp->next;
    temp->next = node_to_remove->next;
	ept_priv->tasknode = NULL;
    metal_free_memory(node_to_remove);
}

static int is_empty(Node* head) {
	return (head->next == NULL);
}

static uint8_t ma35_rpmsg_retrieve_status(struct rpmsg_endpoint_priv *ept_priv)
{
	struct rsc_table_desc *desc;

	desc = ept_priv->bind_desc;

	if(desc)
		return desc->STS;
	else
		return -1;
}

static int ma35_rpmsg_receive_status(struct rpmsg_endpoint_priv *ept_priv, uint8_t sts)
{
	struct rsc_table_desc *desc;

	desc = ept_priv->bind_desc;

	if(desc)
		desc->STS = sts;

	return 0;
}

int ma35_rpmsg_poll(struct rpmsg_endpoint *ept)
{
	struct rpmsg_endpoint_priv *ept_priv;
	struct rxbuf_queue_t rxqueue;
	int ret, len;
	void *buf;

	if(!ept)
		return RPMSG_ERR_PARAM;

	ept_priv = ept->priv;

	if(!ept_priv->xQueue)
		return RPMSG_ERR_INIT;
		
	ret = xQueueReceive(ept_priv->xQueue, &rxqueue, QUEUE_WAIT_TICK);
	if(ret == pdTRUE)
	{
		if(rxqueue.cmd & VRING_DESC_CMD_HEAD)
		{
			if(ept->cb)
				ept->cb(ept, rxqueue.rxbuf, rxqueue.len, 0, NULL);

			if(ma35_rpmsg_retrieve_status(ept_priv) == VRING_DESC_STS_ERR)
				ret = RPMSG_ERR_NO_BUFF;	

			if(uxQueueSpacesAvailable(ept_priv->xQueue))
				ma35_rpmsg_receive_status(ept_priv, VRING_DESC_STS_ACK); // Its fine if no binding

			return ret;
		}
		else if(rxqueue.cmd & VRING_DESC_CMD_CLOSE)
		{
			return RPMSG_ERR_PERM;
		}
	}

	return 0;
}

static int ma35_rpmsg_desc_reset(struct rpmsg_endpoint *ept)
{
	struct rpmsg_endpoint_priv *ept_priv;
	struct rsc_table_desc *desc;

	if(!ept)
		return RPMSG_ERR_PARAM;

	ept_priv = ept->priv;
	desc = ept_priv->pDesc;

	while(desc)
	{
		desc->CMD = desc->STS = desc->len = 0;
		desc = (desc->nxt_offset == 0) ? NULL : (struct rsc_table_desc *)(rproc_priv.shmem_base + desc->nxt_offset);
	}

	return RPMSG_SUCCESS;
}

static int ma35_rpmsg_send_offchannel_raw(struct rpmsg_endpoint *ept, uint32_t src,
			      uint32_t dst, const void *data, int len,
			      bool wait)
{
	int i, res;
	struct rpmsg_endpoint_priv *ept_priv;
	struct rsc_table_desc *desc;

	if(!ept || !ept->priv || !data || len < 0)
		return RPMSG_ERR_PARAM;

	ept_priv = ept->priv;
	if(len > ept_priv->available_len)
		return RPMSG_ERR_NO_MEM;

	desc = ept_priv->pDesc;
	if(!desc)
		return RPMSG_ERR_DEV_STATE;

	res = check_tx_bind_ready(ept_priv);
	if(res)
		return res;

	metal_mutex_acquire(&ept_priv->lock);

	ma35_rpmsg_desc_reset(ept);

	for(i = 0, res = len; i < len; i += rproc_priv.desc_txbuf, res -= rproc_priv.desc_txbuf)
	{
		if(res <= rproc_priv.desc_txbuf)
		{
			memcpy((void *)(rproc_priv.shmem_tx_base + desc->buf_offset), (void *)((uint8_t *)data + i), res);
			desc->len = res;
		}
		else
		{
			memcpy((void *)(rproc_priv.shmem_tx_base + desc->buf_offset), (void *)((uint8_t *)data + i), rproc_priv.desc_txbuf);
			desc->len = rproc_priv.desc_txbuf;
			desc = (struct rsc_table_desc *)(rproc_priv.shmem_base + desc->nxt_offset);
		}
	}
	((struct rsc_table_desc *)ept_priv->pDesc)->CMD = VRING_DESC_CMD_HEAD;

	ma35_notify_remote(NULL, 0);

	metal_mutex_release(&ept_priv->lock);
	
	return len;
}

static int ma35_allocate_buf_pool(struct rpmsg_endpoint_priv *ept_priv, int size)
{
	int i, j;

	if(!ept_priv)
		return -1;

	if(!ept_priv->rxpool)
		return -1;

	ept_priv->poolid = 0;
	for(i = 0; i < RXBUF_POOL_SIZE; i++)
	{
		ept_priv->rxpool[i] = metal_allocate_memory(size);
		if(!ept_priv->rxpool[i])
		{
			for(j = 0; j < i; j++)
				metal_free_memory(ept_priv->rxpool[j]);
			return -1;
		}
	}

	return 0;
}

/* used to bind desc remote to local. no need to scan if binded. return rx size provide by remote  */
static int ma35_rpmsg_ns_bind_remote(struct rpmsg_endpoint *ept, char *ns, bool wait)
{
	// poll for desc to search ns
	struct rpmsg_endpoint_priv *ept_priv;
	struct rpmsg_device *rdev;
	struct remote_resource_table *rsc_table = resource_table_shmem;
	struct rsc_table_desc *rxdesc;
	int i, ret = 0;

	if(!ept || !ns)
		return RPMSG_ERR_PARAM;

	ept_priv = ept->priv;
	rdev = ept->rdev;

NS_MATCHING:
	for(i = 0; i < rproc_priv.desc_num; i++)
	{
		if((rsc_table->desc_vring1[i].CMD & VRING_DESC_CMD_CLAIM) == 0)
			continue;
		if(!memcmp(ns, rsc_table->desc_vring1[i].ns, NO_NAME_SERVICE))
		{
			metal_mutex_acquire(&rdev->lock);

			ept_priv->bind_desc = (struct rsc_table_desc *)&rsc_table->desc_vring1[i];
			ept_priv->bind_id = i;
			rproc_priv.kick_ept[ept_priv->bind_id] = ept_priv;

			rxdesc = ept_priv->bind_desc;
			while(rxdesc)
			{
				ret += rproc_priv.desc_rxbuf;
				rxdesc = (rxdesc->nxt_offset == 0) ? NULL : (struct rsc_table_desc *)(rproc_priv.shmem_base + rxdesc->nxt_offset);
			}

			// bind success, print something
			if(ma35_allocate_buf_pool(ept_priv, ret))
				ret = 0;
			else
				sysprintf("Binded with remote ept \"%s\" with size %d.\n", ns, ret);

			metal_mutex_release(&rdev->lock);
			
			return ret;
		}
	}

	if(wait)
		goto NS_MATCHING;

	return -1; // no match
}

static int check_tx_bind_ready(struct rpmsg_endpoint_priv *ept_priv)
{
	struct rsc_table_desc *desc;

	if(!ept_priv->remote_binded)
		return RPMSG_ERR_INIT;

	desc = ept_priv->pDesc;

	if(desc->STS & (VRING_DESC_STS_READING | VRING_DESC_STS_ERR))
		return RPMSG_ERR_NO_BUFF;
	else if(desc->STS == VRING_DESC_STS_CLOSE)
	{
		ma35_rpmsg_reconnect_ept(ept_priv);
		return RPMSG_ERR_PERM;
	}

	return 0;
}

static int check_rx_bind_ready(struct rpmsg_endpoint_priv *ept_priv)
{
	if(!ept_priv)
		return -1;

	if(!ept_priv->bind_desc)
		return -1;
	
	return RPMSG_SUCCESS;
}

static int ma35_kill_ns_bind(struct rpmsg_endpoint_priv *ept_priv)
{
	struct rsc_table_desc *rxdesc;
	struct rxbuf_queue_t rxqueue;

	if(check_rx_bind_ready(ept_priv) < 0)
		return RPMSG_EOPNOTSUPP;

	rxdesc = ept_priv->bind_desc;
	ept_priv->bind_desc = NULL;
	rproc_priv.kick_ept[ept_priv->bind_id] = NULL;
	ept_priv->bind_id = 0;

	for (int i = 0; i < RXBUF_POOL_SIZE; i++)
		metal_free_memory(ept_priv->rxpool[i]);

	rxqueue.cmd = ept_priv->cmd;
	// No matter success or not
	xQueueSend(ept_priv->xQueue, &rxqueue, QUEUE_WAIT_TICK);
	rxdesc->STS = VRING_DESC_STS_CLOSE;

	return RPMSG_SUCCESS;
}

int ma35_rpmsg_send(struct rpmsg_endpoint *ept, const void *data, int len)
{
	struct rpmsg_device *rdev;
	struct rpmsg_endpoint_priv *ept_priv;
	
	if (!ept || !ept->rdev || !data || len < 0)
		return RPMSG_ERR_PARAM;

	rdev = ept->rdev;
	ept_priv = ept->priv;
	if(ept_priv->ept_type == EPT_TYPE_TX)
		return ma35_rpmsg_send_offchannel_raw(ept, 0, 0, data, len, true);
	else
		return RPMSG_EOPNOTSUPP;
}

static int ma35_rpmsg_receive(struct rpmsg_endpoint_priv *ept_priv)
{
	struct rsc_table_desc *desc;
	struct rxbuf_queue_t rxqueue;
	int rxlen = 0;
	unsigned char id;
	void *buf;

	buf = ept_priv->rxpool[ept_priv->poolid];
	desc = (struct rsc_table_desc *)ept_priv->bind_desc;
	while(desc && desc->len)
	{
		memcpy((void *)((char *)buf + rxlen), (void *)(rproc_priv.shmem_rx_base + desc->buf_offset), desc->len);
		rxlen += desc->len;
		desc = (desc->nxt_offset == 0) ? NULL : (struct rsc_table_desc *)(rproc_priv.shmem_base + desc->nxt_offset);
	}

	// prepare desc to be copied		
	rxqueue.rxbuf = buf;
	rxqueue.len = rxlen;
	rxqueue.cmd = ept_priv->cmd;
	rxqueue.id = ept_priv->poolid;
	// enqueue to rx queue
	if(xQueueSend(ept_priv->xQueue, &rxqueue, QUEUE_WAIT_TICK) != pdTRUE)
	{
		// queue is full, flow control on queue
		ma35_rpmsg_receive_status(ept_priv, VRING_DESC_STS_ERR);
		return RPMSG_ERR_NO_BUFF;
	}
	if(++ept_priv->poolid >= RXBUF_POOL_SIZE)
		ept_priv->poolid = 0;
	ma35_rpmsg_receive_status(ept_priv, VRING_DESC_STS_ACK);

	return RPMSG_SUCCESS;
}

static int ma35_check_available_desc(struct rpmsg_endpoint_priv *ept_priv,
							  struct remote_resource_table *rsc_table,
							  size_t len)
{
	int i, req, avail, id;
	struct rpmsg_endpoint *ept = ept_priv->ept_parent;
	struct rpmsg_device *rdev;
	struct rsc_table_desc head;
	struct rsc_table_desc *desc = &head;

	rdev = ept->rdev;
	metal_mutex_acquire(&rdev->lock);

	for (i = 0, avail = 0; i < rproc_priv.desc_num; i++)
	{
		if(!rproc_priv.buf_flag[i])
			avail++;
	}

	req = len / rproc_priv.desc_txbuf;
	if((len % rproc_priv.desc_txbuf) != 0)
		req++;

	if(req > avail || len == 0)
		goto err; /* buffer insufficient */

	ept_priv->no_desc = req;
	ept_priv->available_len = len;

	id = -1;
	for (i = 0; (i < rproc_priv.desc_num) && (req > 0); i++)
	{
		if(!rproc_priv.buf_flag[i])
		{
			rproc_priv.buf_flag[i] = 1;
			desc->nxt_offset = offsetof(struct remote_resource_table, desc_vring0) + sizeof(struct rsc_table_desc) * i;
			desc = (struct rsc_table_desc *)(rproc_priv.shmem_base + desc->nxt_offset);
			req--;
			if(id < 0)
				id = i;
		}
	}
	if(id < 0)
		goto err;

	ept_priv->id = id;
	ept_priv->pDesc = (void *)(rproc_priv.shmem_base + head.nxt_offset);

	metal_mutex_release(&rdev->lock);
	
	return len;
err:
	metal_mutex_release(&rdev->lock);
	sysprintf("no buffer available\n");
	return -1;
}

// use desc status to ensure buffer has been read by remote
static int ma35_request_tx_desc(struct rpmsg_endpoint *ept,
						 struct rpmsg_virtio_device *rvdev,
						 int len)
{
	int i, ret;
	struct remote_resource_table *rsc_table = resource_table_shmem;
	struct rpmsg_endpoint_priv *ept_priv;
	struct rsc_table_desc *desc;

	if (!ept || !rvdev || !rsc_table || len < 0)
		return -1;

	ept_priv = metal_allocate_memory(sizeof(struct rpmsg_endpoint_priv));
	memset(ept_priv, 0, sizeof(struct rpmsg_endpoint_priv));
	ept_priv->ept_type = EPT_TYPE_TX;
	ept->priv = ept_priv;
	ept_priv->ept_parent = ept;
	metal_mutex_init(&ept_priv->lock);

	ret = ma35_check_available_desc(ept_priv, rsc_table, len);
	if(ret <= 0)
	{
		metal_free_memory(ept_priv);
		return RPMSG_ERR_NO_BUFF;
	}

	desc = ept_priv->pDesc;
	desc->CMD = VRING_DESC_CMD_CLAIM;
	desc->STS = 0;
	strncpy(desc->ns, ept->name, NO_NAME_SERVICE);

	return ret;
}

static int ma35_rpmsg_release_shmem(struct rpmsg_endpoint *ept)
{
	u32 i, offset;
	struct rpmsg_endpoint_priv *ept_priv = ept->priv;
	struct remote_resource_table *rsc_table = resource_table_shmem;
	struct rsc_table_desc *desc, *next;

	if(!ept || !ept_priv)
		return -1;

	desc = ept_priv->pDesc;

	metal_mutex_acquire(&ept_priv->lock);

	while(desc)
	{
		next = (struct rsc_table_desc *)(rproc_priv.shmem_base + desc->nxt_offset);
		i = desc->buf_offset/rproc_priv.desc_txbuf;
		rproc_priv.buf_flag[i] = 0;
		offset = desc->nxt_offset;
		// resume, but CMD is reserved
		memset(&desc->STS, 0, sizeof(struct rsc_table_desc) - 1);
		desc->buf_offset = rproc_priv.desc_txbuf * i;
		desc = (offset == 0) ? NULL : next;
	}

	metal_mutex_release(&ept_priv->lock);

	return 0;
}

int ma35_rpmsg_get_buffer_size(int *avail, int *total)
{
	int i, j;

	if(!rproc_priv.ready)
		return -1;

	for (i = 0, j = 0; i < rproc_priv.desc_num; i++)
	{
		if(!rproc_priv.buf_flag[i])
			j++;
	}
	*avail = j * rproc_priv.desc_txbuf;
	*total = rproc_priv.desc_num * rproc_priv.desc_txbuf;

	return 0;
}

static int ma35_rpmsg_request_shmem(struct rpmsg_endpoint *ept, struct rpmsg_device *rdev,
						uint32_t len, bool wait)
{
	// request desc
	struct rpmsg_virtio_device *rvdev;
	struct rsc_table_desc *tx_desc;
	int ret;

	rvdev = metal_container_of(rdev, struct rpmsg_virtio_device, rdev);

	ret = ma35_request_tx_desc(ept, rvdev, len);

	if(ret < 0)
		return -1; /* no desc available */

	return ret;
}

static int ma35_rpmsg_reconnect_ept(struct rpmsg_endpoint_priv *ept_priv)
{
	struct rsc_table_desc *desc;

	if(ept_priv->tasknode)
		return -1;

	if(ept_priv->ept_type == EPT_TYPE_TX)
	{
		// shared memory not released
		ept_priv->remote_binded = 0;
		desc = ept_priv->pDesc;
		desc->STS = 0;
		desc->CMD = VRING_DESC_CMD_CLAIM;
	}
	else // EPT_TYPE_RX
	{
		ma35_kill_ns_bind(ept_priv);
	}

	// ept reset finished, add to list
	add_node(rproc_priv.head_ept, ept_priv->ept_parent);

	return RPMSG_SUCCESS;
}

static int ma35_rpmsg_bind_process(struct rpmsg_endpoint *ept)
{
	struct rpmsg_endpoint_priv *ept_priv = ept->priv;
	struct rsc_table_desc *desc;
	int buf_size;

	if(ept_priv->ept_type == EPT_TYPE_TX)
	{
		// just check STS
		desc = ept_priv->pDesc;
		if(desc->STS & VRING_DESC_STS_BIND) {
			ept_priv->remote_binded = 1;
			sysprintf("Remote binded to \"%s\".\n", desc->ns);
			return 1;
		}
	}
	else // EPT_TYPE_RX
	{
		// scan for rxns
		buf_size = ma35_rpmsg_ns_bind_remote(ept, ept_priv->rxns, false);
		if(buf_size > 0) {
			ept_priv->available_len = buf_size;
			desc = ept_priv->bind_desc;
			desc->STS = VRING_DESC_STS_BIND;
			return 2;
		}
	}

	return 0;
}

static int ma35_rpmsg_create_ept(struct rpmsg_endpoint *ept, struct rpmsg_device *rdev, const char *name, rpmsg_ept_cb cb)
{
	if (!ept || !rdev || !rproc_priv.ready)
		return RPMSG_ERR_PARAM;

	metal_mutex_acquire(&rdev->lock);
	
	memset(ept->name, 0, sizeof(ept->name));
	strncpy(ept->name, name ? name : "", NO_NAME_SERVICE);
	ept->refcnt = 1;
	ept->cb = cb;
	ept->rdev = rdev;

	metal_list_add_tail(&rdev->endpoints, &ept->node);

	metal_mutex_release(&rdev->lock);

	return RPMSG_SUCCESS;
}

int ma35_rpmsg_destroy_ept(struct rpmsg_endpoint *ept)
{
	struct rpmsg_device *rdev;
	struct rpmsg_endpoint_priv *ept_priv = ept->priv;
	struct rsc_table_desc *desc;

	rdev = ept->rdev;

	if (!ept || !rdev || !ept_priv)
		return -1;

	if(ept_priv->ept_type == EPT_TYPE_TX)
	{
		if(ept_priv->remote_binded)
		{
			ept_priv->remote_binded = 0;
			desc = ept_priv->pDesc;
			desc->CMD = VRING_DESC_CMD_CLOSE;
			ma35_notify_remote(NULL, 0);
		}
		// maybe no need to wait, just leave the CMD
		ma35_rpmsg_release_shmem(ept);
	}
	else // EPT_TYPE_RX
	{
		desc = ept_priv->bind_desc;
		if(desc) { // NULL: no bind or already get close cmd
			desc->STS = VRING_DESC_STS_CLOSE;
			for (int i = 0; i < RXBUF_POOL_SIZE; i++)
				metal_free_memory(ept_priv->rxpool[i]);
		}
		ept_priv->bind_desc = NULL;
		rproc_priv.kick_ept[ept_priv->bind_id] = NULL;
		ept_priv->bind_id = 0;
		// release rx queue
		metal_mutex_acquire(&ept_priv->lock);
		vQueueDelete(ept_priv->xQueue);
		ept_priv->xQueue = NULL;
		metal_mutex_release(&ept_priv->lock);

		metal_free_memory(ept_priv->rxpool);
		metal_free_memory(ept_priv->rxns);
	}

	// kill binding task
	if(ept_priv->tasknode)
		remove_node(rproc_priv.head_ept, ept);

	metal_mutex_deinit(&ept_priv->lock);
	metal_free_memory(ept_priv);
	ept->priv = NULL;

	metal_mutex_acquire(&rdev->lock);

	metal_list_del(&ept->node);

	metal_mutex_release(&rdev->lock);

	return RPMSG_SUCCESS;
}

int ma35_rpmsg_create_txept(struct rpmsg_endpoint *ept, struct rpmsg_device *rdev, const char *name, int size)
{
	int ret;

	ma35_rpmsg_create_ept(ept, rdev, name, NULL);

	ret = ma35_rpmsg_request_shmem(ept, rdev, size, 0);

	if(ret > 0)
	{
		// ept init finished, add to list
		add_node(rproc_priv.head_ept, ept);
	}

	return ret;
}

int ma35_rpmsg_create_rxept(struct rpmsg_endpoint *ept, struct rpmsg_device *rdev, const char *name, rpmsg_ept_cb cb)
{
	struct rpmsg_endpoint_priv *ept_priv;
	int ret;

	ret = ma35_rpmsg_create_ept(ept, rdev, name, cb);
	if(ret)
		return ret;

	ept_priv = metal_allocate_memory(sizeof(struct rpmsg_endpoint_priv));
	if(!ept_priv)
		return RPMSG_ERR_NO_MEM;
	memset(ept_priv, 0, sizeof(struct rpmsg_endpoint_priv));
	ept_priv->ept_type = EPT_TYPE_RX;
	ept->priv = ept_priv;
	ept_priv->ept_parent = ept;
	metal_mutex_init(&ept_priv->lock);

	// create rxbuf queue
	ept_priv->xQueue = xQueueCreate(RXBUF_QUEUE_SIZE, sizeof(struct rxbuf_queue_t));
	if(!ept_priv->xQueue)
		goto err;
	ept_priv->rxpool = metal_allocate_memory(sizeof(void *) * RXBUF_POOL_SIZE);
	if(!ept_priv->rxpool)
		goto err1;
	ept_priv->rxns = metal_allocate_memory(sizeof(char) * RPMSG_NAME_SIZE);
	if(!ept_priv->rxns)
		goto err2;
	strncpy(ept_priv->rxns, name, NO_NAME_SERVICE);

	// ept init finished, add to list
	add_node(rproc_priv.head_ept, ept);

	return RPMSG_SUCCESS;
err2:
	metal_free_memory(ept_priv->rxpool);
err1:
	vQueueDelete(ept_priv->xQueue);
err:
	metal_free_memory(ept_priv);
	return RPMSG_ERR_NO_MEM;
}

int ma35_rpmsg_remote_ready(void)
{
	return rproc_priv.ready == 1;
}

static int ma35_desc_init(struct remoteproc_priv *priv)
{
	struct remote_resource_table *rsc_table = resource_table_shmem;
	int i;
	(void)priv;

	memset(rsc_table->desc_vring0, 0, sizeof(struct rsc_table_desc) * rproc_priv.desc_num);
	for (i = 0; i < rproc_priv.desc_num; i++)
	{
		rsc_table->desc_vring0[i].buf_offset = rproc_priv.desc_txbuf * i;
	}

	rproc_priv.ready = 1;

	return 0;
}

static void vBindingTask(void *pvParameters)
{
	Node *head = (Node *)pvParameters;
	Node *scan;
    struct rpmsg_endpoint *ept;

	sysprintf("Start %s...\n", pcTaskGetName(NULL));

    while(1)
    {
		scan = head->next;
		/* scan ept by ept */
		while(scan)
		{
			ept = scan->rpmsg_ept;
			if(ma35_rpmsg_bind_process(ept))
			{
				// Binding success, remove from list
				remove_node(head, ept);
				if(is_empty(head))
				{
					vTaskPrioritySet(NULL, tskIDLE_PRIORITY);
					//sysprintf("%s: Binding complete.\n", pcTaskGetName(NULL));
				}
			}
			scan = scan->next;
		}
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay for 500ms
    }

	vTaskDelete(NULL);
}

int ma35_rpmsg_init_vdev(struct rpmsg_virtio_device *rvdev,
		    struct virtio_device *vdev,
		    rpmsg_ns_bind_cb ns_bind_cb,
		    struct metal_io_region *shm_io,
		    struct rpmsg_virtio_shm_pool *shpool)
{
	struct rpmsg_device *rdev;
	struct remote_resource_table *rsc_table = resource_table_shmem;
	int i;
	u64 base;

	if (!rvdev || !vdev || !shm_io)
		return RPMSG_ERR_PARAM;

	rdev = &rvdev->rdev;
	memset(rdev, 0, sizeof(*rdev));
	metal_mutex_init(&rdev->lock);
	rvdev->vdev = vdev;
	vdev->priv = rvdev;

	metal_list_init(&rdev->endpoints);

	rdev->support_ns = &(rsc_table->rpmsg_vdev).dfeatures;

	rvdev->shbuf_io = shm_io;
	rvdev->shpool = shpool;

	/* init shmem pool */
	rproc_priv.shmem_base = SHARED_RSC_TABLE;
	rproc_priv.desc_num = VRING_SIZE;
	rproc_priv.desc_txbuf = RING_TX_SIZE/rproc_priv.desc_num;
	rproc_priv.desc_rxbuf = RING_RX_SIZE/rproc_priv.desc_num;
	base = (u64)rvdev->shpool->base;
	rproc_priv.shmem_tx_base = base;
	rproc_priv.shmem_rx_base = base + RING_TX_SIZE;

	rproc_priv.buf_flag = (uint8_t *)metal_allocate_memory(rproc_priv.desc_num);
	memset(rproc_priv.buf_flag, 0, (size_t)rproc_priv.desc_num);

	rproc_priv.kick_ept = (void **)metal_allocate_memory(sizeof(void *) * rproc_priv.desc_num);
	memset(rproc_priv.kick_ept, 0, sizeof(void *) * rproc_priv.desc_num);

	// Give a head and never be used
	Node dummy;
	dummy.rpmsg_ept = dummy.next = NULL;
	rproc_priv.head_ept = &dummy;
	xTaskCreate(vBindingTask, "BindingTask", configMINIMAL_STACK_SIZE,
				rproc_priv.head_ept, tskIDLE_PRIORITY + 1, NULL);

	return 0;
}

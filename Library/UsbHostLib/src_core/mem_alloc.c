/**************************************************************************//**
 * @file    mem_alloc.c
 * @brief   USB host library memory allocation functions.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usbh_lib.h"

/// @cond HIDDEN_SYMBOLS

//#define MEM_DEBUG

#ifdef MEM_DEBUG
#define mem_debug       sysprintf
#else
#define mem_debug(...)
#endif

static uint8_t _hw_mem_pool[HW_MEM_UNIT_NUM][HW_MEM_UNIT_SIZE] __attribute__((aligned(64)));
static int _hw_unit_used[HW_MEM_UNIT_NUM];

static uint8_t _dma_mem_pool[DMA_MEM_UNIT_NUM][DMA_MEM_UNIT_SIZE] __attribute__((aligned(64)));
static int _dma_unit_used[DMA_MEM_UNIT_NUM];


UDEV_T * g_udev_list;

uint8_t  _dev_addr_pool[128];
static volatile int  _device_addr;

static  int  _sidx = 0;;

volatile int  _hw_mem_used_cnt;
volatile int  _dma_mem_used_cnt;
volatile int _ehci_qh_used, _ehci_qtd_used, _ehci_itd_used, _ehci_sitd_used;
volatile int _ohci_ed_used, _ohci_td_used;
volatile int _utr_used;

/**
  * @brief Initialize USB host library memory pool.
  * @return  None
  */
void usbh_memory_init(void)
{
	memset(_hw_unit_used, 0, sizeof(_hw_unit_used));
	_hw_mem_used_cnt = 0;

	memset(_dma_unit_used, 0, sizeof(_dma_unit_used));
	_dma_mem_used_cnt = 0;

	_sidx = 0;
	g_udev_list = NULL;
	memset(_dev_addr_pool, 0, sizeof(_dev_addr_pool));
	_device_addr = 1;

	_ehci_qh_used = _ehci_qtd_used = _ehci_itd_used = _ehci_sitd_used = 0;
	_ohci_ed_used = _ohci_td_used = 0;
	_utr_used = 0;
}

/**
  * @brief Print out memory usage of USB host library. Only for debug purpose.
  * @return  bytes count of memory allocated from USB DMA memory.
  */
uint32_t  usbh_memory_used(void)
{
	sysprintf("USB H/W memory: %d/%d, DMA memory: %d/%d\n", _hw_mem_used_cnt, HW_MEM_UNIT_NUM,
			   _dma_mem_used_cnt, DMA_MEM_UNIT_NUM);
	//sysprintf("_ehci_qh_used = %d, _ehci_qtd_used = %d, _ehci_itd_used = %d, _ehci_sited_used = %d\n",
	//		  _ehci_qh_used, _ehci_qtd_used, _ehci_itd_used, _ehci_sited_used);
	//sysprintf("_ohci_ed_used = %d, _ohci_td_used = %d\n", _ohci_ed_used, _ohci_td_used);
	//sysprintf("_utr_used = %d\n", _utr_used);
	return _dma_mem_used_cnt;
}

/*--------------------------------------------------------------------------*/
/*   Allocate memory for USB host DMA transfer buffer                       */
/*--------------------------------------------------------------------------*/

/**
  * @brief Allocate a DMA buffer from USB host library reserved DMA memory pool.
  * @param[in] size Byte count of memory block to allocate.
  * @return  Non-cache buffer pointer
  */
void *usbh_alloc_mem(int size)
{
	int  i, start;
	int  found, wanted;
	void *p;

	start = -1;
	found = 0;
	wanted = (size + DMA_MEM_UNIT_SIZE - 1) / DMA_MEM_UNIT_SIZE;

	for (i = 0; i < DMA_MEM_UNIT_NUM - wanted + 1; i++) {
		if (_dma_unit_used[i] == 0) {
			if (found == 0)
				start = i;
			found++;
			if (found >= wanted)
				break;
		} else {
			found = 0;
		}
	}

	if (found < wanted) {
		sysprintf("%s failed to allocate %d KB!!! (%d / %d)\n", __func__,
					size / 1024, _dma_mem_used_cnt, DMA_MEM_UNIT_NUM);
		return NULL;
	}

	/* Go allocate it */
	for (i = start; found > 0; i++, found--) {
		_dma_unit_used[i] = 1;
	}
	_dma_mem_used_cnt += wanted;

	// sysprintf("%s - allocate %d bytes done. block %d, (%d / %d)\n", __func__,
	//			size, start, _dma_mem_used_cnt, DMA_MEM_UNIT_NUM);

	memset(nc_ptr(&_dma_mem_pool[start]), 0, DMA_MEM_UNIT_SIZE * wanted);
	return nc_ptr(&_dma_mem_pool[start]);
}

int usbh_free_mem(void *p, int size)
{
	int i, start, wanted;
	uint64_t    paddr, base;

	paddr = addr_s(p);
	base = addr_s(&_dma_mem_pool[0]);

	if ((paddr < base) || (paddr > base + (DMA_MEM_UNIT_NUM - 1) * DMA_MEM_UNIT_SIZE)) {
		sysprintf("%s - invalid DMA address 0x%x!\n", __func__, (u32)paddr);
		return USBH_ERR_MEM_FREE_INVALID;
	}

	start = (paddr - base) / DMA_MEM_UNIT_SIZE;
	if (addr_s(&_dma_mem_pool[start]) != paddr) {
		sysprintf("%s paddr not block aligned: 0x%x\n", __func__, (u32)paddr);
		return USBH_ERR_MEM_FREE_INVALID;
	}
	wanted = (size + DMA_MEM_UNIT_SIZE - 1) / DMA_MEM_UNIT_SIZE;

	if ((paddr + wanted * DMA_MEM_UNIT_SIZE) > (base + DMA_MEM_UNIT_NUM * DMA_MEM_UNIT_SIZE)) {
		sysprintf("%s - invalid DMA address 0x%x, size %d!\n", __func__, (u32)paddr, size);
		return USBH_ERR_MEM_FREE_INVALID;
	}

	for (i = start; i < start + wanted; i++) {
		if (!_dma_unit_used[i])
			sysprintf("%s warning - try to free an unused block %d!\n", __func__, i);
		_dma_unit_used[i] = 0;
	}
	_dma_mem_used_cnt -= wanted;

	// sysprintf("%s free %d KB done. block %d, (%d / %d)\n", __func__,
	//       wanted * (DMA_MEM_UNIT_SIZE / 1024), start, _dma_mem_used_cnt, DMA_MEM_UNIT_NUM);
	return 0;
}

/*--------------------------------------------------------------------------*/
/*   USB device allocate/free                                               */
/*--------------------------------------------------------------------------*/

UDEV_T * alloc_device(void)
{
	UDEV_T  *udev;

	udev = usbh_alloc_mem(sizeof(*udev));
	if (udev == NULL) {
		USB_error("alloc_device failed!\n");
		return NULL;
	}
	memset(udev, 0, sizeof(*udev));
	udev->cur_conf = -1;                    /* must! used to identify the first SET CONFIGURATION */
	udev->next = g_udev_list;               /* chain to global device list */
	g_udev_list = udev;
	return udev;
}

void free_device(UDEV_T *udev)
{
	UDEV_T  *d;

	if (udev == NULL)
		return;

	udev->cfd_buff = ptr_s(udev->cfd_buff);
	if (udev->cfd_buff != NULL) {
		usbh_free_mem(udev->cfd_buff, MAX_DESC_BUFF_SIZE);
	}

	/*
	 *  Remove it from the global device list
	 */
	if (g_udev_list == udev) {
		g_udev_list = g_udev_list->next;
	} else {
		d = g_udev_list;
		while (d != NULL) {
			if (d->next == udev) {
				d->next = udev->next;
				break;
			}
			d = d->next;
		}
	}

	usbh_free_mem(udev, sizeof(*udev));
}

int  alloc_dev_address(void)
{
	_device_addr++;

	if (_device_addr >= 128)
		_device_addr = 1;

	while (1) {
		if (_dev_addr_pool[_device_addr] == 0) {
			_dev_addr_pool[_device_addr] = 1;
			return _device_addr;
		}
		_device_addr++;
		if (_device_addr >= 128)
			_device_addr = 1;
	}
}

void  free_dev_address(int dev_addr)
{
	if (dev_addr < 128)
		_dev_addr_pool[dev_addr] = 0;
}

/*--------------------------------------------------------------------------*/
/*   UTR (USB Transfer Request) allocate/free                               */
/*--------------------------------------------------------------------------*/
UTR_T * alloc_utr(UDEV_T *udev)
{
	UTR_T  *utr;

	utr = usbh_alloc_mem(sizeof(*utr));
	if (utr == NULL) {
		USB_error("alloc_utr failed!\n");
		return NULL;
	}
	memset(utr, 0, sizeof(*utr));
	utr->udev = udev;
	mem_debug("[ALLOC] [UTR] - 0x%x\n", (int)utr);
	_utr_used++;
	return utr;
}

void free_utr(UTR_T *utr)
{
	if (utr == NULL)
		return;

	mem_debug("[FREE] [UTR] - 0x%x\n", (int)utr);
	usbh_free_mem(utr, sizeof(*utr));
	_utr_used--;
}

/*--------------------------------------------------------------------------*/
/*   OHCI ED allocate/free                                                  */
/*--------------------------------------------------------------------------*/

ED_T * alloc_ohci_ED(void)
{
	int    i;
	ED_T   *ed;

	for (i = 0; i < HW_MEM_UNIT_NUM; i++) {
		if (_hw_unit_used[i] == 0) {
			_hw_unit_used[i] = 1;
			_hw_mem_used_cnt++;
			_ohci_ed_used++;
			ed = nc_ptr(&_hw_mem_pool[i]);
			memset(ed, 0, sizeof(*ed));
			mem_debug("[ALLOC] [ED] - 0x%x\n", (int)ed);
			return ed;
		}
	}
	USB_error("alloc_ohci_ED failed!\n");
	return NULL;
}

void free_ohci_ED(ED_T *ed)
{
	int      i;

	for (i = 0; i < HW_MEM_UNIT_NUM; i++) {
		if (ptr_to_u32(&_hw_mem_pool[i]) == ptr_to_u32(ed)) {
			mem_debug("[FREE]  [ED] - 0x%x\n", ptr_to_u32(ed));
			_hw_mem_used_cnt--;
			_ohci_ed_used--;
			_hw_unit_used[i] = 0;
			return;
		}
	}
	USB_debug("free_ohci_ED - not found! (ignored in case of multiple UTR)\n");
}

/*--------------------------------------------------------------------------*/
/*   OHCI TD allocate/free                                                  */
/*--------------------------------------------------------------------------*/
TD_T * alloc_ohci_TD(UTR_T *utr)
{
	int    i;
	TD_T   *td;

	for (i = 0; i < HW_MEM_UNIT_NUM; i++) {
		if (_hw_unit_used[i] == 0) {
			_hw_unit_used[i] = 1;
			_hw_mem_used_cnt++;
			_ohci_td_used++;
			td = nc_ptr(&_hw_mem_pool[i]);
			memset(td, 0, sizeof(*td));
			td->utr = utr;
			mem_debug("[ALLOC] [TD] - 0x%x\n", (int)td);
			return td;
		}
	}
	USB_error("alloc_ohci_TD failed!\n");
	return NULL;
}

void free_ohci_TD(TD_T *td)
{
	int   i;

	for (i = 0; i < HW_MEM_UNIT_NUM; i++) {
		if (ptr_to_u32(&_hw_mem_pool[i]) == ptr_to_u32(td)) {
			mem_debug("[FREE]  [TD] - 0x%x\n", ptr_to_u32(td));
			_ohci_td_used--;
			_hw_mem_used_cnt--;
			_hw_unit_used[i] = 0;
			return;
		}
	}
	USB_error("free_ohci_TD - not found!\n");
}

/*--------------------------------------------------------------------------*/
/*   EHCI QH allocate/free                                                  */
/*--------------------------------------------------------------------------*/
QH_T * alloc_ehci_QH(void)
{
	int    i;
	QH_T   *qh = NULL;

	for (i = (_sidx+1) % HW_MEM_UNIT_NUM; i != _sidx; i = (i+1) % HW_MEM_UNIT_NUM) {
		if (_hw_unit_used[i] == 0) {
			_hw_unit_used[i] = 1;
			_sidx = i;
			_hw_mem_used_cnt++;
			_ehci_qh_used++;
			qh = nc_ptr(&_hw_mem_pool[i]);
			memset(qh, 0, sizeof(*qh));
			mem_debug("[ALLOC] [QH] - 0x%x\n", (int)qh);
			break;
		}
	}
	if (qh == NULL) {
		USB_error("alloc_ehci_QH failed!\n");
		return NULL;
	}
	qh->Curr_qTD        = QTD_LIST_END;
	qh->OL_Next_qTD     = QTD_LIST_END;
	qh->OL_Alt_Next_qTD = QTD_LIST_END;
	qh->OL_Token        = QTD_STS_HALT;
	return qh;
}

void free_ehci_QH(QH_T *qh)
{
	int      i;

	for (i = 0; i < HW_MEM_UNIT_NUM; i++) {
		if (ptr_to_u32(&_hw_mem_pool[i]) == ptr_to_u32(qh)) {
			mem_debug("[FREE]  [QH] - 0x%x\n", ptr_to_u32(qh));
			_hw_unit_used[i] = 0;
			_hw_mem_used_cnt--;
			_ehci_qh_used--;
			return;
		}
	}
	USB_debug("free_ehci_QH - not found! (ignored in case of multiple UTR)\n");
}

/*--------------------------------------------------------------------------*/
/*   EHCI qTD allocate/free                                                 */
/*--------------------------------------------------------------------------*/
qTD_T * alloc_ehci_qTD(UTR_T *utr)
{
	int     i;
	qTD_T   *qtd;

	for (i = (_sidx+1) % HW_MEM_UNIT_NUM; i != _sidx; i = (i+1) % HW_MEM_UNIT_NUM) {
		if (_hw_unit_used[i] == 0) {
			_hw_unit_used[i] = 1;
			_sidx = i;
			_hw_mem_used_cnt++;
			_ehci_qtd_used++;
			qtd = nc_ptr(&_hw_mem_pool[i]);
			memset(qtd, 0, sizeof(*qtd));
			qtd->Next_qTD     = QTD_LIST_END;
			qtd->Alt_Next_qTD = QTD_LIST_END;
			qtd->Token        = 0x1197B7F; // QTD_STS_HALT;  visit_qtd() will not remove a qTD with this mark. It means the qTD still not ready for transfer.
			qtd->utr = utr;
			mem_debug("[ALLOC] [qTD] - 0x%x\n", (int)qtd);
			return qtd;
		}
	}
	USB_error("alloc_ehci_qTD failed!\n");
	return NULL;
}

void free_ehci_qTD(qTD_T *qtd)
{
	int   i;

	for (i = 0; i < HW_MEM_UNIT_NUM; i++) {
		if (ptr_to_u32(&_hw_mem_pool[i]) == ptr_to_u32(qtd)) {
			mem_debug("[FREE]  [qTD] - 0x%x\n", ptr_to_u32(qtd));
			_hw_unit_used[i] = 0;
			_hw_mem_used_cnt--;
			_ehci_qtd_used--;
			return;
		}
	}
	USB_error("free_ehci_qTD 0x%x - not found!\n", ptr_to_u32(qtd));
}

/*--------------------------------------------------------------------------*/
/*   EHCI iTD allocate/free                                                 */
/*--------------------------------------------------------------------------*/
iTD_T * alloc_ehci_iTD(void)
{
	int     i;
	iTD_T   *itd;

	for (i = (_sidx+1) % HW_MEM_UNIT_NUM; i != _sidx; i = (i+1) % HW_MEM_UNIT_NUM) {
		if (_hw_unit_used[i] == 0) {
			_hw_unit_used[i] = 1;
			_sidx = i;
			_hw_mem_used_cnt++;
			_ehci_itd_used++;
			itd = nc_ptr(&_hw_mem_pool[i]);
			memset(itd, 0, sizeof(*itd));
			mem_debug("[ALLOC] [iTD] - 0x%x\n", (int)itd);
			return itd;
		}
	}
	USB_error("alloc_ehci_iTD failed!\n");
	return NULL;
}

void free_ehci_iTD(iTD_T *itd)
{
	int   i;

	for (i = 0; i < HW_MEM_UNIT_NUM; i++) {
		if (ptr_to_u32(&_hw_mem_pool[i]) == ptr_to_u32(itd)) {
			mem_debug("[FREE]  [iTD] - 0x%x\n", ptr_to_u32(itd));
			_hw_unit_used[i] = 0;
			_hw_mem_used_cnt--;
			_ehci_itd_used--;
			return;
		}
	}
	USB_error("free_ehci_iTD 0x%x - not found!\n", ptr_to_u32(itd));
}

/*--------------------------------------------------------------------------*/
/*   EHCI iTD allocate/free                                                 */
/*--------------------------------------------------------------------------*/
siTD_T * alloc_ehci_siTD(void)
{
	int     i;
	siTD_T  *sitd;

	for (i = (_sidx+1) % HW_MEM_UNIT_NUM; i != _sidx; i = (i+1) % HW_MEM_UNIT_NUM) {
		if (_hw_unit_used[i] == 0) {
			_hw_unit_used[i] = 1;
			_sidx = i;
			_hw_mem_used_cnt ++;
			_ehci_sitd_used++;
			sitd = nc_ptr(&_hw_mem_pool[i]);
			memset(sitd, 0, sizeof(*sitd));
			mem_debug("[ALLOC] [siTD] - 0x%x\n", (int)sitd);
			return sitd;
		}
	}
	USB_error("alloc_ehci_siTD failed!\n");
	return NULL;
}

void free_ehci_siTD(siTD_T *sitd)
{
	int   i;

	for (i = 0; i < HW_MEM_UNIT_NUM; i++) {
		if (ptr_to_u32(&_hw_mem_pool[i]) == ptr_to_u32(sitd)) {
			mem_debug("[FREE]  [siTD] - 0x%x\n", ptr_to_u32(sitd));
			_hw_unit_used[i] = 0;
			_hw_mem_used_cnt--;
			_ehci_sitd_used--;
			return;
		}
	}
	USB_error("free_ehci_siTD 0x%x - not found!\n", ptr_to_u32(sitd));
}

/// @endcond HIDDEN_SYMBOLS


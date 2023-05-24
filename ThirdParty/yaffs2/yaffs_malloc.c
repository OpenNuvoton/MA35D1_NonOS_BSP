/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2007 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * yaffscfg.c  The configuration for the "direct" use of yaffs.
 *
 * This is set up for u-boot.
 *
 * This version now uses the ydevconfig mechanism to set up partitions.
 */

#include "stdio.h"
#include "malloc.h"

//unsigned yaffs_trace_mask = 0; /* Disable logging */

/***************************************/

#define YAFFS_MEM_ALLOC_MAGIC     0x41090908    /* magic number in leading block */
#define YAFFS_MEMORY_POOL_SIZE   (4*1024*1024)
#define YAFFS_MEM_BLOCK_SIZE      512

typedef struct YAFFS_mhdr
{
	unsigned int  flag;  /* 0:free, 1:allocated, 0x3:first block */
	unsigned int  bcnt;  /* if allocated, the block count of allocated memory block */
	unsigned int  magic;
	unsigned int  reserved;
}  YAFFS_MHDR_T;

unsigned char  _YAFFSMemoryPool[YAFFS_MEMORY_POOL_SIZE] __attribute__((aligned(YAFFS_MEM_BLOCK_SIZE)));

static YAFFS_MHDR_T  *_pCurrent;
static unsigned int  _FreeMemorySize;
unsigned int  _AllocatedMemorySize;   
unsigned int  *_YAFFS_pCurrent = (unsigned int *)&_pCurrent;

static unsigned int  _MemoryPoolBase, _MemoryPoolEnd;  

void  YAFFS_InitializeMemoryPool(void)
{
	_MemoryPoolBase = (unsigned long)&_YAFFSMemoryPool[0] | 0x100000000;

	_MemoryPoolEnd = _MemoryPoolBase + YAFFS_MEMORY_POOL_SIZE;
	_FreeMemorySize = _MemoryPoolEnd - _MemoryPoolBase;
	_AllocatedMemorySize = 0;
	_pCurrent = (YAFFS_MHDR_T *)(unsigned long)_MemoryPoolBase;
	memset((char *)(unsigned long)_MemoryPoolBase, 0, _FreeMemorySize);
}

/***************************************/
void *yaffs_malloc(size_t size)
{
// 	return malloc(size);
	YAFFS_MHDR_T  *pPrimitivePos = _pCurrent;
	YAFFS_MHDR_T  *pFound;
	int   found_size=-1;
	int   i, block_count;
	int   wrap = 0;

	if (size >= _FreeMemorySize)
	{
		printf("yaffs_malloc - want=%d, free=%d\n", size, _FreeMemorySize);
		return 0;
	}

	if ((unsigned long)_pCurrent >= _MemoryPoolEnd)
	   _pCurrent = (YAFFS_MHDR_T *)(unsigned long)_MemoryPoolBase;   /* wrapped */

	do 
	{
		if (_pCurrent->flag)          /* is not a free block */
		{
			if (_pCurrent->magic != YAFFS_MEM_ALLOC_MAGIC)
			{
				printf("\nyaffs_malloc - incorrect magic number! C:%x F:%x, wanted:%d, Base:0x%x, End:0x%x\n", 
						(unsigned long)_pCurrent, _FreeMemorySize, size, (unsigned long)_MemoryPoolBase, (unsigned long)_MemoryPoolEnd);
				return NULL;
			}

			if (_pCurrent->flag == 0x3)
				_pCurrent = (YAFFS_MHDR_T *)((unsigned long)_pCurrent + _pCurrent->bcnt * YAFFS_MEM_BLOCK_SIZE);
			else
			{
				printf("USB_malloc warning - not the first block!\n");
				_pCurrent = (YAFFS_MHDR_T *)((unsigned long)_pCurrent + YAFFS_MEM_BLOCK_SIZE);
			}
			
			if ((unsigned long)_pCurrent > _MemoryPoolEnd)
				printf("yaffs_malloc - exceed limit!!\n");

			if ((unsigned long)_pCurrent == _MemoryPoolEnd)
			{
				printf("yaffs_malloc - warp!!\n");
				wrap = 1;
				_pCurrent = (YAFFS_MHDR_T *)(unsigned long)_MemoryPoolBase;   /* wrapped */
			}
			
			found_size = -1;          /* reset the accumlator */
		}
		else                         /* is a free block */
		{
			if (found_size == -1)     /* the leading block */
			{
				pFound = _pCurrent;
				block_count = 1;
			   
				found_size = YAFFS_MEM_BLOCK_SIZE - sizeof(YAFFS_MHDR_T);
			}
			else                      /* not the leading block */
			{
				found_size += YAFFS_MEM_BLOCK_SIZE;
				block_count++;
			}
			   
			if (found_size >= size)
			{
				pFound->bcnt = block_count;
				pFound->magic = YAFFS_MEM_ALLOC_MAGIC;
				_FreeMemorySize -= block_count * YAFFS_MEM_BLOCK_SIZE;
				_AllocatedMemorySize += block_count * YAFFS_MEM_BLOCK_SIZE;
				_pCurrent = pFound;
				for (i=0; i<block_count; i++)
				{
					_pCurrent->flag = 1;     /* allocate block */
					_pCurrent = (YAFFS_MHDR_T *)((unsigned long)_pCurrent + YAFFS_MEM_BLOCK_SIZE);
				} 
				pFound->flag = 0x3;
				
				return (void *)((unsigned long)pFound + sizeof(YAFFS_MHDR_T));
			}
			 
			/* advance to the next block */
			_pCurrent = (YAFFS_MHDR_T *)((unsigned long)_pCurrent + YAFFS_MEM_BLOCK_SIZE);
			if ((unsigned long)_pCurrent >= _MemoryPoolEnd)
			{
				wrap = 1;
				_pCurrent = (YAFFS_MHDR_T *)(unsigned long)_MemoryPoolBase;   /* wrapped */
				found_size = -1;     /* reset accumlator */
			}
		}
	} while ((wrap == 0) || (_pCurrent < pPrimitivePos));
	   
	printf("yaffs_malloc - No free memory!\n");
	return 0;
}

void yaffs_free(void *ptr)
{
// 	free(ptr);
	YAFFS_MHDR_T  *pMblk;
	unsigned int  addr = (unsigned int)(unsigned long)ptr;
	int     i, count;

	if ((addr < _MemoryPoolBase) || (addr >= _MemoryPoolEnd))
	{
		if (addr)
			free(ptr);
		return;
	}

	/* get the leading block address */
	if (addr % YAFFS_MEM_BLOCK_SIZE == 0)
		addr -= YAFFS_MEM_BLOCK_SIZE;
	else
		addr -= sizeof(YAFFS_MHDR_T);
		
	if (addr % YAFFS_MEM_BLOCK_SIZE != 0)
	{
		printf("yaffs_free fatal error on address: %x!!\n", (unsigned long)ptr);
		return;
	}
	
	pMblk = (YAFFS_MHDR_T *)(unsigned long)addr;
	if (pMblk->flag == 0)
	{
		printf("yaffs_free(), warning - try to free a free block: %x\n", (unsigned long)ptr);
		return;
	}
	if (pMblk->magic != YAFFS_MEM_ALLOC_MAGIC)
	{
		printf("yaffs_free(), warning - try to free an unknow block at address:%x.\n", addr);
		return;
	}

	count = pMblk->bcnt;
	for (i = 0; i < count; i++)
	{
		pMblk->flag = 0;     /* release block */
		pMblk = (YAFFS_MHDR_T *)((unsigned long)pMblk + YAFFS_MEM_BLOCK_SIZE);
	}

	_FreeMemorySize += count * YAFFS_MEM_BLOCK_SIZE;
	_AllocatedMemorySize -= count * YAFFS_MEM_BLOCK_SIZE;
	return;
}






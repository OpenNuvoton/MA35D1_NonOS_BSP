/*************************************************************************//**
 * @file     qspi.c
 * @version  V1.00
 * @brief    baremetal loader QSPI0 source for MA35D1 MPU.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NuMicro.h"
#include "..\loader.h"

void spiCmd(unsigned char *cmd, unsigned int cmdlen, unsigned char *data, unsigned int datalen)
{
	int volatile i;

	QSPI0->SSCTL = 0x01;   // CS0 low

	for (i=0; i<cmdlen; i++)
		QSPI0->TX = cmd[i];

	// wait tx finish
	while(QSPI0->STATUS & 0x01){}
	// clear RX buffer
	QSPI0->FIFOCTL |= 0x1;
	while(QSPI0->STATUS & 0x800000);

	WDT_RESET_COUNTER(WDT1);
	// read data
	for (i=0; i<datalen; i++)
	{
		QSPI0->TX = 0x00;
		while(QSPI0->STATUS & 0x01);
		*data++ = QSPI0->RX;
	}
	QSPI0->SSCTL = 0x05;   // CS0 high
}

/*
	addr: memory address
	len: byte count
	buf: buffer to put the read back data
*/
int spiRead(unsigned int addr, unsigned int len, unsigned int *buf)
{
	int volatile i;
	unsigned int count;

	QSPI0->SSCTL = 0x01;   // CS0 low

	QSPI0->TX = CMD_READ_ARRAY_SLOW;
	QSPI0->TX = (addr >> 16) & 0xFF;
	QSPI0->TX = (addr >> 8) & 0xFF;
	QSPI0->TX = addr & 0xFF;

	// wait tx finish
	while(QSPI0->STATUS & 0x01){}
	// clear RX buffer
	QSPI0->FIFOCTL |= 0x1;
	while(QSPI0->STATUS & 0x800000);
	WDT_RESET_COUNTER(WDT1);

	// set DWIDTH to 32 bit and enable byte reorder
	QSPI0->CTL = (QSPI0->CTL & ~0x1F00) | (1<<19);

	// read data
	count = (len / 4) + 1;
	for (i=0; i<count; i++)
	{
		QSPI0->TX = 0x00;
		while(QSPI0->STATUS & 0x01);
		*buf++ = QSPI0->RX;
	}
	QSPI0->SSCTL = 0x05;   // CS0 high
	// set DWIDTH to 8 bit and disable byte reorder
	QSPI0->CTL = (QSPI0->CTL & ~0x80000) | (8<<8);

	return 0;
}


int spiNorReset()
{
	/* reset SPI flash */
	QSPI0->SSCTL = 0x01;   // CS0 low
	QSPI0->TX = 0x66;
	while(QSPI0->STATUS & 0x01);    // wait tx finish
	QSPI0->SSCTL = 0x05;   // CS0 high
	DelayMicrosecond(200);

	QSPI0->SSCTL = 0x01;   // CS0 low
	QSPI0->TX = 0x99;
	while(QSPI0->STATUS & 0x01);    // wait tx finish
	QSPI0->SSCTL = 0x05;   // CS0 high
	DelayMicrosecond(200);

	QSPI0->SSCTL = 0x01;   // CS0 low
	QSPI0->TX = 0xf0;
	while(QSPI0->STATUS & 0x01);    // wait tx finish
	QSPI0->SSCTL = 0x05;   // CS0 high
	DelayMicrosecond(200);

	return 0;

} /* end spiNorInit */

/***************************************************************/

void spiNandReset()
{
	unsigned char cmd[1];

	cmd[0] = 0xff;
	spiCmd(cmd, 1, 0, 0);
	DelayMicrosecond(1000);

	cmd[0] = 0x66;
	spiCmd(cmd, 1, 0, 0);
	DelayMicrosecond(1000);

	cmd[0] = 0x99;
	spiCmd(cmd, 1, 0, 0);
	DelayMicrosecond(1000);
}

int spiNandGetStatus(unsigned char offset)
{
	unsigned char data[2];
	unsigned char cmd[4];

	/* read status */
	cmd[0] = 0x0f;
	cmd[1] = offset;
	spiCmd(cmd, 2, data, 1);
	return data[0];
}

void spiNandSetStatus(unsigned char offset, unsigned char value)
{
	unsigned char cmd[4];

	/* write enable */
	cmd[0] = CMD_WRITE_ENABLE;
	spiCmd(cmd, 1, 0, 0);

	/* write status */
	cmd[0] = 0x1f;
	cmd[1] = offset;
	cmd[2] = value;
	spiCmd(cmd, 3, 0, 0);
}


int spiNandRead(unsigned int addr, unsigned int len, unsigned int *buf)
{
	unsigned char cmd[4];
	int volatile i, status=0;

	cmd[0] = 0x13;  /* page read */
	cmd[1] = (addr >> 16) & 0xFF;
	cmd[2] = (addr >> 8) & 0xFF;
	cmd[3] = addr & 0xFF;
	spiCmd(cmd, 4, 0, 0);
	gStartTime = raw_read_cntpct_el0();
	while(spiNandGetStatus(0xc0) & 0x1)		// wait ready
	{
		if ((raw_read_cntpct_el0() - gStartTime) > 120000) /* 10ms */
		{
			sysprintf("spiNandRead: timeout!\n");
			return 1;
		}
	}
	status = spiNandGetStatus(0xc0);
	if ((addr <= 256) && ((status & 0xf0) != 0))
	{
		sysprintf("spiNandRead: 0xc0 status: 0x%x", status);
	}
	if ((status & 0xf0) == 0xf0)
	{
		sysprintf("spiNandRead: ECC error!\n");
		return 1;
	}

	QSPI0->SSCTL = 0x01;   // CS0 low

	QSPI0->TX = 0x03;
	QSPI0->TX = 0;
	QSPI0->TX = 0;
	QSPI0->TX = 0;

	// wait tx finish
	while(QSPI0->STATUS & 0x01){}
	// clear RX buffer
	QSPI0->FIFOCTL |= 0x1;
	while(QSPI0->STATUS & 0x800000);
	WDT_RESET_COUNTER(WDT1);

	// set DWIDTH to 32 bit and enable byte reorder
	QSPI0->CTL = (QSPI0->CTL & ~0x1F00) | (1<<19);

	// read data
	for (i=0; i<len/4; i++)
	{
		QSPI0->TX = 0x00;
		while(QSPI0->STATUS & 0x01);
		*buf++ = QSPI0->RX;
	}
	QSPI0->SSCTL = 0x05;   // CS0 high
	// set DWIDTH to 8 bit and disable byte reorder
	QSPI0->CTL = (QSPI0->CTL & ~0x80000) | (8<<8);

	return 0;
}


int spiNandReadSpare(unsigned int addr, unsigned int len, unsigned char *buf)
{
	unsigned char cmd[4];

	cmd[0] = 0x13;  /* page read */
	cmd[1] = (addr >> 16) & 0xFF;
	cmd[2] = (addr >> 8) & 0xFF;
	cmd[3] = addr & 0xFF;
	spiCmd(cmd, 4, 0, 0);
	gStartTime = raw_read_cntpct_el0();
	while(spiNandGetStatus(0xc0) & 0x1)		// wait ready
	{
		if ((raw_read_cntpct_el0() - gStartTime) > 120000) /* 10ms */
		{
			sysprintf("spiNandReadSpare: timeout!\n");
			return 1;
		}
	}

	if ((spiNandGetStatus(0xc0) & 0xf0) == 0xf0)
	{
		sysprintf("spiNandReadSpare: ECC error!\n");
		return 1;
	}
	/* read data */
	cmd[0] = 0x03;  /* page read */
	cmd[1] = (SPINAND_PAGE_SIZE >> 8) & 0xff;
	cmd[2] = SPINAND_PAGE_SIZE & 0xff;
	cmd[3] = 0;
	spiCmd(cmd, 4, buf, len);
	return 0;
}


int spiNandIsBlockValid(unsigned int block)
{
	unsigned int page = block * SPINAND_PAGE_PER_BLOCK;
	unsigned char data;

	spiNandReadSpare(page, 1, &data);
	if (data != 0xff)
		return 0;   /* bad block */

	spiNandReadSpare(page+1, 1, &data);
	if (data != 0xff)
		return 0;   /* bad block */

	return 1;   /* good block */
}

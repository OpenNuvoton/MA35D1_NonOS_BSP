/*************************************************************************//**
 * @file     sd.c
 * @version  V1.00
 * @brief    baremetal loader SDH source for MA35D1 MPU.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NuMicro.h"
#include "sd.h"
#include "..\loader.h"

/*-----------------------------------------------------------------------------
 * Define some constants
 *---------------------------------------------------------------------------*/
#define SDH_CMD_MAX_TIMEOUT         3200
#define SDH_CMD_DEFAULT_TIMEOUT     100
#define SDH_MAX_DIV_SPEC_300        2046

/*---------------------------------------------------------------------------*/
static void sdh_reset(struct mmc *mmc, unsigned char mask)
{
	unsigned long timeout;

	/* Wait max 100 ms */
	timeout = 100;
	outpb((void *)(long)(mmc->base+SDH_SW_RESET), mask);
	while (inpb((void *)(long)(mmc->base+SDH_SW_RESET)) & mask)
	{
		WDT_RESET_COUNTER(WDT1);
		if (timeout == 0)
		{
			sysprintf("SD Reset fail\n");
			return;
		}
		timeout--;
		DelayMicrosecond(1000);
	}
}

static void sdh_set_clock(struct mmc *mmc, unsigned int clock)
{
	unsigned long div, clk = 0, timeout;

	/* Wait max 20 ms */
	timeout = 200;
	while (inpw((void *)(long)(mmc->base+SDH_PRESENT_STATE)) & 0x3) //(SDH_CMD_INHIBIT | SDH_DATA_INHIBIT))
	{
		if (timeout == 0)
		{
			sysprintf("Timeout to wait cmd & data inhibit\n");
			return;
		}
		timeout--;
		DelayMicrosecond(100);
	}

	outp16((void *)(long)(mmc->base+SDH_CLOCK_CONTROL), 0);

	/* Version 3.00 divisors must be a multiple of 2. */
	div = (180000000 / 2) / clock;

	clk |= (div & 0xff) << 8;
	clk |= 0x01;    //SDHCI_CLOCK_INT_EN;
	outp16((void *)(long)(mmc->base+SDH_CLOCK_CONTROL), clk);

	/* Wait max 20 ms */
	timeout = 20;
	while (!((clk = inp16((void *)(long)(mmc->base+SDH_CLOCK_CONTROL))) & 0x2)) // SDHCI_CLOCK_INT_STABLE))
	{
		if (timeout == 0)
		{
			sysprintf("Internal clock never stable\n");
			return;
		}
		timeout--;
		DelayMicrosecond(1000);
	}

	clk |= 0x4; //SDHCI_CLOCK_CARD_EN;
	outp16((void *)(long)(mmc->base+SDH_CLOCK_CONTROL), clk);
}

static void sdh_set_power(struct mmc *mmc)
{
	outpb((void *)(long)(mmc->base+SDH_POWER_CONTROL), 0xF);
}


static void sdh_cmd_done(struct mmc *mmc, struct mmc_cmd *cmd)
{
	int i;
	if (cmd->resp_type & MMC_RSP_136)
	{
		/* CRC is stripped so we need to do some shifting. */
		for (i = 0; i < 4; i++)
		{
			cmd->response[i] = inpw((void *)(long)(mmc->base + SDH_RESPONSE + (3-i)*4)) << 8;
			if (i != 3)
				cmd->response[i] |= inpb((void *)(long)(mmc->base + SDH_RESPONSE + (3-i)*4-1));
		}
	}
	else
	{
		cmd->response[0] = inpw((void *)(long)(mmc->base + SDH_RESPONSE));
	}
}

static int sdh_transfer_data(struct mmc *mmc, struct mmc_data *data)
{
	unsigned int stat, rdy, mask, timeout, block = 0;
	char transfer_done = 0;
	unsigned long start_addr;

	if (data->flags == MMC_DATA_READ)
		start_addr=(unsigned long)data->dest;
	else
		start_addr=(unsigned long)data->src;

	timeout = 1000000;
	rdy = 0x30; /* SDHCI_INT_SPACE_AVAIL | SDHCI_INT_DATA_AVAIL */
	mask = 0xc00; /* SDHCI_DATA_AVAILABLE | SDHCI_SPACE_AVAILABLE */
	do {
		WDT_RESET_COUNTER(WDT1);
		stat = inpw((void *)(long)(mmc->base+SDH_INT_STATUS));
		if (stat & 0x8000)   /* SDHCI_INT_ERROR */
			return -1;

		if (!transfer_done && (stat & (1<<3)))
		{ /* SDHCI_INT_DMA_END */
			outpw((void *)(long)(mmc->base+SDH_INT_STATUS),(1<<3)); /* SDHCI_INT_DMA_END */
			start_addr &=~(512*1024 - 1);
			start_addr += 512*1024;
			outpw((void *)(long)(mmc->base+SDH_DMA_ADDRESS),start_addr);
		}
		if (timeout-- > 0)
			DelayMicrosecond(10);
		else
			return -2;

	} while (!(stat & 0x2));    /* SDHCI_INT_DATA_END */
	return 0;
}


static int sdh_send_command(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
	unsigned int stat = 0;
	int ret = 0;
	unsigned int mask, flags, mode;
	unsigned int time = 0;
	/* Timeout unit - ms */
	unsigned int cmd_timeout = SDH_CMD_DEFAULT_TIMEOUT;

	mask = 0x3; /* SDH_CMD_INHIBIT | SDH_DATA_INHIBIT */

	/* We shouldn't wait for data inhibit for stop commands, even
	   though they might use busy signaling */
	if (cmd->cmdidx == MMC_CMD_STOP_TRANSMISSION)
		mask &= ~0x2;   /* SDH_DATA_INHIBIT */

	while (inpw((void *)(long)(mmc->base+SDH_PRESENT_STATE)) & mask)
	{
		WDT_RESET_COUNTER(WDT1);
		if (time >= cmd_timeout)
		{
			if (2 * cmd_timeout <= SDH_CMD_MAX_TIMEOUT)
			{
				cmd_timeout += cmd_timeout;
			}
			else
				return -3;
		}
		time++;
		DelayMicrosecond(1000);
	}

	outpw((void *)(long)(mmc->base+SDH_INT_STATUS), 0xffffffff);    /* SDHCI_INT_ALL_MASK */

	mask = 0x1;     /* SDHCI_INT_RESPONSE */

	if (!(cmd->resp_type & MMC_RSP_PRESENT))
		flags = SDH_CMD_RESP_NONE;
	else if (cmd->resp_type & MMC_RSP_136)
		flags = SDH_CMD_RESP_LONG;
	else if (cmd->resp_type & MMC_RSP_BUSY)
	{
		flags = SDH_CMD_RESP_SHORT_BUSY;
		if (data)
			mask |= 0x2;    /* SDHCI_INT_DATA_END */
	}
	else
		flags = SDH_CMD_RESP_SHORT;

	if (cmd->resp_type & MMC_RSP_CRC)
		flags |= SDH_CMD_CRC;
	if (cmd->resp_type & MMC_RSP_OPCODE)
		flags |= SDH_CMD_INDEX;
	if (data)
		flags |= SDH_CMD_DATA;

	/* Set Transfer mode regarding to data flag */
	if (data)
	{
		outpb((void *)(long)(mmc->base+SDH_TOUT_CONTROL), 0xe);
		mode = 0x2; /* SDHCI_TRNS_BLK_CNT_EN */
		if (data->blocks > 1)
			mode |= 0x20;   /* SDHCI_TRNS_MULTI */

		if (data->flags == MMC_DATA_READ)
		{
			mode |= 0x10;   /* SDHCI_TRNS_READ */
			outpw((void *)(long)(mmc->base+SDH_DMA_ADDRESS),(unsigned long)data->dest);
		}
		else
		{
			outpw((void *)(long)(mmc->base+SDH_DMA_ADDRESS),(unsigned long)data->src);
		}
		mode |= 0x1; /* schung: SDH_DMA */
		outpb((void *)(long)(mmc->base+SDH_HOST_CONTROL),inpb((void *)(long)(mmc->base+SDH_HOST_CONTROL))&(~0x18));

		outp16((void *)(long)(mmc->base+SDH_BLOCK_SIZE), 0x7000|(data->blocksize & 0xfff));
		outp16((void *)(long)(mmc->base+SDH_BLOCK_COUNT), data->blocks);
		outp16((void *)(long)(mmc->base+SDH_XFER_MODE), mode);
	}
	else if (cmd->resp_type & MMC_RSP_BUSY)
	{
		outpb((void *)(long)(mmc->base+SDH_TOUT_CONTROL), 0xe);
	}

	outpw((void *)(long)(mmc->base+SDH_ARGUMENT), cmd->cmdarg);
	outp16((void *)(long)(mmc->base+SDH_COMMAND), (((cmd->cmdidx & 0xff) << 8) | (flags & 0xff)));

	gStartTime = raw_read_cntpct_el0();
	do
	{
		WDT_RESET_COUNTER(WDT1);
		stat = inpw((void *)(long)(mmc->base+SDH_INT_STATUS));
		if (stat & 0x8000) /* SDHCI_INT_ERROR */
			break;

		if ((raw_read_cntpct_el0() - gStartTime) > 600000)  /* 50ms */
		{
			sysprintf("Timeout for status update!\n");
			return -2;
		}
	}
	while ((stat & mask) != mask);

	if ((stat & (0x8000 | mask)) == mask)
	{
		sdh_cmd_done(mmc, cmd);
		outpw((void *)(long)(mmc->base+SDH_INT_STATUS), mask);
	}
	else
		ret = -4;

	if (!ret && data)
		ret = sdh_transfer_data(mmc, data);

	stat = inpw((void *)(long)(mmc->base+SDH_INT_STATUS));
	outpw((void *)(long)(mmc->base+SDH_INT_STATUS), 0xffffffff);    /* SDHCI_INT_ALL_MASK */

	if (ret == 0)
		return 0;

	sdh_reset(mmc, SDH_RESET_CMD);
	sdh_reset(mmc, SDH_RESET_DATA);
	if (stat & 0x10000) /* SDH_INT_TIMEOUT */
		return -2;
	else
		return -1;
}


int sdhSelectCard(struct mmc *mmc)
{
	struct mmc_cmd cmd;

	cmd.cmdidx = MMC_CMD_SELECT_CARD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->rca;
	sdh_send_command(mmc, &cmd, 0);

	if (mmc->version != MMC_VERSION)    /* SD */
	{
		cmd.cmdidx = MMC_CMD_APP_CMD;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = mmc->rca;
		sdh_send_command(mmc, &cmd, 0);

		cmd.cmdidx = SD_CMD_APP_SET_BUS_WIDTH;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = 0x2;
		sdh_send_command(mmc, &cmd, 0);

		/* Set bus width */
		outpb((void *)(long)(mmc->base+SDH_HOST_CONTROL), inpb((void *)(long)(mmc->base+SDH_HOST_CONTROL)) | 0x2);  /* SDH_CTRL_4BITBUS */
	}
	else    /* eMMC */
	{
		cmd.cmdidx = SD_CMD_SWITCH_FUNC;
		cmd.resp_type = MMC_RSP_R1b;
		if (mmc->bus_width == 0)
		{
			//sysprintf("eMMC 4-bit\n");
			/* set CMD6 argument Access field to 3, Index to 183, Value to 1 (4-bit mode) */
			cmd.cmdarg = (3ul << 24) | (183ul << 16) | (1ul << 8);
			outpb((void *)(long)(mmc->base+SDH_HOST_CONTROL), inpb((void *)(long)(mmc->base+SDH_HOST_CONTROL)) | 0x2);  /* SDH_CTRL_4BITBUS */
		}
		else /* 8-bit */
		{
			//sysprintf("eMMC 8-bit\n");
			/* set CMD6 argument Access field to 3, Index to 183, Value to 2 (8-bit mode) */
			cmd.cmdarg = (3ul << 24) | (183ul << 16) | (2ul << 8);
			outpb((void *)(long)(mmc->base+SDH_HOST_CONTROL), inpb((void *)(long)(mmc->base+SDH_HOST_CONTROL)) | 0x20); /* SDHCI_CTRL_8BITBUS */
		}
		sdh_send_command(mmc, &cmd, 0);
	}

	/* set block length */
	cmd.cmdidx = MMC_CMD_SET_BLOCKLEN;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 512;
	sdh_send_command(mmc, &cmd, 0);

	return 0;
}

int sdhReadBlocks(struct mmc *mmc, void *dst, unsigned int start, unsigned int blkcnt)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err;

	if (blkcnt > 1)
		cmd.cmdidx = MMC_CMD_READ_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_READ_SINGLE_BLOCK;

	if (mmc->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * 512;

	cmd.resp_type = MMC_RSP_R1;

	data.dest = dst;
	data.blocks = blkcnt;
	data.blocksize = 512;
	data.flags = MMC_DATA_READ;

	err = sdh_send_command(mmc, &cmd, &data);
	if (err)
		return err;

	if (blkcnt > 1)
	{
		cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resp_type = MMC_RSP_R1b;
		err = sdh_send_command(mmc, &cmd, 0);
		if (err)
			return err;
	}
	return 0;
}

#if 1

#define SD_CMD_APP_SET_CLR_CARD_DETECT  42
static int sd_set_clear_card_detect(struct mmc *mmc)
{
	int err;
	struct mmc_cmd cmd;

	cmd.cmdidx = MMC_CMD_APP_CMD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->rca;
	//cmd.cmdarg = 0;
	err = sdh_send_command(mmc, &cmd, 0);
	if (err)
		return err;

	cmd.cmdidx = SD_CMD_APP_SET_CLR_CARD_DETECT;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0; //0 or 1
	err = sdh_send_command(mmc, &cmd, 0);
	if (err)
		return err;
	return 0;
}

static int sd_switch(struct mmc *mmc, int mode, int group, unsigned char value, unsigned char *resp)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	/* Switch the frequency */
	cmd.cmdidx = SD_CMD_SWITCH_FUNC;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = (mode << 31) | 0xffffff;
	cmd.cmdarg &= ~(0xf << (group * 4));
	cmd.cmdarg |= value << (group * 4);

	data.dest = (char *)resp;
	data.blocksize = 64;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	return sdh_send_command(mmc, &cmd, &data);
}
#endif


int sdhInit(struct mmc *mmc)
{
	struct mmc_cmd cmd;
	int err;
	int volatile timeout;

	WDT_RESET_COUNTER(WDT1);    /* reset WDT counter */

	sdh_reset(mmc, SDH_RESET_ALL);
	sdh_set_power(mmc);

	/* Enable only interrupts served by the SD controller */
	outpw((void *)(long)(mmc->base+SDH_INT_ENABLE), 0x027180FB);    /* SDHCI_INT_DATA_MASK | SDHCI_INT_CMD_MASK */
	/* Mask all sdhci interrupt sources */
	outpw((void *)(long)(mmc->base+SDH_SIGNAL_ENABLE), 0);

	/* set initial state: 1-bit bus width, normal speed */
	outpb((void *)(long)(mmc->base+SDH_HOST_CONTROL), inpb((void *)(long)(mmc->base+SDH_HOST_CONTROL)) & ~0x6);

	/* schung: set sd mode */
	outp16((void *)(long)(mmc->base+SDH_HOST_CONTROL2), inp16((void *)(long)(mmc->base+SDH_HOST_CONTROL2))|2);

	/* Set clock : initial clock 200KHz */
	/* SDH_CLOCK_CONTROL */
	sdh_set_clock(mmc, 200000);

	/* reset SD bus */
	cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
	cmd.cmdarg = 0;
	cmd.resp_type = MMC_RSP_NONE;
	sdh_send_command(mmc, &cmd, 0);

	/* timer delay */
	DelayMicrosecond(0x1000);

	mmc->rca = 0;
	// initial SDHC
	/* Test for SD version 2 */
	cmd.cmdidx = SD_CMD_SEND_IF_COND;
	/* We set the bit if the host supports voltages between 2.7 and 3.6 V */
	cmd.cmdarg = 0x100 | 0xaa;
	cmd.resp_type = MMC_RSP_R7;
	sdh_send_command(mmc, &cmd, 0);

	if ((cmd.response[0] & 0xff) == 0xaa)
	{
		/* SD 2.0 */
		mmc->version = SD_VERSION_2;

		timeout = 1000;
		while (1)
		{
			WDT_RESET_COUNTER(WDT1);
			cmd.cmdidx = MMC_CMD_APP_CMD;
			cmd.resp_type = MMC_RSP_R1;
			cmd.cmdarg = 0;
			sdh_send_command(mmc, &cmd, 0);

			cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
			cmd.resp_type = MMC_RSP_R3;
			cmd.cmdarg = 0x40ff8000;
			sdh_send_command(mmc, &cmd, 0);
			if (cmd.response[0] & 0x80000000)   /* OCR_BUSY */
				break;

			if (timeout-- <= 0)
			{
				sysprintf("SD timeout\n");
				return -1;
			}
			DelayMicrosecond(1000);
		}
		if (cmd.response[0] & 0x40000000)
			mmc->high_capacity = 0x40000000;
		else
			mmc->high_capacity = 0;
	}
	else
	{
		/* SD 1.1 */
		/* reset SD bus */
		cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
		cmd.cmdarg = 0;
		cmd.resp_type = MMC_RSP_NONE;
		sdh_send_command(mmc, &cmd, 0);

		/* timer delay */
		DelayMicrosecond(0x1000);

		cmd.cmdidx = MMC_CMD_APP_CMD;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = 0;
		if (sdh_send_command(mmc, &cmd, 0) < 0)
		{
			/* eMMC */
			/* reset SD bus */
			cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
			cmd.cmdarg = 0;
			cmd.resp_type = MMC_RSP_NONE;
			sdh_send_command(mmc, &cmd, 0);

			/* timer delay */
			DelayMicrosecond(0x1000);

			timeout = 1000;
			while (1)
			{
				WDT_RESET_COUNTER(WDT1);
				cmd.cmdidx = MMC_CMD_SEND_OP_COND;
				cmd.resp_type = MMC_RSP_R3;
				cmd.cmdarg = 0x40ff8080;
				sdh_send_command(mmc, &cmd, 0);
				if (cmd.response[0] & 0x80000000)   /* OCR_BUSY */
					break;
				if (timeout-- <= 0)
				{
					sysprintf("SD timeout\n");
					return -1;
				}
				DelayMicrosecond(1000);
			}
			mmc->high_capacity = 0x40000000;
			mmc->version = MMC_VERSION;
			mmc->rca = 0x10000;
		}
		else
		{
			cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
			cmd.resp_type = MMC_RSP_R3;
			cmd.cmdarg = 0x00ff8000;
			sdh_send_command(mmc, &cmd, 0);

			timeout = 1000;
			while (1)
			{
				WDT_RESET_COUNTER(WDT1);
				cmd.cmdidx = MMC_CMD_APP_CMD;
				cmd.resp_type = MMC_RSP_R1;
				cmd.cmdarg = 0;
				sdh_send_command(mmc, &cmd, 0);

				cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
				cmd.resp_type = MMC_RSP_R3;
				cmd.cmdarg = 0x00ff8000;
				sdh_send_command(mmc, &cmd, 0);
				if (cmd.response[0] & 0x80000000)   /* OCR_BUSY */
					break;

				if (timeout-- <= 0)
				{
					sysprintf("SD timeout\n");
					return -1;
				}
				DelayMicrosecond(1000);
			}
			mmc->version = SD_VERSION_1_0;
			mmc->high_capacity = 0;
		}
	}

	// CMD2, CMD3
	/* Put the Card in Identify Mode */
	cmd.cmdidx = MMC_CMD_ALL_SEND_CID;
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = 0;
	sdh_send_command(mmc, &cmd, 0);

	cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
	cmd.cmdarg = mmc->rca;
	cmd.resp_type = MMC_RSP_R6;
	sdh_send_command(mmc, &cmd, 0);

	if (mmc->version != MMC_VERSION)
		mmc->rca = cmd.response[0] & 0xffff0000;

	sdhSelectCard(mmc);
	if (mmc->version == SD_VERSION_2)
	{
		sd_set_clear_card_detect(mmc);
		{
			unsigned char *switch_status[64];
			err = sd_switch(mmc, 1, 0, 0, (unsigned char *)switch_status);
			if (err){
				sysprintf("sd_switch failed\n");
				return err;
			}
		}
	}
	/* 12MHz */
	sdh_set_clock(mmc, 20000000);

	return 0;
}







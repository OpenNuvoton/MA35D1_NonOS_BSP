/**************************************************************************//**
 * @file     SDH.c
 * @brief    SDH driver source file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NuMicro.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup SDH_Driver SDH Driver
  @{
*/


/** @addtogroup SDH_EXPORTED_FUNCTIONS SDH Exported Functions
  @{
*/

#define SDH_BLOCK_SIZE	512ul
#define MAX_TUNING_LOOP 16

/** @cond HIDDEN_SYMBOLS */

/* global variables */
/* For response R3 (such as ACMD41, CRC-7 is invalid; but SD controller will still */
/* calculate CRC-7 and get an error result, software should ignore this error and clear SDISR [CRC_IF] flag */
/* _sd_uR3_CMD is the flag for it. 1 means software should ignore CRC-7 error */

static uint32_t _SDH0_ReferenceClock, _SDH1_ReferenceClock;

#ifdef __ICCARM__
#pragma data_alignment = 4
static uint8_t _SDH0_ucSDHCBuffer[512];
static uint8_t _SDH1_ucSDHCBuffer[512];
#else
static uint8_t _SDH0_ucSDHCBuffer[512] __attribute__((aligned(4)));
static uint8_t _SDH1_ucSDHCBuffer[512] __attribute__((aligned(4)));
#endif

SDH_INFO_T SD0, SD1;

/*-----------------------------------------------------------------------------
 * Define some constants
 *---------------------------------------------------------------------------*/
#define SDH_CMD_MAX_TIMEOUT			3200
#define SDH_CMD_DEFAULT_TIMEOUT     100

struct mmc mmcInfo;

struct mode_width_tuning {
	enum bus_mode mode;
	uint widths;
};

int sd_modes[] = {
		UHS_SDR104,
		UHS_SDR50,
		UHS_SDR25,
		SD_HS,
		UHS_SDR12,
		SD_LEGACY
};

int mmc_modes[] = {
		MMC_HS_200,
		MMC_HS_52,
		MMC_HS,
		MMC_LEGACY
};

extern uint32_t volatile msTicks0;
void SDH_DelayMicrosecond(uint32_t ticks) {
	uint32_t tgtTicks = msTicks0 + ticks;             // target tick count to delay
	while (msTicks0 < tgtTicks);
}

const char *SDH_mode_name(enum bus_mode mode)
{
	static const char *const names[] = {
	      [MMC_LEGACY]	= "MMC legacy",
	      [SD_LEGACY]	= "SD Legacy",
	      [MMC_HS]		= "MMC High Speed (26MHz)",
	      [SD_HS]		= "SD High Speed (50MHz)",
	      [UHS_SDR12]	= "UHS SDR12 (25MHz)",
	      [UHS_SDR25]	= "UHS SDR25 (50MHz)",
	      [UHS_SDR50]	= "UHS SDR50 (100MHz)",
	      [UHS_SDR104]	= "UHS SDR104 (208MHz)",
	      [MMC_HS_52]	= "MMC High Speed (52MHz)",
		  [MMC_HS_200]	= "HS200 (200MHz)",
	};

	if (mode >= MMC_MODES_END)
		return "Unknown mode";
	else
		return names[mode];
}

int32_t SDH_reset(SDH_T *sdh, unsigned char mask)
{
    uint32_t timeout;

    /* Wait max 100 ms */
    timeout = 100;
    sdh->SW_RST_R = mask;
     
    while (sdh->SW_RST_R & mask)
    {
       
        if (timeout == 0)
        {
			sysprintf("SD Reset fail\n");
            return -1;
        }
        timeout--;
        SDH_DelayMicrosecond(1);
    }
    return 0;
}

static void SDH_set_power(SDH_T *sdh)
{
    sdh->PWR_CTRL_R = 0xF;
}

static void SDH_cmd_done(SDH_T *sdh, struct mmc_cmd *cmd)
{
    int i;
    if (cmd->resp_type & MMC_RSP_136)
    {
        /* CRC is stripped so we need to do some shifting. */
        for (i = 0; i < 4; i++)
        {
            cmd->response[i] = inpw((&sdh->RESP01_R + (3-i)*4)) << 8;
            if (i != 3)
                cmd->response[i] |= inpb((void *)(&sdh->RESP01_R + (3-i)*4-1));
        }
    }
    else
    {
        cmd->response[0] = sdh->RESP01_R;
    }
}

static int SDH_transfer_data(SDH_T *sdh, struct mmc_data *data)
{
    unsigned int stat, timeout;
    /* unsigned int rdy, mask, block = 0; */
    char transfer_done = 0;
    unsigned long start_addr;

    if (data->flags == MMC_DATA_READ)
        start_addr=(unsigned long)data->dest;
    else
        start_addr=(unsigned long)data->src;

    timeout = 1000000;
    //rdy = 0x30; /* SDHCI_INT_SPACE_AVAIL | SDHCI_INT_DATA_AVAIL */
    //mask = 0xc00; /* SDHCI_DATA_AVAILABLE | SDHCI_SPACE_AVAILABLE */
    do {
        stat = sdh->NORMAL_INT_STAT_R;
        if (stat & 0x8000) {  /* SDHCI_INT_ERROR */
			sysprintf("stat 0x%08x ret -1\n",stat);
            return -1;
        }

        if (!transfer_done && (stat & (1<<3)))
        {	/* SDHCI_INT_DMA_END */
            sdh->NORMAL_INT_STAT_R = (1<<3);
            start_addr &=~(512*1024 - 1);
            start_addr += 512*1024;
            sdh->SDMASA_R = start_addr;
        }
        if (timeout-- > 0)
            SDH_DelayMicrosecond(1);
        else
            return -2;

    } while (!(stat & 0x2));    /* SDHCI_INT_DATA_END */
    return 0;
}

int SDH_send_command(SDH_T *sdh, struct mmc_cmd *cmd, struct mmc_data *data)
{
    unsigned int stat = 0;
    int ret = 0;
    unsigned int mask, flags, mode;
    unsigned int time = 0;
    /* Timeout unit - ms */
    volatile int cmd_timeout = SDH_CMD_DEFAULT_TIMEOUT;

    mask = 0x3; /* SDH_CMD_INHIBIT | SDH_DATA_INHIBIT */
    /* We shouldn't wait for data inhibit for stop commands, even
       though they might use busy signaling */
    if (cmd->cmdidx == MMC_CMD_STOP_TRANSMISSION)
        mask &= ~0x2;   /* SDH_DATA_INHIBIT */

    while (sdh->PSTATE_REG & mask)
    {
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
        SDH_DelayMicrosecond(1);
    }

    /* SDHCI_INT_ALL_MASK */
    sdh->NORMAL_INT_STAT_R = 0xffff;
    sdh->ERROR_INT_STAT_R = 0xffff;

    mask = 0x1;     /* SDHCI_INT_RESPONSE */
    if ((cmd->cmdidx == MMC_CMD_SEND_TUNING_BLOCK ||
    		 cmd->cmdidx == MMC_CMD_SEND_TUNING_BLOCK_HS200) && !data)
    		mask = SDH_INT_DATA_AVAIL;
    if (!(cmd->resp_type & MMC_RSP_PRESENT))
        flags = SDH_CMD_RESP_NONE;
    else if (cmd->resp_type & MMC_RSP_136)
        flags = SDH_CMD_RESP_LONG;
    else if (cmd->resp_type & MMC_RSP_BUSY) {
        flags = SDH_CMD_RESP_SHORT_BUSY;
        if (data)
            mask |= 0x2;    /* SDHCI_INT_DATA_END */
    } else
        flags = SDH_CMD_RESP_SHORT;

    if (cmd->resp_type & MMC_RSP_CRC)
        flags |= SDH_CMD_CRC;
    if (cmd->resp_type & MMC_RSP_OPCODE)
        flags |= SDH_CMD_INDEX;
    if (data || cmd->cmdidx ==  MMC_CMD_SEND_TUNING_BLOCK ||
    	    cmd->cmdidx == MMC_CMD_SEND_TUNING_BLOCK_HS200)
    		flags |= SDH_CMD_DATA;

    /* Set Transfer mode regarding to data flag */
    if (data)
    {
        sdh->TOUT_CTRL_R = 0xe;
        mode = 0x2; /* SDHCI_TRNS_BLK_CNT_EN */
        if (data->blocks > 1)
            mode |= 0x20;   /* SDHCI_TRNS_MULTI */

        if (data->flags == MMC_DATA_READ)
        {
            mode |= 0x10;   /* SDHCI_TRNS_READ */
            sdh->SDMASA_R = (unsigned long)data->dest;
        }
        else
        {
            sdh->SDMASA_R = (unsigned long)data->src;
        }
        mode |= 0x1; /* Enable SDH_DMA */
        sdh->HOST_CTRL1_R &= ~0x18;
        sdh->BLOCKSIZE_R = 0x7000|(data->blocksize & 0xfff);
        sdh->BLOCKCOUNT_R = data->blocks;
        sdh->XFER_MODE_R = mode;

    } else if (cmd->resp_type & MMC_RSP_BUSY) {
        sdh->TOUT_CTRL_R = 0xe;
    }

    sdh->ARGUMENT_R = cmd->cmdarg;
    sdh->CMD_R = (((cmd->cmdidx & 0xff) << 8) | (flags & 0xff));

    cmd_timeout = SDH_CMD_DEFAULT_TIMEOUT;
    do
    {
        stat = sdh->NORMAL_INT_STAT_R;
        if (stat & 0x8000) { /* SDHCI_INT_ERROR */
            break;
        }

        SDH_DelayMicrosecond(1);
        if (cmd_timeout--<=0)  /* 50ms */
        {
			sysprintf("Timeout for status update! ret %d\n",ret);
            return -2;
        }
    }
    while ((stat & mask) != mask);

    if ((stat & (0x8000 | mask)) == mask)
    {
        SDH_cmd_done(sdh, cmd);
        sdh->NORMAL_INT_STAT_R = mask & 0xffff;
        sdh->ERROR_INT_STAT_R = ((mask & 0xffff0000)>>16);
    }
    else
        ret = -4;

    if (!ret && data)
        ret = SDH_transfer_data(sdh, data);

    stat = (sdh->ERROR_INT_STAT_R<<16)|sdh->NORMAL_INT_STAT_R;
    sdh->NORMAL_INT_STAT_R = 0xffff;
    sdh->ERROR_INT_STAT_R = 0xffff;

    if (ret == 0)
        return 0;

    SDH_reset(sdh, SDH_RESET_CMD);
    SDH_reset(sdh, SDH_RESET_DATA);
    if (stat & 0x10000) /* SDH_INT_TIMEOUT */
        return -2;
    else
        return -1;
}

int SDH_set_card_speed(SDH_T *sdh, enum bus_mode mode)
{
	int err;
    struct mmc_cmd cmd;
    struct mmc_data data;
    SDH_INFO_T *pSD;
    uint8_t switch_status[512];
    int speed;
    int group = 0;
    int retries;

    if (sdh == SDH0)
        pSD = &SD0;
    else
    	pSD = &SD1;

    if(pSD->CardType != SDH_TYPE_EMMC) {
		/* SD version 1.00 and 1.01 does not support CMD 6 */
		if (pSD->CardType == SDH_TYPE_SD_LOW)
			return 0;
		switch (mode) {
		case SD_LEGACY:
			speed = UHS_SDR12_BUS_SPEED;
			break;
		case SD_HS:
			speed = HIGH_SPEED_BUS_SPEED;
			break;
		case UHS_SDR12:
			speed = UHS_SDR12_BUS_SPEED;
			break;
		case UHS_SDR25:
			speed = UHS_SDR25_BUS_SPEED;
			break;
		case UHS_SDR50:
			speed = UHS_SDR50_BUS_SPEED;
			break;
		case UHS_DDR50:
			speed = UHS_DDR50_BUS_SPEED;
			break;
		case UHS_SDR104:
			speed = UHS_SDR104_BUS_SPEED;
			break;
		default:
			return -1;
		}
		/* Switch the frequency */
		cmd.cmdidx = SD_CMD_SWITCH_FUNC;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = (1<<31) | 0xffffff;
		cmd.cmdarg &= ~(0xf << (group * 4));
		cmd.cmdarg |= speed << (group * 4);

		data.dest = (char *)switch_status;
		data.blocksize = 64;
		data.blocks = 1;
		data.flags = MMC_DATA_READ;
		return SDH_send_command(sdh, &cmd, &data);
	} else {
			switch (mode) {
			case MMC_HS:
			case MMC_HS_52:
			case MMC_DDR_52:
				speed = EXT_CSD_TIMING_HS;
				break;
			case MMC_HS_200:
				speed = EXT_CSD_TIMING_HS200;
				break;
			case MMC_LEGACY:
				speed = EXT_CSD_TIMING_LEGACY;
				break;
			default:
				return -1;
			}

			cmd.cmdidx = MMC_CMD_SWITCH;
			cmd.resp_type = MMC_RSP_R1b;
			cmd.cmdarg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
						 (EXT_CSD_HS_TIMING << 16) |
						 (speed << 8);
			return SDH_send_command(sdh, &cmd, NULL);
    }
}

void SDH_set_timing(SDH_T *sdh, enum bus_mode mode)
{
	switch (mode) {
	case UHS_SDR50:
	case MMC_HS_52:
		sdh->HOST_CTRL2_R = (sdh->HOST_CTRL2_R & ~SDH_CTRL_UHS_MASK) | SDH_CTRL_UHS_SDR50;
		break;
	case UHS_DDR50:
	case MMC_DDR_52:
		sdh->HOST_CTRL2_R = (sdh->HOST_CTRL2_R & ~SDH_CTRL_UHS_MASK) | SDH_CTRL_UHS_DDR50;
		break;
	case UHS_SDR104:
	case MMC_HS_200:
		sdh->HOST_CTRL2_R = (sdh->HOST_CTRL2_R & ~SDH_CTRL_UHS_MASK) | SDH_CTRL_UHS_SDR104;
		sdh->HOST_CTRL2_R |= SDH_CTRL_1_8_V;
		break;
	default:
		sdh->HOST_CTRL2_R = (sdh->HOST_CTRL2_R & ~SDH_CTRL_UHS_MASK) | SDH_CTRL_UHS_SDR12;
	}
}

void SDH_set_clock(SDH_T *sdh, uint32_t clock)
{
    unsigned long div, clk = 0, timeout;
    unsigned int ReferenceClock;
    VENDOR_SPECIFIC_AREA_T * pVendor;

    if (sdh == SDH0) {
    	ReferenceClock =_SDH0_ReferenceClock = 180000000;
    	pVendor = VENDOR0;
    } else {
		ReferenceClock = _SDH1_ReferenceClock = 180000000;
		pVendor = VENDOR1;
	}

    /* Wait max 20 ms */
    timeout = 200;
    while (sdh->PSTATE_REG & 0x3) /* (SDH_CMD_INHIBIT | SDH_DATA_INHIBIT)) */
    {
        if (timeout == 0)
        {
            sysprintf("Timeout to wait cmd & data inhibit\n");
            return;
        }
        timeout--;
        SDH_DelayMicrosecond(10);
    }

    sdh->CLK_CTRL_R = 0;

	if (clock == 0)
		return;

    /* Version 3.00 divisors must be a multiple of 2. */
    div = (ReferenceClock / 2) / clock;

    clk |= (div & 0xff) << 8;
    clk |= 0x01;    /* SDHCI_CLOCK_INT_EN; */
    sdh->CLK_CTRL_R = clk;

    /* Wait max 20 ms */
    timeout = 20;
    while (!(sdh->CLK_CTRL_R & 0x2)) /* SDHCI_CLOCK_INT_STABLE */
    {
        if (timeout == 0)
        {
            sysprintf("Internal clock never stable\n");
            return;
        }
        timeout--;
        SDH_DelayMicrosecond(10);
    }

    if(clock >= SDH_CHCEK_FREQ)
    	pVendor->MSHC_CTRL_R &= ~1;
    else
    	pVendor->MSHC_CTRL_R |= 1;

    clk = sdh->CLK_CTRL_R|0x4; /* SDHCI_CLOCK_CARD_EN */
    sdh->CLK_CTRL_R = clk;
}

void SDH_dll_reset(SDH_T *sdh){
	unsigned int i,reg[20],value[20];
	reg[0]=0x2c;
	value[0]= inpw(ptr_to_u32(sdh) + reg[0]);
	reg[1]=0x4;
	value[1]= inpw(ptr_to_u32(sdh) + reg[1]);
	reg[2]=0x34;
	value[2]= inpw(ptr_to_u32(sdh) + reg[2]);
	reg[3]=0x38;
	value[3]= inpw(ptr_to_u32(sdh) + reg[3]);
	reg[4]=0x3C;
	value[4]= inpw(ptr_to_u32(sdh) + reg[4]);
	reg[5]=0x28;
	value[5]= inpw(ptr_to_u32(sdh) + reg[5]);
	reg[6]=0x2E;
	value[6]= inpb(ptr_to_u32(sdh) + reg[6]);
	SDH_Reset(sdh);

	for(i=0;i<6;i++)
		outpw(addr_s(sdh)+reg[i], value[i]);
	outpb(addr_s(sdh)+reg[6], value[6]);
	outpb(addr_s(sdh)+0x00C, 0x10);
	outpb(addr_s(sdh)+0x508, 0x00);
}

void SDH_tuning(SDH_T *sdh)
{
	struct mmc_cmd cmd;
	SDH_INFO_T *pSD;
	int i, opcode;

    if (sdh == SDH0)
        pSD = &SD0;
    else
    	pSD = &SD1;

    SDH_dll_reset(sdh);

	if(pSD->CardType != SDH_TYPE_EMMC)
		opcode = MMC_CMD_SEND_TUNING_BLOCK;
	else
		opcode = MMC_CMD_SEND_TUNING_BLOCK_HS200;

	sdh->HOST_CTRL2_R |= SDH_HOST_CTRL2_R_EXEC_TUNING_Msk;

	/*
	 * As per the Host Controller spec v3.00, tuning command
	 * generates Buffer Read Ready interrupt, so enable that.
	 *
	 * Note: The spec clearly says that when tuning sequence
	 * is being performed, the controller does not generate
	 * interrupts other than Buffer Read Ready interrupt. But
	 * to make sure we don't hit a controller bug, we _only_
	 * enable Buffer Read Ready interrupt here.
	 */
	sdh->NORMAL_INT_STAT_EN_R |= (1<<5);
	sdh->ERROR_INT_STAT_EN_R |= (1<<5);

	/*
	 * In response to CMD19, the card sends 64 bytes of tuning
	 * block to the Host Controller. So we set the block size
	 * to 64 here.
	 */
	if ((cmd.cmdidx == MMC_CMD_SEND_TUNING_BLOCK_HS200) && (pSD->busWidth == 8))
		sdh->BLOCKSIZE_R = 0x7000| (128 & 0xfff);
	else
		sdh->BLOCKSIZE_R = 0x7000| (64 & 0xfff);

	cmd.cmdidx = opcode;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;

	for(i=0; i<MAX_TUNING_LOOP; i++) {

		if(SDH_send_command(sdh, &cmd, NULL) == 0) {
			if (!(sdh->HOST_CTRL2_R & SDH_HOST_CTRL2_R_EXEC_TUNING_Msk))
				return; /* Success! */
		}
	}
	sysprintf("Tuning failed, falling back to fixed sampling clock\n");
	sdh->HOST_CTRL2_R &= ~SDH_HOST_CTRL2_R_EXEC_TUNING_Msk;
	sdh->HOST_CTRL2_R &= ~SDH_HOST_CTRL2_R_SAMPLE_CLK_SEL_Msk;
}

enum bus_mode SDH_set_mode(SDH_T *sdh, uint32_t freq)
{
	int timeout, err, i;
	SDH_INFO_T *pSD;
	struct mmc_cmd cmd;
	struct mmc_data data;
	uint8_t switch_status[512];
	uint8_t ext_csd[512];
	uint8_t scr[8];

	int freqs[] = {
	      [MMC_LEGACY]	= 25000000,
	      [SD_LEGACY]	= 25000000,
	      [MMC_HS]		= 26000000,
	      [SD_HS]		= 50000000,
	      [MMC_HS_52]	= 52000000,
	      [MMC_DDR_52]	= 52000000,
	      [UHS_SDR12]	= 25000000,
	      [UHS_SDR25]	= 50000000,
	      [UHS_SDR50]	= 100000000,
	      [UHS_DDR50]	= 50000000,
	      [UHS_SDR104]	= 200000000,
	      [MMC_HS_200]	= 200000000,
	};

    if (sdh == SDH0)
        pSD = &SD0;
    else
    	pSD = &SD1;

    if(pSD->CardType != SDH_TYPE_EMMC) {
		for(i=0;i<sizeof(sd_modes)/sizeof(int);i++)
			if(freq >= freqs[sd_modes[i]]) {
				sysprintf("trying mode %s width %d (at %d MHz)\n",
									SDH_mode_name(sd_modes[i]),
									pSD->busWidth,
									freqs[sd_modes[i]] / 1000000);

				if(SDH_set_card_speed(sdh, sd_modes[i])==0) {
					SDH_set_clock(sdh, freqs[sd_modes[i]]);
					SDH_set_timing(sdh, sd_modes[i]);
					if(freq >= 100000000 && sdh==SDH1)
						SDH_tuning(sdh);
					return sd_modes[i];
				}
			}
	} else {
		for(i=0;i<sizeof(mmc_modes)/sizeof(int);i++)
			if(freq >= freqs[mmc_modes[i]]) {
				sysprintf("trying mode %s width %d (at %d MHz)\n",
									SDH_mode_name(mmc_modes[i]),
									pSD->busWidth,
									freqs[mmc_modes[i]] / 1000000);
				if(SDH_set_card_speed(sdh, mmc_modes[i])==0) {
					SDH_set_clock(sdh, freqs[mmc_modes[i]]);
					SDH_set_timing(sdh, mmc_modes[i]);
					if(freq >= 100000000 && sdh==SDH1)
						SDH_tuning(sdh);
					return mmc_modes[i];
				}

			}
	}
    return -1;
}

int SDH_switch_voltage(SDH_T *sdh, int voltage) {
    struct mmc_cmd cmd;
    SDH_INFO_T *pSD;
    int err = 0;

    if (sdh == SDH0)
        pSD = &SD0;
    else
    	pSD = &SD1;

	if(pSD->signalVoltage == voltage)
		return 0;

	if(voltage == MMC_SIGNAL_VOLTAGE_180) {
		/*
		 * Send CMD11 only if the req uest is to switch the card to
		 * 1.8V signaling.
		 */
		cmd.cmdidx = SD_CMD_SWITCH_UHS18V;
		cmd.cmdarg = 0;
		cmd.resp_type = MMC_RSP_R1;
		err = SDH_send_command(sdh, &cmd, NULL);
		if (err)
			return err;

		SDH_set_clock(sdh, 0);

		SDH_DelayMicrosecond(5);

		GPIO_SetMode(PN,BIT11,GPIO_MODE_OUTPUT);
		PN11=1;

		SYS->MISCFCR0 |= SYS_MISCFCR0_SDH1VSTB_Msk;
		outp32((void *)(GPIOJ_BASE+0x26C),0x0000); //PJ to 1.8v

		sdh->HOST_CTRL2_R |= SDH_HOST_CTRL2_R_SIGNALING_EN_Msk;
		while((sdh->HOST_CTRL2_R & SDH_HOST_CTRL2_R_SIGNALING_EN_Msk)!=SDH_HOST_CTRL2_R_SIGNALING_EN_Msk);

		SDH_set_clock(sdh, INIT_FREQ);

		SDH_DelayMicrosecond(5);
	}

	return err;
}

int SDH_set_width(SDH_T *sdh)
{
    struct mmc_cmd cmd;
    SDH_INFO_T *pSD;

    if (sdh == SDH0)
        pSD = &SD0;
    else
    	pSD = &SD1;

    cmd.cmdidx = MMC_CMD_SELECT_CARD;
    cmd.resp_type = MMC_RSP_R1;
    cmd.cmdarg = pSD->RCA;
    SDH_send_command(sdh, &cmd, 0);

    if (pSD->CardType == SDH_TYPE_SD_HIGH || pSD->CardType == SDH_TYPE_SD_LOW)   /* SD */
    {
        cmd.cmdidx = MMC_CMD_APP_CMD;
        cmd.resp_type = MMC_RSP_R1;
        cmd.cmdarg = pSD->RCA;
        SDH_send_command(sdh, &cmd, 0);

        cmd.cmdidx = SD_CMD_APP_SET_BUS_WIDTH;
        cmd.resp_type = MMC_RSP_R1;
        cmd.cmdarg = 0x2;
        SDH_send_command(sdh, &cmd, 0);

        /* Set 4-bit bus width */
        sdh->HOST_CTRL1_R |= 0x2;

        pSD->busWidth = 4;
    }
    else    /* eMMC */
    {
        cmd.cmdidx = SD_CMD_SWITCH_FUNC;
        cmd.resp_type = MMC_RSP_R1b;
        if (sdh == SDH0) {
            sysprintf("eMMC 4-bit\n");
            /* set CMD6 argument Access field to 3, Index to 183, Value to 1 (4-bit mode) */
            cmd.cmdarg = (3ul << 24) | (183ul << 16) | (1ul << 8);
            sdh->HOST_CTRL1_R |= 0x2;
            pSD->busWidth = 4;
        } else {
			#ifndef SDH1_MMC_ENABLE_8BIT
            sysprintf("eMMC 4-bit\n");
            /* set CMD6 argument Access field to 3, Index to 183, Value to 1 (4-bit mode) */
            cmd.cmdarg = (3ul << 24) | (183ul << 16) | (1ul << 8);
            sdh->HOST_CTRL1_R |= 0x2; 
            pSD->busWidth = 4;
			#else
            sysprintf("eMMC 8-bit\n");
            /* set CMD6 argument Access field to 3, Index to 183, Value to 2 (8-bit mode) */
            cmd.cmdarg = (3ul << 24) | (183ul << 16) | (2ul << 8);

            /* Set 8-bit bus width */
            sdh->HOST_CTRL1_R = (sdh->HOST_CTRL1_R & ~0x2) | 0x20;
            pSD->busWidth = 8;
			#endif
        }
        SDH_send_command(sdh, &cmd, 0);
    }
    return 0;
    }

int SDH_read_blocks(SDH_T *sdh,struct mmc *mmc, void *dst, unsigned int start, unsigned int blkcnt)
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

    err = SDH_send_command(sdh, &cmd, &data);
    if (err)
        return err;

    if (blkcnt > 1)
    {
        cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
        cmd.cmdarg = 0;
        cmd.resp_type = MMC_RSP_R1b;
        err = SDH_send_command(sdh, &cmd, 0);
        if (err)
            return err;
    }
    return 0;
}

void SDH_Get_SD_info(SDH_T *sdh)
{
    struct mmc_cmd cmd;
    struct mmc_data data;
    unsigned int R_LEN, C_Size, MULT, size;
    SDH_INFO_T *pSD;

    if (sdh == SDH0)
        pSD = &SD0;
    else
    	pSD = &SD1;

    cmd.cmdidx = MMC_CMD_SEND_CSD;
    cmd.resp_type = MMC_RSP_R2;
    cmd.cmdarg = pSD->RCA;
    SDH_send_command(sdh, &cmd, 0);
    if ((pSD->CardType == SDH_TYPE_MMC) || (pSD->CardType == SDH_TYPE_EMMC))
    {
        /* for MMC/eMMC card */
        if ((cmd.response[0] & 0xc0000000) == 0xc0000000)
        {
            /* CSD_STRUCTURE [127:126] is 3 */
            /* CSD version depend on EXT_CSD register in eMMC v4.4 for card size > 2GB */
            cmd.cmdidx = MMC_CMD_SELECT_CARD;
            cmd.resp_type = MMC_RSP_R1;
            cmd.cmdarg = pSD->RCA;
            SDH_send_command(sdh, &cmd, 0);

            data.dest = (char *)pSD->dmabuf;
            data.blocks = 1;
            data.blocksize = MMC_MAX_BLOCK_LEN;
            data.flags = MMC_DATA_READ;
            SDH_send_command(sdh, &cmd, &data);

            if (SDH_send_command(sdh, &cmd, &data) == Successful)
            {
                cmd.cmdidx = MMC_CMD_SELECT_CARD;
                cmd.resp_type = MMC_RSP_R2;
                cmd.cmdarg = pSD->RCA;
                SDH_send_command(sdh, &cmd, 0);

                pSD->totalSectorN = (uint32_t)(*(pSD->dmabuf+215))<<24;
                pSD->totalSectorN |= (uint32_t)(*(pSD->dmabuf+214))<<16;
                pSD->totalSectorN |= (uint32_t)(*(pSD->dmabuf+213))<<8;
                pSD->totalSectorN |= (uint32_t)(*(pSD->dmabuf+212));
                pSD->diskSize = pSD->totalSectorN / 2ul;
            }
        }
        else
        {
            /* CSD version v1.0/1.1/1.2 in eMMC v4.4 spec for card size <= 2GB */
            R_LEN = (cmd.response[1] & 0x000f0000ul) >> 16;
            C_Size = ((cmd.response[1] & 0x000003fful) << 2) | ((cmd.response[2] & 0xc0000000ul) >> 30);
            MULT = (cmd.response[2] & 0x00038000ul) >> 15;
            size = (C_Size+1ul) * (1ul<<(MULT+2ul)) * (1ul<<R_LEN);

            pSD->diskSize = size / 1024ul;
            pSD->totalSectorN = size / 512ul;
        }
    }
    else
    {
        if ((cmd.response[0] & 0xc0000000) != 0x0ul)
        {
            C_Size = ((cmd.response[1] & 0x0000003ful) << 16) | ((cmd.response[2] & 0xffff0000ul) >> 16);
            size = (C_Size+1ul) * 512ul;    /* Kbytes */

            pSD->diskSize = size;
            pSD->totalSectorN = size << 1;
        }
        else
        {
            R_LEN = (cmd.response[1] & 0x000f0000ul) >> 16;
            C_Size = ((cmd.response[1] & 0x000003fful) << 2) | ((cmd.response[2] & 0xc0000000ul) >> 30);
            MULT = (cmd.response[2] & 0x00038000ul) >> 15;
            size = (C_Size+1ul) * (1ul<<(MULT+2ul)) * (1ul<<R_LEN);

            pSD->diskSize = size / 1024ul;
            pSD->totalSectorN = size / 512ul;
        }
    }
    pSD->sectorSize = (int)512;
}

int32_t SDH_Init(SDH_T *sdh)
{
    struct mmc * mmc = &mmcInfo;
    struct mmc_cmd cmd;
    int err;
    int volatile timeout;
    SDH_INFO_T *pSD;
    VENDOR_SPECIFIC_AREA_T * pVendor;

    if (sdh == SDH0) {
        pSD = &SD0;
		pVendor = VENDOR0;
    } else {
    	pSD = &SD1;
    	pVendor = VENDOR1;
    }

    pSD->busWidth = 1;
    pSD->signalVoltage = MMC_SIGNAL_VOLTAGE_330;

    SDH_reset(sdh, SDH_RESET_ALL);
    SDH_set_power(sdh);

    pVendor->MBIU_CTRL_R = (pVendor->MBIU_CTRL_R & ~0xF) | 0xC;

    /* Enable only interrupts served by the SD controller */
    sdh->NORMAL_INT_STAT_EN_R |= 0x80FB;
    sdh->ERROR_INT_STAT_EN_R |= 0x0271;

    /* set initial state: 1-bit bus width, normal speed */
    sdh->HOST_CTRL1_R = sdh->HOST_CTRL1_R & ~0x6;

    /* Set SDR50 */
    sdh->HOST_CTRL2_R = sdh->HOST_CTRL2_R | 2;

    /* Set clock : initial clock 200KHz */
    SDH_set_clock(sdh, INIT_FREQ);

    /* reset SD bus */
    cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
    cmd.cmdarg = 0;
    cmd.resp_type = MMC_RSP_NONE;
    SDH_send_command(sdh, &cmd, 0);

    /* timer delay */
    SDH_DelayMicrosecond(100);

    pSD->RCA = 0;
    /* initial SDHC */
//    pSD->R7Flag = 1ul;

    /* Test for SD version 2 */
    cmd.cmdidx = SD_CMD_SEND_IF_COND;
    /* We set the bit if the host supports voltages between 2.7 and 3.6 V */
    cmd.cmdarg = 0x100 | 0xaa;
    cmd.resp_type = MMC_RSP_R7;
    SDH_send_command(sdh, &cmd, 0);

    if ((cmd.response[0] & 0xff) == 0xaa)
    {
        /* SD 2.0 */
        mmc->version = SD_VERSION_2;
//        pSD->R7Flag = 1ul;
        timeout = 1000;
        while (1)
        {
            cmd.cmdidx = MMC_CMD_APP_CMD;
            cmd.resp_type = MMC_RSP_R1;
            cmd.cmdarg = 0;
            SDH_send_command(sdh, &cmd, 0);

            cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
            cmd.resp_type = MMC_RSP_R3;
            cmd.cmdarg = 0x41ff8000;
            SDH_send_command(sdh, &cmd, 0);
            if (cmd.response[0] & 0x80000000)   /* OCR_BUSY */
                break;

            if (timeout-- <= 0)
            {
                sysprintf("SD Tout 1\n");
                return -1;
            }
            SDH_DelayMicrosecond(10);
        }
        if (cmd.response[0] & 0x40000000) {
#ifdef SDH1_ENABLE_1_8_V
        	if (sdh == SDH1) {
				if((cmd.response[0] & 0x41000000) == 0x41000000) {
					SDH_switch_voltage(sdh, MMC_SIGNAL_VOLTAGE_180);
					pSD->signalVoltage = MMC_SIGNAL_VOLTAGE_180;
				}
        	}
#endif
            mmc->high_capacity = 0x40000000;
            pSD->CardType = SDH_TYPE_SD_HIGH;
        } else {
            mmc->high_capacity = 0;
            pSD->CardType = SDH_TYPE_SD_LOW;
        }
    }
    else
    {
        /* SD 1.1 */

        /* reset SD bus */
        cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
        cmd.cmdarg = 0;
        cmd.resp_type = MMC_RSP_NONE;
        SDH_send_command(sdh, &cmd, 0);

        /* timer delay */
        SDH_DelayMicrosecond(100);

//        pSD->R3Flag = 1ul;

        cmd.cmdidx = MMC_CMD_APP_CMD;
        cmd.resp_type = MMC_RSP_R1;
        cmd.cmdarg = 0;
        if (SDH_send_command(sdh, &cmd, 0) < 0)
        {
            /* eMMC */
            /* reset SD bus */
            cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
            cmd.cmdarg = 0;
            cmd.resp_type = MMC_RSP_NONE;
            SDH_send_command(sdh, &cmd, 0);

            /* timer delay */
            SDH_DelayMicrosecond(100);

            timeout = 3;
            while (1)
            {
                cmd.cmdidx = MMC_CMD_SEND_OP_COND;
                cmd.resp_type = MMC_RSP_R3;
                cmd.cmdarg = 0x40ff8080;
                SDH_send_command(sdh, &cmd, 0);
                if (cmd.response[0] & 0x80000000)   /* OCR_BUSY */
                    break;
                if (timeout-- <= 0)
                {
                    sysprintf("SD Tout 2\n");
                    return -1;
                }
                SDH_DelayMicrosecond(100);
            }
            mmc->high_capacity = 0x40000000;
            mmc->version = MMC_VERSION;
            pSD->RCA = 0x10000;
            pSD->CardType = SDH_TYPE_EMMC;
        }
        else
        {
            cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
            cmd.resp_type = MMC_RSP_R3;
            cmd.cmdarg = 0x00ff8000;
            SDH_send_command(sdh, &cmd, 0);

            timeout = 1000;
            while (1)
            {
                cmd.cmdidx = MMC_CMD_APP_CMD;
                cmd.resp_type = MMC_RSP_R1;
                cmd.cmdarg = 0;
                SDH_send_command(sdh, &cmd, 0);

                cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
                cmd.resp_type = MMC_RSP_R3;
                cmd.cmdarg = 0x00ff8000;
                SDH_send_command(sdh, &cmd, 0);
                if (cmd.response[0] & 0x80000000)   /* OCR_BUSY */
                    break;

                if (timeout-- <= 0)
                {
                    sysprintf("SD Tout 3\n");
                    return -1;
                }
                SDH_DelayMicrosecond(1000);
            }
            mmc->version = SD_VERSION_1_0;
            mmc->high_capacity = 0;
            pSD->CardType = SDH_TYPE_SD_LOW;
        }
    }

    /* CMD2, CMD3 */
    /* Put the Card in Identify Mode */
    cmd.cmdidx = MMC_CMD_ALL_SEND_CID;
    cmd.resp_type = MMC_RSP_R2;
    cmd.cmdarg = 0;
    SDH_send_command(sdh, &cmd, 0);

    cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
    cmd.cmdarg = pSD->RCA;
    cmd.resp_type = MMC_RSP_R6;
    SDH_send_command(sdh, &cmd, 0);

    if (mmc->version != MMC_VERSION)
    	pSD->RCA = cmd.response[0] & 0xffff0000;

    SDH_Get_SD_info(sdh);
    SDH_set_width(sdh);

    /* set block length */
    cmd.cmdidx = MMC_CMD_SET_BLOCKLEN;
    cmd.resp_type = MMC_RSP_R1;
    cmd.cmdarg = 512;
    SDH_send_command(sdh, &cmd, 0);


    if (sdh == SDH0)
    	SDH_set_mode(sdh, SDH0_FREQ);
    else
    	SDH_set_mode(sdh, SDH1_FREQ);

    return 0;
}

/** @endcond HIDDEN_SYMBOLS */


/**
 *  @brief  This function use to reset SD function and select card detection source and pin.
 *
 *  @param[in]  sdh    Select SDH0 or SDH1.
 *
 *  @return None
 */
void SDH_Open(SDH_T *sdh)
{
    if (sdh == SDH0)
    {
        IRQ_Enable((IRQn_ID_t)SDH0_IRQn);
        memset(&SD0, 0, sizeof(SDH_INFO_T));
        SD0.dmabuf = _SDH0_ucSDHCBuffer;
    } else {
        IRQ_Enable((IRQn_ID_t)SDH1_IRQn);
        memset(&SD1, 0, sizeof(SDH_INFO_T));
        SD1.dmabuf = _SDH1_ucSDHCBuffer;
    }
}

/**
 *  @brief  This function use to initial SD card.
 *
 *  @param[in]    sdh    Select SDH0 or SDH1.
 *
 *  @retval   Successful SD card initial success.
 *
 *  @details This function is used to initial SD card.
 *           SD initial state needs 400KHz clock output, driver will use HIRC for SD initial clock source.
 *           And then switch back to the user's setting.
 */
uint32_t SDH_Probe(SDH_T *sdh)
{
    return SDH_Init(sdh);
}

/**
 *  @brief  This function use to read data from SD card.
 *
 *  @param[in]     sdh           Select SDH0 or SDH1.
 *  @param[out]    pu8BufAddr    The buffer to receive the data from SD card.
 *  @param[in]     u32StartSec   The start read sector address.
 *  @param[in]     u32SecCount   The the read sector number of data
 *
 *  @retval   Successful Write data to SD card success.
 */
int SDH_Read(SDH_T *sdh, uint8_t *pu8BufAddr, uint32_t u32StartSec, uint32_t u32SecCount)
{
    struct mmc_cmd cmd;
    struct mmc_data data;
    int err;
    SDH_INFO_T *pSD;

    if (sdh == SDH0)
        pSD = &SD0;
    else
    	pSD = &SD1;

    if (u32SecCount > 1)
        cmd.cmdidx = MMC_CMD_READ_MULTIPLE_BLOCK;
    else
        cmd.cmdidx = MMC_CMD_READ_SINGLE_BLOCK;

    if ( (pSD->CardType == SDH_TYPE_SD_HIGH) || (pSD->CardType == SDH_TYPE_EMMC) )
        cmd.cmdarg = u32StartSec;
    else
        cmd.cmdarg = u32StartSec * 512;

    cmd.resp_type = MMC_RSP_R1;

    data.dest = (void *)pu8BufAddr;
    data.blocks = u32SecCount;
    data.blocksize = 512;
    data.flags = MMC_DATA_READ;

    //dcache_clean_invalidate_by_mva(pu8BufAddr,data.blocks*data.blocksize);
    dcache_clean_by_mva(pu8BufAddr,data.blocks*data.blocksize);
    err = SDH_send_command(sdh, &cmd, &data);
    if (err)
        return err;

    if (u32SecCount > 1)
    {
        cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
        cmd.cmdarg = 0;
        cmd.resp_type = MMC_RSP_R1b;
        err = SDH_send_command(sdh, &cmd, 0);
        if (err)
            return err;
    }
    dcache_invalidate_by_mva(pu8BufAddr,data.blocks*data.blocksize);
    return Successful;
}


/**
 *  @brief  This function use to write data to SD card.
 *
 *  @param[in]    sdh           Select SDH0 or SDH1.
 *  @param[in]    pu8BufAddr    The buffer to send the data to SD card.
 *  @param[in]    u32StartSec   The start write sector address.
 *  @param[in]    u32SecCount   The the write sector number of data.
 *
 *  @retval   Successful Write data to SD card success.
 */
uint32_t SDH_Write(SDH_T *sdh, uint8_t *pu8BufAddr, uint32_t u32StartSec, uint32_t u32SecCount)
{
    struct mmc_cmd cmd;
    struct mmc_data data;
    int err;
    SDH_INFO_T *pSD;

    if (sdh == SDH0)
        pSD = &SD0;
    else
    	pSD = &SD1;
    
    if (u32SecCount == 0)
        return 0;
    else if (u32SecCount == 1)
        cmd.cmdidx = MMC_CMD_WRITE_SINGLE_BLOCK;
    else
        cmd.cmdidx = MMC_CMD_WRITE_MULTIPLE_BLOCK;
    
    if ( (pSD->CardType == SDH_TYPE_SD_HIGH) || (pSD->CardType == SDH_TYPE_EMMC) )
        cmd.cmdarg = u32StartSec;
    else
        cmd.cmdarg = u32StartSec * 512;
    
    cmd.resp_type = MMC_RSP_R1;
    data.src = (void *)pu8BufAddr;
    data.blocks = u32SecCount;
    data.blocksize = 512;
    data.flags = MMC_DATA_WRITE;
    
    //dcache_clean_invalidate_by_mva(pu8BufAddr,data.blocks*data.blocksize);
    dcache_invalidate_by_mva(pu8BufAddr,data.blocks*data.blocksize);
    err = SDH_send_command(sdh, &cmd, &data);
    if (err)
        return err;
    if (u32SecCount > 1)
    {
        cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
        cmd.cmdarg = 0;
        cmd.resp_type = MMC_RSP_R1b;
        err = SDH_send_command(sdh, &cmd, 0);
        if (err)
            return err;
    }
    dcache_clean_by_mva(pu8BufAddr,data.blocks*data.blocksize);
    return Successful;
}

/**
 *  @brief  This function use to reset SD engine.
 *
 *  @param[in]    sdh           Select SDH0 or SDH1.
 *
 *  @retval   None.
 */
void SDH_Reset(SDH_T *sdh) {
	SYS_UnlockReg();
	if(sdh == SDH0) {
		SYS->IPRST0 |= (0x1<<24); //SDH0 RST;
		SYS->IPRST0 &= ~(0x1<<24); //SDH0 RST;
	}else {
		SYS->IPRST0 |= (0x1<<25); //SDH1 RST;
		SYS->IPRST0 &= ~(0x1<<25); //SDH1 RST;
	}
	SYS_LockReg();
}


/**
 *  @brief  This function use to enable SD card detection.
 *
 *  @param[in]    sdh           Select SDH0 or SDH1.
 *
 *  @retval   None.
 */
uint32_t SDH_CardDetection(SDH_T *sdh)
{
    sdh->NORMAL_INT_STAT_EN_R |= (SDH_INT_CARD_INSERT| \
    		SDH_INT_CARD_REMOVE);
    sdh->NORMAL_INT_SIGNAL_EN_R |= (SDH_INT_CARD_INSERT| \
    		SDH_INT_CARD_REMOVE);
    return 0;
}


/*@}*/ /* end of group SDH_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group SDH_Driver */

/*@}*/ /* end of group Standard_Driver */

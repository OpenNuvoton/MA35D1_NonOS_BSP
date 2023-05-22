/*************************************************************************//**
 * @file     nand.c
 * @version  V1.00
 * @brief    baremetal loader NFI source for MA35D1 MPU.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "NuMicro.h"
#include "..\loader.h"

/*-----------------------------------------------------------------------------
 * Define some constants
 *---------------------------------------------------------------------------*/
// define the total padding bytes for 512/1024 data segment
#define BCH_PADDING_LEN_512     32
#define BCH_PADDING_LEN_1024    64
// define the BCH parity code length for 512 bytes data pattern
#define BCH_PARITY_LEN_T8       15
#define BCH_PARITY_LEN_T12      23
// define the BCH parity code length for 1024 bytes data pattern
#define BCH_PARITY_LEN_T24      45

#define NAND_EXTRA_2K           64
#define NAND_EXTRA_4K           128
#define NAND_EXTRA_8K           376

const unsigned short BCH12_SPARE[3] = { 92, 184, 368};/* 2K, 4K, 8K */
const unsigned short BCH24_SPARE[3] = { 90, 180, 360};/* 2K, 4K, 8K */

NAND_INFO_T tNAND;

/*********************************************************/
int nfiWaitReady(void)
{
	gStartTime = raw_read_cntpct_el0();
	while(1)
	{
		WDT_RESET_COUNTER(WDT1);
		if (NFI->NANDINTSTS & 0x400)
		{
			NFI->NANDINTSTS = 0x400;
			return 0;
		}

		if ((raw_read_cntpct_el0() - gStartTime) > 120000)   /* 10ms */
		{
			sysprintf("R/B# timeout!\n");
			return 1;
		}
	}
}

int nfiReset(void)
{
	NFI->NANDINTSTS = 0x400;
	NFI->NANDCMD = 0xff;	/* RESET command */

	/* delay for NAND flash tWB time */
	DelayMicrosecond(100);

	if (nfiWaitReady())
		return 1;
	else
		return 0;
}


static unsigned short onfi_crc16(unsigned short crc, unsigned char const *p, unsigned int len)
{
	int i;

	WDT_RESET_COUNTER(WDT1);
	while (len--)
	{
		crc ^= *p++ << 8;
		for (i = 0; i < 8; i++)
			crc = (crc << 1) ^ ((crc & 0x8000) ? 0x8005 : 0);
	}

	return crc;
}

unsigned int nfiReadOnfi(NAND_INFO_T *pNAND)
{
	unsigned char tempID[256];
	int volatile i;
	unsigned int extendlen, parampages, offset;

	/* set READ_ID command */
	NFI->NANDCMD = 0x90;		// read ID command
	NFI->NANDADDR = 0x80000020;	// address 0x20;

	for (i=0; i<4; i++)
		tempID[i] = NFI->NANDDATA;

	if ((tempID[0] == 'O') && (tempID[1] == 'N') && (tempID[2] == 'F') && (tempID[3] == 'I'))
	{
		/* read parameter */
		NFI->NANDCMD = 0xec;
		NFI->NANDADDR = 0x80000000;
		nfiWaitReady();
		for (i=0; i<256; i++)
			tempID[i] = NFI->NANDDATA;
		if (onfi_crc16(0x4F4E, (unsigned char *)tempID, 254) == (tempID[254]|(tempID[255]<<8)))
		{
            pNAND->uPageSize = tempID[80]|(tempID[81]<<8)|(tempID[82]<<16)|(tempID[83]<<24);
            pNAND->uPagePerBlock = tempID[92]|(tempID[93]<<8)|(tempID[94]<<16)|(tempID[95]<<24);
            pNAND->uBlockPerFlash = tempID[96]|(tempID[97]<<8)|(tempID[98]<<16)|(tempID[99]<<24);
            pNAND->uBlockPerFlash = pNAND->uBlockPerFlash * tempID[100];
        	pNAND->uSpareSize = tempID[84]|(tempID[85]<<8);
            if (((tempID[101] & 0xf) >= 3) && ((tempID[101] & 0xf0) >= 0x20))
                pNAND->bIsMulticycle = 1;
            else
                pNAND->bIsMulticycle = 0;

            if (tempID[112] == 0)
            	pNAND->NandECC = 0;
            else if (tempID[112] <= 8)
            	pNAND->NandECC = NAND_BCH_T8;
            else if (tempID[112] <= 12)
            	pNAND->NandECC = NAND_BCH_T12;
            else if (tempID[112] <= 24)
            	pNAND->NandECC = NAND_BCH_T24;
            else if (tempID[112] == 0xff)
			{
				/* Read out the Extended Parameter Page */
				extendlen = (tempID[12]|(tempID[13]<<8)) * 16;
				parampages = tempID[14] * 256;
				/* read parameter */
				NFI->NANDCMD = 0xec;
				NFI->NANDADDR = 0x80000000;
				nfiWaitReady();
				NFI->NANDCMD = 0x05;
				NFI->NANDADDR = parampages & 0xFF;
				NFI->NANDADDR = ((parampages >> 8) & 0xFF) | 0x80000000; // PA8 - PA15
				NFI->NANDCMD = 0xe0;
				for (i=0; i<100; i++);
				for (i=0; i<extendlen; i++)
					tempID[i] = NFI->NANDDATA;
				if (onfi_crc16(0x4F4E, (unsigned char *)&tempID[2], extendlen-2) == (tempID[0]|(tempID[1]<<8)))
				{
					/* Find the Extended Parameter Page */
					if ((tempID[2] == 'E') && (tempID[3] == 'P') && (tempID[4] == 'P') && (tempID[5] == 'S'))
					{
						/* Search the ECC section */
						for (i=0; i<16; i+=2)     /* extend maximum section is 8 */
						{
							if (tempID[16+i] == 2)    /* get ECC section */
							{
								offset = (tempID[17+i]+1) * 16;
                                if (tempID[offset] == 0)
                                	pNAND->NandECC = 0;
                                else if (tempID[offset] <= 8)
                                	pNAND->NandECC = NAND_BCH_T8;
                                else if (tempID[offset] <= 12)
                                	pNAND->NandECC = NAND_BCH_T12;
                                else if (tempID[offset] <= 24)
                                	pNAND->NandECC = NAND_BCH_T24;
								break;
							}
						}
					}
				}
			}
			return 0;   /* OK */
		}
	}
	return 1;   /* not ONFI */
}

int nfiReadID(NAND_INFO_T *pNAND)
{
	uint32_t tempID[5], i;

	nfiReset();

	/* set READ_ID command */
	NFI->NANDCMD = 0x90;	// read ID command
	NFI->NANDADDR = 0x80000000;	// address 0x00;

	for (i=0; i<5; i++)
		tempID[i] = NFI->NANDDATA;

    pNAND->bIsCheckECC = 1;
	pNAND->NandECC = 0;

    switch (tempID[1])
    {
	case 0xf1:
	case 0xd1:
    	pNAND->uBlockPerFlash = 1023;
        pNAND->uPagePerBlock = 64;
        pNAND->bIsMulticycle = 0;
        pNAND->uPageSize = NAND_PAGE_2KB;
    	pNAND->NandECC = NAND_BCH_T8;
		break;

	case 0xda:
	case 0xdc:
	case 0xd3:  // 1024M / 4kbyte
		if ((tempID[3] & 0x33) == 0x32)
		{
            pNAND->uPagePerBlock = 128;
            pNAND->uPageSize = NAND_PAGE_4KB;
		}
        else if ((tempID[3] & 0x33) == 0x21)
		{
            pNAND->uPageSize = NAND_PAGE_2KB;
        	pNAND->NandECC = NAND_BCH_T8;
            pNAND->uPagePerBlock = 128;
		}
        else if ((tempID[3] & 0x33) == 0x22)
		{
        	pNAND->NandECC = NAND_BCH_T8;
            pNAND->uPagePerBlock = 64;
            pNAND->uPageSize = NAND_PAGE_4KB;
		}
        else if ((tempID[3] & 0x33) == 0x11)
        {
            pNAND->uPageSize = NAND_PAGE_2KB;
        	pNAND->NandECC = NAND_BCH_T8;
            pNAND->uPagePerBlock = 64;
        }
        pNAND->bIsMulticycle = 1;
		break;

	case 0xd5:  // 2048M / 2kbyte
        if ((tempID[0]==0xAD)&&(tempID[2]==0x94)&&(tempID[3]==0x25))
		{
            pNAND->uPagePerBlock   = 128;
            pNAND->uPageSize       = NAND_PAGE_4KB;
        	pNAND->NandECC = NAND_BCH_T12;
		}
        else if ((tempID[0]==0xAD)&&(tempID[2]==0x94)&&(tempID[3]==0x9A))
		{
            pNAND->uPagePerBlock   = 256;
            pNAND->uPageSize       = NAND_PAGE_8KB;
        	pNAND->NandECC = NAND_BCH_T24;
		}
        else if ((tempID[0]==0x98)&&(tempID[2]==0x94)&&(tempID[3]==0x32))
		{
            pNAND->uPagePerBlock   = 128;
            pNAND->uPageSize       = NAND_PAGE_8KB;
        	pNAND->NandECC = NAND_BCH_T24;
		}
		else
		{
            if ((tempID[3] & 0x33) == 0x32)
			{
                pNAND->uPagePerBlock = 128;
                pNAND->uPageSize = NAND_PAGE_4KB;
            	pNAND->NandECC = NAND_BCH_T8;
			}
            else if ((tempID[3] & 0x33) == 0x11)
			{
                pNAND->uPagePerBlock = 64;
                pNAND->uPageSize = NAND_PAGE_2KB;
            	pNAND->NandECC = NAND_BCH_T8;
			}
            else if ((tempID[3] & 0x33) == 0x21)
			{
            	pNAND->NandECC = NAND_BCH_T8;
                pNAND->uPagePerBlock = 128;
                pNAND->uPageSize = NAND_PAGE_2KB;
			}
			else if ((tempID[3] & 0x3) == 0x3)
			{
            	pNAND->NandECC = NAND_BCH_T12;
                pNAND->uPagePerBlock = 128;
                pNAND->uPageSize = NAND_PAGE_8KB;
			}
		}
        pNAND->bIsMulticycle = 1;
		break;

	default:
		return 1;
    }
}

int nfiOpen(void)
{
	uint32_t volatile uPowerOn;

	// reset NFI
	SYS->IPRST0 |= SYS_IPRST0_NANDRST_Msk;
	SYS->IPRST0 &= ~SYS_IPRST0_NANDRST_Msk;
	NFI->DMACTL |= NFI_DMACTL_DMACEN_Msk;
	NFI->DMACTL |= (NFI_DMACTL_DMACEN_Msk | NFI_DMACTL_DMARST_Msk);
	while(NFI->DMACTL & NFI_DMACTL_DMARST_Msk);
	NFI->GCTL = NFI_GCTL_NANDEN_Msk;

	/* enable CS0, disable CS1 */
	NFI->NANDCTL = (NFI->NANDCTL & ~0x02000000) | 0x04000000;
	NFI->NANDCTL |= (NFI_NANDCTL_PROT3BEN_Msk | NFI_NANDCTL_ECCCHK_Msk | NFI_NANDCTL_REDUNAUTOWEN_Msk);

	tNAND.uBlockPerFlash = 0;
	tNAND.uPagePerBlock = 0;
	tNAND.uPageSize = 0;
	tNAND.uSpareSize = 0;
	tNAND.NandECC = 0;
	tNAND.bIsMulticycle = 0;
	tNAND.bIsMLCNand = 0;
	tNAND.bIsCheckECC = 0;

	/* ONFI */
	if (nfiReadOnfi(&tNAND))
	{
		if (nfiReadID(&tNAND))
		{
			sysprintf("Not support ID !!\n");
			return 1;
		}
		else
		{
			sysprintf("page size %d, page/block %d\n", tNAND.uPageSize, tNAND.uPagePerBlock);
		}
	}

	/* check power-on-setting */
	uPowerOn = SYS->PWRONOTP;
	if ((uPowerOn & 0x1) == 0)
		uPowerOn = (uPowerOn & ~0xfd00) | (SYS->PWRONPIN << 8);

	if ((uPowerOn & 0x00003000) != 0)    /* page */
	{
		tNAND.uPageSize = 1024 << ((uPowerOn >> 12) & 0x3);

		switch(tNAND.uPageSize)
		{
			case NAND_PAGE_2KB:
				sysprintf("2KB\n");
				tNAND.uPagePerBlock = 64;
	        	tNAND.NandECC = NAND_BCH_T8;
				break;
			case NAND_PAGE_4KB:
				sysprintf("4KB\n");
				tNAND.uPagePerBlock = 128;
	        	tNAND.NandECC = NAND_BCH_T8;
				break;
			case NAND_PAGE_8KB:
				sysprintf("8KB\n");
				tNAND.uPagePerBlock = 128;
	        	tNAND.NandECC = NAND_BCH_T12;
				break;
		}
	}

	if ((uPowerOn & 0x0000C000) != 0)   /* BCH */
	{
		switch((uPowerOn & 0x0000C000))
		{
			case 0xc000:
				sysprintf("NoECC\n");
	        	tNAND.NandECC = 0;
				tNAND.uSpareSize = 8;
				break;
			case 0x4000:
				sysprintf("T12\n");
	        	tNAND.NandECC = NAND_BCH_T12;
				tNAND.uSpareSize = BCH12_SPARE[tNAND.uPageSize >> 12] + 8;
				break;
			case 0x8000:
				sysprintf("T24\n");
	        	tNAND.NandECC = NAND_BCH_T24;
				tNAND.uSpareSize = BCH24_SPARE[tNAND.uPageSize >> 12] + 8;
				break;
		}
	}
	else
	{
		if (tNAND.uSpareSize == 0)
		{
			switch(tNAND.uPageSize)
			{
				case NAND_PAGE_2KB:
					tNAND.uSpareSize = NAND_EXTRA_2K;
					break;
				case NAND_PAGE_4KB:
					tNAND.uSpareSize = NAND_EXTRA_4K;
					break;
				case NAND_PAGE_8KB:
					tNAND.uSpareSize = NAND_EXTRA_8K;
					break;
				default:
					;
			}
		}
	}

	if (tNAND.NandECC != 0)
	{
		NFI->NANDCTL |= (NFI_NANDCTL_ECCEN_Msk | NFI_NANDCTL_ECCCHK_Msk);	// enable ECC
		tNAND.bIsCheckECC = 1;
	}
	else
	{
		NFI->NANDCTL &= ~(NFI_NANDCTL_ECCEN_Msk | NFI_NANDCTL_ECCCHK_Msk);	// disable ECC
		tNAND.bIsCheckECC = 0;
	}

    //--- Set register to disable Mask ECC feature
	NFI->NANDRACTL &= ~0xffff0000;
	NFI->NANDCTL &= ~0x7c0000;
	NFI->NANDCTL |= tNAND.NandECC;

	if (tNAND.uPageSize == NAND_PAGE_8KB)
		NFI->NANDCTL = (NFI->NANDCTL  & ~0x30000) | 0x30000;
	else if (tNAND.uPageSize == NAND_PAGE_4KB)
		NFI->NANDCTL = (NFI->NANDCTL  & ~0x30000) | 0x20000;
	else
		NFI->NANDCTL = (NFI->NANDCTL  & ~0x30000) | 0x10000;
	NFI->NANDRACTL = (NFI->NANDRACTL & ~0x1ff) | tNAND.uSpareSize;

	return 0;
}


int nfiReadRA(NAND_INFO_T *pNAND, uint32_t uPage, uint32_t ucColAddr)
{
	while(!(NFI->NANDINTSTS & 0x40000));
	NFI->NANDINTSTS = 0x400;

	NFI->NANDCMD = 0x00;		// read command
	NFI->NANDADDR = ucColAddr;	                // CA0 - CA7
	NFI->NANDADDR = (ucColAddr >> 8) & 0xFF;	// CA8 - CA11
	NFI->NANDADDR = uPage & 0xff;	            // PA0 - PA7
	if (!pNAND->bIsMulticycle)
		NFI->NANDADDR = ((uPage >> 8) & 0xff)|0x80000000;    // PA8 - PA15
	else
	{
		NFI->NANDADDR = (uPage >> 8) & 0xff;		        // PA8 - PA15
		NFI->NANDADDR = ((uPage >> 16) & 0xff)|0x80000000;   // PA16 - PA18
	}
	NFI->NANDCMD = 0x30;		// read command

	if (nfiWaitReady() < 0)
		return -1;
    return 0;
}

static void nfiCorrectDataBCH(unsigned char ucFieidIndex, unsigned char ucErrorCnt, unsigned char* pDAddr)
{
	unsigned int uaData[24], uaAddr[24];
	unsigned int uaErrorData[6];
	unsigned char i, j;
	unsigned int uPageSize;
	unsigned int field_len, padding_len, parity_len;
	unsigned int total_field_num;
	unsigned char *smra_index;

	//--- assign some parameters for different BCH and page size
	field_len   = 512;
	padding_len = BCH_PADDING_LEN_512;

	switch (NFI->NANDCTL & 0x7c0000)
	{
		case NAND_BCH_T24:
			field_len   = 1024;
			padding_len = BCH_PADDING_LEN_1024;
			parity_len  = BCH_PARITY_LEN_T24;
			break;
		case NAND_BCH_T12:
			parity_len  = BCH_PARITY_LEN_T12;
			break;
		case NAND_BCH_T8:
			parity_len  = BCH_PARITY_LEN_T8;
			break;
		default:
			return;
	}

	uPageSize = NFI->NANDCTL & 0x30000;
	switch (uPageSize)
	{
		case 0x30000:  total_field_num = 8192 / field_len; break;
		case 0x20000:  total_field_num = 4096 / field_len; break;
		case 0x10000:  total_field_num = 2048 / field_len; break;
		default:
			return;
	}

	//--- got valid BCH_ECC_DATAx and parse them to uaData[]
	// got the valid register number of BCH_ECC_DATAx since one register include 4 error bytes
	j = ucErrorCnt/4;
	j++;
	if (j > 6)
		j = 6;     // there are 6 BCH_ECC_DATAx registers to support BCH T24

	for(i=0; i<j; i++)
	{
		uaErrorData[i] = NFI->NANDECCED[i];
	}

	for(i=0; i<j; i++)
	{
		uaData[i*4+0] = uaErrorData[i] & 0xff;
		uaData[i*4+1] = (uaErrorData[i]>>8) & 0xff;
		uaData[i*4+2] = (uaErrorData[i]>>16) & 0xff;
		uaData[i*4+3] = (uaErrorData[i]>>24) & 0xff;
	}

	//--- got valid REG_BCH_ECC_ADDRx and parse them to uaAddr[]
	// got the valid register number of REG_BCH_ECC_ADDRx since one register include 2 error addresses
	j = ucErrorCnt/2;
	j++;
	if (j > 12)
		j = 12;    // there are 12 REG_BCH_ECC_ADDRx registers to support BCH T24

	for(i=0; i<j; i++)
	{
	    uaAddr[i*2+0] = NFI->NANDECCEA[i] & 0x07ff;   // 11 bits for error address
	    uaAddr[i*2+1] = (NFI->NANDECCEA[i] >> 16) & 0x07ff;
	}

	//--- pointer to begin address of field that with data error
	pDAddr += (ucFieidIndex-1) * field_len;

	//--- correct each error bytes
	for(i=0; i<ucErrorCnt; i++)
	{
		// for wrong data in field
		if (uaAddr[i] < field_len)
		{
			*(pDAddr+uaAddr[i]) ^= uaData[i];
		}
		// for wrong first-3-bytes in redundancy area
		else if (uaAddr[i] < (field_len+3))
		{
			uaAddr[i] -= field_len;
			uaAddr[i] += (parity_len*(ucFieidIndex-1));    // field offset

  	        *((uint8_t *)(u64)(&NFI->NANDRA[0])+uaAddr[i]) ^= uaData[i];
		}
		// for wrong parity code in redundancy area
		else
		{
			// BCH_ERR_ADDRx = [data in field] + [3 bytes] + [xx] + [parity code]
			//                                   |<--     padding bytes      -->|
			// The BCH_ERR_ADDRx for last parity code always = field size + padding size.
			// So, the first parity code = field size + padding size - parity code length.
			// For example, for BCH T12, the first parity code = 512 + 32 - 23 = 521.
			// That is, error byte address offset within field is
			uaAddr[i] = uaAddr[i] - (field_len + padding_len - parity_len);

			// smra_index point to the first parity code of first field in register SMRA0~n
            smra_index = (uint8_t *)
                         ((u64)(&NFI->NANDRA[0]) + (NFI->NANDRACTL & 0x1ff) - // bottom of all parity code -
                          (parity_len * total_field_num)                            // byte count of all parity code
                         );

			// final address = first parity code of first field +
			//                 offset of fields +
			//                 offset within field
			*((unsigned char *)smra_index + (parity_len * (ucFieidIndex-1)) + uaAddr[i]) ^= uaData[i];
		}
	}   // end of for (i<ucErrorCnt)
}

int nfiReadDataEccCheck(long addr)
{
	uint32_t uStatus;
	uint32_t uErrorCnt, i, j;
	uint32_t volatile uError = 0;
	uint32_t uLoop;

    //--- uLoop is the number of SM_ECC_STx should be check.
    //      One SM_ECC_STx include ECC status for 4 fields.
    //      Field size is 1024 bytes for BCH_T24 and 512 bytes for other BCH.
    switch (NFI->NANDCTL & 0x30000)	/* page size */
    {
        case 0x10000:
            uLoop = 1;
            break;
        case 0x20000:
            if ((NFI->NANDCTL & 0x7c0000) == NAND_BCH_T24)
                uLoop = 1;
            else
                uLoop = 2;
            break;
        case 0x30000:
            if ((NFI->NANDCTL & 0x7c0000) == NAND_BCH_T24)
                uLoop = 2;
            else
                uLoop = 4;
            break;
        default:
            return 1;     // don't work for 512 bytes page
    }

    NFI->DMASA = addr;
    NFI->NANDINTSTS = (NFI_NANDINTSTS_DMAIF_Msk | NFI_NANDINTSTS_ECCFLDIF_Msk);
    NFI->NANDCTL |= NFI_NANDCTL_DRDEN_Msk;

    //--- waiting for DMA transfer stop since complete or ECC error
    // IF no ECC error, DMA transfer complete and make SMCR[DRD_EN]=0
    // IF ECC error, DMA transfer suspend     and make SMISR[ECC_FIELD_IF]=1 but keep keep SMCR[DRD_EN]=1
    //      If we clear SMISR[ECC_FIELD_IF] to 0, DMA transfer will resume.
    // So, we should keep wait if DMA not complete (SMCR[DRD_EN]=1) and no ERR error (SMISR[ECC_FIELD_IF]=0)
	while ((NFI->NANDCTL & 0x2) && ((NFI->NANDINTSTS & 0x4) == 0))
	{}

    //--- DMA transfer completed or suspend by ECC error, check and correct ECC error
	if ((tNAND.bIsCheckECC) || (NFI->NANDCTL & 0x80))
	{
		while(1)
		{
			if (NFI->NANDINTSTS & 0x4)
			{
				for (j=0; j<uLoop; j++)
				{
		            uStatus = NFI->NANDECCES[j];
		            if (!uStatus)
		            	continue;   // no error on this register for 4 fields
                    // ECC error !! Check 4 fields. Each field has 512 bytes data
		        	for (i=1; i<5; i++)
		        	{
			            if (!(uStatus & 0x3))     // no error for this field
			            {
							uStatus >>= 8;  // next field
			            	continue;
						}

			    		if ((uStatus & 0x3) == 0x01)  // correctable error in field (j*4+i)
			            {
			                uErrorCnt = (uStatus >> 2) & 0x1F;
			                nfiCorrectDataBCH(j*4+i, uErrorCnt, (uint8_t *)addr);
							sysprintf("Warning: Field %d have %d BCH error. Corrected!!\n", j*4+i, uErrorCnt);
			                break;
			            }
			    		else if (((uStatus & 0x3) == 0x02) ||
			    		         ((uStatus & 0x3) == 0x03)) // uncorrectable error or ECC error in 1st field
			            {
			        	    sysprintf("ERROR: Field %d encountered uncorrectable BCH error!! 0x%x\n", j*4+i, uStatus);
				        	uError = 1;
			                break;
			            }
						uStatus >>= 8;  // next field
					}
				}
				NFI->NANDINTSTS = 0x4;	   	// clear ECC_FIELD_IF to resume DMA transfer
			}

			if (NFI->NANDINTSTS & 0x1)      // wait to finish DMAC transfer.
			{
				if ( !(NFI->NANDINTSTS & 0x4) )
					break;
			}
		}   // end of while(1)
	}
	//--- Don't check ECC. Just wait the DMA finish.
	else
	{
		while(1)
		{
			NFI->NANDINTSTS = 0x4;
			if (NFI->NANDINTSTS & 0x1)
			{
				NFI->NANDINTSTS = 0x1;	                // clear DMA flag
				break;
			}
		}   // end of while(1)
	}

    if (uError)
   		return 1;
    else
	    return 0;
}


int nfiPageRead(int page, uint8_t *buff)
{
	int volatile i;
	unsigned char *ptr;
	int spareSize;
	unsigned int reg;

    //--- read redundancy area to register NANDRAx
	spareSize = NFI->NANDRACTL & 0x1ff;
	ptr = (unsigned char *)NFI->NANDRA;
	nfiReadRA(&tNAND, page, tNAND.uPageSize);
	for (i=0; i<spareSize; i++)
		*ptr++ = NFI->NANDDATA & 0xff;		// copy RA data from NAND to NANDRA by SW

	if ((NFI->NANDRA[0] & 0xffff0000) != 0)
		return 1;	/* empty page */

	while(!(NFI->NANDINTSTS & 0x40000));
	NFI->NANDINTSTS = 0x400;

	NFI->NANDCMD = 0x00;
	NFI->NANDADDR = 0;	        // CA0 - CA7
	NFI->NANDADDR = 0;			// CA8 - CA12
	NFI->NANDADDR = page & 0xff;	// PA0 - PA7
	if (!tNAND.bIsMulticycle)
		NFI->NANDADDR = ((page >> 8) & 0xff)|0x80000000;  // PA8 - PA15
	else
	{
		NFI->NANDADDR = (page >> 8) & 0xff;				// PA8 - PA15
		NFI->NANDADDR = ((page >> 16) & 0xff)|0x80000000; // PA16 - PA17
	}
	NFI->NANDCMD = 0x30;		// read command

	while(1)
	{
		if (NFI->NANDINTSTS & 0x400)
		{
			while(!(NFI->NANDINTSTS & 0x40000));
			NFI->NANDINTSTS = 0x400;
			break;
		}
	}

	if (nfiReadDataEccCheck((long)buff))
	{
		sysprintf("read page error!\n");
	}

	return 0;
}



int nfiIsBlockValid(NAND_INFO_T *pNAND, uint32_t pba)
{
	unsigned int page, data=0xff;

	/* check first 2 page and last 2 page */
	page = pba * pNAND->uPagePerBlock;
	nfiReadRA(pNAND, page, pNAND->uPageSize);
	data = NFI->NANDDATA & 0xff;
	if (data != 0xFF)
		return 0;   /* invalid */

	nfiReadRA(pNAND, page+1, pNAND->uPageSize);
	data = NFI->NANDDATA & 0xff;
	if (data != 0xFF)
		return 0;   /* invalid */

	nfiReadRA(pNAND, page+pNAND->uPagePerBlock-1, pNAND->uPageSize);
	data = NFI->NANDDATA & 0xff;
	if (data != 0xFF)
		return 0;   /* invalid */

	nfiReadRA(pNAND, page+pNAND->uPagePerBlock-2, pNAND->uPageSize);
	data = NFI->NANDDATA & 0xff;
	if (data != 0xFF)
		return 0;   /* invalid */

	nfiReset();

	return 1;   /* good block */
}




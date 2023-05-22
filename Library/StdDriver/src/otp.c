/*************************************************************************//**
 * @file     otp.c
 * @version  V1.00
 * @brief    MA35D1 OTP driver.
 *
 * @copyright (C) 2021 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "MA35D1.h"
#include "otp.h"


static int  otp_check_busy()
{
	volatile int  wait = 0x10000000;
	
	while (--wait > 0) {
		if ((OTP->OTP_STS & OTP_STS_BUSY_Msk) == 0)
			return 0;
	}
	return -1;
}

int OTP_Program(uint32_t u32Addr, uint32_t u32Data)
{
	OTP->OTP_ADDR = u32Addr;
	OTP->OTP_DATA = u32Data;
	OTP->OTP_CTL = OTP_CMD_PROGRAM;
	OTP->OTP_CTL |= OTP_CTL_START_Msk;
	
	if (otp_check_busy() != 0)
		return -1;	/* OTP busy */

	if (OTP->OTP_STS & (OTP_STS_PFF_Msk | OTP_STS_ADDRFF_Msk | OTP_STS_CMDFF_Msk)) {
		printf("OTP program failed, status = 0x%x\n", OTP->OTP_STS);
		return -2;
	}
	return 0;	// success
}

int OTP_Read(uint32_t u32Addr, uint32_t *u32Data)
{
	OTP->OTP_ADDR = u32Addr;
	OTP->OTP_CTL = OTP_CMD_READ;
	OTP->OTP_CTL |= OTP_CTL_START_Msk;
	
	if (otp_check_busy() != 0)
		return -1;	/* OTP busy */

	if (OTP->OTP_STS & (OTP_STS_ADDRFF_Msk | OTP_STS_CMDFF_Msk)) {
		printf("OTP read failed, status = 0x%x\n", OTP->OTP_STS);
		return -2;
	}
	*u32Data = OTP->OTP_DATA;
	return 0;	// success
}

int OTP_Read_Lock(uint32_t u32Addr, uint32_t u32Password)
{
	OTP->OTP_ADDR = u32Addr;
	OTP->OTP_DATA = u32Password;
	OTP->OTP_CTL = OTP_CMD_READ_ONLY;
	OTP->OTP_CTL |= OTP_CTL_START_Msk;
	
	if (otp_check_busy() != 0)
		return -1;	/* OTP busy */

	if (OTP->OTP_STS & (OTP_STS_PFF_Msk | OTP_STS_ADDRFF_Msk)) {
		printf("OTP read lock failed, status = 0x%x\n", OTP->OTP_STS);
		return -2;
	}
	return 0;	// success
}

int OTP_Erase(uint32_t u32BlkAddr, uint32_t u32Password)
{
	OTP->OTP_ADDR = u32BlkAddr;
	OTP->OTP_DATA = u32Password;
	OTP->OTP_CTL = OTP_CMD_FTM;
	OTP->OTP_CTL |= OTP_CTL_START_Msk;
	
	if (otp_check_busy() != 0)
		return -1;	/* OTP busy */

	if (OTP->OTP_STS & (OTP_STS_PFF_Msk | OTP_STS_ADDRFF_Msk |
			OTP_STS_FTMFF_Msk | OTP_STS_CMDFF_Msk)) {
		printf("OTP erase failed, status = 0x%x\n", OTP->OTP_STS);
		return -2;
	}
	return 0;	// success
}

int OTP_Read_Checker(uint32_t *u32Data)
{
	OTP->OTP_CTL = OTP_CMD_READ_CHKER;
	OTP->OTP_CTL |= OTP_CTL_START_Msk;
	
	if (otp_check_busy() != 0)
		return -1;	/* OTP busy */

	if (OTP->OTP_STS & OTP_STS_CMDFF_Msk) {
		printf("OTP read checker, status = 0x%x\n", OTP->OTP_STS);
		return -2;
	}
	*u32Data = OTP->OTP_DATA;
	return 0;	// success
}

int OTP_Read_CID(uint32_t *u32Data)
{
	OTP->OTP_CTL = OTP_CMD_READ_CID;
	OTP->OTP_CTL |= OTP_CTL_START_Msk;
	
	if (otp_check_busy() != 0)
		return -1;	/* OTP busy */

	if (OTP->OTP_STS & OTP_STS_CMDFF_Msk) {
		printf("OTP read CID, status = 0x%x\n", OTP->OTP_STS);
		return -2;
	}
	*u32Data = OTP->OTP_DATA;
	return 0;	// success
}

int OTP_Read_DID(uint32_t *u32Data)
{
	OTP->OTP_CTL = OTP_CMD_READ_DID;
	OTP->OTP_CTL |= OTP_CTL_START_Msk;
	
	if (otp_check_busy() != 0)
		return -1;	/* OTP busy */

	if (OTP->OTP_STS & OTP_STS_CMDFF_Msk) {
		printf("OTP read DID, status = 0x%x\n", OTP->OTP_STS);
		return -2;
	}
	*u32Data = OTP->OTP_DATA;
	return 0;	// success
}

int OTP_Read_UID(uint32_t *u32Data)
{
	OTP->OTP_CTL = OTP_CMD_READ_UID;
	OTP->OTP_CTL |= OTP_CTL_START_Msk;
	
	if (otp_check_busy() != 0)
		return -1;	/* OTP busy */

	if (OTP->OTP_STS & OTP_STS_CMDFF_Msk) {
		printf("OTP read UID, status = 0x%x\n", OTP->OTP_STS);
		return -2;
	}
	*u32Data = OTP->OTP_DATA;
	return 0;	// success
}



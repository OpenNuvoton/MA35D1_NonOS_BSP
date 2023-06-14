/*************************************************************************//**
 * @file     readme.txt
 * @version  V1.00
 * @brief    non-os loader read me for MA35D1 MPU.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
Non-OS loader is for custom DDR used. 
Customer should modify some files and rebuild the project.

The flow is as follow.
1. Open ddrmctl2_PAIS_Tool_v3.04.xlsm.
2. Obtain the desired DRAM data sheet from the DRAM vendor.
3. Update the DRAM relative parameters Worksheet.
4. In "Whole DDR settings" tab, click "Cal & Gen" button, and custom_ddr.h will be generated.
5. Copy custom_ddr.h to Loader project.
6. In loader.h, modify the follow definition for self-application.
	- APP_EXE_ADDR: application execute address
	- storage offset:
		* APP_OFFSET_NAND: at least strat from block 6
		* APP_OFFSET_SPINAND: at least start from block 6
		* APP_OFFSET_SPINOR: at least start from 0x10000 (64KB)
		* APP_OFFSET_EMMC: at least start from 0x10000 (64KB)
	- APP_SIZE: application binary size
	- SPINAND_PAGE_SIZE: SPI-NAND page size
	- SPINAND_PAGE_PER_BLOCK: SPI-NAND page per-block count
7. Rebuild the Loader project to generate the new loader.bin.
8. Use the NuWriter to program the storage. The relative json files are put at NuWriter directory.


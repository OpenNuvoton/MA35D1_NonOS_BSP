/*************************************************************************//**
 * @file     readme.txt
 * @version  V1.00
 * @brief    FreeRTOS_Dual read me for MA35D1 MPU.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
FreeRTOS independently on Core1 and Core0
Customer should rebuild the project and burn binary files.

The flow is as follow.
1. Rebuild the FreeRTOS_Core1 project to generate the new FreeRTOS_Core1.bin.
2. Rebuild the FreeRTOS_Core0 project to generate the new FreeRTOS_Core0.bin.
3. Use the NuWriter to program the storage.
	- Burn FreeRTOS_Core1.bin to DDR 0x88000000 
	- Burn FreeRTOS_Core1.bin to DDR 0x80000000 and Run
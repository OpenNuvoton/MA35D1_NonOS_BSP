 /**************************************************************************//**
 * @file	vectors.h
 * @brief   Vectors Header File
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef _VECTORS_H_
#define _VECTORS_H_

/***************************** Include Files *********************************/

#include "irq_ctrl.h"
#include "assert.h"

#ifdef __cplusplus
extern "C" {
#endif
/************************** Function Prototypes ******************************/
void FIQInterrupt(void);
void IRQInterrupt(void);
void SynchronousInterrupt(void);
void SErrorInterrupt(void);

#ifdef __cplusplus
}
#endif

#endif /* protection macro */
